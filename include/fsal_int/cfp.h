/* *
 * * $ID: $
 * * 
 * * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 * * 
 * * $Copyright: (c) 2020 Broadcom Inc.
 * *
 * * File:
 * *     cfp.h
 * * Purpose:
 * *     This file contains definitions internal to cfp(ACL) module.
 */

#ifndef _CBX_INT_CFP_H
#define _CBX_INT_CFP_H

#include <bsl_log/bsl.h>
#include <sal_types.h>
#include <shared/types.h>
#include <soc/types.h>
#include <soc/robo2/common/tables.h>
#include <soc/robo2/common/memregs.h>
#include <soc/robo2/common/intenum.h>
#include <fsal/error.h>
#include <fsal/cfp.h>
#include <fsal/cosq.h>
#include <fsal/mirror.h>
#include <fsal/mcast.h>
#include <fsal_int/types.h>
#include <fsal_int/port.h>
#include <fsal_int/types.h>
#include <fsal_int/lag.h>
#include <fsal_int/meter.h>

#define CBXI_CFP_SUPPORTED_ACTION_FLAGS ( CBX_CFP_ACTION_OVERRIDE | CBX_CFP_ACTION_UPDATE_DP | \
                                          CBX_CFP_ACTION_DLF | CBX_CFP_ACTION_LOOP_ENABLE | \
                                          CBX_CFP_ACTION_IN_POLICY_UPDATE_PRI | \
                                          CBX_CFP_ACTION_OUT_POLICY_UPDATE_PRI | \
                                          CBX_CFP_ACTION_IN_POLICY_LEARN_DISABLE | \
                                          CBX_CFP_ACTION_OUT_POLICY_LEARN_DISABLE)

#define CBXI_CFP_MAX_INT_PRI           0x7
#define CBXI_CFP_MAX_INT_DP            0x2
#define CBXI_CFP_MAX_TRAP_ID           0xF

#define CBXI_CFP_ENTRY_ID_INVALID      0xFFFFFFFF
#define CBXI_CFP_INIT_COMPLETE         1
#define CBXI_CFP_INIT_NOT_DONE         0

#define CBXI_CFP_DISABLE               0
#define CBXI_CFP_ENABLE                1

#define CBXI_CFP_ACTION_DELETE         0
#define CBXI_CFP_ACTION_ADD            1
#define CBXI_CFP_ACTION_DUMP           2

#define CBXI_CFP_CFP_RULE_MAX                 1024
#define CBXI_CFP_TCAM_ENTRY_VALID             0x1
#define CBXI_CFP_TCAM_ENTRY_DOUBLE_WIDE       0x10
#define CBXI_CFP_STAGE_INGRESS_HANDLE         0x10
#define CBXI_CFP_RULE_INDEX_STAGE_SHIFT       24
#define CBXI_CFP_RULE_INDEX_STAGE_MASK        0xFF000000
#define CBXI_CFP_RULE_INDEX_KEY_TYPE_SHIFT    16
#define CBXI_CFP_RULE_INDEX_KEY_TYPE_MASK     0xF0000
#define CBXI_CFP_RULE_INDEX_L4_PKT_TYPE_SHIFT 8
#define CBXI_CFP_RULE_INDEX_L4_PKT_TYPE_MASK  0xF00

#define CBXI_SLIC_LAG_CFP_KEYSET_OTHER        0
#define CBXI_SLIC_LAG_CFP_KEYSET_IPV4         1
#define CBXI_SLIC_LAG_CFP_KEYSET_IPV6         2

