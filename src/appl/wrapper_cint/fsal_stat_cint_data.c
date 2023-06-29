/*
 *
 * $Copyright: (c) 2017 Broadcom Corporation
 * All Rights Reserved.$
 *
 * fsal_stat_cint_data.c
 *
 * Hand-coded support for FSAL routines.
 */
int fsal_stat_cint_data_not_empty;

#if defined(INCLUDE_LIB_CINT)
#include <sal_types.h>
#include <types.h>
#include <cint_config.h>
#include <cint_types.h>
#include <cint_porting.h>
#include <stat.h>

CINT_FWRAPPER_CREATE_RP0(int, int, 0, 0,
                         cbx_stat_init);
CINT_FWRAPPER_CREATE_RP0(int, int, 0, 0,
                         cbx_stat_reset);
CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         cbx_port_stat_get,
                         cbx_portid_t, cbx_portid_t, portid, 0, 0,
                         cbx_port_stat_counter_t, cbx_port_stat_counter_t,
						 type, 0, 0,
                         uint64_t*, uint64_t, value, 1, 0);
CINT_FWRAPPER_CREATE_RP4(int, int, 0, 0,
                         cbx_port_stat_mult_get,
                         cbx_portid_t, cbx_portid_t, portid, 0, 0,
                         int, int, number_of_stats, 0, 0,
                         cbx_port_stat_counter_t*, cbx_port_stat_counter_t,
						 type_array, 1, 0,
                         uint64_t*, uint64_t, value_array, 1, 0);
CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         cbx_port_stat_clear,
                         cbx_portid_t, cbx_portid_t, portid, 0, 0);
CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         cbx_vlan_stat_clear,
                         cbx_vlanid_t, cbx_vlanid_t, vlanid, 0, 0);
CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         cbx_cosq_stat_get,
                         cbx_cosqid_t, cbx_cosqid_t, cosqid, 0, 0,
                         cbx_cosq_stat_counter_t, cbx_cosq_stat_counter_t,
						 type, 0, 0,
                         uint64_t*, uint64_t, value, 1, 0);
CINT_FWRAPPER_CREATE_RP4(int, int, 0, 0,
                         cbx_cosq_stat_mult_get,
                         cbx_cosqid_t, cbx_cosqid_t, cosqid, 0, 0,
                         int, int, number_of_stats, 0, 0,
                         cbx_cosq_stat_counter_t*, cbx_cosq_stat_counter_t,
						 type_array, 1, 0,
                         uint64_t*, uint64_t, value_array, 1, 0);
CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         cbx_cosq_stat_clear,
                         cbx_cosqid_t, cbx_cosqid_t, cosqid, 0, 0);
CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         cbx_meter_stat_get,
                         cbx_meterid_t, cbx_meterid_t, meterid, 0, 0,
                         cbx_meter_stat_counter_t, cbx_meter_stat_counter_t,
						 type, 0, 0,
                         uint64_t*, uint64_t, value, 1, 0);
CINT_FWRAPPER_CREATE_RP4(int, int, 0, 0,
                         cbx_meter_stat_mult_get,
                         cbx_meterid_t, cbx_meterid_t, meterid, 0, 0,
                         int, int, number_of_stats, 0, 0,
                         cbx_meter_stat_counter_t*, cbx_meter_stat_counter_t,
						 type_array, 1, 0,
                         uint64_t*, uint64_t, value_array, 1, 0);
CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         cbx_meter_stat_clear,
                         cbx_meterid_t, cbx_meterid_t, meterid, 0, 0);

static cint_struct_type_t __cint_fsal_stat_structures[] =
{
    CINT_ENTRY_LAST
};


