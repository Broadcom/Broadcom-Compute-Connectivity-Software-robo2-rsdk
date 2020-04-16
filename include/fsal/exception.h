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
 * CBX EXCEPTION
 */

#ifndef __EXCEPTION_H
#define __EXCEPTION_H

#include "soc/drv.h"
#include "soc/defs.h"

#define CBXI_HW_TRAP_ID_START           1
#define CBXI_HW_TRAP_ID_END           127
#define CBXI_MIRROR_GRP_ID_START      128
#define CBXI_MIRROR_GRP_ID_END        191
#define CBXI_SLIC_ID_START            256
#define CBXI_SLIC_ID_END              511
#define CBXI_CFP_ACTION_ID_START     2048
#define CBXI_CFP_ACTION_ID_END       3071
#define CBXI_MCAST_ID_START             0
#define CBXI_MCAST_ID_END            1024

#define NO_TRAP                 ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? NO_TRAP_B0              : NO_TRAP_A0)
#define MAC_ERROR               ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? MAC_ERROR_B0            : MAC_ERROR_A0)
#define BMU_RES_DROP            ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? BMU_RES_DROP_B0         : BMU_RES_DROP_A0)
#define TLV_INV_LEN             ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? TLV_INV_LEN_B0          : TLV_INV_LEN_A0)
#define TLV_INV_TYPE            ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? TLV_INV_TYPE_B0         : TLV_INV_TYPE_A0)
#define TLV_MAX_NUM             ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? TLV_MAX_NUM_B0          : TLV_MAX_NUM_A0)
#define TLV_MAX_LEN             ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? TLV_MAX_LEN_B0          : TLV_MAX_LEN_A0)
#define RESERVED_TRAP0          ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? RESERVED_TRAP0_B0       : RESERVED_TRAP0_A0)
#define RESERVED_TRAP1          ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? RESERVED_TRAP1_B0       : RESERVED_TRAP1_A0)
#define ETH_BCAST_DMAC          ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? ETH_BCAST_DMAC_B0       : ETH_BCAST_DMAC_A0)
#define ETH_MCAST_SMAC          ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? ETH_MCAST_SMAC_B0       : ETH_MCAST_SMAC_A0)
#define ETH_MAC_LAND            ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? ETH_MAC_LAND_B0         : ETH_MAC_LAND_A0)
#define ETH_VID_FFF             ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? ETH_VID_FFF_B0          : ETH_VID_FFF_A0)
#define ETH_ZOMBIE_LEN          ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? ETH_ZOMBIE_LEN_B0       : ETH_ZOMBIE_LEN_A0)
#define RESERVED_TRAP2          ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? RESERVED_TRAP2_B0       : RESERVED_TRAP2_A0)
#define RESERVED_TRAP3          ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? RESERVED_TRAP3_B0       : RESERVED_TRAP3_A0)
#define RP_TERMINATE            ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? RP_TERMINATE_B0         : RP_TERMINATE_A0)
#define SLIC_GROUP1             ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? SLIC_GROUP1_B0          : SLIC_GROUP1_A0)
#define SLIC_GROUP2             ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? SLIC_GROUP2_B0          : SLIC_GROUP2_A0)
#define SLIC_GROUP3             ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? SLIC_GROUP3_B0          : SLIC_GROUP3_A0)
#define SLIC_GROUP4             ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? SLIC_GROUP4_B0          : SLIC_GROUP4_A0)
#define SLIC_GROUP5             ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? SLIC_GROUP5_B0          : SLIC_GROUP5_A0)
#define SLIC_GROUP6             ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? SLIC_GROUP6_B0          : SLIC_GROUP6_A0)
#define SLIC_GROUP7             ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? SLIC_GROUP7_B0          : SLIC_GROUP7_A0)
#define SLIC_GROUP8             ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? SLIC_GROUP8_B0          : SLIC_GROUP8_A0)
#define SLIC_GROUP9             ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? SLIC_GROUP9_B0          : SLIC_GROUP9_A0)
#define SLIC_GROUP10            ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? SLIC_GROUP10_B0         : SLIC_GROUP10_A0)
#define SLIC_GROUP11            ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? SLIC_GROUP11_B0         : SLIC_GROUP11_A0)
#define SLIC_GROUP12            ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? SLIC_GROUP12_B0         : SLIC_GROUP12_A0)
#define SLIC_GROUP13            ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? SLIC_GROUP13_B0         : SLIC_GROUP13_A0)
#define SLIC_GROUP14            ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? SLIC_GROUP14_B0         : SLIC_GROUP14_A0)
#define SLIC_GROUP15            ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? SLIC_GROUP15_B0         : SLIC_GROUP15_A0)
#define BMU_METER_DROP          ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? BMU_METER_DROP_B0       : BMU_METER_DROP_A0)
#define CFP_GROUP1              ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? CFP_GROUP1_B0           : CFP_GROUP1_A0)
#define CFP_GROUP2              ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? CFP_GROUP2_B0           : CFP_GROUP2_A0)
#define CFP_GROUP3              ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? CFP_GROUP3_B0           : CFP_GROUP3_A0)
#define CFP_GROUP4              ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? CFP_GROUP4_B0           : CFP_GROUP4_A0)
#define CFP_GROUP5              ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? CFP_GROUP5_B0           : CFP_GROUP5_A0)
#define CFP_GROUP6              ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? CFP_GROUP6_B0           : CFP_GROUP6_A0)
#define CFP_GROUP7              ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? CFP_GROUP7_B0           : CFP_GROUP7_A0)
#define CFP_GROUP8              ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? CFP_GROUP8_B0           : CFP_GROUP8_A0)
#define CFP_GROUP9              ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? CFP_GROUP9_B0           : CFP_GROUP9_A0)
#define CFP_GROUP10             ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? CFP_GROUP10_B0          : CFP_GROUP10_A0)
#define CFP_GROUP11             ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? CFP_GROUP11_B0          : CFP_GROUP11_A0)
#define CFP_GROUP12             ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? CFP_GROUP12_B0          : CFP_GROUP12_A0)
#define CFP_GROUP13             ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? CFP_GROUP13_B0          : CFP_GROUP13_A0)
#define CFP_GROUP14             ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? CFP_GROUP14_B0          : CFP_GROUP14_A0)
#define CFP_GROUP15             ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? CFP_GROUP15_B0          : CFP_GROUP15_A0)
#define MPLS_TTL_ONE            ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? MPLS_TTL_ONE_B0         : MPLS_TTL_ONE_A0)
#define IPV4_HDR_CHECKSUM       ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? IPV4_HDR_CHECKSUM_B0    : IPV4_HDR_CHECKSUM_A0)
#define IPV4_HDR_VERSION        ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? IPV4_HDR_VERSION_B0     : IPV4_HDR_VERSION_A0)
#define IPV4_HDR_LENGTH         ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? IPV4_HDR_LENGTH_B0      : IPV4_HDR_LENGTH_A0)
#define IPV4_MIN_LENGTH         ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? IPV4_MIN_LENGTH_B0      : IPV4_MIN_LENGTH_A0)
#define IPV4_MAX_LENGTH         ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? IPV4_MAX_LENGTH_B0      : IPV4_MAX_LENGTH_A0)
#define IPV4_HDR_OPTIONS        ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? IPV4_HDR_OPTIONS_B0     : IPV4_HDR_OPTIONS_A0)
#define IPV4_FRAGMENT           ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? IPV4_FRAGMENT_B0        : IPV4_FRAGMENT_A0)
#define IPV4_SHORT_FRAGMENT     ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? IPV4_SHORT_FRAGMENT_B0  : IPV4_SHORT_FRAGMENT_A0)
#define IPV4_MCAST_SA           ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? IPV4_MCAST_SA_B0        : IPV4_MCAST_SA_A0)
#define IPV4_BCAST_DA           ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? IPV4_BCAST_DA_B0        : IPV4_BCAST_DA_A0)
#define IPV4_DA_ZERO            ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? IPV4_DA_ZERO_B0         : IPV4_DA_ZERO_A0)
#define IPV4_LOOPBACK           ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? IPV4_LOOPBACK_B0        : IPV4_LOOPBACK_A0)
#define IPV4_DA_EQ_SA           ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? IPV4_DA_EQ_SA_B0        : IPV4_DA_EQ_SA_A0)
#define IPV4_NONAT              ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? IPV4_NONAT_B0           : IPV4_NONAT_A0)
#define IPV4_PRIVATE            ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? IPV4_PRIVATE_B0         : IPV4_PRIVATE_A0)
#define IPV4_TCP_HDR_REWRITE    ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? IPV4_TCP_HDR_REWRITE_B0 : IPV4_TCP_HDR_REWRITE_A0)
#define IPV4_ICMP_FRAGMENT      ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? IPV4_ICMP_FRAGMENT_B0   : IPV4_ICMP_FRAGMENT_A0)
#define IPV4_TTL_ZERO           ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? IPV4_TTL_ZERO_B0        : IPV4_TTL_ZERO_A0)
#define IPV4_TTL_ONE            ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? IPV4_TTL_ONE_B0         : IPV4_TTL_ONE_A0)
#define IPV6_VERSION            ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? IPV6_VERSION_B0         : IPV6_VERSION_A0)
#define IPV6_TLEN_CHK           ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? IPV6_TLEN_CHK_B0        : IPV6_TLEN_CHK_A0)
#define IPV6_FRAGMENT           ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? IPV6_FRAGMENT_B0        : IPV6_FRAGMENT_A0)
#define IPV6_SHORT_FRAGMENT     ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? IPV6_SHORT_FRAGMENT_B0  : IPV6_SHORT_FRAGMENT_A0)
#define IPV6_MCAST_SA           ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? IPV6_MCAST_SA_B0        : IPV6_MCAST_SA_A0)
#define IPV6_DA_EQ_SA           ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? IPV6_DA_EQ_SA_B0        : IPV6_DA_EQ_SA_A0)
#define IPV6_DA_ZERO            ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? IPV6_DA_ZERO_B0         : IPV6_DA_ZERO_A0)
#define IPV6_LOOPBACK           ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? IPV6_LOOPBACK_B0        : IPV6_LOOPBACK_A0)
#define IPV6_HOP_LIMIT_ZERO     ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? IPV6_HOP_LIMIT_ZERO_B0  : IPV6_HOP_LIMIT_ZERO_A0)
#define IPV6_HOP_LIMIT_ONE      ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? IPV6_HOP_LIMIT_ONE_B0   : IPV6_HOP_LIMIT_ONE_A0)
#define IPV6_ICMP_FRAGMENT      ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? IPV6_ICMP_FRAGMENT_B0   : IPV6_ICMP_FRAGMENT_A0)
#define TCP_DP_EQ_SP            ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? TCP_DP_EQ_SP_B0         : TCP_DP_EQ_SP_A0)
#define TCP_NULL_SCAN           ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? TCP_NULL_SCAN_B0        : TCP_NULL_SCAN_A0)
#define TCP_XMAS_SCAN           ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? TCP_XMAS_SCAN_B0        : TCP_XMAS_SCAN_A0)
#define TCP_SYN_FIN_SCAN        ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? TCP_SYN_FIN_SCAN_B0     : TCP_SYN_FIN_SCAN_A0)
#define TCP_SYNC                ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? TCP_SYNC_B0             : TCP_SYNC_A0)
#define TCP_HDR_LENGTH          ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? TCP_HDR_LENGTH_B0       : TCP_HDR_LENGTH_A0)
#define TCP_HDR_FRAGMENT        ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? TCP_HDR_FRAGMENT_B0     : TCP_HDR_FRAGMENT_A0)
#define UDP_DP_EQ_SP            ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? UDP_DP_EQ_SP_B0         : UDP_DP_EQ_SP_A0)
#define ICMP4_LONG_PING         ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? ICMP4_LONG_PING_B0      : ICMP4_LONG_PING_A0)
#define ICMP6_LONG_PING         ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? ICMP6_LONG_PING_B0      : ICMP6_LONG_PING_A0)
#define IPV4_SMURF              ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? IPV4_SMURF_B0           : IPV4_SMURF_A0)
#define IPV6_SMURF              ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? IPV6_SMURF_B0           : IPV6_SMURF_A0)
#define SNOOP_PTP_UDP           ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? SNOOP_PTP_UDP_B0        : SNOOP_PTP_UDP_A0)
#define SNOOP_PTP_ET            ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? SNOOP_PTP_ET_B0         : SNOOP_PTP_ET_A0)
#define SNOOP_ARP               ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? SNOOP_ARP_B0            : SNOOP_ARP_A0)
#define SNOOP_RARP              ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? SNOOP_RARP_B0           : SNOOP_RARP_A0)
#define SNOOP_DHCP4             ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? SNOOP_DHCP4_B0          : SNOOP_DHCP4_A0)
#define SNOOP_ICMP4             ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? SNOOP_ICMP4_B0          : SNOOP_ICMP4_A0)
#define SNOOP_IGMP              ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? SNOOP_IGMP_B0           : SNOOP_IGMP_A0)
#define SNOOP_MLD               ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? SNOOP_MLD_B0            : SNOOP_MLD_A0)
#define SNOOP_ND_RD             ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? SNOOP_ND_RD_B0          : SNOOP_ND_RD_A0)
#define SNOOP_ICMP6             ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? SNOOP_ICMP6_B0          : SNOOP_ICMP6_A0)
#define SNOOP_DHCP6             ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? SNOOP_DHCP6_B0          : SNOOP_DHCP6_A0)
#define RESERVED_TRAP4          ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? RESERVED_TRAP4_B0       : RESERVED_TRAP4_A0)
#define RESERVED_TRAP5          ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? RESERVED_TRAP5_B0       : RESERVED_TRAP5_A0)
#define RESERVED_TRAP6          ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? RESERVED_TRAP6_B0       : RESERVED_TRAP6_A0)
#define RESERVED_TRAP7          ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? RESERVED_TRAP7_B0       : RESERVED_TRAP7_A0)
#define RESERVED_TRAP8          ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? RESERVED_TRAP8_B0       : RESERVED_TRAP8_A0)
#define RESERVED_TRAP9          ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? RESERVED_TRAP9_B0       : RESERVED_TRAP9_A0)
#define RESERVED_TRAP10         ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? RESERVED_TRAP10_B0      : RESERVED_TRAP10_A0)
#define RESERVED_TRAP11         ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? RESERVED_TRAP11_B0      : RESERVED_TRAP11_A0)
#define RESERVED_TRAP12         ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? RESERVED_TRAP12_B0      : RESERVED_TRAP12_A0)
#define RESERVED_TRAP13         ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? RESERVED_TRAP13_B0      : RESERVED_TRAP13_A0)
#define FM_DST_COPY_CPU         ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? FM_DST_COPY_CPU_B0      : FM_DST_COPY_CPU_A0)
#define FM_COPY_CPU             ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? FM_COPY_CPU_B0          : FM_COPY_CPU_A0)
#define ULF_DROP                ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? ULF_DROP_B0             : ULF_DROP_A0)
#define MLF_DROP                ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? MLF_DROP_B0             : MLF_DROP_A0)
#define BC_DROP                 ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? BC_DROP_B0              : BC_DROP_A0)
#define CA_SA_MOVE              ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? CA_SA_MOVE_B0           : CA_SA_MOVE_A0)
#define SA_MOVE                 ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? SA_MOVE_B0              : SA_MOVE_A0)
#define MAC_LEARN_LIMIT         ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? MAC_LEARN_LIMIT_B0      : MAC_LEARN_LIMIT_A0)
#define MAC_SA_FULL             ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? MAC_SA_FULL_B0          : MAC_SA_FULL_A0)
#define CA_LEARN_FRAME          ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? CA_LEARN_FRAME_B0       : CA_LEARN_FRAME_A0)
#define LEARN_FRAME             ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? LEARN_FRAME_B0          : LEARN_FRAME_A0)
#define NO_LRN_LKP_FAIL         ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? NO_LRN_LKP_FAIL_B0      : NO_LRN_LKP_FAIL_A0)
#define VLAN_UNREG              ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? VLAN_UNREG_B0           : VLAN_UNREG_A0)
#define INV_SLI                 ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? INV_SLI_B0              : INV_SLI_A0)
#define INV_FWD_OP              ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? INV_FWD_OP_B0           : INV_FWD_OP_A0)
#define RESERVED_TRAP14         ((SOC_DRIVER(0)->type == SOC_CHIP_BCM53158_B0) ? RESERVED_TRAP14_B0      : RESERVED_TRAP14_A0)

