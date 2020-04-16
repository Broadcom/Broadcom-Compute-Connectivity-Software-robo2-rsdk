/*
 * * $ID: $
 * * 
 * * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 * * 
 * * $Copyright: (c) 2020 Broadcom Inc.
 * *
 * * File:
 * *     counter.c
 * * Purpose:
 * *     Avenger Packet Statistics Counter Management
 * *
 * */

#include <bsl_log/bsl.h>
#include <bsl_log/bslenum.h>
#include <soc/drv.h>
#include <soc/robo2/common/driver.h>
#include <soc/robo2/common/memregs.h>
#include <soc/robo2/common/tables.h>
#include <soc/counter.h>
#include <soc/error.h>
#include <fsal/stat.h>
#include <fsal/port.h>
#include <soc/types.h>
#include <sal_sync.h>
#include <fsal_int/stat.h>
#include <fsal_int/types.h>
#include <sal_time.h>
#include <sal_task.h>
#include <sal_util.h>

#define LOW32_MASK  0x00000000ffffffff
#define HIGH32_MASK 0xffffffff00000000
#define LS32        32
#define RS32        32

#define INDEX      ROBO2_INDEX
#define sal_sem_BINARY      1
#define COUNTER_ATOMIC_DEF soc_control_t *
extern void *sal_memset(void *dest, int c, size_t cnt );

#ifdef COUNTER_DEBUG
const char* port_stat[] = {
    "CBX_PORT_STAT_IF_IN_OCTETS",
    "CBX_PORT_STAT_IF_IN_UCAST_PKTS",
    "CBX_PORT_STAT_IF_IN_NON_UCAST_PKTS",
    "CBX_PORT_STAT_IF_IN_DISCARDS",
    "CBX_PORT_STAT_IF_IN_ERRORS",
    "CBX_PORT_STAT_IF_IN_UNKNOWN_PROTOS",
    "CBX_PORT_STAT_IF_IN_BROADCAST_PKTS",
    "CBX_PORT_STAT_IF_IN_MULTICAST_PKTS",
    "CBX_PORT_STAT_IF_IN_VLAN_DISCARDS",
    "CBX_PORT_STAT_IF_OUT_OCTETS",
    "CBX_PORT_STAT_IF_OUT_UCAST_PKTS",
    "CBX_PORT_STAT_IF_OUT_NON_UCAST_PKTS",
    "CBX_PORT_STAT_IF_OUT_DISCARDS",
    "CBX_PORT_STAT_IF_OUT_ERRORS",
    "CBX_PORT_STAT_IF_OUT_QLEN",
    "CBX_PORT_STAT_IF_OUT_BROADCAST_PKTS",
    "CBX_PORT_STAT_IF_OUT_MULTICAST_PKTS",
    "CBX_PORT_STAT_ETHER_STATS_DROP_EVENTS",
    "CBX_PORT_STAT_ETHER_STATS_MULTICAST_PKTS",
    "CBX_PORT_STAT_ETHER_STATS_BROADCAST_PKTS",
    "CBX_PORT_STAT_ETHER_STATS_UNDERSIZE_PKTS",
    "CBX_PORT_STAT_ETHER_STATS_FRAGMENTS",
    "CBX_PORT_STAT_ETHER_STATS_PKTS_64_OCTETS",
    "CBX_PORT_STAT_ETHER_STATS_PKTS_65_TO_127_OCTETS",
    "CBX_PORT_STAT_ETHER_STATS_PKTS_128_TO_255_OCTETS",
    "CBX_PORT_STAT_ETHER_STATS_PKTS_256_TO_511_OCTETS",
    "CBX_PORT_STAT_ETHER_STATS_PKTS_512_TO_1023_OCTETS",
    "CBX_PORT_STAT_ETHER_STATS_PKTS_1024_TO_1522_OCTETS",
    "CBX_PORT_STAT_ETHER_STATS_PKTS_1523_TO_2047_OCTETS",
    "CBX_PORT_STAT_ETHER_STATS_PKTS_2048_TO_4095_OCTETS",
    "CBX_PORT_STAT_ETHER_STATS_PKTS_4096_TO_8191_OCTETS",
    "CBX_PORT_STAT_ETHER_STATS_PKTS_8192_TO_MAX_OCTETS",
    "CBX_PORT_STAT_ETHER_STATS_OVERSIZE_PKTS",
    "CBX_PORT_STAT_ETHER_RX_OVERSIZE_PKTS",
    "CBX_PORT_STAT_ETHER_TX_OVERSIZE_PKTS",
    "CBX_PORT_STAT_ETHER_STATS_JABBERS",
    "CBX_PORT_STAT_ETHER_STATS_OCTETS",
    "CBX_PORT_STAT_ETHER_STATS_PKTS",
    "CBX_PORT_STAT_ETHER_STATS_COLLISIONS",
    "CBX_PORT_STAT_ETHER_STATS_CRC_ALIGN_ERRORS",
    "CBX_PORT_STAT_ETHER_STATS_TX_NO_ERRORS",
    "CBX_PORT_STAT_ETHER_STATS_RX_NO_ERRORS",
    "CBX_PORT_STAT_IP_IN_RECEIVES",
    "CBX_PORT_STAT_IP_IN_OCTETS",
    "CBX_PORT_STAT_IP_IN_UCAST_PKTS",
    "CBX_PORT_STAT_IP_IN_NON_UCAST_PKTS",
    "CBX_PORT_STAT_IP_IN_DISCARDS",
    "CBX_PORT_STAT_IP_OUT_OCTETS",
    "CBX_PORT_STAT_IP_OUT_UCAST_PKTS",
    "CBX_PORT_STAT_IP_OUT_NON_UCAST_PKTS",
    "CBX_PORT_STAT_IP_OUT_DISCARDS",
    "CBX_PORT_STAT_IPV6_IN_RECEIVES",
    "CBX_PORT_STAT_IPV6_IN_OCTETS",
    "CBX_PORT_STAT_IPV6_IN_UCAST_PKTS",
    "CBX_PORT_STAT_IPV6_IN_NON_UCAST_PKTS",
    "CBX_PORT_STAT_IPV6_IN_MCAST_PKTS",
    "CBX_PORT_STAT_IPV6_IN_DISCARDS",
    "CBX_PORT_STAT_IPV6_OUT_OCTETS",
    "CBX_PORT_STAT_IPV6_OUT_UCAST_PKTS",
    "CBX_PORT_STAT_IPV6_OUT_NON_UCAST_PKTS",
    "CBX_PORT_STAT_IPV6_OUT_MCAST_PKTS",
    "CBX_PORT_STAT_IPV6_OUT_DISCARDS",
    "CBX_PORT_STAT_GREEN_DISCARD_DROPPED_PACKETS",
    "CBX_PORT_STAT_GREEN_DISCARD_DROPPED_BYTES",
    "CBX_PORT_STAT_YELLOW_DISCARD_DROPPED_PACKETS",
    "CBX_PORT_STAT_YELLOW_DISCARD_DROPPED_BYTES",
    "CBX_PORT_STAT_RED_DISCARD_DROPPED_PACKETS",
    "CBX_PORT_STAT_RED_DISCARD_DROPPED_BYTES",
    "CBX_PORT_STAT_DISCARD_DROPPED_PACKETS",
    "CBX_PORT_STAT_DISCARD_DROPPED_BYTES",
    "CBX_PORT_STAT_ETHER_IN_PKTS_64_OCTETS",
    "CBX_PORT_STAT_ETHER_IN_PKTS_65_TO_127_OCTETS",
    "CBX_PORT_STAT_ETHER_IN_PKTS_128_TO_255_OCTETS",
    "CBX_PORT_STAT_ETHER_IN_PKTS_256_TO_511_OCTETS",
    "CBX_PORT_STAT_ETHER_IN_PKTS_512_TO_1023_OCTETS",
    "CBX_PORT_STAT_ETHER_IN_PKTS_1024_TO_1522_OCTETS",
    "CBX_PORT_STAT_ETHER_IN_PKTS_1523_TO_2047_OCTETS",
    "CBX_PORT_STAT_ETHER_IN_PKTS_2048_TO_4095_OCTETS",
    "CBX_PORT_STAT_ETHER_IN_PKTS_4096_TO_8191_OCTETS",
    "CBX_PORT_STAT_ETHER_IN_PKTS_8192_TO_MAX_OCTETS",
    "CBX_PORT_STAT_ETHER_OUT_PKTS_64_OCTETS",
    "CBX_PORT_STAT_ETHER_OUT_PKTS_65_TO_127_OCTETS",
    "CBX_PORT_STAT_ETHER_OUT_PKTS_128_TO_255_OCTETS",
    "CBX_PORT_STAT_ETHER_OUT_PKTS_256_TO_511_OCTETS",
    "CBX_PORT_STAT_ETHER_OUT_PKTS_512_TO_1023_OCTETS",
    "CBX_PORT_STAT_ETHER_OUT_PKTS_1024_TO_1522_OCTETS",
    "CBX_PORT_STAT_ETHER_OUT_PKTS_1523_TO_2047_OCTETS",
    "CBX_PORT_STAT_ETHER_OUT_PKTS_2048_TO_4095_OCTETS",
    "CBX_PORT_STAT_ETHER_OUT_PKTS_4096_TO_8191_OCTETS",
    "CBX_PORT_STAT_ETHER_OUT_PKTS_8192_TO_MAX_OCTETS",
};
#endif

typedef enum xlmac_ctr{
    RBYTr_CTR = 0,
    TMGVr_CTR = 1,
    RBCAr_CTR = 2,
    RUCAr_CTR = 3,
    RMCAr_CTR = 4,
    TERRr_CTR = 5,
    RMGVr_CTR = 6,
    RFCSr_CTR = 7,
    RFRGr_CTR = 8,
    RJBRr_CTR = 9,
    ROVRr_CTR = 10,
    RUNDr_CTR = 11,
    TBYTr_CTR = 12,
    RALNr_CTR = 13,
    TBCAr_CTR = 14,
    TMCAr_CTR = 15,
    TUCAr_CTR = 16,
    TFCSr_CTR = 17,
    TXCLr_CTR = 18,
    TLCLr_CTR = 19,
    TNCLr_CTR = 20,
    T64r_CTR = 21,
    T127r_CTR = 22,
    T255r_CTR = 23,
    T511r_CTR = 24,
    T1023r_CTR = 25,
    T1518r_CTR = 26,
    T2047r_CTR = 27,
    T4095r_CTR = 28,
    T9216r_CTR = 29,
    T16383r_CTR = 30,
    R64r_CTR = 31,
    R127r_CTR = 32,
    R255r_CTR = 33,
    R511r_CTR = 34,
    R1023r_CTR = 35,
    R1518r_CTR = 36,
    R2047r_CTR = 37,
    R4095r_CTR = 38,
    R9216r_CTR = 39,
    R16383r_CTR = 40,
    RPOKr_CTR = 41,
    TPOKr_CTR = 42,
    RERPKTr_CTR = 43,
    TOVRr_CTR = 44,
    TPKTr_CTR = 45,
    RPKTr_CTR = 46,
/*  Needed if stat_pkt=tx+rx for below cases
    TFRGr_CTR = 47,
    TJBRr_CTR = 48,
*/
}offset;

typedef enum unimac_ctr{
    MIB_RX_ALL_OCTETS_HIr_CTR = 0,
    MIB_RX_ALL_OCTETS_LOr_CTR = 1,
    MIB_RX_BRDCASTr_CTR = 2,
    MIB_RXUNICASTPKTSr_CTR = 3,
    MIB_RX_MULTr_CTR = 4,
    MIB_RX_PAUSr_CTR = 5,
    MIB_RX_DROPr_CTR = 6,
    MIB_RX_CRC_ALIGNr_CTR = 7,
    MIB_RX_FRAGr_CTR = 8,
    MIB_RX_JABr_CTR = 9,
    MIB_RX_OVRr_CTR = 10,
    MIB_RX_UNDr_CTR = 11,
    MIB_TX_ALL_OCTETS_HIr_CTR = 12,
    MIB_TX_ALL_OCTETS_LOr_CTR = 13,
    MIB_TX_BRDCASTr_CTR = 14,
    MIB_TX_MULTr_CTR = 15,
    MIB_TXUNICASTPKTr_CTR = 16,
    MIB_TX_PAUSr_CTR = 17,
    MIB_TX_EX_COLr_CTR = 18,
    MIB_TX_LATEr_CTR = 19,
    MIB_TX_COLr_CTR = 20,
    MIB_TX_64r_CTR = 21,
    MIB_TX_65_127r_CTR = 22,
    MIB_TX_128_255r_CTR = 23,
    MIB_TX_256_511r_CTR = 24,
    MIB_TX_512_1023r_CTR = 25,
    MIB_TX_1024_1522r_CTR = 26,
    MIB_TX_1523_2047r_CTR = 27,
    MIB_TX_2048_4095r_CTR = 28,
    MIB_TX_4096_8191r_CTR = 29,
    MIB_TX_8192_MAXr_CTR = 30,
    MIB_RX_64r_CTR = 31,
    MIB_RX_65_127r_CTR = 32,
    MIB_RX_128_255r_CTR = 33,
    MIB_RX_256_511r_CTR = 34,
    MIB_RX_512_1023r_CTR = 35,
    MIB_RX_1024_1522r_CTR = 36,
    MIB_RX_1523_2047r_CTR = 37,
    MIB_RX_2048_4095r_CTR = 38,
    MIB_RX_4096_8191r_CTR = 39,
    MIB_RX_8192_MAXr_CTR = 40,
    MIB_RX_GD_PKTSr_CTR = 41,
    MIB_TX_GD_PKTSr_CTR = 42,
    MIB_RX_SYMr_CTR = 43,
    MIB_TX_OVERr_CTR = 44,
    MIB_TX_ALL_PKTSr_CTR = 45,
    MIB_RX_ALL_PKTSr_CTR = 46,
/*  Needed if stat_pkt=tx+rx for below cases
    MIB_TX_FRAGr_CTR = 47,
    MIB_TX_JABr_CTR = 48,
*/
}counter_offset;



/*
 * Packet Statistics Counter Map for bcm53158_a0
 *
 * NOTE: soc_attach verifies this map is correct and prints warnings if not.
 *
 * The _avenger_ge_counters array is a list of counter registers in the order
 * we save in the internal software database.
 * counter_map[0 to NUM_COUNTER-1] corresponds to internal addresses
 * COUNTER_REG_FIRST through COUNTER_REG_LAST.
 *
 * This map structure, contents, and size are exposed only to provide a
 * convenient way to loop through all available counters.
 */
