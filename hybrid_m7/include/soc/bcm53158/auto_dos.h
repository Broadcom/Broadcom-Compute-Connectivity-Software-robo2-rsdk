/*
 * $Copyright: (c) 2017 Broadcom Limited
 */

/*
 * auto_dos.h 
 */

#ifndef _AUTO_DOS_H_
#define _AUTO_DOS_H_

#define DOS_WDATA_EN                    0x01100000 /* term = 0x1; drop = 0x1 */

/* DOS type flags */
#define MAC_LAND_EN             0x0001
#define IP_DA_EQ_SA_EN          0x0002
#define TCP_DP_EQ_SP_EN         0x0004
#define UDP_DP_EQ_SP_EN         0x0008
#define TCP_NULL_SCAN_EN        0x0010
#define TCP_XMAS_SCAN_EN        0x0020
#define TCP_SYN_FIN_SCAN_EN     0x0040
#define TCP_SYNC_EN             0x0080
#define TCP_HDR_FRAGMENT_EN     0x0100
#define IPV4_ICMP_FRAGMENT_EN   0x0200
#define IPV6_ICMP_FRAGMENT_EN   0x0400
#define ICMP4_LONG_PING_EN      0x0800
#define ICMP6_LONG_PING_EN      0x1000
#define AUTO_DOS_EN_ALL         0x1FFF

