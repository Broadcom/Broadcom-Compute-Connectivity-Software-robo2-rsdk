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

#ifndef CBX_API_MCAST_H_
#define CBX_API_MCAST_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @defgroup cbx_mcast CBX Multicast
 * Forwarding Services Abstraction Layer (FSAL) Multicast operations.
 *
 * <h3>Basic VLAN Flooding</h3>
 * Basic VLAN flooding is supported directly in the VSI Table.
 * The port_map field defines the set of destination ports (DPGs)
 * that are a member of the VSI.
 * The default Destination Group ID = 0 in this usecase.
 *
 * The DLIs are generated based on the port_map and VSI:
 *  DLI == {Type = 1, DPG = from VSI.port_map, VSI}
 *
 * For example:
 *  VLAN = 0x44 has a port_map = 0b0_0000_0000_0010_0010
 *  the frame is forwarded to the following DLIs:
 *  DLI = {0b10_0101_0000_0100_0100}  Type = 0x1, DPG = 0x5 & VSI = 0x044
 *  DLI = {0b10_0001_0000_0100_0100}  Type = 0x1, DPG = 0x1 & VSI = 0x044
 * The VSI.port_map covers all VSI of Type PV.
 *
 * <h4>Programming information</h4>
 * In Basic VLAN flooding, one does not create Multicast entities.
 * The programming is contained within the VLAN module.
 * In the VSIT the Port Group Map (pg_map[15:0]) reflects Vlan membership and is
 * used as a VLAN broadcast domain when DGID is valid (default_is_multicast = 1)
 * and dlliid_dgid[12:0] = 0
 *
 * <h3>Advanced VLAN Flooding</h3>
 * Advanced VLAN flooding is used when there is at least one Logical Interface
 * that is a member of the flood domain, one uses the Destination Group
 * field in the VSI Table.
 *
 * The Destination Group Table (DGT) is indexed by the Destination Group ID (DGID).
 * The Logical Interface Link Table (LILT) is used to maintain sets of
 * Destination Logical Interfaces (DLIIDs) for multicast forwarding.
 * The LILT is indexed by the lilt_ndx field derived from the DGT (or MGT or RGT).

 * When a port or vlan_port is added to a VLAN, the appropiate entries need to be set
 * in the Logical Interface Link Table to configure the sets of
 * Destination Logical Interfaces
 *
 * <h4>Programming information</h4>
 * In Advanced VLAN flooding, a Multicast entity is created per VLAN.
 * dlliid_dgid[12:0] corresponds to the Multicast entity.
 * Logical Interface Link Table (LILT) is allocated as ports are added.
 *
 *  @{
 */

/**
 * @typedef cbx_mcastid_t
 * CBX mcast
 */

/**
 * @def CBX_MCAST_INVALID
 * Invalid Multicast
 */
#define CBX_MCAST_INVALID   0xffffffff

/** @def CBX_MCAST_MAX
 * Maximum Multicast entries
 */
#define CBX_MCAST_MAX  1024

/**
 * Multicast Logical Interface Table mode
 */
typedef enum cbx_mcast_lilt_mode_e {
    cbxMcastLiltModeNone,     /**< Basic Multicast  One Copy per Port, all PV-Type */
    cbxMcastLiltModeArray,    /**< Basic Multicast  One Copy per Port with LIN-Type */
    cbxMcastLiltModeList,     /**< Virtual Port Multicast  Multiple Copies per Port */
} cbx_mcast_lilt_mode_t;

/**
 * Multicast parameters
 */
typedef struct cbx_mcast_params_t {
    cbx_mcast_lilt_mode_t    lilt_mode;
} cbx_mcast_params_t;


/**
 * Initialize mcast entry parameters.
 *
 * @param  mcast_params parameter object
 */
void cbx_mcast_params_t_init( cbx_mcast_params_t *mcast_params );


/**
 * Multicast Information struct
 * information about Multicast membership
 */
typedef struct cbx_mcast_member_info_t {
    int                 max_portid;    /**< max portid array */
    cbx_portid_t       *portid_array;  /**< portid array */
    int                 valid_portid;  /**< number of valid portids in array */
} cbx_mcast_member_info_t;


/**
 * Multicast Create
 * This routine is used to create a mcast.
 *
 * @param mcast_params   (IN)  Multicast parameters.
 * @param mcastid        (OUT) Handle of the mcast created
 *                             CBX_MCAST_INVALID: Multicast could not be created
 *
 * @return return code
 * @retval CBX_E_NONE Success
 *         CBX_E_XXXX Failure
 */

int cbx_mcast_create ( cbx_mcast_params_t *mcast_params,
                       cbx_mcastid_t      *mcastid );

/**
 * Multicast destroy
 * This routine destroys a mcast previously created by cbx_mcast_create()
 *
 * @param mcastid  (IN)  Handle of the mcast to be destroyed
 *
 * @return return code
 * @retval CBX_E_NONE Success
 *         CBX_E_XXXX Failure
 *
 * @note This routine destroys a previously created mcast.
 */

int cbx_mcast_destroy ( cbx_mcastid_t mcastid );

/**
 * Multicast member add
 * This routine is used to add a port to a mcast.
 *
 * @param mcastid   (IN)  MCAST Identifier
 * @param portid    (IN)  Port Identifier
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_mcast_member_add ( cbx_mcastid_t mcastid,
                           cbx_portid_t  portid );

/**
 * Multicast remove
 * This routine is used to remove a port from a mcast.
 *
 * @param mcastid   (IN)  MCAST Identifier
 * @param portid    (IN)  Port Identifier
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_mcast_member_remove ( cbx_mcastid_t mcastid,
                              cbx_portid_t  portid );

/**
 * Multicast Member get
 * This routine is used to get information including array of ports on a mcast.
 *
 * @param mcastid            (IN)  MCAST Identifier
 * @param mcast_member_info  (IN/OUT) MCAST Information structure
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_mcast_member_info_get( cbx_mcastid_t            mcastid,
                               cbx_mcast_member_info_t *mcast_member_info );



/**
 * @}
 */


#ifdef __cplusplus
}
#endif

#endif
