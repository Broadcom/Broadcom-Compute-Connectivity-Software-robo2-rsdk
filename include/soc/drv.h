/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved.$
 * File:
 *    drv.h
 * Description
 *    drv.h
 */

#ifndef __DRV_H
#define __DRV_H

#include <soc/chip.h>
#include <soc/register.h>
#include <soc/memory.h>
#include <soc/counter.h>
#include <soc/error.h>
#include <soc/drv_if.h>
#include <soc/robo2/common/memregs.h>
#include <sal_task.h>
#include <sal_sync.h>


#define BROADCOM_VENDOR_ID 0x14e4

#define BCM53112_DEVICE_ID 0xCF78
#define BCM53112_A0_REV_ID 0x0
#define BCM53112_B0_REV_ID 0x10

#define BCM53154_DEVICE_ID 0xCFA2
#define BCM53154_B1_REV_ID 0x11

#define BCM53156_DEVICE_ID 0xCFA4
#define BCM53156_A0_REV_ID 0x0
#define BCM53156_B0_REV_ID 0x10

#define BCM53157_DEVICE_ID 0xCFA5
#define BCM53157_A0_REV_ID 0x0
#define BCM53157_B0_REV_ID 0x10

#define BCM53158_DEVICE_ID 0xCFA6
#define BCM53158_A0_REV_ID 0x0
#define BCM53158_B0_REV_ID 0x10
#define BCM53158_B1_REV_ID 0x11

#define BCM53161_DEVICE_ID 0xCFA9
#define BCM53161_A0_REV_ID 0x0
#define BCM53161_B0_REV_ID 0x10

#define BCM53162_DEVICE_ID 0xCFAA
#define BCM53162_A0_REV_ID 0x0
#define BCM53162_B0_REV_ID 0x10

#define BCM53168_DEVICE_ID 0xCFB0
#define BCM53168_A0_REV_ID 0x0
#define BCM53168_B0_REV_ID 0x10

#define BCM53158_CHIP_ID_REG    0x402001a0
#define BCM53158_CHIP_REV_REG   0x402001a4

#define ROBO2_REG_APB2PBUS_WRITE_WITH_ID(u, r, p, i, v) \
    DRV_APB2PBUS_REG_WRITE(u, p, DRV_REG_ADDR(u, r, p, i), v, DRV_REG_LENGTH_GET(u, r))

#define ROBO2_REG_APB2PBUS_READ_WITH_ID(u, r, p, i, v) \
    DRV_APB2PBUS_REG_READ(u, p, DRV_REG_ADDR(u, r, p, i), v, DRV_REG_LENGTH_GET(u, r))

/****************************************************************
 * UNIT DRIVER ACCESS MACROS
 *
 *         MACRO                             EVALUATES TO
 *  ________________________________________________________________
 *      SOC_DRIVER(unit)                Chip driver structure
 *      SOC_INFO(unit)                  SOC Info structure
 *      SOC_MEM_INFO(unit,mem)          Memory info structure
 *      SOC_REG_INFO(unit,reg)          Register info structure
 *      SOC_BLOCK_INFO(unit,blk)        Block info structure
 *      SOC_PORT_INFO(unit,port)        Port info structure
 *      SOC_BLOCK2SCH(unit,blk)         Integer schan num for block
 *      SOC_BLOCK2OFFSET(unit,blk)      Block to idx for cmic cmds
 *      SOC_HAS_CTR_TYPE(unit, ctype)   Does the device have a given
 *                                      counter map defined?
 *      SOC_CTR_DMA_MAP(unit, ctype)    Return pointer to the counter
 *                                      map of the indicated type.
 *      SOC_CTR_TO_REG(unit, ctype, ind) Return the register index for
 *                                       a given counter index.
 *      SOC_CTR_MAP_SIZE(unit, ctype)   How many entries in a given
 *                                      counter map.
 ****************************************************************/

#define SOC_CONTROL(unit)               (soc_control[unit])
#define SOC_DRIVER(unit)                (SOC_CONTROL(unit)->chip_driver)
#define SOC_FUNCTIONS(unit)             (SOC_CONTROL(unit)->soc_functions)
#define SOC_PORTCTRL_FUNCTIONS(unit)    (SOC_CONTROL(unit)->soc_portctrl_functions)
#define SOC_INFO(unit)                  (SOC_CONTROL(unit)->info)
#define SOC_STAT(unit)                  (&(SOC_CONTROL(unit)->stat))
#define SOC_REG_MASK_SUBSET(unit)       (SOC_CONTROL(unit)->reg_subset_mask)