#ifndef BCM53158_B0
typedef enum soc_robo2_trapid_e {
    NO_TRAP                     = 0,
    MAC_ERROR                   = 1,
    BMU_RES_DROP                = 2,
    TLV_INV_LEN                 = 3,
    TLV_INV_TYPE                = 4,
    TLV_MAX_NUM                 = 5,
    TLV_MAX_LEN                 = 6,
    RESERVED0                   = 7,
    RESERVED1                   = 8,
    ETH_BCAST_DMAC              = 9,
    ETH_MCAST_SMAC              = 10,
    ETH_MAC_LAND                = 11,
    ETH_VID_FFF                 = 12,
    ETH_ZOMBIE_LEN              = 13,
    RESERVED2                   = 14,
    RESERVED3                   = 15,
    RP_TERMINATE                = 16,
    SLIC_GROUP1                 = 17,
    SLIC_GROUP2                 = 18,
    SLIC_GROUP3                 = 19,
    SLIC_GROUP4                 = 20,
    SLIC_GROUP5                 = 21,
    SLIC_GROUP6                 = 22,
    SLIC_GROUP7                 = 23,
    SLIC_GROUP8                 = 24,
    SLIC_GROUP9                 = 25,
    SLIC_GROUP10                = 26,
    SLIC_GROUP11                = 27,
    SLIC_GROUP12                = 28,
    SLIC_GROUP13                = 29,
    SLIC_GROUP14                = 30,
    SLIC_GROUP15                = 31,
    BMU_METER_DROP              = 32,
    CFP_GROUP1                  = 33,
    CFP_GROUP2                  = 34,
    CFP_GROUP3                  = 35,
    CFP_GROUP4                  = 36,
    CFP_GROUP5                  = 37,
    CFP_GROUP6                  = 38,
    CFP_GROUP7                  = 39,
    CFP_GROUP8                  = 40,
    CFP_GROUP9                  = 41,
    CFP_GROUP10                 = 42,
    CFP_GROUP11                 = 43,
    CFP_GROUP12                 = 44,
    CFP_GROUP13                 = 45,
    CFP_GROUP14                 = 46,
    CFP_GROUP15                 = 47,
    MPLS_TTL_ONE                = 48,
    IPV4_HDR_CHECKSUM           = 49,
    IPV4_HDR_VERSION            = 50,
    IPV4_HDR_LENGTH             = 51,
    IPV4_MIN_LENGTH             = 52,
    IPV4_MAX_LENGTH             = 53,
    IPV4_HDR_OPTIONS            = 54,
    IPV4_FRAGMENT               = 55,
    IPV4_SHORT_FRAGMENT         = 56,
    IPV4_MCAST_SA               = 57,
    IPV4_BCAST_DA               = 58,
    IPV4_DA_ZERO                = 59,
    IPV4_LOOPBACK               = 60,
    IPV4_DA_EQ_SA               = 61,
    IPV4_NONAT                  = 62,
    IPV4_PRIVATE                = 63,
    IPV4_TCP_HDR_REWRITE        = 64,
    IPV4_ICMP_FRAGMENT          = 65,
    IPV4_TTL_ZERO               = 66,
    IPV4_TTL_ONE                = 67,
    IPV6_VERSION                = 68,
    IPV6_TLEN_CHK               = 69,
    IPV6_FRAGMENT               = 70,
    IPV6_SHORT_FRAGMENT         = 71,
    IPV6_MCAST_SA               = 72,
    IPV6_DA_EQ_SA               = 73,
    IPV6_DA_ZERO                = 74,
    IPV6_LOOPBACK               = 75,
    IPV6_HOP_LIMIT_ZERO         = 76,
    IPV6_HOP_LIMIT_ONE          = 77,
    IPV6_ICMP_FRAGMENT          = 78,
    TCP_DP_EQ_SP                = 79,
    TCP_NULL_SCAN               = 80,
    TCP_XMAS_SCAN               = 81,
    TCP_SYN_FIN_SCAN            = 82,
    TCP_SYNC                    = 83,
    TCP_HDR_LENGTH              = 84,
    TCP_HDR_FRAGMENT            = 85,
    UDP_DP_EQ_SP                = 86,
    ICMP4_LONG_PING             = 87,
    ICMP6_LONG_PING             = 88,
    IPV4_SMURF                  = 89,
    IPV6_SMURF                  = 90,
    SNOOP_PTP_UDP               = 91,
    SNOOP_PTP_ET                = 92,
    SNOOP_ARP                   = 93,
    SNOOP_RARP                  = 94,
    SNOOP_DHCP4                 = 95,
    SNOOP_ICMP4                 = 96,
    SNOOP_IGMP                  = 97,
    SNOOP_MLD                   = 98,
    SNOOP_ND_RD                 = 99,
    SNOOP_ICMP6                 = 100,
    SNOOP_DHCP6                 = 101,
    FM_DST_COPY_CPU             = 112,
    FM_COPY_CPU                 = 113,
    ULF_DROP                    = 114,
    MLF_DROP                    = 115,
    BC_DROP                     = 116,
    CA_SA_MOVE                  = 117,
    SA_MOVE                     = 118,
    MAC_LEARN_LIMIT             = 119,
    MAC_SA_FULL                 = 120,
    CA_LEARN_FRAME              = 121,
    LEARN_FRAME                 = 122,
    NO_LRN_LKP_FAIL             = 123,
    VLAN_UNREG                  = 124,
    INV_SLI                     = 125,
} soc_robo2_trapid_t;
#else
typedef enum soc_robo2_trapid_e {
    NO_TRAP              = 0,    /* No Error, Trap, or Filter match */
    CFP_GROUP1           = 1,   /* CFP Trap Group1 */
    CFP_GROUP2           = 2,   /* CFP Trap Group2 */
    CFP_GROUP3           = 3,   /* CFP Trap Group3 */
    CFP_GROUP4           = 4,   /* CFP Trap Group4 */
    CFP_GROUP5           = 5,   /* CFP Trap Group5 */
    CFP_GROUP6           = 6,   /* CFP Trap Group6 */
    CFP_GROUP7           = 7,   /* CFP Trap Group7 */
    CFP_GROUP8           = 8,   /* CFP Trap Group8 */
    CFP_GROUP9           = 9,   /* CFP Trap Group9 */
    CFP_GROUP10          = 10,   /* CFP Trap Group10 */
    CFP_GROUP11          = 11,   /* CFP Trap Group11 */
    CFP_GROUP12          = 12,   /* CFP Trap Group12 */
    CFP_GROUP13          = 13,   /* CFP Trap Group13 */
    CFP_GROUP14          = 14,   /* CFP Trap Group14 */
    CFP_GROUP15          = 15,   /* CFP Trap Group15 */
    MAC_ERROR            = 16,    /* Error indication from MAC. This encompasses a number of errors from the MAC. */
    BMU_RES_DROP         = 17,    /* BMU is out of resource */
    TLV_INV_LEN          = 18,    /* TLV received with invalid length (=0,1 or great than MAX TLV) */
    TLV_INV_TYPE         = 19,    /* TLV received with invalid type */
    TLV_MAX_NUM          = 20,    /* Maximum number of TLVs were processed without LAST set */
    TLV_MAX_LEN          = 21,    /* Maximum length of all TLV was exceed. */
    RESERVED0            = 22,    /* Reserved */
    RESERVED1            = 23,    /* Reserved */
    ETH_BCAST_DMAC       = 24,    /* Broadcast MAC Address (DA=ff-ff-ff-ff-ff-ff) */
    ETH_MCAST_SMAC       = 25,   /* Multicast MAC Address */
    ETH_MAC_LAND         = 26,   /* MAC DA = MAC SA */
    ETH_VID_FFF          = 27,   /* VID is 0xFFF in a C_TAG or S_TAG frame */
    ETH_ZOMBIE_LEN       = 28,   /* A length/Etype field was between 1500 and 1536 */
    RESERVED2            = 29,   /* Reserved */
    RESERVED3            = 30,   /* Reserved */
    RP_TERMINATE         = 31,   /*
                                  * Indicates a terminal trap condition occurred in the Ingress Receive Processor.
                                  * If there is no metadata to communicate the trap condition then this is the trap
                                  * that is signaled. If there is metadata then the precise trap condition will be signaled.
                                  */
    SLIC_GROUP1          = 32,   /* SLIC Trap Group1 */
    SLIC_GROUP2          = 33,   /* SLIC Trap Group2 */
    SLIC_GROUP3          = 34,   /* SLIC Trap Group3 */
    SLIC_GROUP4          = 35,   /* SLIC Trap Group4 */
    SLIC_GROUP5          = 36,   /* SLIC Trap Group5 */
    SLIC_GROUP6          = 37,   /* SLIC Trap Group6 */
    SLIC_GROUP7          = 38,   /* SLIC Trap Group7 */
    SLIC_GROUP8          = 39,   /* SLIC Trap Group8 */
    SLIC_GROUP9          = 40,   /* SLIC Trap Group9 */
    SLIC_GROUP10         = 41,   /* SLIC Trap Group10 */
    SLIC_GROUP11         = 42,   /* SLIC Trap Group11 */
    SLIC_GROUP12         = 43,   /* SLIC Trap Group12 */
    SLIC_GROUP13         = 44,   /* SLIC Trap Group13 */
    SLIC_GROUP14         = 45,   /* SLIC Trap Group14 */
    SLIC_GROUP15         = 46,   /* SLIC Trap Group15 */
    BMU_METER_DROP       = 47,   /* BMU Meter Drop */
    MPLS_TTL_ONE         = 48,   /*
                                  * The MPLS header's TTL is one and the packet is being routed
                                  * (FPSLICT dst_key field is FID_MPLS). This would lead to a TTL decrement on
                                  * egress which would result in a TTL of zero.
                                  */
    IPV4_HDR_CHECKSUM    = 49,   /* IPv4 Header Checksum Failure([RFC1812 5.5.2 (2)] */
    IPV4_HDR_VERSION     = 50,   /* IPv4 Version failure ([RFC1812 5.5.2 (3)] */
    IPV4_HDR_LENGTH      = 51,   /* The IHL field is must be greater than or equal to 5 ([RFC1812 5.5.2 (4)] */
    IPV4_MIN_LENGTH      = 52,   /*
                                  * If the number of bytes from the L2 payload must be greater than 4*IHL Bytes.
                                  * [RFC1812 5.5.2. (1)]
                                  */
    IPV4_MAX_LENGTH      = 53,   /*
                                  * The Total Length from the IPv4 header is compared to the length of the entire packet.
                                  * ([RFC1812 5.5.2 (5)]  Not available for cut-through packets
                                  */
    IPV4_HDR_OPTIONS     = 54,   /* The IP Header Length (IHL) is compared to 5 to determine if options are present. */
    IPV4_FRAGMENT        = 55,   /* Fragmented Packet (ip.MF=1 or ip.offset =! 0) */
    IPV4_SHORT_FRAGMENT  = 56,   /*
                                  * (MF) bit set to one and has a Total Length field less than the amount specified
                                  * in the Short_Frag_Config register. AKA Teardrop.
                                  */
    IPV4_MCAST_SA        = 57,   /* (SA[31:28]=14 or SA=255.255.255.255). */
    IPV4_BCAST_DA        = 58,   /* (DA=255.255.255.255). */
    IPV4_DA_ZERO         = 59,   /* IPv4 Source Address is multicast (DA[31:28] = 0x0) 0.0.0.0/8 */
    IPV4_LOOPBACK        = 60,   /* IPv4 SA or DA is loopback (127.0.0.0/8) */
    IPV4_DA_EQ_SA        = 61,   /* IPv4 SA == DA */
    IPV4_NONAT           = 62,   /*
                                  * The IPv4 header's SA and DA are not in the same address space where an address
                                  * space is one of the private address spaces (10.0.0.0/8, 172.16.0.0/12, 192.168.0.0/16)
                                  * or the public address space.
                                  */
    IPV4_PRIVATE         = 63,   /* DA or SA is 10.0.0.0/8, 172.16.0.0/ 12, or 192.168.0.0./16 */
    IPV4_TCP_HDR_REWRITE = 64,   /* IPv4 Protocol is TCP (0x6) and IPv4 offset is 0x1 or 0x2 */
    IPV4_ICMP_FRAGMENT   = 65,   /* IPv4 protocol is ICMP (0x01) and offset != 0x00 || mf=1 */
    IPV4_TTL_ZERO        = 66,   /* IPv4_TTL is zero */
    IPV4_TTL_ONE         = 67,   /* IPv4_TTL expired for routed traffic */
    IPV6_VERSION         = 68,   /* The Version field must be 6 for frames reported at IPv6 */
    IPV6_TLEN_CHK        = 69,   /*
                                  * The number of bytes remaining in the frame is less than 40,
                                  * then a valid IPv6 packet is not possible.
                                  */
    IPV6_FRAGMENT        = 70,   /* IPv6 packet contains an IPv6 Fragmentation header, then it is considered fragmented. */
    IPV6_SHORT_FRAGMENT  = 71,   /*
                                  *  More Fragments (MF) bit set to one and Payload Length field less than the
                                  *  amount specified in the Short_Frag_Config register. Teardrop.
                                  */
    IPV6_MCAST_SA        = 72,   /* IPv6 SA is multicast (SA[127:120] = 0xFF) */
    IPV6_DA_EQ_SA        = 73,   /* IPv6 SA = DA */
    IPV6_DA_ZERO         = 74,   /* IPv6 DA = ::0 */
    IPV6_LOOPBACK        = 75,   /* IPv6 DA or SA is 0x0000001 */
    IPV6_HOP_LIMIT_ZERO  = 76,   /* IPv6 Next Hop is 0x0 */
    IPV6_HOP_LIMIT_ONE   = 77,   /*
                                  * The IPv6 header's Next Hop is one and the packet is being routed. This would
                                  * lead to a Next Hop decrement on egress  which would result in a Next Hop of zero.
                                  * This condition is recognized by the Deep Frame Parser but the trap isn't
                                  * invoked until the VSI is determined and the VSIT lookup indicates that
                                  * the VSI is an L3 switch.
                                  */
    IPV6_ICMP_FRAGMENT   = 78,   /* ICMPv6 protocol data carried in a fragmented IPV6 datagram. */
    TCP_DP_EQ_SP         = 79,   /*
                                  * DPort = SPort in a TCP header, in an unfragmented IP datagram or
                                  * the first fragment of a fragmented IP datagram
                                  */
    TCP_NULL_SCAN        = 80,   /*
                                  * Seq_Num=0 and all TCP flags=0 in a TCP header carried an unfragmented
                                  * IP datagram or the first fragment of a fragmented IP datagram
                                  */
    TCP_XMAS_SCAN        = 81,   /*
                                  * Seq_Num=0, FIN=1, URG=1, and PSH=1 in a TCP header carried an unfragmented
                                  * IP datagram or the first fragment of a fragmented IP datagram
                                  */
    TCP_SYN_FIN_SCAN     = 82,   /*
                                  * SYN=1 and FIN=1 in a TCP header carried an unfragmented IP datagram or
                                  * the first fragment of a fragmented IP datagram
                                  */
    TCP_SYNC             = 83,   /*
                                  * SYN=1, ACK=0, and SRC_PORT<1024 in a TCP header carried an unfragmented IP
                                  * datagram or the first fragment of a fragmented IP datagram.
                                  */
    TCP_HDR_LENGTH       = 84,   /*
                                  * The length of a TCP header carried in the first fragment of a fragmented IP
                                  * datagram is less than a configured MIN_TCP_Header_Size.
                                  * (Default set at 20B, which is the minimal length)
                                  */
    TCP_HDR_FRAGMENT     = 85,   /*
                                  * The first IP fragment must be large enough to contain all required TCP header. For
                                  * IP packets with Fragment Offset = 0, its length must be longer than a set threshold.
                                  */
    UDP_DP_EQ_SP         = 86,   /*
                                  * DPort=SPort in a UDP header, in an unfragmented IP datagram or
                                  * the first fragment of a fragmented IP datagram
                                  */
    ICMP4_LONG_PING      = 87,   /*
                                  * ICMPv4 Ping (Echo Request) PDU is carried in an un-fragmented IPv4 datagram
                                  * whose Total Length has a value that is greater than MAX_ICMPv4_Size +
                                  * size of IPv4 header. Where the MAX_ICMPv4_Size is programmable between
                                  * 0 and 9.6K bytes, inclusive. The default is set to 512.
                                  */
    ICMP6_LONG_PING      = 88,   /*
                                  * ICMPv6 Ping (Echo Request) PDU is carried in an un-fragmented IPv6 datagram whose
                                  * Payload Length has a value greater than MAX_ICMPv6_Size, where the MAX_ICMPv6_Size
                                  * is programmable between 0 and 9.6K bytes, inclusive. The default is set to 512.
                                  */
    IPV4_SMURF           = 89,   /*
                                  * IPv4 frame to broadcast address (255.255.255.255) with protocol=ICMP OR UDP,
                                  * CODE=ECHO OR DPORT=7 {SMURF, FRAGLE}
                                  */
    IPV6_SMURF           = 90,   /*
                                  * IPv6 frame to broadcast link local address FF02:: with protocol=ICMP OR UDP,
                                  * CODE=ECHO OR DPORT=7 {SMURF, FRAGLE}
                                  */
    SNOOP_PTP_UDP        = 91,   /* PTP over UDP. IPV4: DA=224.0.0.107; IPv6: DA=FF02::6B UDP DPORT=319 */
    SNOOP_PTP_ET         = 92,   /* PTP over L2 (802.1AS): Ethertype = 0x88F7 */
    SNOOP_ARP            = 93,   /* IPv4 ARP: DMAC=broadcast, Ethertype = 0x0806 */
    SNOOP_RARP           = 94,   /* IPv4 RARP: Ethertype = 0x8035 */
    SNOOP_DHCP4          = 95,   /*
                                  * IPv4 DHCP Packet: L2 Payload is IPv4, IP Protocol = 17 (UDP)
                                  * Dest UDP Port = 67(server) Src UDP Port = 68(client)
                                  */
    SNOOP_ICMP4          = 96,   /* IPv4 ICMP L2 Payload is IPv4 IPv4 Protocol number: 1 (ICMP) */
    SNOOP_IGMP           = 97,   /* IPv4 IGMP L2 Payload is IPv4 IPv4 Protocol number: 2 (IGMP) */
    SNOOP_MLD            = 98,   /* IPv6 MLD L2 Payload is IPv6 IPv6 Next Header is ICMPv6 (58) + Type = 130/131/132 */
    SNOOP_ND_RD          = 99,   /*
                                  * IPv6 Discovery (ND/RD) L2 Payload is IPv6 IPv6 Next Header is
                                  * ICMPv6 (58) + Type = 133/134/135/136/137
                                  */
    SNOOP_ICMP6          = 100,  /* L2 Payload is IPv6 IPv6 next header is ICMPv6(58) */
    SNOOP_DHCP6          = 101,  /* IPv6 DHCP6 Packet: IPv6 Next Header is UDP, DP=546/547 */
    RESERVED4            = 102,  /* Reserved */
    RESERVED5            = 103,  /* Reserved */
    RESERVED6            = 104,  /* Reserved */
    RESERVED7            = 105,  /* Reserved */
    RESERVED8            = 106,  /* Reserved */
    RESERVED9            = 107,  /* Reserved */
    RESERVED10           = 108,  /* Reserved */
    RESERVED11           = 109,  /* Reserved */
    RESERVED12           = 110,  /* Reserved */
    RESERVED13           = 111,  /* Reserved */
    FM_DST_COPY_CPU      = 112,  /* FM destination lookup fwd_ctrl indicates copy to CPU. */
    FM_COPY_CPU          = 113,  /* L2 ARL copy to cpu */
    ULF_DROP             = 114,  /* L2 DMAC Unicast frame lookup failure. */
    MLF_DROP             = 115,  /* L2 DMAC Multicast frame lookup failure. */
    BC_DROP              = 116,  /* L2 DMAC Broadcast frame lookup failure. */
    CA_SA_MOVE           = 117,  /* Trap from cascade device, SA (station) moved SLI */
    SA_MOVE              = 118,  /* SA (station) moved SLI */
    MAC_LEARN_LIMIT      = 119,  /* Port has reached L2 SMAC self-learned limit */
    MAC_SA_FULL          = 120,  /* Hardware learning tried to insert a SMAC and all buckets are full */
    CA_LEARN_FRAME       = 121,  /* Trap from cascade device, Learning Frame was copied to the CPU */
    LEARN_FRAME          = 122,  /* L2 Learning Frame was copied to the CPU */
    NO_LRN_LKP_FAIL      = 123,  /* Source lookup fail for a frame for which learning was not attempted */
    VLAN_UNREG           = 124,  /*
                                  * Indexed VLAN is not a registered VLAN. This can occur based on a LIM miss
                                  * or PV2LI port not in membership.
                                  */
    INV_SLI              = 125,  /* SLI of the packet is invalid (=0) */
    INV_FWD_OP           = 126,  /* A CB-tag was received with an invalid/unsupported FWD_OP code. */
    RESERVED14           = 127,  /* Reserved */


} soc_robo2_trapid_t;


#endif /*BCM53158_B0*/
extern sys_error_t bcm53158_auto_dos_trap_set(uint8 unit, uint16 flags);
extern sys_error_t bcm53158_auto_dos_init(uint8 unit);

#endif /* _AUTO_DOS_H_ */
