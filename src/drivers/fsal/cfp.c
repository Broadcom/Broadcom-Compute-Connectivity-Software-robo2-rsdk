/*
 * * $ID: $
 * * 
 * * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 * * 
 * * $Copyright: (c) 2020 Broadcom Inc.
 * *
 * * File:
 * *     cfp.c
 * * Purpose:
 * *     Robo2 CFP(ACL) module.
 * *
 * */
#include <fsal_int/cfp.h>


/* Routine to check if CFP is initialized or not */
#define CFP_INIT    \
      if (field_ctrl.cfp_state != CBXI_CFP_INIT_COMPLETE) {\
          return CBX_E_INIT; \
      }

static     cbxi_field_control_t   field_ctrl;
uint32_t   l2_rule_attr_bmp;
uint32_t   v4_rule_attr_bmp;
uint32_t   v6_rule_attr_bmp_0;
uint32_t   v6_rule_attr_bmp_1;
uint32_t   l2_v4_rule_attr_bmp_0;
uint32_t   l2_v4_rule_attr_bmp_1;
uint32_t   udf_rule_attr_bmp_0;
uint32_t   udf_rule_attr_bmp_1;
uint32_t   tcam_index_info[CBXI_CFP_CFP_RULE_MAX + 1] = {0};

static cbxi_cfp_rule_attribute_info_t cfp_rule_attr_info[] = {
/*  valid len  data mask
 ------------------------ */
    { 0x1F, 4,   0, 0xFFFF }, /* cbxCfpRuleAttrSourcePort */
    { 0x1F, 1,   0, 0x7    }, /* cbxCfpRuleAttrIncomingTrafficClass */
    { 0x1F, 1,   0, 0x3    }, /* cbxCfpRuleAttrOuterVlanTagStatus */
    { 0x1F, 1,   0, 0x3    }, /* cbxCfpRuleAttrInnerVlanTagStatus */
    { 0x1F, 1,   0, 0x1    }, /* cbxCfpRuleAttrMimTagStatus */
    { 0x1F, 1,   0, 0x1    }, /* cbxCfpRuleAttrETagStatus */
    { 0x1F, 1,   0, 0x1    }, /* cbxCfpRuleAttrEtherTypeStatus */
    { 0x1F, 1,   0, 0x1    }, /* cbxCfpRuleAttrMplsStatus */
    { 0x1F, 1,   0, 0x1    }, /* cbxCfpRuleAttrPppStatus */
    { 0x1F, 1,   0, 0x7    }, /* cbxCfpRuleAttrL3Type */
    { 0x0F, 1,   0, 0xFF   }, /* cbxCfpRuleAttrRangeResult */
    { 0x09, 6,   0, 0xFFFF }, /* cbxCfpRuleAttrDestMac */
    { 0x09, 6,   0, 0xFFFF }, /* cbxCfpRuleAttrSrcMac */
    { 0x0F, 2,   0, 0xFFF  }, /* cbxCfpRuleAttrOuterVlanId */
    { 0x01, 2,   0, 0xFFF  }, /* cbxCfpRuleAttrInnerVlanId */
    { 0x01, 2,   0, 0xFFFF }, /* cbxCfpRuleAttrEtherType  */
    { 0x0E, 1,   0, 0x7    }, /* cbxCfpRuleAttrL4Type */
    { 0x0E, 1,   0, 0x3    }, /* cbxCfpRuleAttrL3TtlSummary */
    { 0x0E, 1,   0, 0x3    }, /* cbxCfpRuleAttrL3FragSummary */
    { 0x0A, 4,   0, 0xFFFF }, /* cbxCfpRuleAttrSrcIp */
    { 0x0A, 4,   0, 0xFFFF }, /* cbxCfpRuleAttrDestIp */
    { 0x0E, 1,   0, 0xFF   }, /* cbxCfpRuleAttrDscp */
    { 0x0E, 2,   0, 0xFFFF }, /* cbxCfpRuleAttrL4SrcPort */
    { 0x0E, 2,   0, 0xFFFF }, /* cbxCfpRuleAttrL4DestPort */
    { 0x0E, 2,   0, 0xFFFF }, /* cbxCfpRuleAttrIcmpCode */
    { 0x0E, 2,   0, 0xFFFF }, /* cbxCfpRuleAttrIgmpType */
    { 0x0E, 1,   0, 0x3F   }, /* cbxCfpRuleAttrTcpControl */
    { 0x04, 16,  0, 0xFFFF }, /* cbxCfpRuleAttrSrcIp6 */
    { 0x04, 8,   0, 0xFFFF }, /* cbxCfpRuleAttrSrcIp6High */
    { 0x04, 8,   0, 0xFFFF }, /* cbxCfpRuleAttrSrcIp6Low */
    { 0x04, 16,  0, 0xFFFF }, /* cbxCfpRuleAttrDestIp6 */
    { 0x04, 8,   0, 0xFFFF }, /* cbxCfpRuleAttrDestIp6High */
    { 0x04, 8,   0, 0xFFFF }, /* cbxCfpRuleAttrDestIp6Low */
    { 0x10, 8,   0, 0xFFFF }, /* cbxCfpRuleAttrUDF0 */
    { 0x10, 8,   0, 0xFFFF }, /* cbxCfpRuleAttrUDF1 */
    { 0x10, 4,   0, 0xFFFF }, /* cbxCfpRuleAttrUDF2 */
    { 0x10, 4,   0, 0xFFFF }, /* cbxCfpRuleAttrUDF3 */
    { 0x10, 2,   0, 0xFFFF }, /* cbxCfpRuleAttrUDF4 */
    { 0x10, 2,   0, 0xFFFF }, /* cbxCfpRuleAttrUDF5 */
    { 0x10, 2,   0, 0xFFFF }, /* cbxCfpRuleAttrUDF6 */
    { 0x10, 2,   0, 0xFFFF }, /* cbxCfpRuleAttrUDF7 */
    { 0x00, 0,   0, 0      }  /* cbxCfpRuleAttrMax */
};

char *cfp_rule_attr_names[] = {
      "SourcePort",
      "IncomingTrafficClass",
      "OuterVlanTagStatus",
      "InnerVlanTagStatus",
      "MimTagStatus",
      "ETagStatus",
      "EtherTypeStatus",
      "MplsStatus",
      "PppStatus",
      "L3Type",
      "RangeResult",
      "DestMac",
      "SrcMac",
      "OuterVlanId",
      "InnerVlanId",
      "EtherType",
      "L4Type",
      "L3TtlSummary",
      "L3FragSummary",
      "SrcIp",
      "DestIp",
      "Dscp",
      "L4SrcPort",
      "L4DestPort",
      "IcmpCode",
      "IgmpType",
      "TcpControl",
      "SrcIp6",
      "SrcIp6High",
      "SrcIp6Low",
      "DestIp6",
      "DestIp6High",
      "DestIp6Low",
      "UDF0",
      "UDF1",
      "UDF2",
      "UDF3",
      "UDF4",
      "UDF5",
      "UDF6",
      "UDF7",
};

cbxi_cfp_rule_field_info_t  cfp_l2_rule_field_info[cbxCfpRuleAttrMax + 1] = {
    { 16, 173 }, /* cbxCfpRuleAttrSourcePort */
    {  3, 170 }, /* cbxCfpRuleAttrIncomingTrafficClass */
    {  2, 168 }, /* cbxCfpRuleAttrOuterVlanTagStatus */
    {  2, 166 }, /* cbxCfpRuleAttrInnerVlanTagStatus */
    {  1, 165 }, /* cbxCfpRuleAttrMimTagStatus */
    {  1, 164 }, /* cbxCfpRuleAttrETagStatus */
    {  1, 163 }, /* cbxCfpRuleAttrEtherTypeStatus */
    {  1, 162 }, /* cbxCfpRuleAttrMplsStatus */
    {  1, 161 }, /* cbxCfpRuleAttrPppStatus */
    {  4, 157 }, /* cbxCfpRuleAttrL3Type */
    {  8, 149 }, /* cbxCfpRuleAttrRangeResult */
    { 48, 101 }, /* cbxCfpRuleAttrDestMac */
    { 48,  53 }, /* cbxCfpRuleAttrSrcMac */
    { 12,  41 }, /* cbxCfpRuleAttrOuterVlanId */
    { 12,  29 }, /* cbxCfpRuleAttrInnerVlanId */
    { 16,  13 }, /* cbxCfpRuleAttrEtherType  */
    {  0,  0 }, /* cbxCfpRuleAttrL4Type */
    {  0,  0 }, /* cbxCfpRuleAttrL3TtlSummary */
    {  0,  0 }, /* cbxCfpRuleAttrL3FragSummary */
    {  0,  0 }, /* cbxCfpRuleAttrSrcIp */
    {  0,  0 }, /* cbxCfpRuleAttrDestIp */
    {  0,  0 }, /* cbxCfpRuleAttrDscp */
    {  0,  0 }, /* cbxCfpRuleAttrL4SrcPort */
    {  0,  0 }, /* cbxCfpRuleAttrL4DestPort */
    {  0,  0 }, /* cbxCfpRuleAttrIcmpCode */
    {  0,  0 }, /* cbxCfpRuleAttrIgmpType */
    {  0,  0 }, /* cbxCfpRuleAttrTcpControl */
    {  0,  0 }, /* cbxCfpRuleAttrSrcIp6 */
    {  0,  0 }, /* cbxCfpRuleAttrSrcIp6High */
    {  0,  0 }, /* cbxCfpRuleAttrSrcIp6Low */
    {  0,  0 }, /* cbxCfpRuleAttrDestIp6 */
    {  0,  0 }, /* cbxCfpRuleAttrDestIp6High */
    {  0,  0 }, /* cbxCfpRuleAttrDestIp6Low */
    {  0,  0 }, /* cbxCfpRuleAttrUDF0 */
    {  0,  0 }, /* cbxCfpRuleAttrUDF1 */
    {  0,  0 }, /* cbxCfpRuleAttrUDF2 */
    {  0,  0 }, /* cbxCfpRuleAttrUDF3 */
    {  0,  0 }, /* cbxCfpRuleAttrUDF4 */
    {  0,  0 }, /* cbxCfpRuleAttrUDF5 */
    {  0,  0 }, /* cbxCfpRuleAttrUDF6 */
    {  0,  0 }, /* cbxCfpRuleAttrUDF7 */
    {  0,  0 }  /* cbxCfpRuleAttrMax */
};

cbxi_cfp_rule_field_info_t  cfp_ipv4_rule_field_info[cbxCfpRuleAttrMax + 1] = {
    { 16, 173 }, /* cbxCfpRuleAttrSourcePort */
    {  3, 170 }, /* cbxCfpRuleAttrIncomingTrafficClass */
    {  2, 168 }, /* cbxCfpRuleAttrOuterVlanTagStatus */
    {  2, 166 }, /* cbxCfpRuleAttrInnerVlanTagStatus */
    {  1, 165 }, /* cbxCfpRuleAttrMimTagStatus */
    {  1, 164 }, /* cbxCfpRuleAttrETagStatus */
    {  1, 163 }, /* cbxCfpRuleAttrEtherTypeStatus */
    {  1, 162 }, /* cbxCfpRuleAttrMplsStatus */
    {  1, 161 }, /* cbxCfpRuleAttrPppStatus */
    {  4, 157 }, /* cbxCfpRuleAttrL3Type */
    {  8, 149 }, /* cbxCfpRuleAttrRangeResult */
    {  0,  0  }, /* cbxCfpRuleAttrDestMac */
    {  0,  0  }, /* cbxCfpRuleAttrSrcMac */
    { 12, 130 }, /* cbxCfpRuleAttrOuterVlanId */
    {  0,  0  }, /* cbxCfpRuleAttrInnerVlanId */
    {  0,  0  }, /* cbxCfpRuleAttrEtherType  */
    {  3, 146 }, /* cbxCfpRuleAttrL4Type */
    {  2, 144 }, /* cbxCfpRuleAttrL3TtlSummary */
    {  2, 142 }, /* cbxCfpRuleAttrL3FragSummary */
    { 32, 52  }, /* cbxCfpRuleAttrSrcIp */
    { 32, 20  }, /* cbxCfpRuleAttrDestIp */
    {  8, 122 }, /* cbxCfpRuleAttrDscp */
    { 16, 106 }, /* cbxCfpRuleAttrL4SrcPort */
    { 16, 90  }, /* cbxCfpRuleAttrL4DestPort */
    { 16, 106 }, /* cbxCfpRuleAttrIcmpCode */
    { 16, 106 }, /* cbxCfpRuleAttrIgmpType */
    {  6, 84  }, /* cbxCfpRuleAttrTcpControl */
    {  0,  0  }, /* cbxCfpRuleAttrSrcIp6 */
    {  0,  0  }, /* cbxCfpRuleAttrSrcIp6High */
    {  0,  0  }, /* cbxCfpRuleAttrSrcIp6Low */
    {  0,  0  }, /* cbxCfpRuleAttrDestIp6 */
    {  0,  0  }, /* cbxCfpRuleAttrDestIp6High */
    {  0,  0  }, /* cbxCfpRuleAttrDestIp6Low */
    {  0,  0  }, /* cbxCfpRuleAttrUDF0 */
    {  0,  0  }, /* cbxCfpRuleAttrUDF1 */
    {  0,  0  }, /* cbxCfpRuleAttrUDF2 */
    {  0,  0  }, /* cbxCfpRuleAttrUDF3 */
    {  0,  0  }, /* cbxCfpRuleAttrUDF4 */
    {  0,  0  }, /* cbxCfpRuleAttrUDF5 */
    {  0,  0  }, /* cbxCfpRuleAttrUDF6 */
    {  0,  0  }, /* cbxCfpRuleAttrUDF7 */
    {  0,  0  }  /* cbxCfpRuleAttrMax */
};

cbxi_cfp_rule_field_info_t  cfp_ipv6_rule_field_info[cbxCfpRuleAttrMax + 1] = {
    { 16, 173 }, /* cbxCfpRuleAttrSourcePort */
    {  3, 170 }, /* cbxCfpRuleAttrIncomingTrafficClass */
    {  2, 168 }, /* cbxCfpRuleAttrOuterVlanTagStatus */
    {  2, 166 }, /* cbxCfpRuleAttrInnerVlanTagStatus */
    {  1, 165 }, /* cbxCfpRuleAttrMimTagStatus */
    {  1, 164 }, /* cbxCfpRuleAttrETagStatus */
    {  1, 163 }, /* cbxCfpRuleAttrEtherTypeStatus */
    {  1, 162 }, /* cbxCfpRuleAttrMplsStatus */
    {  1, 161 }, /* cbxCfpRuleAttrPppStatus */
    {  4, 157 }, /* cbxCfpRuleAttrL3Type */
    {  8, 149 }, /* cbxCfpRuleAttrRangeResult */
    {  0,  0  }, /* cbxCfpRuleAttrDestMac */
    {  0,  0  }, /* cbxCfpRuleAttrSrcMac */
    { 12, 130 }, /* cbxCfpRuleAttrOuterVlanId */
    {  0,  0  }, /* cbxCfpRuleAttrInnerVlanId */
    {  0,  0  }, /* cbxCfpRuleAttrEtherType  */
    {  3, 146 }, /* cbxCfpRuleAttrL4Type */
    {  2, 144 }, /* cbxCfpRuleAttrL3TtlSummary */
    {  2, 142 }, /* cbxCfpRuleAttrL3FragSummary */
    {  0,  0  }, /* cbxCfpRuleAttrSrcIp */
    {  0,  0  }, /* cbxCfpRuleAttrDestIp */
    {  8, 245 }, /* cbxCfpRuleAttrDscp */
    { 16, 229 }, /* cbxCfpRuleAttrL4SrcPort */
    { 16, 213 }, /* cbxCfpRuleAttrL4DestPort */
    { 16, 229 }, /* cbxCfpRuleAttrIcmpCode */
    { 16, 229 }, /* cbxCfpRuleAttrIgmpType */
    {  6, 207 }, /* cbxCfpRuleAttrTcpControl */
    {128,   2 }, /* cbxCfpRuleAttrSrcIp6 */
    { 64,  66 }, /* cbxCfpRuleAttrSrcIp6High */
    { 64,   2 }, /* cbxCfpRuleAttrSrcIp6Low */
    { 128,253 }, /* cbxCfpRuleAttrDestIp6 */
    { 64, 317 }, /* cbxCfpRuleAttrDestIp6High */
    { 64, 253 }, /* cbxCfpRuleAttrDestIp6Low */
    {  0,  0  }, /* cbxCfpRuleAttrUDF0 */
    {  0,  0  }, /* cbxCfpRuleAttrUDF1 */
    {  0,  0  }, /* cbxCfpRuleAttrUDF2 */
    {  0,  0  }, /* cbxCfpRuleAttrUDF3 */
    {  0,  0  }, /* cbxCfpRuleAttrUDF4 */
    {  0,  0  }, /* cbxCfpRuleAttrUDF5 */
    {  0,  0  }, /* cbxCfpRuleAttrUDF6 */
    {  0,  0  }, /* cbxCfpRuleAttrUDF7 */
    {  0,  0  }  /* cbxCfpRuleAttrMax */
};

