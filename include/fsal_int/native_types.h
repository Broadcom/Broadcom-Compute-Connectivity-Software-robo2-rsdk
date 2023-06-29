/* *
 * * $ID: $
 * * $Copyright: (c) 2017 Broadcom Limited.
 * *
 * * File:
 * *     native_types.h
 * * Purpose:
 * *     This file contains internal Native Type Defs.
 */

#ifndef __NATIVE_TYPES_H
#define __NATIVE_TYPES_H

/**
 * Native Types
 * Use the STT to modify/override the etype that identifies the Native type
 */
#define CBXI_NT_NOP              0    /* NOP header */
#define CBXI_NT_TAG_INVALID      1    /* Indicates an invalid tag. Should not appear on the parse stack. */
#define CBXI_NT_ING_TIMESTAMP    2    /* Ingress Timestamp header */
#define CBXI_NT_CTAG             3    /* Q-in-Q customer tag C-TAG (matches ctpid0 or ctpid1). */
#define CBXI_NT_MIRROR           4    /* Mirror Header */
#define CBXI_NT_STAG             5    /* Q-in-Q provider tag S-TAG (matches stpid0 or stpid1). */
#define CBXI_NT_ETYPE            7    /* Ethertype header (2B) */
#define CBXI_NT_ETAG             9    /* Port Extender E-TAG */
#define CBXI_NT_ITAG            11    /* MAC-in-MAC I-TAG */
#define CBXI_NT_EGR_TIMESTAMP   12    /* Egress Timestamp header */
#define CBXI_NT_LLC             13    /* LLC Header (less than llc_config.limit). */
#define CBXI_NT_SNAP            15    /* LLC/SNAP Header (AA-AA-03-00-00-00) */
#define CBXI_NT_COE4            17    /* 4B Broadcom Channel over Ethernet Tag. */
#define CBXI_NT_COE8            19    /* 8B Broadcom Channel over Ethernet Tag. */
#define CBXI_NT_SW2CP           21    /* CB (CP/cascade) tag (SW2CP format) */
#define CBXI_NT_CP2SW           23    /* CB (CP/cascade) tag (CP2SW format) */
#define CBXI_NT_BRCM            25    /* Legacy Robo BRCM header */
#define CBXI_NT_IMP0            27    /* IMP Header – CB_TAG */
#define CBXI_NT_IMP1            29    /* IMP Header – CB_TAG with timestamp */
#define CBXI_NT_MAC             31    /* MAC header. DMAC and SMAC of Ethernet Frame */
#define CBXI_NT_UTAG0           33    /* Available for assignment as a soft tag. */
#define CBXI_NT_UTAG1           35    /* Available for assignment as a soft tag. */
#define CBXI_NT_UTAG2           37    /* Available for assignment as a soft tag. */
#define CBXI_NT_UTAG3           39    /* Available for assignment as a soft tag. */
#define CBXI_NT_UTAG4           41    /* Available for assignment as a soft tag. */
#define CBXI_NT_UTAG5           43    /* Available for assignment as a soft tag. */
#define CBXI_NT_PAYLOAD         45    /* End of header stack (payload) */
#define CBXI_NT_IPV4            47    /* IPv4 Header */
#define CBXI_NT_IPV6            49    /* IPv6 Header */
#define CBXI_NT_UCMPLS          51    /* Unicast MPLS */
#define CBXI_NT_MCMPLS          53    /* Multicast MPLS */
#define CBXI_NT_PPPOE           55    /* PPPoE - PPP over Ethernet */
#define CBXI_NT_UDP             67    /* UDP: IPv4 or IPv6 Proto equals 17 */
#define CBXI_NT_TCP             69    /* TCP: IPv4 or IPv6 Proto equals 6 */
#define CBXI_NT_ICMP            71    /* ICMP: IPv4 or IPv6 Proto equals 1 */
#define CBXI_NT_IGMP            73    /* IGMP: IPv4 or IPv6 Proto equals 2 */
#define CBXI_NT_PTP             75    /* Precision Time Protocol */
#define CBXI_NT_ARP             77    /* Address Resolution Protocol */
#define CBXI_NT_BLOB2           81    /* Blob header type, 2 bytes in length */
#define CBXI_NT_BLOB4           83    /* Blob header type, 4 bytes in length */
#define CBXI_NT_BLOB6           85    /* Blob header type, 6 bytes in length */
#define CBXI_NT_BLOB8           87    /* Blob header type, 8 bytes in length */
#define CBXI_NT_BLOB10          89    /* Blob header type, 10 bytes in length */
#define CBXI_NT_BLOB12          91    /* Blob header type, 12 bytes in length */
#define CBXI_NT_BLOB14          93    /* Blob header type, 14 bytes in length */
#define CBXI_NT_BLOB16          95    /* Blob header type, 16 bytes in length */
#define CBXI_NT_BLOB18          97    /* Blob header type, 18 bytes in length */
#define CBXI_NT_BLOB20          99    /* Blob header type, 20 bytes in length */
#define CBXI_NT_DIAG            101    /* A 64B diag tag with ether type populated from config register */
#define CBXI_NT_IPV6_EH         103    /* IPv6 extension header of variable length */
#define CBXI_NT_IPV6_FRAG_EH    105    /* IPv6 fragment extension header, 8B in length */
#define CBXI_NT_IPV6_ICMP       106    /* IPv6 Internet Control Message. */

#endif /* __NATIVE_TYPES_H */
