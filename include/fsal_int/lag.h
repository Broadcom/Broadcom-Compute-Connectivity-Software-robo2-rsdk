/* *
 * * $ID: $
 * * 
 * * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 * * 
 * * $Copyright: (c) 2020 Broadcom Inc.
 * *
 * * File:
 * *     lag.h
 * * Purpose:
 * *     This file contains definitions internal to LAG module.
 */

#ifndef CBX_INT_LAG_H
#define CBX_INT_LAG_H

#include <fsal_int/types.h>
#include <fsal/lag.h>

/* Value to be same as CBXI_PORT_TABLE_ENTRY_ADD */
#define CBXI_LAG_PORT_TABLE_ENTRY_ADD    0

/* Value to be same as CBXI_PORT_TABLE_ENTRY_REMOVE */
#define CBXI_LAG_PORT_TABLE_ENTRY_REMOVE 1

#define MAX_PG  32

/** LAG LBH Key index */
#define CBX_LAG_LBH_OTHER_KEY_ID     8
#define CBX_LAG_LBH_IPv4_KEY_ID      9
#define CBX_LAG_LBH_IPv6_KEY_ID     10

/** LAG LBH Max Key types */
#define CBX_LAG_LBH_MAX_KEYS         3

#define  CBX_LAG_IKFT_FIELD_ID_F0_HDR_TYPE        255   /* F0 */
#define  CBX_LAG_IKFT_FIELD_ID_F1_HDR_TYPE        255   /* F1 */
#define  CBX_LAG_IKFT_FIELD_ID_SPG_HDR_TYPE       255   /* NT_HWH,F2 */
#define  CBX_LAG_IKFT_FIELD_ID_F3_HDR_TYPE        255   /* F3 */
#define  CBX_LAG_IKFT_FIELD_ID_DSTMAC_HDR_TYPE     31  /* NT_MAC,F4 */
#define  CBX_LAG_IKFT_FIELD_ID_SRCMAC_HDR_TYPE     31  /* NT_MAC,F5 */
#define  CBX_LAG_IKFT_FIELD_ID_OUTER_TCI_HDR_TYPE 255  /* NT_CTAG,F6 */
#define  CBX_LAG_IKFT_FIELD_ID_F7_HDR_TYPE          3  /* NT_STAG,F7 */
#define  CBX_LAG_IKFT_FIELD_ID_F8_HDR_TYPE        255  /* F8 */
#define  CBX_LAG_IKFT_FIELD_ID_SRCIP_HDR_TYPE     255  /* NT_IPV4,F9 */
#define  CBX_LAG_IKFT_FIELD_ID_DSTIP_HDR_TYPE     255  /* NT_IPV4,F10 */
#define  CBX_LAG_IKFT_FIELD_ID_SRCIPV6_HDR_TYPE   255  /* NT_IPV6,F11 */
#define  CBX_LAG_IKFT_FIELD_ID_DSTIPV6_HDR_TYPE   255  /* NT_IPV6,F12 */
#define  CBX_LAG_IKFT_FIELD_ID_F13_HDR_TYPE       255  /* F13 */
#define  CBX_LAG_IKFT_FIELD_ID_F14_HDR_TYPE       255  /* F14 */
#define  CBX_LAG_IKFT_FIELD_ID_L3PROTO_HDR_TYPE   255  /* F15 */
#define  CBX_LAG_IKFT_FIELD_ID_L4BYTES_HDR_TYPE   255  /* F16 */