static soc_ctr_ref_t _avenger_unimac_counters[] = {
    {MIB_RX_ALL_OCTETS_HIr_ROBO2, MIB_RX_ALL_OCTETS_HIr_CTR},
    {MIB_RX_ALL_OCTETS_LOr_ROBO2, MIB_RX_ALL_OCTETS_LOr_CTR},
    {MIB_RX_BRDCASTr_ROBO2, MIB_RX_BRDCASTr_CTR},
    {MIB_RXUNICASTPKTSr_ROBO2, MIB_RXUNICASTPKTSr_CTR},
    {MIB_RX_MULTr_ROBO2, MIB_RX_MULTr_CTR},
    {INVALID_Rr_ROBO2, MIB_RX_PAUSr_CTR},
    {MIB_RX_DROPr_ROBO2, MIB_RX_DROPr_CTR},
    {MIB_RX_CRC_ALIGNr_ROBO2, MIB_RX_CRC_ALIGNr_CTR},
    {MIB_RX_FRAGr_ROBO2, MIB_RX_FRAGr_CTR},
    {MIB_RX_JABr_ROBO2, MIB_RX_JABr_CTR},
    {MIB_RX_OVRr_ROBO2, MIB_RX_OVRr_CTR},
    {MIB_RX_UNDr_ROBO2, MIB_RX_UNDr_CTR},
    {MIB_TX_ALL_OCTETS_HIr_ROBO2, MIB_TX_ALL_OCTETS_HIr_CTR},
    {MIB_TX_ALL_OCTETS_LOr_ROBO2, MIB_TX_ALL_OCTETS_LOr_CTR},
    {MIB_TX_BRDCASTr_ROBO2, MIB_TX_BRDCASTr_CTR},
    {MIB_TX_MULTr_ROBO2, MIB_TX_MULTr_CTR},
    {MIB_TXUNICASTPKTr_ROBO2, MIB_TXUNICASTPKTr_CTR},
    {INVALID_Rr_ROBO2, MIB_TX_PAUSr_CTR},
    {MIB_TX_EX_COLr_ROBO2, MIB_TX_EX_COLr_CTR},
    {MIB_TX_LATEr_ROBO2, MIB_TX_LATEr_CTR},
    {MIB_TX_COLr_ROBO2, MIB_TX_COLr_CTR},
    {MIB_TX_64r_ROBO2, MIB_TX_64r_CTR},
    {MIB_TX_65_127r_ROBO2, MIB_TX_65_127r_CTR},
    {MIB_TX_128_255r_ROBO2, MIB_TX_128_255r_CTR},
    {MIB_TX_256_511r_ROBO2, MIB_TX_256_511r_CTR},
    {MIB_TX_512_1023r_ROBO2, MIB_TX_512_1023r_CTR},
    {MIB_TX_1024_1522r_ROBO2, MIB_TX_1024_1522r_CTR},
    {MIB_TX_1523_2047r_ROBO2, MIB_TX_1523_2047r_CTR},
    {MIB_TX_2048_4095r_ROBO2, MIB_TX_2048_4095r_CTR},
    {MIB_TX_4096_8191r_ROBO2, MIB_TX_4096_8191r_CTR},
    {MIB_TX_8192_MAXr_ROBO2, MIB_TX_8192_MAXr_CTR},
    {MIB_RX_64r_ROBO2, MIB_RX_64r_CTR},
    {MIB_RX_65_127r_ROBO2, MIB_RX_65_127r_CTR},
    {MIB_RX_128_255r_ROBO2, MIB_RX_128_255r_CTR},
    {MIB_RX_256_511r_ROBO2, MIB_RX_256_511r_CTR},
    {MIB_RX_512_1023r_ROBO2, MIB_RX_512_1023r_CTR},
    {MIB_RX_1024_1522r_ROBO2, MIB_RX_1024_1522r_CTR},
    {MIB_RX_1523_2047r_ROBO2, MIB_RX_1523_2047r_CTR},
    {MIB_RX_2048_4095r_ROBO2, MIB_RX_2048_4095r_CTR},
    {MIB_RX_4096_8191r_ROBO2, MIB_RX_4096_8191r_CTR},
    {MIB_RX_8192_MAXr_ROBO2, MIB_RX_8192_MAXr_CTR},
    {MIB_RX_GD_PKTSr_ROBO2, MIB_RX_GD_PKTSr_CTR},
    {MIB_TX_GD_PKTSr_ROBO2, MIB_TX_GD_PKTSr_CTR},
    {MIB_RX_SYMr_ROBO2, MIB_RX_SYMr_CTR},
    {MIB_TX_OVERr_ROBO2, MIB_TX_OVERr_CTR},
    {MIB_TX_ALL_PKTSr_ROBO2, MIB_TX_ALL_PKTSr_CTR},
    {MIB_RX_ALL_PKTSr_ROBO2, MIB_RX_ALL_PKTSr_CTR},
/*  {MIB_TX_FRAGr_ROBO2, MIB_TX_FRAGr_CTR},
    {MIB_TX_JABr_ROBO2, MIB_TX_JABr_CTR},
*/
};


static soc_ctr_ref_t _avenger_xlmac_counters[] = {
    {RBYTr_ROBO2, RBYTr_CTR},
    {TMGVr_ROBO2, TMGVr_CTR},
    {RBCAr_ROBO2, RBCAr_CTR},
    {RUCAr_ROBO2, RUCAr_CTR},
    {RMCAr_ROBO2, RMCAr_CTR},
    {TERRr_ROBO2, TERRr_CTR},
    {RMGVr_ROBO2, RMGVr_CTR},
    {RFCSr_ROBO2, RFCSr_CTR},
    {RFRGr_ROBO2, RFRGr_CTR},
    {RJBRr_ROBO2, RJBRr_CTR},
    {ROVRr_ROBO2, ROVRr_CTR},
    {RUNDr_ROBO2, RUNDr_CTR},
    {TBYTr_ROBO2, TBYTr_CTR},
    {RALNr_ROBO2, RALNr_CTR},
    {TBCAr_ROBO2, TBCAr_CTR},
    {TMCAr_ROBO2, TMCAr_CTR},
    {TUCAr_ROBO2, TUCAr_CTR},
    {TFCSr_ROBO2, TFCSr_CTR},
    {TXCLr_ROBO2, TXCLr_CTR},
    {TLCLr_ROBO2, TLCLr_CTR},
    {TNCLr_ROBO2, TNCLr_CTR},
    {T64r_ROBO2, T64r_CTR},
    {T127r_ROBO2, T127r_CTR},
    {T255r_ROBO2, T255r_CTR},
    {T511r_ROBO2, T511r_CTR},
    {T1023r_ROBO2, T1023r_CTR},
    {T1518r_ROBO2, T1518r_CTR},
    {T2047r_ROBO2, T2047r_CTR},
    {T4095r_ROBO2, T4095r_CTR},
    {T9216r_ROBO2, T9216r_CTR},
    {T16383r_ROBO2, T16383r_CTR},
    {R64r_ROBO2, R64r_CTR},
    {R127r_ROBO2, R127r_CTR},
    {R255r_ROBO2, R255r_CTR},
    {R511r_ROBO2, R511r_CTR},
    {R1023r_ROBO2, R1023r_CTR},
    {R1518r_ROBO2, R1518r_CTR},
    {R2047r_ROBO2, R2047r_CTR},
    {R4095r_ROBO2, R4095r_CTR},
    {R9216r_ROBO2, R9216r_CTR},
    {R16383r_ROBO2, R16383r_CTR},
    {RPOKr_ROBO2, RPOKr_CTR},
    {TPOKr_ROBO2, TPOKr_CTR},
    {RERPKTr_ROBO2, RERPKTr_CTR},
    {TOVRr_ROBO2, TOVRr_CTR},
    {TPKTr_ROBO2, TPKTr_CTR},
    {RPKTr_ROBO2, RPKTr_CTR},
/*  {TFRGr_ROBO2, TFRGr_CTR},
    {TJBRr_ROBO2, TJBRr_CTR},
*/
};

#ifdef COUNTER_THREAD
/* Per port mapping to counter map structures */
static soc_cmap_t *_port_cmap[SOC_MAX_NUM_SWITCH_DEVICES][SOC_MAX_NUM_PORTS];

#define PORT_CTR_REG(unit, port, idx) \
    (&_port_cmap[unit][port]->cmap_base[idx])
#define PORT_CTR_NUM(unit, port) \
    (_port_cmap[unit][port]->cmap_size)

#endif

/*
 * Function:
 *  soc_robo_counter_illegal
 * Purpose:
 *  Routine to display info about bad counter
 * Parameters:
 *      unit     - unit number
 *      ctr_reg  - counter register
 * Returns:
 *      None
 */

void
soc_robo_counter_illegal(int unit, soc_reg_t ctr_reg)
{
    LOG_ERROR(BSL_S_STAT,
            (BSL_META_U(unit,
            "stat counter:unit %d:ctr_reg %d (%s) is not a counter\n"),
             unit, ctr_reg,
             ( SOC_REG_IS_VALID(unit, ctr_reg) ?
             SOC_ROBO2_REG_NAME(unit, ctr_reg):"invalid")));
}


/*
 * The array to put into chip parameters.
 * This array is indexed by SOC_CTR_TYPE_xxx (soc_ctr_type_t)
 */
static soc_cmap_t counter_maps_avenger[SOC_CTR_NUM_TYPES] = {
    {NULL, 0},                                         /* FE */
    {_avenger_unimac_counters, SOC_MAX_COUNTER_NUM},   /* GE */
    {NULL, 0},                                         /* GFE */
    {NULL, 0},                                         /* HG */
    {_avenger_xlmac_counters, SOC_MAX_COUNTER_NUM},    /* XE */
    {NULL, 0},                                         /* CE */
    {NULL, 0},                                         /* CPU */
    {NULL, 0},                                         /* RX */
    {NULL, 0}                                          /* TX */
};

#ifdef COUNTER_THREAD

/*
 * Function:
 *      soc_robo_counter_cmap_init
 * Purpose:
 *      Initialize the counter map according to port types
 * Parameters:
 *      unit     - unit number
 * Returns:
 *      None
 * Notes:
 *
 */
void
soc_robo_counter_cmap_init(int unit)
{
    uint32_t  port = 0;

    /* Set up and install counter maps */
    PBMP_ITER(PBMP_ALL(unit), port){
        _port_cmap[unit][port] = NULL;
        if (IS_FE_PORT(unit, port)) {
            _port_cmap[unit][port] =
                &(counter_maps_avenger[SOC_CTR_TYPE_FE]);
        } else if (IS_GE_PORT(unit, port)) {
            _port_cmap[unit][port] =
                &(counter_maps_avenger[SOC_CTR_TYPE_GE]);
        } else if (IS_XL_PORT(unit, port)) {
            _port_cmap[unit][port] =
                &(counter_maps_avenger[SOC_CTR_TYPE_XE]);
        } else {
            _port_cmap[unit][port] = NULL;
        /*TO BE UPDATED after below macro definition is available
        } else if (IS_CPU_PORT(unit, port)) {
            // For Robo, CMIC = MII = FE.
            _port_cmap[unit][port] = NULL;
        */
        }
    }
}


/*
 * Function:
 *      soc_robo_port_cmap_get/set
 * Purpose:
 *      Access the counter map structure per port
 * Parameters:
 *      unit     - unit number
 *      port     - Port number
 * Returns:
 *      base address of the given port's counter map
 */

soc_cmap_t *
soc_robo_port_cmap_get(int unit, uint32_t port)
{
    return (_port_cmap[unit][port]);
}

int
soc_robo_port_cmap_set(int unit, uint32_t port, soc_ctr_type_t ctype)
{
    _port_cmap[unit][port] = &(counter_maps_avenger[ctype]);

    return SOC_E_NONE;
}



/*
 * Function:
 *      soc_robo_counter_collect_sync
 * Purpose:
 *      This routine gets called to sync and
 *      accumulate sw counter for specified register of port.
 * Parameters:
 *      unit - uint number.
 *      port - port number.
 *      ctr_reg - ref structure for counter register.
 *      ctr_idx - counter offset
 * Returns:
 *      SOC_E_XXX
 *
 */
static int
soc_robo_counter_collect_sync(int unit, uint32_t port, soc_reg_t ctr_reg,
                                uint32_t ctr_idx)
{
    COUNTER_ATOMIC_DEF soc = SOC_CONTROL(unit);
    uint32_t  counter_index = 0;
    uint64_t  ctr_new, ctr_prev, ctr_diff;

    counter_index = ctr_idx;
    ctr_prev = soc->counter_hw_val[counter_index];

    COMPILER_64_ZERO(ctr_new);
    if (IS_GE_PORT(unit, port)) {
        SOC_IF_ERROR_RETURN(ROBO2_REG_READ_WITH_ID(unit, ctr_reg,
                                                port, 0, &ctr_new));
    } else if (IS_XL_PORT(unit, port)) {
        SOC_IF_ERROR_RETURN(ROBO2_REG_APB2PBUS_READ_WITH_ID(unit,
                              ctr_reg, port, 0, &ctr_new));
    } else {
        return SOC_E_NOT_FOUND;
    }
    ctr_diff = ctr_new;

    if (COMPILER_64_LT(ctr_diff, ctr_prev)) {
        int             width;
        uint64_t        wrap_amt;
        /*
         * Counter must have wrapped around.
         * Add the proper wrap-around amount.
         */
        width = SOC_REG_FIELD_INFO(unit, ctr_reg)->len;

        if (width < 32) {
            COMPILER_64_SET(wrap_amt, 0, 1UL << width);
            COMPILER_64_ADD_64(ctr_diff, wrap_amt);
        } else if (width < 64) {
		    COMPILER_64_SET(wrap_amt, 1UL << (width - 32), 0);
			COMPILER_64_ADD_64(ctr_diff, wrap_amt);
		}
    }

    COMPILER_64_SUB_64(ctr_diff, ctr_prev);

    COMPILER_64_ADD_64(soc->counter_sw_val[counter_index], ctr_diff);
    soc->counter_hw_val[counter_index] = ctr_new;

    return SOC_E_NONE;
}

#endif

/*
 * Function:
 *      soc_robo_counter_get
 * Purpose:
 *      Given a counter register number and port number, fetch the
 *      64-bit software-accumulated counter value.  The software-
 *      accumulated counter value is zeroed if requested.
 * Parameters:
 *      unit - uint number.
 *      port - port number.
 *      ctr_ref - ref structure for counter register.
 *      zero - if TRUE, current counter is zeroed after reading.
 *      sync_hw - if TRUE, read from device else software accumulated value.
 *      val - (OUT) 64-bit counter value.
 * Returns:
 *      SOC_E_XXX.
 * Notes:
 *      Returns 0 if ctr_reg is INVALID_Rr.
 */