static cint_enum_map_t __cint_enum_map__cbx_port_stat_counter_t[] =
{
    { "CBX_PORT_STAT_IF_IN_OCTETS",
	   CBX_PORT_STAT_IF_IN_OCTETS },
    { "CBX_PORT_STAT_IF_IN_UCAST_PKTS",
	   CBX_PORT_STAT_IF_IN_UCAST_PKTS },
    { "CBX_PORT_STAT_IF_IN_NON_UCAST_PKTS",
	   CBX_PORT_STAT_IF_IN_NON_UCAST_PKTS },
    { "CBX_PORT_STAT_IF_IN_DISCARDS",
	   CBX_PORT_STAT_IF_IN_DISCARDS },
    { "CBX_PORT_STAT_IF_IN_ERRORS",
	   CBX_PORT_STAT_IF_IN_ERRORS },
    { "CBX_PORT_STAT_IF_IN_UNKNOWN_PROTOS",
	   CBX_PORT_STAT_IF_IN_UNKNOWN_PROTOS },
    { "CBX_PORT_STAT_IF_IN_BROADCAST_PKTS",
	   CBX_PORT_STAT_IF_IN_BROADCAST_PKTS },
    { "CBX_PORT_STAT_IF_IN_MULTICAST_PKTS",
	   CBX_PORT_STAT_IF_IN_MULTICAST_PKTS },
    { "CBX_PORT_STAT_IF_IN_VLAN_DISCARDS",
	   CBX_PORT_STAT_IF_IN_VLAN_DISCARDS },
    { "CBX_PORT_STAT_IF_OUT_OCTETS",
	   CBX_PORT_STAT_IF_OUT_OCTETS },
    { "CBX_PORT_STAT_IF_OUT_UCAST_PKTS",
	   CBX_PORT_STAT_IF_OUT_UCAST_PKTS },
    { "CBX_PORT_STAT_IF_OUT_NON_UCAST_PKTS",
	   CBX_PORT_STAT_IF_OUT_NON_UCAST_PKTS },
    { "CBX_PORT_STAT_IF_OUT_DISCARDS",
	   CBX_PORT_STAT_IF_OUT_DISCARDS },
    { "CBX_PORT_STAT_IF_OUT_ERRORS",
	   CBX_PORT_STAT_IF_OUT_ERRORS },
    { "CBX_PORT_STAT_IF_OUT_QLEN",
	   CBX_PORT_STAT_IF_OUT_QLEN },
    { "CBX_PORT_STAT_IF_OUT_BROADCAST_PKTS",
	   CBX_PORT_STAT_IF_OUT_BROADCAST_PKTS },
    { "CBX_PORT_STAT_IF_OUT_MULTICAST_PKTS",
	   CBX_PORT_STAT_IF_OUT_MULTICAST_PKTS },
    { "CBX_PORT_STAT_ETHER_STATS_DROP_EVENTS",
	   CBX_PORT_STAT_ETHER_STATS_DROP_EVENTS },
    { "CBX_PORT_STAT_ETHER_STATS_MULTICAST_PKTS",
	   CBX_PORT_STAT_ETHER_STATS_MULTICAST_PKTS },
    { "CBX_PORT_STAT_ETHER_STATS_BROADCAST_PKTS",
	   CBX_PORT_STAT_ETHER_STATS_BROADCAST_PKTS },
    { "CBX_PORT_STAT_ETHER_STATS_UNDERSIZE_PKTS",
	   CBX_PORT_STAT_ETHER_STATS_UNDERSIZE_PKTS },
    { "CBX_PORT_STAT_ETHER_STATS_FRAGMENTS",
	   CBX_PORT_STAT_ETHER_STATS_FRAGMENTS },
    { "CBX_PORT_STAT_ETHER_STATS_PKTS_64_OCTETS",
	   CBX_PORT_STAT_ETHER_STATS_PKTS_64_OCTETS },
    { "CBX_PORT_STAT_ETHER_STATS_PKTS_65_TO_127_OCTETS",
	   CBX_PORT_STAT_ETHER_STATS_PKTS_65_TO_127_OCTETS },
    { "CBX_PORT_STAT_ETHER_STATS_PKTS_128_TO_255_OCTETS",
	   CBX_PORT_STAT_ETHER_STATS_PKTS_128_TO_255_OCTETS },
    { "CBX_PORT_STAT_ETHER_STATS_PKTS_256_TO_511_OCTETS",
	   CBX_PORT_STAT_ETHER_STATS_PKTS_256_TO_511_OCTETS },
    { "CBX_PORT_STAT_ETHER_STATS_PKTS_512_TO_1023_OCTETS",
	   CBX_PORT_STAT_ETHER_STATS_PKTS_512_TO_1023_OCTETS },
    { "CBX_PORT_STAT_ETHER_STATS_PKTS_1024_TO_1522_OCTETS",
	   CBX_PORT_STAT_ETHER_STATS_PKTS_1024_TO_1522_OCTETS },
    { "CBX_PORT_STAT_ETHER_STATS_PKTS_1523_TO_2047_OCTETS",
	   CBX_PORT_STAT_ETHER_STATS_PKTS_1523_TO_2047_OCTETS },
    { "CBX_PORT_STAT_ETHER_STATS_PKTS_2048_TO_4095_OCTETS",
	   CBX_PORT_STAT_ETHER_STATS_PKTS_2048_TO_4095_OCTETS },
    { "CBX_PORT_STAT_ETHER_STATS_PKTS_4096_TO_8191_OCTETS",
	   CBX_PORT_STAT_ETHER_STATS_PKTS_4096_TO_8191_OCTETS },
    { "CBX_PORT_STAT_ETHER_STATS_PKTS_8192_TO_MAX_OCTETS",
	   CBX_PORT_STAT_ETHER_STATS_PKTS_8192_TO_MAX_OCTETS },
    { "CBX_PORT_STAT_ETHER_STATS_OVERSIZE_PKTS",
	   CBX_PORT_STAT_ETHER_STATS_OVERSIZE_PKTS },
    { "CBX_PORT_STAT_ETHER_RX_OVERSIZE_PKTS",
	   CBX_PORT_STAT_ETHER_RX_OVERSIZE_PKTS },
    { "CBX_PORT_STAT_ETHER_TX_OVERSIZE_PKTS",
	   CBX_PORT_STAT_ETHER_TX_OVERSIZE_PKTS },
    { "CBX_PORT_STAT_ETHER_STATS_JABBERS",
	   CBX_PORT_STAT_ETHER_STATS_JABBERS },
    { "CBX_PORT_STAT_ETHER_STATS_OCTETS",
	   CBX_PORT_STAT_ETHER_STATS_OCTETS },
    { "CBX_PORT_STAT_ETHER_STATS_PKTS",
	   CBX_PORT_STAT_ETHER_STATS_PKTS },
    { "CBX_PORT_STAT_ETHER_STATS_COLLISIONS",
	   CBX_PORT_STAT_ETHER_STATS_COLLISIONS },
    { "CBX_PORT_STAT_ETHER_STATS_CRC_ALIGN_ERRORS",
	   CBX_PORT_STAT_ETHER_STATS_CRC_ALIGN_ERRORS },
    { "CBX_PORT_STAT_ETHER_STATS_TX_NO_ERRORS",
	   CBX_PORT_STAT_ETHER_STATS_TX_NO_ERRORS },
    { "CBX_PORT_STAT_ETHER_STATS_RX_NO_ERRORS",
	   CBX_PORT_STAT_ETHER_STATS_RX_NO_ERRORS },
    { "CBX_PORT_STAT_IP_IN_RECEIVES",
	   CBX_PORT_STAT_IP_IN_RECEIVES },
    { "CBX_PORT_STAT_IP_IN_OCTETS",
	   CBX_PORT_STAT_IP_IN_OCTETS },
    { "CBX_PORT_STAT_IP_IN_UCAST_PKTS",
	   CBX_PORT_STAT_IP_IN_UCAST_PKTS },
    { "CBX_PORT_STAT_IP_IN_NON_UCAST_PKTS",
	   CBX_PORT_STAT_IP_IN_NON_UCAST_PKTS },
    { "CBX_PORT_STAT_IP_IN_DISCARDS",
	   CBX_PORT_STAT_IP_IN_DISCARDS },
    { "CBX_PORT_STAT_IP_OUT_OCTETS",
	   CBX_PORT_STAT_IP_OUT_OCTETS},
    { "CBX_PORT_STAT_IP_OUT_UCAST_PKTS",
	   CBX_PORT_STAT_IP_OUT_UCAST_PKTS },
    { "CBX_PORT_STAT_IP_OUT_NON_UCAST_PKTS",
	   CBX_PORT_STAT_IP_OUT_NON_UCAST_PKTS },
    { "CBX_PORT_STAT_IP_OUT_DISCARDS",
	   CBX_PORT_STAT_IP_OUT_DISCARDS },
    { "CBX_PORT_STAT_IPV6_IN_RECEIVES",
	   CBX_PORT_STAT_IPV6_IN_RECEIVES },
    { "CBX_PORT_STAT_IPV6_IN_OCTETS",
	   CBX_PORT_STAT_IPV6_IN_OCTETS },
    { "CBX_PORT_STAT_IPV6_IN_UCAST_PKTS",
	   CBX_PORT_STAT_IPV6_IN_UCAST_PKTS },
    { "CBX_PORT_STAT_IPV6_IN_NON_UCAST_PKTS",
	   CBX_PORT_STAT_IPV6_IN_NON_UCAST_PKTS },
    { "CBX_PORT_STAT_IPV6_IN_MCAST_PKTS",
	   CBX_PORT_STAT_IPV6_IN_MCAST_PKTS },
    { "CBX_PORT_STAT_IPV6_IN_DISCARDS",
	   CBX_PORT_STAT_IPV6_IN_DISCARDS },
    { "CBX_PORT_STAT_IPV6_OUT_OCTETS",
	   CBX_PORT_STAT_IPV6_OUT_OCTETS },
    { "CBX_PORT_STAT_IPV6_OUT_UCAST_PKTS",
	   CBX_PORT_STAT_IPV6_OUT_UCAST_PKTS },
    { "CBX_PORT_STAT_IPV6_OUT_NON_UCAST_PKTS",
	   CBX_PORT_STAT_IPV6_OUT_NON_UCAST_PKTS },
    { "CBX_PORT_STAT_IPV6_OUT_MCAST_PKTS",
	   CBX_PORT_STAT_IPV6_OUT_MCAST_PKTS },
    { "CBX_PORT_STAT_IPV6_OUT_DISCARDS",
	   CBX_PORT_STAT_IPV6_OUT_DISCARDS },
    { "CBX_PORT_STAT_GREEN_DISCARD_DROPPED_PACKETS",
	   CBX_PORT_STAT_GREEN_DISCARD_DROPPED_PACKETS },
    { "CBX_PORT_STAT_GREEN_DISCARD_DROPPED_BYTES",
	   CBX_PORT_STAT_GREEN_DISCARD_DROPPED_BYTES },
    { "CBX_PORT_STAT_YELLOW_DISCARD_DROPPED_PACKETS",
	   CBX_PORT_STAT_YELLOW_DISCARD_DROPPED_PACKETS },
    { "CBX_PORT_STAT_YELLOW_DISCARD_DROPPED_BYTES",
	   CBX_PORT_STAT_YELLOW_DISCARD_DROPPED_BYTES },
    { "CBX_PORT_STAT_RED_DISCARD_DROPPED_PACKETS",
	   CBX_PORT_STAT_RED_DISCARD_DROPPED_PACKETS},
    { "CBX_PORT_STAT_RED_DISCARD_DROPPED_BYTES",
	   CBX_PORT_STAT_RED_DISCARD_DROPPED_BYTES },
    { "CBX_PORT_STAT_DISCARD_DROPPED_PACKETS",
	   CBX_PORT_STAT_DISCARD_DROPPED_PACKETS },
    { "CBX_PORT_STAT_DISCARD_DROPPED_BYTES",
	   CBX_PORT_STAT_DISCARD_DROPPED_BYTES },
    { "CBX_PORT_STAT_ETHER_IN_PKTS_64_OCTETS",
	   CBX_PORT_STAT_ETHER_IN_PKTS_64_OCTETS },
    { "CBX_PORT_STAT_ETHER_IN_PKTS_65_TO_127_OCTETS",
	   CBX_PORT_STAT_ETHER_IN_PKTS_65_TO_127_OCTETS },
    { "CBX_PORT_STAT_ETHER_IN_PKTS_128_TO_255_OCTETS",
	   CBX_PORT_STAT_ETHER_IN_PKTS_128_TO_255_OCTETS },
    { "CBX_PORT_STAT_ETHER_IN_PKTS_256_TO_511_OCTETS",
	   CBX_PORT_STAT_ETHER_IN_PKTS_256_TO_511_OCTETS },
    { "CBX_PORT_STAT_ETHER_IN_PKTS_512_TO_1023_OCTETS",
	   CBX_PORT_STAT_ETHER_IN_PKTS_512_TO_1023_OCTETS  },
    { "CBX_PORT_STAT_ETHER_IN_PKTS_1024_TO_1522_OCTETS",
	   CBX_PORT_STAT_ETHER_IN_PKTS_1024_TO_1522_OCTETS },
    { "CBX_PORT_STAT_ETHER_IN_PKTS_1523_TO_2047_OCTETS",
	   CBX_PORT_STAT_ETHER_IN_PKTS_1523_TO_2047_OCTETS },
    { "CBX_PORT_STAT_ETHER_IN_PKTS_2048_TO_4095_OCTETS",
	   CBX_PORT_STAT_ETHER_IN_PKTS_2048_TO_4095_OCTETS },
    { "CBX_PORT_STAT_ETHER_IN_PKTS_4096_TO_8191_OCTETS",
	   CBX_PORT_STAT_ETHER_IN_PKTS_4096_TO_8191_OCTETS },
    { "CBX_PORT_STAT_ETHER_IN_PKTS_8192_TO_MAX_OCTETS",
	   CBX_PORT_STAT_ETHER_IN_PKTS_8192_TO_MAX_OCTETS },
    { "CBX_PORT_STAT_ETHER_OUT_PKTS_64_OCTETS",
	   CBX_PORT_STAT_ETHER_OUT_PKTS_64_OCTETS },
    { "CBX_PORT_STAT_ETHER_OUT_PKTS_65_TO_127_OCTETS",
	   CBX_PORT_STAT_ETHER_OUT_PKTS_65_TO_127_OCTETS },
    { "CBX_PORT_STAT_ETHER_OUT_PKTS_128_TO_255_OCTETS",
	   CBX_PORT_STAT_ETHER_OUT_PKTS_128_TO_255_OCTETS },
    { "CBX_PORT_STAT_ETHER_OUT_PKTS_256_TO_511_OCTETS",
	   CBX_PORT_STAT_ETHER_OUT_PKTS_256_TO_511_OCTETS },
    { "CBX_PORT_STAT_ETHER_OUT_PKTS_512_TO_1023_OCTETS",
	   CBX_PORT_STAT_ETHER_OUT_PKTS_512_TO_1023_OCTETS },
    { "CBX_PORT_STAT_ETHER_OUT_PKTS_1024_TO_1522_OCTETS",
	   CBX_PORT_STAT_ETHER_OUT_PKTS_1024_TO_1522_OCTETS },
    { "CBX_PORT_STAT_ETHER_OUT_PKTS_1523_TO_2047_OCTETS",
	   CBX_PORT_STAT_ETHER_OUT_PKTS_1523_TO_2047_OCTETS },
    { "CBX_PORT_STAT_ETHER_OUT_PKTS_2048_TO_4095_OCTETS",
	   CBX_PORT_STAT_ETHER_OUT_PKTS_2048_TO_4095_OCTETS },
    { "CBX_PORT_STAT_ETHER_OUT_PKTS_4096_TO_8191_OCTETS",
	   CBX_PORT_STAT_ETHER_OUT_PKTS_4096_TO_8191_OCTETS },
    { "CBX_PORT_STAT_ETHER_OUT_PKTS_8192_TO_MAX_OCTETS",
	   CBX_PORT_STAT_ETHER_OUT_PKTS_8192_TO_MAX_OCTETS },
    CINT_ENTRY_LAST
};

