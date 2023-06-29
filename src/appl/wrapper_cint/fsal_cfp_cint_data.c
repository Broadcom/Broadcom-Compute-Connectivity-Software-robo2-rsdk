/*
 *
 * $Copyright: (c) 2016 Broadcom Ltd
 * All Rights Reserved.$
 *
 * fsal_cfp_cint_data.c
 *
 * Hand-coded support for FSAL routines.
 */
int fsal_cfp_cint_data_not_empty;

#if defined(INCLUDE_LIB_CINT)
#include <sal_types.h>
#include <types.h>
#include <cint_config.h>
#include <cint_types.h>
#include <cint_porting.h>
#include <cfp.h>

CINT_FWRAPPER_CREATE_VP1(cbx_cfp_action_t_init,
                         cbx_cfp_action_t*, cbx_cfp_action_t,
                         action, 1, 0);

CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         cbx_cfp_init,
                         cbx_cfp_params_t*, cbx_cfp_params_t,
                         cfp_params, 1, 0,
                         cbx_cfpid_t*, cbx_cfpid_t,
                         cfpid, 1, 0);

CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         cbx_cfp_detach,
                         cbx_cfpid_t, cbx_cfpid_t,
                         cfpid, 0, 0);

CINT_FWRAPPER_CREATE_VP2(cbx_cfp_rule_t_init,
                         cbx_cfp_l3_l4_pkt_type_t, cbx_cfp_l3_l4_pkt_type_t,
                         pkt_type, 0, 0,
                         cbx_cfp_rule_t*, cbx_cfp_rule_t,
                         rule, 1, 0);

CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         cbx_cfp_rule_set,
                         cbx_cfpid_t*, cbx_cfpid_t,
                         cfpid, 1, 0,
                         cbx_cfp_rule_t*, cbx_cfp_rule_t,
                         rule, 1, 0,
                         uint32_t, uint32_t,
                         index, 0, 0);

CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         cbx_cfp_rule_clear,
                         cbx_cfpid_t*, cbx_cfpid_t,
                         cfpid, 1, 0,
                         uint32_t, uint32_t,
                         index, 0, 0);


CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         cbx_cfp_action_set,
                         cbx_cfpid_t*, cbx_cfpid_t,
                         cfpid, 1, 0,
                         cbx_cfp_action_t*, cbx_cfp_action_t,
                         action, 1, 0,
                         uint32_t, uint32_t,
                         index, 0, 0);

CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         cbx_cfp_action_clear,
                         cbx_cfpid_t*, cbx_cfpid_t,
                         cfpid, 1, 0,
                         uint32_t, uint32_t,
                         index, 0, 0);

CINT_FWRAPPER_CREATE_VP2(cbx_cfp_rule_dump,
                         cbx_cfpid_t*, cbx_cfpid_t,
                         cfpid, 1, 0,
                         uint32_t, uint32_t,
                         index, 0, 0);

CINT_FWRAPPER_CREATE_VP2(cbx_cfp_action_dump,
                         cbx_cfpid_t*, cbx_cfpid_t,
                         cfpid, 1, 0,
                         uint32_t, uint32_t,
                         index, 0, 0);

CINT_FWRAPPER_CREATE_RP5(int, int, 0, 0,
                         cbx_cfp_stat_multi_get,
                         cbx_cfpid_t*, cbx_cfpid_t,
                         cfpid, 1, 0,
                         uint32_t, uint32_t,
                         index, 0, 0,
                         int, int,
                         nstat, 0, 0,
                         cbx_cfp_stat_t*, cbx_cfp_stat_t,
                         stat_arr, 1, 0,
                         uint64*, uint64,
                         value_arr, 1, 0);

CINT_FWRAPPER_CREATE_RP5(int, int, 0, 0,
                         cbx_cfp_stat_multi_set,
                         cbx_cfpid_t*, cbx_cfpid_t,
                         cfpid, 1, 0,
                         uint32_t, uint32_t,
                         index, 0, 0,
                         int, int,
                         nstat, 0, 0,
                         cbx_cfp_stat_t*, cbx_cfp_stat_t,
                         stat_arr, 1, 0,
                         uint64*, uint64,
                         value_arr, 1, 0);


