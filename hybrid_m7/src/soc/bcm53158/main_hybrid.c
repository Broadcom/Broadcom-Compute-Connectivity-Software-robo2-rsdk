/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved.$
 *
 * $ main.c $
 */
#include "common/system.h"

//#define DEBUG_TS_WARN_MSG
#define DEBUG_TS_INTR
//#define DEBUG_TS_TOD_UPDATE
//#define DEBUG_TS_TOD_SET
#define DEBUG_TS_PHASE_ADJ
//#define DEBUG_TS_TOD_OVER_SERIAL_DETAILED
#define TS_PHASE_ADJ_LIMITATION
#ifdef TS_PHASE_ADJ_LIMITATION
    #define TS_PHASE_ADJ_INCREMENT 0x1000000
#endif
/* Any amount greater than this is considered a coarse adjustment and is
   performed by writing the timestamp registers directly. Fine adjustements are
   made usign the phase adjust registers. Arbitrarily 60 seconds */
#define TS_PHASE_ADJ_COARSE (60*1e9)

/* Automatically start 1pps frame sync out */
#define M7_FRAME_SYNC_OUT

#define TIMESYNC_INTR                   55
#define TSE_INTR_INTERRUPT_MASK         (1<<19)
#define TSE_INTR_INTERRUPT_STATUS_MASK  (0x3FFFF)



static int volatile intr=0;
static int volatile init=0;
static int volatile last_depth=0;
static int volatile last_capture_status=0;
static int volatile last_intsts=0;
static int volatile intsts_reads=0;
static int volatile intsts_reads_nz=0;
static int volatile last_val=0;
static int volatile last_val2=0;

typedef struct cbx_ts_time_s {
    volatile uint64 sec;
    volatile uint32 nsec; /* always >=0 and < 1e9 */
} cbx_ts_time_t;


static volatile cbx_ts_time_t tod={0}; /* actual time of day Epoch, 1970-01-01 00:00:00 +0000 */
static volatile uint64 tod_timestamp=0; /* value of timestamper at tod */
static volatile uint64 phase_sync_last_ts = 0ULL;
static volatile uint8  tod_event9_recvd = 0;

#ifdef M7_FRAME_SYNC_OUT
static uint8 start_1pps = 0;
#endif

static uint8 show_phase_diff = 1;

/* Forwards */
void main(void);

uint8 is_cascaded = 0;

sys_error_t bcm53158_reg_get(uint8 unit, uint32 addr, uint32 *rdata)
{
#ifdef M7_COMPILE
    uint32 address_with_unit;
    if (unit == 1 ) {
        address_with_unit = CASCADE_ADDR_GEN(addr);
    } else {
        address_with_unit = addr;
    }
    *rdata = (uint32) *(volatile uint32 *)address_with_unit;
#else
    uint8 count = 0;
    SYS_REG_WRITE32(SYSREG_IA_ADDR0, addr);
    SYS_REG_WRITE8(SYSREG_IA_CMD, 0xF2);

    while(SYS_REG_READ8(SYSREG_IA_STATUS) & SYSFLD_IA_STATUS_REG_RBUSY) {
        count++;
        if (count == SWITCH_REG_READ_WRITE_TIMEOUT_COUNT) {
            sal_printf("Reg get timeout addr:0x%lx\n", addr);
            return SYS_ERR_TIMEOUT;
        }
    }

   *rdata = SYS_REG_READ32(SYSREG_IA_RDATA0);
#endif
    return SYS_OK;
}

sys_error_t bcm53158_reg_set_by_addr(uint32 addr, uint32 wdata)
{
#ifdef M7_COMPILE
    *(volatile uint32 *)addr = wdata;
#else
    uint8 count = 0;

    SYS_REG_WRITE32(SYSREG_IA_ADDR0, addr);
    SYS_REG_WRITE32(SYSREG_IA_WDATA0, wdata);
    SYS_REG_WRITE8(SYSREG_IA_CMD, 0xF3);

    while(SYS_REG_READ8(SYSREG_IA_STATUS) & SYSFLD_IA_STATUS_REG_WBUSY) {
        count++;
        if (count == SWITCH_REG_READ_WRITE_TIMEOUT_COUNT) {
            sal_printf("Reg set timeout addr:0x%lx\n", addr);
            return SYS_ERR_TIMEOUT;
        }
    }

#endif
    return SYS_OK;
}

