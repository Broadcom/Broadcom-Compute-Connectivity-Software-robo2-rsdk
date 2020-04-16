/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 */

/**
 * @file
 * TimeSync
 * Forwarding Services Abstraction Layer (FSAL) API
 * TS SYNCE EVENT GEN
 */

#ifndef TS_API_H_
#define TS_API_H_

#include <sal_sync.h>


#ifdef __cplusplus
extern "C" {
#endif

#if 00 /* makes auto indent work properly */
}
#endif

#ifdef TS_TOD_OVER_SERIAL
  //#define DEBUG_SERIAL_PORT
  #define DEBUG_TS_TOD_OVER_SERIAL_BASIC
  //#define DEBUG_TS_TOD_OVER_SERIAL_DETAILED
  // #define DEBUG_TS_TOD_OVER_SERIAL_VERBOSE
#endif // TS_TOD_OVER_SERIAL

//#define DEBUG_TS_TOD_SET
//#define DEBUG_TS_TOD_UPDATE
#define DEBUG_TS_PHASE_ADJ
#define TS_PHASE_ADJ_LIMITATION
#ifdef TS_PHASE_ADJ_LIMITATION
    #define TS_PHASE_ADJ_INCREMENT 0x1000000
#endif

/* Any amount greater than this is considered a coarse adjustment and is
   performed by writing the timestamp registers directly. Fine adjustements are
   made usign the phase adjust registers */
#define TS_PHASE_ADJ_COARSE (1000000000ULL)  /* 1 Second */

//#define DEBUG_TS_CALLBACK
//#define DEBUG_TS_CALLBACK1
//#define DEBUG_TS_CALLBACK2

