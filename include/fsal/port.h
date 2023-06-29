/*
 * $Copyright: (c) 2017 Broadcom Limited.
 */

/**
 * @file
 * Coronado Bridge
 * Forwarding Services Abstraction Layer (FSAL) API
 * CBX PORT
 */

#ifndef CBX_API_PORT_H_
#define CBX_API_PORT_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @defgroup cbx_port CBX PORT
 * Forwarding Services Abstraction Layer (FSAL) Port operations.
 *
 * Ports are the basic building block of a network switch.
 * Ports can take several forms:
 * - Physical Port, represented by a front panel interface for example
 * - Port Group (LAG), a collection of one or more interfaces in a link aggregation
 * - Logical Interface, a matched subset of a physical interface (typically based on vlan tag)
 *
 * The Port APIs provide control over the port configurations such as:
 * - Port mode (Customer, Provider, Host,  Cascade,  Extender)
 * - parameters (default vlan, traffic class, drop precedence)
 * - attributes (Speed, Duplex, MTU)
 * - security and isolation

 * PORT Programming
 * Avenger implemenation details of cbx_port_create()
 * Physical Port to Port Group (PP2PG) Table
 * - when a port is created, its mapping to port group is also created
 *   this mapping will initially be 1:1 but will be dynamic based on
 *   the addition and removed of LAGs.
 * Port Group Table (PGT)
 * - the cbx_port_param_t are Software defined parameters in the PGT.pepper
 *   these pepper fields coorspond to the configuration of the SLIC TCAM
 *   and the associated context mapping as well as the Hardware fields of the PGT
 *
 *  @{
 */

/**
 * @typedef cbx_portid_t
 * Port Identifier
 * - Physical Port
 * - Port Group (lag)
 * - Logical Interface (vlan_port)
 */

/**
 * @def CBX_PORT_INVALID
 * Invalid PORT
 */
#define CBX_PORT_INVALID   0xffffffff

/** @def CBX_PORT_MAX
 * Maximum Port entries
 */
#define CBX_PORT_MAX  32

/** @{
 * Port Attribute Flags
 */
#define  CBX_PORT_DUPLEX_HALF        0
#define  CBX_PORT_DUPLEX_FULL        1

#define  CBX_PORT_SPEED_10           10
#define  CBX_PORT_SPEED_100          100
#define  CBX_PORT_SPEED_1000         1000
#define  CBX_PORT_SPEED_2500         2500
#define  CBX_PORT_SPEED_10000        10000

#define  CBX_PORT_STATUS_LINK_DOWN   0
#define  CBX_PORT_STATUS_LINK_UP     1

#define  CBX_PORT_FRAME_SIZE_MIN     64
#define  CBX_PORT_FRAME_SIZE_MAX     9720

#define  CBX_PORT_AUTO_NEGOIATE_FALSE    0
#define  CBX_PORT_AUTO_NEGOIATE_TRUE     1

#define  CBX_PORT_ADMIN_DISABLE          0
#define  CBX_PORT_ADMIN_ENABLE           1

#define  CBX_PORT_PHY_EEE_NONE           0
#define  CBX_PORT_PHY_EEE_802_3          1
#define  CBX_PORT_PHY_EEE_AUTO           2

#define  CBX_PORT_PHY_MEDIA_TYPE_FIBER   0
#define  CBX_PORT_PHY_MEDIA_TYPE_COPPER  1


/** @} */

/**
 * Port Type
 */
typedef enum cbx_port_type_e {
    cbxPortTypeCustomer,
    cbxPortTypeProvider,
    cbxPortTypeHost,
    cbxPortTypeCascade,
    cbxPortTypeExtender,
    cbxPortTypeExtenderCascade,
    cbxPortTypeExtenderUpstream,
    cbxPortTypePortVLAN,
    cbxPortTypeMax,   /**<  Max value -  not to be used */
    cbxPortTypeInvalid = 0xffffffff  /**< Added for CINT workaround - value - not to be used */
} cbx_port_type_t;

/**
 * Port SLI
 * - mapping to the PGT.vid_src (VID_Source_e) field
 *   for which tag (C or S) is to provide the VID for that mapping
 * - in the case of Novid
 *   the vlan tag is ignored and the default_vid is used
 */
typedef enum cbx_port_sli_mode_e {
    cbxPortSliPvCtag,
    cbxPortSliPvStag,
    cbxPortSliPvNovid,
    cbxPortSliPvMax,  /**<  Max value -  not to be used */
    cbxPortSliPvInvalid = 0xffffffff  /**< Added for CINT workaround - value - not to be used */
} cbx_port_sli_mode_t;

/**
 * Port Admission Control
 * PG2AP ap[1:0]  Maps the port group to an admission control policy
 * (Admission Profile) including buffer usage thresholds, WRED, etc.
 */
typedef enum cbx_cosq_threshold_e {
    cbxPortAdmissionControlPolicy0,
    cbxPortAdmissionControlPolicy1,
    cbxPortAdmissionControlPolicy2,
    cbxPortAdmissionControlPolicy3,
    cbxPortAdmissionControlPolicyMax,  /**<  Max value -  not to be used */
    cbxPortAdmissionControlPolicyInvalid = 0xffffffff /**< Added for CINT workaround - last value - not to be used */
} cbx_cosq_threshold_t;

/** @{
 * Port Flags
 */
#define  CBX_PORT_VIRTUAL_PORTS_ENABLE        0x00000001
#define  CBX_PORT_DROP_UNTAGGED               0x00000002
#define  CBX_PORT_DROP_TAGGED                 0x00000004
#define  CBX_PORT_DROP_ULF                    0x00000010
#define  CBX_PORT_DROP_MLF                    0x00000020
#define  CBX_PORT_DROP_SLF                    0x00000040
#define  CBX_PORT_DROP_BCAST                  0x00000080
/** @} */

/**
 * PORT parameter structure
 * information about a port
 */
typedef struct cbx_port_params_t {
    uint32_t         flags;            /**< Flags:  CBX_PORT_XX */
    cbx_port_type_t  port_type;        /**< port type */
    int              port_index;       /**< port index based on device */
    cbx_port_sli_mode_t  sli_mode;     /**< Source Logical Interface mode */
    cbx_cosq_threshold_t ac_policy;    /**< Admission Control policy */
    cbx_vlan_t       default_vid;      /**< default vid */
    int              default_tc;       /**< default traffic class */
    int              default_dp;       /**< default drop precedence */
    int              learn_limit;      /**< Maximum number of MACs to be learned */
    int              mtu;              /**< MTU for egress port */
    int              port_group;       /**< Global Port Group index (read-only) */
} cbx_port_params_t;

/**
 * Initialize Port entry parameters.
 *
 * @param  port_params  parameter object
 */
void cbx_port_params_t_init( cbx_port_params_t *port_params );

/**
 * Port Lookup
 * - returns portid and port_group based on port_index
 *
 */
typedef struct cbx_port_lookup_t {
    int              port_index;       /**< port index based on device */
} cbx_port_lookup_t;

/**
 * Port Isolation Group
 */
typedef enum cbx_port_isolation_group_e {
    cbxPortIsolation_0,  /**<  promiscuous port group  */
    cbxPortIsolation_1,  /**<  community port group 1  */
    cbxPortIsolation_2,  /**<  community port group 2  */
    cbxPortIsolation_3,  /**<  isolated port group */
    cbxPortIsolationMax,  /**<  Max value - not to be used */
    cbxPortIsolation_Invalid = 0xffffffff /**< CINT workaround - last value - not to be used */
} cbx_port_isolation_group_t;

/**
 * Port Isolation Group
 * PG2IG  sig[1:0]  Maps the Port Group to a Isolation Group
 *
 * Each VLAN has an isolation map which defines the Isolation Groups
 * that must be enforced for that VLAN.
 * If the Source PG and the Destination PG are in the same Isolation Group
 * and that Isolation Group is enabled for the VLAN,
 * then the frame is filtered/dropped.
 *
 */
typedef struct cbx_port_isolation_t {
    cbx_port_isolation_group_t group;
} cbx_port_isolation_t;


/**
 *  Port Attribute types
 */
typedef enum cbx_port_attr_e {
    /** Port duplex mode */
    cbxPortAttrDuplex,

    /** Port speed */
    cbxPortAttrSpeed,

    /** Port MTU - frame max size */
    cbxPortAttrMTU,

    /** Port link status */
    cbxPortAttrLinkStatus,

    /** Port auto-negotiation */
    cbxPortAttrAutoNegotiate,

    /** Port administration enable/disable */
    cbxPortAttrAdmin,

    /** Port Phy EEE mode */
    cbxPortPhyEEE,

    /** Port phy media type  -Fiber or copper */
    cbxPortPhyMediaType,

    /** Last value - not to be used */
    cbxPortAttrMax
} cbx_port_attr_t;


/**
 * Create a port
 * This routine is used to create a port interface.
 * It allocates an entry in the Port Group Table (PGT)
 * It sets this IPP Table:
 *  - Physical Port to Port Group (PP2PG) table
 * It sets these PQM Tables:
 *  - Port Group to Physical Port Fan-Out Vector (PG2PPFOV) table
 *  - Physical Port to Port Group Identifier (PP2PGID) table
 *  - Squelch Vector Table (SVT)
 * These tables are also used by the LAG module.
 * The set of Ports and LAGs make up the Global Port Number space
 *
 * Port Group Table has cosq and mirror parameters that are
 * configured in the cosq and mirror modules.
 *
 * Port Group Table has default SLI:
 *  default_sli[17:0]   Default Source Logical Interface.
 *  the API for the L2 Forwarding uses default vid, the
 *  SLI is type PV and is built as per:
 *     Type=1, SPG[4:0], VSI[11:0]
 *
 * @param port_params   (IN)  Port parameters, including:
 *                            - default vid
 *                            - default traffic class
 *                            - default drop presedence
 *                            - MTU for egress port
 * @param portid        (OUT) Handle of the port created
 *                            CBX_PORT_INVALID: Port could not be created
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_port_create ( cbx_port_params_t *port_params,
                      cbx_portid_t      *portid );

/**
 * Destroy a port previously created by cbx_port_create()
 *
 * @param portid    (IN)  Handle of the port to be destroyed
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 *
 * @note This routine destroys a previously created Port.
 */

int cbx_port_destroy ( cbx_portid_t portid );

/**
 * PORT Information get
 * This routine is used to get the parameters of a Port.
 *
 * @param portid       (IN)  PORT Identifier
 * @param port_params  (OUT) Port parameters, including:
 * - port type
 * - drop tagged packets
 * - drop untagged packets
 * - default vid
 * - default traffic class
 * - default drop presedence
 * - MTU for egress port
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_port_info_get ( cbx_portid_t       portid,
                        cbx_port_params_t *port_params );

/**
 * PORT Information set
 * This routine is used to set the parameters of a Port.
 *
 * @param portid       (IN)  PORT Identifier
 * @param port_params  (IN)  Port parameters, including:
 * - port type
 * - drop tagged packets
 * - drop untagged packets
 * - default vid
 * - default traffic class
 * - default drop presedence
 * - MTU for egress port
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_port_info_set ( cbx_portid_t       portid,
                        cbx_port_params_t *port_params );

/**
 * PORT Lookup
 * This routine is used to get the Port Identifier matching lookup criteria.
 *
 * @param lookup     (IN)  port lookkup criteria
 * @param portid     (OUT) PORT Identifier
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_port_lookup ( cbx_port_lookup_t *lookup,
                      cbx_portid_t      *portid );

/**
 * PORT isolation get
 * This routine is used to get the isolation group on a port.
 *
 * @param portid     (IN)  PORT Identifier
 * @param port_iso   (OUT) Isolation group
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_port_isolation_get ( cbx_portid_t          portid,
                             cbx_port_isolation_t *port_iso);

/**
 * PORT isolation set
 * This routine is used to set the isolation group on a port.
 *
 * @param portid     (IN)  PORT Identifier
 * @param port_iso   (IN)  Isolation group
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_port_isolation_set ( cbx_portid_t          portid,
                             cbx_port_isolation_t *port_iso);


/**
 * PORT driver extensions
 * - these APIs are not a proper part of the Forwarding Services Abstraction Layer
 *   they interface to the MAC and/or PHY rather than the Coronado Bridge core
 *   provided here for reference.
 * - these APIs can/should confirm to the "cbx_port" API conventions
 */

/**
 * PORT attribute get
 * This routine is used to get an attribute value on a port.
 *
 * @param portid     (IN)  PORT Identifier
 * @param attr       (IN)  attribute type
 * @param value      (OUT) attributevalue
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_port_attribute_get ( cbx_portid_t     portid,
                             cbx_port_attr_t  attr,
                             uint32_t         *value );

/**
 * PORT attribute set
 * This routine is used to set an attribute value on a port.
 *
 * @param portid     (IN)  PORT Identifier
 * @param attr       (IN)  attribute type
 * @param value      (IN)  attribute value
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_port_attribute_set ( cbx_portid_t     portid,
                             cbx_port_attr_t  attr,
                             uint32_t         value );


/**
 * PORT PHY set
 * This routine is used to set a port phy register.
 *
 * @param portid        (IN)  PORT Identifier
 * @param flags         (IN)  Flags
 * @param phy_reg_addr  (IN)  Register address
 * @param phy_data      (OUT) data
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */
int cbx_port_phy_get ( cbx_portid_t portid,
                       uint32_t flags,
                       uint32_t phy_reg_addr,
                       uint32_t *phy_data );

/**
 * PORT PHY set
 * This routine is used to get a port phy register.
 *
 * @param portid        (IN)  PORT Identifier
 * @param flags         (IN)  Flags
 * @param phy_reg_addr  (IN)  Register address
 * @param phy_data      (IN)  data
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */
int cbx_port_phy_set ( cbx_portid_t portid,
                       uint32_t flags,
                       uint32_t phy_reg_addr,
                       uint32_t phy_data );


/**
 * @}
 */


#ifdef __cplusplus
}
#endif

#endif