static cint_enum_map_t __cint_enum_map__cbx_cosq_stat_counter_t[] =
{
    { "CBX_COSQ_STAT_PACKETS",
	   CBX_COSQ_STAT_PACKETS },
    { "CBX_COSQ_STAT_BYTES",
	   CBX_COSQ_STAT_BYTES },
    { "CBX_COSQ_STAT_DROPPED_PACKETS",
	   CBX_COSQ_STAT_DROPPED_PACKETS },
    { "CBX_COSQ_STAT_DROPPED_BYTES",
	   CBX_COSQ_STAT_DROPPED_BYTES },
    { "CBX_COSQ_STAT_GREEN_PACKETS",
	   CBX_COSQ_STAT_GREEN_PACKETS },
    { "CBX_COSQ_STAT_GREEN_BYTES",
	   CBX_COSQ_STAT_GREEN_BYTES },
    { "CBX_COSQ_STAT_GREEN_DROPPED_PACKETS",
	   CBX_COSQ_STAT_GREEN_DROPPED_PACKETS },
    { "CBX_COSQ_STAT_GREEN_DROPPED_BYTES",
	   CBX_COSQ_STAT_GREEN_DROPPED_BYTES },
    { "CBX_COSQ_STAT_YELLOW_PACKETS",
	   CBX_COSQ_STAT_YELLOW_PACKETS },
    { "CBX_COSQ_STAT_YELLOW_BYTES",
	   CBX_COSQ_STAT_YELLOW_BYTES },
    { "CBX_COSQ_STAT_YELLOW_DROPPED_PACKETS",
	   CBX_COSQ_STAT_YELLOW_DROPPED_PACKETS },
    { "CBX_COSQ_STAT_YELLOW_DROPPED_BYTES",
	   CBX_COSQ_STAT_YELLOW_DROPPED_BYTES },
    { "CBX_COSQ_STAT_RED_PACKETS",
	   CBX_COSQ_STAT_RED_PACKETS },
    { "CBX_COSQ_STAT_RED_BYTES",
	   CBX_COSQ_STAT_RED_BYTES },
    { "CBX_COSQ_STAT_RED_DROPPED_PACKETS",
	   CBX_COSQ_STAT_RED_DROPPED_PACKETS },
    { "CBX_COSQ_STAT_RED_DROPPED_BYTES",
	   CBX_COSQ_STAT_RED_DROPPED_BYTES },
    { "CBX_COSQ_STAT_GREEN_DISCARD_DROPPED_PACKETS",
	   CBX_COSQ_STAT_GREEN_DISCARD_DROPPED_PACKETS },
    { "CBX_COSQ_STAT_GREEN_DISCARD_DROPPED_BYTES",
	   CBX_COSQ_STAT_GREEN_DISCARD_DROPPED_BYTES },
    { "CBX_COSQ_STAT_YELLOW_DISCARD_DROPPED_PACKETS",
	   CBX_COSQ_STAT_YELLOW_DISCARD_DROPPED_PACKETS },
    { "CBX_COSQ_STAT_YELLOW_DISCARD_DROPPED_BYTES",
	   CBX_COSQ_STAT_YELLOW_DISCARD_DROPPED_BYTES },
    { "CBX_COSQ_STAT_RED_DISCARD_DROPPED_PACKETS",
	   CBX_COSQ_STAT_RED_DISCARD_DROPPED_PACKETS },
    { "CBX_COSQ_STAT_RED_DISCARD_DROPPED_BYTES",
	   CBX_COSQ_STAT_RED_DISCARD_DROPPED_BYTES },
    { "CBX_COSQ_STAT_DISCARD_DROPPED_PACKETS",
	   CBX_COSQ_STAT_DISCARD_DROPPED_PACKETS },
    { "CBX_COSQ_STAT_DISCARD_DROPPED_BYTES",
	   CBX_COSQ_STAT_DISCARD_DROPPED_BYTES },
    CINT_ENTRY_LAST

};
static cint_enum_map_t __cint_enum_map__cbx_meter_stat_counter_t[] =
{
    { "CBX_METER_STAT_PACKETS", CBX_METER_STAT_PACKETS },
    { "CBX_METER_STAT_ATTR_BYTES", CBX_METER_STAT_ATTR_BYTES },
    { "CBX_METER_STAT_GREEN_PACKETS", CBX_METER_STAT_GREEN_PACKETS },
    { "CBX_METER_STAT_GREEN_BYTES", CBX_METER_STAT_GREEN_BYTES },
    { "CBX_METER_STAT_YELLOW_PACKETS", CBX_METER_STAT_YELLOW_PACKETS },
    { "CBX_METER_STAT_YELLOW_BYTES", CBX_METER_STAT_YELLOW_BYTES },
    { "CBX_METER_STAT_RED_PACKETS", CBX_METER_STAT_RED_PACKETS },
    { "CBX_METER_STAT_RED_BYTES", CBX_METER_STAT_RED_BYTES },
    CINT_ENTRY_LAST
};