#if defined (CONFIG_EXTERNAL_HOST) || defined (CONFIG_BOOTLOADER)
    #define SOC_REG_PTR(unit, reg)      (SOC_DRIVER(unit)->reg_info[reg])
    #define SOC_MEM_PTR(unit, mem)      (SOC_DRIVER(unit)->mem_info[mem])
#else
    extern uint32_t reloc_data_offset;
    #define SOC_REG_PTR(unit, reg)      ((soc_reg_info_t *)((uint32_t)SOC_DRIVER(unit)->reg_info[reg]+reloc_data_offset))
    #define SOC_MEM_PTR(unit, mem)      ((soc_mem_info_t *)((uint32_t)SOC_DRIVER(unit)->mem_info[mem]+reloc_data_offset))
#endif

#define DRV_SERVICES(unit)              (SOC_DRIVER(unit)->services)
#define SOC_MEM_INFO(unit, mem)         (*(SOC_MEM_PTR(unit, mem))
#define SOC_MEM_AGGR(unit, index)       (SOC_DRIVER(unit)->mem_aggr[index])
#define SOC_MEM_UNIQUE_ACC(unit, mem)   (SOC_DRIVER(unit)->mem_unique_acc[mem])

#define SOC_REG_INFO(unit, reg)         (*(SOC_REG_PTR(unit, reg)))
#define SOC_REG_FIELD_INFO(unit, reg)   (&SOC_DRIVER(unit)->field_info[(SOC_REG_INFO(unit, reg).fidx)])
#define SOC_REG_STAGE(unit, reg)        ((SOC_REG_INFO(unit, reg).offset >> 26) & 0x3F)
#define SOC_REG_UNIQUE_ACC(unit, reg)   (SOC_DRIVER(unit)->reg_unique_acc[reg])

#define SOC_BLOCK_INFO(unit, blk)       (SOC_DRIVER(unit)->block_info[blk])
#define SOC_PORT_INFO(unit, port)       (SOC_DRIVER(unit)->port_info[port])

#define SOC_CHIP_STRING(unit)           (SOC_DRIVER(unit)->chip_string)
#define SOC_ORIGIN(unit)                (SOC_DRIVER(unit)->origin)

#define SOC_APB2PBUS_ACCESS(unit)       (SOC_CONTROL(unit)->apb2pbus_access)

#define SOC_IS_ROBO2_ARCH(unit) \
              (SOC_INFO(unit).chip_type == SOC_INFO_CHIP_TYPE_AVENGER)
#define SOC_IS_ROBO2(unit) (SOC_IS_ROBO2_ARCH(unit))

#define SOC_MCAST_ADD_ALL_ROUTER_PORTS(unit) \
    SOC_CONTROL(unit)->mcast_add_all_router_ports

#define SOC_MAC_LOW_POWER_ENABLED(unit) \
    SOC_CONTROL(unit)->mac_low_power_enabled

#define SOC_AUTO_MAC_LOW_POWER(unit) \
        SOC_CONTROL(unit)->auto_mac_low_power

#define SOC_MAX_COUNTER_NUM 47
/*array size=32*47(max 32 ports and  per port 47 sw mib counters) */
#define SW_COUNT_ARR        1504

#ifndef XGXS_SLEEP_US
#define XGXS_SLEEP_US 1000
#endif


/*
 * Cascaded Mode
 */
#define SOC_ROBO2_CASCADED_ADDR (1 << 26)
#define SOC_IS_CASCADED(unit) (SOC_INFO(unit).cascaded_mode == 1)
#define drv_robo2_map_to_device_addr(unit, addr)  \
             ((SOC_IS_CASCADED(0) && (unit > 0)) ? (addr | SOC_ROBO2_CASCADED_ADDR) : \
                       (addr & ~(SOC_ROBO2_CASCADED_ADDR)))



#define SOC_REG_IS_64(unit, reg)     \
    (SOC_REG_INFO(unit, reg).flags & SOC_REG_FLAG_64_BITS)

#define SOC_REG_IS_ABOVE_32(unit, reg) \
    (SOC_REG_INFO(unit, reg).flags & (SOC_REG_FLAG_64_BITS | SOC_REG_FLAG_ABOVE_64_BITS))