//#define DEBUG_TS_INTR
//#define TS_DEBUG
#ifdef TS_DEBUG
#define TS_LOG_TMP(x...) sal_printf(x)
#else
#define TS_LOG_TMP(x...)
#endif

 /*
 Driver Documentation:

 The driver and top-half of interrupt handler lives in this file.
 The bottom half of the interrupt driver lives in tse.c
 The driver provides:
   0. TOD management - this mostly lives in the tse.c. We maintain
     a mapping of the timestamper (48bit value) and a Time-of-Day.
     Assuming an initial relationship between TOD and timestamper,
     every time an event is received, the timestamp of that event
     is compared with the tod_timestamp and the delta is then added
     to the TOD. This way, the tod is always in sync with tod_timestamp.
     This TOD/timestamp mechanism allows for an application to do stuff
     like:
       a) get current TOD
       b) schedule an event to happen precisely at some future TOD
       c) get a callback with the TOD/timestamp for an event that
         has occurred.
     The main reason for TOD is that the timestamper is 48bits and the
     LSB is nanonseconds, so it rolls over every 78 hours or so.
     Phase-aware timing applications need to maintain accurate phase for
     longer than that, so TOD is used for this purpose.
     The TOD can also be set at some future timestamp value, which is how
     an application may adjust the time. For instance, if I get a 1 pulse
     per second from a GPS, and I timestamp that. A bit after, I'll get the
     actual timecode over UART or some other slow mechansim. The SW will then
     take that timecode and apply it when the local timestamper reaches the
     next second (based on the previous timestamp of the 1pps from the GPS)
   1. event configuration - maps sources to one of the event inputs
     of the timesync block. Not all sources can be connected to all
     the inputs, so there are some helper defines to describe what's
     allowed
     An event can result in interrupts and captured timestamps, OR
     it can be used to drive the HW DPLL, which is the DPLL block in
     the timesync block.
     There are several muxes along the way from source to timestamping
     as well as to the HW DPLL. The single cbx_ts_event_config_set should
     mask all of this complexity, and return an error for any unsupported
     configuration.s
   2. generator configuration - this allows for oneshot or repetitive
     generation of pulses.
   3. HW DPLL - mentioned abit about this above. The HW dpll APIs need only
     configure/enable the DPLL. Any muxing required is handled by the
     cbx_ts_event_config_set call
   4. DFLL: this code is run in the bottom half of the IRQ, and provides
     closed loop operation of the TimeSync LCPLL. A typical application is that
     we get synce clock from GE port. This is divided down to something like 8Khz.
     We connect this clock to the HW DPLL, and our local timebase will noe be
     frequency aligned with the synce reference.
     In order to propagate SYNCE to othe GE ports, we need to control the LCPLL
     via fractional modifications. The LCPLL output can be timestamped by the
     timesync timestamper, and a collection of such timestamps is used by the
     DFLL code to make further adjustments tothe LCPLL fraction control, in order
     to bring the LCPLL's frequency to be exactly the same as that of the local
     timestamper. It's initially expected that this adjustment is made at 8Khz
     (every 125Us, TS IRQ gets an event, and this event is passed along to the DFLL
     code). We may reduce this frequency once the DFLL is locked.
   5. SW DPLL: This code performs the same function as the HW DPLL, however, it also
     incorporates a 1pps phase input.  This code may not be needed, given that the HW
     DPLL can be used for most applications. If the Avenger is acting as a 1588 GM or is
     being synchronized to another Avenger, this function may be used.
   6. event ring: the bottom half of TS interrupt handler communicates with the
     top half via a simple event ring and a semaphore. The same basic mechanism
     seems to exist in hpa.c/rx.c.
     The driver will provide a simple bitmask hint to the interrupt handler which
     specifies which event IDs should be steered to the ring. Actual configuration of
     hardware to cause the events to happen is handled by the driver.  Please keep in mind that
     in DFLL/SWDPLL scenarios, events will be configured, but the IRQ handler will
     not send these events to the driver. Instead, they are consumed locally.
     An application may register for an event via provided callback. For instance, when
     using HW DPLL, the phase information is not handled directly by HW DPLL. The application
     must select some input (eg. frame_sync_i) as the 1pps source, and must enable this
     event for timestamping. The application provides a callback, and will be notified
     of any arriving event, with tod + tod_timestamp. This allows the application to
     take required steps as required.
     For example, let's assume we're doing TSN. In this example, the TSN cycle of 20ms will be
     aligned  exactly to when the TOD is 1second aligned. For example, when TOD strikes
     12:04.000000000 I can expect that a TSN cycle will start at that exact time.
     The TSN application will register for 1pps event (this can be external, or it can be
     due to 1588/AS running, and the 1pps is generated by an internal generator). When this
     event first occurs, the TSN application can schedule the TSN sync strobes to start
     when the next second rolls around. The TSN application gets the 1second event callback
     and this callback contains the tod and tod_timestamp of this event. The application
     then adds 1 second to the TOD, and then uses the new (future) TOD to schedule the TSN
     events.
   7. Telecom PLL: further investigation is needed to bring the full-featured Telecom PLL
     from CSG code base. This SW DPLL will support multiple frequency and phase sources,
     and can perform stuff like automatic failover from one frequency source to another as
     well as holdover determination.
   8. NCO control: The Timesync block has a software controllable NCO. The use of this
     NCO is exclusive to the use of the HW DPLL, meaning that the HW DPLL uses the NCO
     controls to perform it's DPLL function.
     The API must support setting the nominal frequency which is used to configure the
     nominal NCO increment value, and will also be used to compute the fractional control
     values as a function of the passed in frequency_offset, which is specified in parts-
     per-billion.
   A. Appendix - 1588 slave usecase
     In this usecase, a 1588 stack and servo is using the TimeSync time base to collect
     the 1588 T1,T2,T3 and T4 timesamps and to convert a sequence of these time stamps
     into a frequency and phase offset changes to the local TOD. The Tx timestamps
     are Time-of-Day.  Typically, the 1588
     application will provide sets of timestamps (T1,T2,T3 and T4) to the servo at a rate
     that's typically  is around 64 or 128/second. The servo collects these timestamps
     and when it has collected enough data, it will start providing frequency and
     phase correction information back to the platform. The platform should apply the
     frequency and phase correction to the local NCO. This interaction creates a
     control loop that will run throughout the duration of the 1588 master/slave session.
     The APIs used in this case are primarily the NCO API: ts_nco_config_set.
     The API is used to set the nominal frequency to 250Mhz, and then when the servo provides
     frequency and phase corrections, the application will use the ts_nco_config_set to
     implement the offsets the servo requires.
     For example, say our local oscillator is 150ppb off from the master. Initially, our
     NCO is configured for nominal increment. After a while, the servo will detect the
     frequency offset (150ppb), and ask the application to apply a -150ppb frequency offset
     to the NCO. Once this is done, the local frequency is the same as the master's frequency.
     Around this same time, phase corrections will also be provided.
   B. Appendix - 1588 slave with SyncE frequency
     There are two flavors of this. One is where the servo also receives a timestamp of
     a 1pps, which is aligned to the recovered SyncE
 */




