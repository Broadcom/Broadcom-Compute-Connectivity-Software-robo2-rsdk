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

#ifndef CBX_API_LAG_H_
#define CBX_API_LAG_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @defgroup cbx_lag CBX LAG
 * Forwarding Services Abstraction Layer (FSAL) Link Aggregration Group operations.
 *
 * Port Selection Criteria
 * Avenger programming:
 * - the port selection criteria is a global configuration
 * - smac/dmac/vid/l3/l4 and combinations thereof
 * - three keys, based on packet type (ipv4/ipv6/other)
 *
 * Squelch Table
 *
 * Implemented using an 8b Load Balance Hash (LBH) and a 256 entry squelch table.
 * Calculate a hash over a select subset of flow immutable fields within the PDU.
 * This set of fields may vary depending on the nature of the PDU.
 * The hash is known as the Load Balancing Hash (LBH) and it is calculated for every PDU.
 * The LBH is used to index the Squelch Table (SQT) to obtain a Squelch Vector (SQV).
 * The SQVs in this table are constructed such that they will eliminate all but
 * one PP from all sets of PPs that participate in a PG.
 *
 * @{
 */

/**
 * @typedef cbx_lag_t
 * CBX lag
 */

/**
 * @def CBX_LAG_INVALID
 * Invalid Lag
 */
#define CBX_LAG_INVALID   0xffffffff

/**
 * @def CBX_LAG_ALL
 * Link Aggregration ID used for global LAG configuration
 */
#define CBX_LAG_ALL       0x00000000

/** @def CBX_LAG_MAX
 * Maximum LAG entries
 */
#define CBX_LAG_MAX  32

/**
 * Link Aggregration mode
 */
typedef enum cbx_lag_mode_e {
    cbxLagModeNormal,
    cbxLagModeCascade,
} cbx_lag_mode_t;

/**
 * Link Aggregration Reseed mode
 */
typedef enum cbx_lag_reseed_mode_e {
    cbxLagReseedNormal,
} cbx_lag_reseed_mode_t;

/** @{
 * PSC Flags
 */
#define  CBX_LAG_PSC_SRCMAC   0x00000001  /**< LAG Hash based on source MAC address */
#define  CBX_LAG_PSC_DSTMAC   0x00000002  /**< LAG Hash based on destination MAC address */
#define  CBX_LAG_PSC_VLAN     0x00000004  /**< LAG Hash based on Vlan ID */
#define  CBX_LAG_PSC_SRCIP    0x00000008  /**< LAG Hash based on source IP address */
#define  CBX_LAG_PSC_DSTIP    0x00000010  /**< LAG Hash based on destination IP address */
#define  CBX_LAG_PSC_SRCPORT  0x00000020  /**< LAG Hash based on source L4 port */
#define  CBX_LAG_PSC_DSTPORT  0x00000040  /**< LAG Hash based on destination L4 port */
#define  CBX_LAG_PSC_SRCIPV6  0x00000080  /**< LAG Hash based on source IPv6 address */
#define  CBX_LAG_PSC_DSTIPV6  0x00000100  /**< LAG Hash based on destination IPv6 address */
#define  CBX_LAG_PSC_SPG      0x00000200  /**< LAG Hash based on source port group */
#define  CBX_LAG_PSC_L3PROTO  0x00000400  /**< LAG Hash based on L3 protocol */
#define  CBX_LAG_PSC_L4BYTES  0x00000800  /**< LAG Hash based on L4 bytes */
/** @} */

/**
 * LAG PSC paramater structure
 * information about LAG port selection criteria
 */
typedef struct cbx_lag_psc_t {
    uint32_t           flags;           /**< Flags:  CBX_LAG_PSC_XX */
    cbx_packet_type_t  pkt_type;        /**< packet type to apply PSC */
} cbx_lag_psc_t;

/**
 * Initialize lag port selection criteria
 *
 * @param  lag_psc      port selection criteria object
 */
void cbx_lag_psc_t_init ( cbx_lag_psc_t *lag_psc );

/**
 * LAG Parameter struct
 *  - configuration information
 */
typedef struct cbx_lag_params_t {
    cbx_lag_mode_t  lag_mode;    /**< Link Aggregation mode */
    int             port_group;  /**< Global Port Group index (read-only) */
} cbx_lag_params_t;

/**
 * Initialize lag entry parameters.
 *
 * @param  lag_params   parameter object
 */
void cbx_lag_params_t_init ( cbx_lag_params_t *lag_params );

/**
 * LAG Information struct
 * information about a Link Aggregration Group membership
 */
typedef struct cbx_lag_member_info_t {
    cbx_lag_params_t  lag_params;    /**< lag configuration */
    int               max_portid;    /**< max portid array */
    cbx_portid_t     *portid_array;  /**< portid array */
    int               valid_portid;  /**< number of valid portids in array */
} cbx_lag_member_info_t;

/**
 * Create a lag
 * This routine is used to create a Link Aggregration Group.
 * It allocates an entry in the Port Group Table (PGT)
 * It sets this IPP Table:
 *  - Physical Port to Port Group (PP2PG) table
 * It sets these PQM Tables:
 *  - Port Group to Physical Port Fan-Out Vector (PG2PPFOV) table
 *  - Physical Port to Port Group Identifier (PP2PGID) table
 *  - Squelch Vector Table (SVT)
 * These tables are also used by the Port module.
 * The set of Ports and LAGs make up the Global Port Number space
 *
 * @param lag_params  (IN)  Lag parameters.
 * @param lagid       (OUT) Handle of the Lag created
 *                          CBX_LAG_INVALID: Lag could not be created
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_lag_create ( cbx_lag_params_t *lag_params,
                     cbx_portid_t     *lagid );

/**
 * Destroy a lag previously created by cbx_lag_create()
 *
 * @param lagid    (IN)  Handle of the lag to be destroyed
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 *
 * @note This routine destroys a previously created Link Aggregration Group.
 */

int cbx_lag_destroy ( cbx_portid_t lagid );

/**
 * LAG add
 * This routine is used to add a port to a LAG.
 *
 * @param lagid     (IN)  LAG Identifier
 * @param portid    (IN)  Port Identifier
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_lag_member_add ( cbx_portid_t lagid, cbx_portid_t portid );

/**
 * LAG remove
 * This routine is used to remove a port from a LAG.
 *
 * @param lagid     (IN)  LAG Identifier
 * @param portid    (IN)  Port Identifier
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_lag_member_remove ( cbx_portid_t lagid, cbx_portid_t portid );


/**
 * LAG Member get
 * This routine is used to get information including array of ports on a LAG.
 *
 * @param lagid            (IN)     LAG Identifier
 * @param lag_member_info  (IN/OUT) LAG Member Information structure
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_lag_member_get( cbx_portid_t           lagid,
                        cbx_lag_member_info_t *lag_member_info );


/**
 * LAG PSC get
 * This routine is used to get the Port Selection Criteria value.
 *
 * @param lagid     (IN)     LAG Identifier
 *                           when PSC is global, use CBX_LAG_ALL
 * @param psc       (IN/OUT) PSC object
 *                           input - traffic type
 *                           output - PSC flags
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_lag_psc_get ( cbx_portid_t lagid, cbx_lag_psc_t *psc );

/**
 * LAG PSC set
 * This routine is used to set the Port Selection Criteria value.
 *
 * @param lagid     (IN)  LAG Identifier
 *                        when PSC is global, use CBX_LAG_ALL
 * @param psc       (IN)  PSC object
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_lag_psc_set ( cbx_portid_t lagid, cbx_lag_psc_t *psc );


/**
 * LAG reseed
 * This routine is used to re-seed the Port Selection Criteria value.
 *
 * @param lagid     (IN)  LAG Identifier
 *                        when PSC is global, use CBX_LAG_ALL
 * @param mode      (IN)  LAG Reseed mode
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_lag_psc_reseed ( cbx_portid_t lagid, cbx_lag_reseed_mode_t mode );


/**
 * @}
 */


#ifdef __cplusplus
}
#endif

#endif