/** CFP Key index */
#define CBXI_IKFT_L2_KEY_ID                   0
#define CBXI_IKFT_IPV4_KEY_ID                 1
#define CBXI_IKFT_IPV6_KEY_ID_EVEN            2
#define CBXI_IKFT_IPV6_KEY_ID_ODD             3
#define CBXI_IKFT_L2_IPV4_KEY_ID_EVEN         4
#define CBXI_IKFT_L2_IPV4_KEY_ID_ODD          5
#define CBXI_IKFT_UDF_KEY_ID_EVEN             6
#define CBXI_IKFT_UDF_KEY_ID_ODD              7
#define CBXI_IKFT_LBH_OTHER_KEY_ID            8
#define CBXI_IKFT_LBH_IPV4_KEY_ID             9
#define CBXI_IKFT_LBH_IPV6_KEY_ID             10

#ifdef CONFIG_VIRTUAL_PORT_SUPPORT

#define CBXI_IKFT_LIM_PV_KEY_ID               11
#define CBXI_IKFT_LIM_PVV_KEY_ID              12
#define CBXI_IKFT_LIM_MIM_KEY_ID              13
#define CBXI_IKFT_LIM_MPLS_KEY_ID             14

#endif /* CONFIG_VIRTUAL_PORT_SUPPORT */

/* DBID for keys */
#define CBXI_CFP_L2_KEY_DBID                 0
#define CBXI_CFP_IPV4_KEY_DBID               1
#define CBXI_CFP_UDF_KEY_DBID                2
#define CBXI_CFP_IPV6_KEY_DBID               3
#define CBXI_CFP_L2_IPV4_KEY_DBID            4

#define  CBXI_IKFT_CFP_KEY_TYPE_L2  ((CBXI_IKFT_FIELD_FRAME_SUMMARY) | \
                                    (CBXI_IKFT_FIELD_RANGE_RESULT) | \
                                    (CBXI_IKFT_FIELD_OUTER_TCI) | \
                                    (CBXI_IKFT_FIELD_CTAG_VID) |  \
                                    (CBXI_IKFT_FIELD_DMAC) | \
                                    (CBXI_IKFT_FIELD_SMAC) | \
                                    (CBXI_IKFT_FIELD_ETH_TYPE_SAP))

#define  CBXI_IKFT_CFP_KEY_TYPE_IPV4 ((CBXI_IKFT_FIELD_FRAME_SUMMARY) | \
                                    (CBXI_IKFT_FIELD_RANGE_RESULT) | \
                                    (CBXI_IKFT_FIELD_L3_STATUS) | \
                                    (CBXI_IKFT_FIELD_OUTER_TCI) | \
                                    (CBXI_IKFT_FIELD_L3_SA) | \
                                    (CBXI_IKFT_FIELD_L3_DA) | \
                                    (CBXI_IKFT_FIELD_TOS) | \
                                    (CBXI_IKFT_FIELD_L4_BYTES) | \
                                    (CBXI_IKFT_FIELD_TCP_FLAGS))

#define  CBXI_IKFT_CFP_KEY_TYPE_IPV6_EVEN ((CBXI_IKFT_FIELD_FRAME_SUMMARY) | \
                                    (CBXI_IKFT_FIELD_RANGE_RESULT) | \
                                    (CBXI_IKFT_FIELD_L3_STATUS) | \
                                    (CBXI_IKFT_FIELD_OUTER_TCI) | \
                                    (CBXI_IKFT_FIELD_V4V6_L3_SA) | \
                                    (CBXI_IKFT_FIELD_V6_L3_SA_LOW))

#define  CBXI_IKFT_CFP_KEY_TYPE_IPV6_ODD ((CBXI_IKFT_FIELD_V4V6_L3_DA) | \
                                    (CBXI_IKFT_FIELD_V6_L3_DA_LOW) | \
                                    (CBXI_IKFT_FIELD_TOS) | \
                                    (CBXI_IKFT_FIELD_L4_BYTES) | \
                                    (CBXI_IKFT_FIELD_TCP_FLAGS))

