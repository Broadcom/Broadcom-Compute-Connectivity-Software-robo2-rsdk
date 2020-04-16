/*
 * * $ID: $
 * * 
 * * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 * * 
 * * $Copyright: (c) 2020 Broadcom Inc.
 * *
 * * File:
 * *     ts.c
 * * Purpose:
 * *     timesync module
 * *
 * * Note:
 * *
 * */

#include "sal_types.h"
#include "sal_util.h"
#include "soc/drv.h"
#include "sal_task.h"
#include "sal_alloc.h"
#include "sal_console.h"
#include "sal_time.h"
#include "sal_sync.h"
#include "fsal/error.h"
#include "fsal_int/tse.h"
#include "fsal/ts.h"
#include "soc/robo2/common/tables.h"
#ifndef CONFIG_EXTERNAL_HOST
#include "soc/cpu_m7.h"
#include <FreeRTOS.h>
#endif

/*
  Driver provides
  2. event queue - HW irq to SW task

*/


/* private structures */
typedef struct ts_event_state_s {
  cbx_ts_event_config_t config;
  cbx_ts_time_t tod;
  uint32_t capture;   /* down counter if config.flags has CAPTURE flag */
} ts_event_state_t;

STATIC ts_event_state_t ts_event_state[TS_EVENT_NUM_EVENTS];

ts_context_t ts_context;


static int ts_tod_m7_hybrid_mode = 0;
#ifdef TS_TOD_OVER_SERIAL
  static int ts_tod_rsdk_slave = 0;
#endif
static sal_thread_t ts_event_task_id;

extern int ts_tod_test(void);
extern int ts_gen_config_test(int gen_id, uint32_t flags, uint64_t start_sec, uint32_t start_nsec, uint32_t high_nsec, uint32_t low_nsec);
extern int ts_event_config_test(int event_id, int source, uint32_t flags, uint32_t divider);

#ifdef TS_TOD_OVER_SERIAL
/*
 * Is the rsdk running hybrid mode, if so this is the slave device
 */
int
ts_tod_slave_get(void) {
    return ts_tod_rsdk_slave;
}
#endif

/*
 * TimeSync-API : cbx_ts_tod_get(*time, *timestamp)
 * ToD Functions - tse functions will lock IRQ context
 */
int
cbx_ts_tod_get(cbx_ts_time_t* time, uint64* timestamp){
    return tse_tod_get(time, timestamp);
}

/*
   TimeSync-API : cbx_ts_tod_set(*time, timestamp)

 Sets the time for some _past_ timestamp value. This value can not be in the future
*/

int
cbx_ts_tod_set(cbx_ts_time_t *time, uint64_t timestamp){
    return tse_tod_set(time, timestamp);
}

#ifdef TS_TOD_OVER_SERIAL
/*
  $GPZDA,hhmmss.ss,dd,mm,yyyy,xx,yy*CC
  "$GPZDA,%02d%02d%02d.%02d,%02d,%02d,%04d,%02d,%02d*%02x",
   hh,mm,ss,ss_frac,dd,mo,yyyy,zh,zm,cksum

  $GPZDA,133640.00,01,01,1970,00,00*00
*/


int ts_tod_string_create(uint64_t secs, char *str_tod, uint8_t len) {
    uint64_t  val = secs;
    int   dd,hh,mm,ss;
    int   ss_frac=0,mo=01,zh=0,zm=0,cksum=0;
    int   yyyy = 1970;

#ifdef  DEBUG_TS_TOD_OVER_SERIAL_VERBOSE
    sal_printf("ts_tod_string_create() ToD secs : %llu\n", secs);
#endif
    if (sal_strlen(("$GPZDA,hhmmss.ss,dd,mm,yyyy,xx,yy*CC")+1) > len) {
        sal_printf("ts_tod_string_create() failed, buffer too small\n");
        return CBX_E_PARAM;
    }

    ss = (int) val % 60;
    val = val / 60;
    //sal_printf("val (min) : %llu\n", val);
    mm = (int) val % 60;
    val = val / 60;
    //sal_printf("val (hr) : %llu\n", val);
    hh = (int) val % 24;
    val = val / 24;
    dd = (int) val;
    //sal_printf("val (days) : %llu\n", val);
    dd += 1;

    sal_memset(str_tod, 0, len);

    (void) sal_snprintf(str_tod, len,
    "$GPZDA,%02d%02d%02d.%02d,%02d,%02d,%04d,%02d,%02d*%02x\n",
    hh,mm,ss,ss_frac,dd,mo,yyyy,zh,zm,cksum);

#ifdef  DEBUG_TS_TOD_OVER_SERIAL_VERBOSE
    sal_printf("ToD String Create : %s\n", str_tod);
#endif
    /* TODO:
        - Validate checksum
        - Convert to time since epoch (only using days/hrs/min/sec for now)
    */
    return CBX_E_NONE;
}

static int ts_tod_serial_tx(cbx_ts_time_t *tod) {
    char     msg[64];

#ifdef  DEBUG_TS_TOD_OVER_SERIAL_VERBOSE
    sal_printf("ts_tod_serial_tx : %llusec, %lunsec\n", tod->sec, tod->nsec);
#endif
    (void) ts_tod_string_create(tod->sec, msg, sizeof(msg));
#ifdef  DEBUG_TS_TOD_OVER_SERIAL_BASIC
    sal_printf("Sending ToD : %s\n", msg);
#endif
    usb_serial_msg_write(msg);

    return CBX_E_NONE;
}
#endif // TS_TOD_OVER_SERIAL



/*
 * EVENT functions
 *
 */

void ts_event_task_wake(void)
{
    if (ts_context.er_sem) {
        sal_sem_give(ts_context.er_sem);
    }
}

void ts_event_enqueue(uint32_t event_id, cbx_ts_time_t *tod) {
    uint32_t idx;

    if (ts_context.er_status == TS_CONTEXT_ER_STATUS_F_FULL) {
        ts_context.er_status = TS_CONTEXT_ER_STATUS_F_OVERFLOW;
        sal_printf("Event %d dropped, queue is full\n", event_id);
        return;
    }
    idx = ts_context.er_producer_idx;
    ts_context.event_ring[idx].event_id = event_id;
    ts_context.event_ring[idx].tod.sec = tod->sec;
    ts_context.event_ring[idx].tod.nsec = tod->nsec;
    TS_LOG_TMP("Queueing event %d at index %d\n", event_id, ts_context.er_producer_idx);

    /* When the er_producer_idx is one less than the er_consumer_idx (with
       wrapping accounted for), then the ring is full
    */
    if (   ((ts_context.er_producer_idx > ts_context.er_consumer_idx) &&
           ((ts_context.er_producer_idx - ts_context.er_consumer_idx) == (TS_EVENT_RING_SIZE-1)))
        || ((ts_context.er_producer_idx < ts_context.er_consumer_idx) &&
           ((ts_context.er_producer_idx - ts_context.er_consumer_idx) == 1)))
    {
        ts_context.er_status = TS_CONTEXT_ER_STATUS_F_FULL;
    } else {
        if (ts_context.er_producer_idx < (TS_EVENT_RING_SIZE-1)) {
            ts_context.er_producer_idx++;
        } else {
            ts_context.er_producer_idx = 0;
        }
    }
}

