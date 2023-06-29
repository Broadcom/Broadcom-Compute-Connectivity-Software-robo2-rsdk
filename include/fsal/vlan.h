/*
 * $Copyright: (c) 2017 Broadcom Limited.
 */

/**
 * @file
 * Coronado Bridge
 * Forwarding Services Abstraction Layer (FSAL) API
 * CBX VLAN
 */

#ifndef CBX_API_VLAN_H_
#define CBX_API_VLAN_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @defgroup cbx_vlan CBX VLAN
 * Forwarding Services Abstraction Layer (FSAL) Vlan operations.
 *
 *  Calling sequence:
 *  VLAN = basic port-vid bridging:
 *   Setup:
 *  1. Create vlan and Add port for basic bridging, Add l2 address for unicast.
 *     - Call vlan_create()
 *     - Call vlan_port_add() port A
 *     - Call vlan_port_add() port B
 *   Removal:
 *  2. Delete port from vlan
 *     - Call vlan_port_delete() port A
 *     - Call vlan_port_delete() port B
 *  3. Destory the vlan
 *     - Call vlan_destory()
 *
 * The VSI Table (VSIT) has these fields to control default destination:
 * - pg_map[15:0]          Port Group Map:  VLAN broadcast domain
 * - default_is_multicast  indicates default destination is multicast
 * - dlliid_dgid[15:0]     A union of:
 *         - dlliid[15:0]  Destination Local Logical Interface Identifier
 *         - dgid[15:0]    Destination Group (when default_is_multicast=1)
 *
 * In Avenger L2 Forwarding application - PV mode: the 16-bit port map is used.
 * The coorsponding bit in the port map is added/removed when a Port Group
 * is added/removed to/from the VLAN.
 *
 * On VLANs which contain LIN mode ports, the Destination Group ID is used.
 *
 * VLAN Programming
 * Avenger implemenation details of cbx_vlan_create()
 *
 * The VSI Table (VSIT) has these parameters that are set in the cbx_vlan_params_t struct:
 *     Learn mode
 *     Spanning Tree Group Identifier
 *     Isolation Group membership
 *     Drop on Unicast Lookup Fail
 *     Drop on Multicast Lookup Fail
 *     Drop on Source Lookup Fail
 *     Drop on Broadcast
         (these drop flags are also per-port setting)
 *  There is a 1:1 mapping between the FSAL API and these VSIT fields.
 *
 *
 * The VSI Table (VSIT) has these fields to control the lookup:
 *  - fid[11:0]     Filter ID.
 *  - dst_key[2:0]  Destination key.
 *  - src_key[2:0]  Source key
 *
 * There is suppport for both:
 *   - Independent VLAN learning (IVL)
 *   - single Shared VLAN learning (SVL).
 *  In the IVL case, set filter = VLAN
 *  In the SVL case, filter = Constant = 0
 * Which mode is selected is based on Switch configuration (cbx_switch_vlan_mode_t)
 *
 * Destination and Source key mode is FID_MAC (value=1).
 * This selects building the ARL key consisting of the VSI's Filter ID
 *  and DMAC/SMAC from the packet.
 *
 * Isolation Groups entry parameters
 * - isolation groups are Private VLANs
 *   they are used to prevent a set of ports from transmitting to each other.
 * - the VLAN table provides an isolation map indicating which
 *   Isolation Groups should be enforced for the VLAN.
 * - If the SPG, DPG and VLAN are all members of the same Isolation Group,
 *   then the packet is dropped.
 *
 * defined in vlan_parameters as:
 *     int  isolation[CBX_VLAN_ISOLATION_MAX]
 *    which is a boolean 0=not member, 1=member
 *
 *  @{
 */

/**
 * @typedef cbx_vlanid_t
 * VLAN Identifier
 */

/**
 * @def CBX_VLAN_INVALID
 * Invalid Vlan
 */
#define CBX_VLAN_INVALID   0xffffffff

/** @def CBX_VLAN_MAX
 * Maximum VLAN entries
 */
#define CBX_VLAN_MAX  4096

/** @def CBX_VLAN_ISOLATION_MAX
 * Maximum VLAN_ISOLATION groups
 */
#define CBX_VLAN_ISOLATION_MAX 4

/** VLAN Learn mode
 */
typedef enum cbx_vlan_learn_mode_e {
    cbxVlanLearnModeDisable,
    cbxVlanLearnModeCopyHost,
    cbxVlanLearnModeMarkPending,
    cbxVlanLearnModeMarkReady,
} cbx_vlan_learn_mode_t;

/** @{
 * VLAN Flags
 */
#define  CBX_VLAN_WRED_ENABLE        0x00000001
#define  CBX_VLAN_MCAST_DGT          0x00000002

#define  CBX_VLAN_DROP_ULF           0x00000010  /**< Drop on Unicast Lookup Fail */
#define  CBX_VLAN_DROP_DLF           0x00000020  /**< Drop on Multicast Lookup Fail */
#define  CBX_VLAN_DROP_SLF           0x00000040  /**< Drop on Source Lookup Fail */
#define  CBX_VLAN_DROP_BCAST         0x00000080


#define  CBX_VLAN_ALLOCATE_VSI       0x00000100
/** @} */

/**
 * Vlan parameters struct
 * information about a vlan (VSI)
 */
typedef struct cbx_vlan_params_t {
    uint32_t               flags;       /**< Flags:  CBX_VLAN_XX */
    cbx_vlan_t             vlan;        /**< VLAN index */
    cbx_vlan_learn_mode_t  learn_mode;  /**< Learn mode */
    cbx_stgid_t            stgid;       /**< Spanning Tree Group Identifier */
    cbx_mcastid_t          mcastid;     /**< Multicast Identifier, if MCAST_DGT */
    cbx_portid_t           ucastid;     /**< Unicast interface flood, if !MCAST_DGT */
    int  isolation[CBX_VLAN_ISOLATION_MAX];  /**< Isolation Group membership */
} cbx_vlan_params_t;

/**
 * Initialize vlan entry parameters.
 *
 * @param  vlan_params  parameter object
 */
void cbx_vlan_params_t_init( cbx_vlan_params_t *vlan_params );


/**
 * Create a vlan
 * This routine is used to create a vlan.
 *
 * @param vlan_params   (IN)  Vlan parameters.
 * @param vlanid        (OUT) Handle of the vlan created
 *                            CBX_VLAN_INVALID: Vlan could not be created
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_vlan_create ( cbx_vlan_params_t *vlan_params,
                      cbx_vlanid_t      *vlanid );

/**
 * Destroy a vlan previously created by cbx_vlan_create()
 *
 * @param vlanid    (IN)  Handle of the vlan to be destroyed
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 *
 * @note This routine destroys a previously created vlan.
 */

int cbx_vlan_destroy ( cbx_vlanid_t vlanid );

/**
 * Vlan Destroy All
 * This routine is used to destroy all Vlans that have been created.
 * The special Vlan vlanid=1 is not affected.
 *
 * @param void
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */
int cbx_vlan_destroy_all( void );

/**
 * VLAN Information get
 * This routine is used to get the parameters of a Vlan.
 *
 * @param vlanid       (IN)  VLAN Identifier
 * @param vlan_params  (OUT) Vlan parameters
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_vlan_get ( cbx_vlanid_t       vlanid,
                   cbx_vlan_params_t *vlan_params );

/**
 * VLAN Information set
 * This routine is used to set the parameters of a Vlan.
 *
 * @param vlanid       (IN)  VLAN Identifier
 * @param vlan_params  (IN)  Vlan parameters
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_vlan_set ( cbx_vlanid_t      vlanid,
                   cbx_vlan_params_t vlan_params );


// FIXME this should be an enum: cbx_vlan_port_mode_e
/** @{
 * VLAN Port Modes
 */
#define  CBX_VLAN_PORT_MODE_UNTAGGED    0x00000001
#define  CBX_VLAN_PORT_MODE_TAGGED      0x00000002
/** @} */

/**
 * Add a Port to a VLAN
 * This routine is used to add a port (cbx_portid) to a VLAN.
 * The port can be a Physical Port or a Port Group (LAG) or a
 * Logical Interface (vlan_port).
 *
 * @param vlanid     (IN)  VLAN Identifier
 * @param portid     (IN)  Port Identifier
 * @param mode       (IN)  tagged, untagged modes
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_vlan_port_add( cbx_vlanid_t vlanid,
                       cbx_portid_t portid,
                       uint32_t     mode );

/**
 * Delete a Port from a VLAN
 * This routine is used to remove a port (cbx_portid) from a VLAN.
 * The port can be a Physical Port or a Port Group (LAG) or a
 * Logical Interface (vlan_port).
 *
 * The implementation deletes the ingress VLAN port memebership, but should
 * not remove the egress editing directives, since there can be packets in flight.
 *
 * @param vlanid      (IN)  VLAN Identifier
 * @param portid      (IN)  Port Identifier
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_vlan_port_remove( cbx_vlanid_t vlanid,
                          cbx_portid_t portid );

/**
 * Get an array of Ports on a VLAN
 * This routine is used to get an array of ports (cbx_portid) on a VLAN.
 * The ports can be a Physical Port or a Port Group (LAG) or a
 * Logical Interface (vlan_port).
 *
 * @param vlanid        (IN)  VLAN Identifier
 * @param array_max     (IN)  Maximum number of elements in the array
 * @param portid_array  (OUT) Array of portids
 * @param mode_array    (OUT) Array of port mode CBX_VLAN_PORT_MODE_XX flags
 * @param array_size    (OUT) Actual number of elements in the array
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_vlan_port_get ( cbx_vlanid_t    vlanid,
                        int             array_max,
                        cbx_portid_t   *portid_array,
                        uint32_t       *mode_array,
                        int            *array_size);


#ifdef CONFIG_VLAN_PVID
/**
 * Set pvid field of Port to given VLAN id
 *
 * @param vlanid     (IN)  VLAN Identifier
 * @param portid     (IN)  Port Identifier
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_vlan_port_pvid_set( cbx_vlanid_t vlanid,
                            cbx_portid_t portid);
#endif
/**
 * @}
 */


#ifdef __cplusplus
}
#endif

#endif
