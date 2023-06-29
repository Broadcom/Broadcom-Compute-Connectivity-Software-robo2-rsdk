/*
 * types.h
 * $Copyright: (c) 2017 Broadcom Limited.
 *
 * Description:
 *    Contains typedefs for shared types (compatiblity with SDK)
 */

#ifndef _SHR_TYPES_H
#define _SHR_TYPES_H

#include <shared/gport.h>

typedef int _shr_module_t;

typedef uint32_t cbx_port_t;
typedef uint16_t cbx_vlan_t;
typedef uint32_t cbx_portid_t;
typedef uint32_t cbx_vlanid_t;
typedef uint32_t cbx_stgid_t;
typedef uint32_t cbx_meterid_t;
typedef uint32_t cbx_cosqid_t;
typedef uint32_t cbx_mcastid_t;
typedef uint32_t cbx_trapid_t;
typedef uint32_t cbx_mirrorid_t;
typedef uint8_t  eth_mac_t[6];
typedef int      cbx_encap_info_t;

typedef enum cbx_packet_type_e {
    /** IPv4 packets */
    cbxPacketIpv4,

    /** IPv6 packets */
    cbxPacketIpv6,

    /** other (non IPv4 or IPv6) packets */
    cbxPacketOther,
} cbx_packet_type_t;

typedef enum cbx_traffic_type_e {

    /** Broadcast packets */
    cbxTrafficTypeDLF,

    /** Unicast packets */
    cbxTrafficTypeUnicast,

    /** Multicast packets */
    cbxTrafficTypeMulticast,

    /** Broadcast packets */
    cbxTrafficTypeBroadcast,

} cbx_traffic_type_t;

/* For declaring re-usable API functions */
#define APIFUNC(_fn)    _fn
#define REENTRANT
#define USE_INTERNAL_RAM

/* Normal static attribute */
#ifndef STATIC
#define STATIC static
#define STATICFN        static
#define STATICCBK       static
/* For re-usable static function  */
#define APISTATIC
#endif


/* Add "const" declaration to web related generated files from sspgen.pl
 * so those will be put in .rodata section and reduce RAM usage.
 */
#define RES_CONST_DECL const

#define XDATA
#define CODE

/* Type of system ticks */
typedef uint32 tick_t;

typedef int _shr_if_t;
#define CBX_PORTID_TYPE_LOCAL                _SHR_GPORT_TYPE_LOCAL
#define CBX_PORTID_IS_SET(_portid)           _SHR_GPORT_IS_SET(_portid)
#define CBX_PORTID_IS_LOCAL(_portid)         _SHR_GPORT_IS_LOCAL(_portid)
#define CBX_PORTID_IS_VLAN_PORT(_portid)     _SHR_GPORT_IS_VLAN_PORT(_portid)
#define CBX_PORTID_IS_TRUNK(_portid)         _SHR_GPORT_IS_TRUNK(_portid)
#define CBX_PORTID_IS_MCAST(_portid)         _SHR_GPORT_IS_MCAST(_portid)


#define CBX_PORTID_LOCAL_SET(_portid, _port)  \
    _SHR_GPORT_LOCAL_SET(_portid, _port)

#define CBX_PORTID_LOCAL_GET(_portid)  \
    (!_SHR_GPORT_IS_LOCAL(_portid) ? -1 : \
    _SHR_GPORT_LOCAL_GET(_portid))

#define CBX_PORTID_VLAN_PORT_ID_SET(_portid, _id)    \
        _SHR_GPORT_VLAN_PORT_ID_SET(_portid, _id)

#define CBX_PORTID_VLAN_PORT_ID_GET(_portid)  \
    (!_SHR_GPORT_IS_VLAN_PORT(_portid) ? -1 :  \
    _SHR_GPORT_VLAN_PORT_ID_GET(_portid))

#define CBX_PORTID_TRUNK_SET(_portid, _port)  \
    _SHR_GPORT_TRUNK_SET(_portid, _port)

#define CBX_PORTID_TRUNK_GET(_portid)  \
    (!_SHR_GPORT_IS_TRUNK(_portid) ? -1 : \
    _SHR_GPORT_TRUNK_GET(_portid))

#define CBX_PORTID_MCAST_ID_SET(_portid, _id)  \
    _SHR_GPORT_MCAST_ID_SET(_portid, _id)

#define CBX_PORTID_MCAST_ID_GET(_portid)  \
    (!_SHR_GPORT_IS_MCAST(_portid) ? -1 : \
    _SHR_GPORT_MCAST_ID_GET(_portid))

#endif