static void*
__cint_maddr__cbx_cfp_params_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    cbx_cfp_params_t* s = (cbx_cfp_params_t *) p;
    switch(mnum)
    {
        case 0: rv = &(s->stage); break;
        default: rv = NULL; break;
    }
    return rv;
}

static void*
__cint_maddr__cbx_cfp_common_action_t(void* p, int mnum,
                                      cint_struct_type_t* parent)
{
    void* rv;
    cbx_cfp_common_action_t* s = (cbx_cfp_common_action_t *) p;
    switch(mnum)
    {
        case 0: rv = &(s->meterid); break;
        case 1: rv = &(s->dp); break;
        case 2: rv = &(s->count_mode); break;
        default: rv = NULL; break;
    }
    return rv;
}

static void*
__cint_maddr__cbx_cfp_policy_action_t(void* p, int mnum,
                                      cint_struct_type_t* parent)
{
    void* rv;
    cbx_cfp_policy_action_t* s = (cbx_cfp_policy_action_t *) p;
    switch(mnum)
    {
        case 0: rv = &(s->forward_mode); break;
        case 1: rv = &(s->ucastid); break;
        case 2: rv = &(s->mcastid); break;
        case 3: rv = &(s->sourceid); break;
        case 4: rv = &(s->pri); break;
        case 5: rv = &(s->trapid); break;
        case 6: rv = &(s->mirrorid); break;
        default: rv = NULL; break;
    }
    return rv;
}

static void*
__cint_maddr__cbx_cfp_action_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    cbx_cfp_action_t* s = (cbx_cfp_action_t *) p;
    switch(mnum)
    {
        case 0: rv = &(s->flags); break;
        case 1: rv = &(s->common_action); break;
        case 2: rv = &(s->in_policy_action); break;
        case 3: rv = &(s->out_policy_action); break;
        default: rv = NULL; break;
    }
    return rv;
}

static void*
__cint_maddr__cbx_cfp_rule_attr_selector_t(void* p, int mnum,
                                           cint_struct_type_t* parent)
{
    void* rv;
    cbx_cfp_rule_attr_selector_t* s = (cbx_cfp_rule_attr_selector_t *) p;
    switch(mnum)
    {
        case 0: rv = &(s->attr); break;
        case 1: rv = &(s->attr_len); break;
        case 2: rv = &(s->attr_val); break;
        case 3: rv = &(s->attr_mask); break;
        default: rv = NULL; break;
    }
    return rv;
}


static void*
__cint_maddr__cbx_cfp_rule_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    cbx_cfp_rule_t* s = (cbx_cfp_rule_t *) p;
    switch(mnum)
    {
        case 0: rv = &(s->key_type); break;
        case 1: rv = &(s->num_attributes); break;
        case 2: rv = &(s->attr_selector); break;
        default: rv = NULL; break;
    }
    return rv;
}

static cint_parameter_desc_t __cint_struct_members__cbx_cfp_params_t[] =
{
    {
        "cbx_cfp_stage_t",
        "stage",
        0,
        0
    },
    CINT_ENTRY_LAST
};

static cint_parameter_desc_t __cint_struct_members__cbx_cfp_common_action_t[] =
{
    {
        "cbx_meterid_t",
        "meterid",
        0,
        0
    },
    {
        "int",
        "dp",
        0,
        0
    },
    {
        "cbx_cfp_count_mode_t",
        "count_mode",
        0,
        0
    },
    CINT_ENTRY_LAST
};

static cint_parameter_desc_t __cint_struct_members__cbx_cfp_policy_action_t[] =
{
    {
        "cbx_cfp_forward_mode_t",
        "forward_mode",
        0,
        0
    },
    {
        "cbx_portid_t",
        "ucastid",
        0,
        0
    },
    {
        "cbx_mcastid_t",
        "mcastid",
        0,
        0
    },
    {
        "cbx_portid_t",
        "sourceid",
        0,
        0
    },
    {
        "int",
        "pri",
        0,
        0
    },
    {
        "cbx_trapid_t",
        "trapid",
        0,
        0
    },
    {
        "cbx_mirrorid_t",
        "mirrorid",
        0,
        0
    },
    CINT_ENTRY_LAST
};