static  int
soc_robo_counter_get(int unit, uint32_t port, soc_reg_t ctr_reg,
                     int sync_hw, uint64_t *val, uint32_t ctr_idx)
{
#ifdef COUNTER_THREAD
    int                 counter_index = 0;
    COUNTER_ATOMIC_DEF  soc = SOC_CONTROL(unit);
#endif

    /* The input register is invalid */
    if (SOC_COUNTER_INVALID(unit, ctr_reg)) {
        return SOC_E_UNAVAIL;
    }
/*uncomment below code after reg file is properly updated with the counter flag*/
#if 0
    /* The input register is not counter register */
    if (!SOC_REG_IS_COUNTER(unit, ctr_reg)) {
        soc_robo_counter_illegal(unit, ctr_reg);
        COMPILER_64_ZERO(*val);
        return SOC_E_NONE;
    }
#endif
#ifdef COUNTER_THREAD
    if (unit == CBX_AVENGER_PRIMARY) {
        counter_index =(port * SOC_MAX_COUNTER_NUM) +
                        ctr_idx;
    } else {
        counter_index =((port + CBX_MAX_PORT_PER_UNIT) * SOC_MAX_COUNTER_NUM) +
                        ctr_idx;
    }

    if (sync_hw == TRUE) {
        /* Sync the software counter with hardware counter */
        soc_robo_counter_collect_sync(unit, port,
                                        ctr_reg, counter_index);
    }

    *val = soc->counter_sw_val[counter_index];
    return SOC_E_NONE;

#else
    if (IS_GE_PORT(unit, port)) {
        SOC_IF_ERROR_RETURN(ROBO2_REG_READ_WITH_ID(unit, ctr_reg, port, 0, val));
    } else if (IS_XL_PORT(unit, port)) {
        SOC_IF_ERROR_RETURN(ROBO2_REG_APB2PBUS_READ_WITH_ID(unit, ctr_reg, port, 0, val));
    } else {
        return SOC_E_NOT_FOUND;
    }
    return SOC_E_NONE;
#endif
}

/*
 * Function:
 *      soc_robo_counter_set
 * Purpose:
 *      Given a counter register number, port number, and a counter
 *      value, set both the hardware and software-accumulated counters
 *      to the value.
 * Parameters:
 *      unit - uint number.
 *      port - port number.
 *      ctr_reg - counter register to retrieve.
 *      val - 64/32-bit counter value.
 *      ctr_idx - counter offset
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      Use the value 0 to clear counters.
 */
static int
soc_robo_counter_set(int unit, uint32_t port, soc_reg_t ctr_reg,
                     uint64_t val, uint32_t ctr_idx)
{
#ifdef COUNTER_THREAD
    int                 counter_index = 0;
    COUNTER_ATOMIC_DEF  soc = SOC_CONTROL(unit);
#endif

    /* The input register is invalid */
    if (SOC_COUNTER_INVALID(unit, ctr_reg)) {
        return SOC_E_UNAVAIL;
    }

/*uncomment below code after reg file is properly updated with the counter flag*/
#if 0
    /* The input register is not counter register */
    if (!SOC_REG_IS_COUNTER(unit, ctr_reg)) {
        soc_robo_counter_illegal(unit, ctr_reg);
        return SOC_E_NONE;
    }
#endif

#ifdef COUNTER_THREAD
    if (unit == CBX_AVENGER_PRIMARY) {
        counter_index =(port * SOC_MAX_COUNTER_NUM) +
                        ctr_idx;
    } else {
        counter_index =((port + CBX_MAX_PORT_PER_UNIT) * SOC_MAX_COUNTER_NUM) +
                        ctr_idx;
    }

    /* Update S/W and H/W counter */
    soc->counter_sw_val[counter_index] = val;
    soc->counter_hw_val[counter_index] = val;
#endif
    if (IS_GE_PORT(unit, port)) {
        SOC_IF_ERROR_RETURN(ROBO2_REG_WRITE_WITH_ID(unit, ctr_reg, port, 0, &val));
    } else if (IS_XL_PORT(unit, port)) {
        SOC_IF_ERROR_RETURN(ROBO2_REG_APB2PBUS_WRITE_WITH_ID(unit, ctr_reg, port, 0, &val));
    } else {
        return SOC_E_NOT_FOUND;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_robo_counter_set_by_port
 * Purpose:
 *      Sets all 64-bit software shadow counters for a given
 *  set of ports to a given 64-bit value.
 * Parameters:
 *      unit - RoboSwitch unit number.
 *      pbm  - Port bitmap of the ports to set. Use PBMP_ALL for all ports.
 *      val  - 64-bit value to set the counters to.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_robo_counter_set_by_port(int unit, uint32_t port, uint64_t val)
{
    int             i;
    soc_ctr_ref_t   *ctr_ref = NULL;
    soc_cmap_t *port_cmap = NULL;
    if (port >= CBX_PORT_MAX) {
        return SOC_E_PORT;
    }
/*
   *If CBX_PORT_MAX is updated with 16/32  based on cascade mode,
   *below check can be removed
*/
    if((!SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) &&
       (port >= CBX_MAX_PORT_PER_UNIT )) {
        return SOC_E_PORT;
    }
    if ((port >= CBX_MAX_PORT_PER_UNIT)) {
        port = port - CBX_MAX_PORT_PER_UNIT ;
        unit = CBX_AVENGER_SECONDARY;
    } else {
        unit = CBX_AVENGER_PRIMARY;
    }
    if (IS_FE_PORT(unit, port)) {
        port_cmap=&(counter_maps_avenger[SOC_CTR_TYPE_FE]);
    } else if (IS_GE_PORT(unit, port)) {
        port_cmap =
                &(counter_maps_avenger[SOC_CTR_TYPE_GE]);
    } else if (IS_XL_PORT(unit, port)) {
        port_cmap =
                &(counter_maps_avenger[SOC_CTR_TYPE_XE]);
    }
    if(!port_cmap || !(port_cmap->cmap_base)) {
        return SOC_E_NOT_FOUND;
    }
    for (i = 0; i < SOC_MAX_COUNTER_NUM; i++) {
        ctr_ref = &port_cmap->cmap_base[i];
        if (ctr_ref->reg != INDEX(INVALID_Rr)) {
            SOC_IF_ERROR_RETURN
                (soc_robo_counter_set(unit, port, ctr_ref->reg, val,
                                        ctr_ref->index));
        }
    }
    return SOC_E_NONE;
}



/*
 * Function:
 *      soc_robo_counter_collect
 * Purpose:
 *      This routine gets called each time the counter transfer has
 *      completed a cycle.
 * Parameters:
 *      unit - uint number.
 *      discard - If true, the software counters are not updated; this
 *              results in only synchronizing the previous hardware
 *              count buffer.
 * Returns:
 *      SOC_E_XXX

 */
#ifdef COUNTER_THREAD
int
soc_robo_counter_collect(int unit, int discard)
{
    COUNTER_ATOMIC_DEF   soc = SOC_CONTROL(unit);
    int                  counter_index = 0;
    uint32_t             i = 0;
    uint32_t             port, internal_port, internal_unit;
    uint64_t             ctr_new, ctr_prev, ctr_diff;
    soc_ctr_ref_t        *ctr_ref;
    PBMP_ITER(soc->counter_pbmp, port) {

#ifdef CONFIG_EXTERNAL_HOST
        /*
         * With XMC card, a crash is observed for ports 14 and above.
         * Remove this after fix
         */
         if (((port >= 14) && (port <= 16)) || (port >=30)) {
             continue;
         }
#endif
        if((port >= CBX_MAX_PORT_PER_UNIT) && (port < CBX_PORT_MAX)) {
            internal_port = port - CBX_MAX_PORT_PER_UNIT ;
            internal_unit = CBX_AVENGER_SECONDARY;
        } else {
            internal_port = port;
            internal_unit = CBX_AVENGER_PRIMARY;
        }
        for (i = 0; i < PORT_CTR_NUM(internal_unit, internal_port); i++) {
            if (_port_cmap[internal_unit][internal_port] == NULL) {
                break;
            }
            ctr_ref = PORT_CTR_REG(internal_unit, internal_port, i);
            if (ctr_ref->reg == INDEX(INVALID_Rr)) {
                continue;
            }

            if (internal_unit == CBX_AVENGER_PRIMARY) {
                counter_index =(internal_port * SOC_MAX_COUNTER_NUM) +
                                ctr_ref->index;
            } else {
                counter_index =((internal_port + CBX_MAX_PORT_PER_UNIT) *
                                 SOC_MAX_COUNTER_NUM) + ctr_ref->index;
            }
            ctr_prev = soc->counter_hw_val[counter_index];
            COMPILER_64_ZERO(ctr_new);
            if (IS_GE_PORT(internal_unit, internal_port)) {
                SOC_IF_ERROR_RETURN(ROBO2_REG_READ_WITH_ID(internal_unit,
                                                         ctr_ref->reg,
                                                internal_port, 0, &ctr_new));
            } else if (IS_XL_PORT(internal_unit, internal_port)) {
                SOC_IF_ERROR_RETURN(ROBO2_REG_APB2PBUS_READ_WITH_ID(
                                    internal_unit,
                                    ctr_ref->reg,
                                    internal_port, 0, &ctr_new));
            } else {
                break;
            }

            if (discard) {
                /* Update the previous value buffer */
                soc->counter_hw_val[counter_index] = ctr_new;
                continue;
            }

            ctr_diff = ctr_new;

            if (COMPILER_64_LT(ctr_diff, ctr_prev)) {
                uint64          wrap_amt;
                int             width = 0;
                /*
                 * Counter must have wrapped around.
                 * Add the proper wrap-around amount.
                 */
                width = SOC_REG_FIELD_INFO(unit, ctr_ref->reg)->len;
                if (width < 32) {
                    COMPILER_64_SET(wrap_amt, 0, 1UL << width);
                    COMPILER_64_ADD_64(ctr_diff, wrap_amt);
                } else if (width < 64) {
		            COMPILER_64_SET(wrap_amt, 1UL << (width - 32), 0);
			        COMPILER_64_ADD_64(ctr_diff, wrap_amt);
		        }
            }

            COMPILER_64_SUB_64(ctr_diff, ctr_prev);

            COMPILER_64_ADD_64(soc->counter_sw_val[counter_index], ctr_diff);
            soc->counter_hw_val[counter_index] = ctr_new;
        }
    }
    return SOC_E_NONE;
}

static int counter_thread_run = 1;
/*
 * Starts or stops the counter thread
 */
void
soc_robo_counter_thread_run_set(int run)
{
    counter_thread_run = run;
}

/*
 * Function:
 *      soc_robo_counter_thread
 * Purpose:
 *      Master counter collection and accumulation thread.
 * Parameters:
 *      args  -  (as a void *).
 * Returns:
 *      Nothing, does not return.
 */
void
soc_robo_counter_thread(void *args)
{
    int                unit = CBX_AVENGER_PRIMARY;
    COUNTER_ATOMIC_DEF soc = SOC_CONTROL(unit);
    int                rv;
    int                interval;
    int                sync_gnt = FALSE;

    /*
     * There's a race condition since the PID is used as a
     * signal of whether the counter thread is running.  We sleep
     * here to make sure it gets initialized properly in SOC_CONTROL
     * by the thread_create return.
     */

    sal_sleep(1);
    LOG_INFO(BSL_LS_SOC_COUNTER,
             (BSL_META_U(unit,
                         "soc_robo_counter_thread: unit=%d\n"), unit));


    /*
     * Create a semaphore used to time the trigger scans.
     */
    soc->counter_notify = sal_sem_create("ctr-notify", sal_sem_BINARY, 0);

    if (soc->counter_notify == NULL) {

        LOG_ERROR(BSL_LS_SOC_COMMON,(BSL_META_U(unit,
                         "soc_robo_counter_thread: semaphore init failed\n")));

        rv = SOC_E_INTERNAL;
        goto done;
    }
    while ((interval = soc->counter_interval) != 0) {

        /*
         * Use a semaphore timeout instead of a sleep to achieve the
         * desired delay between scans.  This allows this thread to exit
         * immediately when soc_robo_counter_stop wants it to.
         */

        LOG_VERBOSE(BSL_LS_SOC_COUNTER,
                    (BSL_META_U(unit,"soc_robo_counter_thread: sleep %d\n"),
                    interval));
/* modify 10 to interval once tested with actual device.with emulator(qt16) use 4*/
        (void)sal_sem_take(soc->counter_notify, interval);
        if (soc->counter_interval == 0) {   /* Exit signalled */
            break;
        }

        if ( !counter_thread_run )
            continue;

        if (soc->counter_sync_req) {
            sync_gnt = TRUE;
        }


        if ((rv = soc_robo_counter_collect(unit, 0)) < 0) {
            LOG_ERROR(BSL_LS_SOC_COMMON,(BSL_META_U(unit,
                        "soc_robo_counter_thread: collect failed. \n")));
        }

        if(SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)){
            if ((rv = soc_robo_counter_collect(CBX_AVENGER_SECONDARY, 0)) < 0) {
                LOG_ERROR(BSL_LS_SOC_COMMON,(BSL_META_U(unit,
                            "soc_robo_counter_thread: collect failed. \n")));
            }
        }

        if (sync_gnt) {
            soc->counter_sync_req = 0;
            sync_gnt = 0;
        }
    }

    rv = SOC_E_NONE;

 done:
    if (rv < 0) {

        LOG_ERROR(BSL_LS_SOC_COMMON,(BSL_META_U(unit,
                    "soc_robo_counter_thread: Operation failed - exiting\n")));
    }


    LOG_INFO(BSL_LS_SOC_COUNTER,
             (BSL_META_U(unit,
                         "soc_robo_counter_thread: exiting\n")));

    soc->counter_interval = 0;
    soc->counter_pid = SAL_THREAD_ERROR;
    sal_thread_exit(0);
}


/*
 * Function:
 *      soc_robo_counter_start
 * Purpose:
 *      Perform counter MIB Autocast copying counters to memory.
 * Parameters:
 *      unit      - RoboSwitch unit number.
 *      flags    - Mode settings:
 *         0: Use polled mode for collecting statistics.
 *                 Others: SOC_COUNTER_F_DMA
 *      interval - Hardware counter scan interval in microseconds, maximum
 *         1048575.
 *      pbmp     - Port bitmap of the ports to monitor. Use PBMP_ALL for
 *         all ports.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_robo_counter_start(int unit, uint32_t flags, int interval, pbmp_t pbmp)
{
    COUNTER_ATOMIC_DEF soc = SOC_CONTROL(unit);
    sal_sem_t          sem;

    LOG_INFO(BSL_LS_SOC_COUNTER,
             (BSL_META_U(unit,
                         "soc_robo_counter_start: unit=%d flags=0x%x "
                         "interval=%d pbmp=%d\n"),
              unit, flags, interval, pbmp));

    /*
     * If the following SAL_ASSERT triggers, 'unsigned long long' must not
     * be 64 bits on your platform.  You may need to fix compiler.h to
     * the correct 64 bit type or disable COMPILER_HAS_LONGLONG.
     */

    SAL_ASSERT(sizeof(uint64) == 8);

    /* Stop if already running */

    if (soc->counter_pid != SAL_THREAD_ERROR) {
        SOC_IF_ERROR_RETURN(soc_robo_counter_stop(unit));
    }
    /* Configure */
    sal_sprintf(soc->counter_name, "tCOUNTER.%d", unit);

    /* Create fresh semaphores */

    if ((sem = soc->counter_notify) != NULL) {
        soc->counter_notify = NULL;    /* Stop others from waking sem */
        sal_sem_destroy(sem);           /* Then destroy it */
    }
    soc->counter_pbmp = (pbmp & (PBMP_ALL(CBX_AVENGER_PRIMARY)));
    soc->counter_flags = flags;

    /* Initialize pbmp in secondary avenger soc control structure */
    if(SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)){
        soc = SOC_CONTROL(CBX_AVENGER_SECONDARY);
        soc->counter_pbmp = (pbmp &
                 (PBMP_ALL(CBX_AVENGER_SECONDARY) << CBX_MAX_PORT_PER_UNIT));
        soc->counter_flags = flags;
        /* Revert back soc pointer to primary avenger */
        soc = SOC_CONTROL(unit);
    }

    /* Synchronize counter 'prev' values with current hardware counters */

    SOC_IF_ERROR_RETURN(soc_robo_counter_collect(unit, 1));

    if(SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)){
        SOC_IF_ERROR_RETURN(soc_robo_counter_collect(CBX_AVENGER_SECONDARY, 1));
    }

    /*
     * The hardware timer can only be used for intervals up to about
     * 1.048 seconds.  This implementation uses a software timer instead
     * of the hardware timer.
     */


    soc->counter_interval = interval;
    if (interval) {
        soc->counter_pid =
            sal_thread_create(soc->counter_name,
                              1024,
                              50,
                              soc_robo_counter_thread, NULL);

        if (soc->counter_pid == SAL_THREAD_ERROR) {

            LOG_ERROR(BSL_LS_SOC_COMMON,(BSL_META_U(unit,
                        "soc_robo_counter_start: Thread create failed\n")));

            return (SOC_E_MEMORY);
        }

        LOG_INFO(BSL_LS_SOC_COUNTER,
                 (BSL_META_U(unit,
                             "soc_robo_counter_start: Complete\n")));

    } else {
        soc->counter_pid = SAL_THREAD_ERROR;

        LOG_INFO(BSL_LS_SOC_COUNTER,
                 (BSL_META_U(unit,
                             "soc_robo_counter_start: Inactive\n")));

    }
    return (SOC_E_NONE);

}