#ifdef M7_FRAME_SYNC_OUT
void slave_1pps_start(uint64 ts) {
    uint64 ts_secs = ts / 1000000000 + 5;
    uint32 val;

    // CBX_TS_GEN_ID_TS_FRAME_SYNC_O, Generator 5

    /* program output enable - this does nothing in Avenger */
    // CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_5_OUTPUT_ENABLE 0x40300190
    bcm53158_reg_set(0, 0x40300190, 0);

    // CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_5_CTRL 0x4030018c
    bcm53158_reg_set(0, 0x4030018c, 0);

    /* TODO: TRGTRGTRG - Deal with rollover.
       Fine for now because only an issue if the system has been running for
       78 hours or more so this is fine for the demo
    */

    // CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_5_OUTPUT_ENABLE 0x40300190
    bcm53158_reg_set(0, 0x40300190, 1);

    /* UP/Down control */

    // CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_5_UP_EVENT_CTRL 0x40300194
    bcm53158_reg_set(0, 0x40300194, 0x1dcd6500);  // 500ms

    // CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_5_DOWN_EVENT_CTRL 0x40300198
    bcm53158_reg_set(0, 0x40300198, 0x1dcd6500);  // 500ms

    /* program this in PHASE_ADJUST_LOWER and UPPER */

    // CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_5_PHASE_ADJUST_UPPER 0x403001a0
    val = (uint32)((ts_secs >> 32) & 0xffffffffLL);
    bcm53158_reg_set(0, 0x403001a0, val);

    // CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_5_PHASE_ADJUST_LOWER 0x4030019c
    val = (uint32)(ts_secs & 0xffffffffLL);
    bcm53158_reg_set(0, 0x4030019c, val);

    /* Final enable */

    // CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_5_CTRL 0x4030018c
    bcm53158_reg_set(0, 0x4030018c, 1);

    sal_printf("Starting 1pps output on slave at %llu seconds\n", ts_secs);
    start_1pps = 1;
}
#endif // M7_FRAME_SYNC_OUT


sys_error_t bcm53158_reg_set(uint8 unit, uint32 addr, uint32 wdata)
{
    sys_error_t rv = SYS_OK;

    if (unit == 0) {
        rv = bcm53158_reg_set_by_addr(addr, wdata);
    } else {
        if ((SOC_IS_CASCADED) && (unit == 1)) {
            rv = bcm53158_reg_set_by_addr(CASCADE_ADDR_GEN(addr), wdata);
        }
    }
    return rv;
}




/*
    Convert a time of day to it's corresponding 48bit timestamp value
*/
uint64 tse_tod_to_ts(cbx_ts_time_t tod) {
    uint64 ts;

    ts = (((uint64)tod.nsec + tod.sec*1000000000) & 0x0000FFFFFFFFFFFFULL);
    //sal_printf("ToD 0x%llx.0x%08lx [%llu.%09lu] >> ts 0x%llx\n", tod.sec, tod.nsec, tod.sec, tod.nsec, ts);
    return ts;
}

int tse_phase_adjust(uint64 val, uint8 neg) {
    uint32 upper;
    uint32 lower;
    uint64 adj = val;

    /* Should we check for values greater than 48bit? */
    if (neg) {
        val = 0x1000000000000ULL - val;
    }

    upper = (uint32) (val >> 32);
    lower = (uint32) (val & 0x0FFFFFFFFULL);
    // CRU_TS_CORE_TIMESYNC_TS0_PHASE_ADJUST_UPPER 0x40300234
    bcm53158_reg_set(0, 0x40300234, upper);
    // CRU_TS_CORE_TIMESYNC_TS0_PHASE_ADJUST_LOWER 0x40300230
    bcm53158_reg_set(0, 0x40300230, lower);
#ifdef DEBUG_TS_PHASE_ADJ
    sal_printf("Phase_adjust %s0x%llx [%s%llu], as %lx:%lx\n",
                neg? "-" : "", adj, neg? "-" : "", adj, upper, lower);
#endif
    return 0;
}

/*
 * Sets the ToD at the specified _past_ timestamp. Please note that the
 * timestamper has likely advanced since this timestamp, so we
 * include the tod_timestamp-timestamp delta into the actual ToD which
 * is being set.
 *
 */