static cint_enum_type_t __cint_fsal_stat_enums[] =
{
    { "cbx_port_stat_counter_t", __cint_enum_map__cbx_port_stat_counter_t },
    { "cbx_cosq_stat_counter_t", __cint_enum_map__cbx_cosq_stat_counter_t },
    { "cbx_meter_stat_counter_t", __cint_enum_map__cbx_meter_stat_counter_t },
    CINT_ENTRY_LAST
};

static cint_parameter_desc_t __cint_fsal_stat_typedefs[] =
{
    {
#ifdef CONFIG_EXTERNAL_HOST
        "unsigned int",
#else
        "unsigned long",
#endif
        "uint32",
        0,
        0
    },
    {
        "uint32",
        "uint32_t",
        0,
        0
    },
    {
        "uint32",
        "cbx_portid_t",
        0,
        0
    },
    {
        "uint32",
        "cbx_vlanid_t",
        0,
        0
    },
    {
        "uint32",
        "cbx_cosqid_t",
        0,
        0
    },
    {
        "uint32",
        "cbx_meterid_t",
        0,
        0
    },
    {
        "unsigned long long",
        "uint64",
        0,
        0
    },
    {
        "uint64",
        "uint64_t",
        0,
        0
    },
    {
        "unsigned short",
        "uint16",
        0,
        0
    },
    {
        "uint16",
        "uint16_t",
        0,
        0
    },
    {
        "unsigned char",
        "uint8",
        0,
        0
    },
    {
        "uint8",
        "uint8_t",
        0,
        0
    },

    CINT_ENTRY_LAST
};