#define SOC_REG_IS_32(unit, reg) \
    (SOC_REG_INFO(unit, reg).flags & SOC_REG_FLAG_32_BITS)

#define SOC_REG_IS_16(unit, reg) \
    (SOC_REG_INFO(unit, reg).flags & SOC_REG_FLAG_16_BITS)

#define SOC_REG_IS_8(unit, reg) \
    (SOC_REG_INFO(unit, reg).flags & SOC_REG_FLAG_8_BITS)

#define SOC_REG_IS_VALID(unit, reg) \
    ((reg >= 0 && reg < NUM_SOC_REG) && \
      (SOC_REG_PTR(unit, reg) != NULL))

#define SOC_REG_IS_ENABLED(unit, reg) \
    (SOC_REG_IS_VALID(unit, reg) && \
     !(SOC_REG_INFO(unit, reg).flags & \
       SOC_CONTROL(unit)->disabled_reg_flags))


#define SOC_ROBO_DRIVER_ACTIVE(i) (soc_robo2_base_driver_table[i]->block_info)

    /* Find an active chip to get reg info from base driver table. */
#define SOC_ROBO_FIRST_ACTIVE_DRIVER(chip) \
     do { \
     chip = 0; \
     while (!SOC_ROBO_DRIVER_ACTIVE(chip) && chip < SOC_ROBO2_NUM_SUPPORTED_CHIPS) { \
           chip++; \
     } \
     SAL_ASSERT(chip < SOC_ROBO2_NUM_SUPPORTED_CHIPS); \
     } while (0)


#define SOC_UNIT_NUM_VALID(unit)        (((unit) >= 0) &&                  \
                                         ((unit) < SOC_MAX_NUM_DEVICES))

#define SOC_UNIT_VALID(unit)            (SOC_UNIT_NUM_VALID(unit) && \
                                         (SOC_CONTROL(unit) != NULL))

#define SOC_PORT(unit,ptype,pno)        (SOC_INFO(unit).ptype.port[pno])
#define SOC_PORT_NUM(unit,ptype)        (SOC_INFO(unit).ptype.num)
#define SOC_PORT_MIN(unit,ptype)        (SOC_INFO(unit).ptype.min)
#define SOC_PORT_MAX(unit,ptype)        (SOC_INFO(unit).ptype.max)
#define SOC_PORT_BITMAP(unit,ptype)     (SOC_INFO(unit).ptype.bitmap)
#define SOC_PORT_DISABLED_BITMAP(unit,ptype) \
    (SOC_INFO(unit).ptype.disabled_bitmap)

#define NUM_FE_PORT(unit)               SOC_PORT_NUM(unit,fe)
#define NUM_GE_PORT(unit)               SOC_PORT_NUM(unit,ge)
#define NUM_XE_PORT(unit)               SOC_PORT_NUM(unit,xe)
#define NUM_GX_PORT(unit)               SOC_PORT_NUM(unit,gx)
#define NUM_XL_PORT(unit)               SOC_PORT_NUM(unit,xl)
#define NUM_XG_PORT(unit)               SOC_PORT_NUM(unit,xg)
#define NUM_PORT(unit)                  SOC_PORT_NUM(unit,port)
#define NUM_ALL_PORT(unit)              SOC_PORT_NUM(unit,all)
#define MAX_PORT(unit)                  (SOC_INFO(unit).port_num)

/*
 * E is ethernet port (FE|GE|XE)
 * PORT is all net ports (E|HG|IL)
 * ALL is all ports (PORT|CMIC)
 */
#define PBMP_FE_ALL(unit)               SOC_PORT_BITMAP(unit,fe)
#define PBMP_GE_ALL(unit)               SOC_PORT_BITMAP(unit,ge)
#define PBMP_XE_ALL(unit)               SOC_PORT_BITMAP(unit,xe)
#define PBMP_GX_ALL(unit)               SOC_PORT_BITMAP(unit,gx)
#define PBMP_XL_ALL(unit)               SOC_PORT_BITMAP(unit,xl)
#define PBMP_PORT_ALL(unit)             SOC_PORT_BITMAP(unit,port)
#define PBMP_LB(unit)                   SOC_INFO(unit).lb_pbm
#define PBMP_ALL(unit)                  SOC_PORT_BITMAP(unit,all)