cbxi_cfp_rule_field_info_t
    cfp_l2_ipv4_rule_field_info[cbxCfpRuleAttrMax + 1] = {
    { 16, 173 }, /* cbxCfpRuleAttrSourcePort */
    {  3, 170 }, /* cbxCfpRuleAttrIncomingTrafficClass */
    {  2, 168 }, /* cbxCfpRuleAttrOuterVlanTagStatus */
    {  2, 166 }, /* cbxCfpRuleAttrInnerVlanTagStatus */
    {  1, 165 }, /* cbxCfpRuleAttrMimTagStatus */
    {  1, 164 }, /* cbxCfpRuleAttrETagStatus */
    {  1, 163 }, /* cbxCfpRuleAttrEtherTypeStatus */
    {  1, 162 }, /* cbxCfpRuleAttrMplsStatus */
    {  1, 161 }, /* cbxCfpRuleAttrPppStatus */
    {  4, 157 }, /* cbxCfpRuleAttrL3Type */
    {  8, 149 }, /* cbxCfpRuleAttrRangeResult */
    { 48,  94 }, /* cbxCfpRuleAttrDestMac */
    { 48,  46 }, /* cbxCfpRuleAttrSrcMac */
    { 12,  34 }, /* cbxCfpRuleAttrOuterVlanId */
    {  0,  0 }, /* cbxCfpRuleAttrInnerVlanId */
    {  0,  0 }, /* cbxCfpRuleAttrEtherType  */
    {  3, 146 }, /* cbxCfpRuleAttrL4Type */
    {  2, 144 }, /* cbxCfpRuleAttrL3TtlSummary */
    {  2, 142 }, /* cbxCfpRuleAttrL3FragSummary */
    { 32, 349}, /* cbxCfpRuleAttrSrcIp */
    { 32, 317}, /* cbxCfpRuleAttrDestIp */
    {  8, 309}, /* cbxCfpRuleAttrDscp */
    { 16, 293}, /* cbxCfpRuleAttrL4SrcPort */
    { 16, 277}, /* cbxCfpRuleAttrL4DestPort */
    { 16, 293}, /* cbxCfpRuleAttrIcmpCode */
    { 16, 293}, /* cbxCfpRuleAttrIgmpType */
    {  6, 271}, /* cbxCfpRuleAttrTcpControl */
    {  0,  0 }, /* cbxCfpRuleAttrSrcIp6 */
    {  0,  0 }, /* cbxCfpRuleAttrSrcIp6High */
    {  0,  0 }, /* cbxCfpRuleAttrSrcIp6Low */
    {  0,  0 }, /* cbxCfpRuleAttrDestIp6 */
    {  0,  0 }, /* cbxCfpRuleAttrDestIp6High */
    {  0,  0 }, /* cbxCfpRuleAttrDestIp6Low */
    {  0,  0 }, /* cbxCfpRuleAttrUDF0 */
    {  0,  0 }, /* cbxCfpRuleAttrUDF1 */
    {  0,  0 }, /* cbxCfpRuleAttrUDF2 */
    {  0,  0 }, /* cbxCfpRuleAttrUDF3 */
    {  0,  0 }, /* cbxCfpRuleAttrUDF4 */
    {  0,  0 }, /* cbxCfpRuleAttrUDF5 */
    {  0,  0 }, /* cbxCfpRuleAttrUDF6 */
    {  0,  0 }, /* cbxCfpRuleAttrUDF7 */
    {  0,  0 }  /* cbxCfpRuleAttrMax */
};

cbxi_cfp_rule_field_info_t
    cfp_udf_rule_field_info[cbxCfpRuleAttrMax + 1] = {
    { 16, 173 }, /* cbxCfpRuleAttrSourcePort */
    {  3, 170 }, /* cbxCfpRuleAttrIncomingTrafficClass */
    {  2, 168 }, /* cbxCfpRuleAttrOuterVlanTagStatus */
    {  2, 166 }, /* cbxCfpRuleAttrInnerVlanTagStatus */
    {  1, 165 }, /* cbxCfpRuleAttrMimTagStatus */
    {  1, 164 }, /* cbxCfpRuleAttrETagStatus */
    {  1, 163 }, /* cbxCfpRuleAttrEtherTypeStatus */
    {  1, 162 }, /* cbxCfpRuleAttrMplsStatus */
    {  1, 161 }, /* cbxCfpRuleAttrPppStatus */
    {  4, 157 }, /* cbxCfpRuleAttrL3Type */
    {  0,  0  }, /* cbxCfpRuleAttrRangeResult */
    {  0,  0  }, /* cbxCfpRuleAttrDestMac */
    {  0,  0  }, /* cbxCfpRuleAttrSrcMac */
    {  0,  0  }, /* cbxCfpRuleAttrOuterVlanId */
    {  0,  0  }, /* cbxCfpRuleAttrInnerVlanId */
    {  0,  0  }, /* cbxCfpRuleAttrEtherType  */
    {  0,  0  }, /* cbxCfpRuleAttrL4Type */
    {  0,  0  }, /* cbxCfpRuleAttrL3TtlSummary */
    {  0,  0  }, /* cbxCfpRuleAttrL3FragSummary */
    {  0,  0  }, /* cbxCfpRuleAttrSrcIp */
    {  0,  0  }, /* cbxCfpRuleAttrDestIp */
    {  0,  0  }, /* cbxCfpRuleAttrDscp */
    {  0,  0  }, /* cbxCfpRuleAttrL4SrcPort */
    {  0,  0  }, /* cbxCfpRuleAttrL4DestPort */
    {  0,  0  }, /* cbxCfpRuleAttrIcmpCode */
    {  0,  0  }, /* cbxCfpRuleAttrIgmpType */
    {  0,  0  }, /* cbxCfpRuleAttrTcpControl */
    {  0,  0  }, /* cbxCfpRuleAttrSrcIp6 */
    {  0,  0  }, /* cbxCfpRuleAttrSrcIp6High */
    {  0,  0  }, /* cbxCfpRuleAttrSrcIp6Low */
    {  0,  0  }, /* cbxCfpRuleAttrDestIp6 */
    {  0,  0  }, /* cbxCfpRuleAttrDestIp6High */
    {  0,  0  }, /* cbxCfpRuleAttrDestIp6Low */
    { 64,  93 }, /* cbxCfpRuleAttrUDF0 */
    { 64,  29 }, /* cbxCfpRuleAttrUDF1 */
#if 0
    { 32, 352 }, /* cbxCfpRuleAttrUDF2 */
    { 32, 320 }, /* cbxCfpRuleAttrUDF3 */
    { 16, 304 }, /* cbxCfpRuleAttrUDF4 */
    { 16, 288 }, /* cbxCfpRuleAttrUDF5 */
    { 16, 272 }, /* cbxCfpRuleAttrUDF6 */
    { 16, 256 }, /* cbxCfpRuleAttrUDF7 */
#endif
    { 32, 349 }, /* cbxCfpRuleAttrUDF2 */
    { 32, 317 }, /* cbxCfpRuleAttrUDF3 */
    { 16, 301 }, /* cbxCfpRuleAttrUDF4 */
    { 16, 285 }, /* cbxCfpRuleAttrUDF5 */
    { 16, 269 }, /* cbxCfpRuleAttrUDF6 */
    { 16, 253 }, /* cbxCfpRuleAttrUDF7 */
    {  0,  0  }  /* cbxCfpRuleAttrMax */
};

cbxi_cfp_rule_field_info_t  dbid_field_info = { 3, 189 }; /* cbxCfpRuleAttrSourcePort */

cbxi_cfp_ikft_info_t key_info[] = {
       /* Length, Header type, Offset-255 HWH 31 MAC, 3 CTAG 69 TCP*/
       { 32, 255, 416 },    /*  F0 */
       { 8,  255, 408 },    /*  F1 */
       { 5,  255, 387 },    /*  F2 */
       { 7,  255, 448 },    /*  F3 */
       { 48,  31,   0 },    /*  F4 */
       { 48,  31,  48 },    /*  F5 */
       { 12, 255, 372 },    /*  F6 */
       { 12,   3,  20 },    /*  F7 */
       { 16, 255, 352 },    /*  F8 */
       { 64, 255,  64 },    /*  F9 */
       { 64, 255, 192 },    /*  F10 */
       { 64, 255, 128 },    /*  F11 */
       { 64, 255, 256 },    /*  F12 */
       { 8,  255,   4 },    /*  F13 */
       { 8,  255,  56 },    /*  F14 */
       { 8,  255,  48 },    /*  F15 */
       { 32, 255, 320 },    /*  F16 */
       { 6,   69, 106 },    /*  F17 */
       { 64,  31,  96 },    /*  F24 - UDF0 */
       { 64,  31, 160 },    /*  F25 - UDF1 */
       { 32,  31, 224 },    /*  F26 - UDF2 */
       { 32,  31, 272 },    /*  F27 - UDF3 */
       { 16,  31, 304 },    /*  F28 - UDF4 */
       { 16,  31, 320 },    /*  F29 - UDF5 */
       { 16,  31, 336 },    /*  F30 - UDF6 */
       { 16,  31, 352 },    /*  F31 - UDF7 */
       { 32, 255,  64 },    /*  F9 */
       { 32, 255, 192 },    /*  F10 */
       { 24,   9,   8 },    /*  F19 */
       { 20,  51,   0 },    /*  F20 */
};


/*
 * Function:
 *  cbxi_cfp_tcam_config
 * Purpose:
 *  Take TCAM out of reset and powerdown state
 *
 */