static cint_parameter_desc_t __cint_struct_members__cbx_cfp_action_t[] =
{
    {
        "uint32_t",
        "flags",
        0,
        0
    },
    {
        "cbx_cfp_common_action_t",
        "common_action",
        0,
        0
    },
    {
        "cbx_cfp_policy_action_t",
        "in_policy_action",
        0,
        0
    },
    {
        "cbx_cfp_policy_action_t",
        "out_policy_action",
        0,
        0
    },
    CINT_ENTRY_LAST
};

static cint_parameter_desc_t
                __cint_struct_members__cbx_cfp_rule_attr_selector_t[] =
{
    {
        "cbx_cfp_rule_attribute_t",
        "attr",
        0,
        0
    },
    {
        "uint8_t",
        "attr_len",
        0,
        0
    },
    {
        "uint8_t",
        "attr_val",
        1,
        0
    },
    {
        "uint8_t",
        "attr_mask",
        1,
        0
    },
    CINT_ENTRY_LAST
};


static cint_parameter_desc_t __cint_struct_members__cbx_cfp_rule_t[] =
{
    {
        "cbx_cfp_key_type_t",
        "key_type",
        0,
        0
    },
    {
        "uint32_t",
        "num_attributes",
        0,
        0
    },
    {
        "cbx_cfp_rule_attr_selector_t",
        "attr_selector",
        1,
        0
    },
    CINT_ENTRY_LAST
};

static cint_struct_type_t __cint_fsal_cfp_structures[] =
{
    {
    "cbx_cfp_params_t",
    sizeof(cbx_cfp_params_t),
    __cint_struct_members__cbx_cfp_params_t,
    __cint_maddr__cbx_cfp_params_t
    },
    {
    "cbx_cfp_common_action_t",
    sizeof(cbx_cfp_common_action_t),
    __cint_struct_members__cbx_cfp_common_action_t,
    __cint_maddr__cbx_cfp_common_action_t
    },
    {
    "cbx_cfp_policy_action_t",
    sizeof(cbx_cfp_policy_action_t),
    __cint_struct_members__cbx_cfp_policy_action_t,
    __cint_maddr__cbx_cfp_policy_action_t
    },
    {
    "cbx_cfp_action_t",
    sizeof(cbx_cfp_action_t),
    __cint_struct_members__cbx_cfp_action_t,
    __cint_maddr__cbx_cfp_action_t
    },
    {
    "cbx_cfp_rule_attr_selector_t",
    sizeof(cbx_cfp_rule_attr_selector_t),
    __cint_struct_members__cbx_cfp_rule_attr_selector_t,
    __cint_maddr__cbx_cfp_rule_attr_selector_t
    },
    {
    "cbx_cfp_rule_t",
    sizeof(cbx_cfp_rule_t),
    __cint_struct_members__cbx_cfp_rule_t,
    __cint_maddr__cbx_cfp_rule_t
    },
    CINT_ENTRY_LAST
};

static cint_enum_map_t __cint_enum_map__cbx_cfp_l3_l4_pkt_type_t[] =
{
    { "cbxCfpL3L4PktTypeTcp", cbxCfpL3L4PktTypeTcp },
    { "cbxCfpL3L4PktTypeUdp", cbxCfpL3L4PktTypeUdp },
    { "cbxCfpL3L4PktTypeIcmp", cbxCfpL3L4PktTypeIcmp },
    { "cbxCfpL3L4PktTypeIgmp", cbxCfpL3L4PktTypeIgmp },
    CINT_ENTRY_LAST
};