/*
 * Function:
 *  soc_robo_counter_sync
 * Purpose:
 *  Force an immediate counter update
 * Parameters:
 *  unit - uint number.
 * Returns:
 *  SOC_E_XXX
 * Notes:
 *  Ensures that ALL counter activity that occurred before the sync
 *  is reflected in the results of any soc_robo_counter_get()-type
 *  routine that is called after the sync.
 */

int
soc_robo_counter_sync(int unit)
{
    COUNTER_ATOMIC_DEF soc = SOC_CONTROL(unit);
    int                interval;

    if ((interval = soc->counter_interval) == 0) {
        return SOC_E_DISABLED;
    }

    /* Trigger a collection */

    soc->counter_sync_req = TRUE;

    sal_sem_give(soc->counter_notify);
    sal_sleep(1);
/* check the reqd sleep time in actual device and update
    sal_sleep(CBX_MAX_PORT_PER_UNIT);
*/
    /* The sleep value should be defined as meanningful for port number*/

    if (soc->counter_sync_req) {
        LOG_ERROR(BSL_LS_SOC_COMMON,(BSL_META_U(unit,
                "soc_robo_counter_sync: counter thread not responding\n")));

        soc->counter_sync_req = FALSE;
        return SOC_E_INTERNAL;
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_robo_counter_stop
 * Purpose:
 *      Cancel counter MIB Autocast and software counter collection.
 * Parameters:
 *      unit - RoboSwitch unit number.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_robo_counter_stop(int unit)
{
    COUNTER_ATOMIC_DEF soc = SOC_CONTROL(unit);
    int                rv = SOC_E_NONE;

    LOG_INFO(BSL_LS_SOC_COUNTER,
             (BSL_META_U(unit,
                         "soc_robo_counter_stop: unit=%d\n"), unit));

    /* Stop thread if present.  It notices interval becoming 0. */
    soc->counter_interval = 0;
    if (soc->counter_pid != SAL_THREAD_ERROR) {

        // Wake up thread to speed its exit //
        if (soc->counter_notify) {
            sal_sem_give(soc->counter_notify);
        }

        /* Give thread a few seconds to wake up and exit */
        sal_sleep(1);
        if (soc->counter_pid != SAL_THREAD_ERROR) {
            LOG_ERROR(BSL_LS_SOC_COMMON,(BSL_META_U(unit,
                           "soc_robo_counter_stop: thread will not exit\n")));
            rv = SOC_E_INTERNAL;
        }
    }

    LOG_INFO(BSL_LS_SOC_COUNTER,
             (BSL_META_U(unit,
                         "soc_robo_counter_stop: stopped\n")));
    return (rv);
}

#endif
/*
 * Function:
 *      soc_robo2_counter_attach
 * Purpose:
 *      Initialize counter module.
 * Notes:
 *      resets counter collection buffers prior to use
 */
int
soc_robo2_counter_attach(int unit)
{
#ifdef COUNTER_THREAD
    COUNTER_ATOMIC_DEF soc;
    int                i;
#endif
    SAL_ASSERT(SOC_UNIT_VALID(unit));
    if (!SOC_UNIT_VALID(unit)) {
        return SOC_E_UNIT;
    }

#ifdef COUNTER_THREAD
    soc = SOC_CONTROL(unit);
    soc->counter_pid = SAL_THREAD_ERROR;
    soc->counter_notify = NULL;
    /* clear SW counter table*/
    for(i = 0;i < SW_COUNT_ARR;i++)
    {
        soc->counter_sw_val[i] = 0;
    }
    /* clear HW counter table*/
    for(i = 0;i < SW_COUNT_ARR;i++)
    {
        soc->counter_hw_val[i] = 0;
    }

    soc->counter_interval = 0;
    SOC_PBMP_CLEAR(soc->counter_pbmp);


    soc_robo_counter_cmap_init(unit);

#endif
    return SOC_E_NONE;

}

/*
 * Function:
 *      soc_robo2_counter_detach
 * Purpose:
 *      Finalize counter module.
 * Notes:
 *      Stops counter task if running.
 */
int
soc_robo2_counter_detach(int unit)
{
    SAL_ASSERT(SOC_UNIT_VALID(unit));
    if (!SOC_UNIT_VALID(unit)) {
        return SOC_E_UNIT;
    }

#ifdef COUNTER_THREAD
    if(unit == CBX_AVENGER_SECONDARY){
        return SOC_E_DISABLED;
    }

    SOC_IF_ERROR_RETURN(soc_robo_counter_stop(unit));

#endif
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_robo_counter_reset
 * Purpose:
 *      Resets all the counter registers.
 * Notes:
 *      Resets all the MIB registers used for port based stats.
 *
 */
int
soc_robo_counter_reset(uint32_t unit)
{
    uint32_t    port = 0;
    uint32_t    global_port = 0;
    uint64_t    ctrreset = 0;
    PBMP_ITER(PBMP_ALL(unit), port) {
        global_port = port;
        if(unit == CBX_AVENGER_SECONDARY) {
            global_port += CBX_MAX_PORT_PER_UNIT;
        }
        soc_robo_counter_set_by_port(unit, global_port, ctrreset);
    }
    return SOC_E_NONE;
}

/*
  *  Function : counter_thread_set
  *
  *  Purpose :
  *      Set the operation code of the counter thread.
  *
  *  Parameters :
  *      thread_op   :   thread op code.
  *      uint    :   uint number.
  *      flags        :   mode settings.
  *      interval   :   counters scan interval in microseconds.
  *      bmp  :   port bitmap.
  *
  *  Return :
  *      SOC_E_XXX
  *
  *  Note :
  *
  *
  */
int
counter_thread_set(int unit, counter_thread_t thread_op, uint32 flags,
                       int interval, soc_pbmp_t bmp)
{
    int  rv = SOC_E_NONE;
#ifdef COUNTER_THREAD

    switch (thread_op) {
        case COUNTER_THREAD_START:
            rv = soc_robo_counter_start(unit, flags, interval, bmp);
            break;
        case COUNTER_THREAD_STOP:
            rv = soc_robo_counter_stop(unit);
            break;
        case COUNTER_THREAD_SYNC:
            rv = soc_robo_counter_sync(unit);
            break;
        default:
            rv = SOC_E_PARAM;
    }
#endif
    return rv;
 }


/*
 *  Function : counter_set
 *
 *  Purpose :
 *      Set the snmp counter value.
 *
 *  Parameters :
 *      uint           :   uint number.
 *      port           :   port bitmap.
 *      counter_type   :   counter_type.
 *                  If want to clear all counters associated to this port,
 *                  counter_type = SET_PORT
 *      val            :   counter val.
 *              Now only support to set zero.
 *
 *  Return :
 *      SOC_E_XXX
 *
 *  Note :
 *
 *
 */
int
counter_set(int unit,  uint32_t port,
                cbx_port_stat_counter_t counter_type, uint64_t val)
{
    int       rv = SOC_E_NONE;
    uint64_t  val_lo, val_hi;

    if (!COMPILER_64_IS_ZERO(val)) {
        return SOC_E_UNAVAIL;
    }
    if (port >= CBX_PORT_MAX) {
        return SOC_E_PORT;
    }
/*
   *If CBX_PORT_MAX is updated with 16/32  based on cascade mode,
   *below check can be removed
*/
    if((!SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) &&
           (port >= CBX_MAX_PORT_PER_UNIT )) {
        return SOC_E_PORT;
    }
    if((port >= CBX_MAX_PORT_PER_UNIT )) {
        port = port - CBX_MAX_PORT_PER_UNIT;
        unit = CBX_AVENGER_SECONDARY;
    } else {
        unit = CBX_AVENGER_PRIMARY;
    }

    if (IS_GE_PORT(unit, port)) {
        switch (counter_type) {
            /* *** RFC 1213 *** */
            case CBX_PORT_STAT_IF_IN_OCTETS:
                val_lo = (val & LOW32_MASK);
                val_hi = ((val & HIGH32_MASK) >> RS32);

                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_RX_ALL_OCTETS_LOr), val_lo,
                            MIB_RX_ALL_OCTETS_LOr_CTR));
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_RX_ALL_OCTETS_HIr), val_hi,
                            MIB_RX_ALL_OCTETS_HIr_CTR));
                break;

            case CBX_PORT_STAT_IF_IN_UCAST_PKTS:
                /* Total non-error frames minus broadcast/multicast frames */
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_RXUNICASTPKTSr), val,
                            MIB_RXUNICASTPKTSr_CTR));
                break;

            case CBX_PORT_STAT_IF_IN_NON_UCAST_PKTS:
                /* All frames minus unicast frames */
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_RXUNICASTPKTSr), val,
                            MIB_RXUNICASTPKTSr_CTR));
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_RX_ALL_PKTSr), val,
							MIB_RX_ALL_PKTSr_CTR));
                break;

            case CBX_PORT_STAT_IF_IN_DISCARDS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_RX_DROPr), val, MIB_RX_DROPr_CTR));
                break;

            case CBX_PORT_STAT_IF_IN_ERRORS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_RX_CRC_ALIGNr), val,
                            MIB_RX_CRC_ALIGNr_CTR));
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_RX_FRAGr), val, MIB_RX_FRAGr_CTR));
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_RX_OVRr), val, MIB_RX_OVRr_CTR));
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_RX_UNDr), val, MIB_RX_UNDr_CTR));
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_RX_SYMr), val, MIB_RX_SYMr_CTR));
                break;

            case CBX_PORT_STAT_IF_IN_UNKNOWN_PROTOS:
                return SOC_E_UNAVAIL;
                break;

            case CBX_PORT_STAT_IF_IN_BROADCAST_PKTS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_RX_BRDCASTr), val, MIB_RX_BRDCASTr_CTR));
                break;

            case CBX_PORT_STAT_IF_IN_MULTICAST_PKTS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_RX_MULTr), val, MIB_RX_MULTr_CTR));
                break;

            case CBX_PORT_STAT_IF_IN_VLAN_DISCARDS:
                return SOC_E_UNAVAIL;
                break;

            case CBX_PORT_STAT_IF_OUT_OCTETS:
                val_lo = (val & LOW32_MASK);
                val_hi = ((val & HIGH32_MASK) >> RS32);

                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_TX_ALL_OCTETS_LOr), val_lo,
                            MIB_TX_ALL_OCTETS_LOr_CTR));
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_TX_ALL_OCTETS_HIr), val_hi,
                            MIB_TX_ALL_OCTETS_HIr_CTR));
                break;

            case CBX_PORT_STAT_IF_OUT_UCAST_PKTS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_TXUNICASTPKTr), val,
                            MIB_TXUNICASTPKTr_CTR));
                break;

            case CBX_PORT_STAT_IF_OUT_NON_UCAST_PKTS:
                /* all frames minus unicast frames */
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_TXUNICASTPKTr), val,
                            MIB_TXUNICASTPKTr_CTR));
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_TX_ALL_PKTSr), val,
							MIB_TX_ALL_PKTSr_CTR));
                break;

            case CBX_PORT_STAT_IF_OUT_DISCARDS:
                return SOC_E_UNAVAIL;
                break;

            case CBX_PORT_STAT_IF_OUT_ERRORS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_TX_EX_COLr), val, MIB_TX_EX_COLr_CTR));
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_TX_LATEr), val, MIB_TX_LATEr_CTR));
                break;

            case CBX_PORT_STAT_IF_OUT_QLEN:  /* robo not suppport */
                return SOC_E_UNAVAIL;
                break;

            case CBX_PORT_STAT_IF_OUT_BROADCAST_PKTS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_TX_BRDCASTr), val, MIB_TX_BRDCASTr_CTR));
                break;

            case CBX_PORT_STAT_IF_OUT_MULTICAST_PKTS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_TX_MULTr), val, MIB_TX_MULTr_CTR));
                break;

             /* *** RFC 1757 *** */
            case CBX_PORT_STAT_ETHER_STATS_DROP_EVENTS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_RX_DROPr), val, MIB_RX_DROPr_CTR));
                break;

            case CBX_PORT_STAT_ETHER_STATS_MULTICAST_PKTS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_TX_MULTr), val, MIB_TX_MULTr_CTR));
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_RX_MULTr), val, MIB_RX_MULTr_CTR));
                break;

            case CBX_PORT_STAT_ETHER_STATS_BROADCAST_PKTS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_RX_BRDCASTr), val, MIB_RX_BRDCASTr_CTR));
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_TX_BRDCASTr), val, MIB_TX_BRDCASTr_CTR));
                break;

            case CBX_PORT_STAT_ETHER_STATS_UNDERSIZE_PKTS:
                /* Undersize frames-as per RFC, use only count from RX */
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_RX_UNDr), val, MIB_RX_UNDr_CTR));
                break;

            case CBX_PORT_STAT_ETHER_STATS_FRAGMENTS:
                /* Fragment frames-as per RFC, use only count from RX */
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_RX_FRAGr), val, MIB_RX_FRAGr_CTR));
                break;

            case CBX_PORT_STAT_ETHER_STATS_PKTS_64_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_RX_64r), val, MIB_RX_64r_CTR));
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_TX_64r), val, MIB_TX_64r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_STATS_PKTS_65_TO_127_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_RX_65_127r), val, MIB_RX_65_127r_CTR));
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_TX_65_127r), val, MIB_TX_65_127r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_STATS_PKTS_128_TO_255_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_RX_128_255r), val, MIB_RX_128_255r_CTR));
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_TX_128_255r), val, MIB_TX_128_255r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_STATS_PKTS_256_TO_511_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_RX_256_511r), val, MIB_RX_256_511r_CTR));
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_TX_256_511r), val, MIB_TX_256_511r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_STATS_PKTS_512_TO_1023_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_RX_512_1023r), val,
                            MIB_RX_512_1023r_CTR));
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_TX_512_1023r), val,
                            MIB_TX_512_1023r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_STATS_PKTS_1024_TO_1522_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_RX_1024_1522r), val,
                            MIB_RX_1024_1522r_CTR));
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_TX_1024_1522r), val,
                            MIB_TX_1024_1522r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_STATS_PKTS_1523_TO_2047_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_RX_1523_2047r), val,
                            MIB_RX_1523_2047r_CTR));
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_TX_1523_2047r), val,
                            MIB_TX_1523_2047r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_STATS_PKTS_2048_TO_4095_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_RX_2048_4095r), val,
                            MIB_RX_2048_4095r_CTR));
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_TX_2048_4095r), val,
                            MIB_TX_2048_4095r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_STATS_PKTS_4096_TO_8191_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_RX_4096_8191r), val,
                            MIB_TX_4096_8191r_CTR));
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_RX_4096_8191r), val,
                            MIB_TX_4096_8191r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_STATS_PKTS_8192_TO_MAX_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_RX_8192_MAXr), val,
                            MIB_RX_8192_MAXr_CTR));
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_TX_8192_MAXr), val,
                            MIB_TX_8192_MAXr_CTR));
                break;

            case CBX_PORT_STAT_ETHER_STATS_OVERSIZE_PKTS:
                /* oversize frames-as per RFC, use only count from RX */
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_RX_OVRr), val, MIB_RX_OVRr_CTR));
                break;

            case CBX_PORT_STAT_ETHER_RX_OVERSIZE_PKTS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_RX_OVRr), val, MIB_RX_OVRr_CTR));
                break;

            case CBX_PORT_STAT_ETHER_TX_OVERSIZE_PKTS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_TX_OVERr), val, MIB_TX_OVERr_CTR));
                break;

            case CBX_PORT_STAT_ETHER_STATS_JABBERS:
                /* jabber frames-as per RFC, use only count from RX */
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_RX_JABr), val, MIB_RX_JABr_CTR));
                break;

            case CBX_PORT_STAT_ETHER_STATS_OCTETS:
                val_lo = (val & LOW32_MASK);
                val_hi = ((val & HIGH32_MASK) >> RS32);
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_RX_ALL_OCTETS_LOr), val_lo,
                            MIB_RX_ALL_OCTETS_LOr_CTR));
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_RX_ALL_OCTETS_HIr), val_hi,
                            MIB_RX_ALL_OCTETS_HIr_CTR));
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_TX_ALL_OCTETS_LOr), val_lo,
                            MIB_TX_ALL_OCTETS_LOr_CTR));
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_TX_ALL_OCTETS_HIr), val_hi,
                            MIB_TX_ALL_OCTETS_HIr_CTR));
                break;

            case CBX_PORT_STAT_ETHER_STATS_PKTS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_TX_ALL_PKTSr), val,
                            MIB_TX_ALL_PKTSr_CTR));
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_RX_ALL_PKTSr), val,
                            MIB_RX_ALL_PKTSr_CTR));
                break;

            case CBX_PORT_STAT_ETHER_STATS_COLLISIONS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_TX_COLr), val, MIB_TX_COLr_CTR));
                break;

            case CBX_PORT_STAT_ETHER_STATS_CRC_ALIGN_ERRORS:
                /* CRC errors + alignment errors */
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_RX_CRC_ALIGNr), val,
                            MIB_RX_CRC_ALIGNr_CTR));
                break;


            case CBX_PORT_STAT_ETHER_STATS_TX_NO_ERRORS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_TX_GD_PKTSr), val, MIB_TX_GD_PKTSr_CTR));
                break;

            case CBX_PORT_STAT_ETHER_STATS_RX_NO_ERRORS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_RX_GD_PKTSr), val, MIB_RX_GD_PKTSr_CTR));
                break;

                /* IPV4 */
            case CBX_PORT_STAT_IP_IN_RECEIVES:
            case CBX_PORT_STAT_IP_IN_OCTETS:
            case CBX_PORT_STAT_IP_IN_UCAST_PKTS:
            case CBX_PORT_STAT_IP_IN_NON_UCAST_PKTS:
            case CBX_PORT_STAT_IP_IN_DISCARDS:
            case CBX_PORT_STAT_IP_OUT_OCTETS:
            case CBX_PORT_STAT_IP_OUT_UCAST_PKTS:
            case CBX_PORT_STAT_IP_OUT_NON_UCAST_PKTS:
            case CBX_PORT_STAT_IP_OUT_DISCARDS:
                /*robo not suppport */
                return SOC_E_UNAVAIL;
                break;

                /*IPV6*/
            case CBX_PORT_STAT_IPV6_IN_RECEIVES:
            case CBX_PORT_STAT_IPV6_IN_OCTETS:
            case CBX_PORT_STAT_IPV6_IN_UCAST_PKTS:
            case CBX_PORT_STAT_IPV6_IN_NON_UCAST_PKTS:
            case CBX_PORT_STAT_IPV6_IN_MCAST_PKTS:
            case CBX_PORT_STAT_IPV6_IN_DISCARDS:
            case CBX_PORT_STAT_IPV6_OUT_OCTETS:
            case CBX_PORT_STAT_IPV6_OUT_UCAST_PKTS:
            case CBX_PORT_STAT_IPV6_OUT_NON_UCAST_PKTS:
            case CBX_PORT_STAT_IPV6_OUT_MCAST_PKTS:
            case CBX_PORT_STAT_IPV6_OUT_DISCARDS:
                /*robo not suppport */
                return SOC_E_UNAVAIL;
                break;

            case CBX_PORT_STAT_GREEN_DISCARD_DROPPED_PACKETS:
            case CBX_PORT_STAT_GREEN_DISCARD_DROPPED_BYTES:
            case CBX_PORT_STAT_YELLOW_DISCARD_DROPPED_PACKETS:
            case CBX_PORT_STAT_YELLOW_DISCARD_DROPPED_BYTES:
            case CBX_PORT_STAT_RED_DISCARD_DROPPED_PACKETS:
            case CBX_PORT_STAT_RED_DISCARD_DROPPED_BYTES:
            case CBX_PORT_STAT_DISCARD_DROPPED_PACKETS:
            case CBX_PORT_STAT_DISCARD_DROPPED_BYTES:
                /*robo not suppport */
                return SOC_E_UNAVAIL;
                break;

            /* Broadcom specific*/
            case CBX_PORT_STAT_ETHER_IN_PKTS_64_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_RX_64r), val, MIB_RX_64r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_IN_PKTS_65_TO_127_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_RX_65_127r), val, MIB_RX_65_127r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_IN_PKTS_128_TO_255_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_RX_128_255r), val, MIB_RX_128_255r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_IN_PKTS_256_TO_511_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_RX_256_511r), val, MIB_RX_256_511r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_IN_PKTS_512_TO_1023_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_RX_512_1023r), val,
                            MIB_RX_512_1023r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_IN_PKTS_1024_TO_1522_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_RX_1024_1522r), val,
                            MIB_RX_1024_1522r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_IN_PKTS_1523_TO_2047_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_RX_1523_2047r), val,
                            MIB_RX_1523_2047r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_IN_PKTS_2048_TO_4095_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_RX_2048_4095r), val,
                            MIB_RX_2048_4095r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_IN_PKTS_4096_TO_8191_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_RX_4096_8191r), val,
                            MIB_RX_4096_8191r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_IN_PKTS_8192_TO_MAX_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_RX_8192_MAXr), val,
                            MIB_RX_8192_MAXr_CTR));
                break;

            case CBX_PORT_STAT_ETHER_OUT_PKTS_64_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_TX_64r), val, MIB_TX_64r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_OUT_PKTS_65_TO_127_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_TX_65_127r), val, MIB_TX_65_127r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_OUT_PKTS_128_TO_255_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_TX_128_255r), val, MIB_TX_128_255r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_OUT_PKTS_256_TO_511_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_TX_256_511r), val, MIB_TX_256_511r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_OUT_PKTS_512_TO_1023_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_TX_512_1023r), val,
                            MIB_TX_512_1023r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_OUT_PKTS_1024_TO_1522_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_TX_1024_1522r), val,
                            MIB_TX_1024_1522r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_OUT_PKTS_1523_TO_2047_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_TX_1523_2047r),val,
                            MIB_TX_1523_2047r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_OUT_PKTS_2048_TO_4095_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_TX_2048_4095r), val,
                            MIB_TX_2048_4095r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_OUT_PKTS_4096_TO_8191_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_TX_4096_8191r), val,
                            MIB_TX_4096_8191r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_OUT_PKTS_8192_TO_MAX_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(MIB_TX_8192_MAXr), val,
                            MIB_TX_8192_MAXr_CTR));
                break;

            default:

                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                            "counter_set:Statistic not supported:%d\n"),
                            counter_type));

                return SOC_E_PARAM;
        }
    } else if (IS_XL_PORT(unit, port)) {
        switch (counter_type) {
            /* *** RFC 1213 *** */
            case CBX_PORT_STAT_IF_IN_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(RBYTr), val,
                            RBYTr_CTR));
                break;

            case CBX_PORT_STAT_IF_IN_UCAST_PKTS:
                /* Unicast frames */
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(RUCAr), val, RUCAr_CTR));
                break;

            case CBX_PORT_STAT_IF_IN_NON_UCAST_PKTS:
                /* ALL frames minus unicast frames */
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(RUCAr), val, RUCAr_CTR));
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(RPKTr), val, RPKTr_CTR));
                break;

            case CBX_PORT_STAT_IF_IN_DISCARDS:
                return SOC_E_UNAVAIL;
                break;

            case CBX_PORT_STAT_IF_IN_ERRORS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(RALNr), val, RALNr_CTR));
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(RFCSr), val, RFCSr_CTR));
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(RFRGr), val, RFRGr_CTR));
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(ROVRr), val, ROVRr_CTR));
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(RUNDr), val, RUNDr_CTR));
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(RJBRr), val, RJBRr_CTR));
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(RERPKTr), val, RERPKTr_CTR));
                break;

            case CBX_PORT_STAT_IF_IN_UNKNOWN_PROTOS:
                return SOC_E_UNAVAIL;
                break;

            case CBX_PORT_STAT_IF_IN_BROADCAST_PKTS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(RBCAr), val, RBCAr_CTR));
                break;

            case CBX_PORT_STAT_IF_IN_MULTICAST_PKTS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(RMCAr), val, RMCAr_CTR));
                break;

            case CBX_PORT_STAT_IF_IN_VLAN_DISCARDS:
                return SOC_E_UNAVAIL;
                break;

            case CBX_PORT_STAT_IF_OUT_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(TBYTr), val,
                            TBYTr_CTR));
                break;

            case CBX_PORT_STAT_IF_OUT_UCAST_PKTS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(TUCAr), val,
                            TUCAr_CTR));
                break;

            case CBX_PORT_STAT_IF_OUT_NON_UCAST_PKTS:
                /* All frames minus unicast frames */
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(TUCAr), val, TUCAr_CTR));
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(TPKTr), val, TPKTr_CTR));
                break;

            case CBX_PORT_STAT_IF_OUT_DISCARDS:
                return SOC_E_UNAVAIL;
                break;

            case CBX_PORT_STAT_IF_OUT_ERRORS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(TXCLr), val, TXCLr_CTR));
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(TLCLr), val, TLCLr_CTR));
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(TERRr), val, TERRr_CTR));
                break;

            case CBX_PORT_STAT_IF_OUT_QLEN:  /* robo not suppport */
                return SOC_E_UNAVAIL;
                break;

            case CBX_PORT_STAT_IF_OUT_BROADCAST_PKTS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(TBCAr), val, TBCAr_CTR));
                break;

            case CBX_PORT_STAT_IF_OUT_MULTICAST_PKTS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(TMCAr), val, TMCAr_CTR));
                break;

             /* *** RFC 1757 *** */
            case CBX_PORT_STAT_ETHER_STATS_DROP_EVENTS:
                return SOC_E_UNAVAIL;
                break;

            case CBX_PORT_STAT_ETHER_STATS_MULTICAST_PKTS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(TMCAr), val, TMCAr_CTR));
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(RMCAr), val, RMCAr_CTR));
                break;

            case CBX_PORT_STAT_ETHER_STATS_BROADCAST_PKTS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(TBCAr), val, TBCAr_CTR));
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(RBCAr), val, RBCAr_CTR));
                break;

            case CBX_PORT_STAT_ETHER_STATS_UNDERSIZE_PKTS:
                /* Undersize frames-as per RFC, use only count from RX */
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(RUNDr), val, RUNDr_CTR));
                break;

            case CBX_PORT_STAT_ETHER_STATS_FRAGMENTS:
                /* Fragment frames-as per RFC, use only count from RX */
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(RFRGr), val, RFRGr_CTR));
                break;

            case CBX_PORT_STAT_ETHER_STATS_PKTS_64_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(R64r), val, R64r_CTR));
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(T64r), val, T64r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_STATS_PKTS_65_TO_127_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(R127r), val, R127r_CTR));
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(T127r), val, T127r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_STATS_PKTS_128_TO_255_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(R255r), val, R255r_CTR));
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(T255r), val, T255r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_STATS_PKTS_256_TO_511_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(R511r), val, R511r_CTR));
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(T511r), val, T511r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_STATS_PKTS_512_TO_1023_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(R1023r), val,    R1023r_CTR));
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(T1023r), val,    T1023r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_STATS_PKTS_1024_TO_1522_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(R1518r), val, R1518r_CTR));
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(T1518r), val, T1518r_CTR));
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(RMGVr), val, RMGVr_CTR));
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(TMGVr), val, TMGVr_CTR));
                break;

            case CBX_PORT_STAT_ETHER_STATS_PKTS_1523_TO_2047_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(R2047r), val, R2047r_CTR));
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(T2047r), val, T2047r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_STATS_PKTS_2048_TO_4095_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(R4095r), val,    R4095r_CTR));
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(T4095r), val,    T4095r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_STATS_PKTS_4096_TO_8191_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(R9216r), val, R9216r_CTR));
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(T9216r), val, T9216r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_STATS_PKTS_8192_TO_MAX_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(R16383r), val, R16383r_CTR));
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(T16383r), val, T16383r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_STATS_OVERSIZE_PKTS:
                /* oversize frames-as per RFC, use only count from RX */
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(ROVRr), val, ROVRr_CTR));
                break;

            case CBX_PORT_STAT_ETHER_RX_OVERSIZE_PKTS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(ROVRr), val, ROVRr_CTR));
                break;

            case CBX_PORT_STAT_ETHER_TX_OVERSIZE_PKTS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(TOVRr), val, TOVRr_CTR));
                break;

            case CBX_PORT_STAT_ETHER_STATS_JABBERS:
                /* jabber frames-as per RFC, use only count from RX */
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(RJBRr), val, RJBRr_CTR));
                break;

            case CBX_PORT_STAT_ETHER_STATS_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(RBYTr), val, RBYTr_CTR));
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(TBYTr), val, TBYTr_CTR));
                break;

            case CBX_PORT_STAT_ETHER_STATS_PKTS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(TPKTr), val,
                            TPKTr_CTR));
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(RPKTr), val,
                            RPKTr_CTR));
                break;

            case CBX_PORT_STAT_ETHER_STATS_COLLISIONS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(TNCLr), val, TNCLr_CTR));
                break;

            case CBX_PORT_STAT_ETHER_STATS_CRC_ALIGN_ERRORS:
                /* CRC errors + alignment errors */
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(RALNr), val, RALNr_CTR));
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(RFCSr), val, RFCSr_CTR));
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(TFCSr), val, TFCSr_CTR));
                break;


            case CBX_PORT_STAT_ETHER_STATS_TX_NO_ERRORS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(TPOKr), val, TPOKr_CTR));
                break;

            case CBX_PORT_STAT_ETHER_STATS_RX_NO_ERRORS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(RPOKr), val, RPOKr_CTR));
                break;

                /* IPV4 */
            case CBX_PORT_STAT_IP_IN_RECEIVES:
            case CBX_PORT_STAT_IP_IN_OCTETS:
            case CBX_PORT_STAT_IP_IN_UCAST_PKTS:
            case CBX_PORT_STAT_IP_IN_NON_UCAST_PKTS:
            case CBX_PORT_STAT_IP_IN_DISCARDS:
            case CBX_PORT_STAT_IP_OUT_OCTETS:
            case CBX_PORT_STAT_IP_OUT_UCAST_PKTS:
            case CBX_PORT_STAT_IP_OUT_NON_UCAST_PKTS:
            case CBX_PORT_STAT_IP_OUT_DISCARDS:
                /*robo not suppport */
                return SOC_E_UNAVAIL;
                break;

                /*IPV6*/
            case CBX_PORT_STAT_IPV6_IN_RECEIVES:
            case CBX_PORT_STAT_IPV6_IN_OCTETS:
            case CBX_PORT_STAT_IPV6_IN_UCAST_PKTS:
            case CBX_PORT_STAT_IPV6_IN_NON_UCAST_PKTS:
            case CBX_PORT_STAT_IPV6_IN_MCAST_PKTS:
            case CBX_PORT_STAT_IPV6_IN_DISCARDS:
            case CBX_PORT_STAT_IPV6_OUT_OCTETS:
            case CBX_PORT_STAT_IPV6_OUT_UCAST_PKTS:
            case CBX_PORT_STAT_IPV6_OUT_NON_UCAST_PKTS:
            case CBX_PORT_STAT_IPV6_OUT_MCAST_PKTS:
            case CBX_PORT_STAT_IPV6_OUT_DISCARDS:
                /*robo not suppport */
                return SOC_E_UNAVAIL;
                break;

            case CBX_PORT_STAT_GREEN_DISCARD_DROPPED_PACKETS:
            case CBX_PORT_STAT_GREEN_DISCARD_DROPPED_BYTES:
            case CBX_PORT_STAT_YELLOW_DISCARD_DROPPED_PACKETS:
            case CBX_PORT_STAT_YELLOW_DISCARD_DROPPED_BYTES:
            case CBX_PORT_STAT_RED_DISCARD_DROPPED_PACKETS:
            case CBX_PORT_STAT_RED_DISCARD_DROPPED_BYTES:
            case CBX_PORT_STAT_DISCARD_DROPPED_PACKETS:
            case CBX_PORT_STAT_DISCARD_DROPPED_BYTES:
                /*robo not suppport */
                return SOC_E_UNAVAIL;
                break;

            /* Broadcom specific*/
            case CBX_PORT_STAT_ETHER_IN_PKTS_64_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(R64r), val, R64r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_IN_PKTS_65_TO_127_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(R127r), val, R127r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_IN_PKTS_128_TO_255_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(R255r), val, R255r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_IN_PKTS_256_TO_511_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(R511r), val, R511r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_IN_PKTS_512_TO_1023_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(R1023r), val, R1023r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_IN_PKTS_1024_TO_1522_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(R1518r), val,    R1518r_CTR));
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(RMGVr), val,    RMGVr_CTR));
                break;

            case CBX_PORT_STAT_ETHER_IN_PKTS_1523_TO_2047_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(R2047r), val,    R2047r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_IN_PKTS_2048_TO_4095_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(R4095r), val,    R4095r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_IN_PKTS_4096_TO_8191_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(R9216r), val, R9216r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_IN_PKTS_8192_TO_MAX_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(R16383r), val, R16383r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_OUT_PKTS_64_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(T64r), val, T64r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_OUT_PKTS_65_TO_127_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(T127r), val, T127r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_OUT_PKTS_128_TO_255_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(T255r), val, T255r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_OUT_PKTS_256_TO_511_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(T511r), val, T511r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_OUT_PKTS_512_TO_1023_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(T1023r), val, T1023r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_OUT_PKTS_1024_TO_1522_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(T1518r), val, T1518r_CTR));
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(TMGVr), val, TMGVr_CTR));
                break;

            case CBX_PORT_STAT_ETHER_OUT_PKTS_1523_TO_2047_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(T2047r),val, T2047r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_OUT_PKTS_2048_TO_4095_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(T4095r), val, T4095r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_OUT_PKTS_4096_TO_8191_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(T9216r), val,    T9216r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_OUT_PKTS_8192_TO_MAX_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_set(unit, port,
                            INDEX(T16383r), val, T16383r_CTR));
                break;

            default:

                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                            "counter_set:Statistic not supported:%d\n"),
                            counter_type));

                return SOC_E_PARAM;
        }
    } else {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                     "counter_set: port %d type not supported: \n"), port));
        return SOC_E_NOT_FOUND;
    }

    return rv;
}