#define  CBXI_IKFT_CFP_KEY_TYPE_L2_IPV4_EVEN ((CBXI_IKFT_FIELD_FRAME_SUMMARY) | \
                                    (CBXI_IKFT_FIELD_RANGE_RESULT) | \
                                    (CBXI_IKFT_FIELD_L3_STATUS) | \
                                    (CBXI_IKFT_FIELD_OUTER_TCI) | \
                                    (CBXI_IKFT_FIELD_DMAC) | \
                                    (CBXI_IKFT_FIELD_SMAC))

#define  CBXI_IKFT_CFP_KEY_TYPE_L2_IPV4_ODD ((CBXI_IKFT_FIELD_V4V6_L3_SA) | \
                                    (CBXI_IKFT_FIELD_V4V6_L3_DA) | \
                                    (CBXI_IKFT_FIELD_TOS) | \
                                    (CBXI_IKFT_FIELD_L4_BYTES) | \
                                    (CBXI_IKFT_FIELD_TCP_FLAGS))

#define  CBXI_IKFT_CFP_KEY_TYPE_UDF_EVEN ((CBXI_IKFT_FIELD_FRAME_SUMMARY) | \
                                    (CBXI_IKFT_FIELD_UDF0) | \
                                    (CBXI_IKFT_FIELD_UDF1))

#define  CBXI_IKFT_CFP_KEY_TYPE_UDF_ODD ((CBXI_IKFT_FIELD_UDF2) | \
                                    (CBXI_IKFT_FIELD_UDF3) | \
                                    (CBXI_IKFT_FIELD_UDF4) | \
                                    (CBXI_IKFT_FIELD_UDF5) | \
                                    (CBXI_IKFT_FIELD_UDF6) | \
                                    (CBXI_IKFT_FIELD_UDF7))

#ifdef CONFIG_VIRTUAL_PORT_SUPPORT
/* LIM Keys */
#define  CBXI_IKFT_LIM_KEY_TYPE_PV ((CBXI_IKFT_FIELD_SPG) | \
                                   (CBXI_IKFT_FIELD_CTAG_VID))
#define  CBXI_IKFT_LIM_KEY_TYPE_PVV ((CBXI_IKFT_FIELD_SPG) | \
                                    (CBXI_IKFT_FIELD_OUTER_TCI) | \
                                    (CBXI_IKFT_FIELD_CTAG_VID))
#define  CBXI_IKFT_LIM_KEY_TYPE_MIM ((CBXI_IKFT_FIELD_SPG) | \
                                    (CBXI_IKFT_FIELD_ITAG_ISID) | \
                                    (CBXI_IKFT_FIELD_CTAG_VID))
#define  CBXI_IKFT_LIM_KEY_TYPE_MPLS ((CBXI_IKFT_FIELD_SPG) | \
                                    (CBXI_IKFT_FIELD_MPLS))
#endif /* CONFIG_VIRTUAL_PORT_SUPPORT */


