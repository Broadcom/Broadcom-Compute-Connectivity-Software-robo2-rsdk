/*
 * $ID:$
 * $Copyright: (c) 2017 Broadcom Corp.
 * All Rights Reserved$
 *
 * Filename:
 *    defs.h
 * Description:
 *    chip based defintions
 */

#ifndef __DEFS_H
#define __DEFS_H

/* Place holder */
/****************************************************************
 *
 * This is a list of all known chips which may or may not be supported
 * by a given image.
 *
 * Use soc_chip_supported() for which are supported
 * CHANGE soc_chip_type_map if you change this
 *
 * See also socchip.h
 *
 * Enumerated types include:
 *   Unit numbers
 *   Chip type numbers (specific chip references w/ rev id)
 *   Chip group numbers (groups of chips, usually dropping revision
 *        number)
 *
 * All of the above are 0-based.
 *
 ****************************************************************/

typedef enum soc_chip_types_e {
    SOC_CHIP_BCM53158_A0,
    SOC_CHIP_BCM53158_B0,
    SOC_CHIP_BCM53158_B1,
    SOC_CHIP_TYPES_COUNT,
} soc_chip_types;


/*
 * If you add anything here, check soc/common.c for arrays
 * indexed by soc_regtype_t.
 */
typedef enum soc_regtype_t {
    soc_schan_reg,      /* Generic register read thru SCHAN */
    soc_genreg,         /* General soc registers */
    soc_portreg,        /* Port soc registers */
    soc_ppportreg,      /* Packet-Processing-Port soc registers */
    soc_cosreg,         /* COS soc registers */
    soc_pipereg,        /* per pipe soc registers */
    soc_xpereg,         /* per XPE soc registers */
    soc_slicereg,       /* per slice soc registers */
    soc_layerreg,       /* per layer soc registers */
    soc_cpureg,         /* AKA PCI memory */
    soc_pci_cfg_reg,    /* PCI configuration space register */
    soc_phy_reg,        /* PHY register, access thru mii */
    soc_spi_reg,        /* SPI relevant Registers*/
    soc_mcsreg,         /* Microcontroller Subsystem - Indirect Access */
    soc_iprocreg,       /* iProc Reg in AXI Address Space */
    soc_hostmem_w,      /* word */
    soc_hostmem_h,      /* half word */
    soc_hostmem_b,      /* byte */
    soc_customreg,      /* Custom Register */
    soc_invalidreg
} soc_regtype_t;


#define SOC_MAX_NUM_DEVICES 2

#endif /* __DEFS_H */