int
cbxi_cfp_tcam_config(int unit) {
    uint32  reg_data = 0;
    uint32  fval = 1;

    /* Take TCAM out of Valid Reset state */
    CBX_IF_ERROR_RETURN(REG_READ_CB_CFP_TCAM_CONFIGr(unit, &reg_data));
    soc_CB_CFP_TCAM_CONFIGr_field_set(unit, &reg_data, TCAM_RESETBf, &fval);
    CBX_IF_ERROR_RETURN(
           REG_WRITE_CB_CFP_TCAM_CONFIGr(unit, &reg_data));

    /* Take TCAM out of PowerDown State */
    fval = 0;
    soc_CB_CFP_TCAM_CONFIGr_field_set(unit, &reg_data, TCAM0_PWRDWNf, &fval);
    soc_CB_CFP_TCAM_CONFIGr_field_set(unit, &reg_data, TCAM1_PWRDWNf, &fval);
    soc_CB_CFP_TCAM_CONFIGr_field_set(unit, &reg_data, TCAM2_PWRDWNf, &fval);
    soc_CB_CFP_TCAM_CONFIGr_field_set(unit, &reg_data, TCAM3_PWRDWNf, &fval);
    CBX_IF_ERROR_RETURN(
           REG_WRITE_CB_CFP_TCAM_CONFIGr(unit, &reg_data));
    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_cfp_table_init
 * Purpose:
 *  Global Initialization for all cfp related tables.
 *
 */
int
cbxi_cfp_table_init(int unit) {

    int          max_index = 0;
    int          index = 0;
    uint32       status  = 0;
    art_t        art;
    /* Enable and reset CFP tcam */
    CBX_IF_ERROR_RETURN(soc_robo2_cfpcam_enable(unit));
    CBX_IF_ERROR_RETURN(soc_robo2_cfpcam_reset(unit));

    /* Enable and reset CFP action */
    CBX_IF_ERROR_RETURN(soc_robo2_action_enable(unit));
    CBX_IF_ERROR_RETURN(soc_robo2_action_reset(unit));

    /* Enable and reset CFP ECC table */
    CBX_IF_ERROR_RETURN(soc_robo2_tecc_enable(unit));
    CBX_IF_ERROR_RETURN(soc_robo2_tecc_reset(unit));

    /* Enable and reset CFP wide rule table */
    CBX_IF_ERROR_RETURN(soc_robo2_wrt_enable(unit));
    CBX_IF_ERROR_RETURN(soc_robo2_wrt_reset(unit));

    /* Enable and reset CFP STAT table */
    CBX_IF_ERROR_RETURN(soc_robo2_stat_enable(unit));
    CBX_IF_ERROR_RETURN(soc_robo2_stat_reset(unit));

    /* Enable and reset CFP action redirection table */
    CBX_IF_ERROR_RETURN(soc_robo2_art_enable(unit));
    CBX_IF_ERROR_RETURN(soc_robo2_art_reset(unit));

    max_index = soc_robo2_art_max_index(unit);

    for(index = 0; index < max_index; index++) {
        art.ndx = index;
        CBX_IF_ERROR_RETURN(soc_robo2_art_set(unit, index,
                                                  &art, &status));
    }
    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_cfp_ikft_key_set
 * Purpose:
 *  Set fields for the keys
 *
 */
int
cbxi_cfp_ikft_key_set(int unit, int key_location, int key_field) {
    ikft_t        ikft_entry;
    uint32        status  = 0;
#if 0
    cbxi_cfp_ikft_info_t key_info[] = {
                 /* Length, Header type, Offset-255 HWH 31 MAC, 3 CTAG 69 TCP*/
                         { 32, 255, 416 },    /*  F0 */
                         { 8,  255, 408 },    /*  F1 */
                         { 5,  255, 387 },    /*  F2 */
                         { 7,  255, 448 },    /*  F3 */
                         { 48,  31,   0 },    /*  F4 */
                         { 48,  31,  48 },    /*  F5 */
                         { 12, 255, 368 },    /*  F6 */
                         { 12,   3,  20 },    /*  F7 */
                         { 16, 255, 352 },    /*  F8 */
                         { 64, 255,  64 },    /*  F9 */
                         { 64, 255, 192 },    /*  F10 */
                         { 64, 255, 128 },    /*  F11 */
                         { 64, 255, 256 },    /*  F12 */
                         { 8,  255,   4 },    /*  F13 */
                         { 8,  255,  56 },    /*  F14 */
                         { 8,  255,  48 },    /*  F15 */
                         { 32, 255, 320 },    /*  F16 */
                         { 6,   69, 106 },    /*  F17 */
                         { 64,   7,   0 },    /*  F24 - UDF0 */
                         { 64,  45,   0 },    /*  F25 - UDF1 */
                     };
#endif
    /* Read entry from IKFT */
    CBX_IF_ERROR_RETURN(soc_robo2_ikft_get(unit, key_location, &ikft_entry));
    ikft_entry.key |= (1 << key_field);
    ikft_entry.length = key_info[key_location].length;
    ikft_entry.htype = key_info[key_location].header_type;
    ikft_entry.offset = key_info[key_location].header_offset;
    CBX_IF_ERROR_RETURN(soc_robo2_ikft_set(unit, key_location,
                                           &ikft_entry, &status));
    return CBX_E_NONE;
}
/*
 * Function:
 *  cbxi_cfp_create_default_lookup_keys
 * Purpose:
 *  Set CFP defaullt keys - L2 Ethernet, IPv4, IPv6, L2/IPv4
 *
 */
int
cbxi_cfp_create_default_lookup_keys(int unit){

    uint32_t key_fields;
    int        field = 0;

    key_fields = CBXI_IKFT_CFP_KEY_TYPE_L2;
    CBX_BMP_ITER(key_fields, field) {
        /* Enable key 0 for L2 packets  */
        CBX_IF_ERROR_RETURN(cbxi_cfp_ikft_key_set(unit, field,
                                                   CBXI_IKFT_L2_KEY_ID));
    }
    key_fields = CBXI_IKFT_CFP_KEY_TYPE_IPV4;
    CBX_BMP_ITER(key_fields, field) {
        /* Enable key 1 for IPv4 packets  */
        CBX_IF_ERROR_RETURN(cbxi_cfp_ikft_key_set(unit, field,
                                                   CBXI_IKFT_IPV4_KEY_ID));
    }
    key_fields = CBXI_IKFT_CFP_KEY_TYPE_IPV6_ODD;
    CBX_BMP_ITER(key_fields, field) {
        /* Enable key 2 for IPv6 packets - first part  */
        CBX_IF_ERROR_RETURN(cbxi_cfp_ikft_key_set(unit, field,
                                                   CBXI_IKFT_IPV6_KEY_ID_ODD));
    }
    key_fields = CBXI_IKFT_CFP_KEY_TYPE_IPV6_EVEN;
    CBX_BMP_ITER(key_fields, field) {
        /* Enable key 3 for IPv6 packets - second part  */
        CBX_IF_ERROR_RETURN(cbxi_cfp_ikft_key_set(unit, field,
                                                   CBXI_IKFT_IPV6_KEY_ID_EVEN));
    }
    key_fields = CBXI_IKFT_CFP_KEY_TYPE_L2_IPV4_ODD;
    CBX_BMP_ITER(key_fields, field) {
        /* Enable key 4 for L2 + IPV4 packets - first part  */
        CBX_IF_ERROR_RETURN(cbxi_cfp_ikft_key_set(unit, field,
                                                 CBXI_IKFT_L2_IPV4_KEY_ID_ODD));
    }
    key_fields = CBXI_IKFT_CFP_KEY_TYPE_L2_IPV4_EVEN;
    CBX_BMP_ITER(key_fields, field) {
        /* Enable key 5 for L2 + IPV4 packets - second part  */
        CBX_IF_ERROR_RETURN(cbxi_cfp_ikft_key_set(unit, field,
                                                 CBXI_IKFT_L2_IPV4_KEY_ID_EVEN));
    }
    key_fields = CBXI_IKFT_CFP_KEY_TYPE_UDF_ODD;
        /* Enable key 6 for UDF - first part  */
    CBX_BMP_ITER(key_fields, field) {
        CBX_IF_ERROR_RETURN(cbxi_cfp_ikft_key_set(unit, field,
                                                 CBXI_IKFT_UDF_KEY_ID_ODD));
    }
    key_fields = CBXI_IKFT_CFP_KEY_TYPE_UDF_EVEN;
        /* Enable key 6 for UDF - second part  */
    CBX_BMP_ITER(key_fields, field) {
        CBX_IF_ERROR_RETURN(cbxi_cfp_ikft_key_set(unit, field,
                                                 CBXI_IKFT_UDF_KEY_ID_EVEN));
    }

#ifdef CONFIG_VIRTUAL_PORT_SUPPORT
    /* Configure LIM keys */
    key_fields = CBXI_IKFT_LIM_KEY_TYPE_PV;
        /* Enable key 11 for LIM Port VID */
    CBX_BMP_ITER(key_fields, field) {
        CBX_IF_ERROR_RETURN(cbxi_cfp_ikft_key_set(unit, field,
                                                 CBXI_IKFT_LIM_PV_KEY_ID));
    }
    key_fields = CBXI_IKFT_LIM_KEY_TYPE_PVV;
        /* Enable key 12 for LIM Port VID-VID  */
    CBX_BMP_ITER(key_fields, field) {
        CBX_IF_ERROR_RETURN(cbxi_cfp_ikft_key_set(unit, field,
                                                 CBXI_IKFT_LIM_PVV_KEY_ID));
    }
    key_fields = CBXI_IKFT_LIM_KEY_TYPE_MIM;
        /* Enable key 13 for LIM Mac-in-Mac */
    CBX_BMP_ITER(key_fields, field) {
        CBX_IF_ERROR_RETURN(cbxi_cfp_ikft_key_set(unit, field,
                                                 CBXI_IKFT_LIM_MIM_KEY_ID));
    }
    key_fields = CBXI_IKFT_LIM_KEY_TYPE_MPLS;
        /* Enable key 14 for LIM MPLS  */
    CBX_BMP_ITER(key_fields, field) {
        CBX_IF_ERROR_RETURN(cbxi_cfp_ikft_key_set(unit, field,
                                                 CBXI_IKFT_LIM_MPLS_KEY_ID));
    }
#endif /* CONFIG_VIRTUAL_PORT_SUPPORT */

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_fpslict_table_update
 * Purpose:
 *  Set the KST table entry to be used for selecting LBH and CFP keys
 *
 */
int
cbxi_fpslict_table_update(int unit) {
    int max_index = 0;
    int index = 1;
    fpslict_t     fpslict_entry;
    uint32        status  = 0;

    max_index = soc_robo2_fpslict_max_index(unit);

    for(index = 0; index <= max_index; index++) {
        CBX_IF_ERROR_RETURN(soc_robo2_fpslict_get(unit, index,
                                                  &fpslict_entry));
        fpslict_entry.other_keyset = CBXI_SLIC_LAG_CFP_KEYSET_OTHER;
        fpslict_entry.ipv4_keyset  = CBXI_SLIC_LAG_CFP_KEYSET_IPV4;
        fpslict_entry.ipv6_keyset  = CBXI_SLIC_LAG_CFP_KEYSET_IPV6;
        CBX_IF_ERROR_RETURN(soc_robo2_fpslict_set(unit, index,
                                      &fpslict_entry, &status));
    }
    return CBX_E_NONE;
}


/*
 * Function:
 *  cbxi_cfp_kst_entry_update
 * Purpose:
 *  Enable/disbale LBH/CFP and select LBH and 4 CFP keys
 *
 */
int
cbxi_cfp_kst_entry_update(int unit, int flag) {

    int           index = 0;
    kst_t         kst_entry;
    uint32        status  = 0;

    if (flag == CBXI_CFP_ENABLE) {
        for (index = CBXI_SLIC_LAG_CFP_KEYSET_OTHER;
                  index <= CBXI_SLIC_LAG_CFP_KEYSET_IPV6; index++) {

        /* KST_entry  lbh_en  cfp_en   lbh_key  cfp_key
         * 0           1       1           8      0,x,x,x
         * 1           1       1           9      1,3,0,x
         * 2           1       1          10      2,0,x,x
         */
            CBX_IF_ERROR_RETURN(soc_robo2_kst_get(unit, index, &kst_entry));

            /* Add LBH entries for default packet types IPv4, IPv6 and non-IP */
            kst_entry.lbh_en = 1; /* Set lbh_en bit for the entry */
            kst_entry.cfp_key0 = CBXI_IKFT_L2_KEY_ID; /*  cfp key 0 */
            kst_entry.cfp_key1 = CBXI_IKFT_IPV4_KEY_ID; /*  cfp key 1 */
            kst_entry.cfp_key2 = CBXI_IKFT_UDF_KEY_ID_EVEN; /*  cfp key 6 */
            kst_entry.cfp_key3 = CBXI_IKFT_UDF_KEY_ID_ODD; /*  cfp key 7 */
            kst_entry.dbid0 = CBXI_CFP_L2_KEY_DBID;       /*  dbid 0 */
            kst_entry.dbid1 = CBXI_CFP_IPV4_KEY_DBID;     /*  dbid 1 */
            kst_entry.dbid2 = CBXI_CFP_UDF_KEY_DBID;      /*  dbid 2 */
            kst_entry.dbid3 = CBXI_CFP_UDF_KEY_DBID;      /*  dbid 2 */
            kst_entry.cfp_key_en = 0xf; /* Set cfp_key_en bit for the entry */
            if (index == CBXI_SLIC_LAG_CFP_KEYSET_OTHER) {
                kst_entry.lbh_key = CBXI_IKFT_LBH_OTHER_KEY_ID;/* key 8*/
            } else if(index == CBXI_SLIC_LAG_CFP_KEYSET_IPV4) {
                kst_entry.lbh_key = CBXI_IKFT_LBH_IPV4_KEY_ID; /* key 9 */
                kst_entry.cfp_key0 = CBXI_IKFT_IPV4_KEY_ID;    /* key 1 */
                kst_entry.cfp_key1 = CBXI_IKFT_L2_KEY_ID;      /* key 0 */
                kst_entry.dbid0 = CBXI_CFP_IPV4_KEY_DBID;  /*  dbid 1 */
                kst_entry.dbid1 = CBXI_CFP_L2_KEY_DBID;    /*  dbid 0 */
                if (SOC_DRIVER(unit)->type != SOC_CHIP_BCM53158_A0) {
                    kst_entry.cfp_key2 = CBXI_IKFT_L2_IPV4_KEY_ID_EVEN; /*  cfp key 6 */
                    kst_entry.cfp_key3 = CBXI_IKFT_L2_IPV4_KEY_ID_ODD; /*  cfp key 7 */
                    kst_entry.dbid2 = CBXI_CFP_L2_IPV4_KEY_DBID;      /*  dbid 2 */
                    kst_entry.dbid3 = CBXI_CFP_L2_IPV4_KEY_DBID;      /*  dbid 2 */
                }
                kst_entry.cfp_key_en = 0xf; /* Set cfp_key_en bit for the entry */
            } else {
                kst_entry.lbh_key = CBXI_IKFT_LBH_IPV6_KEY_ID ;/* key 10 */
                kst_entry.cfp_key0 = CBXI_IKFT_IPV6_KEY_ID_EVEN; /*   key 2 */
                kst_entry.cfp_key1 = CBXI_IKFT_IPV6_KEY_ID_ODD; /*   key 3 */
                kst_entry.dbid0 = CBXI_CFP_IPV6_KEY_DBID;  /*  dbid 3 */
                kst_entry.dbid1 = CBXI_CFP_IPV6_KEY_DBID;  /*  dbid 3 */
                kst_entry.cfp_key_en = 0xf; /* Set cfp_key_en bit for the entry */
            }
            CBX_IF_ERROR_RETURN(soc_robo2_kst_set(unit, index,
                                              &kst_entry, &status));
        }
    } else {
        for (index = CBXI_SLIC_LAG_CFP_KEYSET_OTHER;
                  index <= CBXI_SLIC_LAG_CFP_KEYSET_IPV6; index++) {
            CBX_IF_ERROR_RETURN(soc_robo2_kst_get(unit, index, &kst_entry));
            kst_entry.cfp_key_en = 0; /* disable cfp_key_en bit for the entry */
            CBX_IF_ERROR_RETURN(soc_robo2_kst_set(unit, index,
                                              &kst_entry, &status));
        }
    }
    return CBX_E_NONE;
}

/*
 * Function:
 *     cbxi_cfp_get_first_port_set
 * Purpose:
 *     Get first PP set in the FOV
 *
 */
int
cbxi_cfp_get_first_port_set(lpg2ppfov_t ppfov_entry, cbx_portid_t *portid)
{
    pbmp_t     pp_map;
    cbx_port_t port = 0;
    CBX_PBMP_CLEAR(pp_map);
    CBX_PBMP_ASSIGN(pp_map,ppfov_entry.ppfov);
    CBX_PBMP_ITER(pp_map, port) {
        *portid = port;
        return CBX_E_NONE;
    }
    return CBX_E_NONE;
}
/*
 * Function:
 *     cbxi_cfp_action_convert_entry_to_param
 * Purpose:
 *     Convert HW action entry to API parameters
 *
 */
int
cbxi_cfp_action_convert_entry_to_param(int unit, action_t entry, cbx_cfp_action_t *action)
{
    pg2lpg_t      pg2lpg_entry;
    lpg2ppfov_t   ppfov_entry;
    cbxi_pgid_t   gpg;
    /*Non meter actions - FLAGS */
    if (entry.override == 1) {
        action->flags |= CBX_CFP_ACTION_OVERRIDE;
    }
    if (entry.change_dp == 1) {
        action->flags |= CBX_CFP_ACTION_UPDATE_DP;
    }
    if (entry.sli_lpbk_ena == 1) {
        action->flags |= CBX_CFP_ACTION_LOOP_ENABLE;
    }
    if (entry.dlf_override == 1) {
        action->flags |= CBX_CFP_ACTION_DLF;
    }
    if (entry.ipf_change_tc == 1) {
        action->flags |= CBX_CFP_ACTION_IN_POLICY_UPDATE_PRI;
    }
    if (entry.opf_change_tc == 1) {
        action->flags |= CBX_CFP_ACTION_OUT_POLICY_UPDATE_PRI;
    }
    if (entry.ipf_learn_disable == 1) {
        action->flags |= CBX_CFP_ACTION_IN_POLICY_LEARN_DISABLE;
    }
    if (entry.opf_learn_disable == 1) {
        action->flags |= CBX_CFP_ACTION_OUT_POLICY_LEARN_DISABLE;
    }
    /*Non meter actions */
    if (entry.meter) {
        CBX_IF_ERROR_RETURN(cbxi_meter_id_create(unit, entry.meter,
                     cbxMeterCFP, &(action->common_action.meterid)));
    }
    action->common_action.dp = entry.dp;
    action->common_action.count_mode = entry.counter_mode;
    /* IN profile actions */
    action->in_policy_action.pri = entry.ipf_tc;
    action->in_policy_action.trapid = entry.ipf_trap_offset;
    action->in_policy_action.mirrorid = entry.ipf_mirror;

    action->in_policy_action.forward_mode = entry.ipf_change_fwd;

    action->in_policy_action.sourceid = CBX_PORT_INVALID;
    action->in_policy_action.ucastid = CBX_PORT_INVALID;
    gpg = ((entry.ipf_dliid_dgid_sliid >> 12) & 0x1F);
    CBX_IF_ERROR_RETURN(soc_robo2_pg2lpg_get(unit,
                                             gpg, &pg2lpg_entry));
    CBX_IF_ERROR_RETURN(soc_robo2_lpg2ppfov_get(unit,
                                           pg2lpg_entry.lpg, &ppfov_entry));
    if (entry.ipf_change_fwd == cbxCFPForwardUnicast) {
        /* Get first port that is set in fov */
        CBX_IF_ERROR_RETURN(cbxi_cfp_get_first_port_set(ppfov_entry,
                                   &(action->in_policy_action.ucastid)));
        if (action->in_policy_action.ucastid != CBX_PORT_INVALID) {
            CBX_PORTID_LOCAL_SET(action->in_policy_action.ucastid,
                                    action->in_policy_action.ucastid);
        }
    } else if (entry.ipf_change_fwd == cbxCFPForwardMulticast) {
        CBX_PORTID_MCAST_ID_SET(action->in_policy_action.mcastid,
                                     entry.ipf_dliid_dgid_sliid);
    } else if (entry.ipf_change_fwd == cbxCFPForwardFrom) {
        /* Get first port that is set in fov */
        CBX_IF_ERROR_RETURN(cbxi_cfp_get_first_port_set(ppfov_entry,
                                       &(action->in_policy_action.sourceid)));
        if (action->in_policy_action.sourceid != CBX_PORT_INVALID) {
            CBX_PORTID_LOCAL_SET(action->in_policy_action.sourceid,
                                    action->in_policy_action.sourceid);
        }
    }
    /* Out of profile actions */
    action->out_policy_action.forward_mode = entry.opf_change_fwd;
    action->out_policy_action.sourceid = CBX_PORT_INVALID;
    action->out_policy_action.ucastid = CBX_PORT_INVALID;
    gpg = ((entry.ipf_dliid_dgid_sliid >> 12) & 0x1F);
    CBX_IF_ERROR_RETURN(soc_robo2_pg2lpg_get(unit,
                                             gpg, &pg2lpg_entry));
    CBX_IF_ERROR_RETURN(soc_robo2_lpg2ppfov_get(unit,
                                           pg2lpg_entry.lpg, &ppfov_entry));
    if (entry.opf_change_fwd == cbxCFPForwardUnicast) {
        /* Get first port that is set in fov */
        CBX_IF_ERROR_RETURN(cbxi_cfp_get_first_port_set(ppfov_entry,
                                   &(action->out_policy_action.ucastid)));
        if (action->out_policy_action.ucastid != CBX_PORT_INVALID) {
            CBX_PORTID_LOCAL_SET(action->out_policy_action.ucastid,
                                    action->out_policy_action.ucastid);
        }
    }
    if (entry.opf_change_fwd == cbxCFPForwardMulticast) {
        CBX_PORTID_MCAST_ID_SET(action->out_policy_action.mcastid,
                                     entry.opf_dliid_dgid_sliid);
    }
    if (entry.opf_change_fwd == cbxCFPForwardFrom) {
        /* Get first port that is set in fov */
        CBX_IF_ERROR_RETURN(cbxi_cfp_get_first_port_set(ppfov_entry,
                                   &(action->out_policy_action.sourceid)));
        if (action->out_policy_action.sourceid != CBX_PORT_INVALID) {
            CBX_PORTID_LOCAL_SET(action->out_policy_action.sourceid,
                                    action->out_policy_action.sourceid);
        }
    }
    action->out_policy_action.pri = entry.opf_tc;
    action->out_policy_action.trapid = entry.opf_trap_offset;
    action->out_policy_action.mirrorid = entry.opf_mirror;
    return CBX_E_NONE;
}

/*
 * Function:
 *     cbxi_cfp_action_dump_param
 * Purpose:
 *     Dump CFP action parameters
 *
 */
int
cbxi_cfp_action_dump_param(cbx_cfp_action_t *action) {

    sal_printf("===========================================================\n");
    sal_printf("  FLAGS = 0x%x\n",action->flags);
    sal_printf("---------------------- Common Actions ---------------------\n");
    sal_printf(" Meter Id = 0x%x        DP = 0x%x        Counter Mode = 0x%x\n",
                 action->common_action.meterid,
                 action->common_action.dp,
                 action->common_action.count_mode);
    sal_printf("----------------  In Profile Meter Actions ----------------\n");
    sal_printf(" Forward Mode = 0x%x ucast id = 0x%x  Mcast Id = 0x%x\n",
                 action->in_policy_action.forward_mode,
                 action->in_policy_action.ucastid,
                 action->in_policy_action.mcastid);
    sal_printf(" Source Id = 0x%x pri = 0x%x trap Id = 0x%x\n",
                 action->in_policy_action.sourceid,
                 action->in_policy_action.pri,
                 action->in_policy_action.trapid);
    sal_printf(" Mirror Id = 0x%x\n",
                 action->in_policy_action.mirrorid);
    sal_printf("---------------  Out Of Profle Meter Actions --------------\n");
    sal_printf(" Forward Mode = 0x%x ucast id = 0x%x  Mcast Id = 0x%x\n",
                 action->out_policy_action.forward_mode,
                 action->out_policy_action.ucastid,
                 action->out_policy_action.mcastid);
    sal_printf(" Source Id = 0x%x pri = 0x%x trap Id = 0x%x\n",
                 action->out_policy_action.sourceid,
                 action->out_policy_action.pri,
                 action->out_policy_action.trapid);
    sal_printf(" Mirror Id = 0x%x\n",
                 action->out_policy_action.mirrorid);
    sal_printf("===========================================================\n");
    return CBX_E_NONE;
}

/*
 * Function:
 *     cbxi_cfp_action_convert_param_to_entry
 * Purpose:
 *     Convert action parameters to HW entry
 *
 */
int
cbxi_cfp_action_convert_param_to_entry(int unit,
                                       cbx_cfp_action_t *action,
                                       action_t  *entry) {
    cbx_port_t  port_out;
    cbxi_pgid_t lpg;
    cbxi_pgid_t gpg;
    if (action->flags & CBX_CFP_ACTION_IN_POLICY_LEARN_DISABLE) {
        entry->ipf_learn_disable = 1;
    }
    if (action->flags & CBX_CFP_ACTION_OUT_POLICY_LEARN_DISABLE) {
        entry->opf_learn_disable = 1;
    }
    entry->ipf_trap_offset = action->in_policy_action.trapid;
    entry->ipf_mirror = action->in_policy_action.mirrorid;
    if (action->in_policy_action.forward_mode == cbxCFPForwardUnicast) {
        CBX_IF_ERROR_RETURN(cbxi_robo2_port_validate(
                      action->in_policy_action.ucastid, &port_out, &lpg, &unit));

        CBX_IF_ERROR_RETURN(cbxi_robo2_lpg_to_gpg(unit, lpg, &gpg));
        entry->ipf_dliid_dgid_sliid = gpg << 12;
        entry->ipf_dliid_dgid_sliid |= 0x20001; /*Set PV type= 1 and VSI =1 */
    }
    if (action->in_policy_action.forward_mode == cbxCFPForwardMulticast) {
        entry->ipf_dliid_dgid_sliid =
                  CBX_PORTID_MCAST_ID_GET(action->in_policy_action.mcastid);
    }
    if (action->in_policy_action.forward_mode == cbxCFPForwardFrom) {
        CBX_IF_ERROR_RETURN(cbxi_robo2_port_validate(
                    action->in_policy_action.sourceid, &port_out, &lpg, &unit));
        CBX_IF_ERROR_RETURN(cbxi_robo2_lpg_to_gpg(unit, lpg, &gpg));
        entry->ipf_dliid_dgid_sliid = gpg << 12;
        entry->ipf_dliid_dgid_sliid |= 0x20001; /*Set PV type= 1 and VSI =1 */
    }
    entry->ipf_change_fwd = action->in_policy_action.forward_mode;
    /* No inputs from user to configure cfp_decap */
    entry->ipf_cfp_decap = 0;
    if (action->flags & CBX_CFP_ACTION_IN_POLICY_UPDATE_PRI) {
        entry->ipf_change_tc = 1;
        entry->ipf_tc = action->in_policy_action.pri;
    }
    if (action->flags & CBX_CFP_ACTION_OUT_POLICY_UPDATE_PRI) {
        entry->opf_change_tc = 1;
        entry->opf_tc = action->out_policy_action.pri;
    }
    entry->opf_trap_offset = action->out_policy_action.trapid;
    entry->opf_mirror = action->out_policy_action.mirrorid;

    entry->opf_change_fwd = action->out_policy_action.forward_mode;
    if (action->out_policy_action.forward_mode == cbxCFPForwardUnicast) {
        CBX_IF_ERROR_RETURN(cbxi_robo2_port_validate(
                      action->out_policy_action.ucastid, &port_out, &lpg, &unit));
        CBX_IF_ERROR_RETURN(cbxi_robo2_lpg_to_gpg(unit, lpg, &gpg));
        entry->opf_dliid_dgid_sliid = gpg << 12;
        entry->opf_dliid_dgid_sliid |= 0x20001; /*Set PV type= 1 and VSI =1 */
    }
    if (action->out_policy_action.forward_mode == cbxCFPForwardMulticast) {
        entry->opf_dliid_dgid_sliid =
                  CBX_PORTID_MCAST_ID_GET(action->out_policy_action.mcastid);
    }
    if (action->out_policy_action.forward_mode == cbxCFPForwardFrom) {
        CBX_IF_ERROR_RETURN(cbxi_robo2_port_validate(
                    action->out_policy_action.sourceid, &port_out, &lpg, &unit));
        CBX_IF_ERROR_RETURN(cbxi_robo2_lpg_to_gpg(unit, lpg, &gpg));
        entry->opf_dliid_dgid_sliid = gpg << 12;
        entry->opf_dliid_dgid_sliid |= 0x20001; /*Set PV type= 1 and VSI =1 */
    }
    /* No inputs from user to configure cfp_decap */
    entry->opf_cfp_decap = 0;
    entry->counter_mode = action->common_action.count_mode;
    entry->meter = (action->common_action.meterid & CBXI_METER_INDEX_MASK);
    if (action->flags & CBX_CFP_ACTION_OVERRIDE) {
        entry->override = 1;
    }
    if (action->flags & CBX_CFP_ACTION_UPDATE_DP) {
        entry->change_dp = 1;
        entry->dp = action->common_action.dp;
    }
    if (action->flags & CBX_CFP_ACTION_LOOP_ENABLE) {
        entry->sli_lpbk_ena = 1;
        entry->ipf_change_fwd = cbxCFPForwardLoopback;
        entry->opf_change_fwd = cbxCFPForwardLoopback;
    }
    if (action->flags & CBX_CFP_ACTION_DLF) {
        entry->dlf_override = 1;
    }
    return CBX_E_NONE;
}

/*
 * Function:
 *     cbxi_cfp_action_param_validate
 * Purpose:
 *     Validate cfp action parameters
 *
 */
int
cbxi_cfp_action_param_validate(cbx_cfp_action_t *action, int index) {


    int         max_index = 0;
    int         unit = CBX_AVENGER_PRIMARY;
    int         rv = CBX_E_NONE;
    cbx_port_t  port_out;
    cbxi_pgid_t lpg;
    int         valid = 0;
    cbx_meter_type_t type;

    /* validate the index passed */
    max_index = soc_robo2_action_max_index(CBX_AVENGER_PRIMARY);
    if ((max_index < 0) || (index > max_index)) {
        return CBX_E_PARAM;
    }
    if (action->flags & (~CBXI_CFP_SUPPORTED_ACTION_FLAGS)) {
        return CBX_E_PARAM;
    }
    if (action->flags & CBX_CFP_ACTION_LOOP_ENABLE) {
        if ((action->in_policy_action.forward_mode != cbxCFPForwardLoopback) &&
            (action->in_policy_action.forward_mode != cbxCFPForwardNoChange)) {
            return CBX_E_PARAM;
        }
        if ((action->out_policy_action.forward_mode != cbxCFPForwardLoopback) &&
            (action->out_policy_action.forward_mode != cbxCFPForwardNoChange)) {
            return CBX_E_PARAM;
        }
    }
    if (action->common_action.meterid) {
        valid = (((action->common_action.meterid) & CBXI_METER_INDEX_VALID)
                                          >> CBXI_METER_INDEX_VALID_SHIFT);
        type = ((action->common_action.meterid) &
                      CBXI_METER_TYPE_MASK) >> CBXI_METER_TYPE_SHIFT;
        if ((valid == 0) || (type != cbxMeterCFP)) {
            return CBX_E_PARAM;
        }
    }
    if ((action->common_action.dp) > CBXI_CFP_MAX_INT_DP) {
        return CBX_E_PARAM;
    }
    if ((action->common_action.count_mode) > cbxCFPCountPacketsByProfile) {
        return CBX_E_PARAM;
    }
    if ((action->in_policy_action.forward_mode) > cbxCFPForwardOverrideDrop) {
        return CBX_E_PARAM;
    }
    if ((action->common_action.dp) > CBXI_CFP_MAX_INT_DP) {
        return CBX_E_PARAM;
    }
    if ((action->common_action.count_mode) > cbxCFPCountPacketsByProfile) {
        return CBX_E_PARAM;
    }
    if ((action->in_policy_action.forward_mode) > cbxCFPForwardOverrideDrop) {
        return CBX_E_PARAM;
    }
    if (action->in_policy_action.forward_mode == cbxCFPForwardUnicast) {
        rv = cbxi_robo2_port_validate(
                      action->in_policy_action.ucastid, &port_out, &lpg, &unit);
        if (CBX_FAILURE(rv)) {
            return CBX_E_PARAM;
        }
    }
    if ((action->in_policy_action.forward_mode == cbxCFPForwardMulticast) &&
        (action->in_policy_action.forward_mode > 0)) {
        if(CBX_PORTID_MCAST_ID_GET(action->in_policy_action.mcastid) > CBX_MCAST_MAX) {
            return CBX_E_PARAM;
        }
    }
    if (action->in_policy_action.forward_mode == cbxCFPForwardFrom) {
        rv = cbxi_robo2_port_validate(
                    action->out_policy_action.sourceid, &port_out, &lpg, &unit);
        if (CBX_FAILURE(rv)) {
            return CBX_E_PARAM;
        }
    }
    if ((action->in_policy_action.pri) > CBXI_CFP_MAX_INT_PRI) {
        return CBX_E_PARAM;
    }
    if ((action->in_policy_action.trapid) > CBXI_CFP_MAX_TRAP_ID) {
        return CBX_E_PARAM;
    }
    if ((action->in_policy_action.mirrorid) > CBX_MIRROR_DEST_MAX) {
        return CBX_E_PARAM;
    }
    if (action->out_policy_action.forward_mode == cbxCFPForwardUnicast) {
        rv = cbxi_robo2_port_validate(
                    action->out_policy_action.ucastid, &port_out, &lpg, &unit);
        if (CBX_FAILURE(rv)) {
            return CBX_E_PARAM;
        }
    }
    if ((action->out_policy_action.forward_mode == cbxCFPForwardMulticast) &&
        (action->out_policy_action.forward_mode > 0)) {
        if(CBX_PORTID_MCAST_ID_GET(action->out_policy_action.mcastid) > CBX_MCAST_MAX) {
            return CBX_E_PARAM;
        }
    }
    if (action->out_policy_action.forward_mode == cbxCFPForwardFrom) {
        rv = cbxi_robo2_port_validate(
                    action->out_policy_action.sourceid, &port_out, &lpg, &unit);
        if (CBX_FAILURE(rv)) {
            return CBX_E_PARAM;
        }
    }
    if ((action->out_policy_action.pri) > CBXI_CFP_MAX_INT_PRI) {
        return CBX_E_PARAM;
    }
    if ((action->out_policy_action.trapid) > CBXI_CFP_MAX_TRAP_ID) {
        return CBX_E_PARAM;
    }
    if ((action->out_policy_action.mirrorid) > CBX_MIRROR_DEST_MAX) {
        return CBX_E_PARAM;
    }
    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_cfp_action_op
 * Purpose:
 *  ADD/DELETE/DUMP CFP action table entry
 *
 */
int
cbxi_cfp_action_op(int unit, int index,
                   cbx_cfp_action_t *action, int operation) {

    action_t     entry;
    uint32       status = 0;

    sal_memset(&entry, 0, sizeof(action_t));
    if (operation == CBXI_CFP_ACTION_ADD) {
        /* Convert HW entry to API parameter format */
        CBX_IF_ERROR_RETURN(cbxi_cfp_action_convert_param_to_entry(unit, action,
                                                                   &entry));
        /* Write entry to HW */
        CBX_IF_ERROR_RETURN(soc_robo2_action_set(unit, index,
                                              &entry, &status));

    } else if (operation == CBXI_CFP_ACTION_DELETE) {
        CBX_IF_ERROR_RETURN(soc_robo2_action_set(unit, index,
                                              &entry, &status));

    } else {  /* dump */
        cbx_cfp_action_t_init(action);
        /* Read action entry from HW */
        CBX_IF_ERROR_RETURN(soc_robo2_action_get(unit, index, &entry));
        /* Convert HW entry to API parameter formati */
        CBX_IF_ERROR_RETURN(cbxi_cfp_action_convert_entry_to_param(unit, entry,
                                                                   action));
        /* Dump the entry */
        CBX_IF_ERROR_RETURN(cbxi_cfp_action_dump_param(action));
    }
    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_cfp_rule_attr_bmp_set
 * Purpose:
 *   Init valid attributes bmp for each key type
 *
 */
void
cbxi_cfp_rule_attr_bmp_set() {

    uint8_t                  attr_valid = 0;
    cbx_cfp_rule_attribute_t attr;

    CBX_BMP_CLEAR(l2_rule_attr_bmp);
    CBX_BMP_CLEAR(v4_rule_attr_bmp);
    CBX_BMP_CLEAR(v6_rule_attr_bmp_0);
    CBX_BMP_CLEAR(v6_rule_attr_bmp_1);
    CBX_BMP_CLEAR(l2_v4_rule_attr_bmp_0);
    CBX_BMP_CLEAR(l2_v4_rule_attr_bmp_1);
    CBX_BMP_CLEAR(udf_rule_attr_bmp_0);
    CBX_BMP_CLEAR(udf_rule_attr_bmp_1);

    for (attr = cbxCfpRuleAttrSourcePort;
                          attr < cbxCfpRuleAttrDestIp6Low; attr++) {
        attr_valid = cfp_rule_attr_info[attr].valid_for_key;
        CBX_BMP_OR(l2_rule_attr_bmp, ((attr_valid & 1) << attr));
        attr_valid = attr_valid >> 1;
        CBX_BMP_OR(v4_rule_attr_bmp, ((attr_valid & 1) << attr));
        attr_valid = attr_valid >> 1;
        CBX_BMP_OR(v6_rule_attr_bmp_0, ((attr_valid & 1) << attr));
        attr_valid = attr_valid >> 1;
        CBX_BMP_OR(l2_v4_rule_attr_bmp_0, ((attr_valid & 1) << attr));
        attr_valid = attr_valid >> 1;
        CBX_BMP_OR(udf_rule_attr_bmp_0, ((attr_valid & 1) << attr));
    }
    for (attr = cbxCfpRuleAttrDestIp6Low; attr < cbxCfpRuleAttrMax; attr++) {
        attr_valid = cfp_rule_attr_info[attr].valid_for_key;
        /* get bmp for IPv6 */
        attr_valid = attr_valid >> 2;
        CBX_BMP_OR(v6_rule_attr_bmp_1,
                  ((attr_valid & 1) << (attr - cbxCfpRuleAttrDestIp6Low)));
        /* get bmp for UDF */
        attr_valid = attr_valid >> 2;
        CBX_BMP_OR(udf_rule_attr_bmp_1,
                  ((attr_valid & 1) << (attr - cbxCfpRuleAttrDestIp6Low)));
    }
    return;
}

/*
 * Function:
 *  cbxi_cfp_prefill_attribute
 * Purpose:
 *  Prefill all valid attribute names, their default value/mask
 *  for a given key type
 *
 */
void
cbxi_cfp_prefill_attribute(cbx_cfp_rule_t *rule, cbx_cfp_l3_l4_pkt_type_t pkt_type) {
    uint32_t                 attr_bmp[2];
    cbx_cfp_rule_attribute_t attr;
    uint32_t                 index = 0;
    uint32_t                 i = 0;
    uint32_t                 no_of_bmps = 1;
    uint32_t                 bmp_no = 0;

    CBX_BMP_CLEAR(attr_bmp[0]);
    CBX_BMP_CLEAR(attr_bmp[1]);

    switch (rule->key_type) {
        case cbxCfpKeyL2EthernetACL:
            CBX_BMP_ASSIGN(attr_bmp[0], l2_rule_attr_bmp);
            break;
        case cbxCfpKeyIPv4ACL:
            CBX_BMP_ASSIGN(attr_bmp[0], v4_rule_attr_bmp);
            break;
        case cbxCfpKeyIPv6ACL:
            CBX_BMP_ASSIGN(attr_bmp[0], v6_rule_attr_bmp_0);
            CBX_BMP_ASSIGN(attr_bmp[1], v6_rule_attr_bmp_1);
            no_of_bmps = 2;
            break;
        case cbxCfpKeyL2IPv4ACL:
            CBX_BMP_ASSIGN(attr_bmp[0], l2_v4_rule_attr_bmp_0);
            CBX_BMP_ASSIGN(attr_bmp[1], l2_v4_rule_attr_bmp_1);
            break;
        case cbxCfpKeyUDFACL:
            CBX_BMP_ASSIGN(attr_bmp[0], udf_rule_attr_bmp_0);
            CBX_BMP_ASSIGN(attr_bmp[1], udf_rule_attr_bmp_1);
            no_of_bmps = 2;
            break;
        default:
            return;
    }
    cbxi_cfp_update_valid_bmp(rule->key_type, pkt_type, &attr_bmp[0]);
    while (bmp_no < no_of_bmps) {
        CBX_BMP_ITER(attr_bmp[bmp_no], attr) {
            index = 0;
            rule->attr_selector[i].attr = attr + (32 * bmp_no);
            if ((rule->attr_selector[i].attr_val != NULL) &&
                (rule->attr_selector[i].attr_len >=
                          cfp_rule_attr_info[rule->attr_selector[i].attr].attr_len)) {
                sal_memset(&(rule->attr_selector[i].attr_val[0]), 0,
                          cfp_rule_attr_info[rule->attr_selector[i].attr].attr_len);
            }
            rule->attr_selector[i].attr_len =
                          cfp_rule_attr_info[rule->attr_selector[i].attr].attr_len;
            if ((rule->attr_selector[i].attr_mask != NULL) &&
                (rule->attr_selector[i].attr_len >=
                             cfp_rule_attr_info[rule->attr_selector[i].attr].attr_len)) {
                while(index < cfp_rule_attr_info[rule->attr_selector[i].attr].attr_len) {
                    rule->attr_selector[i].attr_mask[index] = 0x0;
                    index++;
                }
            }
            i++;
        }
        bmp_no++;
    }
    rule->num_attributes = i;
    return;
}

/*
 * Function:
 *    cbxi_cfp_validate_rule_attribute_values
 * Purpose:
 *    Validate values of all the attributes passed for rule creation
 *
 */
int
cbxi_cfp_validate_rule_attribute_values(cbx_cfp_rule_attribute_t attr,
                                        uint32_t val) {
    switch(attr) {
        case cbxCfpRuleAttrSourcePort:
            break;
        case cbxCfpRuleAttrIncomingTrafficClass:
            if (val > CBXI_CFP_MAX_INT_PRI) {
                return CBX_E_PARAM;
            }
            break;
        case cbxCfpRuleAttrOuterVlanTagStatus:
            if (val > CBX_CFP_ATTR_OUTER_VLAN_SINGLE_TAG) {
                return CBX_E_PARAM;
            }
            break;
        case cbxCfpRuleAttrInnerVlanTagStatus:
            if (val > CBX_CFP_ATTR_INNER_VLAN_SINGLE_TAG) {
                return CBX_E_PARAM;
            }
            break;
        case cbxCfpRuleAttrMimTagStatus:
        case cbxCfpRuleAttrETagStatus:
        case cbxCfpRuleAttrMplsStatus:
        case cbxCfpRuleAttrEtherTypeStatus:
        case cbxCfpRuleAttrPppStatus:
            if (val > CBX_CFP_ATTR_INNER_VLAN_PRESENT) {
                return CBX_E_PARAM;
            }
            break;
        case cbxCfpRuleAttrL3Type:
            if (val > CBX_CFP_L3_TYPE_ARP) {
                return CBX_E_PARAM;
            }
            break;
        case cbxCfpRuleAttrRangeResult:
            if (val > 0xFF) {
                return CBX_E_PARAM;
            }
            break;
        case cbxCfpRuleAttrOuterVlanId:
            if (val > 4095) {
                return CBX_E_PARAM;
            }
            break;
        case cbxCfpRuleAttrInnerVlanId:
            if (val > 4095) {
                return CBX_E_PARAM;
            }
            break;
        case cbxCfpRuleAttrEtherType:
            break;
        case cbxCfpRuleAttrL4Type:
            if (val > CBX_CFP_L4_TYPE_IGMP) {
                return CBX_E_PARAM;
            }
            break;
        case cbxCfpRuleAttrL3TtlSummary:
            if (val > CBX_CFP_L3_TTL_255) {
                return CBX_E_PARAM;
            }
            break;
        case cbxCfpRuleAttrL3FragSummary:
            if (val > CBX_CFP_L3_FRAG_NONE) {
                return CBX_E_PARAM;
            }
            break;
        case cbxCfpRuleAttrDscp:
            if (val > 0xFF ) {
                return CBX_E_PARAM;
            }
            break;
        case cbxCfpRuleAttrL4SrcPort:
        case cbxCfpRuleAttrL4DestPort:
        case cbxCfpRuleAttrIcmpCode:
        case cbxCfpRuleAttrIgmpType:
            if (val > 0xFFFF) {
                return CBX_E_PARAM;
            }
            break;
        case cbxCfpRuleAttrTcpControl:
            if (val > CBX_CFP_TCP_CONTROL_URG) {
                return CBX_E_PARAM;
            }
            break;
        case cbxCfpRuleAttrDestMac:
        case cbxCfpRuleAttrSrcMac:
        case cbxCfpRuleAttrSrcIp:
        case cbxCfpRuleAttrDestIp:
        case cbxCfpRuleAttrSrcIp6:
        case cbxCfpRuleAttrSrcIp6High:
        case cbxCfpRuleAttrSrcIp6Low:
        case cbxCfpRuleAttrDestIp6:
        case cbxCfpRuleAttrDestIp6High:
        case cbxCfpRuleAttrDestIp6Low:
        case cbxCfpRuleAttrUDF0:
        case cbxCfpRuleAttrUDF1:
        case cbxCfpRuleAttrUDF2:
        case cbxCfpRuleAttrUDF3:
        case cbxCfpRuleAttrUDF4:
        case cbxCfpRuleAttrUDF5:
        case cbxCfpRuleAttrUDF6:
        case cbxCfpRuleAttrUDF7:
        default:
            return CBX_E_NONE;
    }
    return CBX_E_NONE;
}


/*
 * Function:
 *    cbxi_cfp_rule_param_validate
 * Purpose:
 *    Validate all the parameters passed for rule creation
 *
 */
int
cbxi_cfp_rule_param_validate(cbx_cfpid_t *cfpid,
                             cbx_cfp_rule_t *rule,
                             uint32_t *l4_pkt_type_attr) {
    uint32_t                 index = 0;
    uint32_t                 no_of_attributes = 0;
    uint32_t                 attr_bmp[2];
    uint32_t                 passed_bmp[2];
    uint32_t                 cmp_bmp[2];
    uint32_t                 v6_addr_attr;
    int                      l4_attr = 0;
    CBX_BMP_CLEAR(attr_bmp[0]);
    CBX_BMP_CLEAR(attr_bmp[1]);
    CBX_BMP_CLEAR(passed_bmp[0]);
    CBX_BMP_CLEAR(passed_bmp[1]);
    CBX_BMP_CLEAR(cmp_bmp[0]);
    CBX_BMP_CLEAR(cmp_bmp[1]);
    /* validate the handle passed */
    if ((cfpid == NULL)) {
        LOG_ERROR(BSL_LS_FSAL_CFP,
            (BSL_META("FSAL API : cbx_cfp_rule_set()..invalid handle \n")));
        return CBX_E_PARAM;
    }
    if (*cfpid != CBXI_CFP_STAGE_INGRESS_HANDLE) {
        LOG_ERROR(BSL_LS_FSAL_CFP,
            (BSL_META("FSAL API : cbx_cfp_rule_set()..invalid handle \n")));
        return CBX_E_PARAM;
    }

    /* validate the handle passed */
    if ((rule == NULL) || (rule->attr_selector == NULL)) {
        LOG_ERROR(BSL_LS_FSAL_CFP,
           (BSL_META("FSAL API : cbx_cfp_rule_set()..rule pointer is NULL\n")));
        return CBX_E_PARAM;
    }
    /* Check if the number of attributes passed is more than the allowed number
       for the key type */
    switch (rule->key_type) {
        case cbxCfpKeyL2EthernetACL:
            no_of_attributes = CBX_CFP_L2_ETHERNET_ATTR_MAX;
            CBX_BMP_ASSIGN(attr_bmp[0], l2_rule_attr_bmp);
            break;
        case cbxCfpKeyIPv4ACL:
            no_of_attributes = CBX_CFP_IPV4_ATTR_MAX;
            CBX_BMP_ASSIGN(attr_bmp[0], v4_rule_attr_bmp);
            break;
        case cbxCfpKeyIPv6ACL:
            no_of_attributes = CBX_CFP_IPV6_ATTR_MAX;
            CBX_BMP_ASSIGN(attr_bmp[0], v6_rule_attr_bmp_0);
            CBX_BMP_ASSIGN(attr_bmp[1], v6_rule_attr_bmp_1);
            break;
        case cbxCfpKeyL2IPv4ACL:
            no_of_attributes = CBX_CFP_L2_IPV4_ATTR_MAX;
            CBX_BMP_ASSIGN(attr_bmp[0], l2_v4_rule_attr_bmp_0);
            CBX_BMP_ASSIGN(attr_bmp[1], l2_v4_rule_attr_bmp_1);
            break;
        case cbxCfpKeyUDFACL:
            no_of_attributes = CBX_CFP_UDF_ATTR_MAX;
            CBX_BMP_ASSIGN(attr_bmp[0], udf_rule_attr_bmp_0);
            CBX_BMP_ASSIGN(attr_bmp[1], udf_rule_attr_bmp_1);
            break;
        default:
            /* Invalid key type */
            LOG_ERROR(BSL_LS_FSAL_CFP,
                (BSL_META("FSAL API : cbx_cfp_rule_set()..invalid  \
                     key type\n")));
            return CBX_E_PARAM;
    }
    if (rule->num_attributes > no_of_attributes) {
        LOG_ERROR(BSL_LS_FSAL_CFP,
            (BSL_META("FSAL API : cbx_cfp_rule_set()..No of rule attributes \
               is more than the max possible attributes for the key type \n")));
        return CBX_E_PARAM;
    }
    /* Check whether wrong attribute is being passed */
    for (index = 0; index < rule->num_attributes; index++) {
        if (rule->attr_selector[index].attr >= cbxCfpRuleAttrMax) {
            LOG_ERROR(BSL_LS_FSAL_CFP,
             (BSL_META("FSAL API : cbx_cfp_rule_set()..invalid attribute \n")));
            return CBX_E_PARAM;
        }
        if ((rule->attr_selector[index].attr_val == NULL) ||
            (rule->attr_selector[index].attr_mask == NULL)) {
            LOG_ERROR(BSL_LS_FSAL_CFP,
             (BSL_META("FSAL API : cbx_cfp_rule_set()..attribute is NULL \n")));
            return CBX_E_PARAM;
        }
        if (rule->attr_selector[index].attr >= cbxCfpRuleAttrDestIp6Low) {
            CBX_BMP_ADD(passed_bmp[1],
                     (rule->attr_selector[index].attr - cbxCfpRuleAttrDestIp6Low));
        } else {
            CBX_BMP_ADD(passed_bmp[0], rule->attr_selector[index].attr);
        }
    }
    CBX_BMP_ASSIGN(cmp_bmp[0], passed_bmp[0]);
    CBX_BMP_ASSIGN(cmp_bmp[1], passed_bmp[1]);

    CBX_BMP_OR(cmp_bmp[0], attr_bmp[0]);
    if (CBX_BMP_NEQ(cmp_bmp[0], attr_bmp[0])) {
        LOG_ERROR(BSL_LS_FSAL_CFP,
             (BSL_META("FSAL API : cbx_cfp_rule_set()..Invalid attribute \n")));
        return CBX_E_PARAM;
    }
    CBX_BMP_OR(cmp_bmp[1], attr_bmp[1]);
    if (CBX_BMP_NEQ(cmp_bmp[1], attr_bmp[1])) {
        LOG_ERROR(BSL_LS_FSAL_CFP,
             (BSL_META("FSAL API : cbx_cfp_rule_set()..Invalid attribute \n")));
        return CBX_E_PARAM;
    }
    /* Check if multiple L4 packet type attributes are passed */
    l4_attr = ((passed_bmp[0] >> cbxCfpRuleAttrL4SrcPort) & 0xF);
    switch(l4_attr) {
        case 0x0:
        case 0x1:
        case 0x2:
        case 0x3:
            *l4_pkt_type_attr = cbxCfpL3L4PktTypeTcp;
            break;
        case 0x4:
            *l4_pkt_type_attr = cbxCfpL3L4PktTypeIcmp;
            break;
        case 0x8:
            *l4_pkt_type_attr = cbxCfpL3L4PktTypeIgmp;
            break;
        default:
            LOG_ERROR(BSL_LS_FSAL_CFP,
                 (BSL_META("FSAL API : cbx_cfp_rule_set()..Overlapping L4  \
                                   attributes passed \n")));
            return CBX_E_PARAM;
            break;
    }
    /* Check if IPv6 addresses are passed in both 128 bit and 2 64 bit formats*/
    v6_addr_attr = ((passed_bmp[0] >> cbxCfpRuleAttrSrcIp6) & 0x7);
    if ((v6_addr_attr & 1) && (v6_addr_attr & 0x6)) {
        LOG_ERROR(BSL_LS_FSAL_CFP,
             (BSL_META("FSAL API : cbx_cfp_rule_set()..V6 address attributes \
                                 passed in both 128bit and 64bit formats \n")));
        return CBX_E_PARAM;
    }
    v6_addr_attr = ((passed_bmp[0] >> cbxCfpRuleAttrDestIp6) & 0x7);
    if ((v6_addr_attr & 1) && (v6_addr_attr & 0x6)) {
        LOG_ERROR(BSL_LS_FSAL_CFP,
             (BSL_META("FSAL API : cbx_cfp_rule_set()..V6 address attributes \
                                passed in both 128bit and 64bit formats \n")));
        return CBX_E_PARAM;
    }
    return CBX_E_NONE;
}


/*
 * Function:
 *    cbxi_cfp_tcam_entry_field_set
 * Purpose:
 *    Set tcam entry fields
 *
 */
int
cbxi_cfp_tcam_entry_field_set(cbxi_cfp_rule_field_info_t *field_info,
                              uint32 *regbuf,
                              uint32 *fldbuf) {
    uint32      mask;
    int         index, wp, bp, len;

    index = field_info->bp;
    if (index >= 192) {
        bp = field_info->bp - 192;
    } else {
        bp = field_info->bp;
    }
    wp = bp / 32;
    bp = bp & (32 - 1);
    index = 0;
    for (len = field_info->len; len > 0; len -= 32) {
        if (bp) {
            if (len < 32) {
                mask = (1 << len) - 1;
            } else {
                mask = -1;
            }
            regbuf[wp] &= ~(mask << bp);
            fldbuf[index] &= mask;
            regbuf[wp++] |= fldbuf[index] << bp;
            regbuf[wp] &= ~(mask >> (32 - bp));
            regbuf[wp] |=
            fldbuf[index] >> (32 - bp) & ((1 << bp) - 1);
        } else {
            if (len < 32) {
                mask = (1 << len) - 1;
                regbuf[wp] &= ~mask;
                regbuf[wp++] |= fldbuf[index] << bp;
            } else {
                regbuf[wp++] = fldbuf[index];
            }
        }
        index++;
    }
    return CBX_E_NONE;
}

/*
 * Function:
 *    cbxi_cfp_tcam_entry_field_get
 * Purpose:
 *    Get individual fields from TCAM entry
 *
 */
int
cbxi_cfp_tcam_entry_field_get(cbxi_cfp_rule_field_info_t  *field_info,
                              uint32 *regbuf,
                              uint32 *fldbuf) {
    int         index, wp, bp, len;

/*    index = field_info->bp / 32; */
    index = field_info->bp;
    if (index >= 192) {
        bp = field_info->bp - 192;
    } else {
        bp = field_info->bp;
    }

    wp = bp / 32;
    bp = bp & (32 - 1);
    index = 0;

    for (len = field_info->len; len > 0; len -= 32) {
        if (bp) {
            fldbuf[index] = regbuf[wp++] >> bp & ((1 << (32 - bp)) - 1);
            fldbuf[index] |= regbuf[wp] << (32 - bp);
        } else {
            fldbuf[index] = regbuf[wp++];
        }
        if (len < 32) {
            fldbuf[index] &= ((1 << len) - 1);
        }
        index++;
    }
    return CBX_E_NONE;
}

/*
 * Function:
 *    cbxi_cfp_write_to_tcam
 * Purpose:
 *    Write to CFP CAM entry at the index provided. Update
 *  TCAM at index and index+1 for even rule. Update
 *  TCAM at index+2 and index+3 for odd rule.
 *
 */
int
cbxi_cfp_write_to_tcam(cfpcam_t *rule_x_even,
                       cfpcam_t *rule_x_odd,
                       cfpcam_t *rule_y_even,
                       cfpcam_t *rule_y_odd,
                       uint32_t src_port_x,
                       uint32_t src_port_y,
                       cbx_cfp_key_type_t key_type,
                       int      no_of_rules,
                       uint32_t rule_index) {

    wrt_t              wrt_entry;
    uint32_t           no_of_units = 1;
    int                unit = CBX_AVENGER_PRIMARY;
    uint32             status = 0;
    int                rv = CBX_E_NONE;
    /* Check cascaded set up or not */
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        no_of_units = 2;
    }
    for (unit = 0; unit < no_of_units; unit++) {
        /* if specified source port belongs to secondary avenger, disable
           primary rule */
        if ((unit == CBX_AVENGER_SECONDARY) && (key_type <= cbxCfpKeyUDFACL)) {
            rule_x_even->tcam_data5 &= 0xE0001FFF;
            rule_y_even->tcam_data5 &= 0xE0001FFF;
            rule_x_even->tcam_data5 |= (src_port_x & 0x1FFFE000);
            rule_y_even->tcam_data5 |= (src_port_y & 0x1FFFE000);
        }
        rv = soc_robo2_cfpcam_set(unit, ((rule_index) * 2),
                                  rule_x_even, &status);
        if (CBX_SUCCESS(rv)) {
            rv = soc_robo2_cfpcam_set(unit, ((rule_index * 2) + 1),
                                      rule_y_even, &status);
        }
        /* Add odd rule */
        if ((no_of_rules == 2) && (CBX_SUCCESS(rv))) {
            rv = soc_robo2_cfpcam_set(unit, ((rule_index + 1) * 2),
                                      rule_x_odd, &status);
            if (CBX_SUCCESS(rv)) {
                rv = soc_robo2_cfpcam_set(unit, (((rule_index + 1) * 2) + 1),
                                      rule_y_odd, &status);
            }
            if (CBX_SUCCESS(rv)) {
                /* Update WRT entry */
                wrt_entry.wide_rule_entry = 1;
                rv = soc_robo2_wrt_set(unit, rule_index/2, &wrt_entry, &status);
            }
        }
    }
    return rv;
}

/*
 * Function:
 *    cbxi_cfp_rule_set
 * Purpose:
 *    Create the rule and write it to HW.
 *
 */
int
cbxi_cfp_rule_set(cbx_cfpid_t *cfpid,
                  cbx_cfp_rule_t *rule,
                  uint32_t rule_index) {

    cbxi_cfp_rule_field_info_t *field_info;
    cfpcam_t                  rule_x_even;
    cfpcam_t                  rule_y_even;
    cfpcam_t                  rule_x_odd;
    cfpcam_t                  rule_y_odd;
    cfpcam_t                  mask_even;
    cfpcam_t                  mask_odd;
    cfpcam_t                  *rule_x_ptr;
    cfpcam_t                  *rule_y_ptr;
    cfpcam_t                  *mask_x_ptr;
    uint32_t                  no_of_rules = 1;
    uint32_t                  index = 0;
    uint32                    fval[4] =  { 0, 0, 0, 0 };
    uint8_t                   val[16] = { 0,0,0,0,
                                          0,0,0,0,
                                          0,0,0,0,
                                          0,0,0,0
                                        };

    cfpcam_t                  rule_x_src_port;
    cfpcam_t                  rule_y_src_port;
    uint32                    dbid_val =  0;
    uint32                    dbid_mask = 0x7;
    uint32                    sp = 0;

    /* use key type to get field info */
    switch (rule->key_type) {
        case cbxCfpKeyL2EthernetACL:
            field_info = &cfp_l2_rule_field_info[0];
            dbid_val = CBXI_CFP_L2_KEY_DBID;
            break;
        case cbxCfpKeyIPv4ACL:
            field_info = &cfp_ipv4_rule_field_info[0];
            dbid_val = CBXI_CFP_IPV4_KEY_DBID;
            break;
        case cbxCfpKeyIPv6ACL:
            field_info = &cfp_ipv6_rule_field_info[0];
            dbid_val = CBXI_CFP_IPV6_KEY_DBID;
            no_of_rules = 2;
            break;
        case cbxCfpKeyL2IPv4ACL:
            field_info = &cfp_l2_ipv4_rule_field_info[0];
            dbid_val = CBXI_CFP_L2_IPV4_KEY_DBID;
            no_of_rules = 2;
            break;
        case cbxCfpKeyUDFACL:
            field_info = &cfp_udf_rule_field_info[0];
            dbid_val = CBXI_CFP_UDF_KEY_DBID;
            no_of_rules = 2;
            break;
        default:
            return CBX_E_PARAM;
    }
    sal_memset(&rule_x_even, 0, sizeof(cfpcam_t));
    sal_memset(&rule_y_even, 0, sizeof(cfpcam_t));
    sal_memset(&rule_x_odd, 0, sizeof(cfpcam_t));
    sal_memset(&rule_y_odd, 0, sizeof(cfpcam_t));
    sal_memset(&rule_x_src_port, 0, sizeof(cfpcam_t));
    sal_memset(&rule_y_src_port, 0, sizeof(cfpcam_t));
    sal_memset(&mask_even, 0, sizeof(cfpcam_t));
    sal_memset(&mask_odd, 0, sizeof(cfpcam_t));
    for (index = 0; index < rule->num_attributes; index++) {
        /* Memset rule, mask and value */
        sal_memset(val, 0, 16);
        sal_memset(fval, 0, 16);
        if (rule->attr_selector[index].attr_len == 0) {
            continue;
        }
        /* Convert attribute values from uint8 to uint32 */
        sal_memcpy(&val[0],
                   rule->attr_selector[index].attr_val,
                   rule->attr_selector[index].attr_len);
        CBX_UINT8_VAL_TO_UINT32(val, fval);
        /* validate the attribute value */
        CBX_IF_ERROR_RETURN(
               cbxi_cfp_validate_rule_attribute_values(
                                         rule->attr_selector[index].attr,
                                         fval[0]));
        /* Check whether even or odd rule needs to be updated */
        if (field_info[rule->attr_selector[index].attr].bp >= 192) {
            rule_x_ptr = &rule_x_odd;
            rule_y_ptr = &rule_y_odd;
        } else {
            rule_x_ptr = &rule_x_even;
            rule_y_ptr = &rule_y_even;
        }
        /* Update rule for the attribute */
        if (rule->attr_selector[index].attr == cbxCfpRuleAttrSourcePort) {
            sp = fval[0] >> 16;
            CBX_IF_ERROR_RETURN(
                 cbxi_cfp_tcam_entry_field_set(&field_info[rule->attr_selector[index].attr],
                                               &(rule_x_src_port.tcam_data0),
                                               &sp));
        }
        CBX_IF_ERROR_RETURN(
             cbxi_cfp_tcam_entry_field_set(&field_info[rule->attr_selector[index].attr],
                                           &(rule_x_ptr->tcam_data0),
                                           &fval[0]));
        /* Convert attribute mask from uint8 to uint32 */
        sal_memset(val, 0, 16);
        sal_memset(fval, 0, 16);
        sal_memcpy(&val[0],
                   rule->attr_selector[index].attr_mask,
                   rule->attr_selector[index].attr_len);
        CBX_UINT8_VAL_TO_UINT32(val, fval);
        /* Check whether even or odd rule needs to be updated */
        if (field_info[rule->attr_selector[index].attr].bp >= 192) {
            mask_x_ptr = &mask_odd;
        } else {
            mask_x_ptr = &mask_even;
        }
        /* Update rule for the attribute */
        if (rule->attr_selector[index].attr == cbxCfpRuleAttrSourcePort) {
            sp = fval[0] >> 16;
            CBX_IF_ERROR_RETURN(
                 cbxi_cfp_tcam_entry_field_set(&field_info[rule->attr_selector[index].attr],
                                           &(rule_y_src_port.tcam_data0),
                                           &sp));
            rule_y_src_port.tcam_data5 =
                           (~rule_x_src_port.tcam_data5 & rule_y_src_port.tcam_data5);
        }
        CBX_IF_ERROR_RETURN(
             cbxi_cfp_tcam_entry_field_set(&field_info[rule->attr_selector[index].attr],
                                           &(mask_x_ptr->tcam_data0),
                                           &fval[0]));
        rule_y_ptr->tcam_data0 =
                           (~rule_x_ptr->tcam_data0 & mask_x_ptr->tcam_data0);
        rule_y_ptr->tcam_data1 =
                           (~rule_x_ptr->tcam_data1 & mask_x_ptr->tcam_data1);
        rule_y_ptr->tcam_data2 =
                           (~rule_x_ptr->tcam_data2 & mask_x_ptr->tcam_data2);
        rule_y_ptr->tcam_data3 =
                           (~rule_x_ptr->tcam_data3 & mask_x_ptr->tcam_data3);
        rule_y_ptr->tcam_data4 =
                           (~rule_x_ptr->tcam_data4 & mask_x_ptr->tcam_data4);
        rule_y_ptr->tcam_data5 =
                           (~rule_x_ptr->tcam_data5 & mask_x_ptr->tcam_data5);
    }
    /* Set TCAM entry valid bit */
    rule_x_even.tcam_valid = 0x3;
    rule_y_even.tcam_valid = 0x0;
    rule_x_odd.tcam_valid = 0x3;
    rule_y_odd.tcam_valid = 0x0;

    /* Set DBID for the rule */
    CBX_IF_ERROR_RETURN(
        cbxi_cfp_tcam_entry_field_set(&dbid_field_info,
                                           &(rule_x_even.tcam_data0),
                                           &dbid_val));
        dbid_mask = ~dbid_val & 0x7;
    CBX_IF_ERROR_RETURN(
        cbxi_cfp_tcam_entry_field_set(&dbid_field_info,
                                           &(rule_y_even.tcam_data0),
                                           &dbid_mask));
    if (no_of_rules == 2) {
        CBX_IF_ERROR_RETURN(
            cbxi_cfp_tcam_entry_field_set(&dbid_field_info,
                                           &(rule_x_odd.tcam_data0),
                                           &dbid_val));
        dbid_mask = ~dbid_val & 0x7;
        CBX_IF_ERROR_RETURN(
            cbxi_cfp_tcam_entry_field_set(&dbid_field_info,
                                           &(rule_y_odd.tcam_data0),
                                           &dbid_mask));
    }

    /* Write TCAM entry to HW */
    CBX_IF_ERROR_RETURN(cbxi_cfp_write_to_tcam(&rule_x_even, &rule_x_odd,
                                               &rule_y_even, &rule_y_odd,
                                               rule_x_src_port.tcam_data5,
                                               rule_y_src_port.tcam_data5,
                                               rule->key_type,
                                               no_of_rules,
                                               rule_index));
    return CBX_E_NONE;
}

/*
 * Function:
 *    cbxi_cfp_index_validate
 * Purpose:
 *    Validate the rule index passed. return key type and l4 packet type
 *
 */
int
cbxi_cfp_index_validate(uint32_t index, cbx_cfp_key_type_t *key_type,
                        cbx_cfp_l3_l4_pkt_type_t *l4_pkt_type) {
   int                       max_index = 0;
   cbx_cfpid_t               cfpid = 0;
   int                       valid = 0;
   valid = tcam_index_info[index] & CBXI_CFP_TCAM_ENTRY_VALID;
   cfpid = ((tcam_index_info[index] & CBXI_CFP_RULE_INDEX_STAGE_MASK) >>
                                    CBXI_CFP_RULE_INDEX_STAGE_SHIFT);
   *key_type = ((tcam_index_info[index] & CBXI_CFP_RULE_INDEX_KEY_TYPE_MASK) >>
                                    CBXI_CFP_RULE_INDEX_KEY_TYPE_SHIFT);
   *l4_pkt_type = (((tcam_index_info[index] &
                       CBXI_CFP_RULE_INDEX_L4_PKT_TYPE_MASK) >>
                       CBXI_CFP_RULE_INDEX_L4_PKT_TYPE_SHIFT));
   if (valid == 0) {
       return CBX_E_PARAM;
   }
    max_index = soc_robo2_wrt_max_index(CBX_AVENGER_PRIMARY);
    sal_printf("CFP WRT MAX idex is 0x%x\n",max_index);
    max_index = soc_robo2_cfpcam_max_index(CBX_AVENGER_PRIMARY);
    sal_printf("CFP cam max index is 0x%x\n",max_index);
    if ((max_index < 0) || (cfpid > max_index)) {
        return CBX_E_PARAM;
    }
    return CBX_E_NONE;
}

/*
 * Function:
 *    cbxi_cfp_rule_clear
 * Purpose:
 *    Delete TCAM entry corresponding to rule index. If rule
 *    being deleted is double wide, clear WRT entry as well
 *
 */
int
cbxi_cfp_rule_clear(cbx_cfp_key_type_t key_type,
                    uint32_t rule_index) {
    int          unit = CBX_AVENGER_PRIMARY;
    wrt_t        wrt_entry;
    uint32       status = 0;
    cfpcam_t     rule_x;
    cfpcam_t     rule_y;

    /* Get Wrt entry */
    CBX_IF_ERROR_RETURN(soc_robo2_wrt_get(unit, rule_index, &wrt_entry));
    if (wrt_entry.wide_rule_entry == 1) {
        wrt_entry.wide_rule_entry = 0;
        CBX_IF_ERROR_RETURN(soc_robo2_wrt_set(unit,
                                              rule_index, &wrt_entry, &status));
        if (SOC_IS_CASCADED(unit)) {
            CBX_IF_ERROR_RETURN(soc_robo2_wrt_set(CBX_AVENGER_SECONDARY,
                                              rule_index, &wrt_entry, &status));
        }
    }
    /* Set TCAM entry to 0  */
    sal_memset(&rule_x, 0, sizeof(cfpcam_t));
    sal_memset(&rule_y, 0, sizeof(cfpcam_t));

    /* Delete even rule */
    CBX_IF_ERROR_RETURN(soc_robo2_cfpcam_set(unit, (rule_index * 2),
                                             &rule_x, &status));
    CBX_IF_ERROR_RETURN(soc_robo2_cfpcam_set(unit, ((rule_index * 2) + 1),
                                             &rule_y, &status));
    if (SOC_IS_CASCADED(unit)) {
        CBX_IF_ERROR_RETURN(soc_robo2_cfpcam_set(CBX_AVENGER_SECONDARY,
                                         (rule_index * 2), &rule_x, &status));
        CBX_IF_ERROR_RETURN(soc_robo2_cfpcam_set(CBX_AVENGER_SECONDARY,
                                         ((rule_index * 2) + 1), &rule_y, &status));
    }
    /* Delete odd rule */
    if ((key_type == cbxCfpKeyL2IPv4ACL) || (key_type == cbxCfpKeyIPv6ACL) ||
        (key_type == cbxCfpKeyUDFACL)) {
        CBX_IF_ERROR_RETURN(soc_robo2_cfpcam_set(unit, ((rule_index + 1) * 2),
                                             &rule_x, &status));
        CBX_IF_ERROR_RETURN(soc_robo2_cfpcam_set(unit, (((rule_index + 1) * 2) + 1),
                                             &rule_y, &status));
        if (SOC_IS_CASCADED(unit)) {
            CBX_IF_ERROR_RETURN(soc_robo2_cfpcam_set(CBX_AVENGER_SECONDARY,
                                     ((rule_index + 1) * 2), &rule_x, &status));
            CBX_IF_ERROR_RETURN(soc_robo2_cfpcam_set(CBX_AVENGER_SECONDARY,
                                     (((rule_index + 1) * 2) + 1), &rule_y, &status));
        }
    }
    return CBX_E_NONE;
}

/*
 * Function:
 *    cbxi_cfp_update_valid_bmp
 * Purpose:
 *    Update valid bmp for a key/pkt type to remove overlapping attributes
 */
int
cbxi_cfp_update_valid_bmp(cbx_cfp_key_type_t key_type,
                          cbx_cfp_l3_l4_pkt_type_t l4_pkt_type, uint32_t *bmp) {
    uint32_t  temp = *bmp;
    if (key_type == cbxCfpKeyIPv6ACL) {
        CBX_BMP_REMOVE(temp, (1 << cbxCfpRuleAttrSrcIp6));
        CBX_BMP_REMOVE(temp, (1 << cbxCfpRuleAttrDestIp6));
    }
    switch(l4_pkt_type) {
        case cbxCfpL3L4PktTypeTcp:
        case cbxCfpL3L4PktTypeUdp:
            CBX_BMP_REMOVE(temp, (1 << cbxCfpRuleAttrIcmpCode));
            CBX_BMP_REMOVE(temp, (1 << cbxCfpRuleAttrIgmpType));
            break;
        case cbxCfpL3L4PktTypeIcmp:
            CBX_BMP_REMOVE(temp, (1 << cbxCfpRuleAttrL4SrcPort));
            CBX_BMP_REMOVE(temp, (1 << cbxCfpRuleAttrL4DestPort));
            CBX_BMP_REMOVE(temp, (1 << cbxCfpRuleAttrIgmpType));
            break;
        case cbxCfpL3L4PktTypeIgmp:
            CBX_BMP_REMOVE(temp, (1 << cbxCfpRuleAttrL4SrcPort));
            CBX_BMP_REMOVE(temp, (1 << cbxCfpRuleAttrL4DestPort));
            CBX_BMP_REMOVE(temp, (1 << cbxCfpRuleAttrIcmpCode));
            break;
        default:
            break;
    }
    *bmp = temp;
    return CBX_E_NONE;
}

/*
 * Function:
 *    cbxi_cfp_rule_dump
 * Purpose:
 *    Read TCAM entry, extract attributes and dump their value and mask
 */
int
cbxi_cfp_rule_dump(cbx_cfp_key_type_t key_type,
                   cbx_cfp_l3_l4_pkt_type_t l4_pkt_type,
                   uint32_t rule_index)  {

    int                       unit = CBX_AVENGER_PRIMARY;
    cfpcam_t                  rule_x_even;
    cfpcam_t                  rule_y_even;
    cfpcam_t                  rule_x_even_cascade;
    cfpcam_t                  rule_y_even_cascade;
    cfpcam_t                  rule_x_odd;
    cfpcam_t                  rule_y_odd;
    cfpcam_t                  mask_even;
    cfpcam_t                  mask_odd;
    cbxi_cfp_rule_field_info_t *field_info;
    uint32_t                  no_of_bmp = 1;
    uint32_t                  bmp = 0;
    uint32_t                  valid_bmp[2];
    cbx_cfp_rule_attribute_t  attr;
    uint32                    fval[4] =  { 0, 0, 0, 0 };
    uint32                    fmask[4] =  { 0, 0, 0, 0 };
    uint8_t                   val[16] = { 0,0,0,0,
                                          0,0,0,0,
                                          0,0,0,0,
                                          0,0,0,0
                                        };
    uint8_t                   mask[16] = { 0,0,0,0,
                                          0,0,0,0,
                                          0,0,0,0,
                                          0,0,0,0
                                        };
    uint32                    sp = 0;

    CBX_BMP_CLEAR(valid_bmp[0]);
    CBX_BMP_CLEAR(valid_bmp[1]);
    rule_x_even_cascade.tcam_data5 = 0;
    rule_y_even_cascade.tcam_data5 = 0;

    /* Retrive cfp tcam info */
    CBX_IF_ERROR_RETURN(soc_robo2_cfpcam_get(
                       unit, (rule_index * 2), &rule_x_even));
    CBX_IF_ERROR_RETURN(soc_robo2_cfpcam_get(
                       unit, ((rule_index * 2) + 1), &rule_y_even));
    mask_even.tcam_data0 = rule_x_even.tcam_data0 | rule_y_even.tcam_data0;
    mask_even.tcam_data1 = rule_x_even.tcam_data1 | rule_y_even.tcam_data1;
    mask_even.tcam_data2 = rule_x_even.tcam_data2 | rule_y_even.tcam_data2;
    mask_even.tcam_data3 = rule_x_even.tcam_data3 | rule_y_even.tcam_data3;
    mask_even.tcam_data4 = rule_x_even.tcam_data4 | rule_y_even.tcam_data4;
    mask_even.tcam_data5 = rule_x_even.tcam_data5 | rule_y_even.tcam_data5;
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        unit = CBX_AVENGER_SECONDARY;
        CBX_IF_ERROR_RETURN(soc_robo2_cfpcam_get(
                      unit, (rule_index * 2), &rule_x_even_cascade));
        CBX_IF_ERROR_RETURN(soc_robo2_cfpcam_get(
                       unit, ((rule_index * 2) + 1), &rule_y_even_cascade));
        rule_y_even_cascade.tcam_data5 = (rule_x_even_cascade.tcam_data5 |
                                          rule_y_even_cascade.tcam_data5);
    }
    unit = CBX_AVENGER_PRIMARY;
    /* check if odd rule is present(double wide) */
    if ((key_type == cbxCfpKeyL2IPv4ACL) || (key_type == cbxCfpKeyIPv6ACL) ||
        (key_type == cbxCfpKeyUDFACL)) {
        CBX_IF_ERROR_RETURN(soc_robo2_cfpcam_get(
                            unit, ((rule_index + 1) * 2), &rule_x_odd));
        CBX_IF_ERROR_RETURN(soc_robo2_cfpcam_get(
                       unit, (((rule_index + 1) * 2) + 1), &rule_y_odd));
        if (rule_x_even.tcam_valid == 0) {
            return CBX_E_EMPTY;
        }
        mask_odd.tcam_data0 = rule_x_odd.tcam_data0 | rule_y_odd.tcam_data0;
        mask_odd.tcam_data1 = rule_x_odd.tcam_data1 | rule_y_odd.tcam_data1;
        mask_odd.tcam_data2 = rule_x_odd.tcam_data2 | rule_y_odd.tcam_data2;
        mask_odd.tcam_data3 = rule_x_odd.tcam_data3 | rule_y_odd.tcam_data3;
        mask_odd.tcam_data4 = rule_x_odd.tcam_data4 | rule_y_odd.tcam_data4;
        mask_odd.tcam_data5 = rule_x_odd.tcam_data5 | rule_y_odd.tcam_data5;
    }
    /* use key type to get field info */
    switch (key_type) {
        case cbxCfpKeyL2EthernetACL:
            field_info = &cfp_l2_rule_field_info[0];
            CBX_BMP_ASSIGN(valid_bmp[0], l2_rule_attr_bmp);
            break;
        case cbxCfpKeyIPv4ACL:
            field_info = &cfp_ipv4_rule_field_info[0];
            CBX_BMP_ASSIGN(valid_bmp[0], v4_rule_attr_bmp);
            break;
        case cbxCfpKeyIPv6ACL:
            field_info = &cfp_ipv6_rule_field_info[0];
            CBX_BMP_ASSIGN(valid_bmp[0], v6_rule_attr_bmp_0);
            CBX_BMP_ASSIGN(valid_bmp[1], v6_rule_attr_bmp_1);
            no_of_bmp = 2;
            break;
        case cbxCfpKeyL2IPv4ACL:
            CBX_BMP_ASSIGN(valid_bmp[0], l2_v4_rule_attr_bmp_0);
            CBX_BMP_ASSIGN(valid_bmp[1], l2_v4_rule_attr_bmp_1);
            field_info = &cfp_l2_ipv4_rule_field_info[0];
            break;
        case cbxCfpKeyUDFACL:
            CBX_BMP_ASSIGN(valid_bmp[0], udf_rule_attr_bmp_0);
            CBX_BMP_ASSIGN(valid_bmp[1], udf_rule_attr_bmp_1);
            no_of_bmp = 2;
            field_info = &cfp_udf_rule_field_info[0];
            break;
        default:
            return CBX_E_PARAM;
    }
    /* Remove overlapping Attributes from bmp */
    CBX_IF_ERROR_RETURN(cbxi_cfp_update_valid_bmp(key_type,
                                                  l4_pkt_type, &valid_bmp[0]));
    sal_printf("====================================================================\n");
    sal_printf("  KEY TYPE = 0x%x, L4 Packet Type = 0x%x INDEX = 0x%x\n",
                  key_type, l4_pkt_type, rule_index);
    sal_printf("------------------------------RULE ATRRIBUTES ---------------------\n");
    /* Fetch rule attributes from TCAM entry and dump them */
    while(bmp < no_of_bmp) {
        /* Iterate through the valid attributes for the key type */
        CBX_BMP_ITER(valid_bmp[bmp], attr)  {
            if (bmp == 1) {
                attr +=32;
            }
            sal_memset(fval, 0, (sizeof(uint32_t) * 4));
            sal_memset(fmask, 0, (sizeof(uint32_t) * 4));
            sal_memset(val, 0, 16);
            sal_memset(mask, 0, 16);
            /* Check odd or even rule */
            if (field_info[attr].bp >= 192) {
                CBX_IF_ERROR_RETURN(
                    cbxi_cfp_tcam_entry_field_get(&field_info[attr],
                                             &rule_x_odd.tcam_data0,
                                             &fval[0]));
                CBX_IF_ERROR_RETURN(
                    cbxi_cfp_tcam_entry_field_get(&field_info[attr],
                                             &mask_odd.tcam_data0,
                                             &fmask[0]));
            } else {
                CBX_IF_ERROR_RETURN(
                    cbxi_cfp_tcam_entry_field_get(&field_info[attr],
                                             &rule_x_even.tcam_data0,
                                             &fval[0]));
                CBX_IF_ERROR_RETURN(
                    cbxi_cfp_tcam_entry_field_get(&field_info[attr],
                                             &mask_even.tcam_data0,
                                             &fmask[0]));
                if (attr == cbxCfpRuleAttrSourcePort) {
                    sp = 0;
                    CBX_IF_ERROR_RETURN(
                        cbxi_cfp_tcam_entry_field_get(&field_info[attr],
                                             &rule_x_even_cascade.tcam_data0,
                                             &sp));
                    fval[0] |= sp << 16;
                    sp = 0;
                    CBX_IF_ERROR_RETURN(
                        cbxi_cfp_tcam_entry_field_get(&field_info[attr],
                                             &rule_y_even_cascade.tcam_data0,
                                             &sp));
                    fmask[0] |= sp << 16;
                }
            }
            /* Convert attribute and mask value from uint32 to uint8 */
            CBX_UINT8_VAL_FROM_UINT32(val, fval);
            CBX_UINT8_VAL_FROM_UINT32(mask,fmask);
            sal_printf("Attribute: cbxCfpRuleAttr%s ",
                                                 cfp_rule_attr_names[attr]);
            if (cfp_rule_attr_info[attr].attr_len == 1) {
                sal_printf("Value 0x%x    mask 0x%x \n", fval[0], fmask[0]);
            } else if (cfp_rule_attr_info[attr].attr_len == 2) {
                sal_printf("Value = 0x%x:0x%x ", val[1], val[0]);
                sal_printf("Mask =  0x%x:0x%x \n", mask[1], mask[0]);
            } else if (cfp_rule_attr_info[attr].attr_len == 4) {
                sal_printf("Value = 0x%x:0x%x:0x%x:0x%x ",
                            val[3], val[2], val[1], val[0]);
                sal_printf("Mask =  0x%x:0x%x:0x%x:0x%x \n",
                          mask[3], mask[2], mask[1], mask[0]);
            } else if (cfp_rule_attr_info[attr].attr_len == 6) {
                sal_printf("Value = 0x%x:0x%x:0x%x:0x%x:0x%x:0x%x ",
                            val[5], val[4], val[3], val[2], val[1], val[0]);
                sal_printf("Mask =  0x%x:0x%x:0x%x:0x%x:0x%x:0x%x\n",
                          mask[5], mask[4], mask[3], mask[2], mask[1], mask[0]);
            } else {
                sal_printf("Value = 0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x ",
                            val[7], val[6], val[5], val[4],
                            val[3], val[2], val[1], val[0]);
                sal_printf("Mask = 0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:0x%x\n",
                            mask[7], mask[6], mask[5], mask[4],
                            mask[3], mask[2], mask[1], mask[0]);
            }
            sal_printf("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");
            if (attr >= 32) {
                attr -= 32;
            }
        }
        bmp++;
    }
    sal_printf("=====================================================================\n");
    sal_printf("returning CBX_E_NONE\n");
    return CBX_E_NONE;
}

/**************************************************************************
 *                 METER FSAL API IMPLEMENTATION                           *
 **************************************************************************/
/**
 * Function :
 *    cbx_cfp_action_t_init
 * Purpose  :
 *     Initialize CFP Action object
 * Parameters:
 *     action     action object
 */
void
cbx_cfp_action_t_init(cbx_cfp_action_t *action) {

    if (action != NULL) {
        sal_memset(action, 0, sizeof(cbx_cfp_action_t));
    }
    return;
}

/**
 * Function :
 *    cbx_cfp_init
 * Purpose  :
 *    This routine is used to initialize a CFP
 * Parameters:
 *     cfp_params   (IN)  CFP parameters.
 *     cfpid        (OUT) Handle of the cfp
 *                        CBX_CFP_INVALID: CFP could not be created
 * Returns:
 *     CBX_E_NONE Success
 *     CBX_E_XXXX Failure
 */

int
cbx_cfp_init(cbx_cfp_params_t *cfp_params,
             cbx_cfpid_t      *cfpid) {

    int       rv = CBX_E_NONE;
    int       index = 0;
    LOG_INFO(BSL_LS_FSAL_CFP,
            (BSL_META("FSAL API : cbx_cfp_init()..\n")));
    /* check for null */
    if ((cfp_params == NULL) || (cfpid == NULL)) {
        return CBX_E_PARAM;
    }
    *cfpid = CBX_CFP_INVALID;
    /* Check stage - if not ingress return error */
    if (cfp_params->stage != cbxCfpStageIngress) {
        LOG_ERROR(BSL_LS_FSAL_CFP,
            (BSL_META("FSAL API : cbx_cfp_init()..Unsupported stage \n")));
        return CBX_E_PARAM;
    }
    /* Take TCAM out of reset and powerdown */
    rv = cbxi_cfp_tcam_config(CBX_AVENGER_PRIMARY);
    if (CBX_SUCCESS(rv)) {
        /* If cascaded set-up, initialise tables in secondary avenger  */
        if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
            rv = cbxi_cfp_tcam_config(CBX_AVENGER_SECONDARY);
        }
    }
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_CFP,
            (BSL_META("FSAL API : cbx_cfp_init()..Table init failed\n")));
        return rv;
    }
    /* Enable all the tables related to CFP module */
    rv = cbxi_cfp_table_init(CBX_AVENGER_PRIMARY);
    if (CBX_SUCCESS(rv)) {
        /* If cascaded set-up, initialise tables in secondary avenger  */
        if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
            rv = cbxi_cfp_table_init(CBX_AVENGER_SECONDARY);
        }
    }
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_CFP,
            (BSL_META("FSAL API : cbx_cfp_init()..Table init failed\n")));
        return rv;
    }
    /* set tcam_index_info to 0 */
    for (index = 0; index < CBXI_CFP_CFP_RULE_MAX; index++) {
        tcam_index_info[index] = 0;
    }
    /* Create keys - set IKFT */
    rv = cbxi_cfp_create_default_lookup_keys(CBX_AVENGER_PRIMARY);
    if (CBX_SUCCESS(rv)) {
        /* If cascaded set-up, set IKFT  */
        if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
            rv = cbxi_cfp_create_default_lookup_keys(CBX_AVENGER_SECONDARY);
        }
    }
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_CFP,
            (BSL_META("FSAL API : cbx_cfp_init()..Failed to add default \
                       cfp keys\n")));
        return rv;
    }
    /* make FPSLICT entries point to KST entries with CFP enabled and default
       keys set */
    rv = cbxi_fpslict_table_update(CBX_AVENGER_PRIMARY);
    if (CBX_SUCCESS(rv)) {
        /* If cascaded set-up, initialise tables in secondary avenger  */
        if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
            rv = cbxi_fpslict_table_update(CBX_AVENGER_SECONDARY);
        }
    }
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_CFP,
            (BSL_META("FSAL API : cbx_cfp_init()..Failed to update \
                       slic with cfp keys\n")));
        return rv;
    }
    /* make FPSLICT entries point to KST entries with CFP enabled and default
       keys set */
    rv = cbxi_cfp_kst_entry_update(CBX_AVENGER_PRIMARY, CBXI_CFP_ENABLE);
    if (CBX_SUCCESS(rv)) {
        /* If cascaded set-up, initialise tables in secondary avenger  */
        if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
            rv = cbxi_cfp_kst_entry_update(CBX_AVENGER_SECONDARY,
                                           CBXI_CFP_ENABLE);
        }
    }
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_CFP,
            (BSL_META("FSAL API : cbx_cfp_init()..Failed to update \
                       KST entry with cfp keys\n")));
        return rv;
    }
    /* Set valid attribute bit map for all the key types */
    cbxi_cfp_rule_attr_bmp_set();

    field_ctrl.cfp_state = CBXI_CFP_INIT_COMPLETE;
    field_ctrl.cfp_handle = CBXI_CFP_STAGE_INGRESS_HANDLE;
    /* return cfp handle */
    *cfpid = CBXI_CFP_STAGE_INGRESS_HANDLE;
    return rv;
}