int tse_tod_set(cbx_ts_time_t t, uint64 ts) {
    uint8         neg = 0;
    uint64        ts2, ts_delta, diff, nsec;
    uint32        upper, lower;

    /* Make sure the tod is current */
#ifdef CORTEX_M7
    m7_disable_intr();
#else
#endif

    /* The timestamp value corresponding to the new time */
    ts2 = tse_tod_to_ts(t);

#ifdef DEBUG_TS_TOD_SET
    sal_printf("New ToD: %llu.%09lu, ts: 0x%llu  ToD: %llu.%09lu, tod_ts: 0x%llu\n",
        t.sec, t.nsec, ts2, tod.sec, tod.nsec, tod_timestamp);
#endif


    /* Update the current ToD
       Please note that the timestamper has likely advanced since this
       timestamp, so we include the tod_timestamp-timestamp delta into the
       actual ToD which is being set.
    */
    tod.sec = t.sec;
    tod.nsec = t.nsec;
    if (tod_timestamp >= ts2) {
        diff = tod_timestamp-ts2;
    } else {
        diff = ts2-tod_timestamp;
    }
    nsec = (uint64)tod.nsec + diff;
    tod.nsec = nsec % (uint64)1e9;
    tod.sec += tod.nsec/(uint64)1e9;
    tod_timestamp = tse_tod_to_ts(tod);


#ifdef DEBUG_TS_TOD_OVER_SERIAL_DETAILED
    sal_printf("Set ToD to: %llu.%09lu ts: 0x%llxu, tod_ts: 0x%llu\n",
               tod.sec, tod.nsec, tse_tod_to_ts(tod), tod_timestamp);
#endif

    /* The delta between the new time and associated timestamp is the phase adj amount*/
    if (ts2 >= ts) {
        ts_delta = ts2 - ts;
    } else {
        ts_delta = ts - ts2;
        neg = 1;
    }

#ifdef DEBUG_TS_TOD_OVER_SERIAL_DETAILED
    sal_printf("ts2: %llu, ts: %llu  delta: %c%llu\n", ts2, ts, neg ? '-' : ' ', ts_delta);
#endif


#ifdef TS_PHASE_ADJ_LIMITATION
    if (ts_delta < TS_PHASE_ADJ_INCREMENT) {
        /* Can't do any better than this */
        if (show_phase_diff) {
            sal_printf("\n***  Fixed phase offset is : %llu ns  ***\n\n", ts_delta);
            show_phase_diff = 0;
        }
#ifdef M7_FRAME_SYNC_OUT
        if (!start_1pps) {
            slave_1pps_start(ts);
        }
#endif // M7_FRAME_SYNC_OUT
        goto return_succcess;
    } else {
        uint64 ns = ts_delta % TS_PHASE_ADJ_INCREMENT;
        if (ns < TS_PHASE_ADJ_INCREMENT/2) {
            ts_delta -= ns;
        }
        /* Just in case we lost sync */
        show_phase_diff = 1;
    }
#endif

    /* Make the phase adjustment */
    if (ts_delta > 0) {

        if (ts_delta >= TS_PHASE_ADJ_COARSE) {
            /* If greater than 1 second, write the timestamp regs directly */
            upper = (uint32) (ts2 >> 32);
            lower = (uint32) (ts2 & 0x0FFFFFFFFULL);
#ifdef DEBUG_TS_PHASE_ADJ
            sal_printf("Delta: %s0x%llx Writing timestamp regs with 0x%llx as 0x%lx:0x%lx\n",
                neg? "-" : "", ts_delta, ts2, upper, lower);
#endif
            // CRU_TS_CORE_TIMESYNC_TS0_FREQ_CTRL_UPPER 0x40300010
            bcm53158_reg_set(0, 0x40300010, upper);
            // CRU_TS_CORE_TIMESYNC_TS0_FREQ_CTRL_LOWER 0x4030000c
            bcm53158_reg_set(0, 0x4030000c, lower);
        } else {
            /* perform the phase adjustment */
            (void) tse_phase_adjust(ts_delta, neg);
        }
    }

return_succcess:
#ifdef CORTEX_M7
    m7_enable_intr();
#else
#endif
    return 0;
}



/*
 * Function:
 *    tse_tod_update
 * Description:
 *    Called anytime a timestamp is gathered from the event FIFO
 */