/* AB501: add documentation comments
   Notes:
   1. CPU event used internally by this module to
     a. get "recent" timestamps if no other events are enabled
     b. schedule recurring events or cancel events
     c. could implement complex PWM or BroadSync

*/

/*
 ******************************************************
 * TS MISC defines
 ******************************************************
*/

int
ts_init(void);

/*
 ******************************************************
 * TS TIME/TOD defines
 ******************************************************
*/

typedef struct cbx_ts_time_s {
    uint64_t sec;
    uint32_t nsec; /* always >=0 and < 1e9 */
} cbx_ts_time_t;

#define ts_time_t_init(x) (sal_memset(&x, 0, sizeof(cbx_ts_time_t)))

typedef struct cbx_ts_event_s {
    uint32_t flags;
    uint32_t event_id;
    cbx_ts_time_t tod;   /* time event was captured from HW */
} cbx_ts_event_t;

#define TS_EVENT_RING_SIZE 64
#define TS_EVENT_THREAD_PRI 10

/*
  The ts_context contains the communication mechanism between the irq code in tse.c
  and the driver code. The driver will set bits in the event_id_mask corresponding to
  event IDs, which tells the IRQ routine that if that particular event has been captured
  in the timesync timestamp FIFO, the event id and timestamp should be passed along to
  the driver. The mechanism for passing this to the driver is based on a simple ring which
  has a static allocation. The event_ring has a producer pointer which is set by the IRQ and
  is read by the driver, and a consumer pointer which is set by the driver and read by the IRQ.
  No additional locking is needed in the ring transactions.
  The IRQ will also provide an er_status bit map, which can be used to indicate that the ring
  was full. Clearing this bit map must necessarily be wrapped in a m7_disable/enable_intr block

  The event_ring is empty when the er_producer_idx==er_consumer_idx. When the irq adds
  an event to the ring, it will increment the er_producer_idx. When the er_producer_idx reaches
  the TS_EVENT_RING_SIZE-1, it's next increment will wrap to 0. When the er_producer_idx is
  one less than the er_consumer_idx (with wrapping accounted for), then the ring is full, and
  the irq will simply drop the event. An overflow should be accounted for in the er_status.

  The driver will fork off a task which blocks on the er_sem. When the IRQ updates the producer
  index, it will give the sem, kicking the driver task to process the entries in the ring. These
  are processed in a scheduled task, and not in the interrupt context.

  Please note that DFLL and SW DPLL functions will not generate events in the event_ring, and instead
  those events are processed directly in the IRQ, due to the real-time nature and frequency of these
  events. The DFLL and SW DPLL may run at a frequency as high as 8Khz.

  To enable the DFLL, the driver will configure appropriate event (divider, etc), configure the
  DFLL, then set the dfll_event_id accordingly. The event_id_mask will _NOT_ be set for this event.
  The same mechanism exist for the swdpll. In this case, two event IDs must be used, one which
  provides frequency and another which provides phase. It's typical for the swdpll_phase_event_id
  to also be set in the event_id_mask, which allows the application software to be notified at
  a rate that is typically once per second (1pps).

  TODO:
    1. Set up a scheme for capturing first or latest event that caused the overflow error -
      Do we need this ?

*/

#define TS_CONTEXT_ER_STATUS_F_OVERFLOW 0x0001
#define TS_CONTEXT_ER_STATUS_F_FULL     0x0002