/**
 * Function :
 *    cbx_cfp_detach
 * Purpose  :
 *    Detach a cfp previously initialized by cbx_cfp_init()
 * Parameters:
 *   cfpid    (IN)  Handle of the cfp to be destroyed
 * Returns:
 *     CBX_E_NONE Success
 *     CBX_E_XXXX Failure
 *
 */
int
cbx_cfp_detach(cbx_cfpid_t cfpid) {

    int       rv = CBX_E_NONE;
    /* return error if CFP is not initialized */
    CFP_INIT;

    /* validate the handle passed */
    if (cfpid != CBXI_CFP_STAGE_INGRESS_HANDLE) {
        LOG_ERROR(BSL_LS_FSAL_CFP,
            (BSL_META("FSAL API : cbx_cfp_detach()..invalid handle \n")));
        return CBX_E_PARAM;
    }

    /* disable cfp via slic */
    rv = cbxi_cfp_kst_entry_update(CBX_AVENGER_PRIMARY, CBXI_CFP_DISABLE);
    if (CBX_SUCCESS(rv)) {
        /* If cascaded set-up, initialise tables in secondary avenger  */
        if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
            rv = cbxi_cfp_kst_entry_update(CBX_AVENGER_SECONDARY,
                                           CBXI_CFP_DISABLE);
        }
    }
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_CFP,
            (BSL_META("FSAL API : cbx_cfp_detach()..Failed to disable \
                       cfp in fp slict\n")));
        return rv;
    }
    field_ctrl.cfp_state = CBXI_CFP_INIT_NOT_DONE;
    return CBX_E_NONE;
}