/** IKFT fields  */
#define  CBXI_IKFT_FIELD_FRAME_SUMMARY  0x1    /* SP and L2 status */
#define  CBXI_IKFT_FIELD_RANGE_RESULT   0x2    /* range check result */
#define  CBXI_IKFT_FIELD_SPG            0x4    /* SPG  */
#define  CBXI_IKFT_FIELD_L3_STATUS      0x8    /* L3 status  */
#define  CBXI_IKFT_FIELD_DMAC           0x10   /* Destination MAC   */
#define  CBXI_IKFT_FIELD_SMAC           0x20   /* SMAC  */
#define  CBXI_IKFT_FIELD_OUTER_TCI      0x40   /* outermost VID    */
#define  CBXI_IKFT_FIELD_CTAG_VID       0x80   /* CTAG VID  */
#define  CBXI_IKFT_FIELD_ETH_TYPE_SAP   0x100  /* Ethertype or DSAP/SSAP  */
#define  CBXI_IKFT_FIELD_V4V6_L3_SA     0x200  /* L3 Source IP Address V6 L3 SRC IP HI  */
#define  CBXI_IKFT_FIELD_V4V6_L3_DA     0x400  /* L3 Destination IP address L3 Dest IP HI */
#define  CBXI_IKFT_FIELD_V6_L3_SA_LOW   0x800  /* V6 L3 Source IP Address Low  */
#define  CBXI_IKFT_FIELD_V6_L3_DA_LOW   0x1000 /* V6 L3 Destination IP address Low */
#define  CBXI_IKFT_FIELD_TOS            0x2000 /* V4 L3 TTL, V6 DSCP  */
#define  CBXI_IKFT_FIELD_TTL            0x4000 /* V4 TTL, V6 HOPCNT  */
#define  CBXI_IKFT_FIELD_PROTO          0x8000 /* V4 Prot, V6 NEXT_HDR  */
#define  CBXI_IKFT_FIELD_L4_BYTES       0x10000 /* TCP/UDP port, ICMP code, IGMP type */
#define  CBXI_IKFT_FIELD_TCP_FLAGS      0x20000 /* TCP header flags  */
#define  CBXI_IKFT_FIELD_UDF0           0x40000 /* ETYPE  - used for UDF - after SMAC */
#define  CBXI_IKFT_FIELD_UDF1           0x80000 /* PAYLOAD - MAC+64 - used for UDF  */
#define  CBXI_IKFT_FIELD_UDF2           0x100000 /* L4 PROTO  - MAC+128 used for UDF */
#define  CBXI_IKFT_FIELD_UDF3           0x200000 /* L4 PORT - MAC+240 used for UDF  */
#define  CBXI_IKFT_FIELD_UDF4           0x400000 /* L4 PORT - MAC+240 used for UDF  */
#define  CBXI_IKFT_FIELD_UDF5           0x800000 /* L4 PORT - MAC+240 used for UDF  */
#define  CBXI_IKFT_FIELD_UDF6           0x1000000 /* L4 PORT - MAC+240 used for UDF  */
#define  CBXI_IKFT_FIELD_UDF7           0x2000000 /* L4 PORT - MAC+240 used for UDF  */
#define  CBXI_IKFT_FIELD_L3_SA          0x4000000  /* L3 Source IP Address  */
#define  CBXI_IKFT_FIELD_L3_DA          0x8000000 /* L3 Destination IP address */
#define  CBXI_IKFT_FIELD_ITAG_ISID      0x10000000 /* ITAG SID */
#define  CBXI_IKFT_FIELD_MPLS           0x20000000 /* Outer MPLS lebel */

/* Convert uint8 array to uint32 [] array
 * (dst)[0] is lsb ; (dst)[1] */
#define CBX_UINT8_VAL_TO_UINT32(src, dst) do {\
        (dst)[0] = (((uint32)(src)[3]) << 24 | \
                    ((uint32)(src)[2]) << 16 | \
                    ((uint32)(src)[1]) << 8 | \
                    ((uint32)(src)[0])); \
        (dst)[1] = (((uint32)(src)[7]) << 24 | \
                    ((uint32)(src)[6]) << 16 | \
                    ((uint32)(src)[5]) << 8 | \
                    ((uint32)(src)[4])); \
        (dst)[2] = (((uint32)(src)[11]) << 24 | \
                    ((uint32)(src)[10]) << 16 | \
                    ((uint32)(src)[9]) << 8 | \
                    ((uint32)(src)[8])); \
        (dst)[3] = (((uint32)(src)[15]) << 24 | \
                    ((uint32)(src)[14]) << 16 | \
                    ((uint32)(src)[13]) << 8 | \
                    ((uint32)(src)[12])); \
    } while (0)