typedef struct ts_context_s {
    volatile uint32_t flags;
    volatile uint32_t event_id_mask;          /* set by driver, IRQ samples this on interrupts */
    volatile uint32_t dfll_event_id;          /* 0 if dfll_event_id is unused */
    volatile uint32_t swdpll_freq_event_id;   /* eg. 4 or 8 Khz  - 0 if unused */
    volatile uint32_t swdpll_phase_event_id;  /* eg. 1Hz - 0 if unused */
       cbx_ts_event_t event_ring[TS_EVENT_RING_SIZE];
    volatile uint32_t er_producer_idx;        /* IRQ is the producer */
    volatile uint32_t er_consumer_idx;        /* Driver is consumer */
    volatile uint32_t er_status;              /* set by IRQ, cleared by driver using irqdisable */
    sal_sem_t er_sem;
} ts_context_t;


static inline
int64_t ts_time_diff(cbx_ts_time_t s1, cbx_ts_time_t s2){
    return (uint64_t)(s1.sec - s2.sec)*1e9 + s1.nsec - s2.nsec;
}


/* Gets the current TOD and associated timestamp - this function samples the latest
   timestamper value, and also updates the global TOD
*/
int cbx_ts_tod_get(cbx_ts_time_t *time, uint64_t *timestamp);

/*
  Set time at a corresponding future timestamp. This function is mostly
  used in conjunction with cbx_ts_tod_get, where the latest timestamp-tod
  relationship is established, and from there, this function can be used
  to set the time at some future timestamp.
  For example, if the application requires a phase shift of 100ns, then it
  can can call cbx_ts_tod_get(tod, ts). It can then pick some time in the future, say
  tod+1second, and compute the offset of ts such that it's adjusted by 100ns. In this case
  it would call cbx_ts_tod_set(tod+1 second, ts+1e9+100).
*/
int cbx_ts_tod_set(cbx_ts_time_t *time, uint64_t timestamp);


/*
 ******************************************************
 * TS EVENT defines
 ******************************************************
*/
#define TS_EVENT_NUM_EVENTS 20



#define TS_EVENT_F_TIME_VALID   0x0001    /* time portion is valid */
#define TS_EVENT_F_DISABLED     0x0002    /* this event was disabled */
//typedef struct ts_event_s{
//    uint16_t id;
//    uint16_t flags;
//    cbx_ts_time_t time;
//} cbx_ts_event_t;
#define ts_event_t_init(x) (sal_memset(&x, 0, sizeof(cbx_ts_event_t)))

typedef void (*cbx_ts_event_cb_t)( int event_id, cbx_ts_event_t* ts_event);


/* EVENT IDs - */
#define CBX_TS_EVENT_ID_CPU            0
#define CBX_TS_EVENT_ID_GPIO0          1
#define CBX_TS_EVENT_ID_GPIO1          2
#define CBX_TS_EVENT_ID_GPIO2          3
#define CBX_TS_EVENT_ID_GPIO3          4
#define CBX_TS_EVENT_ID_GPIO4          5
#define CBX_TS_EVENT_ID_GPIO5          6
#define CBX_TS_EVENT_ID_GPIO6          7
#define CBX_TS_EVENT_ID_GPIO7          8
#define CBX_TS_EVENT_ID_SYNCE_CLK0     9
#define CBX_TS_EVENT_ID_SYNCE_CLK1     10
#define CBX_TS_EVENT_ID_SYNCE_CLK2     11
#define CBX_TS_EVENT_ID_SYNCE_CLK3     12
#define CBX_TS_EVENT_ID_SYNCE_CLK4     13
#define CBX_TS_EVENT_ID_SYNCE_CLK5     14
#define CBX_TS_EVENT_ID_SYNC_IN0       15
#define CBX_TS_EVENT_ID_SYNC_IN1       16
#define CBX_TS_EVENT_ID_SYNC_IN2       17
#define CBX_TS_EVENT_ID_SYNC_IN3       18