typedef uint8_t cbx_trap_t;

/**
 * @typedef cbx_trap_a0_t
 * Trap Identifier
 */
typedef enum {
    NO_TRAP_A0              = 0,    /* No Error, Trap, or Filter match */
    MAC_ERROR_A0            = 1,    /* Error indication from MAC. This encompasses a number of errors from the MAC. */
    BMU_RES_DROP_A0         = 2,    /* BMU is out of resource */
    TLV_INV_LEN_A0          = 3,    /* TLV received with invalid length (=0,1 or great than MAX TLV) */
    TLV_INV_TYPE_A0         = 4,    /* TLV received with invalid type */
    TLV_MAX_NUM_A0          = 5,    /* Maximum number of TLVs were processed without LAST set */
    TLV_MAX_LEN_A0          = 6,    /* Maximum length of all TLV was exceed. */
    RESERVED_TRAP0_A0       = 7,    /* Reserved */
    RESERVED_TRAP1_A0       = 8,    /* Reserved */
    ETH_BCAST_DMAC_A0       = 9,    /* Broadcast MAC Address (DA=ff-ff-ff-ff-ff-ff) */
    ETH_MCAST_SMAC_A0       = 10,   /* Multicast MAC Address */
    ETH_MAC_LAND_A0         = 11,   /* MAC DA = MAC SA */
    ETH_VID_FFF_A0          = 12,   /* VID is 0xFFF in a C_TAG or S_TAG frame */
    ETH_ZOMBIE_LEN_A0       = 13,   /* A length/Etype field was between 1500 and 1536 */
    RESERVED_TRAP2_A0       = 14,   /* Reserved */
    RESERVED_TRAP3_A0       = 15,   /* Reserved */
    RP_TERMINATE_A0         = 16,   /*
                                     * Indicates a terminal trap condition occurred in the Ingress Receive Processor.
                                     * If there is no metadata to communicate the trap condition then this is the trap
                                     * that is signaled. If there is metadata then the precise trap condition will be signaled.
                                     */
    SLIC_GROUP1_A0          = 17,   /* SLIC Trap Group1 */
    SLIC_GROUP2_A0          = 18,   /* SLIC Trap Group2 */
    SLIC_GROUP3_A0          = 19,   /* SLIC Trap Group3 */
    SLIC_GROUP4_A0          = 20,   /* SLIC Trap Group4 */
    SLIC_GROUP5_A0          = 21,   /* SLIC Trap Group5 */
    SLIC_GROUP6_A0          = 22,   /* SLIC Trap Group6 */
    SLIC_GROUP7_A0          = 23,   /* SLIC Trap Group7 */
    SLIC_GROUP8_A0          = 24,   /* SLIC Trap Group8 */
    SLIC_GROUP9_A0          = 25,   /* SLIC Trap Group9 */
    SLIC_GROUP10_A0         = 26,   /* SLIC Trap Group10 */
    SLIC_GROUP11_A0         = 27,   /* SLIC Trap Group11 */
    SLIC_GROUP12_A0         = 28,   /* SLIC Trap Group12 */
    SLIC_GROUP13_A0         = 29,   /* SLIC Trap Group13 */
    SLIC_GROUP14_A0         = 30,   /* SLIC Trap Group14 */
    SLIC_GROUP15_A0         = 31,   /* SLIC Trap Group15 */
    BMU_METER_DROP_A0       = 32,   /* BMU Meter Drop */
    CFP_GROUP1_A0           = 33,   /* CFP Trap Group1 */
    CFP_GROUP2_A0           = 34,   /* CFP Trap Group2 */
    CFP_GROUP3_A0           = 35,   /* CFP Trap Group3 */
    CFP_GROUP4_A0           = 36,   /* CFP Trap Group4 */
    CFP_GROUP5_A0           = 37,   /* CFP Trap Group5 */
    CFP_GROUP6_A0           = 38,   /* CFP Trap Group6 */
    CFP_GROUP7_A0           = 39,   /* CFP Trap Group7 */
    CFP_GROUP8_A0           = 40,   /* CFP Trap Group8 */
    CFP_GROUP9_A0           = 41,   /* CFP Trap Group9 */
    CFP_GROUP10_A0          = 42,   /* CFP Trap Group10 */
    CFP_GROUP11_A0          = 43,   /* CFP Trap Group11 */
    CFP_GROUP12_A0          = 44,   /* CFP Trap Group12 */
    CFP_GROUP13_A0          = 45,   /* CFP Trap Group13 */
    CFP_GROUP14_A0          = 46,   /* CFP Trap Group14 */
    CFP_GROUP15_A0          = 47,   /* CFP Trap Group15 */
    MPLS_TTL_ONE_A0         = 48,   /*
                                     * The MPLS header's TTL is one and the packet is being routed
                                     * (FPSLICT dst_key field is FID_MPLS). This would lead to a TTL decrement on
                                     * egress which would result in a TTL of zero.
                                     */
    IPV4_HDR_CHECKSUM_A0    = 49,   /* IPv4 Header Checksum Failure([RFC1812 5.5.2 (2)] */
    IPV4_HDR_VERSION_A0     = 50,   /* IPv4 Version failure ([RFC1812 5.5.2 (3)] */
    IPV4_HDR_LENGTH_A0      = 51,   /* The IHL field is must be greater than or equal to 5 ([RFC1812 5.5.2 (4)] */
    IPV4_MIN_LENGTH_A0      = 52,   /*
                                     * If the number of bytes from the L2 payload must be greater than 4*IHL Bytes.
                                     * [RFC1812 5.5.2. (1)]
                                     */
    IPV4_MAX_LENGTH_A0      = 53,   /*
                                     * The Total Length from the IPv4 header is compared to the length of the entire packet.
                                     * ([RFC1812 5.5.2 (5)]  Not available for cut-through packets
                                     */
    IPV4_HDR_OPTIONS_A0     = 54,   /* The IP Header Length (IHL) is compared to 5 to determine if options are present. */
    IPV4_FRAGMENT_A0        = 55,   /* Fragmented Packet (ip.MF=1 or ip.offset =! 0) */
    IPV4_SHORT_FRAGMENT_A0  = 56,   /*
                                     * (MF) bit set to one and has a Total Length field less than the amount specified
                                     * in the Short_Frag_Config register. AKA Teardrop.
                                     */
    IPV4_MCAST_SA_A0        = 57,   /* (SA[31:28]=14 or SA=255.255.255.255). */
    IPV4_BCAST_DA_A0        = 58,   /* (DA=255.255.255.255). */
    IPV4_DA_ZERO_A0         = 59,   /* IPv4 Source Address is multicast (DA[31:28] = 0x0) 0.0.0.0/8 */
    IPV4_LOOPBACK_A0        = 60,   /* IPv4 SA or DA is loopback (127.0.0.0/8) */
    IPV4_DA_EQ_SA_A0        = 61,   /* IPv4 SA == DA */
    IPV4_NONAT_A0           = 62,   /*
                                     * The IPv4 header's SA and DA are not in the same address space where an address
                                     * space is one of the private address spaces (10.0.0.0/8, 172.16.0.0/12, 192.168.0.0/16)
                                     * or the public address space.
                                     */
    IPV4_PRIVATE_A0         = 63,   /* DA or SA is 10.0.0.0/8, 172.16.0.0/ 12, or 192.168.0.0./16 */
    IPV4_TCP_HDR_REWRITE_A0 = 64,   /* IPv4 Protocol is TCP (0x6) and IPv4 offset is 0x1 or 0x2 */
    IPV4_ICMP_FRAGMENT_A0   = 65,   /* IPv4 protocol is ICMP (0x01) and offset != 0x00 || mf=1 */
    IPV4_TTL_ZERO_A0        = 66,   /* IPv4_TTL is zero */
    IPV4_TTL_ONE_A0         = 67,   /* IPv4_TTL expired for routed traffic */
    IPV6_VERSION_A0         = 68,   /* The Version field must be 6 for frames reported at IPv6 */
    IPV6_TLEN_CHK_A0        = 69,   /*
                                     * The number of bytes remaining in the frame is less than 40,
                                     * then a valid IPv6 packet is not possible.
                                     */
    IPV6_FRAGMENT_A0        = 70,   /* IPv6 packet contains an IPv6 Fragmentation header, then it is considered fragmented. */
    IPV6_SHORT_FRAGMENT_A0  = 71,   /*
                                     *  More Fragments (MF) bit set to one and Payload Length field less than the
                                     *  amount specified in the Short_Frag_Config register. Teardrop.
                                     */
    IPV6_MCAST_SA_A0        = 72,   /* IPv6 SA is multicast (SA[127:120] = 0xFF) */
    IPV6_DA_EQ_SA_A0        = 73,   /* IPv6 SA = DA */
    IPV6_DA_ZERO_A0         = 74,   /* IPv6 DA = ::0 */
    IPV6_LOOPBACK_A0        = 75,   /* IPv6 DA or SA is 0x0000001 */
    IPV6_HOP_LIMIT_ZERO_A0  = 76,   /* IPv6 Next Hop is 0x0 */
    IPV6_HOP_LIMIT_ONE_A0   = 77,   /*
                                     * The IPv6 header's Next Hop is one and the packet is being routed. This would
                                     * lead to a Next Hop decrement on egress  which would result in a Next Hop of zero.
                                     * This condition is recognized by the Deep Frame Parser but the trap isn't
                                     * invoked until the VSI is determined and the VSIT lookup indicates that
                                     * the VSI is an L3 switch.
                                     */
    IPV6_ICMP_FRAGMENT_A0   = 78,   /* ICMPv6 protocol data carried in a fragmented IPV6 datagram. */
    TCP_DP_EQ_SP_A0         = 79,   /*
                                     * DPort_A0 = SPort in a TCP header, in an unfragmented IP datagram or
                                     * the first fragment of a fragmented IP datagram
                                     */
    TCP_NULL_SCAN_A0        = 80,   /*
                                     * Seq_Num=0 and all TCP flags=0 in a TCP header carried an unfragmented
                                     * IP datagram or the first fragment of a fragmented IP datagram
                                     */
    TCP_XMAS_SCAN_A0        = 81,   /*
                                     * Seq_Num=0, FIN=1, URG=1, and PSH=1 in a TCP header carried an unfragmented
                                     * IP datagram or the first fragment of a fragmented IP datagram
                                     */
    TCP_SYN_FIN_SCAN_A0     = 82,   /*
                                     * SYN=1 and FIN=1 in a TCP header carried an unfragmented IP datagram or
                                     * the first fragment of a fragmented IP datagram
                                     */
    TCP_SYNC_A0             = 83,   /*
                                     * SYN=1, ACK=0, and SRC_PORT<1024 in a TCP header carried an unfragmented IP
                                     * datagram or the first fragment of a fragmented IP datagram.
                                     */
    TCP_HDR_LENGTH_A0       = 84,   /*
                                     * The length of a TCP header carried in the first fragment of a fragmented IP
                                     * datagram is less than a configured MIN_TCP_Header_Size.
                                     * (Default set at 20B, which is the minimal length)
                                     */
    TCP_HDR_FRAGMENT_A0     = 85,   /*
                                     * The first IP fragment must be large enough to contain all required TCP header. For
                                     * IP packets with Fragment Offset_A0 = 0, its length must be longer than a set threshold.
                                     */
    UDP_DP_EQ_SP_A0         = 86,   /*
                                     * DPort=SPort in a UDP header, in an unfragmented IP datagram or
                                     * the first fragment of a fragmented IP datagram
                                     */
    ICMP4_LONG_PING_A0      = 87,   /*
                                     * ICMPv4 Ping (Echo Request) PDU is carried in an un-fragmented IPv4 datagram
                                     * whose Total Length has a value that is greater than MAX_ICMPv4_Size +
                                     * size of IPv4 header. Where the MAX_ICMPv4_Size is programmable between
                                     * 0 and 9.6K bytes, inclusive. The default is set to 512.
                                     */
    ICMP6_LONG_PING_A0      = 88,   /*
                                     * ICMPv6 Ping (Echo Request) PDU is carried in an un-fragmented IPv6 datagram whose
                                     * Payload Length has a value greater than MAX_ICMPv6_Size, where the MAX_ICMPv6_Size
                                     * is programmable between 0 and 9.6K bytes, inclusive. The default is set to 512.
                                     */
    IPV4_SMURF_A0           = 89,   /*
                                     * IPv4 frame to broadcast address (255.255.255.255) with protocol=ICMP OR UDP,
                                     * CODE=ECHO OR DPORT=7 {SMURF, FRAGLE}
                                     */
    IPV6_SMURF_A0           = 90,   /*
                                     * IPv6 frame to broadcast link local address FF02:: with protocol=ICMP OR UDP,
                                     * CODE=ECHO OR DPORT=7 {SMURF, FRAGLE}
                                     */
    SNOOP_PTP_UDP_A0        = 91,   /* PTP over UDP. IPV4: DA=224.0.0.107; IPv6: DA=FF02::6B UDP DPORT=319 */
    SNOOP_PTP_ET_A0         = 92,   /* PTP over L2 (802.1AS): Ethertype = 0x88F7 */
    SNOOP_ARP_A0            = 93,   /* IPv4 ARP: DMAC=broadcast, Ethertype = 0x0806 */
    SNOOP_RARP_A0           = 94,   /* IPv4 RARP: Ethertype = 0x8035 */
    SNOOP_DHCP4_A0          = 95,   /*
                                     * IPv4 DHCP Packet: L2 Payload is IPv4, IP Protocol_A0 = 17 (UDP)
                                     * Dest UDP Port_A0 = 67(server) Src UDP Port = 68(client)
                                     */
    SNOOP_ICMP4_A0          = 96,   /* IPv4 ICMP L2 Payload is IPv4 IPv4 Protocol number: 1 (ICMP) */
    SNOOP_IGMP_A0           = 97,   /* IPv4 IGMP L2 Payload is IPv4 IPv4 Protocol number: 2 (IGMP) */
    SNOOP_MLD_A0            = 98,   /* IPv6 MLD L2 Payload is IPv6 IPv6 Next Header is ICMPv6 (58) + Type = 130/131/132 */
    SNOOP_ND_RD_A0          = 99,   /*
                                     * IPv6 Discovery (ND/RD) L2 Payload is IPv6 IPv6 Next Header is
                                     * ICMPv6 (58) + Type_A0 = 133/134/135/136/137
                                     */
    SNOOP_ICMP6_A0          = 100,  /* L2 Payload is IPv6 IPv6 next header is ICMPv6(58) */
    SNOOP_DHCP6_A0          = 101,  /* IPv6 DHCP6 Packet: IPv6 Next Header is UDP, DP=546/547 */
    RESERVED_TRAP4_A0       = 102,  /* Reserved */
    RESERVED_TRAP5_A0       = 103,  /* Reserved */
    RESERVED_TRAP6_A0       = 104,  /* Reserved */
    RESERVED_TRAP7_A0       = 105,  /* Reserved */
    RESERVED_TRAP8_A0       = 106,  /* Reserved */
    RESERVED_TRAP9_A0       = 107,  /* Reserved */
    RESERVED_TRAP10_A0      = 108,  /* Reserved */
    RESERVED_TRAP11_A0      = 109,  /* Reserved */
    RESERVED_TRAP12_A0      = 110,  /* Reserved */
    RESERVED_TRAP13_A0      = 111,  /* Reserved */
    FM_DST_COPY_CPU_A0      = 112,  /* FM destination lookup fwd_ctrl indicates copy to CPU. */
    FM_COPY_CPU_A0          = 113,  /* L2 ARL copy to cpu */
    ULF_DROP_A0             = 114,  /* L2 DMAC Unicast frame lookup failure. */
    MLF_DROP_A0             = 115,  /* L2 DMAC Multicast frame lookup failure. */
    BC_DROP_A0              = 116,  /* L2 DMAC Broadcast frame lookup failure. */
    CA_SA_MOVE_A0           = 117,  /* Trap from cascade device, SA (station) moved SLI */
    SA_MOVE_A0              = 118,  /* SA (station) moved SLI */
    MAC_LEARN_LIMIT_A0      = 119,  /* Port has reached L2 SMAC self-learned limit */
    MAC_SA_FULL_A0          = 120,  /* Hardware learning tried to insert a SMAC and all buckets are full */
    CA_LEARN_FRAME_A0       = 121,  /* Trap from cascade device, Learning Frame was copied to the CPU */
    LEARN_FRAME_A0          = 122,  /* L2 Learning Frame was copied to the CPU */
    NO_LRN_LKP_FAIL_A0      = 123,  /* Source lookup fail for a frame for which learning was not attempted */
    VLAN_UNREG_A0           = 124,  /*
                                     * Indexed VLAN is not a registered VLAN. This can occur based on a LIM miss
                                     * or PV2LI port not in membership.
                                     */
    INV_SLI_A0              = 125,  /* SLI of the packet is invalid (=0) */
    INV_FWD_OP_A0           = 126,  /* A CB-tag was received with an invalid/unsupported FWD_OP code. */
    RESERVED_TRAP14_A0      = 127,  /* Reserved */

} cbx_trap_a0_t;