int ts_event_dequeue(void) {
    int rv;
    uint32_t event_id;

    while (ts_context.er_consumer_idx != ts_context.er_producer_idx) {
        event_id = ts_context.event_ring[ts_context.er_consumer_idx].event_id;
        TS_LOG_TMP("Processing event %d at index %d\n", event_id, ts_context.er_consumer_idx);

        (ts_event_state[event_id].config.event_cb_func)
                (event_id, &(ts_context.event_ring[ts_context.er_consumer_idx]));
        rv = CBX_E_NONE;
        if (ts_context.er_consumer_idx < (TS_EVENT_RING_SIZE-1)) {
            ts_context.er_consumer_idx++;
        } else {
            ts_context.er_consumer_idx = 0;
        }
        /* Clear the status flag if we are consuming events */
        ts_context.er_status = 0;
    }
    rv = CBX_E_EMPTY;
    return rv;
}


void ts_event_task(void *params) {
    int rv = CBX_E_NONE;

    while (1) {
        rv = ts_event_dequeue();
        if (rv == CBX_E_EMPTY) {
            sal_sem_take(ts_context.er_sem, sal_sem_FOREVER);
        }
    }
    sal_printf("ts_event_task() exit\n");
}

int
ts_m7_hybrid_mode_init(void) {
    ts_tod_m7_hybrid_mode = 1;
#ifdef TS_TOD_OVER_SERIAL
    ts_tod_rsdk_slave = 1;
    sal_printf("ts_tod_rsdk_slave device identified\n");
#endif
    return CBX_E_NONE;
}

int
ts_init() {
    int rv = CBX_E_NONE;

    sal_memset(ts_event_state, 0, sizeof(ts_event_state_t) * TS_EVENT_NUM_EVENTS);

    sal_memset(&ts_context, 0, sizeof(ts_context_t));
    ts_context.er_sem = sal_sem_create("Event Ring", 1, 0);
    if (ts_context.er_sem == NULL) {
        sal_printf("Event Ring Sem create failed\n");
        return CBX_E_INIT;
    }

#ifndef TS_TOD_OVER_SERIAL
    ts_event_task_id = sal_thread_create("TS_EVENT", 1024, TS_EVENT_THREAD_PRI, ts_event_task, NULL);
    if (ts_event_task_id == SAL_THREAD_ERROR) {
        sal_printf("Failed Creating TS_EVENT Task\n");
    } else {
        sal_printf("Created TS_EVENT Task\n");
    }
#else
    if (ts_tod_slave_get() == 0) {
        ts_event_task_id = sal_thread_create("TS_EVENT", 1024, TS_EVENT_THREAD_PRI, ts_event_task, NULL);
        if (ts_event_task_id == SAL_THREAD_ERROR) {
            sal_printf("Failed Creating TS_EVENT Task\n");
        } else {
            sal_printf("Created TS_EVENT Task\n");
        }
    }
#endif // TS_TOD_OVER_SERIAL
    rv = tse_init();

    return rv;
}