/*
 *  Function : counter_get
 *
 *  Purpose :
 *      Get the snmp counter value.
 *
 *  Parameters :
 *      uint    :   uint number.
 *      port        :   port number.
 *      counter_type   :   counter_type.
 *      val  :   counter val.
 *
 *  Return :
 *      SOC_E_XXX
 *
 *  Note :
 *
 *
 */
int counter_get(int unit, uint32_t port,
                    cbx_port_stat_counter_t counter_type,
                    int sync_hw, uint64_t *val)
{
    uint64_t  count, count_tmp;
    COMPILER_64_ZERO(count);
    COMPILER_64_ZERO(count_tmp);
    if (port >= CBX_PORT_MAX) {
        return SOC_E_PORT;
    }
/*
   *If CBX_PORT_MAX is updated with 16/32  based on cascade mode,
   *below check can be removed
*/
    if((!SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) &&
             (port >= CBX_MAX_PORT_PER_UNIT )) {
        return SOC_E_PORT;
    }
    if((port >= CBX_MAX_PORT_PER_UNIT )) {
        port = port - CBX_MAX_PORT_PER_UNIT;
        unit = CBX_AVENGER_SECONDARY;
    } else {
        unit = CBX_AVENGER_PRIMARY;
    }
    if (IS_GE_PORT(unit, port)) {
        switch (counter_type)
        {
            /* *** RFC 1213 *** */

            case CBX_PORT_STAT_IF_IN_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_RX_ALL_OCTETS_HIr), sync_hw, &count,
                            MIB_RX_ALL_OCTETS_HIr_CTR));
                count = count << LS32;

                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_RX_ALL_OCTETS_LOr), sync_hw, &count_tmp,
                            MIB_RX_ALL_OCTETS_LOr_CTR));
                COMPILER_64_ADD_64(count, count_tmp);
                break;

            case CBX_PORT_STAT_IF_IN_UCAST_PKTS:
                /* Total non-error frames minus broadcast/multicast frames */
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_RXUNICASTPKTSr), sync_hw, &count,
                            MIB_RXUNICASTPKTSr_CTR));
                   break;

            case CBX_PORT_STAT_IF_IN_NON_UCAST_PKTS:
                /* All frames minus unicast frames */
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_RX_ALL_PKTSr), sync_hw, &count,
                            MIB_RX_ALL_PKTSr_CTR));
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_RXUNICASTPKTSr), sync_hw, &count_tmp,
                            MIB_RXUNICASTPKTSr_CTR));
                COMPILER_64_SUB_64(count, count_tmp);
                break;


            case CBX_PORT_STAT_IF_IN_DISCARDS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_RX_DROPr), sync_hw, &count,
                            MIB_RX_DROPr_CTR));
                break;

            case CBX_PORT_STAT_IF_IN_ERRORS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_RX_CRC_ALIGNr), sync_hw, &count_tmp,
                            MIB_RX_CRC_ALIGNr_CTR));
                count=count_tmp;
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_RX_FRAGr), sync_hw, &count_tmp,
                            MIB_RX_FRAGr_CTR));
                COMPILER_64_ADD_64(count, count_tmp);
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_RX_OVRr), sync_hw, &count_tmp,
                            MIB_RX_OVRr_CTR));
                COMPILER_64_ADD_64(count, count_tmp);
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_RX_UNDr), sync_hw, &count_tmp,
                            MIB_RX_UNDr_CTR));
                COMPILER_64_ADD_64(count, count_tmp);
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_RX_SYMr), sync_hw, &count_tmp,
                            MIB_RX_SYMr_CTR));
                COMPILER_64_ADD_64(count, count_tmp);

                break;

            case CBX_PORT_STAT_IF_IN_UNKNOWN_PROTOS:
                return SOC_E_UNAVAIL;

            case CBX_PORT_STAT_IF_IN_BROADCAST_PKTS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_RX_BRDCASTr), sync_hw, &count,
                            MIB_RX_BRDCASTr_CTR));
                break;

            case CBX_PORT_STAT_IF_IN_MULTICAST_PKTS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_RX_MULTr), sync_hw, &count,
                            MIB_RX_MULTr_CTR));
                break;

            case CBX_PORT_STAT_IF_IN_VLAN_DISCARDS:
                return SOC_E_UNAVAIL;
                break;

            case CBX_PORT_STAT_IF_OUT_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_TX_ALL_OCTETS_HIr), sync_hw, &count_tmp,
                            MIB_TX_ALL_OCTETS_HIr_CTR));
                count = count_tmp << LS32;

                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_TX_ALL_OCTETS_LOr), sync_hw, &count_tmp,
                            MIB_TX_ALL_OCTETS_LOr_CTR));
                COMPILER_64_ADD_64(count, count_tmp);
                break;

            case CBX_PORT_STAT_IF_OUT_UCAST_PKTS:  /* ALL - mcast - bcast */
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_TXUNICASTPKTr), sync_hw, &count,
                            MIB_TXUNICASTPKTr_CTR));
                break;

            case CBX_PORT_STAT_IF_OUT_NON_UCAST_PKTS:
                /* All frames minus unicast frames */
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_TX_ALL_PKTSr), sync_hw, &count,
                            MIB_TX_ALL_PKTSr_CTR));
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_TXUNICASTPKTr), sync_hw, &count_tmp,
                            MIB_TXUNICASTPKTr_CTR));
                COMPILER_64_SUB_64(count, count_tmp);
                break;

            case CBX_PORT_STAT_IF_OUT_DISCARDS:
                return SOC_E_UNAVAIL;
                break;

            case CBX_PORT_STAT_IF_OUT_ERRORS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_TX_EX_COLr), sync_hw, &count_tmp,
                            MIB_TX_EX_COLr_CTR));
                count=count_tmp;
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_TX_LATEr), sync_hw, &count_tmp,
                            MIB_TX_LATEr_CTR));
                COMPILER_64_ADD_64(count, count_tmp);
                break;

            case CBX_PORT_STAT_IF_OUT_QLEN:  /* robo not suppport */
                return SOC_E_UNAVAIL;

            case CBX_PORT_STAT_IF_OUT_BROADCAST_PKTS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_TX_BRDCASTr), sync_hw, &count,
                            MIB_TX_BRDCASTr_CTR));
                break;

            case CBX_PORT_STAT_IF_OUT_MULTICAST_PKTS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_TX_MULTr), sync_hw, &count,
                            MIB_TX_MULTr_CTR));
                break;

            case CBX_PORT_STAT_ETHER_STATS_DROP_EVENTS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_RX_DROPr), sync_hw, &count,
                            MIB_RX_DROPr_CTR));
                break;

            case CBX_PORT_STAT_ETHER_STATS_MULTICAST_PKTS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_TX_MULTr), sync_hw, &count_tmp,
                            MIB_TX_MULTr_CTR));
                count = count_tmp;
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_RX_MULTr), sync_hw, &count_tmp,
                            MIB_RX_MULTr_CTR));
                COMPILER_64_ADD_64(count, count_tmp);
                break;

            case CBX_PORT_STAT_ETHER_STATS_BROADCAST_PKTS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_TX_BRDCASTr), sync_hw, &count_tmp,
                            MIB_TX_BRDCASTr_CTR));
                count = count_tmp;
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_RX_BRDCASTr), sync_hw, &count_tmp,
                            MIB_RX_BRDCASTr_CTR));
                COMPILER_64_ADD_64(count, count_tmp);
                break;

            case CBX_PORT_STAT_ETHER_STATS_UNDERSIZE_PKTS:
                /* Undersize frames-as per RFC, use only count from RX */
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_RX_UNDr), sync_hw, &count,
                            MIB_RX_UNDr_CTR));
                break;

            case CBX_PORT_STAT_ETHER_STATS_FRAGMENTS:
                /* Fragment frames-as per RFC, use only count from RX */
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_RX_FRAGr), sync_hw, &count,
                            MIB_RX_FRAGr_CTR));
                break;

            case CBX_PORT_STAT_ETHER_STATS_PKTS_64_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_TX_64r), sync_hw, &count_tmp,
                            MIB_TX_64r_CTR));
                count = count_tmp;
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_RX_64r), sync_hw, &count_tmp,
                            MIB_RX_64r_CTR));
                COMPILER_64_ADD_64(count, count_tmp);
                break;

            case CBX_PORT_STAT_ETHER_STATS_PKTS_65_TO_127_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_TX_65_127r), sync_hw, &count_tmp,
                            MIB_TX_65_127r_CTR));
                count = count_tmp;
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_RX_65_127r), sync_hw, &count_tmp,
                            MIB_RX_65_127r_CTR));
                COMPILER_64_ADD_64(count, count_tmp);
                break;

            case CBX_PORT_STAT_ETHER_STATS_PKTS_128_TO_255_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_TX_128_255r), sync_hw, &count_tmp,
                            MIB_TX_128_255r_CTR));
                count = count_tmp;
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_RX_128_255r), sync_hw, &count_tmp,
                            MIB_RX_128_255r_CTR));
                COMPILER_64_ADD_64(count, count_tmp);
                break;

            case CBX_PORT_STAT_ETHER_STATS_PKTS_256_TO_511_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_TX_256_511r), sync_hw, &count_tmp,
                            MIB_TX_256_511r_CTR));
                count = count_tmp;
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_RX_256_511r), sync_hw, &count_tmp,
                            MIB_RX_256_511r_CTR));
                COMPILER_64_ADD_64(count, count_tmp);
                break;

            case CBX_PORT_STAT_ETHER_STATS_PKTS_512_TO_1023_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_TX_512_1023r), sync_hw, &count_tmp,
                            MIB_TX_512_1023r_CTR));
                count = count_tmp;
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_RX_512_1023r), sync_hw, &count_tmp,
                            MIB_RX_512_1023r_CTR));
                COMPILER_64_ADD_64(count, count_tmp);
                break;

            case CBX_PORT_STAT_ETHER_STATS_PKTS_1024_TO_1522_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_TX_1024_1522r), sync_hw, &count_tmp,
                            MIB_TX_1024_1522r_CTR));
                count = count_tmp;
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_RX_1024_1522r), sync_hw, &count_tmp,
                            MIB_RX_1024_1522r_CTR));
                COMPILER_64_ADD_64(count, count_tmp);
                break;

            case CBX_PORT_STAT_ETHER_STATS_PKTS_1523_TO_2047_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_TX_1523_2047r), sync_hw, &count_tmp,
                            MIB_TX_1523_2047r_CTR));
                count = count_tmp;
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_RX_1523_2047r), sync_hw, &count_tmp,
                            MIB_RX_1523_2047r_CTR));
                COMPILER_64_ADD_64(count, count_tmp);
                break;

            case CBX_PORT_STAT_ETHER_STATS_PKTS_2048_TO_4095_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_TX_2048_4095r), sync_hw, &count_tmp,
                            MIB_TX_2048_4095r_CTR));
                count = count_tmp;
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_RX_2048_4095r), sync_hw, &count_tmp,
                            MIB_RX_2048_4095r_CTR));
                COMPILER_64_ADD_64(count, count_tmp);
                break;

            case CBX_PORT_STAT_ETHER_STATS_PKTS_4096_TO_8191_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_TX_4096_8191r), sync_hw, &count_tmp,
                            MIB_TX_4096_8191r_CTR));
                count = count_tmp;
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_RX_4096_8191r), sync_hw, &count_tmp,
                            MIB_RX_4096_8191r_CTR));
                COMPILER_64_ADD_64(count, count_tmp);
                break;

            case CBX_PORT_STAT_ETHER_STATS_PKTS_8192_TO_MAX_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_TX_8192_MAXr), sync_hw, &count_tmp,
                            MIB_TX_8192_MAXr_CTR));
                count = count_tmp;
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_RX_8192_MAXr), sync_hw, &count_tmp,
                            MIB_RX_8192_MAXr_CTR));
                COMPILER_64_ADD_64(count, count_tmp);
                break;

            case CBX_PORT_STAT_ETHER_STATS_OVERSIZE_PKTS:
                /* Oversize frames-as per RFC, use only count from RX */
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_RX_OVRr), sync_hw, &count,
                            MIB_RX_OVRr_CTR));
                break;

            case CBX_PORT_STAT_ETHER_RX_OVERSIZE_PKTS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_RX_OVRr), sync_hw, &count,
                            MIB_RX_OVRr_CTR));
                break;

            case CBX_PORT_STAT_ETHER_TX_OVERSIZE_PKTS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_TX_OVERr), sync_hw, &count,
                            MIB_TX_OVERr_CTR));
                break;

            case CBX_PORT_STAT_ETHER_STATS_JABBERS:
                /* Jabber frames-as per RFC, use only count from RX */
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_RX_JABr), sync_hw, &count,
                            MIB_RX_JABr_CTR));
                break;

            case CBX_PORT_STAT_ETHER_STATS_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_TX_ALL_OCTETS_HIr), sync_hw, &count,
                            MIB_TX_ALL_OCTETS_HIr_CTR));
                count = count << LS32;

                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_TX_ALL_OCTETS_LOr), sync_hw, &count_tmp,
                            MIB_TX_ALL_OCTETS_LOr_CTR));
                COMPILER_64_ADD_64(count, count_tmp);

                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_RX_ALL_OCTETS_HIr), sync_hw, &count_tmp,
                            MIB_RX_ALL_OCTETS_HIr_CTR));
                count_tmp = count_tmp << LS32;
                COMPILER_64_ADD_64(count, count_tmp);

                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_RX_ALL_OCTETS_LOr), sync_hw, &count_tmp,
                            MIB_RX_ALL_OCTETS_LOr_CTR));
                COMPILER_64_ADD_64(count, count_tmp);
                break;

            case CBX_PORT_STAT_ETHER_STATS_PKTS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_TX_ALL_PKTSr), sync_hw, &count_tmp,
                            MIB_TX_ALL_PKTSr_CTR));
                count = count_tmp;
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_RX_ALL_PKTSr), sync_hw, &count_tmp,
                            MIB_RX_ALL_PKTSr_CTR));
                COMPILER_64_ADD_64(count, count_tmp);
                break;

            case CBX_PORT_STAT_ETHER_STATS_COLLISIONS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_TX_COLr), sync_hw, &count,
                            MIB_TX_COLr_CTR));
                break;

            case CBX_PORT_STAT_ETHER_STATS_CRC_ALIGN_ERRORS:
                /* CRC errors + alignment errors */
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_RX_CRC_ALIGNr), sync_hw, &count,
                            MIB_RX_CRC_ALIGNr_CTR));
                break;

            case CBX_PORT_STAT_ETHER_STATS_TX_NO_ERRORS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_TX_GD_PKTSr), sync_hw, &count,
                            MIB_TX_GD_PKTSr_CTR));
                break;

            case CBX_PORT_STAT_ETHER_STATS_RX_NO_ERRORS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_RX_GD_PKTSr), sync_hw, &count,
                            MIB_RX_GD_PKTSr_CTR));
                break;

               /* IPV4 */
            case CBX_PORT_STAT_IP_IN_RECEIVES:
            case CBX_PORT_STAT_IP_IN_OCTETS:
            case CBX_PORT_STAT_IP_IN_UCAST_PKTS:
            case CBX_PORT_STAT_IP_IN_NON_UCAST_PKTS:
            case CBX_PORT_STAT_IP_IN_DISCARDS:
            case CBX_PORT_STAT_IP_OUT_OCTETS:
            case CBX_PORT_STAT_IP_OUT_UCAST_PKTS:
            case CBX_PORT_STAT_IP_OUT_NON_UCAST_PKTS:
            case CBX_PORT_STAT_IP_OUT_DISCARDS:
                /* robo not suppport */
                return SOC_E_UNAVAIL;
                break;
                /* IPV6 */
            case CBX_PORT_STAT_IPV6_IN_RECEIVES:
            case CBX_PORT_STAT_IPV6_IN_OCTETS:
            case CBX_PORT_STAT_IPV6_IN_UCAST_PKTS:
            case CBX_PORT_STAT_IPV6_IN_NON_UCAST_PKTS:
            case CBX_PORT_STAT_IPV6_IN_MCAST_PKTS:
            case CBX_PORT_STAT_IPV6_IN_DISCARDS:
            case CBX_PORT_STAT_IPV6_OUT_OCTETS:
            case CBX_PORT_STAT_IPV6_OUT_UCAST_PKTS:
            case CBX_PORT_STAT_IPV6_OUT_NON_UCAST_PKTS:
            case CBX_PORT_STAT_IPV6_OUT_MCAST_PKTS:
            case CBX_PORT_STAT_IPV6_OUT_DISCARDS:
                /* robo not suppport */
                return SOC_E_UNAVAIL;
                break;

            case CBX_PORT_STAT_GREEN_DISCARD_DROPPED_PACKETS:
            case CBX_PORT_STAT_GREEN_DISCARD_DROPPED_BYTES:
            case CBX_PORT_STAT_YELLOW_DISCARD_DROPPED_PACKETS:
            case CBX_PORT_STAT_YELLOW_DISCARD_DROPPED_BYTES:
            case CBX_PORT_STAT_RED_DISCARD_DROPPED_PACKETS:
            case CBX_PORT_STAT_RED_DISCARD_DROPPED_BYTES:
            case CBX_PORT_STAT_DISCARD_DROPPED_PACKETS:
            case CBX_PORT_STAT_DISCARD_DROPPED_BYTES:
                /* robo not suppport */
                return SOC_E_UNAVAIL;
                break;
                 /* Broadcom specific*/
            case CBX_PORT_STAT_ETHER_IN_PKTS_64_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_RX_64r), sync_hw, &count,
                            MIB_RX_64r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_IN_PKTS_65_TO_127_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_RX_65_127r), sync_hw, &count,
                            MIB_RX_65_127r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_IN_PKTS_128_TO_255_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_RX_128_255r), sync_hw, &count,
                            MIB_RX_128_255r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_IN_PKTS_256_TO_511_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_RX_256_511r), sync_hw, &count,
                            MIB_RX_256_511r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_IN_PKTS_512_TO_1023_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_RX_512_1023r), sync_hw, &count,
                            MIB_RX_512_1023r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_IN_PKTS_1024_TO_1522_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_RX_1024_1522r), sync_hw, &count,
                            MIB_RX_1024_1522r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_IN_PKTS_1523_TO_2047_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_RX_1523_2047r), sync_hw, &count,
                            MIB_RX_1523_2047r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_IN_PKTS_2048_TO_4095_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_RX_2048_4095r), sync_hw, &count,
                            MIB_RX_2048_4095r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_IN_PKTS_4096_TO_8191_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_RX_4096_8191r), sync_hw, &count,
                            MIB_RX_4096_8191r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_IN_PKTS_8192_TO_MAX_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_RX_8192_MAXr), sync_hw, &count,
                            MIB_RX_8192_MAXr_CTR));
                break;

            case CBX_PORT_STAT_ETHER_OUT_PKTS_64_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_TX_64r), sync_hw, &count,
                            MIB_TX_64r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_OUT_PKTS_65_TO_127_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_TX_65_127r), sync_hw, &count,
                            MIB_TX_65_127r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_OUT_PKTS_128_TO_255_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_TX_128_255r), sync_hw, &count,
                            MIB_TX_128_255r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_OUT_PKTS_256_TO_511_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_TX_256_511r), sync_hw, &count,
                            MIB_TX_256_511r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_OUT_PKTS_512_TO_1023_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_TX_512_1023r), sync_hw, &count,
                            MIB_TX_512_1023r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_OUT_PKTS_1024_TO_1522_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_TX_1024_1522r), sync_hw, &count,
                            MIB_TX_1024_1522r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_OUT_PKTS_1523_TO_2047_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_TX_1523_2047r), sync_hw, &count,
                            MIB_TX_1523_2047r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_OUT_PKTS_2048_TO_4095_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_TX_2048_4095r), sync_hw, &count,
                            MIB_TX_2048_4095r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_OUT_PKTS_4096_TO_8191_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_TX_4096_8191r), sync_hw, &count,
                            MIB_TX_4096_8191r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_OUT_PKTS_8192_TO_MAX_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(MIB_TX_8192_MAXr), sync_hw, &count,
                            MIB_TX_8192_MAXr_CTR));
                break;

            default:
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                            "counter_get: Statistic not supported: %d\n"),
                            counter_type));
                return SOC_E_PARAM;
        }
    } else if (IS_XL_PORT(unit, port)) {
        switch (counter_type)
        {
            /* *** RFC 1213 *** */

            case CBX_PORT_STAT_IF_IN_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(RBYTr), sync_hw, &count,
                            RBYTr_CTR));
                break;

            case CBX_PORT_STAT_IF_IN_UCAST_PKTS:
                /* Total non-error frames minus broadcast/multicast frames */
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(RUCAr), sync_hw, &count,
                            RUCAr_CTR));
                   break;

            case CBX_PORT_STAT_IF_IN_NON_UCAST_PKTS:
                /* Multicast frames plus broadcast frames */
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(RPKTr), sync_hw, &count,
                            RPKTr_CTR));
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(RUCAr), sync_hw, &count_tmp,
                            RUCAr_CTR));
                COMPILER_64_SUB_64(count, count_tmp);
                break;


            case CBX_PORT_STAT_IF_IN_DISCARDS:
                return SOC_E_UNAVAIL;
                break;

            case CBX_PORT_STAT_IF_IN_ERRORS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(RFCSr), sync_hw, &count_tmp,
                            RFCSr_CTR));
                count=count_tmp;
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(RALNr), sync_hw, &count_tmp,
                            RALNr_CTR));
                COMPILER_64_ADD_64(count, count_tmp);
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(RFRGr), sync_hw, &count_tmp,
                            RFRGr_CTR));
                COMPILER_64_ADD_64(count, count_tmp);
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(ROVRr), sync_hw, &count_tmp,
                            ROVRr_CTR));
                COMPILER_64_ADD_64(count, count_tmp);
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(RUNDr), sync_hw, &count_tmp,
                            RUNDr_CTR));
                COMPILER_64_ADD_64(count, count_tmp);
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(RJBRr), sync_hw, &count_tmp,
                            RJBRr_CTR));
                COMPILER_64_ADD_64(count, count_tmp);
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(RERPKTr), sync_hw, &count_tmp,
                            RERPKTr_CTR));
                COMPILER_64_ADD_64(count, count_tmp);

                break;

            case CBX_PORT_STAT_IF_IN_UNKNOWN_PROTOS:
                return SOC_E_UNAVAIL;

            case CBX_PORT_STAT_IF_IN_BROADCAST_PKTS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(RBCAr), sync_hw, &count,
                            RBCAr_CTR));
                break;

            case CBX_PORT_STAT_IF_IN_MULTICAST_PKTS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(RMCAr), sync_hw, &count,
                            RMCAr_CTR));
                break;

            case CBX_PORT_STAT_IF_IN_VLAN_DISCARDS:
                return SOC_E_UNAVAIL;
                break;

            case CBX_PORT_STAT_IF_OUT_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(TBYTr), sync_hw, &count,
                            TBYTr_CTR));
                break;

            case CBX_PORT_STAT_IF_OUT_UCAST_PKTS:  /* Unicast frames */
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(TUCAr), sync_hw, &count,
                            TUCAr_CTR));
                break;

            case CBX_PORT_STAT_IF_OUT_NON_UCAST_PKTS:
                /* ALL frames Minus unicast frames */
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(TPKTr), sync_hw, &count,
                            TPKTr_CTR));
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(TUCAr), sync_hw, &count_tmp,
                            TUCAr_CTR));
                COMPILER_64_SUB_64(count, count_tmp);
                break;

            case CBX_PORT_STAT_IF_OUT_DISCARDS:
                return SOC_E_UNAVAIL;
                break;

            case CBX_PORT_STAT_IF_OUT_ERRORS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(TXCLr), sync_hw, &count_tmp,
                            TXCLr_CTR));
                count=count_tmp;
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(TLCLr), sync_hw, &count_tmp,
                            TLCLr_CTR));
                COMPILER_64_ADD_64(count, count_tmp);
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(TERRr), sync_hw, &count_tmp,
                            TERRr_CTR));
                COMPILER_64_ADD_64(count, count_tmp);
                break;

            case CBX_PORT_STAT_IF_OUT_QLEN:  /* robo not suppport */
                return SOC_E_UNAVAIL;

            case CBX_PORT_STAT_IF_OUT_BROADCAST_PKTS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(TBCAr), sync_hw, &count,
                            TBCAr_CTR));
                break;

            case CBX_PORT_STAT_IF_OUT_MULTICAST_PKTS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(TMCAr), sync_hw, &count,
                            TMCAr_CTR));
                break;

            case CBX_PORT_STAT_ETHER_STATS_DROP_EVENTS:
                return SOC_E_UNAVAIL;
                break;

            case CBX_PORT_STAT_ETHER_STATS_MULTICAST_PKTS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(TMCAr), sync_hw, &count_tmp,
                            TMCAr_CTR));
                count = count_tmp;
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(RMCAr), sync_hw, &count_tmp,
                            RMCAr_CTR));
                COMPILER_64_ADD_64(count, count_tmp);
                break;

            case CBX_PORT_STAT_ETHER_STATS_BROADCAST_PKTS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(TBCAr), sync_hw, &count_tmp,
                            TBCAr_CTR));
                count = count_tmp;
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(RBCAr), sync_hw, &count_tmp,
                            RBCAr_CTR));
                COMPILER_64_ADD_64(count, count_tmp);
                break;

            case CBX_PORT_STAT_ETHER_STATS_UNDERSIZE_PKTS:
                /* Undersize frames-as per RFC, use only count from RX */
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(RUNDr), sync_hw, &count,
                            RUNDr_CTR));
                break;

            case CBX_PORT_STAT_ETHER_STATS_FRAGMENTS:
                /* Fragment frames-as per RFC, use only count from RX */
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(RFRGr), sync_hw, &count,
                            RFRGr_CTR));
                break;

            case CBX_PORT_STAT_ETHER_STATS_PKTS_64_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(T64r), sync_hw, &count_tmp,
                            T64r_CTR));
                count = count_tmp;
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(R64r), sync_hw, &count_tmp,
                            R64r_CTR));
                COMPILER_64_ADD_64(count, count_tmp);
                break;

            case CBX_PORT_STAT_ETHER_STATS_PKTS_65_TO_127_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(T127r), sync_hw, &count_tmp,
                            T127r_CTR));
                count = count_tmp;
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(R127r), sync_hw, &count_tmp,
                            R127r_CTR));
                COMPILER_64_ADD_64(count, count_tmp);
                break;

            case CBX_PORT_STAT_ETHER_STATS_PKTS_128_TO_255_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(T255r), sync_hw, &count_tmp,
                            T255r_CTR));
                count = count_tmp;
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(R255r), sync_hw, &count_tmp,
                            R255r_CTR));
                COMPILER_64_ADD_64(count, count_tmp);
                break;

            case CBX_PORT_STAT_ETHER_STATS_PKTS_256_TO_511_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(T511r), sync_hw, &count_tmp,
                            T511r_CTR));
                count = count_tmp;
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(R511r), sync_hw, &count_tmp,
                            R511r_CTR));
                COMPILER_64_ADD_64(count, count_tmp);
                break;

            case CBX_PORT_STAT_ETHER_STATS_PKTS_512_TO_1023_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(T1023r), sync_hw, &count_tmp,
                            T1023r_CTR));
                count = count_tmp;
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(R1023r), sync_hw, &count_tmp,
                            R1023r_CTR));
                COMPILER_64_ADD_64(count, count_tmp);
                break;

            case CBX_PORT_STAT_ETHER_STATS_PKTS_1024_TO_1522_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(T1518r), sync_hw, &count_tmp,
                            T1518r_CTR));
                count = count_tmp;
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(R1518r), sync_hw, &count_tmp,
                            R1518r_CTR));
                COMPILER_64_ADD_64(count, count_tmp);
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(RMGVr), sync_hw, &count_tmp,
                            RMGVr_CTR));
                COMPILER_64_ADD_64(count, count_tmp);
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(TMGVr), sync_hw, &count_tmp,
                            TMGVr_CTR));
                COMPILER_64_ADD_64(count, count_tmp);
                break;

            case CBX_PORT_STAT_ETHER_STATS_PKTS_1523_TO_2047_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(T2047r), sync_hw, &count_tmp,
                            T2047r_CTR));
                count = count_tmp;
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(R2047r), sync_hw, &count_tmp,
                            R2047r_CTR));
                COMPILER_64_ADD_64(count, count_tmp);
                break;

            case CBX_PORT_STAT_ETHER_STATS_PKTS_2048_TO_4095_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(T4095r), sync_hw, &count_tmp,
                            T4095r_CTR));
                count = count_tmp;
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(R4095r), sync_hw, &count_tmp,
                            R4095r_CTR));
                COMPILER_64_ADD_64(count, count_tmp);
                break;

            case CBX_PORT_STAT_ETHER_STATS_PKTS_4096_TO_8191_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(T9216r), sync_hw, &count_tmp,
                            T9216r_CTR));
                count = count_tmp;
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(R9216r), sync_hw, &count_tmp,
                            R9216r_CTR));
                COMPILER_64_ADD_64(count, count_tmp);
                break;

            case CBX_PORT_STAT_ETHER_STATS_PKTS_8192_TO_MAX_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(T16383r), sync_hw, &count_tmp,
                            T16383r_CTR));
                count = count_tmp;
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(R16383r), sync_hw, &count_tmp,
                            R16383r_CTR));
                COMPILER_64_ADD_64(count, count_tmp);
                break;

            case CBX_PORT_STAT_ETHER_STATS_OVERSIZE_PKTS:
                /* Oversize frames-as per RFC, use only count from RX */
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(ROVRr), sync_hw, &count,
                            ROVRr_CTR));
                break;

            case CBX_PORT_STAT_ETHER_RX_OVERSIZE_PKTS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(ROVRr), sync_hw, &count,
                            ROVRr_CTR));
                break;

            case CBX_PORT_STAT_ETHER_TX_OVERSIZE_PKTS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(TOVRr), sync_hw, &count,
                            TOVRr_CTR));
                break;

            case CBX_PORT_STAT_ETHER_STATS_JABBERS:
                /* Jabber frames-as per RFC, use only count from RX */
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(RJBRr), sync_hw, &count,
                            RJBRr_CTR));
                break;

            case CBX_PORT_STAT_ETHER_STATS_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(TBYTr), sync_hw, &count,
                            TBYTr_CTR));
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(RBYTr), sync_hw, &count_tmp,
                            RBYTr_CTR));
                COMPILER_64_ADD_64(count, count_tmp);
                break;

            case CBX_PORT_STAT_ETHER_STATS_PKTS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(TPKTr), sync_hw, &count_tmp,
                            TPKTr_CTR));
                count = count_tmp;
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(RPKTr), sync_hw, &count_tmp,
                            RPKTr_CTR));
                COMPILER_64_ADD_64(count, count_tmp);
                break;

            case CBX_PORT_STAT_ETHER_STATS_COLLISIONS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(TNCLr), sync_hw, &count,
                            TNCLr_CTR));
                break;

            case CBX_PORT_STAT_ETHER_STATS_CRC_ALIGN_ERRORS:
                /* CRC errors + alignment errors */
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(RFCSr), sync_hw, &count,
                            RFCSr_CTR));
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(RALNr), sync_hw, &count_tmp,
                            RALNr_CTR));
                COMPILER_64_ADD_64(count, count_tmp);
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(TFCSr), sync_hw, &count_tmp,
                            TFCSr_CTR));
                COMPILER_64_ADD_64(count, count_tmp);
                break;

            case CBX_PORT_STAT_ETHER_STATS_TX_NO_ERRORS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(TPOKr), sync_hw, &count,
                            TPOKr_CTR));
                break;

            case CBX_PORT_STAT_ETHER_STATS_RX_NO_ERRORS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(RPOKr), sync_hw, &count,
                            RPOKr_CTR));
                break;

               /* IPV4 */
            case CBX_PORT_STAT_IP_IN_RECEIVES:
            case CBX_PORT_STAT_IP_IN_OCTETS:
            case CBX_PORT_STAT_IP_IN_UCAST_PKTS:
            case CBX_PORT_STAT_IP_IN_NON_UCAST_PKTS:
            case CBX_PORT_STAT_IP_IN_DISCARDS:
            case CBX_PORT_STAT_IP_OUT_OCTETS:
            case CBX_PORT_STAT_IP_OUT_UCAST_PKTS:
            case CBX_PORT_STAT_IP_OUT_NON_UCAST_PKTS:
            case CBX_PORT_STAT_IP_OUT_DISCARDS:
                /* robo not suppport */
                return SOC_E_UNAVAIL;
                break;
                /* IPV6 */
            case CBX_PORT_STAT_IPV6_IN_RECEIVES:
            case CBX_PORT_STAT_IPV6_IN_OCTETS:
            case CBX_PORT_STAT_IPV6_IN_UCAST_PKTS:
            case CBX_PORT_STAT_IPV6_IN_NON_UCAST_PKTS:
            case CBX_PORT_STAT_IPV6_IN_MCAST_PKTS:
            case CBX_PORT_STAT_IPV6_IN_DISCARDS:
            case CBX_PORT_STAT_IPV6_OUT_OCTETS:
            case CBX_PORT_STAT_IPV6_OUT_UCAST_PKTS:
            case CBX_PORT_STAT_IPV6_OUT_NON_UCAST_PKTS:
            case CBX_PORT_STAT_IPV6_OUT_MCAST_PKTS:
            case CBX_PORT_STAT_IPV6_OUT_DISCARDS:
                /* robo not suppport */
                return SOC_E_UNAVAIL;
                break;

            case CBX_PORT_STAT_GREEN_DISCARD_DROPPED_PACKETS:
            case CBX_PORT_STAT_GREEN_DISCARD_DROPPED_BYTES:
            case CBX_PORT_STAT_YELLOW_DISCARD_DROPPED_PACKETS:
            case CBX_PORT_STAT_YELLOW_DISCARD_DROPPED_BYTES:
            case CBX_PORT_STAT_RED_DISCARD_DROPPED_PACKETS:
            case CBX_PORT_STAT_RED_DISCARD_DROPPED_BYTES:
            case CBX_PORT_STAT_DISCARD_DROPPED_PACKETS:
            case CBX_PORT_STAT_DISCARD_DROPPED_BYTES:
                /* robo not suppport */
                return SOC_E_UNAVAIL;
                break;
                 /* Broadcom specific*/
            case CBX_PORT_STAT_ETHER_IN_PKTS_64_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(R64r), sync_hw, &count,
                            R64r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_IN_PKTS_65_TO_127_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(R127r), sync_hw, &count,
                            R127r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_IN_PKTS_128_TO_255_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(R255r), sync_hw, &count,
                            R255r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_IN_PKTS_256_TO_511_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(R511r), sync_hw, &count,
                            R511r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_IN_PKTS_512_TO_1023_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(R1023r), sync_hw, &count,
                            R1023r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_IN_PKTS_1024_TO_1522_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(R1518r), sync_hw, &count,
                            R1518r_CTR));
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(RMGVr), sync_hw, &count_tmp,
                            RMGVr_CTR));
                COMPILER_64_ADD_64(count, count_tmp);
                break;

            case CBX_PORT_STAT_ETHER_IN_PKTS_1523_TO_2047_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(R2047r), sync_hw, &count,
                            R2047r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_IN_PKTS_2048_TO_4095_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(R4095r), sync_hw, &count,
                            R4095r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_IN_PKTS_4096_TO_8191_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(R9216r), sync_hw, &count,
                            R9216r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_IN_PKTS_8192_TO_MAX_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(R16383r), sync_hw, &count,
                            R16383r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_OUT_PKTS_64_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(T64r), sync_hw, &count,
                            T64r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_OUT_PKTS_65_TO_127_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(T127r), sync_hw, &count,
                            T127r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_OUT_PKTS_128_TO_255_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(T255r), sync_hw, &count,
                            T255r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_OUT_PKTS_256_TO_511_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(T511r), sync_hw, &count,
                            T511r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_OUT_PKTS_512_TO_1023_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(T1023r), sync_hw, &count,
                            T1023r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_OUT_PKTS_1024_TO_1522_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(T1518r), sync_hw, &count,
                            T1518r_CTR));
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(TMGVr), sync_hw, &count_tmp,
                            TMGVr_CTR));
                COMPILER_64_ADD_64(count, count_tmp);
                break;

            case CBX_PORT_STAT_ETHER_OUT_PKTS_1523_TO_2047_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(T2047r), sync_hw, &count,
                            T2047r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_OUT_PKTS_2048_TO_4095_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(T4095r), sync_hw, &count,
                            T4095r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_OUT_PKTS_4096_TO_8191_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(T9216r), sync_hw, &count,
                            T9216r_CTR));
                break;

            case CBX_PORT_STAT_ETHER_OUT_PKTS_8192_TO_MAX_OCTETS:
                SOC_IF_ERROR_RETURN(soc_robo_counter_get(unit, port,
                            INDEX(T16383r), sync_hw, &count,
                            T16383r_CTR));
                break;

            default:
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                            "counter_get: Statistic not supported: %d\n"),
                            counter_type));
                return SOC_E_PARAM;
        }
    } else {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                     "counter_get: port %d type not supported: \n"), port));
        return SOC_E_NOT_FOUND;
    }
    *val = count;
     return SOC_E_NONE;
}