static cint_constants_t __cint_fsal_stat_constants[] =
{
    CINT_ENTRY_LAST
};

static cint_function_pointer_t __cint_fsal_stat_function_pointers[] =
{
    CINT_ENTRY_LAST
};

static cint_function_t __cint_fsal_stat_functions[] =
    {
        CINT_FWRAPPER_ENTRY(cbx_stat_init),
        CINT_FWRAPPER_ENTRY(cbx_stat_reset),
        CINT_FWRAPPER_ENTRY(cbx_port_stat_get),
        CINT_FWRAPPER_ENTRY(cbx_port_stat_mult_get),
        CINT_FWRAPPER_ENTRY(cbx_port_stat_clear),
        CINT_FWRAPPER_ENTRY(cbx_vlan_stat_clear),
        CINT_FWRAPPER_ENTRY(cbx_cosq_stat_get),
        CINT_FWRAPPER_ENTRY(cbx_cosq_stat_mult_get),
        CINT_FWRAPPER_ENTRY(cbx_cosq_stat_clear),
        CINT_FWRAPPER_ENTRY(cbx_meter_stat_get),
        CINT_FWRAPPER_ENTRY(cbx_meter_stat_mult_get),
        CINT_FWRAPPER_ENTRY(cbx_meter_stat_clear),
        CINT_ENTRY_LAST
    };
cint_data_t fsal_stat_cint_data =
    {
        NULL,
        __cint_fsal_stat_functions,
        __cint_fsal_stat_structures,
        __cint_fsal_stat_enums,
        __cint_fsal_stat_typedefs,
        __cint_fsal_stat_constants,
        __cint_fsal_stat_function_pointers
    };

#endif /* INCLUDE_LIB_CINT */

