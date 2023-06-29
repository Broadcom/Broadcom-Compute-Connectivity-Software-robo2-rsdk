/*
 * $Copyright: (c) 2017 Broadcom Limited.
 */

/**
 * @file
 * Coronado Bridge
 * Forwarding Services Abstraction Layer (FSAL) API
 */

#ifndef CBX_API_COSQ_H_
#define CBX_API_COSQ_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @defgroup cbx_cosq CBX COSQ
 * Forwarding Services Abstraction Layer (FSAL) CoS Queue operations.
 *
 * <h3>COSQ Mappings</h3>
 *
 * <h4>Port Group Ingress CoS Functionality</h4>
 * Mapping can be based on:
 * - cos_map_basex[4:0]
 * - cos_src_selx[3:0]
 * - tc_src_selx[8]
 *
 * <h4>Traffic Class Functionality</h4>
 * Selection can based on:
 * - Port Group
 * - Mac Destination Address
 * - 802.1p VLAN Pri
 * - IP TOS/DHCP
 *
 * <h3>CoS Queue Programming</h3>
 * Avenger Implementation details
 *
 * <h4>Canonical Priority Mapping Table</h4>
 * - 384 entries
 *   configuration:  16[ports] x 16[dei,pcp] = 256 entries
 *                   per-port mapping of PCP to TC and DEI to DP (green/yellow)
 *                   Port N is at offset  n*16 in the CPMT
 *
 *                   2 maps for DSCP (ipv4 and ipv6) with 64 entries each
 *                   IPv4 at offset 256
 *                   IPv6 at offset 320
 *
 * <h3>Admission Profile Table (APT)</h3>
 * Support up to four admission profiles.
 * An admission profile contains:
 *  TBD
 * It is selected per Port Group
 * PG2AP ap[1:0]  Maps the port group to an admission control policy
 * (Admission Profile) including buffer usage thresholds, WRED, etc.
 *
 * <h3>Egress Queue & Admission Control</h3>
 * Once the forwarding decision is made, frames are placed in egress queues
 * based on {destination port, TC}.
 * Avenger supports RED/WRED as well as buffer usage limitations per egress queue.
 * The final TC, DP is used for these functions.
 *
 * <h3>Egress Edits</h3>
 * Avenger provides a packet editing capability on the egress.
 * The DSCP and/or tag PCP,DEI may be updated based on the final TC, DP.
 *
 * <h4>Priority Map Table (PMT)</h4>
 * TC/DP to CoS mapper
 * Egress Packet Processing Table that translates:
 * (32 maps x 4 bits   OR 16 maps x 8 bits)
 * canonical (TC, DP) to values used to populate priority fields in packet
 * Each map has 24 (DP=0b11 code-point is not used) entries
 * Entries may be either 4 or 8b wide.
 *
 *  @{
 */

/**
 * @def CBX_COSQ_INVALID
 * Invalid COSQ
 */
#define CBX_COSQ_INVALID   0xffffffff

/** @def CBX_COSQ_MAX
 * Maximum COSQ entries
 */
#define CBX_COSQ_MAX  256

/** @def CBX_COS_COUNT
 * COSQ traffic classes:
 * - Number of queues per port
 */
#define CBX_COS_COUNT 8

/**
 * COSQ parameter struct
 * information about a cosq
 */
typedef struct cbx_cosq_params_t {
    int               en_tc;      /**< enable TC from packet */
} cbx_cosq_params_t;


/**
 * Drop Precedence enumerations
 */
typedef enum cbx_dp_e {
    cbxDpGreen,         /**< (0): Indicates a packet with low drop precedence. */
    cbxDpYellow,        /**< (1): Indicates a packet of medium drop precedence. */
    cbxDpRed,           /**< (2): The highest drop precedence. */
    cbxDpDrop,          /**< (3): Used as a drop indicator. */
} cbx_dp_t;

/**
 * COSQ traffic class struct
 * - internal traffic class priority and drop precedence
 */
typedef struct cbx_tc_t {
    int        int_pri;    /**< Internal Priority */
    cbx_dp_t   int_dp;     /**< Internal Drop Precedence */
} cbx_tc_t;


/**
 * COSQ DiffServ struct
 * - DiffServ configuration parameters
 */
typedef struct cbx_dscp_t {
    int        dscp_cp;    /**< DSCP Code Point */
    int        is_ipv6;    /**< IPv4=0, IPv6=1 */
} cbx_dscp_t;

/**
 * Scheduling mode enumerations
 * - in WRR and WDRR, weights of -1 are used to indicated Strict Priority
 * programming:
 *     PQM_qdschcfg                       selects SP/RR
 *     PQS_pqs_config_port_a0             algorithm_sel: WRR/WDRR
 *     PQS_weight_high/low_queue_port_a0  weights
 */
typedef enum cbx_schedule_mode_e {
    cbxScheduleStrict,  /**< (0): highest priority packets egress first.
                                  in this mode weight is ignored. */
    cbxScheduleRR,      /**< (1): simple round robin algorithm.
                                  in this mode weight is ignored. */
    cbxScheduleWRR,     /**< (2): weighted round robin algorithm. */
    cbxScheduleWDRR,    /**< (3): deficit round robin algorithm */
} cbx_schedule_mode_t;

/**
 * Flowcontrol mode enumerations
 * programming:
 * 1) source port based pause
 *    PGT.fcd   set all fcd 0-7 to same value (eg. 0) for one domain
 * 2) PFC based on traffic class
 *    PGT.fcd   set fcd 0-7 to traffic class 0-7
 * 3) BMU Metering to provide shaping based per port pause
 *
 */
typedef enum cbx_flowcontrol_e {
    cbxFlowcontrolNone,    /**< (0): Global Flowcontrol disabled */
    cbxFlowcontrolPause,   /**< (1): Per port flowcontrol - Pause Packets */
    cbxFlowcontrolPFC,     /**< (2): Priority flowcontrol (PFC) mode */
    cbxFlowcontrolMeter,   /**< (3): Shaping per port - Pause Packets */
} cbx_flowcontrol_t;

/**
 * Flowcontrol parameters
 *
 */
typedef struct cbx_flowcontrol_params_t {
    int   enable;     /**< Flowcontrol enable */
    int   thresh3;    /**< The threshold at which all packets from the associated
                           ingress are dropped. Flow control remains asserted. */
    int   thresh2;    /**< The threshold at which packets with yellow drop precedence
                           are dropped. Flow control remains asserted. */
    int   thresh1;    /**< The threshold at which packets with red drop precedence
                           are dropped. Flow control remains asserted. */
    int   thresh0;    /**< The threshold at which flow control is asserted. */
    int   hysteresis; /**< The point to which a flow controlled ingress' buffer occupancy
                           must fall to before flow control goes from asserted to negated. */
} cbx_flowcontrol_params_t;

/**
 * Initialize COSQ flowcontrol object
 *
 * @param  portid               Port Identifier
 * @param  flowcontrol_params   Flow Control parameters
 */
void cbx_flowcontrol_params_t_init( cbx_portid_t              portid,
                                    cbx_flowcontrol_params_t *flowcontrol_params );

/**
 * Shaper Flags
 */
#define  CBX_COSQ_SHAPER_MODE_AVB       0x00000001  /**< AVB mode @hideinitializer */
#define  CBX_COSQ_SHAPER_MODE_PACKETS   0x00000002  /**< Packet mode @hideinitializer */

/**
 * Shaper configuration paramters
 * - configuration information about egress shapers
 */
typedef struct cbx_cosq_shaper_params_t {
    uint32_t flags;             /**< Flags:  CBX_COS_SHAPER_XX */
    uint32_t bits_sec;          /**< Committed rate in kbps or packets per second. */
    uint32_t max_bits_sec;      /**< Maximum rate */
    uint32_t bits_burst;        /**< Committed burst size in kbits or number of packets. */
    uint32_t kbits_current;     /**< Current size. */
} cbx_cosq_shaper_params_t;

#ifdef CONFIG_TIMESYNC
/**
 * TSN Flags
 */
/* if set TSN enabled on queue else disable */
#define  CBX_COSQ_TSN_QUEUE_CONFIG_ENABLE  0x00000001
/* if set signle cycle start else periodic cycle start */
#define  CBX_COSQ_TSN_CT_CONFIG_ONE_SHOT   0x00000002
/* if set config change is triggered */
#define  CBX_COSQ_TSN_CONFIG_CHANGE        0x00000004

/*
 * TSN start states
 */
typedef enum cbx_tsn_state_e {
    cbxTsnStateGb=1, /**< (1): Guardband State */
    cbxTsnStateHp,   /**< (2): High Priority State */
    cbxTsnStateLp,   /**< (3): Low Priority State */
} cbx_tsn_state_t;

typedef struct cbx_cosq_tsn_params_t {
    int cfg_id;          /* TSN configuration identifier 0/1 */
    int flags;           /* TSN Flags - CBX_COSQ_TSN_XX */
    int sync_source;     /* sync source can be SYNC_A or SYNC_B.
                            TSN Cycle starts on this strobe */
    int tick_period;     /* Tick period value. One tick value is 8ns */
    int start_state;     /* start state  Guard, HiPrio, LowPrio */
    int start_duration;  /* start state duration in tick period */
    int gb_duration;     /* Guard band duration in tick period */
    int hp_duration;     /* High priority duration in tick period */
    int lp_duration;     /* Low priority duration in tick period */
} cbx_cosq_tsn_params_t;
#endif

/**
 * Initialize COSQ shaper object
 *
 * @param  portid          Port Identifier
 * @param  shaper_params   Shaper parameters
 */
void cbx_cosq_shaper_params_t_init( cbx_portid_t              portid,
                                    cbx_cosq_shaper_params_t *shaper_params );


/**
 * Create a cosq
 * This routine is used to create a cosq.
 *
 * @param cosq_params   (IN)  COSQ parameters.
 * @param cosqid        (OUT) Handle of the cosq created
 *                            CBX_COSQ_INVALID: COSQ could not be created
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_cosq_create ( cbx_cosq_params_t *cosq_params,
                      cbx_cosqid_t      *cosqid );


/**
 * Destroy a cosq previously created by cbx_cosq_create()
 *
 * @param cosqid    Handle of the cosq to be destroyed
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 *
 * @note This routine destroys a previously created cosq.
 */

int cbx_cosq_destroy( cbx_cosqid_t cosqid );


/**
 * COSQ Traffic Class - port_default_tc_set
 * This routine is used to set the default ingress traffic class on a port
 *
 * @param portid   (IN)  PORT Identifier
 * @param tc       (IN)  Traffic Class structure
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_cosq_port_default_tc_set( cbx_portid_t  portid,
                                  cbx_tc_t      tc );

/**
 * COSQ Traffic Class - port_default_tc_get
 * This routine is used to get the default ingress traffic class on a port
 *
 * @param portid   (IN)  PORT Identifier
 * @param tc       (OUT) Traffic Class structure
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_cosq_port_default_tc_get( cbx_portid_t  portid,
                                  cbx_tc_t     *tc );

/**
 * COSQ Traffic Class - port_default_pcp_set
 * This routine is used to set the default egress {pcp,dei} for port.
 *
 * @param portid   (IN)  PORT Identifier
 * @param pcp      (IN)  Priority Code Point (3 bit encoding)
 * @param dei      (IN)  Drop Eligibility Indicator (1 bit encoding)
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_cosq_port_default_pcp_set( cbx_portid_t  portid,
                                   int           pcp,
                                   int           dei );

/**
 * COSQ Traffic Class - port_default_pcp_get
 * This routine is used to get the default egress {pcp,dei} on a port
 *
 * @param portid   (IN)  PORT Identifier
 * @param pcp      (OUT) Priority Code Point (3 bit encoding)
 * @param dei      (OUT) Drop Eligibility Indicator (1 bit encoding)
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_cosq_port_default_pcp_get( cbx_portid_t  portid,
                                   int          *pcp,
                                   int          *dei );

/**
 * COSQ Traffic Class - port_pcp2tc_set
 * This routine is used to set the ingress mapping of {pcp,dei} to traffic class
 *
 * @param portid   (IN)  PORT Identifier
 * @param pcp      (IN)  Priority Code Point (3 bit encoding)
 * @param dei      (IN)  Drop Eligibility Indicator (1 bit encoding)
 * @param tc       (IN)  Traffic Class structure
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_cosq_port_pcp2tc_set ( cbx_portid_t  portid,
                               int           pcp,
                               int           dei,
                               cbx_tc_t      tc );

/**
 * COSQ Traffic Class - port_pcp2tc_get
 * This routine is used to get the ingress mapping of {pcp,dei} to traffic class
 *
 * @param portid   (IN)  PORT Identifier
 * @param pcp      (IN)  Priority Code Point (3 bit encoding)
 * @param dei      (IN)  Drop Eligibility Indicator (1 bit encoding)
 * @param tc       (OUT) Traffic Class structure
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_cosq_port_pcp2tc_get ( cbx_portid_t  portid,
                               int           pcp,
                               int           dei,
                               cbx_tc_t     *tc );

/**
 * COSQ Traffic Class - port_tc2pcp_set
 * This routine is used to set the egress mapping of traffic class to {pcpi,dei}
 *
 * @param portid   (IN)  PORT Identifier
 * @param tc       (IN)  Traffic Class structure
 * @param pcp      (IN)  Priority Code Point (3 bit encoding)
 * @param dei      (IN)  Drop Eligibility Indicator (1 bit encoding)
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_cosq_port_tc2pcp_set ( cbx_portid_t  portid,
                               cbx_tc_t      tc,
                               int           pcp,
                               int           dei );

/**
 * COSQ Traffic Class - port_tc2pcp_get
 * This routine is used to get the egress mapping of traffic class to {pcp,dei}
 *
 * @param portid   (IN)  PORT Identifier
 * @param tc       (IN)  Traffic Class structure
 * @param pcp      (OUT) Priority Code Point (3 bit encoding)
 * @param dei      (OUT) Drop Eligibility Indicator (1 bit encoding)
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_cosq_port_tc2pcp_get ( cbx_portid_t  portid,
                               cbx_tc_t      tc,
                               int          *pcp,
                               int          *dei );

#if 0
int cbx_cosq_port_dscp2tc_mode_get( cbx_portid_t portid, int *mode );

int cbx_cosq_port_dscp2tc_mode_set( cbx_portid_t portid, int mode );
#endif

/**
 * COSQ Traffic Class - dscp2tc_set
 * This routine is used to set the ingress mapping of dscp to traffic class
 *
 * @param dscp     (IN)  DiffServ structure
 * @param tc       (OUT) Traffic Class structure
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_cosq_dscp2tc_set ( cbx_dscp_t dscp,
                           cbx_tc_t   tc );

/**
 * COSQ Traffic Class - dscp2tc_get
 * This routine is used to get the ingress mapping of dscp to traffic class
 *
 * @param dscp     (IN)  DiffServ structure
 * @param tc       (OUT) Traffic Class structure
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_cosq_dscp2tc_get ( cbx_dscp_t  dscp,
                           cbx_tc_t   *tc );

/**
 * COSQ Traffic Class - tc2dscp_set
 * This routine is used to set the egress mapping of traffic class to dscp
 *
 * @param dscp     (IN)  DiffServ structure
 * @param tc       (IN)  Traffic Class structure
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_cosq_tc2dscp_set ( cbx_tc_t   tc,
                           cbx_dscp_t dscp );

/**
 * COSQ Traffic Class - tc2dscp_get
 * This routine is used to get the egress mapping of traffic class to dscp
 *
 * @param dscp     (IN)  DiffServ structure
 * @param tc       (IN)  Traffic Class structure
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_cosq_tc2dscp_get ( cbx_tc_t    tc,
                           cbx_dscp_t *dscp );


/**
 * COSQ Port Parameters Get
 * This routine is used to get the COSQ parameters of a port
 *
 * @param portid        (IN)  Port Identifier
 * @param cosq_params   (OUT) COSQ parameters
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_cosq_port_get ( cbx_portid_t       portid,
                        cbx_cosq_params_t *cosq_params );

/**
 * COSQ Port Parameters Set
 * This routine is used to set the COSQ parameters of a port
 *
 * @param portid        (IN)  Port Identifier
 * @param cosq_params   (IN)  COSQ parameters
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_cosq_port_set ( cbx_portid_t      portid,
                        cbx_cosq_params_t cosq_params );


/**
 * COSQ Port Scheduler Get
 * This routine is used to get the COSQ scheduling of a port
 *
 * @param portid        (IN)  Port Identifier
 * @param sched_mode    (OUT) COSQ Scheduling mode
 * @param weights       (OUT) Array of scheduling weights
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_cosq_port_sched_get ( cbx_portid_t         portid,
                              cbx_schedule_mode_t *sched_mode,
                              int                  weights[CBX_COS_COUNT] );

/**
 * COSQ Port Scheduler Set
 * This routine is used to set the COSQ scheduling of a port
 *
 * @param portid        (IN)  Port Identifier
 * @param sched_mode    (IN)  COSQ Scheduling mode
 * @param weights       (IN)  Array of scheduling weights
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_cosq_port_sched_set ( cbx_portid_t         portid,
                              cbx_schedule_mode_t  sched_mode,
                              const int            weights[CBX_COS_COUNT] );


/**
 * Get Flow Control configuration mode
 * This routine is used to get the global flowcontrol configuration
 *
 * @param flowcontrol_mode   (OUT)  Global Flow Control mode
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_cosq_flowcontrol_get ( cbx_flowcontrol_t *flowcontrol_mode );

/**
 * Set Flow Control configuration mode
 * This routine is used to set the global flowcontrol configuration
 *
 * There are three options in Avenger:
 * 1) Source Port based pause packets
 *   programming:
 *    PGT.fcd   set all fcd 0-7 to same value to have one count
 *    thresdholds and hysteresis in BMU_fcd_config
 *    scale: 128 of these, but only 16 (one per port) are used
 *
 * 2) PFC based on traffic class - global for all port together
 *   programming:
 *    PGT.fcd   set fcd 0-7 to traffic class 0-7
 *    thresholds and hysteresis in BMU_gfcd_config
 *    scale: 8 of these

 * 3) BMU Meters to shape per port flowcontrol
 *
 * @param flowcontrol_mode   (IN)  Global Flow Control mode
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_cosq_flowcontrol_set ( cbx_flowcontrol_t flowcontrol_mode );

/**
 * Port Flow Control get
 * This routine is used to get the port flowcontrol configuration
 *
 * Flow Control parameters include an enable, 4 XOFF/Drop thresholds,
 * and a hysteresis (XON) threshold
 *
 * @param portid        (IN)  Port Identifier
 * @param fc_params     (IN)  Flow Control parameters
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_cosq_port_flowcontrol_get ( cbx_portid_t              portid,
                                    cbx_flowcontrol_params_t *fc_params);

/**
 * Port Flow Control set
 * This routine is used to set the port flowcontrol configuration
 *
 * Flow Control parameters include an enable, 4 XOFF/Drop thresholds,
 * and a hysteresis (XON) threshold
 *
 * @param portid        (IN)  Port Identifier
 * @param fc_params     (IN)  Flow Control parameters
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_cosq_port_flowcontrol_set ( cbx_portid_t              portid,
                                    cbx_flowcontrol_params_t  fc_params);

/**
 * Priority Flow Control (PFC) get
 * This routine is used to get the PFC flowcontrol configuration
 *
 * Flow Control parameters include an enable, 4 XOFF/Drop thresholds,
 * and a hysteresis (XON) threshold
 *
 * @param int_pri       (IN)  Internal traffic class priority
 * @param fc_params     (IN)  Flow Control parameters
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_cosq_pfc_flowcontrol_get ( int                       int_pri,
                                   cbx_flowcontrol_params_t *fc_params);

/**
 * Priority Flow Control (PFC) set
 * This routine is used to set the PFC flowcontrol configuration
 *
 * Flow Control parameters include an enable, 4 XOFF/Drop thresholds,
 * and a hysteresis (XON) threshold
 *
 * @param int_pri       (IN)  Internal traffic class priority
 * @param fc_params     (IN)  Flow Control parameters
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_cosq_pfc_flowcontrol_set ( int                       int_pri,
                                   cbx_flowcontrol_params_t  fc_params);


/**
 * Get a Egress port shaper configuration
 * This routine is used to get an egress shaper on a port.
 *
 * @param portid         (IN)  PORT Identifier
 * @param shaper_params  (OUT) Shaper parameters.
 *
 * @return return code
 * @retval CBX_E_NONE Success
 *         CBX_E_XXXX Failure
 */

int cbx_cosq_port_shaper_get ( cbx_portid_t              portid,
                               cbx_cosq_shaper_params_t *shaper_params);

/**
 * Set a Egress port shaper configuration
 * This routine is used to set an egress shaper on a port.
 *
 * @param portid         (IN)  PORT Identifier
 * @param shaper_params  (IN)  Shaper parameters.
 *
 * @return return code
 * @retval CBX_E_NONE Success
 *         CBX_E_XXXX Failure
 */

int cbx_cosq_port_shaper_set ( cbx_portid_t              portid,
                               cbx_cosq_shaper_params_t *shaper_params);

/**
 * Get a Egress queue shaper configuration
 * This routine is used to get an egress shaper on a qos queue.
 *
 * @param portid         (IN)  PORT Identifier
 * @param int_pri        (IN)  Internal traffic class priority
 * @param shaper_params  (OUT) Shaper parameters.
 *
 * @return return code
 * @retval CBX_E_NONE Success
 *         CBX_E_XXXX Failure
 */

int cbx_cosq_queue_shaper_get ( cbx_portid_t              portid,
                                int                       int_pri,
                                cbx_cosq_shaper_params_t *shaper_params);

/**
 * Set a Egress queue shaper configuration
 * This routine is used to set an egress shaper on a qos queue.
 *
 * @param portid         (IN)  PORT Identifier
 * @param int_pri        (IN)  Internal traffic class priority
 * @param shaper_params  (IN)  Shaper parameters.
 *
 * @return return code
 * @retval CBX_E_NONE Success
 *         CBX_E_XXXX Failure
 */

int cbx_cosq_queue_shaper_set ( cbx_portid_t              portid,
                                int                       int_pri,
                                cbx_cosq_shaper_params_t *shaper_params);

#ifdef CONFIG_TIMESYNC
/**
 * Initialize COSQ TSN object
 *
 * @param  tsn_params TSN parameters
 */
void cbx_cosq_tsn_params_t_init(cbx_cosq_tsn_params_t *tsn_params);

/**
 * Get a Egress TSN  configuration
 * This routine is used to get an egress TSN parameters on a qos queue.
 *
 * @param portid         (IN)  PORT Identifier
 * @param int_pri        (IN)  Internal traffic class priority
 * @param tsn_params     (OUT) TSN parameters.
 *
 * @return return code
 * @retval CBX_E_NONE Success
 *         CBX_E_XXXX Failure
 */
int cbx_cosq_queue_tsn_config_get(cbx_portid_t portid,
                                  int int_pri,
                                  cbx_cosq_tsn_params_t *tsn_params);

/**
 * Set a Egress queue shaper configuration
 * This routine is used to set an egress TSN parameters on a qos queue.
 *
 * @param portid         (IN)  PORT Identifier
 * @param int_pri        (IN)  Internal traffic class priority
 * @param tsn_params     (IN)  TSN parameters.
 *
 * @return return code
 * @retval CBX_E_NONE Success
 *         CBX_E_XXXX Failure
 */
int cbx_cosq_queue_tsn_config_set(cbx_portid_t portid,
                                  int int_pri,
                                  cbx_cosq_tsn_params_t *tsn_params);
#endif

/**
 * @}
 */


#ifdef __cplusplus
}
#endif

#endif