inline void
tse_tod_update(uint32 event_id, uint64 timestamp){
    /* compare the upper/lower 48bit TS with the tod_timestamp,
       and add the difference in nsec to the ToD.
       This function called from the interrupt context - no locking
    */
    uint64 diff;
    uint64 diff_sec, diff_nsec;
    uint64 nsec;

    if (timestamp >= tod_timestamp) {
        diff = timestamp-tod_timestamp;
        nsec = (uint64)tod.nsec + diff;
        tod.nsec = nsec % (uint64)1e9;
        tod.sec += nsec/(uint64)1e9;
        //sal_printf("tse_tod_update() POS:: New ToD: %llu.%lu, diff: %llx (%llu)\n", tod.sec, tod.nsec, diff, diff);
    } else {
        diff = tod_timestamp-timestamp;
        diff_nsec = diff % (uint64)1e9;
        diff_sec = diff / (uint64)1e9;
        if (tod.nsec < diff_nsec) {
            tod.sec--;
            nsec = (uint64)tod.nsec + (uint64)1e9 - diff_nsec;
            tod.nsec = (uint32) nsec;
        } else {
            tod.nsec -= (uint32) diff_nsec;
        }
        tod.sec -= diff_sec;
    }
    tod_timestamp = timestamp;
#ifdef DEBUG_TS_TOD_UPDATE
     sal_printf("Update ToD ev=%u to: %llu.%09lu ts: 0x%llx (%llu)\n",
               event_id, tod.sec, tod.nsec, tod_timestamp, tod_timestamp);
#endif

    if (event_id == 9) {
        tod_event9_recvd = 1;
        phase_sync_last_ts = timestamp;
    }
}

/*
 * Function:
 *    ts_intr
 * Description:
 *    Intr handler
 */


void
rimesync_handler(void)
{
    uint32 intsts;
    uint32 val,val2, val3;
    uint32 depth=0;
    uint32 fval;
    uint64 ts;
#if 00
    uint32 address=0;
#endif
    int i;
    intr++;
#ifdef CORTEX_M7
    m7_disable_intr();
#else
#endif

    // cb_events = 0;
    do {
        //REG_READ_CRU_TS_CORE_TIMESYNC_INTERRUPT_STATUSr(0, &intsts);
        bcm53158_reg_get(0, 0x40300228, &intsts);
        intsts_reads++;
        /* force reread of the FIFO */
        if (1 || intsts & TSE_INTR_INTERRUPT_MASK) {
            if (intsts){
                intsts_reads_nz++;
                last_intsts = intsts;
            }
            /* AB501 TEMP */
            val=0xFFFFFFFF;
            //REG_WRITE_CRU_TS_CORE_TIMESYNC_INTERRUPT_CLRr(0, &val);
            bcm53158_reg_set(0, 0x4030022c, val);

            //REG_READ_CRU_TS_CORE_TIMESYNC_CAPTURE_STATUSr(0, &val);
            bcm53158_reg_get(0, 0x4030021c, &val);

            //soc_CRU_TS_CORE_TIMESYNC_CAPTURE_STATUSr_field_get(0, &val, FIFO_DEPTHf, &fval);
            // FIFO_DEPTH is bits 12:6. Range for the depth is 0-64.
            fval = (val & 0x1fc0) >> 6;
            depth = fval;

            last_capture_status = val;
            last_depth = depth;
            for (i=0;i<depth;i++) {

                //REG_READ_CRU_TS_CORE_TIMESYNC_INPUT_TIME_FIFO_TS_LOWERr(0, &val);
                //REG_READ_CRU_TS_CORE_TIMESYNC_INPUT_TIME_FIFO_TS_UPPERr(0, &val2);
                bcm53158_reg_get(0, 0x4030002c, &val);
                bcm53158_reg_get(0, 0x40300030, &val2);
                last_val2=val2;
                last_val=val;

                // soc_CRU_TS_CORE_TIMESYNC_INPUT_TIME_FIFO_TS_UPPERr_field_get(0, &val2, VALIDf, &fval);
                // VALID is bit 23
                val3=fval;

                if (val3) {
                    //valid_events++;
                    //soc_CRU_TS_CORE_TIMESYNC_INPUT_TIME_FIFO_TS_UPPERr_field_get(0, &val2, EVENT_IDf, &val3);
                    // EVENT_ID is bits 22:18
                    val3 = (val2 & 0x07c0000) >> 18;

                    //soc_CRU_TS_CORE_TIMESYNC_INPUT_TIME_FIFO_TS_UPPERr_field_get(0, &val2, TIMESTAMPf, &fval);
                    // TIMESTAMP is bits 15:0 [47:32 plus 31:0 of lower]
                    fval = val2 & 0x0FFFF;
                    val2=fval;
                    ts = (((uint64)val2)<<32)|(uint64)val;

                    tse_tod_update(val3, ts);
                    #if 0 //TRGTRGTRG
                    if (ts_context.event_id_mask & 1<<val3) {
                        cb_events++;
                        ts_event_enqueue(val3, &tod);
                    }
                    #endif
#ifdef DEBUG_TS_INTR // Enable to see the timestamps logged on the console
                    // sal_printf("*** EventID-%u\n", val3);
                    sal_printf("\nEventID-%u ToD=%llu.%09lu tod_ts=0x%llx ts=0x%llx\n",
                        val3, tod.sec, tod.nsec, tod_timestamp, ts);
#endif
                } else {
#ifdef DEBUG_TS_WARN_MSG
                    sal_printf("*** EventID-%u (!val3)\n", val3);
#endif
                }
            }
        } else {
            /* AB501 what do we do here ? */
#ifdef DEBUG_TS_WARN_MSG
            sal_printf("*** What happened?\n");
#endif
        }
        if (intsts == 0) {
            break;
        }
    } while(1);
#ifdef CORTEX_M7
    m7_enable_intr();
#else
#endif
}