/**
 * Function :
 *    cbx_cfp_rule_t_init
 * Purpose  :
 *    Initialize CFP Rule object
 *    Prefill attribute array in the Rule with initialized values and masks
 * Parameters:
 *    pkt_type   L3/L4 packet type
 *    rule       rule object
 * Returns:
 *    None
 */
void
cbx_cfp_rule_t_init(cbx_cfp_l3_l4_pkt_type_t pkt_type,
                    cbx_cfp_rule_t *rule) {

    uint32_t no_of_attributes = 0;

    /* validate the handle passed */
    if ((rule == NULL) || (rule->attr_selector == NULL)) {
        LOG_ERROR(BSL_LS_FSAL_CFP,
            (BSL_META("FSAL API : cbx_cfp_rule_init()..invalid handle \n")));
        return;
    }
    /* Validate key type */
    if(rule->key_type > cbxCfpKeyUDFACL) {
        LOG_ERROR(BSL_LS_FSAL_CFP,
            (BSL_META("FSAL API : cbx_cfp_rule_init()..invalid key type \n")));
        return;
    }
    /* validate l3/l4 packet type */
    if((rule->key_type > cbxCfpKeyL2EthernetACL) &&
       (pkt_type > cbxCfpL3L4PktTypeIgmp)) {
        LOG_ERROR(BSL_LS_FSAL_CFP,
          (BSL_META("FSAL API : cbx_cfp_rule_init()..invalid packet type \n")));
        return;
    }
    /* Check whether we have enough space for max possible attributes for the
       given key type */
    switch (rule->key_type) {
        case cbxCfpKeyL2EthernetACL:
            no_of_attributes = CBX_CFP_L2_ETHERNET_ATTR_MAX;
            break;
        case cbxCfpKeyIPv4ACL:
            no_of_attributes = CBX_CFP_IPV4_ATTR_MAX;
            break;
        case cbxCfpKeyIPv6ACL:
            no_of_attributes = CBX_CFP_IPV6_ATTR_MAX;
            break;
        case cbxCfpKeyL2IPv4ACL:
            no_of_attributes = CBX_CFP_L2_IPV4_ATTR_MAX;
            break;
        case cbxCfpKeyUDFACL:
            no_of_attributes = CBX_CFP_UDF_ATTR_MAX;
            break;
        default:
            break;
    }
    if (rule->num_attributes < no_of_attributes) {
        LOG_ERROR(BSL_LS_FSAL_CFP,
            (BSL_META("FSAL API : cbx_cfp_rule_init()..Not enough memory \
                       allocated for holding all attributes of key type \n")));
        return;
    }
    /* Fill in the attributes, their length and
       mssk(if enough memory is already allocated) */
    cbxi_cfp_prefill_attribute(rule, pkt_type);
    return;
}