static cint_enum_map_t __cint_enum_map__cbx_cfp_rule_attribute_t[] =
{
    { "cbxCfpRuleAttrSourcePort", cbxCfpRuleAttrSourcePort },
    { "cbxCfpRuleAttrIncomingTrafficClass", cbxCfpRuleAttrIncomingTrafficClass },
    { "cbxCfpRuleAttrOuterVlanTagStatus", cbxCfpRuleAttrOuterVlanTagStatus },
    { "cbxCfpRuleAttrInnerVlanTagStatus", cbxCfpRuleAttrInnerVlanTagStatus },
    { "cbxCfpRuleAttrMimTagStatus", cbxCfpRuleAttrMimTagStatus },
    { "cbxCfpRuleAttrETagStatus", cbxCfpRuleAttrETagStatus },
    { "cbxCfpRuleAttrEtherTypeStatus", cbxCfpRuleAttrEtherTypeStatus },
    { "cbxCfpRuleAttrMplsStatus", cbxCfpRuleAttrMplsStatus },
    { "cbxCfpRuleAttrPppStatus", cbxCfpRuleAttrPppStatus },
    { "cbxCfpRuleAttrL3Type", cbxCfpRuleAttrL3Type },
    { "cbxCfpRuleAttrRangeResult", cbxCfpRuleAttrRangeResult },
    { "cbxCfpRuleAttrDestMac", cbxCfpRuleAttrDestMac },
    { "cbxCfpRuleAttrSrcMac", cbxCfpRuleAttrSrcMac },
    { "cbxCfpRuleAttrOuterVlanId", cbxCfpRuleAttrOuterVlanId },
    { "cbxCfpRuleAttrInnerVlanId", cbxCfpRuleAttrInnerVlanId },
    { "cbxCfpRuleAttrEtherType", cbxCfpRuleAttrEtherType },
    { "cbxCfpRuleAttrL4Type", cbxCfpRuleAttrL4Type },
    { "cbxCfpRuleAttrL3TtlSummary", cbxCfpRuleAttrL3TtlSummary },
    { "cbxCfpRuleAttrL3FragSummary", cbxCfpRuleAttrL3FragSummary },
    { "cbxCfpRuleAttrSrcIp", cbxCfpRuleAttrSrcIp },
    { "cbxCfpRuleAttrDestIp", cbxCfpRuleAttrDestIp },
    { "cbxCfpRuleAttrDscp", cbxCfpRuleAttrDscp },
    { "cbxCfpRuleAttrL4SrcPort", cbxCfpRuleAttrL4SrcPort },
    { "cbxCfpRuleAttrL4DestPort", cbxCfpRuleAttrL4DestPort },
    { "cbxCfpRuleAttrIcmpCode", cbxCfpRuleAttrIcmpCode },
    { "cbxCfpRuleAttrIgmpType", cbxCfpRuleAttrIgmpType },
    { "cbxCfpRuleAttrTcpControl", cbxCfpRuleAttrTcpControl },
    { "cbxCfpRuleAttrSrcIp6", cbxCfpRuleAttrSrcIp6 },
    { "cbxCfpRuleAttrSrcIp6High", cbxCfpRuleAttrSrcIp6High },
    { "cbxCfpRuleAttrSrcIp6Low", cbxCfpRuleAttrSrcIp6Low },
    { "cbxCfpRuleAttrDestIp6", cbxCfpRuleAttrDestIp6 },
    { "cbxCfpRuleAttrDestIp6High", cbxCfpRuleAttrDestIp6High },
    { "cbxCfpRuleAttrDestIp6Low", cbxCfpRuleAttrDestIp6Low },
    { "cbxCfpRuleAttrUDF0", cbxCfpRuleAttrUDF0 },
    { "cbxCfpRuleAttrUDF1", cbxCfpRuleAttrUDF1 },
    { "cbxCfpRuleAttrUDF2", cbxCfpRuleAttrUDF2 },
    { "cbxCfpRuleAttrUDF3", cbxCfpRuleAttrUDF3 },
    { "cbxCfpRuleAttrUDF4", cbxCfpRuleAttrUDF4 },
    { "cbxCfpRuleAttrUDF5", cbxCfpRuleAttrUDF5 },
    { "cbxCfpRuleAttrUDF6", cbxCfpRuleAttrUDF6 },
    { "cbxCfpRuleAttrUDF7", cbxCfpRuleAttrUDF7 },
    { "cbxCfpRuleAttrMax", cbxCfpRuleAttrMax },
    CINT_ENTRY_LAST
};