/* Convert addr src of type uint32 [] array to uint8 array  */
#define CBX_UINT8_VAL_FROM_UINT32(dst, src) do {\
        (dst)[0] = (uint8) ((src)[0] & 0xff); \
        (dst)[1] = (uint8) ((src)[0] >> 8 & 0xff); \
        (dst)[2] = (uint8) ((src)[0] >> 16 & 0xff); \
        (dst)[3] = (uint8) ((src)[0] >> 24 & 0xff); \
        (dst)[4] = (uint8) ((src)[1] & 0xff); \
        (dst)[5] = (uint8) ((src)[1] >> 8 & 0xff); \
        (dst)[6] = (uint8) ((src)[1] >> 16 & 0xff); \
        (dst)[7] = (uint8) ((src)[1] >> 24 & 0xff); \
        (dst)[8] = (uint8) ((src)[2] & 0xff); \
        (dst)[9] = (uint8) ((src)[2] >> 8 & 0xff); \
        (dst)[10] = (uint8) ((src)[2] >> 16 & 0xff); \
        (dst)[11] = (uint8) ((src)[2] >> 24 & 0xff); \
        (dst)[12] = (uint8) ((src)[3] & 0xff); \
        (dst)[13] = (uint8) ((src)[3] >> 8 & 0xff); \
        (dst)[14] = (uint8) ((src)[3] >> 16 & 0xff); \
        (dst)[15] = (uint8) ((src)[3] >> 24 & 0xff); \
    } while (0)

#define CBX_BMP_CLEAR(bmp)          CBX_PBMP_CLEAR(bmp)
#define CBX_BMP_ASSIGN(dst, src)    CBX_PBMP_ASSIGN(dst, src)
#define CBX_BMP_OR(bmp_a, bmp_b)    CBX_PBMP_OR(bmp_a, bmp_b)
#define CBX_BMP_ASSIGN(dst, src)    CBX_PBMP_ASSIGN(dst, src)
#define CBX_BMP_ITER(bmp, attr)     CBX_PBMP_ITER(bmp, attr)
#define CBX_BMP_EQ(bmp_a, bmp_b)    CBX_PBMP_EQ(bmp_a, bmp_b)
#define CBX_BMP_NEQ(bmp_a, bmp_b)   CBX_PBMP_NEQ(bmp_a, bmp_b)
#define CBX_BMP_COUNT(bmp, count)   CBX_PBMP_COUNT(bmp, count)
#define CBX_BMP_REMOVE(bmp_a, bmp_b) CBX_PBMP_REMOVE(bmp_a, bmp_b)
#define CBX_BMP_ADD(bmp, attr)       CBX_PBMP_PORT_ADD(bmp, attr)


typedef struct cbxi_field_control_s {
   uint32_t           cfp_state;
   cbx_cfpid_t        cfp_handle;
} cbxi_field_control_t;

typedef struct cbxi_cfp_ikft_info_s {
   uint8              length;
   uint8              header_type;
   uint16             header_offset;
} cbxi_cfp_ikft_info_t;


/* Structure for holding information about all attributes */
typedef struct cbxi_cfp_rule_attr_info_s {
        uint8_t  valid_for_key;
        uint8_t  attr_len;
        uint8_t  attr_data;
        uint64_t attr_mask;
} cbxi_cfp_rule_attribute_info_t;

/* CFP rule information */
typedef struct cbxi_cfp_rule_field_info_s {
	uint8       len;	/* Bits in field */
	uint16      bp;  	/* Least bit position of the field */
} cbxi_cfp_rule_field_info_t;

/*  Global Initialization for all cfp related tables */
int cbxi_cfp_table_init(int unit);

/*  Set fields for the keys */
int cbxi_cfp_ikft_key_set(int unit, int key_location, int key_field);

/*  Set CFP defaullt keys - L2 Ethernet, IPv4, IPv6, L2/IPv4 */
int cbxi_cfp_create_default_lookup_keys(int unit);

/*  Set the KST table entry to be used for selecting LBH and CFP keys */
int cbxi_fpslict_table_update(int unit);

/*  Enable/disbale LBH/CFP and select LBH and 4 CFP keys  */
int cbxi_cfp_kst_entry_update(int unit, int flag);