static void ts_cfg_hwdpll(int event_source)
{

    if (event_source != CBX_TS_EVENT_SOURCE_SYNC_IN) {
        sal_printf("ERROR: HWDPLL event source %d is not supported\n", event_source);
    }

#ifdef TS_REG_WRITE_DIRECT
    /* CRU_TS_CORE_TIMESYNC_DPLL_REF_PHASE_DELTA_UPPER 0x4030050c
       Must be written before CRU_TS_CORE_TIMESYNC_DPLL_REF_PHASE_DELTA_LOWER
    */
    reg_write(0, 0x4030050c, 0x0);

    /* CRU_TS_CORE_TIMESYNC_DPLL_REF_PHASE_DELTA_LOWER 0x40300508
       0x1e848 == 125000 => 8KHz is the only frequency supported by software at
       this time
    */
    reg_write(0, 0x40300508, 0x1e848);

    /* CRU_TS_CORE_TIMESYNC_DPLL_LOOP_FLITER_COEFFICIENTS 0x40300510
       The coefficients for frequencies other than 8KHz are not known and it is
       not known how to calculate them.
       Coefficients for 8KHz are:
         k3 = 0x2c [23:16]
         k2 = 0x28 [15:8]
         k1 = 0x2a [7:0]
    */
    reg_write(0, 0x40300510, 0x2c282a);

    /* CRU_TS_CORE_TIMESYNC_SYNC_IN_2_INPUT_DIVISOR 0x40300328
       Set the enable bit:28 and set the divisor value to zero == bypass
    */
    reg_write(0, 0x40300328, 0x10000000);

    /* CRU_TS_CORE_TIMESYNC_DPLL_SYNC_SOURCE_SEL 0x40300524
    */
    reg_write(0, 0x40300524, event_source);

    /* CRU_TS_CORE_TIMESYNC_DPLL_CONTROL 0x4030051c
    */
    reg_write(0, 0x4030051c, 0x1);

    /* CRU_TS_TOP_TIMESYNC_SYNC_MUX_SEL 0x40301044
       b1 : Select sync_i from the GPIO pads to drive SYNC_O to the GPIO pad
       b0 : Select ts_sync_o to drive NSE_SYNC_i
    */
    reg_write(0, 0x40301044, 0x2);

    /* CRU_TS_CORE_TIMESYNC_NCO_CONTROL_SOURCE_SEL 0x40300520
    */
    reg_write(0,0x40300520,0x0);

    /* CRU_TS_CORE_TIMESYNC_DPLL_CONTROL 0x4030051c
       b0 : Set the DPLL Mode to 1
       b1 : Enable the DPLL
    */
    reg_write(0, 0x4030051c, 0x3);

    /* CRU_TS_CORE_TIMESYNC_NCO_CONTROL_SOURCE_SEL 0x40300520
       b0 : DPLL controls the NCO
    */
    reg_write(0,0x40300520,0x1);
#else
    uint32_t addr;
    uint32_t value;

    /* CRU_TS_CORE_TIMESYNC_DPLL_REF_PHASE_DELTA_UPPER 0x4030050c
       Must be written before CRU_TS_CORE_TIMESYNC_DPLL_REF_PHASE_DELTA_LOWER
    */
    addr = DRV_REG_ADDR(0, CRU_TS_CORE_TIMESYNC_DPLL_REF_PHASE_DELTA_UPPERr_ROBO2, 0, 0);
    value = 0;
    DRV_REG_WRITE(0, addr, &value,4);

    /* CRU_TS_CORE_TIMESYNC_DPLL_REF_PHASE_DELTA_LOWER 0x40300508
       0x1e848 == 125000 => 8KHz is the only frequency supported by software at
       this time
    */
    addr = DRV_REG_ADDR(0, CRU_TS_CORE_TIMESYNC_DPLL_REF_PHASE_DELTA_LOWERr_ROBO2, 0, 0);
    value = 0x1e848;
    DRV_REG_WRITE(0, addr, &value,4);

    /* CRU_TS_CORE_TIMESYNC_DPLL_LOOP_FLITER_COEFFICIENTS 0x40300510
       The coefficients for frequencies other than 8KHz are not known and it is
       not known how to calculate them.
       Coefficients for 8KHz are:
         k3 = 0x2c [23:16]
         k2 = 0x28 [15:8]
         k1 = 0x2a [7:0]
    */
    addr = DRV_REG_ADDR(0, CRU_TS_CORE_TIMESYNC_DPLL_LOOP_FLITER_COEFFICIENTSr_ROBO2, 0, 0);
    value = 0x2c282a;
    DRV_REG_WRITE(0, addr, &value,4);

    /* CRU_TS_CORE_TIMESYNC_SYNC_IN_2_INPUT_DIVISOR 0x40300328
       Set the enable bit:28 and set the divisor value to zero == bypass
    */
    addr = DRV_REG_ADDR(0, CRU_TS_CORE_TIMESYNC_SYNC_IN_2_INPUT_DIVISORr_ROBO2, 0, 0);
    value = 0x10000000;
    DRV_REG_WRITE(0, addr, &value,4);

    /* CRU_TS_CORE_TIMESYNC_DPLL_SYNC_SOURCE_SEL 0x40300524
    */
    addr = DRV_REG_ADDR(0, CRU_TS_CORE_TIMESYNC_DPLL_SYNC_SOURCE_SELr_ROBO2, 0, 0);
    value = event_source;
    DRV_REG_WRITE(0, addr, &value,4);

    /* CRU_TS_CORE_TIMESYNC_DPLL_CONTROL 0x4030051c
    */
    addr = DRV_REG_ADDR(0, CRU_TS_CORE_TIMESYNC_DPLL_CONTROLr_ROBO2, 0, 0);
    value = 0x1;
    DRV_REG_WRITE(0, addr, &value,4);

    /* CRU_TS_TOP_TIMESYNC_SYNC_MUX_SEL 0x40301044
       b1 : Select sync_i from the GPIO pads to drive SYNC_O to the GPIO pad
       b0 : Select ts_sync_o to drive NSE_SYNC_i
    */
    addr = DRV_REG_ADDR(0, CRU_TS_TOP_TIMESYNC_SYNC_MUX_SELr_ROBO2, 0, 0);
    value = 0x2;
    DRV_REG_WRITE(0, addr, &value,4);

    /* CRU_TS_CORE_TIMESYNC_NCO_CONTROL_SOURCE_SEL 0x40300520
    */
    addr = DRV_REG_ADDR(0, CRU_TS_CORE_TIMESYNC_NCO_CONTROL_SOURCE_SELr_ROBO2, 0, 0);
    value = 0x0;
    DRV_REG_WRITE(0, addr, &value,4);

    /* CRU_TS_CORE_TIMESYNC_DPLL_CONTROL 0x4030051c
       b0 : Set the DPLL Mode to 1
       b1 : Enable the DPLL
    */
    addr = DRV_REG_ADDR(0, CRU_TS_CORE_TIMESYNC_DPLL_CONTROLr_ROBO2, 0, 0);
    value = 0x3;
    DRV_REG_WRITE(0, addr, &value,4);

    /* CRU_TS_CORE_TIMESYNC_NCO_CONTROL_SOURCE_SEL
       b0 : DPLL controls the NCO
    */
    addr = DRV_REG_ADDR(0, CRU_TS_CORE_TIMESYNC_NCO_CONTROL_SOURCE_SELr_ROBO2, 0, 0);
    value = 0x1;
    DRV_REG_WRITE(0, addr, &value,4);
#endif

#ifdef DEBUG_TS_HWDPLL
    sal_printf("HWDPLL configured at 8KHz for event source %d%s\n", event_source,
                event_source == CBX_TS_EVENT_SOURCE_SYNC_IN ? " SYNC_IN_2" : "");
#endif
}


/*
   It is not possible to provide a callback function from CINT so if the
   CBX_TS_EVENT_CONFIG_F_CB is set the config structure will contain a NULL
   callback function. In this case the internal code will assign
   ts_test_callback() as the callback function. This will print out the event id
   and ToD the event occurred.
*/
void ts_test_callback (int event_id, cbx_ts_event_t* ts_event) {
#ifdef DEBUG_TS_CALLBACK
    sal_printf("ts_test_callback() for event %d with a ToD of %llu.%09lu seconds.\n",
                event_id, ts_event->tod.sec, ts_event->tod.nsec);
#endif
}

void ts_callback1 (int event_id, cbx_ts_event_t* ts_event) {
#ifdef  DEBUG_TS_CALLBACK1
    sal_printf("\nts_callback1() for event %d with a ToD of %d.%09d seconds.\n",
                event_id, ts_event->tod.sec, ts_event->tod.nsec);
#endif
#ifdef TS_TOD_OVER_SERIAL  // Master device sending TOD - Check to make sure not slave device but should never get invoked if so
    ts_tod_serial_tx(&(ts_event->tod));
#endif
}
void ts_callback2 (int event_id, cbx_ts_event_t* ts_event) {
#ifdef  DEBUG_TS_CALLBACK2
    sal_printf("ts_callback2() for event %d with a ToD of %llu.%09lu seconds.\n",
                event_id, ts_event->tod.sec, ts_event->tod.nsec);
#endif
}