/**
 * Function :
 *    cbx_cfp_rule_set
 * Purpose  :
 *    This routine is used to set a CFP rule
 * Parameters:
 *    cfpid    (IN)  CFP Identifier
 *    rule     (IN)  Rule object
 *    index    (IN)  Rule Index
 * Returns:
 *     CBX_E_NONE Success
 *     CBX_E_XXXX Failure
 */

int
cbx_cfp_rule_set(cbx_cfpid_t     *cfpid,
                 cbx_cfp_rule_t  *rule,
                 uint32_t        index) {

    int         rv = CBX_E_NONE;
    uint32_t    l4_pkt_type_attr;

    /* return error if CFP is not initialized */
    CFP_INIT;
    /* Validate all the rule parameters */
    rv = cbxi_cfp_rule_param_validate(cfpid, rule, &l4_pkt_type_attr);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_CFP,
            (BSL_META("FSAL API : cbx_cfp_rule_set()..invalid parameter \n")));
        return CBX_E_PARAM;
    }
    /* Validate the index passed */
    if (index >= CBXI_CFP_CFP_RULE_MAX) {
        LOG_ERROR(BSL_LS_FSAL_CFP,
            (BSL_META("FSAL API : cbx_cfp_rule_set()..Passed  \
                    index is more than max allowed \n")));
        return CBX_E_PARAM;
    }
    if (tcam_index_info[index] & CBXI_CFP_TCAM_ENTRY_VALID) {
        LOG_ERROR(BSL_LS_FSAL_CFP,
            (BSL_META("FSAL API : cbx_cfp_rule_set()..Passed  \
                    index is already being used \n")));
        return CBX_E_EXISTS;
    }
    /* No support for cbxCfpKeyL2IPv4ACL in A0 variants...due to UDF support
       needed for filtering DHCP and IGMP due to native type mismatch workaround */
    if ((rule->key_type == cbxCfpKeyL2IPv4ACL) &&
       (SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_A0)) {
        LOG_ERROR(BSL_LS_FSAL_CFP,
            (BSL_META("FSAL API : cbx_cfp_rule_set()..unsupported key type \n")));
        return CBX_E_PARAM;
    }    
    if ((rule->key_type == cbxCfpKeyL2IPv4ACL) ||
        (rule->key_type == cbxCfpKeyIPv6ACL) ||
        (rule->key_type == cbxCfpKeyUDFACL)) {
        if (tcam_index_info[index + 1] & CBXI_CFP_TCAM_ENTRY_VALID) {
            LOG_ERROR(BSL_LS_FSAL_CFP,
                (BSL_META("FSAL API : cbx_cfp_rule_set()..Passed  \
                        index is already being used \n")));
            return CBX_E_EXISTS;
        }
    }
    /* Create the rule and write it to HW. Return rule index */
    rv = cbxi_cfp_rule_set(cfpid, rule, index);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_CFP,
            (BSL_META("FSAL API : cbx_cfp_rule_set()..Failed to  \
                    create the rule for key type and attributes passed \n")));
        return CBX_E_INTERNAL;
    }
    /* Store info related to rule  - stage, key type, l4_pkt_type  */
    tcam_index_info[index] = ((*cfpid << CBXI_CFP_RULE_INDEX_STAGE_SHIFT) |
               (rule->key_type << CBXI_CFP_RULE_INDEX_KEY_TYPE_SHIFT) |
               (l4_pkt_type_attr << CBXI_CFP_RULE_INDEX_L4_PKT_TYPE_SHIFT) |
               (CBXI_CFP_TCAM_ENTRY_VALID));
    if ((rule->key_type == cbxCfpKeyL2IPv4ACL) ||
        (rule->key_type == cbxCfpKeyIPv6ACL) ||
        (rule->key_type == cbxCfpKeyUDFACL)) {
        tcam_index_info[index + 1] = tcam_index_info[index];
        tcam_index_info[index] |= CBXI_CFP_TCAM_ENTRY_DOUBLE_WIDE;
    }
    return CBX_E_NONE;
}