/**
 * @typedef cbx_trap_b0_t
 * Trap Identifier
 */
typedef enum {
    NO_TRAP_B0              = 0,    /* No Error, Trap, or Filter match */
    CFP_GROUP1_B0           = 1,    /* CFP Trap Group1 */
    CFP_GROUP2_B0           = 2,    /* CFP Trap Group2 */
    CFP_GROUP3_B0           = 3,    /* CFP Trap Group3 */
    CFP_GROUP4_B0           = 4,    /* CFP Trap Group4 */
    CFP_GROUP5_B0           = 5,    /* CFP Trap Group5 */
    CFP_GROUP6_B0           = 6,    /* CFP Trap Group6 */
    CFP_GROUP7_B0           = 7,    /* CFP Trap Group7 */
    CFP_GROUP8_B0           = 8,    /* CFP Trap Group8 */
    CFP_GROUP9_B0           = 9,    /* CFP Trap Group9 */
    CFP_GROUP10_B0          = 10,   /* CFP Trap Group10 */
    CFP_GROUP11_B0          = 11,   /* CFP Trap Group11 */
    CFP_GROUP12_B0          = 12,   /* CFP Trap Group12 */
    CFP_GROUP13_B0          = 13,   /* CFP Trap Group13 */
    CFP_GROUP14_B0          = 14,   /* CFP Trap Group14 */
    CFP_GROUP15_B0          = 15,   /* CFP Trap Group15 */
    MAC_ERROR_B0            = 16,   /* Error indication from MAC. This encompasses a number of errors from the MAC. */
    BMU_RES_DROP_B0         = 17,   /* BMU is out of resource */
    TLV_INV_LEN_B0          = 18,   /* TLV received with invalid length (=0,1 or great than MAX TLV) */
    TLV_INV_TYPE_B0         = 19,   /* TLV received with invalid type */
    TLV_MAX_NUM_B0          = 20,   /* Maximum number of TLVs were processed without LAST set */
    TLV_MAX_LEN_B0          = 21,   /* Maximum length of all TLV was exceed. */
    RESERVED_TRAP0_B0       = 22,   /* Reserved */
    RESERVED_TRAP1_B0       = 23,   /* Reserved */
    ETH_BCAST_DMAC_B0       = 24,   /* Broadcast MAC Address (DA=ff-ff-ff-ff-ff-ff) */
    ETH_MCAST_SMAC_B0       = 25,   /* Multicast MAC Address */
    ETH_MAC_LAND_B0         = 26,   /* MAC DA = MAC SA */
    ETH_VID_FFF_B0          = 27,   /* VID is 0xFFF in a C_TAG or S_TAG frame */
    ETH_ZOMBIE_LEN_B0       = 28,   /* A length/Etype field was between 1500 and 1536 */
    RESERVED_TRAP2_B0       = 29,   /* Reserved */
    RESERVED_TRAP3_B0       = 30,   /* Reserved */
    RP_TERMINATE_B0         = 31,   /*
                                     * Indicates a terminal trap condition occurred in the Ingress Receive Processor.
                                     * If there is no metadata to communicate the trap condition then this is the trap
                                     * that is signaled. If there is metadata then the precise trap condition will be signaled.
                                     */
    SLIC_GROUP1_B0          = 32,   /* SLIC Trap Group1 */
    SLIC_GROUP2_B0          = 33,   /* SLIC Trap Group2 */
    SLIC_GROUP3_B0          = 34,   /* SLIC Trap Group3 */
    SLIC_GROUP4_B0          = 35,   /* SLIC Trap Group4 */
    SLIC_GROUP5_B0          = 36,   /* SLIC Trap Group5 */
    SLIC_GROUP6_B0          = 37,   /* SLIC Trap Group6 */
    SLIC_GROUP7_B0          = 38,   /* SLIC Trap Group7 */
    SLIC_GROUP8_B0          = 39,   /* SLIC Trap Group8 */
    SLIC_GROUP9_B0          = 40,   /* SLIC Trap Group9 */
    SLIC_GROUP10_B0         = 41,   /* SLIC Trap Group10 */
    SLIC_GROUP11_B0         = 42,   /* SLIC Trap Group11 */
    SLIC_GROUP12_B0         = 43,   /* SLIC Trap Group12 */
    SLIC_GROUP13_B0         = 44,   /* SLIC Trap Group13 */
    SLIC_GROUP14_B0         = 45,   /* SLIC Trap Group14 */
    SLIC_GROUP15_B0         = 46,   /* SLIC Trap Group15 */
    BMU_METER_DROP_B0       = 47,   /* BMU Meter Drop */
    MPLS_TTL_ONE_B0         = 48,   /*
                                     * The MPLS header's TTL is one and the packet is being routed
                                     * (FPSLICT dst_key field is FID_MPLS). This would lead to a TTL decrement on
                                     * egress which would result in a TTL of zero.
                                     */
    IPV4_HDR_CHECKSUM_B0    = 49,   /* IPv4 Header Checksum Failure([RFC1812 5.5.2 (2)] */
    IPV4_HDR_VERSION_B0     = 50,   /* IPv4 Version failure ([RFC1812 5.5.2 (3)] */
    IPV4_HDR_LENGTH_B0      = 51,   /* The IHL field is must be greater than or equal to 5 ([RFC1812 5.5.2 (4)] */
    IPV4_MIN_LENGTH_B0      = 52,   /*
                                     * If the number of bytes from the L2 payload must be greater than 4*IHL Bytes.
                                     * [RFC1812 5.5.2. (1)]
                                     */
    IPV4_MAX_LENGTH_B0      = 53,   /*
                                     * The Total Length from the IPv4 header is compared to the length of the entire packet.
                                     * ([RFC1812 5.5.2 (5)]  Not available for cut-through packets
                                     */
    IPV4_HDR_OPTIONS_B0     = 54,   /* The IP Header Length (IHL) is compared to 5 to determine if options are present. */
    IPV4_FRAGMENT_B0        = 55,   /* Fragmented Packet (ip.MF=1 or ip.offset =! 0) */
    IPV4_SHORT_FRAGMENT_B0  = 56,   /*
                                     * (MF) bit set to one and has a Total Length field less than the amount specified
                                     * in the Short_Frag_Config register. AKA Teardrop.
                                     */
    IPV4_MCAST_SA_B0        = 57,   /* (SA[31:28]=14 or SA=255.255.255.255). */
    IPV4_BCAST_DA_B0        = 58,   /* (DA=255.255.255.255). */
    IPV4_DA_ZERO_B0         = 59,   /* IPv4 Source Address is multicast (DA[31:28] = 0x0) 0.0.0.0/8 */
    IPV4_LOOPBACK_B0        = 60,   /* IPv4 SA or DA is loopback (127.0.0.0/8) */
    IPV4_DA_EQ_SA_B0        = 61,   /* IPv4 SA == DA */
    IPV4_NONAT_B0           = 62,   /*
                                     * The IPv4 header's SA and DA are not in the same address space where an address
                                     * space is one of the private address spaces (10.0.0.0/8, 172.16.0.0/12, 192.168.0.0/16)
                                     * or the public address space.
                                     */
    IPV4_PRIVATE_B0         = 63,   /* DA or SA is 10.0.0.0/8, 172.16.0.0/ 12, or 192.168.0.0./16 */
    IPV4_TCP_HDR_REWRITE_B0 = 64,   /* IPv4 Protocol is TCP (0x6) and IPv4 offset is 0x1 or 0x2 */
    IPV4_ICMP_FRAGMENT_B0   = 65,   /* IPv4 protocol is ICMP (0x01) and offset != 0x00 || mf=1 */
    IPV4_TTL_ZERO_B0        = 66,   /* IPv4_TTL is zero */
    IPV4_TTL_ONE_B0         = 67,   /* IPv4_TTL expired for routed traffic */
    IPV6_VERSION_B0         = 68,   /* The Version field must be 6 for frames reported at IPv6 */
    IPV6_TLEN_CHK_B0        = 69,   /*
                                     * The number of bytes remaining in the frame is less than 40,
                                     * then a valid IPv6 packet is not possible.
                                     */
    IPV6_FRAGMENT_B0        = 70,   /* IPv6 packet contains an IPv6 Fragmentation header, then it is considered fragmented. */
    IPV6_SHORT_FRAGMENT_B0  = 71,   /*
                                     *  More Fragments (MF) bit set to one and Payload Length field less than the
                                     *  amount specified in the Short_Frag_Config register. Teardrop.
                                     */
    IPV6_MCAST_SA_B0        = 72,   /* IPv6 SA is multicast (SA[127:120] = 0xFF) */
    IPV6_DA_EQ_SA_B0        = 73,   /* IPv6 SA = DA */
    IPV6_DA_ZERO_B0         = 74,   /* IPv6 DA = ::0 */
    IPV6_LOOPBACK_B0        = 75,   /* IPv6 DA or SA is 0x0000001 */
    IPV6_HOP_LIMIT_ZERO_B0  = 76,   /* IPv6 Next Hop is 0x0 */
    IPV6_HOP_LIMIT_ONE_B0   = 77,   /*
                                     * The IPv6 header's Next Hop is one and the packet is being routed. This would
                                     * lead to a Next Hop decrement on egress  which would result in a Next Hop of zero.
                                     * This condition is recognized by the Deep Frame Parser but the trap isn't
                                     * invoked until the VSI is determined and the VSIT lookup indicates that
                                     * the VSI is an L3 switch.
                                     */
    IPV6_ICMP_FRAGMENT_B0   = 78,   /* ICMPv6 protocol data carried in a fragmented IPV6 datagram. */
    TCP_DP_EQ_SP_B0         = 79,   /*
                                     * DPort_B0 = SPort in a TCP header, in an unfragmented IP datagram or
                                     * the first fragment of a fragmented IP datagram
                                     */
    TCP_NULL_SCAN_B0        = 80,   /*
                                     * Seq_Num=0 and all TCP flags=0 in a TCP header carried an unfragmented
                                     * IP datagram or the first fragment of a fragmented IP datagram
                                     */
    TCP_XMAS_SCAN_B0        = 81,   /*
                                     * Seq_Num=0, FIN=1, URG=1, and PSH=1 in a TCP header carried an unfragmented
                                     * IP datagram or the first fragment of a fragmented IP datagram
                                     */
    TCP_SYN_FIN_SCAN_B0     = 82,   /*
                                     * SYN=1 and FIN=1 in a TCP header carried an unfragmented IP datagram or
                                     * the first fragment of a fragmented IP datagram
                                     */
    TCP_SYNC_B0             = 83,   /*
                                     * SYN=1, ACK=0, and SRC_PORT<1024 in a TCP header carried an unfragmented IP
                                     * datagram or the first fragment of a fragmented IP datagram.
                                     */
    TCP_HDR_LENGTH_B0       = 84,   /*
                                     * The length of a TCP header carried in the first fragment of a fragmented IP
                                     * datagram is less than a configured MIN_TCP_Header_Size.
                                     * (Default set at 20B, which is the minimal length)
                                     */
    TCP_HDR_FRAGMENT_B0     = 85,   /*
                                     * The first IP fragment must be large enough to contain all required TCP header. For
                                     * IP packets with Fragment Offset_B0 = 0, its length must be longer than a set threshold.
                                     */
    UDP_DP_EQ_SP_B0         = 86,   /*
                                     * DPort=SPort in a UDP header, in an unfragmented IP datagram or
                                     * the first fragment of a fragmented IP datagram
                                     */
    ICMP4_LONG_PING_B0      = 87,   /*
                                     * ICMPv4 Ping (Echo Request) PDU is carried in an un-fragmented IPv4 datagram
                                     * whose Total Length has a value that is greater than MAX_ICMPv4_Size +
                                     * size of IPv4 header. Where the MAX_ICMPv4_Size is programmable between
                                     * 0 and 9.6K bytes, inclusive. The default is set to 512.
                                     */
    ICMP6_LONG_PING_B0      = 88,   /*
                                     * ICMPv6 Ping (Echo Request) PDU is carried in an un-fragmented IPv6 datagram whose
                                     * Payload Length has a value greater than MAX_ICMPv6_Size, where the MAX_ICMPv6_Size
                                     * is programmable between 0 and 9.6K bytes, inclusive. The default is set to 512.
                                     */
    IPV4_SMURF_B0           = 89,   /*
                                     * IPv4 frame to broadcast address (255.255.255.255) with protocol=ICMP OR UDP,
                                     * CODE=ECHO OR DPORT=7 {SMURF, FRAGLE}
                                     */
    IPV6_SMURF_B0           = 90,   /*
                                     * IPv6 frame to broadcast link local address FF02:: with protocol=ICMP OR UDP,
                                     * CODE=ECHO OR DPORT=7 {SMURF, FRAGLE}
                                     */
    SNOOP_PTP_UDP_B0        = 91,   /* PTP over UDP. IPV4: DA=224.0.0.107; IPv6: DA=FF02::6B UDP DPORT=319 */
    SNOOP_PTP_ET_B0         = 92,   /* PTP over L2 (802.1AS): Ethertype = 0x88F7 */
    SNOOP_ARP_B0            = 93,   /* IPv4 ARP: DMAC=broadcast, Ethertype = 0x0806 */
    SNOOP_RARP_B0           = 94,   /* IPv4 RARP: Ethertype = 0x8035 */
    SNOOP_DHCP4_B0          = 95,   /*
                                     * IPv4 DHCP Packet: L2 Payload is IPv4, IP Protocol_B0 = 17 (UDP)
                                     * Dest UDP Port_B0 = 67(server) Src UDP Port = 68(client)
                                     */
    SNOOP_ICMP4_B0          = 96,   /* IPv4 ICMP L2 Payload is IPv4 IPv4 Protocol number: 1 (ICMP) */
    SNOOP_IGMP_B0           = 97,   /* IPv4 IGMP L2 Payload is IPv4 IPv4 Protocol number: 2 (IGMP) */
    SNOOP_MLD_B0            = 98,   /* IPv6 MLD L2 Payload is IPv6 IPv6 Next Header is ICMPv6 (58) + Type = 130/131/132 */
    SNOOP_ND_RD_B0          = 99,   /*
                                     * IPv6 Discovery (ND/RD) L2 Payload is IPv6 IPv6 Next Header is
                                     * ICMPv6 (58) + Type_B0 = 133/134/135/136/137
                                     */
    SNOOP_ICMP6_B0          = 100,  /* L2 Payload is IPv6 IPv6 next header is ICMPv6(58) */
    SNOOP_DHCP6_B0          = 101,  /* IPv6 DHCP6 Packet: IPv6 Next Header is UDP, DP=546/547 */
    RESERVED_TRAP4_B0       = 102,  /* Reserved */
    RESERVED_TRAP5_B0       = 103,  /* Reserved */
    RESERVED_TRAP6_B0       = 104,  /* Reserved */
    RESERVED_TRAP7_B0       = 105,  /* Reserved */
    RESERVED_TRAP8_B0       = 106,  /* Reserved */
    RESERVED_TRAP9_B0       = 107,  /* Reserved */
    RESERVED_TRAP10_B0      = 108,  /* Reserved */
    RESERVED_TRAP11_B0      = 109,  /* Reserved */
    RESERVED_TRAP12_B0      = 110,  /* Reserved */
    RESERVED_TRAP13_B0      = 111,  /* Reserved */
    FM_DST_COPY_CPU_B0      = 112,  /* FM destination lookup fwd_ctrl indicates copy to CPU. */
    FM_COPY_CPU_B0          = 113,  /* L2 ARL copy to cpu */
    ULF_DROP_B0             = 114,  /* L2 DMAC Unicast frame lookup failure. */
    MLF_DROP_B0             = 115,  /* L2 DMAC Multicast frame lookup failure. */
    BC_DROP_B0              = 116,  /* L2 DMAC Broadcast frame lookup failure. */
    CA_SA_MOVE_B0           = 117,  /* Trap from cascade device, SA (station) moved SLI */
    SA_MOVE_B0              = 118,  /* SA (station) moved SLI */
    MAC_LEARN_LIMIT_B0      = 119,  /* Port has reached L2 SMAC self-learned limit */
    MAC_SA_FULL_B0          = 120,  /* Hardware learning tried to insert a SMAC and all buckets are full */
    CA_LEARN_FRAME_B0       = 121,  /* Trap from cascade device, Learning Frame was copied to the CPU */
    LEARN_FRAME_B0          = 122,  /* L2 Learning Frame was copied to the CPU */
    NO_LRN_LKP_FAIL_B0      = 123,  /* Source lookup fail for a frame for which learning was not attempted */
    VLAN_UNREG_B0           = 124,  /*
                                     * Indexed VLAN is not a registered VLAN. This can occur based on a LIM miss
                                     * or PV2LI port not in membership.
                                     */
    INV_SLI_B0              = 125,  /* SLI of the packet is invalid (=0) */
    INV_FWD_OP_B0           = 126,  /* A CB-tag was received with an invalid/unsupported FWD_OP code. */
    RESERVED_TRAP14_B0      = 127,  /* Reserved */

} cbx_trap_b0_t;

#endif  /* __EXCEPTION_H */