#define  CBX_LAG_IKFT_FIELD_ID_FO_LEN         32  /* F0 */
#define  CBX_LAG_IKFT_FIELD_ID_F1_LEN          8  /* F1 */
#define  CBX_LAG_IKFT_FIELD_ID_SPG_LEN         5  /* F2 */
#define  CBX_LAG_IKFT_FIELD_ID_F3_LEN          7  /* F3 */
#define  CBX_LAG_IKFT_FIELD_ID_DSTMAC_LEN     48  /* F4 */
#define  CBX_LAG_IKFT_FIELD_ID_SRCMAC_LEN     48  /* F5 */
#define  CBX_LAG_IKFT_FIELD_ID_OUTER_TCI_LEN  12  /* F6 */
#define  CBX_LAG_IKFT_FIELD_ID_F7_LEN         12  /* F7 */
#define  CBX_LAG_IKFT_FIELD_ID_F8_LEN         16  /* F8 */
#define  CBX_LAG_IKFT_FIELD_ID_SRCIP_LEN      64  /* F9 */
#define  CBX_LAG_IKFT_FIELD_ID_DSTIP_LEN      64  /* F10 */
#define  CBX_LAG_IKFT_FIELD_ID_SRCIPV6_LEN    64  /* F11 */
#define  CBX_LAG_IKFT_FIELD_ID_DSTIPV6_LEN    64  /* F12 */
#define  CBX_LAG_IKFT_FIELD_ID_F13_LEN         8  /* F13 */
#define  CBX_LAG_IKFT_FIELD_ID_F14_LEN         8  /* F14 */
#define  CBX_LAG_IKFT_FIELD_ID_L3PROTO_LEN     8  /* F15 */
#define  CBX_LAG_IKFT_FIELD_ID_L4BYTES_LEN    32  /* F16 */

#define  CBX_LAG_IKFT_FIELD_ID_F0_OFFSET         416  /* F0 */
#define  CBX_LAG_IKFT_FIELD_ID_F1_OFFSET         408  /* F1 */
#define  CBX_LAG_IKFT_FIELD_ID_SPG_OFFSET        387 /* F2 */
#define  CBX_LAG_IKFT_FIELD_ID_F3_OFFSET         448  /* F3 */
#define  CBX_LAG_IKFT_FIELD_ID_DSTMAC_OFFSET       0  /* F4 */
#define  CBX_LAG_IKFT_FIELD_ID_SRCMAC_OFFSET      48  /* F5 */
#define  CBX_LAG_IKFT_FIELD_ID_OUTER_TCI_OFFSET  372  /* F6 */
#define  CBX_LAG_IKFT_FIELD_ID_F7_OFFSET          20  /* F7 */
#define  CBX_LAG_IKFT_FIELD_ID_F8_OFFSET         352  /* F8 */
#define  CBX_LAG_IKFT_FIELD_ID_SRCIP_OFFSET       64  /* F9 */
#define  CBX_LAG_IKFT_FIELD_ID_DSTIP_OFFSET      192  /* F10 */
#define  CBX_LAG_IKFT_FIELD_ID_SRCIPV6_OFFSET    128  /* F11 */
#define  CBX_LAG_IKFT_FIELD_ID_DSTIPV6_OFFSET    256  /* F12 */
#define  CBX_LAG_IKFT_FIELD_ID_F13_OFFSET          4  /* F13 */
#define  CBX_LAG_IKFT_FIELD_ID_F14_OFFSET         56  /* F14 */
#define  CBX_LAG_IKFT_FIELD_ID_L3PROTO_OFFSET     48  /* F15 */
#define  CBX_LAG_IKFT_FIELD_ID_L4BYTES_OFFSET    320  /* F16 */

/** IKFT fields  */
#define  CBX_LAG_IKFT_FIELD_ID_SPG         2  /**< LAG Hash based on source port group */
#define  CBX_LAG_IKFT_FIELD_ID_DSTMAC      4  /**< LAG Hash based on destination MAC address */
#define  CBX_LAG_IKFT_FIELD_ID_SRCMAC      5  /**< LAG Hash based on source MAC address */
#define  CBX_LAG_IKFT_FIELD_ID_OUTER_TCI   6  /**< LAG Hash based on outer TCI*/
#define  CBX_LAG_IKFT_FIELD_ID_SRCIP       9  /**< LAG Hash based on source IP address */
#define  CBX_LAG_IKFT_FIELD_ID_DSTIP      10  /**< LAG Hash based on destination IP address */
#define  CBX_LAG_IKFT_FIELD_ID_SRCIPV6    11  /**< LAG Hash based on source IPV6 address */
#define  CBX_LAG_IKFT_FIELD_ID_DSTIPV6    12  /**< LAG Hash based on destination IPV6 address */
#define  CBX_LAG_IKFT_FIELD_ID_L3PROTO    15  /**< LAG Hash based on L3 protocol field */
#define  CBX_LAG_IKFT_FIELD_ID_L4BYTES    16  /**< LAG Hash based on L4 bytes DST/SRC Port field */