#define CBX_TS_EVENT_SOURCE_GPHY0_CLK0     0
#define CBX_TS_EVENT_SOURCE_GPHY0_CLK1     1
#define CBX_TS_EVENT_SOURCE_GPHY0_CLK2     2
#define CBX_TS_EVENT_SOURCE_GPHY0_CLK3     3
#define CBX_TS_EVENT_SOURCE_GPHY1_CLK0     4
#define CBX_TS_EVENT_SOURCE_GPHY1_CLK1     5
#define CBX_TS_EVENT_SOURCE_GPHY1_CLK2     6
#define CBX_TS_EVENT_SOURCE_GPHY1_CLK3     7
#define CBX_TS_EVENT_SOURCE_XFI0_CLK       8
#define CBX_TS_EVENT_SOURCE_XFI1_CLK       9
#define CBX_TS_EVENT_SOURCE_RECOV_CLK0     10
#define CBX_TS_EVENT_SOURCE_RECOV_CLK1     11
#define CBX_TS_EVENT_SOURCE_RECOV_CLK2     12
#define CBX_TS_EVENT_SOURCE_TS_TIMER0      13
#define CBX_TS_EVENT_SOURCE_TS_TIMER1      14
#define CBX_TS_EVENT_SOURCE_LCPLL          15
#define CBX_TS_EVENT_SOURCE_SYNC_IN        16
#define CBX_TS_EVENT_SOURCE_SYNC_OUT       17
#define CBX_TS_EVENT_SOURCE_FRAME_SYNC_IN  18
#define CBX_TS_EVENT_SOURCE_FRAME_SYNC_OUT 19

/* helpers which show what sources are valid for which events */

/* valid for CBX_TS_EVENT_ID_SYNCE* */
#define CBX_TS_EVENT_ID_SYNCE_VALID_SOURCES \
    ((1<<CBX_TS_EVENT_SOURCE_GPHY0_CLK0) | (1<<CBX_TS_EVENT_SOURCE_GPHY0_CLK1) | \
     (1<<CBX_TS_EVENT_SOURCE_GPHY0_CLK2) | (1<<CBX_TS_EVENT_SOURCE_GPHY0_CLK3) | \
     (1<<CBX_TS_EVENT_SOURCE_GPHY1_CLK0) | (1<<CBX_TS_EVENT_SOURCE_GPHY1_CLK1) | \
     (1<<CBX_TS_EVENT_SOURCE_GPHY1_CLK2) | (1<<CBX_TS_EVENT_SOURCE_GPHY1_CLK3) | \
     (1<<CBX_TS_EVENT_SOURCE_XFI0_CLK)   | (1<<CBX_TS_EVENT_SOURCE_XFI1_CLK)   | \
     (1<<CBX_TS_EVENT_SOURCE_RECOV_CLK0) | (1<<CBX_TS_EVENT_SOURCE_RECOV_CLK1) | \
     (1<<CBX_TS_EVENT_SOURCE_RECOV_CLK2) | (1<<CBX_TS_EVENT_SOURCE_TS_TIMER0)  | \
     (1<<CBX_TS_EVENT_SOURCE_TS_TIMER1)  | (1<<CBX_TS_EVENT_SOURCE_LCPLL))

#define CBX_TS_EVENT_ID_SYNC_IN2_VALID_SOURCES \
    ((1<<CBX_TS_EVENT_SOURCE_SYNC_IN) | (1<<CBX_TS_EVENT_SOURCE_SYNC_OUT))

#define CBX_TS_EVENT_ID_SYNC_IN3_VALID_SOURCES \
    ((1<<CBX_TS_EVENT_SOURCE_FRAME_SYNC_IN) | (1<<CBX_TS_EVENT_SOURCE_FRAME_SYNC_OUT))

#define CBX_TS_EVENT_CONFIG_F_ENABLE        0x0001    /* event is enabled */
#define CBX_TS_EVENT_CONFIG_F_DIVIDER       0x0002    /* divider value should be used */
#define CBX_TS_EVENT_CONFIG_F_CB            0x0004    /* callback should be used - enables capture*/
#define CBX_TS_EVENT_CONFIG_F_FORCE_VALID   0x0008    /* for synce, ignore valid signal */
#define CBX_TS_EVENT_CONFIG_F_HW_DPLL       0x0010    /* Event connected directly to HW DPLL */
#define CBX_TS_EVENT_CONFIG_F_CUST_1        0x0020    /* CUST_1 flag to invoke ts_callback1 */
#define CBX_TS_EVENT_CONFIG_F_CUST_2        0x0040    /* CUST_2 flag to invoke ts_callback2 */