/*
    TimeSync-API : cbx_ts_event_config_set(event_id, event_source, *config)

    Event config will program:
    1. enable
    2. divisor
    3. muxing required to use a specific source

    Please note that not all sources are supported for all events.

    AB501: TODO:
    1. add capture flag

    Details:
    cbx_ts_event_config_set is used to configure the capture or other utilization of
    external events. The event_id (CBX_TS_EVENT_ID_*) describes which input to the
    timesync block is being configured. The event_source (CBX_TS_EVENT_SOURCE_*) defines
    describes which logical input is used for this event. Not all sources can be connected
    to any given envent, so helper defines CBX_TS_EVENT_ID_*_VALID_SOURCES are defined.
    If the muxing in the Avenger can not be configured as specified, and error is returned.
    The ts_event_config structure contains:
    flags -
      TS_CONFIG_F_ENABLE - when set, enables this event, otherwise it's disabled
      CBX_TS_EVENT_CONFIG_F_DIVIDER - the divider=N config variable provides divide-by-N
      CBX_TS_EVENT_CONFIG_F_CB - callback is specified in event_cb_func
      CBX_TS_EVENT_CONFIG_F_FORCE_VALID - for SyncE sources - program the pullup on valid pin
      CBX_TS_EVENT_CONFIG_F_HW_DPLL - connect this event to HW DPLL
    divider - when set to N, the frequency of source is divided by N to generate event when F_DIVIDER is set
    event_cb_func - function pointer for callback when F_CB is set

int ts_event_config_test(int event_id, int event_source, uint32_t flags, uint32_t divider)

*/