/* Dummy MACROS, needs to be updated */
#define PBMP_CPU(unit)                  (PBMP_LOCAL_CPU(unit)|PBMP_EXTERNAL_CPU(unit)) /* CPU ports:Ports 14, 15 */
#define PBMP_LOCAL_CPU(unit)            0x8000 /* CPU ports:Port 15 */
#define PBMP_EXTERNAL_CPU(unit)         (1<<CBX_PORT_ECPU) /* External CPU Port */
#define PBMP_CASCADE(unit)              SOC_INFO(unit).cascade_ports
#define SOC_TRAP_DEST(unit)             SOC_INFO(unit).trap_dest




/* use PORT_MIN/_MAX to be more efficient than PBMP_ITER */
#define _SOC_PBMP_ITER(_u,_pt,_p)       \
   for ((_p) = SOC_PORT_MIN(_u,_pt); \
        (_p) >= 0 && (_p) <= SOC_PORT_MAX(_u,_pt); \
        (_p)++) \
           if (_SHR_PBMP_MEMBER(SOC_PORT_BITMAP(_u,_pt), (_p)))

#define PBMP_FE_ITER(_u, _p)            _SOC_PBMP_ITER(_u,fe,_p)
#define PBMP_GE_ITER(_u, _p)            _SOC_PBMP_ITER(_u,ge,_p)
#define PBMP_XE_ITER(_u, _p)            _SOC_PBMP_ITER(_u,xe,_p)
#define PBMP_GX_ITER(_u, _p)            _SOC_PBMP_ITER(_u,gx,_p)
#define PBMP_XL_ITER(_u, _p)            _SOC_PBMP_ITER(_u,xl,_p)
#define PBMP_LB_ITER(_u, _p)            _SOC_PBMP_ITER(_u,lbport,_p)
#define PBMP_PORT_ITER(_u, _p)          _SOC_PBMP_ITER(_u,port,_p)
#define PBMP_ALL_ITER(_u, _p)           _SOC_PBMP_ITER(_u,all,_p)

#define IS_S_PORT(unit, port)           \
        (SOC_PBMP_MEMBER(SOC_INFO(unit).s_pbm, port))
#define IS_GMII_PORT(unit, port)        \
        (SOC_PBMP_MEMBER(SOC_INFO(unit).gmii_pbm, port))
#define IS_FE_PORT(unit,port)           \
        (SOC_PBMP_MEMBER(PBMP_FE_ALL(unit), port))
#define IS_GE_PORT(unit,port)           \
        (SOC_PBMP_MEMBER(PBMP_GE_ALL(unit), port))
#define IS_XE_PORT(unit,port)           \
        (SOC_PBMP_MEMBER(PBMP_XE_ALL(unit), port))
#define IS_GX_PORT(unit,port)           \
        (SOC_PBMP_MEMBER(PBMP_GX_ALL(unit), port))
#define IS_XL_PORT(unit,port)           \
        (SOC_PBMP_MEMBER(PBMP_XL_ALL(unit), port))
#define IS_LB_PORT(unit,port)           \
        (SOC_PBMP_MEMBER(PBMP_LB(unit), port))
#define IS_CPU_PORT(unit,port)          \
        (SOC_PBMP_MEMBER((PBMP_CPU(unit) & PBMP_ALL(unit)), port))
#define IS_SPI_BLK(unit,port)      \
        (SOC_PORT_TYPE(unit, port) == SOC_BLK_SPI)
#define IS_ALL_PORT(unit,port)          \
        (SOC_PORT_TYPE(unit, port) != 0)
#define IS_LOCAL_CPU_PORT(unit,port)          \
        (SOC_PBMP_MEMBER(PBMP_LOCAL_CPU(unit), port))
#define IS_EXTERNAL_CPU_PORT(unit,port)          \
        (SOC_PBMP_MEMBER((PBMP_EXTERNAL_CPU(unit) & PBMP_ALL(unit)), port))



typedef enum {
    SOC_INFO_CHIP_TYPE_AVENGER,
} soc_chip_e;

/*
 * soc_info_t is part of the per-unit soc_control_t.
 * It gets filled in at run time based on the port_info and block_info
 * from the driver structure.
 */

typedef struct {
    int     port[SOC_MAX_NUM_PORTS];
    int     num;                    /* number of entries used in port[] */
    int     min;                    /* minimum bit on */
    int     max;                    /* maximum bit on */
    pbmp_t  bitmap;
    pbmp_t  disabled_bitmap;        /* ports that are forced to be disabled */
} soc_ptype_t;


