/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 */

/**
 * @file
 * Coronado Bridge
 * Forwarding Services Abstraction Layer (FSAL) API
 */

#ifndef CBX_API_STAT_H_
#define CBX_API_STAT_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @defgroup cbx_stat CBX STAT
 * Forwarding Services Abstraction Layer (FSAL) Statistics operations.
 *
 *  @{
 */

/**
 * @typedef cbx_stat_t
 * CBX stat
 */

/**
 * @def CBX_STAT_INVALID
 * Invalid STAT
 */
#define CBX_STAT_INVALID   0xffffffff

/** @def CBX_STAT_MAX
 * Maximum STAT entries
 */
#define CBX_STAT_MAX  -1

/**
 *  Port statistics
 */
typedef enum cbx_port_stat_counter_e {

    /* RFC 1213 */
    CBX_PORT_STAT_IF_IN_OCTETS,
    CBX_PORT_STAT_IF_IN_UCAST_PKTS,
    CBX_PORT_STAT_IF_IN_NON_UCAST_PKTS,
    CBX_PORT_STAT_IF_IN_DISCARDS,
    CBX_PORT_STAT_IF_IN_ERRORS,
    CBX_PORT_STAT_IF_IN_UNKNOWN_PROTOS,
    CBX_PORT_STAT_IF_IN_BROADCAST_PKTS,
    CBX_PORT_STAT_IF_IN_MULTICAST_PKTS,
    CBX_PORT_STAT_IF_IN_VLAN_DISCARDS,
    CBX_PORT_STAT_IF_OUT_OCTETS,
    CBX_PORT_STAT_IF_OUT_UCAST_PKTS,
    CBX_PORT_STAT_IF_OUT_NON_UCAST_PKTS,
    CBX_PORT_STAT_IF_OUT_DISCARDS,
    CBX_PORT_STAT_IF_OUT_ERRORS,
    CBX_PORT_STAT_IF_OUT_QLEN,
    CBX_PORT_STAT_IF_OUT_BROADCAST_PKTS,
    CBX_PORT_STAT_IF_OUT_MULTICAST_PKTS,

    /* RFC 2819 */
    CBX_PORT_STAT_ETHER_STATS_DROP_EVENTS,
    CBX_PORT_STAT_ETHER_STATS_MULTICAST_PKTS,
    CBX_PORT_STAT_ETHER_STATS_BROADCAST_PKTS,
    CBX_PORT_STAT_ETHER_STATS_UNDERSIZE_PKTS,
    CBX_PORT_STAT_ETHER_STATS_FRAGMENTS,
    CBX_PORT_STAT_ETHER_STATS_PKTS_64_OCTETS,
    CBX_PORT_STAT_ETHER_STATS_PKTS_65_TO_127_OCTETS,
    CBX_PORT_STAT_ETHER_STATS_PKTS_128_TO_255_OCTETS,
    CBX_PORT_STAT_ETHER_STATS_PKTS_256_TO_511_OCTETS,
    CBX_PORT_STAT_ETHER_STATS_PKTS_512_TO_1023_OCTETS,
    CBX_PORT_STAT_ETHER_STATS_PKTS_1024_TO_1522_OCTETS,
    CBX_PORT_STAT_ETHER_STATS_PKTS_1523_TO_2047_OCTETS,
    CBX_PORT_STAT_ETHER_STATS_PKTS_2048_TO_4095_OCTETS,
    CBX_PORT_STAT_ETHER_STATS_PKTS_4096_TO_8191_OCTETS,
    CBX_PORT_STAT_ETHER_STATS_PKTS_8192_TO_MAX_OCTETS,
    CBX_PORT_STAT_ETHER_STATS_OVERSIZE_PKTS,
    CBX_PORT_STAT_ETHER_RX_OVERSIZE_PKTS,
    CBX_PORT_STAT_ETHER_TX_OVERSIZE_PKTS,
    CBX_PORT_STAT_ETHER_STATS_JABBERS,
    CBX_PORT_STAT_ETHER_STATS_OCTETS,
    CBX_PORT_STAT_ETHER_STATS_PKTS,
    CBX_PORT_STAT_ETHER_STATS_COLLISIONS,
    CBX_PORT_STAT_ETHER_STATS_CRC_ALIGN_ERRORS,
    CBX_PORT_STAT_ETHER_STATS_TX_NO_ERRORS,
    CBX_PORT_STAT_ETHER_STATS_RX_NO_ERRORS,

    /* RFC 2233 */
    CBX_PORT_STAT_IP_IN_RECEIVES,
    CBX_PORT_STAT_IP_IN_OCTETS,
    CBX_PORT_STAT_IP_IN_UCAST_PKTS,
    CBX_PORT_STAT_IP_IN_NON_UCAST_PKTS,
    CBX_PORT_STAT_IP_IN_DISCARDS,
    CBX_PORT_STAT_IP_OUT_OCTETS,
    CBX_PORT_STAT_IP_OUT_UCAST_PKTS,
    CBX_PORT_STAT_IP_OUT_NON_UCAST_PKTS,
    CBX_PORT_STAT_IP_OUT_DISCARDS,

    /* RFC 2465 */
    CBX_PORT_STAT_IPV6_IN_RECEIVES,
    CBX_PORT_STAT_IPV6_IN_OCTETS,
    CBX_PORT_STAT_IPV6_IN_UCAST_PKTS,
    CBX_PORT_STAT_IPV6_IN_NON_UCAST_PKTS,
    CBX_PORT_STAT_IPV6_IN_MCAST_PKTS,
    CBX_PORT_STAT_IPV6_IN_DISCARDS,
    CBX_PORT_STAT_IPV6_OUT_OCTETS,
    CBX_PORT_STAT_IPV6_OUT_UCAST_PKTS,
    CBX_PORT_STAT_IPV6_OUT_NON_UCAST_PKTS,
    CBX_PORT_STAT_IPV6_OUT_MCAST_PKTS,
    CBX_PORT_STAT_IPV6_OUT_DISCARDS,

    CBX_PORT_STAT_GREEN_DISCARD_DROPPED_PACKETS,
    CBX_PORT_STAT_GREEN_DISCARD_DROPPED_BYTES,
    CBX_PORT_STAT_YELLOW_DISCARD_DROPPED_PACKETS,
    CBX_PORT_STAT_YELLOW_DISCARD_DROPPED_BYTES,
    CBX_PORT_STAT_RED_DISCARD_DROPPED_PACKETS,
    CBX_PORT_STAT_RED_DISCARD_DROPPED_BYTES,
    CBX_PORT_STAT_DISCARD_DROPPED_PACKETS,
    CBX_PORT_STAT_DISCARD_DROPPED_BYTES,

    CBX_PORT_STAT_ETHER_IN_PKTS_64_OCTETS,
    CBX_PORT_STAT_ETHER_IN_PKTS_65_TO_127_OCTETS,
    CBX_PORT_STAT_ETHER_IN_PKTS_128_TO_255_OCTETS,
    CBX_PORT_STAT_ETHER_IN_PKTS_256_TO_511_OCTETS,
    CBX_PORT_STAT_ETHER_IN_PKTS_512_TO_1023_OCTETS,
    CBX_PORT_STAT_ETHER_IN_PKTS_1024_TO_1522_OCTETS,
    CBX_PORT_STAT_ETHER_IN_PKTS_1523_TO_2047_OCTETS,
    CBX_PORT_STAT_ETHER_IN_PKTS_2048_TO_4095_OCTETS,
    CBX_PORT_STAT_ETHER_IN_PKTS_4096_TO_8191_OCTETS,
    CBX_PORT_STAT_ETHER_IN_PKTS_8192_TO_MAX_OCTETS,

    CBX_PORT_STAT_ETHER_OUT_PKTS_64_OCTETS,
    CBX_PORT_STAT_ETHER_OUT_PKTS_65_TO_127_OCTETS,
    CBX_PORT_STAT_ETHER_OUT_PKTS_128_TO_255_OCTETS,
    CBX_PORT_STAT_ETHER_OUT_PKTS_256_TO_511_OCTETS,
    CBX_PORT_STAT_ETHER_OUT_PKTS_512_TO_1023_OCTETS,
    CBX_PORT_STAT_ETHER_OUT_PKTS_1024_TO_1522_OCTETS,
    CBX_PORT_STAT_ETHER_OUT_PKTS_1523_TO_2047_OCTETS,
    CBX_PORT_STAT_ETHER_OUT_PKTS_2048_TO_4095_OCTETS,
    CBX_PORT_STAT_ETHER_OUT_PKTS_4096_TO_8191_OCTETS,
    CBX_PORT_STAT_ETHER_OUT_PKTS_8192_TO_MAX_OCTETS,
} cbx_port_stat_counter_t;

/**
 *  VLAN statistics
 */
typedef enum cbx_vlan_stat_counter_e {
    CBX_VLAN_STAT_IN_OCTETS,
    CBX_VLAN_STAT_IN_UCAST_PKTS,
    CBX_VLAN_STAT_IN_NON_UCAST_PKTS,
    CBX_VLAN_STAT_IN_DISCARDS,
    CBX_VLAN_STAT_IN_ERRORS,
    CBX_VLAN_STAT_IN_UNKNOWN_PROTOS,
    CBX_VLAN_STAT_OUT_OCTETS,
    CBX_VLAN_STAT_OUT_UCAST_PKTS,
    CBX_VLAN_STAT_OUT_NON_UCAST_PKTS,
    CBX_VLAN_STAT_OUT_DISCARDS,
    CBX_VLAN_STAT_OUT_ERRORS,
    CBX_VLAN_STAT_OUT_QLEN
} cbx_vlan_stat_counter_t;


/**
 *  CosQ statistics
 */
typedef enum cbx_cosq_stat_counter_e {
    CBX_COSQ_STAT_PACKETS,
    CBX_COSQ_STAT_BYTES,
    CBX_COSQ_STAT_DROPPED_PACKETS,
    CBX_COSQ_STAT_DROPPED_BYTES,
    CBX_COSQ_STAT_GREEN_PACKETS,
    CBX_COSQ_STAT_GREEN_BYTES,
    CBX_COSQ_STAT_GREEN_DROPPED_PACKETS,
    CBX_COSQ_STAT_GREEN_DROPPED_BYTES,
    CBX_COSQ_STAT_YELLOW_PACKETS,
    CBX_COSQ_STAT_YELLOW_BYTES,
    CBX_COSQ_STAT_YELLOW_DROPPED_PACKETS,
    CBX_COSQ_STAT_YELLOW_DROPPED_BYTES,
    CBX_COSQ_STAT_RED_PACKETS,
    CBX_COSQ_STAT_RED_BYTES,
    CBX_COSQ_STAT_RED_DROPPED_PACKETS,
    CBX_COSQ_STAT_RED_DROPPED_BYTES,
    CBX_COSQ_STAT_GREEN_DISCARD_DROPPED_PACKETS,
    CBX_COSQ_STAT_GREEN_DISCARD_DROPPED_BYTES,
    CBX_COSQ_STAT_YELLOW_DISCARD_DROPPED_PACKETS,
    CBX_COSQ_STAT_YELLOW_DISCARD_DROPPED_BYTES,
    CBX_COSQ_STAT_RED_DISCARD_DROPPED_PACKETS,
    CBX_COSQ_STAT_RED_DISCARD_DROPPED_BYTES,
    CBX_COSQ_STAT_DISCARD_DROPPED_PACKETS,
    CBX_COSQ_STAT_DISCARD_DROPPED_BYTES
} cbx_cosq_stat_counter_t;

/**
 *  Meter statistics
 */
typedef enum cbx_meter_stat_counter_e {
    CBX_METER_STAT_PACKETS,
    CBX_METER_STAT_ATTR_BYTES,
    CBX_METER_STAT_GREEN_PACKETS,
    CBX_METER_STAT_GREEN_BYTES,
    CBX_METER_STAT_YELLOW_PACKETS,
    CBX_METER_STAT_YELLOW_BYTES,
    CBX_METER_STAT_RED_PACKETS,
    CBX_METER_STAT_RED_BYTES
} cbx_meter_stat_counter_t;


/**
 * STAT information struct
 * information about a statistics entry
 */
typedef struct cbx_stat_params_t {
} cbx_stat_params_t;


/**
 * Fowarding Counters index
 */
typedef enum Forward_Counter_e {
    FWD_OK,          /**< (0): number of frames regularly forwarded. */
    INVALID_SLIID,   /**< (1): number of frames discarded because the
                               SLIID was determined to be invalid. */
    INVALID_VSI_PDU, /**< (2): number of frames discarded because the
                               frames PDU did not match that configured for the VSI. */
    DROP_ON_ULF,     /**< (3): number of frames discarded because of
                               unicast lookup failure. */
    DROP_ON_MLF,     /**< (4): number of frames discarded because of
                               multicast lookup failure. */
    DROP_ON_SLF,     /**< (5): number of frames discarded because of
                               source lookup failure. */
    DST_DENIED,      /**< (6): number of frames discarded because the
                               DMAC was configured to deny. */
    SRC_DENIED       /**< (7): number of frames discarded because the
                               SMAC was configured to deny. */
} Forward_Counter_t;


// Address Resolution Logic Counters

/**
 * ARL_Entry Counters (ARL_ENTRY_COUNT_a[17])
 * This 17 entry direct access table. Each counter is 32 bits.
 * ARL_ENTRY_N_TYPE: Entry Counters are indexed by port number but this additional index
 * provides access to the entry count maintained for all dynamic ARL entries
 * that utilize N-type DLIs.
 *
 * Counter consists of a single field:
 * count[13:0] - Current count value.
 *               This is a read-only field.
 *               When read the counter is automatically cleared.
 *               The count value saturates at its maximum positive value.
 *               count defaults to 0.
 */
typedef enum ARL_Entry_Counter_e {
    ARL_ENTRY_PG0,            /**<  (0):  */
    ARL_ENTRY_PG1,            /**<  (1):  */
    ARL_ENTRY_PG2,            /**<  (2):  */
    ARL_ENTRY_PG3,            /**<  (3):  */
    ARL_ENTRY_PG4,            /**<  (4):  */
    ARL_ENTRY_PG5,            /**<  (5):  */
    ARL_ENTRY_PG6,            /**<  (6):  */
    ARL_ENTRY_PG7,            /**<  (7):  */
    ARL_ENTRY_PG8,            /**<  (8):  */
    ARL_ENTRY_PG9,            /**<  (9):  */
    ARL_ENTRY_PG10,           /**<  (10): */
    ARL_ENTRY_PG11,           /**<  (11): */
    ARL_ENTRY_PG12,           /**<  (12): */
    ARL_ENTRY_PG13,           /**<  (13): */
    ARL_ENTRY_PG14,           /**<  (14): */
    ARL_ENTRY_PG15,           /**<  (15): */
    ARL_ENTRY_N_TYPE          /**<  (16): */
} ARL_Entry_Counter_t;

/**
 * ARL_Update Counters (ARL_UPDATE_COUNT_a[4])
 * This 4 entry direct access table.  Each counter is 32 bits.
 *
 * Counter consists of a single field:
 * count[31:0] - Current count value.
 *               This is a read-only field.
 *               When read the counter is automatically cleared.
 *               The count value saturates at its maximum positive value.
 *               count defaults to 0.
 *
 */
typedef enum ARL_Update_Counter_e {
    ARL_UPDATE_OK,                /**< (0): number of ARL updates executed
                (recorded station movement events). */
    ARL_UPDATE_DISABLED,          /**< (1): number of ARL updates not executed because
                updates were disabled on a port. */
    ARL_UPDATE_STATIC_DISABLED,   /**< (2): number of ARL updates not executed because
                updates were disabled on a port for static ARL entries. */
    ARL_UPDATE_PENDING_DISABLED,  /**< (3): number of ARLupdates not executed because
                updates were disabled on a port for pending ARL entries. */
} ARL_Update_Counter_t;

/**
 * ARL_Submit Counters (ARL_SUBMIT_COUNT_a[4])
 * This 4 entry direct access table. Each counter is 32 bits.
 * Counter consists of a single field:
 * count[31:0] - Current count value.
 *               This is a read-only field.
 *               When read the counter is automatically cleared.
 *               The count value saturates at its maximum positive value.
 *               count defaults to 0.
 *
 */
typedef enum ARL_Submit_Counter_e {
    ARL_SUBMIT_OK,          /**< (0): number of ARL submits executed
            (note that this includes successes and failures). */
    ARL_SUBMIT_DISABLED,    /**< (1): number of ARL submits not executed because
            learns were disabled on a port. */
    ARL_SUBMIT_OVERLIMIT,   /**< (2): number of ARL submits not executed because
            the port exceeded its learn limit. */
    ARL_SUBMIT_QUEUE_FULL,  /**< (3): number of ARL submits not executed because
            the submit queue was full. */
} ARL_Submit_Counter_t;

/**
 * ARL_Learn Counters (ARL_LEARN_COUNT_a[2])
 * This 2 entry direct access table. Each counter is 32 bits.
 * Counter consists of a single field:
 * count[31:0] - Current count value.
 *               This is a read-only field.
 *               When read the counter is automatically cleared.
 *               The count value saturates at its maximum positive value.
 *               count defaults to 0.
 *
 */
typedef enum ARL_Learn_Counter_e {
    ARL_LEARN_OK,        /**< (0): number of successfull ARL learn events. */
    ARL_LEARN_NOT_OK,    /**< (1): number of unsuccessfull ARL learn events. */
} ARL_Learn_Counter_t;


/**
 * STAT init
 * This routine is used to initialize the STAT module on the system.
 *
 * @param void
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_stat_init ( void );

/**
 * STAT reset
 * This routine is used to reset all the STAT entries in the system.
 *
 * @param void
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_stat_reset ( void );

/**
 * STAT port get
 * This routine is used to read a STAT entries on a port.
 *
 * @param portid     (IN)  Port Identifier
 * @param type       (IN)  Stat type
 * @param value      (OUT) Stat value
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_port_stat_get( cbx_portid_t             portid,
                       cbx_port_stat_counter_t  type,
                       uint64_t                *value );

/**
 * STAT port multi_get
 * This routine is used to read multiple STAT entries on a port.
 *
 * @param portid           (IN)  Port Identifier
 * @param number_of_stats  (IN)  Number of valid types in the array
 * @param type_array       Type array
 * @param value_array      Value array
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_port_stat_mult_get (cbx_portid_t             portid,
                            int                      number_of_stats,
                            cbx_port_stat_counter_t *type_array,
                            uint64_t                *value_array);


/**
 * STAT port clear
 * This routine is used to clear the STAT entries on a port.
 *
 * @param portid      Port Identifier
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_port_stat_clear( cbx_portid_t portid );


/**
 * STAT vlan get
 * This routine is used to read a STAT entries on a vlan.
 *
 * @param vlanid     (IN)  Vlan Identifier
 * @param type       (IN)  Stat type
 * @param value      (OUT) Stat value
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_vlan_stat_get( cbx_vlanid_t             vlanid,
                       cbx_vlan_stat_counter_t  type,
                       uint64_t                *value );

/**
 * STAT vlan multi_get
 * This routine is used to read multiple STAT entries on a port.
 *
 * @param vlanid           (IN)  Vlan Identifier
 * @param number_of_stats  (IN)  Number of valid types in the array
 * @param type_array       Type array
 * @param value_array      Value array
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_vlan_stat_mult_get (cbx_vlanid_t             vlanid,
                            int                      number_of_stats,
                            cbx_vlan_stat_counter_t *type_array,
                            uint64_t                *value_array);

/**
 * STAT vlan clear
 * This routine is used to clear the STAT entries on a vlan.
 *
 * @param vlanid      Vlan Identifier
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_vlan_stat_clear( cbx_vlanid_t vlanid );


/**
 * STAT cosq get
 * This routine is used to read a STAT entries on a cosq.
 *
 * @param cosqid     (IN)  Cosq Identifier
 * @param type       (IN)  Stat type
 * @param value      (OUT) Stat value
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_cosq_stat_get( cbx_cosqid_t             cosqid,
                       cbx_cosq_stat_counter_t  type,
                       uint64_t                *value );

/**
 * STAT cosq multi_get
 * This routine is used to read multiple STAT entries on a cosq.
 *
 * @param cosqid           (IN)  Cosq Identifier
 * @param number_of_stats  (IN)  Number of valid types in the array
 * @param type_array       Type array
 * @param value_array      Value array
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_cosq_stat_mult_get (cbx_cosqid_t             cosqid,
                            int                      number_of_stats,
                            cbx_cosq_stat_counter_t *type_array,
                            uint64_t                *value_array);

/**
 * STAT cosq clear
 * This routine is used to clear the STAT entries on a cosq.
 *
 * @param cosqid      (IN)  Cosq Identifier
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_cosq_stat_clear( cbx_cosqid_t cosqid );


/**
 * STAT meter get
 * This routine is used to read a STAT entries on a meter.
 *
 * @param meterid    (IN)  Meter Identifier
 * @param type       (IN)  Stat type
 * @param value      (OUT) Stat value
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_meter_stat_get( cbx_meterid_t             meterid,
                        cbx_meter_stat_counter_t  type,
                        uint64_t                 *value );

/**
 * STAT meter multi_get
 * This routine is used to read multiple STAT entries on a meter.
 *
 * @param meterid          (IN)  Meter Identifier
 * @param number_of_stats  (IN)  Number of valid types in the array
 * @param type_array       Type array
 * @param value_array      Value array
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_meter_stat_mult_get (cbx_meterid_t             meterid,
                             int                       number_of_stats,
                             cbx_meter_stat_counter_t *type_array,
                             uint64_t                 *value_array);

/**
 * STAT meter clear
 * This routine is used to clear the STAT entries on a meter.
 *
 * @param meterid     (IN)  Meter Identifier
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_meter_stat_clear( cbx_meterid_t meterid );



#if 0
int cbx_stat_snapshot_mode_get( cbx_snapshot_mode_t *mode );

int cbx_stat_snapshot_mode_set( cbx_snapshot_mode_t mode );

int cbx_stat_snapshot_start( cbx_port_t port );
#endif


/**
 * @}
 */


#ifdef __cplusplus
}
#endif

#endif