typedef struct cbx_ts_event_config_s {
    uint32_t flags;
    uint32_t divider;
    cbx_ts_event_cb_t event_cb_func; /* provide callback to get timestamps */

} cbx_ts_event_config_t;


/*
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
*/

int cbx_ts_event_config_set(int event_id, int event_source, cbx_ts_event_config_t* config);


/*
 ******************************************************
 * TS GENERATOR defines
 ******************************************************
*/



/* Interval generator mapping */
#define CBX_TS_GEN_ID_TAS_1588_SYNC_A       0x0
#define CBX_TS_GEN_ID_TAS_1588_SYNC_B       0x1
#define CBX_TS_GEN_ID_2                     0x2
#define CBX_TS_GEN_ID_TAS_1588_CLK          0x3
#define CBX_TS_GEN_ID_TS_SYNC_O             0x4
#define CBX_TS_GEN_ID_TS_FRAME_SYNC_O       0x5
#define CBX_TS_GEN_ID_LAST                  0x6

/* TS GEN defines*/
#define CBX_TS_GEN_CONFIG_F_ENABLE       0x0001
#define CBX_TS_GEN_CONFIG_F_START_TIME   0x0002 /* if set, uses start time */
#define CBX_TS_GEN_CONFIG_F_ONE_SHOT     0x0004


typedef struct cbx_ts_gen_config_s {
    uint32_t flags;
    cbx_ts_time_t start_time;
    uint32_t high_time_nsec;
    uint32_t low_time_nsec;
} cbx_ts_gen_config_t;

/*
 * ts_gen_config set will enable a repeated or one-shot event which may start
 * at some specified time in the future.
 * generator_id - the generator being configured - use CBX_TS_GEN_ID_*
 * config:
 *   flags
 *     CBX_TS_GEN_CONFIG_F_ENABLE - set this bit to enable generator
 *     CBX_TS_GEN_CONFIG_F_START_TIME - when flag is set, use start_time to generate
 *       the first event, or only even if it's one-shot. The time must be in the future
 *       use cbx_ts_tod_get to ensure start_time is in the future. If not specified, the
 *       event will be scheduled ASAP.
 *     CBX_TS_GEN_CONFIG_F_ONE_SHOT - will generate single event instead of repeated event. If
 *   start_time: TOD when event should start
 *   high_time_nsec: number of nanonseconds of the high pulse
 *   low_time_nsec: number of nanonseconds of the low pulse
 * For repeated events, the period of the event waveform will be high_time+low_time
 */
int cbx_ts_gen_config_set(int generator_id, cbx_ts_gen_config_t* config);



/*
 ******************************************************
 * TS NCO defines
 ******************************************************
*/
#define TS_NCO_CONFIG_F_NOMINAL_FREQUENCY    0x0001 /* nominal_frequency is valid   */
#define TS_NCO_CONFIG_F_FREQ_OFFSET          0x0002 /* frequency offset is provided */
#define TS_NCO_CONFIG_F_PHASE_OFFSET         0x0004 /* one-time phase_offset is provided */


typedef struct ts_nco_config_s {
    uint32_t flags;
    int32_t nominal_frequency; /* Should be set to 250Mhz */
    int32_t freq_offset; /* positive or negative parts-per-billion */
    int32_t phase_offset; /* one-time phase offset in nanoseconds */
} ts_nco_config_t;


/* Numerically Controlled Oscillator (NCO) is used to maintain a timebase which
   is offset in frequency and phase from local frequency reference. Some application
   such as
int ts_nco_set(ts_nco_config_t* config);
*/
/*
 ******************************************************
 * TS HW DPLL config
 ******************************************************
*/

#define TS_HWDPLL_CONFIG_F_

#if 0
typedef struct ts_hwdpll_config_s {


int ts_hwdpll_config_set(ts_hwdpll_config_t config);

#endif // 0



#ifdef __cplusplus
}
#endif

#endif /* TS_API_H */