/*
 * Typedef: soc_ipg_t
 * Purpose: IFG register setting for all speed/duplex combinations
 */

typedef struct soc_ipg_s {
    uint32      hd_10;
    uint32      hd_100;
    uint32      hd_1000;
    uint32      hd_2500;
    uint32      fd_10;
    uint32      fd_100;
    uint32      fd_1000;
    uint32      fd_2500;
    uint32      fd_10000;
    uint32      fd_xe;
    uint32      fd_hg;
    uint32      fd_hg2;
    uint32      fe_hd_10;
    uint32      fe_hd_100;
    uint32      fe_fd_10;
    uint32      fe_fd_100;
    uint32      gth_hd_10;
    uint32      gth_hd_100;
    uint32      gth_fd_10;
    uint32      gth_fd_100;
    uint32      ge_hd_1000;
    uint32      ge_fd_1000;
    uint32      ge_hd_2500;
    uint32      ge_fd_2500;
    uint32      bm_fd_10000;
} soc_ipg_t;

/*
 * Typedef: soc_stat_t
 * Purpose: Driver statistics counts (interrupts, errors, etc).
 */
typedef struct soc_stat_s {
    uint32      intr;           /* Total interrupt count */
    uint32      intr_ls;        /* Link status interrupt count */
    uint32      intr_stats;     /* Stats DMA interrupt count */
    uint32      arl_msg_ins;    /* Count of ARL insert messages processed */
    uint32      arl_msg_del;    /* Count of ARL delete messages processed */
    uint32      arl_msg_bad;    /* Count of bad messages processed */
    uint32      arl_msg_tot;    /* Count of all ARL messages */
} soc_stat_t;


/*
 * Typedef: soc_driver_t
 * Purpose: Chip driver.  All info about a particular device type.
 * Notes: These structures are automatically generated.
 */
typedef struct soc_driver_s {
    soc_chip_types          type;                /* the chip type id */
    char                    *chip_string;        /* chip string for var defs */
    uint16                  pci_vendor;          /* vendor id */
    uint16                  pci_device;          /* device id */
    uint8                   pci_revision;        /* revision */
    int                     num_cos;             /* classes of service */
    soc_reg_info_t          **reg_info;          /* register array */
    soc_field_info_t        *field_info;         /* fields array */
    soc_mem_info_t          **mem_info;          /* memory array */
    soc_robo2_block_info_t  *block_info;         /* block array */
    soc_port_info_t         *port_info;          /* port array */
    void                    *feature;            /* boolean feature function */
} soc_driver_t;


typedef struct {
    soc_chip_types    driver_type;          /* chip type (with rev) */

    soc_chip_e        chip_type;            /* chip type enum - used instead of 'chip'
                                               for newer devices as the bitmap was exhausted */
    uint32            spi_device;

    uint32           chip;                  /* chip id bits */

    int             port_num;               /* count of entries in port_info */
    int             block_num;              /* count of entries in block_info */
    int             port_offset[SOC_MAX_NUM_PORTS];       /* 3 for ge3 */
    soc_ptype_t     fe;
    soc_ptype_t     ge;
    soc_ptype_t     xl;                     /* xl (Warpcore) */
    soc_ptype_t     xe;
    soc_ptype_t     qsgmii;                 /* QSGMII ports */
    soc_ptype_t     lbport;                 /* loopback port */
    soc_ptype_t     spi;                    /* spi interface port */
    soc_ptype_t     spi_subport;            /* spi subport */
    soc_ptype_t     port;                   /* fe|ge|xe|hg|spi|fl */
    soc_ptype_t     ether;                  /* fe|ge|xe */
    soc_ptype_t     all;                    /* fe|ge|xe|hg|cmic|fl */
    char            block_valid[SOC_MAX_NUM_BLKS];        /* is block used? */
    pbmp_t          block_bitmap[SOC_MAX_NUM_BLKS];
    pbmp_t          s_pbm;                  /* 2.5 Gpbs Comboserdes ports */
    pbmp_t          gmii_pbm;               /* GMII port bitmap */
    pbmp_t          spi_bitmap;             /* spi bitmap */
    soc_block_t     has_block[SOC_MAX_NUM_BLKS]; /* list of all blocks */
    int             block_port[SOC_MAX_NUM_BLKS];
    int             num_cpu_cosq;           /* number of cpu cosq */
    int             port_addr_max;          /* max addressable ports */
    int             modid_count;            /* needed modids */
    int             modid_max;              /* maximum modid supported */
    int             ipic_port;              /* ipic port or -1 */
    int             ipic_block;             /* ipic block or -1 */
    int             exp_port;               /* expansion port or -1 */
    int             exp_block;              /* expansion block or -1 */
    int             spi_port;               /* spi port */ /* Add for Robo */
    int             spi_block;              /* spi block */
    int             arl_block;              /* arl block */
    int             mmu_block;              /* mmu block */
    int             mcu_block;              /* mcu block */
    int             inter_block;            /* internal mii block*/
    int             exter_block;            /* external mii block*/
    char            block_name[SOC_MAX_NUM_BLKS][16];
#if 0
    int             mem_block_any[NUM_SOC_MEM];
#endif

    int             port_init_speed[SOC_MAX_NUM_PORTS];   /* ports initial speed */
    int             port_speed_max[SOC_MAX_NUM_PORTS];    /* max port speed */
    char            port_name[SOC_MAX_NUM_PORTS][11];
    int             port_type[SOC_MAX_NUM_PORTS];
    int             max_mtu;               /*max size of system mtu*/
    int             cascaded_mode;         /* Set if cascaded mode is enabled */
    int             cascade_ports;         /* Bitmap of cascade ports */
    int             trap_dest;             /* PGID of trap dest */
    int             qsgmii_enable;         /* Ports 8-11 */
    int             sgmii_speed_1g;
    int             xlport_enable;         /* Ports 10-13 */

} soc_info_t;