static cint_enum_map_t __cint_enum_map__cbx_cfp_forward_mode_t[] =
{
    { "cbxCFPForwardNoChange", cbxCFPForwardNoChange },
    { "cbxCFPForwardLoopback", cbxCFPForwardLoopback },
    { "cbxCFPForwardUnicast", cbxCFPForwardUnicast },
    { "cbxCFPForwardMulticast", cbxCFPForwardMulticast },
    { "cbxCFPForwardFrom", cbxCFPForwardFrom },
    { "cbxCFPForwardDrop", cbxCFPForwardDrop },
    { "cbxCFPForwardOverrideDrop", cbxCFPForwardOverrideDrop },
    CINT_ENTRY_LAST
};

static cint_enum_map_t __cint_enum_map__cbx_cfp_count_mode_t[] =
{
    { "cbxCFPCountPacketsBytes", cbxCFPCountPacketsBytes },
    { "cbxCFPCountPacketsByProfile", cbxCFPCountPacketsByProfile },
    CINT_ENTRY_LAST
};

static cint_enum_map_t __cint_enum_map__cbx_cfp_stat_t[] =
{
    { "cbxCFPStatBytes", cbxCFPStatBytes },
    { "cbxCFPStatPackets", cbxCFPStatPackets },
    { "cbxCFPStatPacketsInProfile", cbxCFPStatPacketsInProfile },
    { "cbxCFPStatPacketsOutProfile", cbxCFPStatPacketsOutProfile },
    CINT_ENTRY_LAST
};

static cint_enum_map_t __cint_enum_map__cbx_cfp_key_type_t[] =
{
    { "cbxCfpKeyL2EthernetACL", cbxCfpKeyL2EthernetACL },
    { "cbxCfpKeyIPv4ACL", cbxCfpKeyIPv4ACL },
    { "cbxCfpKeyIPv6ACL", cbxCfpKeyIPv6ACL },
    { "cbxCfpKeyL2IPv4ACL", cbxCfpKeyL2IPv4ACL },
    { "cbxCfpKeyUDFACL", cbxCfpKeyUDFACL },
    CINT_ENTRY_LAST
};


static cint_enum_map_t __cint_enum_map__cbx_cfp_stage_t[] =
{
    { "cbxCfpStageIngress", cbxCfpStageIngress },
    { "cbxCfpStageEgress", cbxCfpStageEgress },
    CINT_ENTRY_LAST
};

static cint_enum_type_t __cint_fsal_cfp_enums[] =
{
    { "cbx_cfp_stage_t", __cint_enum_map__cbx_cfp_stage_t },
    { "cbx_cfp_key_type_t", __cint_enum_map__cbx_cfp_key_type_t },
    { "cbx_cfp_count_mode_t", __cint_enum_map__cbx_cfp_count_mode_t },
    { "cbx_cfp_stat_t", __cint_enum_map__cbx_cfp_stat_t },
    { "cbx_cfp_forward_mode_t",
	   __cint_enum_map__cbx_cfp_forward_mode_t },
    { "cbx_cfp_rule_attribute_t", __cint_enum_map__cbx_cfp_rule_attribute_t },
    { "cbx_cfp_l3_l4_pkt_type_t", __cint_enum_map__cbx_cfp_l3_l4_pkt_type_t },
    CINT_ENTRY_LAST
};

static cint_parameter_desc_t __cint_fsal_cfp_typedefs[] =
{
    {
        "uint32_t",
        "cbx_cfpid_t",
        0,
        0
    },
    CINT_ENTRY_LAST
};

