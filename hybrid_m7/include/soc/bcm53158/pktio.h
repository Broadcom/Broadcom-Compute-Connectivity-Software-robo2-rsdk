/**
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 */

/**
 * @file
 * Coronado Bridge
 * Forwarding Services Abstraction Layer (FSAL) API
 * CBX PKTIO
 */

#ifndef CBX_API_PKTIO_H_
#define CBX_API_PKTIO_H_

#ifdef __cplusplus
extern "C" {
#endif


#define CBXI_NT_CP2SW 23
#define CBXI_NT_SW2CP 21

#define CBX_STT_INDEX_CP_TO_SWITCH 0
#define CBX_STT_INDEX_SWITCH_TO_CP 1
#define CBXI_RX_BUFFER_SIZE_IN_256B       1
#define CBXI_RX_BUFFER_SIZE               (CBXI_RX_BUFFER_SIZE_IN_256B * 256)
#define CBXI_TX_BUFFER_SIZE_IN_256B       1
#define CBXI_TX_BUFFER_SIZE               (CBXI_TX_BUFFER_SIZE_IN_256B * 256)

/* DMA Control */
#define HPA_DMA_CONTROL_TX_EN          0x00000001
#define HPA_DMA_CONTROL_RX_EN          0x00000010
#define HPA_DMA_CONTROL_TX_RESET       0x00000100
#define HPA_DMA_CONTROL_RX_RESET       0x00001000
#define HPA_DMA_CONTROL_LE_EN          0x00010000

#define HPA_RX         0
#define HPA_TX         1

#define HPA_OWN_CPU    0x0
#define HPA_OWN_HPA    0x1

/* Intr defs */
#define HPA_INTR_TX_DESC_DONE    0x00001
#define HPA_INTR_TX_DESC_EMPTY   0x00002
#define HPA_INTR_TX_DESC_AXIERR  0x00004
#define HPA_INTR_RX_DESC_DONE    0x10000
#define HPA_INTR_RX_DESC_EMPTY   0x20000
#define HPA_INTR_RX_DESC_AXIERR  0x40000

#define HPA_MAX_HW_RX_DESCR  8
#define HPA_MAX_HW_TX_DESCR  8
#define HPA_MAX_SW_TX_DESCR  16
#define HPA_MIN_TX_SIZE   64



#define RX_BUFFER_START    0xA000
#define RX_BUFFER_0        RX_BUFFER_START
#define RX_BUFFER_1        (RX_BUFFER_START + 1 * (CBXI_RX_BUFFER_SIZE))
#define RX_BUFFER_2        (RX_BUFFER_START + 2 * (CBXI_RX_BUFFER_SIZE))
#define RX_BUFFER_3        (RX_BUFFER_START + 3 * (CBXI_RX_BUFFER_SIZE))
#define RX_BUFFER_4        (RX_BUFFER_START + 4 * (CBXI_RX_BUFFER_SIZE))
#define RX_BUFFER_5        (RX_BUFFER_START + 5 * (CBXI_RX_BUFFER_SIZE))
#define RX_BUFFER_6        (RX_BUFFER_START + 6 * (CBXI_RX_BUFFER_SIZE))
#define RX_BUFFER_7        (RX_BUFFER_START + 7 * (CBXI_RX_BUFFER_SIZE))
#define TX_BUFFER_START    (RX_BUFFER_START + 8 * (CBXI_RX_BUFFER_SIZE))
#define TX_BUFFER_0        TX_BUFFER_START
#define TX_BUFFER_1        (TX_BUFFER_START + 1 * (CBXI_TX_BUFFER_SIZE))
#define TX_BUFFER_2        (TX_BUFFER_START + 2 * (CBXI_TX_BUFFER_SIZE))
#define TX_BUFFER_3        (TX_BUFFER_START + 3 * (CBXI_TX_BUFFER_SIZE))
#define TX_BUFFER_4        (TX_BUFFER_START + 4 * (CBXI_TX_BUFFER_SIZE))
#define TX_BUFFER_5        (TX_BUFFER_START + 5 * (CBXI_TX_BUFFER_SIZE))
#define TX_BUFFER_6        (TX_BUFFER_START + 6 * (CBXI_TX_BUFFER_SIZE))
#define TX_BUFFER_7        (TX_BUFFER_START + 7 * (CBXI_TX_BUFFER_SIZE))

#define CBXI_PKT_FLAGS_RX_VALID    0x0001     /* Pkt received ok  */
#define CBXI_PKT_FLAGS_RX_ERROR    0x0002     /* Pkt received with error */
#define CBXI_PKT_FLAGS_RX_SOP      0x0004     /* Pkt buffer has only SOP */
#define CBXI_PKT_FLAGS_RX_EOP      0x0008     /* Pkt buffer has EOP */
#define CBXI_PKT_FLAGS_RX_MID      0x0010     /* Pkt bufffer is part of chained */




/* DMA STATUS */
#define HPA_DMA_STATUS_STOPPED            0
#define HPA_DMA_STATUS_WAIT_FOR_DESCR     1
#define HPA_DMA_STATUS_AXI_REQUEST        2
#define HPA_DMA_STATUS_AXI_DATA_XFER      3
#define HPA_DMA_STATUS_AXI_RESPONSE       4
#define HPA_DMA_STATUS_AXI_ERR_RECOVERY   5
#define HPA_DMA_STATUS_CLOSE_DESCR        6
#define HPA_DMA_STATUS_INCR_DESCR_PTR     7



#define CBX_PORT_ICPU  15
#define CBX_STT_ETYPE_CP_TO_SWITCH    0xAAB1
#define CBX_STT_ETYPE_SWITCH_TO_CP    0xAAB2
#define CBX_STT_ETYPE_BRCM_TAG        0xAAAA
#define CBX_STT_ETYPE_SWITCH_TO_CP_TS 0xBBCC



/** @defgroup cbx_pktio CBX PKTIO
 * Forwarding Services Abstraction Layer (FSAL) Packet_IO operations.
 *
 *  @{
 */

/**
 *  Packet_IO parameter structure
 */
typedef struct cbx_pktio_params_t {
    uint32         flags;            /**< Flags:  CBX_PKTIO_XX */
} cbx_pktio_params_t;

/** PACKET Flags */
#define  CBX_PACKET_RX_UCAST       0x00000001   /**< Unicast packet */
#define  CBX_PACKET_RX_MCAST       0x00000002   /**< Multicast packet */
#define  CBX_PACKET_RX_DLF         0x00000004   /**< Destination Lookup Failure packet */
#define  CBX_PACKET_RX_LEARN       0x00000008   /**< Learning packet */
#define  CBX_PACKET_RX_MIRROR      0x00000010   /**< Mirror packet */
#define  CBX_PACKET_RX_TRAP        0x00000020   /**< Trap packet */
#define  CBX_PACKET_RX_LIN         0x00000040   /**< Source Interface is logical port */
#define  CBX_PACKET_TX_FWD_RAW     0x00010000   /**< Raw Forwarding (no checks) */
#define  CBX_PACKET_TX_FWD_UCAST   0x00020000   /**< Unicast Directed Forwarding */
#define  CBX_PACKET_TX_FWD_MCAST   0x00040000   /**< Multicast Directed Forwarding */
#define  CBX_PACKET_TX_FWD_SWITCH  0x00080000   /**< Switch Forwarding */

/**
 *  Packet attribute structure
 */

typedef uint16 cbx_vlanid_t;
typedef uint8 cbx_portid_t;
typedef uint16 cbx_mcastid_t;
typedef uint8 cbx_trapid_t;

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


typedef struct cbx_packet_attr_t {
    uint32_t         flags;            /**< Flags:  CBX_PACKET_XX */
    cbx_vlanid_t     vlanid;           /**< Vlan ID on which the packet was received */
    cbx_tc_t         tc;               /**< traffic class priority and drop precedence */
    int              source_index;     /**< port index */
    int              dest_index;       /**< port index */
    cbx_portid_t     source_port;      /**< The port on which the packet arrived */
    cbx_portid_t     dest_port;        /**< RX: The port to which the packet was mapped,
                                            TX: The destination port */
    cbx_mcastid_t    mcastid;          /**< Destination multicast group */
    cbx_trapid_t     trapid;           /**< Trap ID */
    uint32_t         trap_data;        /**< additional trap information */
} cbx_packet_attr_t;

/**
 * Initialize Packet attribute parameters.
 *
 * @param  packet_attr    attribute object
 */
void cbx_packet_attr_t_init( cbx_packet_attr_t *packet_attr );


/** Pktio Callback Flags */
#define  CBX_PKTIO_CALLBACK_ALL         0x00000001   /**< All packets, no filtering */
#define  CBX_PKTIO_CALLBACK_TRAP        0x00000002   /**< include trapped packets */
#define  CBX_PKTIO_CALLBACK_LLC         0x00000004   /**< include LLC packets */
#define  CBX_PKTIO_CALLBACK_TCP         0x00000008   /**< include TCP packets */
#define  CBX_PKTIO_CALLBACK_UDP         0x00000010   /**< include UDP packets */
#define  CBX_PKTIO_CALLBACK_IP          0x00000020   /**< include IP packets */
#define  CBX_PKTIO_CALLBACK_LLDP        0x00000040   /**< include LLDP etype=0x88CC packets */
#define  CBX_PKTIO_CALLBACK_ECP         0x00000080   /**< include ECP etype=0x8940 packets */

/**
 *  Packet_IO callback information structure
 */
typedef struct cbx_packet_cb_info_t {
    uint32         flags;            /**< Flags:  CBX_PKTIO_CALLBACK_XX */
    const char      *name;             /**< Registered callback name string */
    uint32         priority;         /**< Callback priority */
    void            *cookie;           /**< Application data passed on registration */
} cbx_packet_cb_info_t;

/** Pktio Callback return codes */
#define CBX_RX_NOT_HANDLED  0      /**< Client unprocessed receive callback */
#define CBX_RX_HANDLED      1      /**< Client handled packet on receive callback */


/**
 * @}
 */


#ifdef __cplusplus
}
#endif

#endif