typedef int (*soc_misc_init_f)(int);
typedef int (*soc_mmu_init_f)(int);
typedef int (*soc_age_timer_get_f)(int, int *, int *);
typedef int (*soc_age_timer_max_get_f)(int, int *);
typedef int (*soc_age_timer_set_f)(int, int, int);
typedef int (*soc_phy_firmware_load_f)(int, int, uint8 *, int);
typedef int (*soc_sbus_mdio_read_f)(int, uint32, uint32, uint32 *);
typedef int (*soc_sbus_mdio_write_f)(int, uint32, uint32, uint32);

/*
 * Typedef: soc_functions_t
 * Purpose: Chip driver functions that are not automatically generated.
 */
typedef struct soc_functions_s {
    /* Get/set age timer value and enable in device, if supported */
    soc_misc_init_f                    soc_misc_init;
    soc_mmu_init_f                     soc_mmu_init;
    soc_age_timer_get_f                soc_age_timer_get;
    soc_age_timer_max_get_f            soc_age_timer_max_get;
    soc_age_timer_set_f                soc_age_timer_set;
    soc_phy_firmware_load_f            soc_phy_firmware_load;
    soc_sbus_mdio_read_f               soc_sbus_mdio_read;
    soc_sbus_mdio_write_f              soc_sbus_mdio_write;
} soc_functions_t;

/*
 * Typedef: soc_control_t
 * Purpose: SOC Control Structure.  All info about a device instance.
 */

