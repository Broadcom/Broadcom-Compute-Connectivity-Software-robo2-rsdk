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
 * CBX VLAN Logical Interface
 */

#ifndef CBX_API_LIF_H_
#define CBX_API_LIF_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @defgroup cbx_lif CBX LIF
 * Forwarding Services Abstraction Layer (FSAL) Vlan Logical Interface operations.
 *
 *  Calling sequence:
 *  VLAN = Logical Interface bridging:
 * Setup:
 *  1. Create vlan
 *     - Call vlan_create()
 *  2. Create vlan logical interfaces
 *     - Call vlan_lif_create() logical i/f A
 *     - Call vlan_lif_create() logical i/f B
 *  3. Add logical interfaces to vlan
 *     - Call vlan_port_add() A
 *     - Call vlan_port_add() B
 * Removal:
 *  4. Delete logical interfaces from vlan
 *     - Call vlan_port_delete() A
 *     - Call vlan_port_delete() B
 *  5. Destroy the vlan logical interfaces
 *     - Call vlan_lif_destroy() logical i/f A
 *     - Call vlan_lif_destroy() logical i/f B
 *  6. Destory the vlan
 *     - Call vlan_destory()
 *
 * The VSI Table (VSIT) has these fields to control default destination:
 * - pg_map[15:0]          Port Group Map:   VLAN broadcast domain
 * - default_is_multicast  indicates default destination is multicast
 * - dlliid_dgid[15:0]     A union of:
 *         - dlliid[15:0]  Destination Local Logical Interface Identifier
 *         - dgid[15:0]    Destination Group (when default_is_multicast=1)
 *
 * On LIFs which contain LIN mode ports, the Destination Group ID is used.
 *
 *  @{
 */


/**
 * @def CBX_VLAN_LIF_INVALID
 * Invalid Vlan Logical Interface
 */
#define CBX_VLAN_LIF_INVALID   0xffffffff

/** @def CBX_VLAN_LIF_MAX
 * Maximum VLAN Logical Interface entries
 */
#define CBX_VLAN_LIF_MAX  2048

/** VLAN Logical Interface Match mode
 */
typedef enum cbx_vlan_lif_match_e {
    cbxVlanLifMatchInvalid,         /* Illegal */
    cbxVlanLifMatchNone,            /* No match criteria */
    cbxVlanLifMatchPortVlan,        /* Port - VID */
    cbxVlanLifMatchPortVlanStacked  /* Port - VID - VID*/
} cbx_vlan_lif_match_t;

/** VLAN Logical Interface tag actions
 */
typedef enum cbx_vlan_lif_tag_actions_e {
    cbxVlanLifTagActionNone,       /**< Do not modify vlan tag */
    cbxVlanLifTagActionAdd,        /**< Add vlan tag */
    cbxVlanLifTagActionReplace,    /**< Replace vlan tag */
    cbxVlanLifTagActionDelete,     /**< Delete vlan tag */
} cbx_vlan_lif_tag_actions_t;

/** VLAN LIF Flags */
#define  CBX_VLAN_LIF_INNER_VLAN_PRESERVE               0x00000001


/**
 * Vlan Logical Interface parameters struct
 *                  cbx_vlan_lif_*
 *   alternate names:
 *                   cbx_vlan_logical_interface_*
 *                   cbx_vlan_li_*
 *                   cbx_vlan_li_port_*
 * information about a VLAN Logical Interface
 */
typedef struct cbx_vlan_lif_params_t {
    uint32_t     flags;             /**< Flags:  CBX_VLAN_LIF_XX */
    cbx_vlan_lif_match_t criteria;  /**< Match criteria */
    int          default_tc;        /**< default traffic class */
    int          default_dp;        /**< default drop presedence */
    cbx_portid_t portid;            /**< Physical Port or Port Group */
    cbx_vlan_t   match_vlan;        /**< Outer vlan tag to match */
    cbx_vlan_t   match_inner_vlan;  /**< Inner vlan tag to match */

    cbx_vlan_lif_tag_actions_t  vlan_action;       /**< Egress Outer tag action */
    cbx_vlan_t   egress_vlan;                      /**< Egress Outer vlan tag */
    cbx_vlan_lif_tag_actions_t  inner_vlan_action; /**< Egress Inner tag action */
    cbx_vlan_t   egress_inner_vlan;                /**< Egress Inner vlan tag */
} cbx_vlan_lif_params_t;

/**
 * Initialize vlan logical interface entry parameters.
 *
 * @param  vlan_lif_params  parameter object
 */
void cbx_vlan_lif_params_t_init( cbx_vlan_lif_params_t *vlan_lif_params );


/**
 * Create a vlan_lif
 * This routine is used to create a VLAN Logical Interface (vlan_lif)
 *
 * @param vlan_lif_params  (IN)  vlan_lif parameters.
 * @param vlan_lifid       (OUT) Handle of the vlan_lif created
 *                            CBX_VLAN_LIF_INVALID  vlan_lif could not be created
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_vlan_lif_create ( cbx_vlan_lif_params_t *vlan_lif_params,
                          cbx_portid_t          *vlan_lifid );

/**
 * Destroy a vlan logical interface previously created by cbx_vlan_lif_create()
 *
 * @param vlan_lifid    Handle of the vlan_lif to be destroyed
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 *
 * @note This routine destroys a previously created vlan logical interface.
 */

int cbx_vlan_lif_destroy ( cbx_portid_t vlan_lifid );

/**
 * Find a vlan logical interface previously created by cbx_vlan_lif_create()
 *  based on vlan_lif parameter match criteria
 *
 * @param vlan_lif_params    (IN)  vlan_lif parameters to match.
 * @param vlan_lifid         (OUT) Handle of the vlan_lif found
 *                               CBX_VLAN_LIF_INVALID if not found
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 *
 */

int cbx_vlan_lif_find ( cbx_vlan_lif_params_t *vlan_lif_params,
                        cbx_portid_t          *vlan_lifid );


/**
 * @}
 */


#ifdef __cplusplus
}
#endif

#endif
