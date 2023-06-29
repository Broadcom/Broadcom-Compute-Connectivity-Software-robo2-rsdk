/*
 * $Copyright: (c) 2017 Broadcom Limited.
 */

/**
 * @file
 * Coronado Bridge
 * Forwarding Services Abstraction Layer (FSAL) API
 */

#ifndef CBX_API_L2_H_
#define CBX_API_L2_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @defgroup cbx_l2 CBX L2
 *  Forwarding Services Abstraction Layer (FSAL) L2 Forwarding Database operations.
 *
 * <h3>L2 Programming</h3>
 * Avenger implemenation details
 *
 * The Address Resolution Logic Forwarding Map 0 and 1 (ARLFM0/1)
 *
 * src_mac    ->  ARLFM.mac_hi[15:0],mac_lo[31:0]  Ethernet MAC address.
 * vlanid     ->  ARLFM.fid[11:0]                  The Filter Identifier.
 *                                                     IVL: set fid = VSI
 *                                                     SVL: set = 0
 * params.static     ->   ARLFM.valid          FM_Valid_e      STATIC
 * params.dest_type  ->   ARLFM.dst_type       FM_Dst_Type_e   DST_IS_MULTICAST
 *                        ARLFM.dst_type       FM_Dst_Type_e   DST_IS_INVALID
 *                        ARLFM.dst_type       FM_Dst_Type_e   DST_IS_N
 * params.portid     ->   ARLFM.dst            Multicast Dest Group
 * params.udp_ctrl   ->   ARLFM.upd_ctrl
 * params.fwd_ctrl   ->   ARLFM.fwd_ctrl
 *
 * <h3>Port Group Learn Limit</h3>
 * @note ARL learn limit is provided per Port Group
 * - PGLLT  learn_limit[13:0]  Maximum number of MACs to be learned
 *
 *  @{
 */

/**
 * @typedef cbx_l2_t
 * CBX l2
 */

/**
 * @def CBX_L2_INVALID
 * Invalid L2
 */
#define CBX_L2_INVALID   0xffffffff

/** @def CBX_L2_MAX
 * Maximum L2 entries
 */
#define CBX_L2_MAX  -1

/** MAC-based security
 *
 */
typedef enum cbx_l2_addr_ctrl_e {
    cbxL2AddrCtrlNormal,    /**< Normal forwarding */
    cbxL2AddrCtrlSrcDrop,   /**< Drop if match on source lookup */
    cbxL2AddrCtrlCopyCPU,   /**< Forward per destination and copy to CPU */
    cbxL2AddrCtrlSrcDropCopyCPU,  /**< Drop and copy to CPU */
} cbx_l2_addr_ctrl_t;

/** Station Movement
 *
 */
typedef enum cbx_l2_addr_movement_e {
    cbxL2AddrMovementUpdate,     /**< update the ARL entry's assoc data */
    cbxL2AddrMovementUpdateCopy, /**< update the ARL entry's assoc data and copy CPU */
    cbxL2AddrMovementCopy,       /**< copy CPU */
    cbxL2AddrMovementDisable,    /**< disable movement */
} cbx_l2_addr_movement_t;

/** Destination Type
 *
 */
typedef enum cbx_l2_addr_dest_type_e {
    cbxL2AddrDestDrop,           /**< Destination Drop packet */
    cbxL2AddrDestMcast,          /**< Destination type Multicast */
    cbxL2AddrDestDLI,            /**< Destination type (LIN) Logical Interface */
    cbxL2AddrDestDPG,            /**< Destination type (PV) Port Group */
} cbx_l2_addr_dest_type_t;

/** @{
 * L2 Address Flags
 */
#define  CBX_L2_ADDR_STATIC      0x00000001  /**< Destination is static */
#define  CBX_L2_ADDR_ECID        0x00000002  /**< E-CID Key */
/*  @} */

/**
 * L2 entry parameter struct
 * l2 address parameters
 */
typedef struct cbx_l2_param_s {
    uint32_t                 flags;      /**< Flags:  CBX_L2_ADDR_XX */
    cbx_l2_addr_dest_type_t  dest_type;  /**< type of portid or drop */
    cbx_portid_t             portid;     /**< Port Identifier:
                                              corresponding to dest_type above
                                             - Multicast Dest Group
                                             - Unicast output Port Group (DPG)
                                             - Unicast output Logical Interface (DLI) */
    cbx_l2_addr_movement_t upd_ctrl;     /**< Address movement update control */
    cbx_l2_addr_ctrl_t     fwd_ctrl;     /**< Control field for MAC-based security */
    uint32_t               e_cid;        /**< E-channel Identifier */
} cbx_l2_param_t;

/**
 * L2 Entry struct
 * information about a l2 entry
 */
typedef struct cbx_l2_entry_s {
    eth_mac_t        src_mac;   /**< MAC address. */
    cbx_vlanid_t     vlanid;    /**< Vlan Identifier */
    cbx_l2_param_t   params;    /**< l2 parameters */
} cbx_l2_entry_t;


/**
 * L2 Address add
 * This routine is used to add a L2 Address to the forwarding database.
 *
 * @param l2entry   (IN)  L2 Address structure
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_l2_entry_add( cbx_l2_entry_t *l2entry );

/**
 * L2 Address remove
 * This routine is used to remove a L2 Address to the forwarding database.
 *
 * @param l2entry   (IN)  L2 Address structure
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_l2_addr_remove ( cbx_l2_entry_t *l2entry );


/**
 * L2 Address lookup
 * This routine is used to find a L2 Address entry that matches MAC and VLAN and
 * update the l2 entry structure passed.
 *
 * @param l2entry   (IN/OUT)  L2 Address structure
 *
 * @return return code
 * @retval CBX_E_NONE      Success
 * @retval CBX_E_NOT_FOUND Not Found
 * @retval CBX_E_XXXX      Failure
 */

int cbx_l2_lookup ( cbx_l2_entry_t *l2entry );

/**
 * L2 Address lookup next
 * This routine is used to find the next L2 Address entry in the database.
 * This provides the ability to walk the database.
 * If the first parameter is NULL, then the first entry in the database is returned.
 * If not NULL, the first parameter must be a valid entry in the database.
 *
 * @param l2entry        (IN)  L2 Address structure
 * @param l2entry_next   (OUT) L2 Address structure
 *
 * @return return code
 * @retval CBX_E_NONE      Success
 * @retval CBX_E_NOT_FOUND Next entry not found
 * @retval CBX_E_EMPTY     Database empty (entry not found when first parameter NULL)
 * @retval CBX_E_XXXX      Failure
 */

int cbx_l2_lookup_next ( cbx_l2_entry_t *l2entry,
                         cbx_l2_entry_t *l2entry_next );


/**
 * ARL aging timer set
 * ARL aging is automatic.
 *  The hardware will automatically delete non-static entries that have
 *  been aged out. The aging process of the ARL is controlled by the
 *  ARL Aging Process Configuration register.
 *
 * - value of zero disables aging
 *
 * @param age_seconds  (IN)  age time in seconds
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_l2_age_timer_set ( int age_seconds );


/**
 * ARL aging timer get
 *
 * @param age_seconds  (OUT) age time in seconds
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_l2_age_timer_get ( int *age_seconds );


/**
 * Reseed of ARL hash
 * Application control to reseed the ARL hash
 *
 * @param void
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_l2_reseed (void);

/**
 * Flush ARL entries
 * - For Independent VLAN Learing, flush all entires on the VLAN
 * - For Single VLAN Learining, all ARL entries are flush
 *
 * @param vlanid       (IN)  Vlan Identifier
 *                           when vlanid=0, then flush all ARL entries
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_l2_flush ( cbx_vlanid_t vlanid);

/**
 * Flush ARL entries on a given portid
 * - Flush all entries learned on the given portid.
 * - To flush all the entries learned on all the ports,
 *   use cbx_l2_flush API with vlanid = 0
 *
 * @param portid       (IN)  portid on which ARL entries to be deleted
 *                           This may represent a LAG or a physical port.
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_l2_port_flush ( cbx_portid_t portid);

/**
 * @}
 */


#ifdef __cplusplus
}
#endif

#endif