static cint_constants_t __cint_fsal_cfp_constants[] =
{
   { "CBX_CFP_INVALID", CBX_CFP_INVALID },
   { "CBX_CFP_ACTION_OVERRIDE", CBX_CFP_ACTION_OVERRIDE },
   { "CBX_CFP_ACTION_UPDATE_DP", CBX_CFP_ACTION_UPDATE_DP },
   { "CBX_CFP_ACTION_DLF", CBX_CFP_ACTION_DLF },
   { "CBX_CFP_ACTION_LOOP_ENABLE", CBX_CFP_ACTION_LOOP_ENABLE },
   { "CBX_CFP_ACTION_IN_POLICY_UPDATE_PRI",
                                         CBX_CFP_ACTION_IN_POLICY_UPDATE_PRI },
   { "CBX_CFP_ACTION_OUT_POLICY_UPDATE_PRI",
                                        CBX_CFP_ACTION_OUT_POLICY_UPDATE_PRI },
   { "CBX_CFP_ACTION_IN_POLICY_LEARN_DISABLE",
                                      CBX_CFP_ACTION_IN_POLICY_LEARN_DISABLE },
   { "CBX_CFP_ACTION_OUT_POLICY_LEARN_DISABLE",
                                     CBX_CFP_ACTION_OUT_POLICY_LEARN_DISABLE },
   { "CBX_CFP_L2_ETHERNET_ATTR_MAX", CBX_CFP_L2_ETHERNET_ATTR_MAX },
   { "CBX_CFP_IPV4_ATTR_MAX", CBX_CFP_IPV4_ATTR_MAX },
   { "CBX_CFP_IPV6_ATTR_MAX", CBX_CFP_IPV6_ATTR_MAX },
   { "CBX_CFP_L2_IPV4_ATTR_MAX", CBX_CFP_L2_IPV4_ATTR_MAX },
   { "CBX_CFP_ATTR_OUTER_VLAN_NOT_PRESENT",
                                         CBX_CFP_ATTR_OUTER_VLAN_NOT_PRESENT },
   { "CBX_CFP_ATTR_OUTER_VLAN_PRIORITY_TAGGED",
                                      CBX_CFP_ATTR_OUTER_VLAN_PRIORITY_TAGGED },
   { "CBX_CFP_ATTR_OUTER_VLAN_SINGLE_TAG", CBX_CFP_ATTR_OUTER_VLAN_SINGLE_TAG },
   { "CBX_CFP_ATTR_INNER_VLAN_NOT_PRESENT",
                                          CBX_CFP_ATTR_INNER_VLAN_NOT_PRESENT },
   { "CBX_CFP_ATTR_INNER_VLAN_PRIORITY_TAGGED",
                                      CBX_CFP_ATTR_INNER_VLAN_PRIORITY_TAGGED },
   { "CBX_CFP_ATTR_INNER_VLAN_SINGLE_TAG", CBX_CFP_ATTR_INNER_VLAN_SINGLE_TAG },
   { "CBX_CFP_ATTR_INNER_VLAN_PRESENT", CBX_CFP_ATTR_INNER_VLAN_PRESENT },
   { "CBX_CFP_ATTR_MIM_TAG_PRESENT", CBX_CFP_ATTR_MIM_TAG_PRESENT },
   { "CBX_CFP_ATTR_E_TAG_PRESENT", CBX_CFP_ATTR_E_TAG_PRESENT },
   { "CBX_CFP_ATTR_MPLS_TAG_PRESENT", CBX_CFP_ATTR_MPLS_TAG_PRESENT },
   { "CBX_CFP_ATTR_PPP_HEADER_PRESENT", CBX_CFP_ATTR_PPP_HEADER_PRESENT },
   { "CBX_CFP_ATTR_ETHERTYPE_LLC_SSAP_DDAP",
                                   CBX_CFP_ATTR_ETHERTYPE_LLC_SSAP_DDAP },
   { "CBX_CFP_ATTR_ETHERTYPE_DIX", CBX_CFP_ATTR_ETHERTYPE_DIX },
   { "CBX_CFP_L3_TYPE_RAW", CBX_CFP_L3_TYPE_RAW },
   { "CBX_CFP_L3_TYPE_LLC", CBX_CFP_L3_TYPE_LLC },
   { "CBX_CFP_L3_TYPE_IPV4", CBX_CFP_L3_TYPE_IPV4 },
   { "CBX_CFP_L3_TYPE_IPV6", CBX_CFP_L3_TYPE_IPV6 },
   { "CBX_CFP_L3_TYPE_UC_MPLS", CBX_CFP_L3_TYPE_UC_MPLS },
   { "CBX_CFP_L3_TYPE_MC_MPLS", CBX_CFP_L3_TYPE_MC_MPLS },
   { "CBX_CFP_L3_TYPE_PTP", CBX_CFP_L3_TYPE_PTP },
   { "CBX_CFP_L3_TYPE_ARP", CBX_CFP_L3_TYPE_ARP },
   { "CBX_CFP_L4_TYPE_UNKNOWN", CBX_CFP_L4_TYPE_UNKNOWN },
   { "CBX_CFP_L4_TYPE_TCP", CBX_CFP_L4_TYPE_TCP },
   { "CBX_CFP_L4_TYPE_UDP", CBX_CFP_L4_TYPE_UDP },
   { "CBX_CFP_L4_TYPE_ICMP", CBX_CFP_L4_TYPE_ICMP },
   { "CBX_CFP_L4_TYPE_IGMP", CBX_CFP_L4_TYPE_IGMP },
   { "CBX_CFP_L3_TTL_ZERO", CBX_CFP_L3_TTL_ZERO },
   { "CBX_CFP_L3_TTL_ONE", CBX_CFP_L3_TTL_ONE },
   { "CBX_CFP_L3_TTL_255", CBX_CFP_L3_TTL_255 },
   { "CBX_CFP_L3_TTL_OTHER", CBX_CFP_L3_TTL_OTHER },
   { "CBX_CFP_L3_FRAG_MIDDLE", CBX_CFP_L3_FRAG_MIDDLE },
   { "CBX_CFP_L3_FRAG_LAST", CBX_CFP_L3_FRAG_LAST },
   { "CBX_CFP_L3_FRAG_FIRST", CBX_CFP_L3_FRAG_FIRST },
   { "CBX_CFP_L3_FRAG_NONE", CBX_CFP_L3_FRAG_NONE },
   { "CBX_CFP_TCP_CONTROL_FIN", CBX_CFP_TCP_CONTROL_FIN },
   { "CBX_CFP_TCP_CONTROL_SYN", CBX_CFP_TCP_CONTROL_SYN },
   { "CBX_CFP_TCP_CONTROL_RST", CBX_CFP_TCP_CONTROL_RST },
   { "CBX_CFP_TCP_CONTROL_PSH", CBX_CFP_TCP_CONTROL_PSH },
   { "CBX_CFP_TCP_CONTROL_ACK", CBX_CFP_TCP_CONTROL_ACK },
   { "CBX_CFP_TCP_CONTROL_URG", CBX_CFP_TCP_CONTROL_URG },
    CINT_ENTRY_LAST
};

