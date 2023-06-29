/*
 * $Copyright: (c) 2017 Broadcom Limited.
 */

/**
 * @file
 * Coronado Bridge
 * Forwarding Services Abstraction Layer (FSAL) API
 */

#ifndef CBX_API_MIRROR_H_
#define CBX_API_MIRROR_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @defgroup cbx_mirror CBX MIRROR
 * Forwarding Services Abstraction Layer (FSAL) Mirror operations.
 *
 *  Calling sequence:
 *   - cbx_mirror_dest_create()  returns mirror_destid
 *       this will allocate and configure a mirror destination
 *       creating a mirror TLV indicating:
 *         where to send the frame
 *           PQM.MGT - mirror group table (for ingress mirror)
 *         how to edit/encap
 *           logical interface entry
 *   - cbx_mirror_create() returns mirrorid
 *           mirror_tc[2:0]  TC to use for mirroring
 *           mirror_dp[1:0]  DP to use for mirroring
 *           truncate        0=no truncation
 *           sample_prob[9:0]  randomly sampling (0 = do not sample)
 *   - cbx_mirror_port_add()   adds mirrorid to port
 *       this will set the PGT paramaters on ingress or egress
 *           mirror[5:0]     mirror group to be used (0 = no mirror)
 *
 * Ingress Mirrored packets:
 * Ingress mirroring sends a copy of the frame as it was received on
 * the ingress port (prior to any edits) to a specific destination.
 *
 * The frame or a portion of the frame maybe encapsulated in another header.
 *
 * A frame may be ingress mirrored based on:
 * - Port Group Table (PGT) which includes a 6b mirror field
 * - Ingress VSI Mirror Table (IVMT) which includes a 6bit mirror field
 * - CFP match includes a 6bit mirror field
 *
 * The 64 mirrors (and the traps) are used to index the Mirror and Trap Group Table (MTGT).
 * The MTGT provides the TC, DP, Truncate, and Sample parameters for the mirror.
 * The MTGT also provides the Destination Logical Interface (DLI) to uncast the packet.
 * Ingress mirrored packets have dont_edit flag set by the PQM.
 * This separate DLI (always N type) is used from the normal switched packet.
 *
 * Egress Mirrored packets:
 * Egress mirroring sends a copy of the frame as it would appear on the network
 * after egress processing.
 *
 * A frame may be egress mirrored based on:
 * - Egress Port Group Table (EPGT)
 * - EgressVSI Mirror Table (EVMT)
 * these tables include a 6bit mirror field, TC, DP, and Sample parameters.
 * (there is no egress mirror truncate option)
 *
 * Frames marked for egress mirror:
 * - Are send back to SIA  input to the IPP  and pass through the switch again
 * - TAG parser is disabled
 * - SLIC mapping is disabled
 * - mirror TLV indicating where to send the frame and how to edit/encap if needed
 * - frame skips through the FP and goes directly to the PMQ for enqueue
 *
 *  @{
 */

/**
 * @typedef cbx_mirrorid_t
 * Mirror Identifier
 */

/**
 * @def CBX_MIRROR_INVALID
 * Invalid MIRROR
 */
#define CBX_MIRROR_INVALID   0xffffffff

/** Maximum Mirror entires  */
#define CBX_MIRROR_MAX  64

/**
 * @typedef cbx_mirror_destid_t
 * Mirror Destination Identifier
 */
typedef uint32_t cbx_mirror_destid_t;

/**
 * @def CBX_MIRROR_DEST_INVALID
 * Invalid MIRROR Destination
 */
#define CBX_MIRROR_DEST_INVALID   0xffffffff

/** @def CBX_MIRROR_DEST_MAX
 * Maximum Mirror entires
 */
#define CBX_MIRROR_DEST_MAX  64

/**
 * Mirror mode
 */
typedef enum cbx_mirror_mode_e {
    cbxMirrorModeIngress,
    cbxMirrorModeEgress,
} cbx_mirror_mode_t;

/** Mirror Destination Flags */
#define  CBX_MIRROR_DEST_ENCAP        0x00000001

/**
 * Mirror destination struct
 * information about a mirror destination
 */
typedef struct cbx_mirror_dest_t {
    uint32_t         flags;          /**< Flags:  CBX_MIRROR_DEST_XX */
    cbx_portid_t     port;           /**< Mirror destination. */
    eth_mac_t        src_mac;        /**< encap L2 source MAC address. */
    eth_mac_t        dst_mac;        /**< encap L2 destination MAC address. */
    cbx_vlan_t       vlan_id;        /**< encap VLAN ID. */
    cbx_encap_info_t encap;          /**< L3/SPAN/ERSPAN encapsulation */
} cbx_mirror_dest_t;

/**
 * Initialize mirror destination parameters.
 *
 * @param mirror_dest  mirror destination parameter object
 */
void cbx_mirror_dest_t_init( cbx_mirror_dest_t *mirror_dest );

/** Mirror Flags */
#define  CBX_MIRROR_TRUNCATE            0x00000001  /**< Truncate mirrored packet */
#define  CBX_MIRROR_MODE_PORT_INGRESS   0x00000002  /**< Port Ingress mirroring */
#define  CBX_MIRROR_MODE_PORT_EGRESS    0x00000004  /**< Port Egress mirroring */
#define  CBX_MIRROR_MODE_VLAN_INGRESS   0x00000008  /**< VLAN Ingress mirroring */
#define  CBX_MIRROR_MODE_VLAN_EGRESS    0x00000010  /**< VLAN Egress mirroring */

/**
 * Mirror struct
 * information about a mirror
 */
typedef struct cbx_mirror_params_t {
    uint32_t          flags;         /**< Flags:  CBX_MIRROR_XX */
    int               mirror_tc;     /**< TC to use for mirroring */
    int               mirror_dp;     /**< DP to use for mirroring */
    uint32_t          sample_rate;   /**< sampling parameter */
} cbx_mirror_params_t;

/**
 * Initialize mirror parameters.
 *
 * @param mirror_params mirror parameter object
 */
void cbx_mirror_params_t_init( cbx_mirror_params_t *mirror_params );

/**
 * Mirror Port struct
 * information about a mirror port
 */
typedef struct cbx_mirror_port_params_t {
    cbx_mirror_mode_t mode;          /**< ingress or egress */
} cbx_mirror_port_params_t;

/**
 * Initialize mirror port parameters.
 *
 * @param mirror_port_params mirror port parameter object
 */
void cbx_mirror_port_params_t_init( cbx_mirror_port_params_t *mirror_port_params );

/**
 * Mirror Vlan struct
 * information about a mirror vlan
 */
typedef struct cbx_mirror_vlan_params_t {
    cbx_mirror_mode_t mode;          /**< ingress or egress */
} cbx_mirror_vlan_params_t;

/**
 * Initialize mirror vlan parameters.
 *
 * @param mirror_vlan_params mirror vlan parameter object
 */
void cbx_mirror_vlan_params_t_init( cbx_mirror_vlan_params_t *mirror_vlan_params );

/**
 * Create a mirror destination
 * This routine is used to create a mirror destination.
 *
 * @param mirror_dest   (IN)  Mirror destination parameters.
 * @param mirror_destid (OUT) Handle of the mirror destination created
 *                      CBX_MIRROR_DEST_INVALID: Mirror destination could not be created
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_mirror_dest_create ( cbx_mirror_dest_t   *mirror_dest,
                             cbx_mirror_destid_t *mirror_destid );


/**
 * Destroy a mirror destination previously created by cbx_mirror_dest_create()
 *
 * @param mirror_destid    (IN)  Handle of the mirror destination to be destroyed
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 *
 * @note This routine destroys a previously created mirror destination.
 */

int cbx_mirror_dest_destroy ( cbx_mirror_destid_t mirror_destid );


/**
 * Create a mirror
 * This routine is used to create a mirror.
 *
 * @param mirror_params (IN)  Mirror parameters.
 * @param mirror_destid (IN)  Mirror destination identifier
 * @param mirrorid      (OUT) Handle of the mirror created
 *                            CBX_MIRROR_INVALID: Mirror could not be created
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_mirror_create ( cbx_mirror_params_t *mirror_params,
                        cbx_mirror_destid_t  mirror_destid,
                        cbx_mirrorid_t      *mirrorid );


/**
 * Destroy a mirror previously created by cbx_mirror_create()
 *
 * @param mirrorid    (IN)  Handle of the mirror to be destroyed
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 *
 * @note This routine destroys a previously created mirror.
 */

int cbx_mirror_destroy ( cbx_mirrorid_t mirrorid );

/**
 * Mirror port add
 * This routine is used to add a Mirror to a port
 *
 * @param mirrorid             (IN)  Mirror Identifier
 * @param portid               (IN)  Port Identifier
 * @param mirror_port_params   (IN)  Mirror port parameters.
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_mirror_port_add ( cbx_mirrorid_t            mirrorid,
                          cbx_portid_t              portid,
                          cbx_mirror_port_params_t *mirror_port_params );

/**
 * Mirror port remove
 * This routine is used to remove a Mirror from a port
 *
 * @param mirrorid    (IN) Mirror Identifier
 * @param portid      (IN) Port Identifier
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_mirror_port_remove ( cbx_mirrorid_t mirrorid,
                             cbx_portid_t   portid );

/**
 * Mirror vlan add
 * This routine is used to add a Mirror to a vlan
 *
 * @param mirrorid             (IN)  Mirror Identifier
 * @param vlanid               (IN)  VLAN Identifier
 * @param mirror_vlan_params   (IN)  Mirror port parameters.
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_mirror_vlan_add ( cbx_mirrorid_t            mirrorid,
                          cbx_vlanid_t              vlanid,
                          cbx_mirror_vlan_params_t *mirror_vlan_params );

/**
 * Mirror vlan remove
 * This routine is used to remove a Mirror from a vlan
 *
 * @param mirrorid   (IN)  Mirror Identifier
 * @param vlanid     (IN)  VLAN Identifier
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_mirror_vlan_remove ( cbx_mirrorid_t mirrorid,
                             cbx_vlanid_t   vlanid );


/**
 * @}
 */


#ifdef __cplusplus
}
#endif

#endif
