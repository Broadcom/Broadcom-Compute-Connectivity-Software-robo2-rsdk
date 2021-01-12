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

#ifndef CBX_API_CFP_H_
#define CBX_API_CFP_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @defgroup cbx_cfp CBX CFP
 * Forwarding Services Abstraction Layer (FSAL) Content Filter Processor operations.
 *
 * The Compact Filter Processor identifies specific flows and
 * provides a range of actions when a key is matched.
 *
 * CFP search: 1024x192b TCAM.
 * Up to four keys are presented to the TCAM over four consecutive search itterations.
 * Each itteration can produce a match result and if multiple entries match in an
 *    itteration, then the lowest rule number is used.
 *
 * CFP Rules
 *
 *
 * CFP Actions
 * The Compact Filter Processor provides actions such as:
 *   - mirror ingress
 *   - changing the destination
 *   - copying to CPU
 *   - updating PCP/DSCP/TC
 *
 * There two groups of actions.
 *  1) Actions that do not depend on the metering result.
 *  2) Actions that depend on the metering result:
 *      in-profile (green) actions
 *      out-of-profile (yellow or red) actions
 *
 * CFP includes integrated meters and counters
 *
 * CFP scope
 * - Avenger provides Ingress Compact Filter Processor functionality
 *
 *  @{
 */

/**
 * @typedef cbx_cfpid_t
 * CBX cfp
 */
typedef uint32_t cbx_cfpid_t;

/**
 * @def CBX_CFP_INVALID
 * Invalid CFP
 */
#define CBX_CFP_INVALID   0xffffffff


/**
 * CFP Stage
 */
typedef enum cbx_cfp_stage_e {
    /** Ingress Stage */
    cbxCfpStageIngress,
    /** Egress Stage */
    cbxCfpStageEgress,
} cbx_cfp_stage_t;


/** CFP Key type
 * There are four keys defined for Avenger CFP.
 */
typedef enum cbx_cfp_key_type_e {
    /** L2 Ethernet ACL*/
    cbxCfpKeyL2EthernetACL,

    /** IPv4 ACL */
    cbxCfpKeyIPv4ACL,

    /** IPV6 ACL */
    cbxCfpKeyIPv6ACL,

    /** L2 / IPv4 ACL */
    cbxCfpKeyL2IPv4ACL,

    /** UDF ACL */
    cbxCfpKeyUDFACL,

    /*  ETAG CFP */
    cbxCfpKeyEtagACL,
} cbx_cfp_key_type_t;


/* FIXME to be defined (types.h) */
typedef uint16_t etype_t;
typedef uint16_t l4_port_t;
typedef uint8_t  v4_addr_t[4];
typedef uint8_t  v6_addr_t[16];


#if 0
Key:
L2 Ethernet {
    PARSE_STATUS  mask
    RANGE_RESULT  mask
    cbx_vlan_t   vlan;
    Outer vlan exact
    int          pcp;
    Outer pcp  exact
    cbx_vlan_t   inner_vlan;
    Inner vlan exact
    eth_mac_t    src_mac;
    L2 DMAC  exact
    eth_mac_t    src_mac;
    L2 SMAC  exact
    etype_t      etype;
    Ethertype  exact
}

Key:
IPv4 ACL {
    PARSE_STATUS  mask
    L3_STATUS  mask
    RANGE_RESULT  mask
    cbx_vlan_t   vlan;
    Outer vlan exact
    v4_addr_t    ipv4_sa;
    L3 SA  LPM
    v4_addr_t    ipv4_da;
    L3 DA  LPM
    int          tos;
    L3 TOS  mask
    l4_port_t    dport;
    PORT_IP_PROTO  exact
    l4_port_t    sport;
    SPORT  exact
    int          l4_flags;
    TCP_FLAGS  mask
}

Key:
IPv6 ACL {
    PARSE_STATUS  mask
    L3_STATUS  mask
    RANGE_RESULT  mask
    cbx_vlan_t  vlan;
    Outer vlan exact
    v6_addr_t   ipv4_sa;
    L3 SA  LPM
    v6_addr_t   ipv4_da;
    L3 DA  LPM
    int         tos;
    L3 TOS  exact
    l4_port_t   dport;
    DPORT_IP_PROTO  exact
    l4_port_t   sport;
    SPORT  exact
    int         l4_flags;
    TCP_FLAGS  mask
}

Key:
L2/IPV4 ACL {
    PARSE_STATUS  mask
    L3_STATUS  mask
    RANGE_RESULT  mask
    cbx_vlan_t  vlan;
    OUTER_VID  exact
    eth_mac_t   src_mac;
    DMAC  exact
    eth_mac_t   src_mac;
    SMAC  exact
    v4_addr_t   ipv4_sa;
    L3SA  LPM
    v4_addr_t   ipv4_da;
    L3DA  LPM
    int         tos;
    L3TOS  exact
    l4_port_t   dport;
    DPORT_IP_PROTO  exact
    l4_port_t   sport;
    SPORT  exact
    int         l4_flags;
    TCP_FLAGS  mask
}

#endif


/**
 * CFP struct
 * information about a cfp entry
 */
typedef struct cbx_cfp_params_t {
    cbx_cfp_stage_t  stage;  /**< CFP Stage - Ingress for Avenger */
} cbx_cfp_params_t;


/** @{
 * CFP Action Flags
 */
#define  CBX_CFP_ACTION_OVERRIDE                 0x00000001  /**< Override lower priority matches
                                                              (do not merge them into final actions */
#define  CBX_CFP_ACTION_UPDATE_DP                0x00000002  /**< change Drop Precedence */
#define  CBX_CFP_ACTION_DLF                      0x00000004  /**< ignore drop_ulf and drop_mlf */
#define  CBX_CFP_ACTION_LOOP_ENABLE              0x00000008  /**< allow packet to loopback to source */
#define  CBX_CFP_ACTION_IN_POLICY_UPDATE_PRI     0x00000010  /**< in policy: change Traffic Class priority */
#define  CBX_CFP_ACTION_OUT_POLICY_UPDATE_PRI    0x00000020  /**< out policy: change Traffic Class priority */
#define  CBX_CFP_ACTION_IN_POLICY_LEARN_DISABLE  0x00000040  /**< in policy: disable learning for this packet */
#define  CBX_CFP_ACTION_OUT_POLICY_LEARN_DISABLE 0x00000080  /**< out policy: disable learning for this packet */

/**< Used for CFP action trap id */
#define CBX_CFP_ACTION_DROP_ALL                          1
#define CBX_CFP_ACTION_TRAP_TO_CPU                       2
#define CBX_CFP_ACTION_TRAP_TO_CPU_AND_FORWARD           3


/** @} */

/** @{
 * CFP Attribute number
 * Defines the maximum number of valid attributes for different key types.
 * Used for allocation of memory for attr_selector field passed to
 * cbx_cfp_attr_selector_init function for prefilling list of qualifiers
 * that are valid for a key type
 */
#define CBX_CFP_L2_ETHERNET_ATTR_MAX   16  /**< L2 Ethernet key */
#define CBX_CFP_IPV4_ATTR_MAX          21  /**< IPv4 key */
#define CBX_CFP_IPV6_ATTR_MAX          23  /**< IPv6 key with addr as 2 64 bits attributes */
#define CBX_CFP_L2_IPV4_ATTR_MAX       23  /**< L2 and IPv4 Key */
#define CBX_CFP_UDF_ATTR_MAX           18  /**< UDF Key */
#define CBX_CFP_ETAG_ATTR_MAX           5  /**< ETAG Key */
/** @} */

/** @{
 * CFP Vlan Tag Status values
 */

/**< Used for cbxCfpRuleAttrOuterVlanTagStatus value */
#define CBX_CFP_ATTR_OUTER_VLAN_NOT_PRESENT     0x0
#define CBX_CFP_ATTR_OUTER_VLAN_PRIORITY_TAGGED 0x1
#define CBX_CFP_ATTR_OUTER_VLAN_SINGLE_TAG      0x2

/**< Used for cbxCfpRuleAttrInnerVlanTagStatus value */
#define CBX_CFP_ATTR_INNER_VLAN_NOT_PRESENT     0x0
#define CBX_CFP_ATTR_INNER_VLAN_PRIORITY_TAGGED 0x1
#define CBX_CFP_ATTR_INNER_VLAN_SINGLE_TAG      0x2

#define CBX_CFP_ATTR_INNER_VLAN_PRESENT 0x0001
#define CBX_CFP_ATTR_MIM_TAG_PRESENT    0x0001
#define CBX_CFP_ATTR_E_TAG_PRESENT      0x0001
#define CBX_CFP_ATTR_MPLS_TAG_PRESENT   0x0001
#define CBX_CFP_ATTR_PPP_HEADER_PRESENT 0x0001

#define CBX_CFP_ATTR_ETHERTYPE_LLC_SSAP_DDAP 0x0
#define CBX_CFP_ATTR_ETHERTYPE_DIX           0x1

/**< Used for cbxCfpRuleAttrL3Type */
#define CBX_CFP_L3_TYPE_RAW       0x0
#define CBX_CFP_L3_TYPE_LLC       0x1
#define CBX_CFP_L3_TYPE_IPV4      0x2
#define CBX_CFP_L3_TYPE_IPV6      0x3
#define CBX_CFP_L3_TYPE_UC_MPLS   0x4
#define CBX_CFP_L3_TYPE_MC_MPLS   0x5
#define CBX_CFP_L3_TYPE_PTP       0x6
#define CBX_CFP_L3_TYPE_ARP       0x7

/**< Used for cbxCfpRuleAttrL4Type value */
#define CBX_CFP_L4_TYPE_UNKNOWN  0x0
#define CBX_CFP_L4_TYPE_TCP      0x1
#define CBX_CFP_L4_TYPE_UDP      0x2
#define CBX_CFP_L4_TYPE_ICMP     0x3
#define CBX_CFP_L4_TYPE_IGMP     0x4

/**< Used for cbxCfpRuleAttrL3TtlSummary value */
#define CBX_CFP_L3_TTL_ZERO     0x0
#define CBX_CFP_L3_TTL_ONE      0x1
#define CBX_CFP_L3_TTL_OTHER    0x2
#define CBX_CFP_L3_TTL_255      0x3

/**< Used for cbxCfpRuleAttrL3FragSummary value */
#define CBX_CFP_L3_FRAG_MIDDLE  0x0
#define CBX_CFP_L3_FRAG_LAST    0x1
#define CBX_CFP_L3_FRAG_FIRST   0x2
#define CBX_CFP_L3_FRAG_NONE    0x3

/**< Used for cbxCfpRuleAttrTcpControl bitmap */
#define CBX_CFP_TCP_CONTROL_FIN 0x01
#define CBX_CFP_TCP_CONTROL_SYN 0x02
#define CBX_CFP_TCP_CONTROL_RST 0x04
#define CBX_CFP_TCP_CONTROL_PSH 0x08
#define CBX_CFP_TCP_CONTROL_ACK 0x10
#define CBX_CFP_TCP_CONTROL_URG 0x20


/** @} */

/**
 * CFP Count Mode enumerations
 */
typedef enum cbx_cfp_count_mode_e {
    cbxCFPCountPacketsBytes,         /**< count packets and bytes */
    cbxCFPCountPacketsByProfile      /**< count packets in and out of profile */
} cbx_cfp_count_mode_t;

/**
 * CFP Stat enumerations
 */
typedef enum cbx_cfp_stat_e {
    cbxCFPStatBytes,
    cbxCFPStatPackets,
    cbxCFPStatPacketsInProfile,
    cbxCFPStatPacketsOutProfile
} cbx_cfp_stat_t;

/**
 * CFP Forward Mode enumerations
 */
typedef enum cbx_cfp_forward_mode_e {
    cbxCFPForwardNoChange,       /**< do not change forwarding */
    cbxCFPForwardLoopback,       /**< forward looping back to source port */
    cbxCFPForwardUnicast,        /**< forward to Unicast destination */
    cbxCFPForwardMulticast,      /**< forward to Multicast Group */
    cbxCFPForwardFrom,           /**< forward overriding source port */
    cbxCFPForwardDrop,           /**< drop packet */
    cbxCFPForwardOverrideDrop    /**< override any previous drop decision */
} cbx_cfp_forward_mode_t;

/**
 * CFP Attribute enumeration
 */
typedef enum cbx_cfp_rule_attribute_e {
    cbxCfpRuleAttrSourcePort,             /**< Source port                        F0  */
    cbxCfpRuleAttrIncomingTrafficClass,   /**< L2 Status -ingress traffic class   F0  */
    cbxCfpRuleAttrOuterVlanTagStatus,     /**< L2 Status - S tag status           F0  */
    cbxCfpRuleAttrInnerVlanTagStatus,     /**< L2 Status - C tag status           F0  */
    cbxCfpRuleAttrMimTagStatus,           /**< L2 Status - MiM tag status         F0  */
    cbxCfpRuleAttrETagStatus,             /**< L2 Status-Port Extender Tag status F0  */
    cbxCfpRuleAttrEtherTypeStatus,        /**< L2 Status -Ether Type DIX/SAP-SNAP F0  */
    cbxCfpRuleAttrMplsStatus,             /**< L2 Status - MPLS present/not       F0  */
    cbxCfpRuleAttrPppStatus,              /**< L2 Status - PPP present/not        F0  */
    cbxCfpRuleAttrL3Type,                 /**< L2 Status - L3 -type               F0  */

    cbxCfpRuleAttrRangeResult,            /**< Range result - 1 bit for each range check hit F1  */
    cbxCfpRuleAttrDestMac,                /**< Destination MAC          F4  */
    cbxCfpRuleAttrSrcMac,                 /**< Source MAC               F5  */
    cbxCfpRuleAttrOuterVlanId,            /**< VLAN id from outer C/S tag   F6  */
    cbxCfpRuleAttrInnerVlanId,            /**< Inner Vlan ID            F7  */
    cbxCfpRuleAttrEtherType,              /**< EtherType/DSAP/SSAP      F8  */

    cbxCfpRuleAttrL4Type,                 /**< L3 Status bits - L3 Payload         F3  */
    cbxCfpRuleAttrL3TtlSummary,           /**< L3 Status bits - L3 TTL             F3  */
    cbxCfpRuleAttrL3FragSummary,          /**< L3 Status bits - IP Fragment summaryF3  */

    cbxCfpRuleAttrSrcIp,                  /**< IPv4 Source IP           F9  */
    cbxCfpRuleAttrDestIp,                 /**< IPv4 Destination IP      F10 */
    cbxCfpRuleAttrDscp,                   /**< IPv4/V6 DSCP/TOS         F13 */
    cbxCfpRuleAttrL4SrcPort,              /**< TCP/UDP source port      F16 */
    cbxCfpRuleAttrL4DestPort,             /**< TCP/UDP destination port F16 */
    cbxCfpRuleAttrIcmpCode,               /**< ICMP code type           F16 */
    cbxCfpRuleAttrIgmpType,               /**< IGMP type                F16 */
    cbxCfpRuleAttrTcpControl,             /**< TCP control flags        F17 */

    cbxCfpRuleAttrSrcIp6,                 /**< IPv6 Source IP -128bits  F9 + F11 */
    cbxCfpRuleAttrSrcIp6High,             /**< IPv6 Source IP MS:64bits F9  */
    cbxCfpRuleAttrSrcIp6Low,              /**< IPv6 Source IP LS:64bits F11  */
    cbxCfpRuleAttrDestIp6,                /**< IPv6 Destination IP -128bits  F10 + F12 */
    cbxCfpRuleAttrDestIp6High,            /**< IPv6 Destination IP - MS 64 bits F10 */
    cbxCfpRuleAttrDestIp6Low,             /**< IPv6 Destination IP - LS 64 bits F12 */
    cbxCfpRuleAttrEtagICID,               /**< ETAG Ingress ECID value F21 */
    cbxCfpRuleAttrEtagECID,               /**< ETAG ECID value F22 */
    cbxCfpRuleAttrEtagGRP,                /**< ETAG GRP bits F23 */
    cbxCfpRuleAttrUDF0,                   /**< UDF0 - F24 - 64 bits starting from the end of SMAC */
    cbxCfpRuleAttrUDF1,                   /**< UDF1 - F25 - 64 bits from the end of UDF0 */
    cbxCfpRuleAttrUDF2,                   /**< UDF2 - F26 - 32 bits from the end of UDF1  */
    cbxCfpRuleAttrUDF3,                   /**< UDF3 - F27 - 32 bits from the end of UDF2+16 */
    cbxCfpRuleAttrUDF4,                   /**< UDF4 - F28 - 16 bits from the end of UDF3 */
    cbxCfpRuleAttrUDF5,                   /**< UDF5 - F29 - 16 bits from the end of UDF4  */
    cbxCfpRuleAttrUDF6,                   /**< UDF6 - F30 - 16 bits from the end of UDF5  */
    cbxCfpRuleAttrUDF7,                   /**< UDF7 - F31 - 16 bits from the end of UDF6  */
    cbxCfpRuleAttrMax                     /**< Last value - not to be used */
} cbx_cfp_rule_attribute_t;

/**
 * CFP Packet Type
 * Type of the packet to decide which to be used for rule creation
 *  - cbxCfpRuleAttrL4SrcPort
 *  - cbxCfpRuleAttrL4DestPort
 *  - cbxCfpRuleAttrIcmpCode
 *  - cbxCfpRuleAttrIgmpType
 */
typedef enum cbx_cfp_l3_l4_pkt_type_e {
   cbxCfpL3L4PktTypeTcp,
   cbxCfpL3L4PktTypeUdp,
   cbxCfpL3L4PktTypeIcmp,
   cbxCfpL3L4PktTypeIgmp
} cbx_cfp_l3_l4_pkt_type_t;

/**
 * CFP Common action structure
 */
typedef struct cbx_cfp_common_action_t {
    cbx_meterid_t         meterid;     /**< Meter Identifier */
    int                   dp;          /**< Drop Precedence */
    cbx_cfp_count_mode_t  count_mode;  /**< Counter mode */
} cbx_cfp_common_action_t;

/**
 * CFP Policy based action structure
 */
typedef struct cbx_cfp_policy_action_t {
    cbx_cfp_forward_mode_t  forward_mode; /**< Change forwarding */
    cbx_portid_t            ucastid;      /**< Fwd mode - Unicast interface */
    cbx_mcastid_t           mcastid;      /**< Fwd mode - Multicast Identifier */
    cbx_portid_t            sourceid;     /**< Fwd mode - Source Port */
    int                     pri;          /**< Traffic Class - priority */
    cbx_trapid_t            trapid;       /**< Trap ID */
    cbx_mirrorid_t          mirrorid;     /**< Mirror Identifier */
} cbx_cfp_policy_action_t;

/**
 * CFP action structure
 */
typedef struct cbx_cfp_action_t {
    uint32_t                 flags;             /**< Flags:  CBX_CFP_ACTION_XX */
    cbx_cfp_common_action_t  common_action;     /**< Common Actions */
    cbx_cfp_policy_action_t  in_policy_action;  /**< In policy actions */
    cbx_cfp_policy_action_t  out_policy_action;  /**< Out of policy actions */
} cbx_cfp_action_t;

/**
 * CFP Attribute Selector structure
 */
typedef struct cbx_cfp_rule_attr_selector_s {
    cbx_cfp_rule_attribute_t attr;       /**< Attribute name */
    uint8_t                  attr_len;   /**< length of the attribute value in bytes */
    uint8_t                 *attr_val;   /**< Value of the attribue */
    uint8_t                 *attr_mask;  /**< Mask */
} cbx_cfp_rule_attr_selector_t;

/**
 * CFP Rule structure
 */
typedef struct cbx_cfp_rule_s {
    cbx_cfp_key_type_t            key_type;        /**< L2/IPv4/IPv6/L2-IPv4/UDF */
    uint32_t                      num_attributes;  /**< Number of rule attributes */
    cbx_cfp_rule_attr_selector_t  *attr_selector;   /**< Array of rule attributes */
} cbx_cfp_rule_t;
/**
 * Initialize CFP Action object
 *
 * @param  action     action object
 */
void cbx_cfp_action_t_init (cbx_cfp_action_t *action);


/**
 * CFP Initialization
 * This routine is used to initialize a CFP
 *
 * @param  cfp_params   (IN)  CFP parameters.
 * @param  cfpid        (OUT) Handle of the cfp
 *                           CBX_CFP_INVALID: CFP could not be created
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_cfp_init ( cbx_cfp_params_t *cfp_params,
                   cbx_cfpid_t      *cfpid );


/**
 * Detach a cfp previously initialized by cbx_cfp_init()
 *
 * @param  cfpid    (IN)  Handle of the cfp to be destroyed
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 *
 * @note This routine destroys a previously initialized cfp.
 */

int cbx_cfp_detach ( cbx_cfpid_t cfpid );

/**
 * Initialize CFP Rule object
 * Prefill attribute array in the Rule with initialized values and masks
 *
 * @param  pkt_type   L3/L4 packet type
 * @param  rule       rule object
 */
void cbx_cfp_rule_t_init( cbx_cfp_l3_l4_pkt_type_t pkt_type,
                          cbx_cfp_rule_t *rule );

/**
 * CFP Rule set
 * This routine is used to set a CFP rule
 *
 * @param cfpid    (IN)  CFP Identifier
 * @param rule     (IN)  Rule object
 * @param index    (IN)  Rule Index
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_cfp_rule_set (cbx_cfpid_t     *cfpid,
                      cbx_cfp_rule_t  *rule,
                      uint32_t         index );

/**
 * CFP Rule clear
 * This routine is used to clear a CFP rule
 *
 * @param cfpid    (IN)  CFP Identifier
 * @param index    (IN)  Rule Index
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_cfp_rule_clear ( cbx_cfpid_t    *cfpid,
                         uint32_t        index );

/**
 * CFP Action set
 * This routine is used to set a CFP action
 *
 * @param cfpid    (IN)  CFP Identifier
 * @param action   (IN)  Action object
 * @param index    (IN)  Action Index
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */
int cbx_cfp_action_set ( cbx_cfpid_t       *cfpid,
                         cbx_cfp_action_t  *action,
                         uint32_t          index );

/**
 * CFP Action clear
 * This routine is used to clear a CFP action
 *
 * @param cfpid    (IN)  CFP Identifier
 * @param index    (IN)  Action Index
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_cfp_action_clear ( cbx_cfpid_t  *cfpid,
                           uint32_t      index );

/**
 * CFP Rule dump
 * This routine is used to dump a CFP rule
 *
 * @param cfpid    (IN)  CFP Identifier
 * @param index    (IN)  Rule Index
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

void cbx_cfp_rule_dump ( cbx_cfpid_t  *cfpid,
                         uint32_t      index );

/**
 * CFP Action dump
 * This routine is used to dump a CFP action
 *
 * @param cfpid    (IN)  CFP Identifier
 * @param index    (IN)  Action Index
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

void cbx_cfp_action_dump ( cbx_cfpid_t  *cfpid,
                           uint32_t      index);

/**
 * CFP Stat Multi Get
 * This routine is used to get the statistics of a given rule idx
 *
 * @param cfpid     (IN)  CFP Identifier
 * @param index     (IN)  Rule Index
 * @param nstat     (IN)  Number of statistics to be retrieved
 * @param stat_arr  (IN)  Type of statistics to be retrieved
 * @param value_arr (OUT) Statictics value
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */
int
cbx_cfp_stat_multi_get(cbx_cfpid_t  *cfpid,
                    uint32_t      index,
                    int nstat,
                    cbx_cfp_stat_t *stat_arr,
                    uint64 *value_arr);

/**
 * CFP Stat Multi Set
 * This routine is used to set the statistics of a given rule idx
 *
 * @param cfpid     (IN)  CFP Identifier
 * @param index     (IN)  Rule Index
 * @param nstat     (IN)  Number of statistics to be set
 * @param stat_arr  (IN)  Type of statistics to be set
 * @param value_arr (OUT) Statictics value
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */
int
cbx_cfp_stat_multi_set(cbx_cfpid_t  *cfpid,
                    uint32_t      index,
                    int nstat,
                    cbx_cfp_stat_t *stat_arr,
                    uint64 *value_arr);



/**
 * @}
 */


#ifdef __cplusplus
}
#endif

#endif