int read_serial(char *buffer)
{
    char ch;
    char *p = buffer;
    do {
        ch = sal_getchar();
        switch(ch) {
        case '\0':
            return (p - buffer);
        case '\n':
        case '\r':
            *p = '\0';
            return (p - buffer);
        default:
            *p++ = ch;
        }
    } while (1);
}

int ts_tod_string_parse(char *str_tod, uint64 *secs) {
    uint32 hh,mm,ss,ss_frac,dd,mo,zh,zm,cksum;
    uint32 yyyy;
#ifdef DEBUG_TS_TOD_OVER_SERIAL_DETAILED
    sal_printf("Parsing ToD : %s\n", str_tod);
#endif
    if (sal_sscanf(str_tod,
        "$GPZDA,%02u%02u%02u.%02u,%02u,%02u,%04u,%02u,%02u*%02x",
        &hh,&mm,&ss,&ss_frac,&dd,&mo,&yyyy,&zh,&zm,&cksum) != 10) {
        sal_printf("ts_tod_string_parse(): Failed to parse ToD\n");
        return -1;
    }
#ifdef DEBUG_TS_TOD_OVER_SERIAL_DETAILED
    sal_printf("Parsed: %02u%02u%02u.%02u,%02u,%02u,%04u,%02u,%02u*%02x\n",
        hh,mm,ss,ss_frac,dd,mo,yyyy,zh,zm,cksum);
#endif
    *secs = ((dd-1)*86400ULL + hh*3600ULL + mm*60ULL + ss*1ULL);
#ifdef DEBUG_TS_TOD_OVER_SERIAL_DETAILED
    sal_printf("ts_tod_string_parse() ToD secs : %llu\n", *secs);
#endif
    return 0;
}


void
main(void)
{
    uint32 val;
    char buf[64];
    int len;
    cbx_ts_time_t tod_serial;
    int tod_len = sal_strlen("$GPZDA,hhmmss.ss,dd,mm,yyyy,xx,yy*CC");
    uint64 secs;

    /* ----------------------------------------------------------------------
       tse_init()
       ---------------------------------------------------------------------- */
    m7_disable_intr();
    m7_enable_irq(TIMESYNC_INTR);
    m7_enable_intr();

    /* Turn off generator 0 (SyncA) which is on by default */
    // CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_0_OUTPUT_ENABLE 0x40300104
    val = 0;
    bcm53158_reg_set(0, 0x40300104, val);
    // CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_0_CTRL 0x40300100
    val = 0;
    bcm53158_reg_set(0, 0x40300100, val);

    /* Turn off generator 1 (SyncB) which is on by default */
    // CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_1_OUTPUT_ENABLE 0x40300120
    val = 0;
    bcm53158_reg_set(0, 0x40300120, val);
    // CRU_TS_CORE_TIMESYNC_INTERVAL_GENERATOR_1_CTRL 0x4030011c
    val=0;
    bcm53158_reg_set(0, 0x4030011c, val);
    /* ----------------------------------------------------------------------
       end tse_init()
       ---------------------------------------------------------------------- */


    sysprintf( "\nBCM53158 SWITCH Flash firmware " __DATE__ " : " __TIME__ "\n");

    sysprintf( "Executing from M7 -- Going into while-1\n");

    /* If this for loop is running we do not see most of the interrupts */
    for(;;){
        len = read_serial(buf);
#ifdef DEBUG_TS_TOD_OVER_SERIAL_DETAILED
        sal_printf("string: %s len:%d\n",buf,len);
#endif
        if (len == tod_len) {
            if (ts_tod_string_parse(buf, &secs) == 0) {
                tod_serial.sec = secs;
                tod_serial.nsec = 0;

                if (tod_event9_recvd == 1) {
                    (void) tse_tod_set(tod_serial, phase_sync_last_ts);
                    tod_event9_recvd = 0;
                } else {
#ifdef DEBUG_TS_WARN_MSG
                    sysprintf("**** No phase sync event received\n");
#endif
                }
            }
        }
    }

    while (1) ;

}

