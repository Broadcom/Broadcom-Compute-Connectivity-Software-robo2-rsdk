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

/** @defgroup cbx_pktio CBX PKTIO
 * Forwarding Services Abstraction Layer (FSAL) Packet_IO operations.
 *
 *  @{
 */

/**
 *  Packet_IO parameter structure
 */
typedef struct cbx_pktio_params_t {
    uint32_t         flags;            /**< Flags:  CBX_PKTIO_XX */
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
    uint32_t         ing_ptp_ts;       /**< Ingress PTP timestamp */
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
    uint32_t         flags;            /**< Flags:  CBX_PKTIO_CALLBACK_XX */
    const char      *name;             /**< Registered callback name string */
    uint32_t         priority;         /**< Callback priority */
    void            *cookie;           /**< Application data passed on registration */
} cbx_packet_cb_info_t;

/** Pktio Callback return codes */
#define CBX_RX_NOT_HANDLED  0      /**< Client unprocessed receive callback */
#define CBX_RX_HANDLED      1      /**< Client handled packet on receive callback */

/**
 * Packet IO Callback prototype
 *
 * Function prototype definition for user supplied Packet IO callback.
 * Callback is performed in the context of the packet_io thread.
 *
 * @param  buffer       (IN)  packet buffer
 * @param  buffer_size  (IN)  buffer size
 * @param  packet_attr  (IN)  packet attributes
 * @param  cb_info      (IN/OUT) application information
 *
 * @return return code
 * @retval CBX_RX_NOT_HANDLED  packet is not handled, continue processing
 * @retval CBX_RX_HANDLED      packet is handled and processing is complete
 */
typedef int ( *cbx_packet_io_callback_t )
( void                 *buffer,
  int                   buffer_size,
  cbx_packet_attr_t    *packet_attr,
  cbx_packet_cb_info_t *cb_info );

/**
 * PKTIO Rx callback handle
 *
 * Handle that identifies the rx callback context. This handle needs to be
 * passed to cbx_pktio_unregister() to disable rx callback registered with
 * cbx_pktio_register() previously
 *
 */
typedef struct cbxi_callback_s *cbx_pktio_rx_cb_handle;

/**
 * Initialize Packet_IO module
 * This routine is used to init pktio.
 *
 * @param pktio_params   (IN)  Pktio parameters.
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_pktio_init ( cbx_pktio_params_t *pktio_params );

/**
 * Detach a Packet_IO previously initialized by cbx_pktio_init()
 *
 * @param pktio_params   (IN)  Pktio parameters.
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 *
 * @note This routine destroys a previously initialized pktio.
 */

int cbx_pktio_detach ( cbx_pktio_params_t *pktio_params );


/**
 * Packet IO receive callback register
 * This routine is used to register a Packet IO callback routine.
 * Register a handler invoked packet receiption.
 *
 * Packets which match the cb_info.flags filter are presented.
 *
 * If multiple handlers are registered, they are invoked on
 * packet reception ordered based on cb_info.priority (lowest first)
 * and clients with the same priority order is not specified.
 *
 * @param cb_info      (IN)  Callback information
 * @param cb           (IN)  Callback function pointer
 *                           see: cbx_packet_io_callback_t
 * @param handle       (OUT) Handle to callback context
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_pktio_register ( cbx_packet_cb_info_t      cb_info,
                         cbx_packet_io_callback_t  cb,
                         cbx_pktio_rx_cb_handle    *handle);


/**
 * Packet IO receive callback unregister
 * This routine is used to unregister a Packet IO callback routine.
 *
 * @param cb           (IN)  Packet IO callback handle
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_pktio_unregister ( cbx_pktio_rx_cb_handle   handle );


/**
 * Packet_IO send function
 *
 * @param  buffer       (IN)  packet buffer
 * @param  buffer_size  (IN)  packet size in bytes
 * @param  packet_attr  (IN)  packet attributess
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int  cbx_pktio_packet_send ( void              *buffer,
                             int                buffer_size,
                             cbx_packet_attr_t *packet_attr );


/**
 * @}
 */


#ifdef __cplusplus
}
#endif

#endif