/*
 * Function:
 *      counter_reset
 * Purpose:
 *      Reset the MIB counters
 * Parameters:
 *      unit    - RoboSwitch unit number.
 * Returns:
 *      SOC_E_XXX.
 */
int
counter_reset(int unit)
{

#ifdef COUNTER_THREAD
    int i = 0;
    COUNTER_ATOMIC_DEF soc = SOC_CONTROL(unit);
    /* clear SW counter table*/
    for(i = 0;i < SW_COUNT_ARR;i++)
    {
        soc->counter_sw_val[i] = 0;
    }
#endif
    soc_robo_counter_reset(unit);
    return SOC_E_NONE;
}

#ifdef COUNTER_DEBUG
int soc_port_stat_clear(uint32_t portid)
{
    int rv = 0;
    uint64 value = 0;
    cbx_port_stat_counter_t type = 0;
    for (type = 0; type < PORT_STAT_MAX ; type++) {
        rv = counter_set(0, portid, type, value);
        if((rv == SOC_E_PORT) || (rv == SOC_E_NOT_FOUND))
            return rv;
        if(rv == SOC_E_UNAVAIL)
            continue;
    }
    return SOC_E_NONE;
}

int soc_port_stat_dump(uint32_t portid)
{
#ifdef CONFIG_EXTERNAL_HOST
    int     sync_hw = TRUE;
#else
    int     sync_hw = FALSE;
#endif
    int     unit = 0,rv;
    uint64  value = 0;
#ifdef BE_HOST
    uint64_t val_lo = 0;
    uint64_t val_hi = 0;
#endif
    cbx_port_stat_counter_t type = 0;
    for (type = 0; type < PORT_STAT_MAX ; type++) {
        rv = counter_get(unit,portid,type,sync_hw,&value);
        if((rv == SOC_E_PORT) || (rv == SOC_E_NOT_FOUND))
            return rv;
        if(rv == SOC_E_UNAVAIL)
            continue;
        if(value != 0) {
#ifdef BE_HOST
            val_lo = COMPILER_64_LO(value);
            val_hi = COMPILER_64_HI(value);
            COMPILER_64_ZERO(value);
            COMPILER_64_SET(value, val_lo, val_hi);
#endif
            sal_printf("Port:%d ctype:%s ctr_val:%llu\n",portid,port_stat[type],value);
        }
    }
    return SOC_E_NONE;
}
#endif