/*  Convert HW action entry to API parameters  */
int cbxi_cfp_action_convert_entry_to_param(int unit, action_t entry,
                                           cbx_cfp_action_t *action);
/*  Dump CFP action parameters  */
int cbxi_cfp_action_dump_param(cbx_cfp_action_t *action);

/*  Convert action parameters to HW entry  */
int cbxi_cfp_action_convert_param_to_entry(int unit, cbx_cfp_action_t *action,
                                           action_t  *entry);

/*  Validate cfp action parameters  */
int cbxi_cfp_action_param_validate(cbx_cfp_action_t *action, int index);

/*  ADD/DELETE/DUMP CFP action table entry  */
int cbxi_cfp_action_op(int unit, int index,
                       cbx_cfp_action_t *action, int operation);

/*  Init valid attributes bmp for each key type  */
void cbxi_cfp_rule_attr_bmp_set();

/*  Prefill all valid attribute names, their default value/mask for
    given key type  */
void cbxi_cfp_prefill_attribute(cbx_cfp_rule_t *rule, cbx_cfp_l3_l4_pkt_type_t pkt_type);

/*  Validate values of all the attributes passed for rule creation */
int cbxi_cfp_validate_rule_attribute_values(cbx_cfp_rule_attribute_t attr,
                                            uint32_t val);

/*  Validate all the parameters passed for rule creation  */
int cbxi_cfp_rule_param_validate(cbx_cfpid_t *cfpid,
                                 cbx_cfp_rule_t *rule,
                                 uint32_t *l4_pkt_type_attr);

/*  Validate the rule index passed. return key type and l4 packet type */
int cbxi_cfp_index_validate(uint32_t index, cbx_cfp_key_type_t *key_type,
                            cbx_cfp_l3_l4_pkt_type_t *l4_pkt_type);


/*  Set tcam entry fields  */
int cbxi_cfp_tcam_entry_field_set(cbxi_cfp_rule_field_info_t *field_info,
                                  uint32 *regbuf,
                                  uint32 *fldbuf);

/*  Get individual fields from TCAM entry  */
int cbxi_cfp_tcam_entry_field_get(cbxi_cfp_rule_field_info_t  *field_info,
                                  uint32 *regbuf,
                                  uint32 *fldbuf);

/*  Write to CFP CAM entry at the index provided. Update
 *  TCAM at index and index+1 for even rule. Update
 *  TCAM at index+2 and index+3 for odd rule  */
int cbxi_cfp_write_to_tcam(cfpcam_t *rule_x_even,
                           cfpcam_t *rule_x_odd,
                           cfpcam_t *rule_y_even,
                           cfpcam_t *rule_y_odd,
                           uint32_t src_port_x,
                           uint32_t src_port_y,
                           cbx_cfp_key_type_t key_type,
                           int      no_of_rules,
                           uint32_t rule_index);

/*  Create the rule and write it to HW. Return rule index  */
int cbxi_cfp_rule_set(cbx_cfpid_t *cfpid, cbx_cfp_rule_t *rule, uint32_t rule_index);

/*  Delete TCAM entry corresponding to rule index. If rule
    being deleted is double wide, clear WRT entry as well */
int cbxi_cfp_rule_clear(cbx_cfp_key_type_t key_type, uint32_t rule_index);

/*  Update valid bmp for a key/pkt type to remove overlapping attributes */
int cbxi_cfp_update_valid_bmp(cbx_cfp_key_type_t key_type,
                          cbx_cfp_l3_l4_pkt_type_t l4_pkt_type, uint32_t *bmp);

/*  Read TCAM entry, extract attributes and dump their value and mask */
int cbxi_cfp_rule_dump(cbx_cfp_key_type_t key_type,
                   cbx_cfp_l3_l4_pkt_type_t l4_pkt_type,
                   uint32_t rule_index);
#endif  /* !_CBX_INT_CFP_H */