int
cbx_ts_event_config_set(int event_id, int event_source, cbx_ts_event_config_t* config) {
    /* NOTE: not all event sources are allowed for all events */
    uint32_t address=0;
    uint32_t value=0;
    uint32_t fval=0;
    uint32_t enabled_events=0;


    if (!!(config->flags & CBX_TS_EVENT_CONFIG_F_HW_DPLL))
    {
        /* HWDPLL current support is limited to:
             event_id     = CBX_TS_EVENT_ID_SYNC_IN2
             event_source = CBX_TS_EVENT_SOURCE_SYNC_IN
           HWDPLL is programmed for 8KHz (no other freq supported)
        */
        if ((event_id != CBX_TS_EVENT_ID_SYNC_IN2) || (event_source != CBX_TS_EVENT_SOURCE_SYNC_IN)) {
            sal_printf("Unsupported configuration for HWDPLL\n");
            return CBX_E_CONFIG;
        }
        ts_cfg_hwdpll(event_source);
        return CBX_E_NONE;
    }



    if ((event_id >= CBX_TS_EVENT_ID_SYNCE_CLK0) && (event_id <= CBX_TS_EVENT_ID_SYNCE_CLK5)) {
        if (!((1<<event_source) &  CBX_TS_EVENT_ID_SYNCE_VALID_SOURCES)) {
            TS_LOG_TMP("Invalid source 0x%x for event id %d\n", event_source,
                   event_id);
            return CBX_E_CONFIG;
        }
       /* program the input mux */

        ROBO2_REG_READ(0, CRU_TS_TOP_TIMESYNC_SYNCE_CLK_GROUP_A_MUX_SELr,
                   REG_PORT_ANY, (event_id-CBX_TS_EVENT_ID_SYNCE_CLK0), &value);
        fval = event_source;
        soc_CRU_TS_TOP_TIMESYNC_SYNCE_CLK_GROUP_A_MUX_SELr_field_set(0, &value, SELf, &fval);
        address = DRV_REG_ADDR(0, CRU_TS_TOP_TIMESYNC_SYNCE_CLK_GROUP_A_MUX_SELr_ROBO2,
                       REG_PORT_ANY, (event_id-CBX_TS_EVENT_ID_SYNCE_CLK0));

        TS_LOG_TMP("address=0x%08x value=0x%08x\n", address, value);

        ROBO2_REG_WRITE(0, CRU_TS_TOP_TIMESYNC_SYNCE_CLK_GROUP_A_MUX_SELr,
                REG_PORT_ANY, (event_id-CBX_TS_EVENT_ID_SYNCE_CLK0), &value);

        /* program divider if any */
        value=0;

        /* set enable if enabled */
        fval = !!(config->flags & CBX_TS_EVENT_CONFIG_F_ENABLE);
        soc_CRU_TS_CORE_TIMESYNC_SYNCE_CLK_GRP_A_COUNT_CTRLr_field_set(0, &value, ENABLEf, &fval);
        /* set divider if specified */
        fval = (!!(config->flags & CBX_TS_EVENT_CONFIG_F_DIVIDER))?config->divider:0;
        soc_CRU_TS_CORE_TIMESYNC_SYNCE_CLK_GRP_A_COUNT_CTRLr_field_set(0, &value, DIVISORf, &fval);
        ROBO2_REG_WRITE(0, CRU_TS_CORE_TIMESYNC_SYNCE_CLK_GRP_A_COUNT_CTRLr,
                REG_PORT_ANY, (event_id-CBX_TS_EVENT_ID_SYNCE_CLK0), &value);


        address = DRV_REG_ADDR(0, CRU_TS_CORE_TIMESYNC_SYNCE_CLK_GRP_A_COUNT_CTRLr_ROBO2,
                       REG_PORT_ANY, (event_id-CBX_TS_EVENT_ID_SYNCE_CLK0));
        TS_LOG_TMP("address=0x%08x value=0x%08x\n", address, value);

        /* Must pull up the input signals if the F_FORCE_VALID is set:
           ONLY for SOURCE_RECOV_CLK0,1,2!!!!
        */
        if (!!(config->flags & CBX_TS_EVENT_CONFIG_F_FORCE_VALID) &&
            (event_source >= CBX_TS_EVENT_SOURCE_RECOV_CLK0) &&
            (event_source <= CBX_TS_EVENT_SOURCE_RECOV_CLK0))
        {
            //TimeStamp Capture
            //pad_control_pdn_register_3  CRU_CRU_PAD_CONTROL_PDN_REGISTER_3r
            REG_READ_CRU_CRU_PAD_CONTROL_PDN_REGISTER_3r(0, &value);
            value &= 0xfffbffff;
            REG_WRITE_CRU_CRU_PAD_CONTROL_PDN_REGISTER_3r(0, &value);
            //reg_read(0, 0x40200428);
            //reg_write(0, 0x40200428, 0xbbfbffff);

            //pad_control_pup_register_3  CRU_CRU_PAD_CONTROL_PUP_REGISTER_3r
            REG_READ_CRU_CRU_PAD_CONTROL_PUP_REGISTER_3r(0, &value);
            value |= 0x00040000;
            REG_WRITE_CRU_CRU_PAD_CONTROL_PUP_REGISTER_3r(0, &value);
            //reg_read(0, 0x40200410);
            //reg_write(0, 0x40200410, 0x44040000);
        }


    } else if (event_id == CBX_TS_EVENT_ID_SYNC_IN2) {
        if (!((1<<event_source) & CBX_TS_EVENT_ID_SYNC_IN2_VALID_SOURCES)) {
            TS_LOG_TMP("Invalid source %d (0x%08x) for event id %d - valid=0x%08x\n",
                   event_source, (1<<event_source),
                   event_id,
                   CBX_TS_EVENT_ID_SYNC_IN2_VALID_SOURCES);
            return CBX_E_CONFIG;
        }
        /* program the top for SYNC_IN2 mux */
        fval = event_source == CBX_TS_EVENT_SOURCE_SYNC_IN;
        REG_READ_CRU_TS_TOP_TIMESYNC_SYNC_MUX_SELr(0, &value);
        soc_CRU_TS_TOP_TIMESYNC_SYNC_MUX_SELr_field_set(0, &value, SYNC_O_SELf, &fval);
        REG_WRITE_CRU_TS_TOP_TIMESYNC_SYNC_MUX_SELr(0, &value);

        address = DRV_REG_ADDR(0, CRU_TS_TOP_TIMESYNC_SYNC_MUX_SELr_ROBO2,
                       REG_PORT_ANY, 0);
        TS_LOG_TMP("address=0x%08x value=0x%08x\n", address, value);

        /* enable and divisor */
        value=0;
        /* set the divider enable if */
        fval=!!(config->flags & CBX_TS_EVENT_CONFIG_F_ENABLE);
        soc_CRU_TS_CORE_TIMESYNC_SYNC_IN_2_INPUT_DIVISORr_field_set(0, &value, ENABLEf, &fval);
        /* set divisor value if specified*/
        fval = (!!(config->flags & CBX_TS_EVENT_CONFIG_F_DIVIDER))?config->divider:0;
        soc_CRU_TS_CORE_TIMESYNC_SYNC_IN_2_INPUT_DIVISORr_field_set(0, &value, DIVISORf, &fval);

        REG_WRITE_CRU_TS_CORE_TIMESYNC_SYNC_IN_2_INPUT_DIVISORr(0, &value);

        address = DRV_REG_ADDR(0, CRU_TS_CORE_TIMESYNC_SYNC_IN_2_INPUT_DIVISORr_ROBO2,
                       REG_PORT_ANY, 0);
        TS_LOG_TMP("address=0x%08x value=0x%08x\n", address, value);
    } else if (event_id == CBX_TS_EVENT_ID_SYNC_IN3) {
        if (!((1<<event_source) & CBX_TS_EVENT_ID_SYNC_IN3_VALID_SOURCES)){
            TS_LOG_TMP("Invalid source %d (0x%08x) for event id %d - valid 0x%08x\n",
                   event_source, (1<<event_source),
                   event_id, CBX_TS_EVENT_ID_SYNC_IN3_VALID_SOURCES);
            return CBX_E_CONFIG;
        }
        /* program the top for SYNC_IN3 mux */
        REG_READ_CRU_TS_TOP_TIMESYNC_FRAME_SYNC_MUX_SELr(0, &value);
        fval = (event_source == CBX_TS_EVENT_SOURCE_FRAME_SYNC_IN);
        soc_CRU_TS_TOP_TIMESYNC_FRAME_SYNC_MUX_SELr_field_set(0, &value, FRAME_SYNC_O_SELf, &fval);
        REG_WRITE_CRU_TS_TOP_TIMESYNC_FRAME_SYNC_MUX_SELr(0, &value);

        address = DRV_REG_ADDR(0, CRU_TS_TOP_TIMESYNC_FRAME_SYNC_MUX_SELr_ROBO2,
                       REG_PORT_ANY, 0);
        TS_LOG_TMP("address=0x%08x value=0x%08x\n", address, value);

        /* enable and divisor */
        value=0;
        fval = !!(config->flags & CBX_TS_EVENT_CONFIG_F_ENABLE);
        soc_CRU_TS_CORE_TIMESYNC_SYNC_IN_3_INPUT_DIVISORr_field_set(0, &value, ENABLEf, &fval);
        fval =  (!!(config->flags & CBX_TS_EVENT_CONFIG_F_DIVIDER))? config->divider:0;
        soc_CRU_TS_CORE_TIMESYNC_SYNC_IN_3_INPUT_DIVISORr_field_set(0, &value, DIVISORf, &fval);
        REG_WRITE_CRU_TS_CORE_TIMESYNC_SYNC_IN_3_INPUT_DIVISORr(0, &value);

        address = DRV_REG_ADDR(0, CRU_TS_CORE_TIMESYNC_SYNC_IN_3_INPUT_DIVISORr_ROBO2,
                   REG_PORT_ANY, 0);
        TS_LOG_TMP("address=0x%08x value=0x%08x\n", address, value);
    } else {
       return CBX_E_CONFIG;
    }
    /* update the event state */

    /* enable time capture mode */
    REG_READ_CRU_TS_CORE_TIMESYNC_TIME_CAPTURE_MODEr(0, &value);
    fval = (!!(config->flags&CBX_TS_EVENT_CONFIG_F_ENABLE)) << event_id;
    soc_CRU_TS_CORE_TIMESYNC_TIME_CAPTURE_MODEr_field_set(0, &value, TIME_CAPTURE_MODEf, &fval);
    REG_WRITE_CRU_TS_CORE_TIMESYNC_TIME_CAPTURE_MODEr(0, &value);

    address = DRV_REG_ADDR(0, CRU_TS_CORE_TIMESYNC_TIME_CAPTURE_MODEr_ROBO2,
               REG_PORT_ANY, 0);
    TS_LOG_TMP("Capture Mode address=0x%08x value=0x%08x\n", address, value);

    enabled_events=value;

    /* capture control must be set if any of the enables are set */
    value = !!enabled_events;
    REG_WRITE_CRU_TS_CORE_TIMESYNC_TIME_CAPTURE_CONTROLr(0, &value);

    address = DRV_REG_ADDR(0, CRU_TS_CORE_TIMESYNC_TIME_CAPTURE_CONTROLr_ROBO2,
               REG_PORT_ANY, 0);
    TS_LOG_TMP("Capture Control address=0x%08x value=0x%08x\n", address, value);

    /* interrupt enable */
    value = 0;
    fval = enabled_events;
    soc_CRU_TS_CORE_TIMESYNC_INTERRUPT_ENABLEr_field_set(0, &value, INT_ENABLEf, &fval);
    REG_WRITE_CRU_TS_CORE_TIMESYNC_INTERRUPT_ENABLEr(0, &value);

    address = DRV_REG_ADDR(0, CRU_TS_CORE_TIMESYNC_INTERRUPT_ENABLEr_ROBO2,
               REG_PORT_ANY, 0);

    TS_LOG_TMP("Interrupt Enable address=0x%08x value=0x%08x\n", address, value);

    /* debug */
    TS_LOG_TMP("enabled_events=0x%x\n", enabled_events);

    /* Need to add more checks to see if we enable capture or not.
       For now assume if the callback flag is set that we enable event capture
    */
    sal_memcpy(&ts_event_state[event_id].config, config, sizeof(cbx_ts_event_config_t));
    value = 1 << event_id;
    if (!!(config->flags&CBX_TS_EVENT_CONFIG_F_ENABLE) &&
        !!(config->flags&CBX_TS_EVENT_CONFIG_F_CB)) {
      ts_context.event_id_mask |= value;
      /* Use ts_test_callback if no function provided so we can test this from CINT */
      if (config->event_cb_func == NULL) {
        if (config->flags&CBX_TS_EVENT_CONFIG_F_CUST_1) {
          ts_event_state[event_id].config.event_cb_func = ts_callback1;
        } else if (config->flags&CBX_TS_EVENT_CONFIG_F_CUST_2) {
          ts_event_state[event_id].config.event_cb_func = ts_callback2;
        } else {
          TS_LOG_TMP("No event cb specified, setting to ts_test_callback().\n");
          ts_event_state[event_id].config.event_cb_func = ts_test_callback;
        }
      }
    } else {
      ts_context.event_id_mask &= ~value;
    }

    #ifndef TS_DEBUG
      (void) address;
    #endif

    return CBX_E_NONE;
}