/**
 * Function :
 *    cbx_cfp_rule_clear
 * Purpose  :
 *    This routine is used to clear a CFP rule
 * Parameters:
 *    cfpid    (IN)  CFP Identifier
 *    index    (IN)  Rule Index
 * Returns:
 *     CBX_E_NONE Success
 *     CBX_E_XXXX Failure
 */

int
cbx_cfp_rule_clear(cbx_cfpid_t    *cfpid,
                   uint32_t        index) {

    int                      rv = CBX_E_NONE;
    cbx_cfp_key_type_t       key_type;
    cbx_cfp_l3_l4_pkt_type_t l4_pkt_type;
    /* return error if CFP is not initialized */
    CFP_INIT;

    /* validate the handle passed */
    if ((cfpid == NULL)) {
        LOG_ERROR(BSL_LS_FSAL_CFP,
            (BSL_META("FSAL API : cbx_cfp_rule_clear()..invalid handle \n")));
        return CBX_E_PARAM;
    }
    if (*cfpid != CBXI_CFP_STAGE_INGRESS_HANDLE) {
        LOG_ERROR(BSL_LS_FSAL_CFP,
            (BSL_META("FSAL API : cbx_cfp_rule_clear()..invalid handle \n")));
        return CBX_E_PARAM;
    }
    /* Validate the rule index passed */
    rv = cbxi_cfp_index_validate(index, &key_type, &l4_pkt_type);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_CFP,
            (BSL_META("FSAL API : cbx_cfp_rule_clear()..invalid index \n")));
        return CBX_E_PARAM;
    }
    /* Delete the rule */
    rv = cbxi_cfp_rule_clear(key_type, index);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_CFP,
            (BSL_META("FSAL API : cbx_cfp_rule_clear()..No entry with \
                                  this index \n")));
        return CBX_E_INTERNAL;
    }
    /* Reset tcam index info */
    tcam_index_info[index] = 0;
    if ((key_type == cbxCfpKeyL2IPv4ACL) ||
        (key_type == cbxCfpKeyIPv6ACL) ||
        (key_type == cbxCfpKeyUDFACL)) {
        tcam_index_info[index + 1] = 0;
    }
    return CBX_E_NONE;
}

