/*
 * $Copyright: (c) 2017 Broadcom Limited
 */

/*
 * phymod.h
 */

#ifndef SOC_PHYMOD_H
#define SOC_PHYMOD_H


#define ERR_CODE_TYPE int8

#define ERR_CODE_NONE                 0
#define ERR_CODE_MICRO_INIT_NOT_DONE 13
#define ERR_CODE_UCODE_LOAD_FAIL     14 
#define ERR_CODE_CORE_DP_NOT_RESET   29
#define ERR_CODE_LANE_DP_NOT_RESET   30
#define ERR_CODE_UC_ACTIVE           37
#define ERR_CODE_PMI_WRITE           126
#define ERR_CODE_PMI_READ            127



#ifdef PHYMOD_VIPER_SUPPORT
extern ERR_CODE_TYPE qsgmii_phy_probe(uint8 unit, uint8 phy_id, uint8 speed, uint8 num_lanes);
#endif

#ifdef PHYMOD_EAGLE_SUPPORT
extern ERR_CODE_TYPE eagle_phy_1_probe (uint8 unit, uint8 phy_id, uint8 speed);
#endif

#if defined(PHYMOD_VIPER_SUPPORT) || defined(PHYMOD_EAGLE_SUPPORT)
extern void stdk_soc_xlport_init(uint8 unit);
extern void xlport_reset(uint8 unit);
#endif

extern int plp_orca_phy_8481_probe(int unit);
extern int plp_orca_phy_8481_init(int unit);

#define SGMII_1G   1000
#define SGMII_2p5G 2500

#define WRITE_XLPORT_SOC_REG(unit, reg_offset, data, len) stdk_soc_xlport_reg_write(unit, 0,reg_offset, data, len, 1)
#define READ_XLPORT_SOC_REG(unit, reg_offset, data, len) stdk_soc_xlport_reg_read(unit, 0, reg_offset, data, len, 1)


#define WRITE_XLPORT_SOC_REG_IDX(unit, port_id, reg_offset, data, len) stdk_soc_xlport_reg_write(unit, port_id, reg_offset, data, len, 0)
#define READ_XLPORT_SOC_REG_IDX(unit, port_id, reg_offset, data, len) stdk_soc_xlport_reg_read(unit, port_id, reg_offset, data, len, 0)

extern void stdk_soc_xlport_reg_write (uint8 unit, uint8 port_idx, uint32_t addr, uint32 *data_buf, uint8 len, uint8 reg_type);
extern int stdk_soc_xlport_reg_read (uint8 unit, uint8 port_idx, uint32_t addr, uint32_t *data_buf, uint8 len, uint8 reg_type);

#endif /* SOC_PHYMOD_H */