/*
 * GENERATOR functions
 *
 */

/*
   TimeSync-API : cbx_ts_gen_config_set(generator_id, *config)
*/

int
cbx_ts_gen_config_set(int generator_id, cbx_ts_gen_config_t* config){
    int offset = generator_id * 0x1c; /* will use INTERVAL_GENERATOR_0_* + offset */
    uint32_t addr;
    uint32_t value;
    //uint32_t fval;
    cbx_ts_time_t tod;
    uint64_t tod_ts;
    uint64_t timestamp;

    /* program output enable - this does nothing in Avenger */
    addr = DRV_REG_ADDR(0, CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_0_OUTPUT_ENABLEr_ROBO2, 0, 0) + offset;
    value = 0;
    DRV_REG_WRITE(0, addr, &value,4);
    TS_LOG_TMP("addr=0x%08x value=0x%08x\n", addr, value);

    addr = DRV_REG_ADDR(0, CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_0_CTRLr_ROBO2, 0, 0) + offset;
    value=0;
    DRV_REG_WRITE(0, addr, &value,4);
    TS_LOG_TMP("addr=0x%08x value=0x%08x\n", addr, value);

    /* configure start time as follows:
       if the F_START_TIME is specified, make sure this is in the future
       and if not, return CBX_E_CONFIG. If no F_START_TIME, then we use some
       time that's slightly less than current timestamper value
    */

    cbx_ts_tod_get(&tod, &tod_ts);
    TS_LOG_TMP("ToD=%llu.%09lu ts=0x%llx\n", tod.sec, tod.nsec, tod_ts);
    /* Rollover detection of the 48bit timestamper. If we are within a msec
       of rollover, wait 2msec then continue */
    if (tod_ts >= 0xFFFFF0BE0ULL) {
        sal_usleep(2000);
    }

    if (!!(config->flags & CBX_TS_GEN_CONFIG_F_START_TIME)) {
        /* check that this is in the future relative to ToD, if in the
           past, then return error */
        if (config->start_time.sec < tod.sec ||
            (config->start_time.sec==tod.sec && config->start_time.nsec < tod.nsec)) {
            return CBX_E_CONFIG;
        }
        /* compute the timestamp associated with config.start_time */
        timestamp = (uint64_t)(config->start_time.sec - tod.sec)*1e9 + tod_ts;
        if (config->start_time.nsec > tod.nsec) {
            timestamp += (config->start_time.nsec - tod.nsec);
        } else {
            timestamp -= (tod.nsec-config->start_time.nsec);
        }
        TS_LOG_TMP("Start time specified: %llusec %unsec timestamp = 0x%llx\n",
            config->start_time.sec, config->start_time.nsec, timestamp);
    } else {
        /* if the PHASE_ADJUST is in the past, then the generator starts immediately */
        timestamp = tod_ts - 100; /* time slightly in the past */
    }

    //TS_LOG_TMP("Start time: %llusec %unsec timestamp = 0x%llx\n",
    //    tod.sec+nsec/(uint64)1e9, tod.nsec+(nsec%(uint64)1e9), timestamp);

    if (!!(config->flags & CBX_TS_GEN_CONFIG_F_ENABLE)) {
        addr = DRV_REG_ADDR(0, CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_0_OUTPUT_ENABLEr_ROBO2, 0, 0) + offset;
        value = 1;
        DRV_REG_WRITE(0, addr, &value,4);
        TS_LOG_TMP("addr=0x%08x value=0x%08x\n", addr, value);
    }

    /* UP/Down control */
    addr = DRV_REG_ADDR(0, CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_0_UP_EVENT_CTRLr_ROBO2, 0, 0) + offset;
    value = config->high_time_nsec;
    DRV_REG_WRITE(0, addr, &value,4);
    TS_LOG_TMP("addr=0x%08x value=0x%08x\n", addr, value);
    addr = DRV_REG_ADDR(0, CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_0_DOWN_EVENT_CTRLr_ROBO2, 0, 0) + offset;
    value = config->low_time_nsec;
    DRV_REG_WRITE(0, addr, &value,4);
    TS_LOG_TMP("addr=0x%08x value=0x%08x\n", addr, value);

    /* program this in PHASE_ADJUST_LOWER and UPPER */
    addr = DRV_REG_ADDR(0, CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_0_PHASE_ADJUST_UPPERr_ROBO2, 0, 0) + offset;
    value = (uint32_t)((timestamp>>32)&0xffffffffLL);
    DRV_REG_WRITE(0, addr, &value,4);
    TS_LOG_TMP("addr=0x%08x value=0x%08x\n", addr, value);
    addr = DRV_REG_ADDR(0, CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_0_PHASE_ADJUST_LOWERr_ROBO2, 0, 0) + offset;
    value = (uint32_t)(timestamp&0xffffffffLL);
    DRV_REG_WRITE(0, addr, &value,4);
    TS_LOG_TMP("addr=0x%08x value=0x%08x\n", addr, value);

    /* final enable if it's enabled */
    if (!!(config->flags & CBX_TS_GEN_CONFIG_F_ENABLE)) {
        addr = DRV_REG_ADDR(0, CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_0_CTRLr_ROBO2, 0, 0) + offset;
        if (!!(config->flags & CBX_TS_GEN_CONFIG_F_ONE_SHOT)) {
            value = 3;
        } else {
            value = 1;
        }
        DRV_REG_WRITE(0, addr, &value,4);
        TS_LOG_TMP("addr=0x%08x value=0x%08x\n", addr, value);
        /* debug logging */
        /* TRG _ What does this do?
          int addr2 = DRV_REG_ADDR(0, CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_0_CTRLr_ROBO2, 0 ,1);
          TS_LOG_TMP("Gen %d enable=1: addr=0x%08x addr=0x%08x\n", addr, addr2);
        TRG */
        TS_LOG_TMP("TS_INTERVAL_GENERATOR_%d:%s enable=%d addr=0x%08x\n", generator_id,
            !!(config->flags & CBX_TS_GEN_CONFIG_F_ONE_SHOT) ? " One-Shot" : "", value, addr);
    } else {
        value = 0;
        TS_LOG_TMP("TS_INTERVAL_GENERATOR_%d:%s enable=%d\n", generator_id,
            !!(config->flags & CBX_TS_GEN_CONFIG_F_ONE_SHOT) ? " One-Shot" : "", value);
    }
    return 0;
}