#define CBX_PACKET_OTHER_PSC_PBMP         ( CBX_LAG_PSC_SPG |\
                                            CBX_LAG_PSC_VLAN |\
                                            CBX_LAG_PSC_DSTMAC |\
                                            CBX_LAG_PSC_SRCMAC )

#define CBX_PACKET_IPV4_PSC_PBMP          ( CBX_LAG_PSC_SPG |\
                                            CBX_LAG_PSC_SRCIP |\
                                            CBX_LAG_PSC_L3PROTO |\
                                            CBX_LAG_PSC_L4BYTES |\
                                            CBX_LAG_PSC_DSTIP )

#define CBX_PACKET_IPV6_PSC_PBMP          ( CBX_LAG_PSC_SPG |\
                                            CBX_LAG_PSC_SRCIP |\
                                            CBX_LAG_PSC_DSTIP |\
                                            CBX_LAG_PSC_L3PROTO |\
                                            CBX_LAG_PSC_L4BYTES |\
                                            CBX_LAG_PSC_SRCIPV6 |\
                                            CBX_LAG_PSC_DSTIPV6 )
typedef struct cbx_lag_info_s {
    int                in_use;/* LAG entry use/free flag */
    int                active[CBX_MAX_UNITS];/* LAG active members flag */
    cbx_port_t         port_one;
    cbx_portid_t       lagid;/* LAG entry ID */
    uint32_t           lpgid;/* Local Avenger Local Port group ID */
    uint32_t           rlpgid;/* Remote Avenger Local Port group ID */
    uint32_t           gpgid;/* Cascaded mode Global Port group ID */
    uint32_t           lgpgid;/* Cascaded mode Global Port group ID */
    uint32_t           rgpgid;/* Cascaded mode Global Port group ID */
    cbx_lag_params_t   lag_params;/* LAG Parameters */
    pbmp_t             lag_pbmp[CBX_MAX_UNITS];/* LAG member port bit map for master/slave Avenger */
} cbx_lag_info_t;

typedef struct cbx_lag_cntl_s {
    int                ngroups;/* number of LAG groups */
    int                nports;/* port count per LAG group */
    cbx_lag_psc_t      psc[CBX_LAG_LBH_MAX_KEYS]; /* port spec criterion */
    cbx_lag_info_t     *lag_info;
    pbmp_t             pscfld_pbmp[CBX_LAG_LBH_MAX_KEYS];/* IKFT fields bit map for each PSC */
} cbx_lag_cntl_t;

int cbx_lag_init(void);
int cbxi_robo2_pp2lpg_slot_get( int unit, cbx_port_t *lpgid );
int cbxi_robo2_lpg2pg_slot_get( int unit, cbx_port_t *gpgid );
int cbxi_robo2_ikft_update( int unit, int fld_index, int key_id );
int cbxi_robo2_lpg2pg_update( int unit, int lpgid, int gpgid );
int cbxi_robo2_pg2lpg_update( int unit, int gpgid, int lpgid );
int cbxi_robo2_lag_svt_update(int unit, pbmp_t lpbmp, cbx_port_t port_in, uint32_t flags );
int cbxi_robo2_lag_get( cbx_portid_t lagid, cbx_lag_info_t *lag_info );
int cbxi_robo2_lag_member_check( cbx_portid_t portid, cbx_portid_t *lagid );
int cbxi_lag_pg_context_clear(int unit, cbx_port_t port_out, cbx_port_t lpgid);
int _shr_popcount(unsigned int n);
int cbxi_robo2_lag_link_status_get( cbx_portid_t lagid, uint32_t *link_sts);

#ifdef CONFIG_CASCADED_MODE
extern int
cbxi_robo2_lag_link_status_update( cbx_port_t gport,
                                   int  link_status,
                                   cbx_portid_t lagid );
#endif /* CONFIG_CASCADED_MODE */

#endif /* !CBX_INT_LAG_H  */