static cint_function_pointer_t __cint_fsal_cfp_function_pointers[] =
{
    CINT_ENTRY_LAST
};
static cint_function_t __cint_fsal_cfp_functions[] =
    {
        CINT_FWRAPPER_ENTRY(cbx_cfp_action_t_init),
        CINT_FWRAPPER_ENTRY(cbx_cfp_init),
        CINT_FWRAPPER_ENTRY(cbx_cfp_detach),
        CINT_FWRAPPER_ENTRY(cbx_cfp_rule_t_init),
        CINT_FWRAPPER_ENTRY(cbx_cfp_rule_set),
        CINT_FWRAPPER_ENTRY(cbx_cfp_rule_clear),
        CINT_FWRAPPER_ENTRY(cbx_cfp_action_set),
        CINT_FWRAPPER_ENTRY(cbx_cfp_action_clear),
        CINT_FWRAPPER_ENTRY(cbx_cfp_rule_dump),
        CINT_FWRAPPER_ENTRY(cbx_cfp_action_dump),
        CINT_FWRAPPER_ENTRY(cbx_cfp_stat_multi_get),
        CINT_FWRAPPER_ENTRY(cbx_cfp_stat_multi_set),
        CINT_ENTRY_LAST
    };

cint_data_t fsal_cfp_cint_data =
    {
        NULL,
        __cint_fsal_cfp_functions,
        __cint_fsal_cfp_structures,
        __cint_fsal_cfp_enums,
        __cint_fsal_cfp_typedefs,
        __cint_fsal_cfp_constants,
        __cint_fsal_cfp_function_pointers
    };
#endif /* INCLUDE_LIB_CINT */