/*
 * TEST Functions
 *
 */



int
ts_test(int t1, int t2){
    /* simple test routine */
    TS_LOG_TMP("ts_test - called - here t1=%d t2=%d\n", t1, t2);
    return 0;
}

int
ts_tod_test(void) {
    cbx_ts_time_t time;
    uint64        timestamp;
    int           rv;

    rv = tse_tod_get(&time, &timestamp);
    sal_printf("ToD: %llu.%09lu timestamp=0x%llx (%llu)\n",
               time.sec, time.nsec, timestamp, timestamp);

    return rv;
}


int ts_event_config_test(int event_id, int source, uint32_t flags, uint32_t divider)
{
    cbx_ts_event_config_t config;

    sal_memset(&config, 0, sizeof(config));
    config.flags = flags;
    config.flags |= CBX_TS_EVENT_CONFIG_F_CB;
    config.event_cb_func = ts_test_callback;

    return cbx_ts_event_config_set(event_id, source, &config);
}


/*
    Wrapper for cbx_ts_gen_config_set
    Flags:
        #define CBX_TS_GEN_CONFIG_F_ENABLE       0x0001
        #define CBX_TS_GEN_CONFIG_F_START_TIME   0x0002 : if set, uses start time
        #define CBX_TS_GEN_CONFIG_F_ONE_SHOT     0x0004
    Generator ID's:
        #define CBX_TS_GEN_ID_TAS_1588_SYNC_A       0x0
        #define CBX_TS_GEN_ID_TAS_1588_SYNC_B       0x1
        #define CBX_TS_GEN_ID_2                     0x2
        #define CBX_TS_GEN_ID_TAS_1588_CLK          0x3
        #define CBX_TS_GEN_ID_TS_SYNC_O             0x4
        #define CBX_TS_GEN_ID_TS_FRAME_SYNC_O       0x5
        #define CBX_TS_GEN_ID_LAST                  0x6
*/
int
ts_gen_config_test( int gen_id, uint32_t flags,
                    uint64_t start_sec, uint32_t start_nsec,
                    uint32_t high_nsec, uint32_t low_nsec)
{
    cbx_ts_gen_config_t config;

    config.flags           = flags;
    config.start_time.sec  = start_sec;
    config.start_time.nsec = start_nsec;
    config.high_time_nsec  = high_nsec;
    config.low_time_nsec   = low_nsec;

    return cbx_ts_gen_config_set(gen_id, &config);
}

