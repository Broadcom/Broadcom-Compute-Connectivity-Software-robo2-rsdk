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

#ifndef CBX_API_STG_H_
#define CBX_API_STG_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @defgroup cbx_stg CBX STG
 * Forwarding Services Abstraction Layer (FSAL) Spanning Tree Group operations.
 *
 * <h3>Spanning Tree Programming</h3>
 * Avenger implemenation details
 *
 * There is architectural support for CBX_STG_MAX (128) Spanning Tree Groups
 * The registers and tables support this scale.
 *
 * Support: IEEE 802.1w Rapid Spanning Tree Protocol (RSTP)
 *               RTSP has simplified port states and improved convergence time
 *          IEEE 802.1s Multiple Spanning Tree Protocol (MSTP)
 *               MSTP uses RTSP port states
 *
 * The Avenger M7 implemenation can provide one RSTP instance for a single STG.
 * It could be practicable to have 1 or more RSTP instances to support multiple STGs.
 * The MSTP protocol management complexity in the control plane software
 *    would limit what is practicable on the Avenger M7.
 *
 * <h3>VSI Table</h3>
 * Each VLAN corresponding to an entry in the VSI Table has a stgid[6:0] field.
 * The basic initialzation would be to create a single (default) spanning tree group
 *   and this STG is used in the creation of all VLANs
 *
 * <h3>RSTP RSTP Port States</h3>
 * Discard - the port does not forward, process, or learn MAC addresses
 *       listens for and receives BPDUs.
 * Learn  - the port receives and transmits BPDUs, learns MAC addresses,
 *       but does not yet forward frames.
 * Forward - the port receives and sends data, learns MAC address,
 *       and receives and transmits BPDUs.
 *
 * The Port States are programmed via:
 *    receive enable vectors in the ISTGET in the IPP
 *    transmit enable vectors in the ESTGET in the PQM
 *
 * <h3>ISTGET Table</h3>
 * Ingress Spanning Tree Group Enable Table - indexed by STG
 * rx_ena[30:0] - Bit map to control transmission on a given PG.
 *    bit cleared when Port Group in Discard state
 *    bit set when Port Group in Learn state
 *    bit set when Port Group in Forward state
 *
 * <h3>ESTGET Table</h3>
 * Egress Spanning Tree Group Enable Table - indexed by STG
 * tx_ena[30:0] - Bit map to control transmission on a given PG.
 *    bit cleared when Port Group in Discard state
 *    bit cleared when Port Group in Learn state
 *    bit set when Port Group in Forward state
 *
 * <h3>Default</h3>
 * STG number 0 is the default and all ports are in forwarding state on this STG
 *
 *  @{
 */

/**
 * @typedef cbx_stgid_t
 * Spanning Tree Group Identifier
 */

/**
 * @def CBX_STG_INVALID
 * Invalid STG
 */
#define CBX_STG_INVALID   0xffffffff

/** Maximum Spanning Tree Groups  */
#define CBX_STG_MAX  128

/**
 * Spanning Tree Group Mode
 */
typedef enum cbx_stg_mode_e {
    cbxStgModeGlobal,
    cbxStgModeNormal,
} cbx_stg_mode_t;

/**
 * Spanning Tree Port State
 */
typedef enum cbx_stg_stp_state_e {
    cbxStgStpStateDiscard,
    cbxStgStpStateLearn,
    cbxStgStpStateForward,
} cbx_stg_stp_state_t;

/**
 * STG parameter struct
 *  Spanning Tree Group parameters
 *
 * cbxStgModeGlobal indicates STP not in use and global port bitmap is used
 * cbxStgModeNormal is used for active STP
 */
typedef struct cbx_stg_params_t {
    cbx_stg_mode_t   stg_mode;    /**< STG mode */
} cbx_stg_params_t;

/**
 * Initialize STG parameters.
 *
 * @param  stg_params  parameter object
 */
void cbx_stg_params_t_init( cbx_stg_params_t *stg_params );

/**
 * STG Information struct
 * information about a Spanning Tree Group
 */
typedef struct cbx_stg_info_t {
    int             max_portid;     /**< max portid array */
    cbx_portid_t   *portid_array;   /**< portid array */
    int             valid_portid;   /**< number of valid portids in array */
} cbx_stg_info_t;

/**
 * Create a STG
 * This routine is used to create a Spanning Tree Group.
 *
 * @param stg_params   (IN)  STG parameters.
 * @param stgid        (OUT) Handle of the STG created
 *                           CBX_STG_INVALID: STG could not be created
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_stg_create ( cbx_stg_params_t *stg_params,
                     cbx_stgid_t      *stgid );


/**
 * Destroy a stg previously created by cbx_stg_create()
 *
 * @param stgid    (IN)  Handle of the STG to be destroyed
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 *
 * @note This routine destroys a previously created Spanning Tree Group.
 */

int cbx_stg_destroy ( cbx_stgid_t stgid );


/**
 * STG vlan add
 * This routine is used to add a vlan to a STG
 *
 * @param stgid   STG Identifier
 * @param vlanid  Vlan Identifier
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure

 */

int cbx_stg_vlan_add( cbx_stgid_t      stgid,
                      cbx_vlanid_t     vlanid );

/**
 * STG vlan remove
 * This routine is used to remove a vlan from a STG
 *
 * @param stgid       (IN)  STG Identifier
 * @param vlanid      (IN)  Vlan Identifier
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_stg_vlan_remove( cbx_stgid_t      stgid,
                         cbx_vlanid_t     vlanid );

/**
 * STG set
 * This routine sets the Spanning Tree Protocol state of a port in the specified STG.
 *
 * @param stgid      (IN)  STG Identifier
 * @param portid     (IN)  Port Identifier
 * @param stp_state  (IN)  Spanning Tree Protocol port state
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 *
 * @note STG must be defined and Port added to STG
 */

int cbx_stg_stp_set ( cbx_stgid_t         stgid,
                      cbx_portid_t        portid,
                      cbx_stg_stp_state_t stp_state);


/**
 * STG STP port state get
 * This routine returns the Spanning Tree Protocol state
 *   of a port in the specified STG.
 *
 * @param stgid      (IN)  STG Identifier
 * @param portid     (IN)  Port Identifier
 * @param stp_state  (OUT) Spanning Tree Protocol port state
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 *
 * @note STG must be defined and Port added to STG
 */

int cbx_stg_stp_get ( cbx_stgid_t          stgid,
                      cbx_portid_t         portid,
                      cbx_stg_stp_state_t *stp_state);

/**
 * STG vlan remove all
 * This routine is used to remove all the vlans from a STG.
 * Place removed VLANs in default STG
 *
 * @param stgid       (IN)  STG Identifier
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_stg_vlan_remove_all(cbx_stgid_t      stgid);


/**
 * @}
 */


#ifdef __cplusplus
}
#endif

#endif
