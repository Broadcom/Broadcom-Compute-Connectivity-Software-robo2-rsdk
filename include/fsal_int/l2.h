/* *
 * * $ID: $
 * * $Copyright: (c) 2017 Broadcom Limited.
 * *
 * * File:
 * *     l2.h
 * * Purpose:
 * *     This file contains definitions internal to L2 module.
 */

#ifndef CBX_INT_L2_H
#define CBX_INT_L2_H

#include <shared/types.h>
#include <fsal_int/types.h>
#include <sal_alloc.h>

/* uint8_t : eth_mac_t[6]
 * uint16  : mac_hi_key ; uint32 : mac_lo_key */

/* eth_mac_t mac;  Just generate a list of the macs for display */
#define CBX_MAC_ADDR_LIST(mac) \
    mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]

/* eth_mac_t m1, m2;  True if equal */
#define CBX_MAC_ADDR_EQUAL(m1, m2) (!sal_memcmp(m1, m2, sizeof(eth_mac_t)))

/* eth_mac_t m1, m2;  like memcmp, returns -1, 0, or 1 */
#define CBX_MAC_ADDR_CMP(m1, m2) sal_memcmp(m1, m2, sizeof(eth_mac_t))

#define CBX_MACADDR_STR_LEN     18              /* Formatted MAC address */
#define CBX_IPADDR_STR_LEN      16              /* Formatted IP address */


/* Convert eth_mac_t type mac addr to uint32 [] array
 * (dst)[0] is mac_lo ; (dst)[1] is mac_hi*/
#define CBX_MAC_ADDR_TO_UINT32(mac, dst) do {\
        (dst)[0] = (((uint32)(mac)[2]) << 24 | \
                  ((uint32)(mac)[3]) << 16 | \
                  ((uint32)(mac)[4]) << 8 | \
                  ((uint32)(mac)[5])); \
        (dst)[1] = (((uint32)(mac)[0]) << 8 | \
                  ((uint32)(mac)[1])); \
    } while (0)

/* Convert eth_mac_t type mac addr to uint32 [] array
 * (dst)[0] is uint16 mac_lo ; (dst)[1] is uint32 mac_hi*/
#define CBX_MAC_ADDR_TO_HI_UINT32(mac, dst) do {\
        (dst)[0] = (((uint32)(mac)[4]) << 8 | \
                  ((uint32)(mac)[5]));\
        (dst)[1] = (((uint32)(mac)[0]) << 24 | \
                  ((uint32)(mac)[1]) << 16 | \
                  ((uint32)(mac)[2]) << 8 | \
                  ((uint32)(mac)[3])); \
    } while (0)
/* Convert addr src of type uint32 [] array to eth_mac_t type */
#define CBX_MAC_ADDR_FROM_UINT32(mac, src) do {\
        (mac)[0] = (uint8) ((src)[1] >> 8 & 0xff); \
        (mac)[1] = (uint8) ((src)[1] & 0xff); \
        (mac)[2] = (uint8) ((src)[0] >> 24 & 0xff); \
        (mac)[3] = (uint8) ((src)[0] >> 16 & 0xff); \
        (mac)[4] = (uint8) ((src)[0] >> 8 & 0xff); \
        (mac)[5] = (uint8) ((src)[0] & 0xff); \
    } while (0)

#define CHECK_L2_INIT(cl2i)       \
        if (!cl2i.init)             \
        return CBX_E_INIT

/* Internal: ARLFM mask flags */
#define CBXI_L2_MAC_MASK                 0x1
#define CBXI_L2_VLAN_MASK                0x2
#define CBXI_L2_DEST_MASK                0x4
#define CBXI_L2_UPD_CTRL_MASK            0x8
#define CBXI_L2_FWD_CTRL_MASK            0x10
#define CBXI_L2_VALID_MASK               0x20
#define CBXI_L2_VALID_FIELD_MASK         0x40
#define CBXI_L2_HIT_MASK                 0x80
#define CBXI_L2_MASK_ALL (CBXI_L2_MAC_MASK | CBXI_L2_VLAN_MASK | \
                        CBXI_L2_DEST_MASK | CBXI_L2_UPD_CTRL_MASK | \
                        CBXI_L2_FWD_CTRL_MASK | CBXI_L2_VALID_MASK | \
                        CBXI_L2_VALID_FIELD_MASK)

#define ARL_MAC_LO_MASK                 0xFFFFFFFF
#define ARL_MAC_HI_MASK                 0xFFFF
#define ARL_FID_MASK                    0xFFF
#define ARL_DST_MASK                    0x7FF
#define ARL_DST_TYPE_MASK               0x3
#define ARL_UPD_CTRL_MASK               0x3
#define ARL_FWD_CTRL_MASK               0x3
#define ARL_VALID_MASK                  0x2
#define ARL_VALID_FIELD_MASK            0x3
#define ARL_HIT_MASK                    0x1


/* Aging defaults */
#define CBXI_L2_ARL_AGE_TIME_DEFAULT    300             /* 300 seconds */
#define CBXI_L2_ARL_NUM_OF_ENTRIES_16K  0x4000          /* 16K entries */
#define CBXI_L2_ARL_NUM_OF_ENTRIES_8K   0x2000          /* 8K entries */
/* Unit of age timer period field is microsecond*/
#define CBXI_SEC_TO_MICROSEC            1000000
/* No. of bits in CB_ITM_arl_aging_config.period field */
#define CBXI_AGING_PERIOD_BITS          26

/* ARL entry update flags */
#define CBXI_ARL_ENTRY_ADD              0x1
#define CBXI_ARL_ENTRY_REMOVE           0x2

/* ARLFM enums */
typedef enum cbxi_l2_fm_valid_e {
    cbxL2EntryEmpty,
    cbxL2EntryPending,
    cbxL2EntryValid,
    cbxL2EntryStatic,
} cbxi_l2_fm_valid_t;

typedef enum cbxi_l2_fm_dst_type_e {
    cbxL2DstInvalid,
    cbxL2DstMulticast,
    cbxL2DstLIN,
    cbxL2DstDPG,
} cbxi_l2_fm_dst_type_t;

/* L2 SW info structure */
typedef struct cbx_l2_info_s {
    uint8_t     init;       /* TRUE if l2 module has been inited */
    uint32_t    num_entries;/* No. of ARL entries in L2. default is 16K */
} cbx_l2_info_t;

extern int cbxi_l2_init(void);
extern int cbxi_robo2_portid_resolve(cbx_portid_t portid, cbxi_pgid_t *gpg);

#endif