typedef struct soc_control_s {
    uint32      soc_flags; /* Flags for this device */
#define   SOC_F_ATTACHED           0x01    /* Device attached */
#define   SOC_F_INITED             0x02    /* Device inited */
    /* Mask of flags that are persistent across init */
#define   SOC_F_RESET              (SOC_F_ATTACHED|SOC_F_INITED)

    /* Port and block information filled in when attached or inited */
    soc_info_t  info;

    void *drv;

    /* Disable set of register based on flags */
    uint32      disabled_reg_flags;

    uint32      irq_mask;       /* Cached copy of IRQ_MASK */

    uint32      switch_event_nominal_storm;

    uint32      board_type;     /* From I2C */
    uint32      remote_cpu;
    int         mcast_add_all_router_ports;
    soc_ipg_t   ipg;

    /* Statistics counts */
    soc_stat_t  stat;
    soc_cmap_t  *counter_map[SOC_MAX_NUM_PORTS];        /* per port cmap */
#ifdef COUNTER_THREAD
    uint64_t    counter_hw_val[SW_COUNT_ARR];        /* Hardware counters */
    uint64_t    counter_sw_val[SW_COUNT_ARR];        /* Software accumulated counters */
#endif
    volatile sal_thread_t counter_pid;  /* Counter DMA emulation task */
    volatile sal_sem_t counter_notify;     /* Scan complete notification */
    char        counter_name[16];       /* Counter task name */
    volatile int     counter_interval;       /* Update interval in usec */
                                        /*   (Zero when thread not running) */
    volatile pbmp_t  counter_pbmp;           /* Ports being scanned */
    volatile uint32  counter_flags;          /* SOC_COUNTER_F_XXX */
    volatile int     counter_sync_req;       /* soc_counter_sync() support */



    /* MAC low power mode for Robo chips */
    uint32      mac_low_power_enabled; /* currently low power mode */
    uint32      auto_mac_low_power; /* Automatic enable low power mode */

    uint32 all_link_down_detected;  /* Timestamp of event that all ports link down is of type sal-time_t  */

    /* Chip driver pointers */
    soc_driver_t *chip_driver;
    soc_functions_t  *soc_functions;
    /* Link status */
    pbmp_t      link_mask;     /* Link status mask */

#if 0 /* Add after SAL layer is added */
    sal_mutex_t miimMutex;      /* MII registers mutual exclusion */
    sal_mutex_t arl_table_Mutex;
    sal_mutex_t arl_mem_search_Mutex;
#endif
    sal_mutex_t apb2pbus_access;

} soc_control_t;

extern soc_control_t    *soc_control[SOC_MAX_NUM_DEVICES];
extern int              soc_ndev_attached;
extern int              soc_ndev;
extern int      soc_eth_ndev;
extern int      soc_eth_unit;

#define SOC_SPI_DEV_TYPE      SAL_SPI_DEV_TYPE    /* SPI device */
#define SOC_SWITCH_DEV_TYPE   SAL_SWITCH_DEV_TYPE /* Switch device */
#define SOC_ETHER_DEV_TYPE    SAL_ETHER_DEV_TYPE  /* Ethernet device */
#define SOC_CPU_DEV_TYPE      SAL_CPU_DEV_TYPE    /* CPU device */

/* ************* OTP flags *************** */
#define SOC_OTP_FLAG_GPHY_1G_DIS        0x0001
#define SOC_OTP_FLAG_RGMII_DIS          0x0002
#define SOC_OTP_FLAG_GPHY_0_3_DIS       0x0004
#define SOC_OTP_FLAG_GPHY_4_7_DIS       0x0008
#define SOC_OTP_FLAG_XFI_13_DIS         0x0010
#define SOC_OTP_FLAG_XFI_12_DIS         0x0020
#define SOC_OTP_FLAG_QSG_MODE_QSG       0x0040
#define SOC_OTP_FLAG_QSG_MODE_SGM       0x0080
#define SOC_OTP_FLAG_QSGMII_DIS         0x0100
#define SOC_OTP_FLAG_SGMII_2G5_DIS      0x0200
#define SOC_OTP_FLAG_SGMII_1G_DIS       0x0400
#define SOC_OTP_FLAG_ARL_SIZE_8K        0x0800
#define SOC_OTP_FLAG_CFP_DIS            0x1000
#define SOC_OTP_FLAG_XFI_13_MAX_2G5     0x2000
#define SOC_OTP_FLAG_XFI_12_MAX_2G5     0x4000
#define SOC_OTP_FLAG_XLPORT_DISABLE     0x8000
#define SOC_OTP_FLAG_PORT_9_DISABLE     0x10000

extern uint32_t otp_flags;


extern int soc_init(int unit);
extern int soc_reboot(int unit);
extern int soc_robo2_reboot(int unit);
extern int soc_robo2_counter_attach(int unit);
extern int soc_robo2_counter_detach(int unit);
extern int soc_robo2_do_init(int unit, int reset);
extern int soc_robo2_deinit(int unit);
extern int soc_robo2_reset(int unit);
extern int soc_robo2_attach(int unit);
extern int soc_robo2_detach(int unit);
extern int soc_robo2_reset_init(int unit);
extern int soc_pkt_send(uint32 port, int vlan, uint32 mgid, int type, int mode);
extern int soc_pktio_init(int type);
extern void dump_hpa_regs(int unit);
extern void dump_cb_counters(int unit);
extern void dump_port_status(int unit);
extern void dump_l2_table(int unit);
extern int soc_robo2_essential_init(int unit);
#endif /* __DRV_H */
