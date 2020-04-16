/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 */

/*
 * $ tables.h  $
 */

#ifdef UM_FLASH_BUILD

typedef struct {
    uint8  ntype;
    uint16 etype;
    uint8  length;
    uint8  tci;
    uint8  ekey_type;
} av_ipp_stt_t;

#endif /* UM_FLASH_BUILD */

extern int soc_robo2_table_read(uint8 unit, uint32 status_reg_address,  uint32 index);
extern int soc_robo2_table_write(uint8 unit, uint32 status_reg_address,  uint32 index);
extern int soc_robo2_table_write_unit(uint8 unit, 
                uint32 status_reg_address,  uint32 index);
extern int soc_robo2_ipp_vsit_enable(uint8 unit);
extern int soc_robo2_etm_pet_enable(uint8 unit);
extern int soc_robo2_itm_arlfm0_enable(uint8 unit);
extern int soc_robo2_itm_arlfm1_ght_h0_enable(uint8 unit);
extern int soc_robo2_itm_arlfm1_ght_h1_enable(uint8 unit);
extern int soc_robo2_itm_pglct_enable(uint8 unit);
extern int soc_robo2_ipp_pgt_enable(uint8 unit);
extern int soc_robo2_ipp_pp2lpg_enable(uint8 unit);
extern int soc_robo2_pqm_pp2lpg_enable(uint8 unit);
extern int soc_robo2_pqm_lpg2pg_enable(uint8 unit);
extern int soc_robo2_pqm_lpg2ppfov_enable(uint8 unit);
extern int soc_robo2_pqm_qfccredit_enable(uint8 unit);
extern int soc_robo2_pqm_qfcqdaddr_enable(uint8 unit);
extern int soc_robo2_pqm_pg2lpg_enable(uint8 unit);
extern int soc_robo2_pqm_lpg2ap_enable(uint8 unit);
extern int soc_robo2_pqm_apt_enable(uint8 unit);
extern int soc_robo2_pqs_max_config_enable(uint8 unit);
extern int soc_robo2_pqm_svt_enable(uint8 unit);
extern int soc_robo2_pqm_svt_entry_write(uint8 unit, uint32 index, uint16 svt_mask);

#ifdef UM_FLASH_BUILD
extern int soc_robo2_ipp_stt_enable(uint8 unit);
extern int soc_robo2_ipp_stt_entry_write(uint8 unit, uint32 index, av_ipp_stt_t *ipp_stt);
//#if defined(CFG_AUTO_QOS) || defined(CFG_AUTO_LOOP)
extern int soc_robo2_ipp_cpmt_enable(uint8 unit);
extern int soc_robo2_pqm_qdschcfg_enable(uint8 unit);
extern int soc_robo2_bmu_fcd_config_enable(uint8 unit);
extern int soc_robo2_bmu_fcd_cntr_enable(uint8 unit);
extern int soc_robo2_bmu_spg_pp_bmp_config_enable(uint8 unit);
extern int soc_robo2_ipp_profile_enable(uint8 unit);
extern int soc_robo2_ipp_pptr_enable(uint8 unit);
extern int soc_robo2_ipp_bucket_enable(uint8 unit);
extern int soc_robo2_ipp_rscale_enable(uint8 unit);
extern int soc_robo2_ipp_pmon_enable(uint8 unit);
extern int soc_robo2_pqm_tc2qd_entry_write(uint8 unit, uint32 index, uint8 qd);
extern int soc_robo2_ipp_cpmt_entry_write(uint8 unit, uint32 index, uint8 tc, uint8 dp);
//#endif /* CFG_AUTO_QOS || CFG_AUTO_LOOP */
#if defined(CFG_AUTO_DOS) || defined(CFG_AUTO_LOOP) || defined(CFG_CASCADE_SUPPORT)
extern int soc_robo2_ipp_tct_enable(uint8 unit);
#endif /* CFG_AUTO_DOS || CFG_AUTO_LOOP */
#if defined(CFG_AUTO_VOIP) || defined(CFG_AUTO_LOOP)
extern int soc_robo2_ipp_slicmap_enable(uint8 unit);
extern int soc_robo2_ipp_srt_enable(uint8 unit);
extern int soc_robo2_ipp_rpslict_enable(uint8 unit);
extern int soc_robo2_ipp_fpslict_enable(uint8 unit);
#endif /* CFG_AUTO_VOIP || CFG_AUTO_LOOP */
#if defined(CFG_AUTO_QOS) || defined(CFG_AUTO_VOIP)
extern int soc_robo2_pqm_tc2qd_enable(uint8 unit);
#endif /* CFG_AUTO_QOS || CFG_AUTO_VOIP */

#if defined(CFG_AUTO_LOOP) || defined(CFG_CASCADE_SUPPORT)
extern int soc_robo2_pqm_dli2ldpg_enable(uint8 unit);
extern int soc_robo2_pqm_litet_enable(uint8 unit);
extern int soc_robo2_pqm_mtgt_enable(uint8 unit);
#endif /* CFG_AUTO_LOOP || CFG_CASCADE_SUPPORT */
extern int soc_robo2_table_copy_read_to_write_reg(uint8 unit, uint32 source, uint32 dest, uint32 count);

#endif /* UM_FLASH_BUILD */
