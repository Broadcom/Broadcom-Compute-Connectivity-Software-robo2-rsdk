/*
 * $Copyright: (c) 2017 Broadcom Limited.
 */

/**
 * @file
 * Coronado Bridge
 * Forwarding Services Abstraction Layer (FSAL) API
 */

#ifndef CBX_API_AUTH_H_
#define CBX_API_AUTH_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @defgroup cbx_auth CBX AUTH
 * Forwarding Services Abstraction Layer (FSAL) Authentication operations.
 *
 * Extensible Authentication Protocol (EAP) is an authentication framework
 * defined in RFC 3748
 *
 * IEEE 802.1x is a port-based authentication protocol based on the exchange of Extensible
 * Authentication Protocol over LAN (EAPOL) packets.
 * IEEE 802.1x defines a port as being either in the authenticated or unauthenticated state.
 *
 * In Avenger, the 802.1X Port Authentication is a Managed mode feature.
 * In Unmanaged and Unmanaged+ mode, the default 802.1X is disabled on all ports.
 * The Management CPU controls the 802.1X state and in this mode,
 * the default 802.1X is enabled on a per port basis and all the ports
 * are in the unauthorized state.
 *
 * An EAPOL frame has a Multicast MAC-DA (01-80-C2-00-00-03) per IEEE 802.1X,
 * or a user-defined Unicast (UC) or Multicast (MC) address.
 *
 * EAPOL frames are are forwarded to the CPU.
 * The CPU then sends the frames to the authenticator server.
 *
 * After the 802.1X EAP authentication completes and successful,
 * the port is changed to authorized state
 *
 * The per-port EAP can be programmed in three modes:
 * - Basic Mode is the standard mode.
 *   The EAP_BLK_MODE bit would be set before authentication to
 *   block all of the incoming packets.
 *   Upon authentication, the EAP_BLK_MODE bit would be cleared to
 *   allow all the incoming packets.
 *   In this mode, the Source Address of incoming packets is not checked.
 *
 * - Extended Mode where an extra filtering mechanism is
 *   implemented after the port is authenticated. If the
 *   Source MAC address is unknown, the incoming packets
 *   would be dropped and the unknown SA would not be learned.
 *   However if the incoming packet is IEEE 802.1X
 *   packet, or special frames, the incoming packets will be forwarded.
 *   The definition of the Unknown SA in this case is when the switch cannot match
 *   the incoming Source MAC address to any of the addresses in ARL table,
 *   or the incoming Source MAC address matches the address in ARL table,
 *   but the port number is mismatched.
 *
 * - Simplified Mode. In this mode, the unknown Source MAC address
 *   packets would be forwarded to the CPU rather than dropped.
 *   Otherwise, it is same as the Extended Mode operation.
 *
 *  @{
 */


/**
 * Authentication mode
 */
typedef enum cbx_auth_mode_e {
    cbxAuthModeUncontrolled,    /**< Port Security is disabled */
    cbxAuthModeUnauth,          /**< Port Security default is UnAuthenticated */
    cbxAuthModeAuth,            /**< Port Security default is Authenticated */
} cbx_auth_mode_t;

/** Authentication Port Flags */
#define  CBX_AUTH_PORT_UNAUTH            0x00000001   /**< UnAuthenticated Port */
#define  CBX_AUTH_PORT_BLOCK_IN          0x00000002   /**< Allow outgoing packets */
#define  CBX_AUTH_PORT_BLOCK_INOUT       0x00000004   /**< Do not allow in or out packets */
#define  CBX_AUTH_PORT_AUTH              0x00000008   /**< Authenticated Port */
#define  CBX_AUTH_PORT_UNKNOWN_DROP      0x00000010   /**< Drop unknown SA packets */
#define  CBX_AUTH_PORT_UNKNOWN_TRAP      0x00000020   /**< Trap unknown SA packets */
#define  CBX_AUTH_PORT_LINK              0x00000040   /**< Link change deauthenticates */

/**
 * Authentication parameter struct
 * - global configuration for Authentication
 */
typedef struct cbx_auth_params_t {
    uint32_t          flags;      /**< Flags:  CBX_AUTH_XX */
    cbx_auth_mode_t   auth_mode;  /**< Authentication mode */
    eth_mac_t         uc_mac;     /**< Auth unicast MAC address */
    eth_mac_t         mc_mac;     /**< Auth multicast MAC address */
} cbx_auth_params_t;

/**
 * Initialize global authentication parameters.
 *
 * @param auth_params global authentication parameter object
 */
void cbx_auth_params_t_init ( cbx_auth_params_t *auth_params );

/**
 * Authentication Port mode struct
 */
typedef struct cbx_auth_port_mode_t {
    uint32_t          flags;      /**< Flags:  CBX_AUTH_PORT_XX */
} cbx_auth_port_mode_t;

/**
 * Initialize authentication port parameters.
 *
 * @param auth_port_mode authentication port parameter object
 */
void cbx_auth_port_mode_t_init ( cbx_auth_port_mode_t *auth_port_mode );

/**
 * Initialize Authentication module
 * This routine is used to init auth.
 *
 * @param auth_params   (IN)  Authentication parameters.
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_auth_init ( cbx_auth_params_t *auth_params );

/**
 * Detach a Authentication previously initialized by cbx_auth_init()
 *
 * @param auth_params   (IN)  Authentication parameters.
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 *
 * @note This routine destroys a previously initialized Authentication module
 */

int cbx_auth_detach ( cbx_auth_params_t *auth_params );


/**
 * Authentication mode get
 * This routine is used to get the Authentication configuration information.
 *
 * @param auth_params   (OUT)  Authentication parameters.
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_auth_mode_get ( cbx_auth_params_t *auth_params );


/**
 * Authentication mode set
 * This routine is used to set the Authentication configuration information.
 *
 * @param auth_params   (IN)   Authentication parameters.
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_auth_mode_set ( cbx_auth_params_t *auth_params );


/**
 * Authentication Port get
 * This routine is used get the Authentication mode of a port.
 *
 * @param portid     (IN)  Port Identifier
 * @param port_mode  (OUT) Port Authentication mode
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_auth_port_get ( cbx_portid_t          portid,
                        cbx_auth_port_mode_t *port_mode );


/**
 * Authentication Port set
 * This routine is used set the Authentication mode of a port.
 *
 * @param portid     (IN)  Port Identifier
 * @param port_mode  (IN)  Port Authentication mode
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_auth_port_set ( cbx_portid_t          portid,
                        cbx_auth_port_mode_t *port_mode );


/**
 * @}
 */


#ifdef __cplusplus
}
#endif

#endif