int
ts_event_test(int test_num, int param){
    /* simple test routine */
    /*
      0: print out stats
      1: frame_sync_o event enable - can use ts_generator_config_set or manually schedule this
      2: frame_sync_0 disable event

    */
    int ret = CBX_E_NONE;
    cbx_ts_event_config_t event_config;
    cbx_ts_gen_config_t gen_config;
    cbx_ts_time_t time;
    uint64 timestamp;

    TS_LOG_TMP("ts_event_test called with test_num=%d\n", test_num);

    switch(test_num) {

    case 0:
        /* print out stats*/
        break;

    case 1:
        /* capture events for frame_sync_o */
        sal_memset(&event_config, 0, sizeof(event_config));
        event_config.flags =  CBX_TS_EVENT_CONFIG_F_CB;
        if (param){
            event_config.flags |= CBX_TS_EVENT_CONFIG_F_ENABLE;
        }
        event_config.event_cb_func = ts_test_callback;
        ret = cbx_ts_event_config_set(CBX_TS_EVENT_ID_SYNC_IN3, CBX_TS_EVENT_SOURCE_FRAME_SYNC_OUT,
                      &event_config);
        break;

    case 2:
        /* capture events for sync_i */
        sal_memset(&event_config, 0, sizeof(event_config));
        event_config.flags = CBX_TS_EVENT_CONFIG_F_CB;
        param=1;
        if (param){
            event_config.flags |= CBX_TS_EVENT_CONFIG_F_ENABLE;
        }
        event_config.event_cb_func = ts_test_callback;
        ret = cbx_ts_event_config_set(CBX_TS_EVENT_ID_SYNC_IN2, CBX_TS_EVENT_SOURCE_SYNC_IN,
                      &event_config);
        break;

    case 3:
        /* capture events for sync_o */
        sal_memset(&event_config, 0, sizeof(event_config));
        event_config.flags = CBX_TS_EVENT_CONFIG_F_CB;
        if (param){
            event_config.flags |= CBX_TS_EVENT_CONFIG_F_ENABLE;
        }
        event_config.event_cb_func = ts_test_callback;
        ret = cbx_ts_event_config_set(CBX_TS_EVENT_ID_SYNC_IN2, CBX_TS_EVENT_SOURCE_SYNC_OUT,
                      &event_config);
        break;

    case 4:
        /* capture events for recov_clk0 */
        sal_memset(&event_config, 0, sizeof(event_config));
        event_config.flags =  CBX_TS_EVENT_CONFIG_F_CB;
        event_config.flags |= CBX_TS_EVENT_CONFIG_F_FORCE_VALID;
        if (param){
            event_config.flags |= CBX_TS_EVENT_CONFIG_F_ENABLE;
        }
        event_config.event_cb_func = ts_test_callback;
        ret = cbx_ts_event_config_set(CBX_TS_EVENT_ID_SYNCE_CLK0, CBX_TS_EVENT_SOURCE_RECOV_CLK0,
                      &event_config);
        break;

    break;
    case 5:
        /* turn on/off the ts_frame_sync_o generator at 1Hz */
        sal_memset(&gen_config, 0, sizeof(gen_config));
        if (param)
            gen_config.flags |= CBX_TS_GEN_CONFIG_F_ENABLE;
        gen_config.high_time_nsec = 0x1dcd6500;
        ret = cbx_ts_gen_config_set(CBX_TS_GEN_ID_TS_FRAME_SYNC_O, &gen_config);
        break;

    case 6:
        /* turn on/off the ts_sync_o generator at 8Khz */
        sal_memset(&gen_config, 0, sizeof(gen_config));
        if (param)
            gen_config.flags |= CBX_TS_GEN_CONFIG_F_ENABLE;
        gen_config.high_time_nsec = 0xf424;
        ret = cbx_ts_gen_config_set(CBX_TS_GEN_ID_TS_SYNC_O, &gen_config);
        break;

    case 7:
        /* 500ms one-shot sync_A CBX_TS_GEN_ID_TAS_1588_SYNC_A */
        sal_memset(&gen_config, 0, sizeof(gen_config));
        gen_config.flags = CBX_TS_GEN_CONFIG_F_ONE_SHOT;
        if (param)
            gen_config.flags |= CBX_TS_GEN_CONFIG_F_ENABLE;
        gen_config.high_time_nsec = 0x1dcd6500;  // 500ms
        ret = cbx_ts_gen_config_set(CBX_TS_GEN_ID_TAS_1588_SYNC_A, &gen_config);
        break;

    case 8:
        /* 1sec (500ms up / 500ms down) sync_A CBX_TS_GEN_ID_TAS_1588_SYNC_A */
        sal_memset(&gen_config, 0, sizeof(gen_config));
        if (param)
            gen_config.flags |= CBX_TS_GEN_CONFIG_F_ENABLE;
        gen_config.high_time_nsec = 0x1dcd6500;  // 500ms
        gen_config.low_time_nsec = 0x1dcd6500;   // 500ms
        ret = cbx_ts_gen_config_set(CBX_TS_GEN_ID_TAS_1588_SYNC_A, &gen_config);
        break;

    case 9:
        /* 500ms one-shot sync_A CBX_TS_GEN_ID_TAS_1588_SYNC_B */
        sal_memset(&gen_config, 0, sizeof(gen_config));
        gen_config.flags = CBX_TS_GEN_CONFIG_F_ONE_SHOT;
        if (param)
            gen_config.flags |= CBX_TS_GEN_CONFIG_F_ENABLE;
        gen_config.high_time_nsec = 0x1dcd6500;  // 500ms
        ret = cbx_ts_gen_config_set(CBX_TS_GEN_ID_TAS_1588_SYNC_B, &gen_config);
        break;

    case 10:
        /*
           Interval Generator 4 generates TS_SYNC_OUT. The default value 0 in
           register CRU_TS_TOP_TIMESYNC_SYNC_MUX_SEL muxes this to be SYNC_OUT.
           The SYNC_OUT event is fed back into the TS block as SYNC_IN2. This
           means that if we start capturing timestamps of SYNC_IN2 when Interval
           Generator 4 is running, the SYNC_IN2 timestamps will be corresponding
           to TS_SYNC_OUT.
        */

        /* capture events for sync_o */
        sal_memset(&event_config, 0, sizeof(event_config));
        event_config.flags = CBX_TS_EVENT_CONFIG_F_CB;
        if (param){
            event_config.flags |= CBX_TS_EVENT_CONFIG_F_ENABLE;
        }
        event_config.event_cb_func = ts_test_callback;
        ret = cbx_ts_event_config_set(CBX_TS_EVENT_ID_SYNC_IN2, CBX_TS_EVENT_SOURCE_SYNC_OUT,
                      &event_config);
        TS_LOG_TMP("cbx_ts_event_config_set ret=%d\n", ret);

        /* 1sec (500ms up / 500ms down) sync_A CBX_TS_GEN_ID_TS_SYNC_O */
        sal_memset(&gen_config, 0, sizeof(gen_config));
        if (param)
            gen_config.flags |= CBX_TS_GEN_CONFIG_F_ENABLE;
        gen_config.high_time_nsec = 0x1dcd6500;  // 500ms
        gen_config.low_time_nsec = 0x1dcd6500;   // 500ms
        ret = cbx_ts_gen_config_set(CBX_TS_GEN_ID_TS_SYNC_O, &gen_config);
        break;

    case 11:
        /* Get the time-of-day */
        (void) cbx_ts_tod_get(&time, &timestamp);
        ret = CBX_E_NONE;
        break;

    case 12:
        /*
           Same as case 10 but this will be one-shot 1 second from now
        */
        (void) cbx_ts_tod_get(&time, &timestamp);

        /* capture events for sync_o */
        sal_memset(&event_config, 0, sizeof(event_config));
        event_config.flags = (CBX_TS_EVENT_CONFIG_F_CB);
        if (param){
            event_config.flags |= CBX_TS_EVENT_CONFIG_F_ENABLE;
        }
        event_config.event_cb_func = ts_test_callback;
        ret = cbx_ts_event_config_set(CBX_TS_EVENT_ID_SYNC_IN2, CBX_TS_EVENT_SOURCE_SYNC_OUT,
                      &event_config);
        TS_LOG_TMP("cbx_ts_event_config_set ret=%d\n", ret);

        /* 1sec (500ms up / 500ms down) sync_A CBX_TS_GEN_ID_TS_SYNC_O */
        sal_memset(&gen_config, 0, sizeof(gen_config));
        gen_config.flags = (CBX_TS_GEN_CONFIG_F_START_TIME | CBX_TS_GEN_CONFIG_F_ONE_SHOT);
        if (param)
            gen_config.flags |= CBX_TS_GEN_CONFIG_F_ENABLE;
        gen_config.high_time_nsec = 0x1dcd6500;  // 500ms
        gen_config.low_time_nsec = 0x1dcd6500;   // 500ms
        gen_config.start_time.sec = time.sec + 1;
        gen_config.start_time.nsec = time.nsec;
        ret = cbx_ts_gen_config_set(CBX_TS_GEN_ID_TS_SYNC_O, &gen_config);
        break;


    default:
        TS_LOG_TMP("ERROR: unknown test %d\n", test_num);
        ret = CBX_E_FAIL;
    }
    TS_LOG_TMP("ret=%d\n", ret);


    return ret;
}