/**
 * Function :
 *    cbx_cfp_rule_dump
 * Purpose  :
 *    This routine is used to dump a CFP rule
 * Parameters:
 *    cfpid    (IN)  CFP Identifier
 *    index    (IN)  Rule Index
 * Returns:
 *    CBX_E_NONE Success
 *    CBX_E_XXXX Failure
 */

void
cbx_cfp_rule_dump(cbx_cfpid_t  *cfpid,
                  uint32_t      index) {
    int                       rv = CBX_E_NONE;
    cbx_cfp_key_type_t        key_type;
    cbx_cfp_l3_l4_pkt_type_t  l4_pkt_type;

    /* return error if CFP is not initialized */
    if (field_ctrl.cfp_state != CBXI_CFP_INIT_COMPLETE) {
        return;
    }
    /* validate the handle passed */
    if ((cfpid == NULL)) {
        LOG_ERROR(BSL_LS_FSAL_CFP,
            (BSL_META("FSAL API : cbx_cfp_rule_dump()..invalid handle \n")));
        return;
    }
    if (*cfpid != CBXI_CFP_STAGE_INGRESS_HANDLE) {
        LOG_ERROR(BSL_LS_FSAL_CFP,
            (BSL_META("FSAL API : cbx_cfp_rule_dump()..invalid handle \n")));
        return;
    }
    /* Validate the rule index passed */
    rv = cbxi_cfp_index_validate(index, &key_type, &l4_pkt_type);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_CFP,
            (BSL_META("FSAL API : cbx_cfp_rule_dump()..invalid index \n")));
        return;
    }
    /* Get the rule and dump the parameters */
    rv = cbxi_cfp_rule_dump(key_type, l4_pkt_type, index);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_CFP,
            (BSL_META("FSAL API : cbx_cfp_rule_dump()..invalid parameter \n")));
        return;
    }
    return;
}

/**
 * Function :
 *    cbx_cfp_action_set
 * Purpose  :
 *    This routine is used to set a CFP action
 * Parameters:
 *    cfpid    (IN)  CFP Identifier
 *    action   (IN)  Action object
 *    index    (IN)  Action Index
 * Returns:
 *     CBX_E_NONE Success
 *     CBX_E_XXXX Failure
 */
int
cbx_cfp_action_set(cbx_cfpid_t      *cfpid,
                   cbx_cfp_action_t *action,
                   uint32_t         index) {
    int       rv = CBX_E_NONE;

    /* return error if CFP is not initialized */
    CFP_INIT;

    /* validate the handle passed */
    if ((cfpid == NULL)) {
        LOG_ERROR(BSL_LS_FSAL_CFP,
            (BSL_META("FSAL API : cbx_cfp_action_set()..invalid handle \n")));
        return CBX_E_PARAM;
    }
    if (*cfpid != CBXI_CFP_STAGE_INGRESS_HANDLE) {
        LOG_ERROR(BSL_LS_FSAL_CFP,
            (BSL_META("FSAL API : cbx_cfp_action_set()..invalid handle \n")));
        return CBX_E_PARAM;
    }
    /* Validate parameters */
    rv = cbxi_cfp_action_param_validate(action, index);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_CFP,
           (BSL_META("FSAL API : cbx_cfp_action_set()..invalid parameter \n")));
        return CBX_E_PARAM;
    }
    /* Add action entry to HW */
    rv = cbxi_cfp_action_op(CBX_AVENGER_PRIMARY, index,
                            action, CBXI_CFP_ACTION_ADD);
    if (CBX_SUCCESS(rv)) {
        /* If cascaded set-up, update secondary avenger  */
        if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
           rv = cbxi_cfp_action_op(CBX_AVENGER_SECONDARY, index,
                            action, CBXI_CFP_ACTION_ADD);
        }
    }
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_CFP,
           (BSL_META("FSAL API : cbx_cfp_action_set()..unable to write \
                             action entry to HW \n")));
        return rv;
    }
    return CBX_E_NONE;
}

/**
 * Function :
 *    cbx_cfp_action_clear
 * Purpose  :
 *    This routine is used to clear a CFP action
 * Parameters:
 *    cfpid    (IN)  CFP Identifier
 *    index    (IN)  Action Index
 * Returns:
 *     CBX_E_NONE Success
 *     CBX_E_XXXX Failure
 */
int
cbx_cfp_action_clear(cbx_cfpid_t  *cfpid,
                     uint32_t      index) {

    int       rv = CBX_E_NONE;
    int       max_index = 0;
    /* return error if CFP is not initialized */
    CFP_INIT;

    /* validate the handle passed */
    if ((cfpid == NULL)) {
        LOG_ERROR(BSL_LS_FSAL_CFP,
            (BSL_META("FSAL API : cbx_cfp_action_clear()..invalid handle \n")));
        return CBX_E_PARAM;
    }
    if (*cfpid != CBXI_CFP_STAGE_INGRESS_HANDLE) {
        LOG_ERROR(BSL_LS_FSAL_CFP,
            (BSL_META("FSAL API : cbx_cfp_action_clear()..invalid handle \n")));
        return CBX_E_PARAM;
    }
    /* validate the index passed */
    max_index = soc_robo2_action_max_index(CBX_AVENGER_PRIMARY);
    if ((max_index < 0) || ((index) > max_index)) {
        LOG_ERROR(BSL_LS_FSAL_CFP,
            (BSL_META("FSAL API : cbx_cfp_action_clear()..invalid index \n")));
        return CBX_E_PARAM;
    }
    /* Clear the entry from HW */
    rv = cbxi_cfp_action_op(CBX_AVENGER_PRIMARY, index, NULL,
                            CBXI_CFP_ACTION_DELETE);
    if (CBX_SUCCESS(rv)) {
        /* If cascaded set-up, update secondary avenger  */
        if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
           rv = cbxi_cfp_action_op(CBX_AVENGER_SECONDARY, index,
                                   NULL, CBXI_CFP_ACTION_DELETE);
        }
    }
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_CFP,
            (BSL_META("FSAL API : cbx_cfp_action_clear()..invalid index \n")));
        return rv;
    }
    return CBX_E_NONE;
}

/**
 * Function :
 *    cbx_cfp_action_dump
 * Purpose  :
 *    This routine is used to dump a CFP action
 * Parameters:
 *    cfpid    (IN)  CFP Identifier
 *    index    (IN)  Action Index
 * Returns:
 *    CBX_E_NONE Success
 *    CBX_E_XXXX Failure
 */
void
cbx_cfp_action_dump(cbx_cfpid_t  *cfpid,
                    uint32_t      index) {

    int       max_index = 0;
    int       rv = CBX_E_NONE;
    cbx_cfp_action_t action;
    /* return error if CFP is not initialized */
    if (field_ctrl.cfp_state != CBXI_CFP_INIT_COMPLETE) {
        return;
    }

    /* validate the handle passed */
    if ((cfpid == NULL)) {
        LOG_ERROR(BSL_LS_FSAL_CFP,
            (BSL_META("FSAL API : cbx_cfp_action_dump()..invalid handle \n")));
        return;
    }
    if (*cfpid != CBXI_CFP_STAGE_INGRESS_HANDLE) {
        LOG_ERROR(BSL_LS_FSAL_CFP,
            (BSL_META("FSAL API : cbx_cfp_action_dump()..invalid handle \n")));
        return;
    }
    /* validate the index passed */
    max_index = soc_robo2_action_max_index(CBX_AVENGER_PRIMARY);
    if ((max_index < 0) || ((index) > max_index)) {
        LOG_ERROR(BSL_LS_FSAL_CFP,
            (BSL_META("FSAL API : cbx_cfp_action_dump()..invalid index \n")));
        return;
    }
    /* Dump action entry */
    rv = cbxi_cfp_action_op(CBX_AVENGER_PRIMARY, index, &action,
                            CBXI_CFP_ACTION_DUMP);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_CFP,
            (BSL_META("FSAL API : cbx_cfp_action_dump()..unable to read \
                         action form HW and dump \n")));
        return;
    }
    return;
}

