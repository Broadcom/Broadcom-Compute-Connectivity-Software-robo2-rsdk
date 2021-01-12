/*
 * $Id: $
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved.$
 *
 * File:	slic.c
 * Purpose:	Robo2 SLIC TCAM programming routines
 */

#include <fsal_int/slic.h>
#include <fsal_int/trap.h>
#include <fsal_int/port.h>
#include <fsal_int/lin.h>
#include <fsal_int/lif.h>
#ifdef CONFIG_PORT_EXTENDER
#include <fsal_int/extender.h>
#endif /* CONFIG_PORT_EXTENDER */
#include <fsal_int/native_types.h>
#include <soc/robo2/common/tables.h>
#include <bsl_log/bslenum.h>
#include "sal_get_set_param.h"

static slicmap_info_t slicmap_sw_info;

extern uint8_t cbxi_ptp_eth_mac_addr[6];
extern uint8_t cbxi_ptp_eth_p2p_mac_addr[6];
extern uint8_t cbxi_ptp_udp_mac_addr[6];
extern uint8_t cbxi_ptp_udp_p2p_mac_addr[6];

/*
 * Function:
 *  cbxi_slic_rule_validate
 * Purpose:
 *  Make a given rule valid / invalid
 *
 * @return return code
 * @retval 0 success.
 */
int
cbxi_slic_rule_validate(
        int unit, cbxi_slic_rule_id_t rule_number, uint8_t valid)
{
    slicmap_t slict_ent_key0;
    uint32 status = 0;

#ifdef EMULATION_BUILD
    if (SOC_DRIVER(unit)->type == SOC_CHIP_BCM53158_B0) {
        /* reduce SLIC rules for emulation */
        if (rule_number < SLIC_PORT_BASED_VLAN) {
            return CBX_E_NONE;
        } else {
            rule_number -= SLIC_PORT_BASED_VLAN;
        }
    }
#endif

    CBX_IF_ERROR_RETURN(soc_robo2_slicmap_get(
                    unit, (rule_number * 2),&slict_ent_key0));
    if (!valid) {
        slict_ent_key0.valid_key0 = 0x0;
    } else {
        slict_ent_key0.valid_key0 = 0x3;
    }
    /* Write updated entry to SLICT */
    CBX_IF_ERROR_RETURN(soc_robo2_slicmap_set(
            unit, (rule_number * 2), &slict_ent_key0, &status));

    return CBX_E_NONE;
}


/*
 * Function:
 *  cbxi_slicmap_rule_get
 * Purpose:
 *  Form the rule at the TCAM index given by rule number.
 *
 * @return return code
 * @retval 0 success.
 */

int cbxi_slicmap_rule_get(int unit,
        cbxi_slic_rule_id_t rule_number, cbxi_slic_rule_t *rule)
{
    slicmap_info_t *sli = &slicmap_sw_info;
    slicmap_t slict_ent_key0, slict_ent_key1;
    uint32_t flags = 0;
    uint8_t  pe_en = 0;

    CHECK_SLIC_INIT(sli);

    LOG_INFO(BSL_LS_FSAL_COMMON,
                (BSL_META("cbxi_slicmap_rule_get()..\n")));

    if (rule == NULL) {
        return CBX_E_PARAM;
    }

#ifdef EMULATION_BUILD
    if (SOC_DRIVER(unit)->type == SOC_CHIP_BCM53158_B0) {
        /* reduce SLIC rules for emulation */
        if (rule_number < SLIC_PORT_BASED_VLAN) {
            return CBX_E_NONE;
        } else {
            rule_number -= SLIC_PORT_BASED_VLAN;
        }
    }
#endif

    sal_memset(rule, 0, sizeof(cbxi_slic_rule_t));

    /* Retrive SLIC info in two consecutive entries */
    CBX_IF_ERROR_RETURN(soc_robo2_slicmap_get(
                    unit, (rule_number * 2),&slict_ent_key0));
    CBX_IF_ERROR_RETURN(soc_robo2_slicmap_get(
                    unit, ((rule_number * 2) + 1) ,&slict_ent_key1));

#ifdef CONFIG_PORT_EXTENDER
    if (cbxi_check_port_extender_mode() == CBX_E_NONE) {
        pe_en = 1;
    }
#endif /* CONFIG_PORT_EXTENDER */

    if (pe_en != 1) {
        /* FIXME: PE mode requires pgmap update for non valid entries also  */
        /* Check for rule validity */
        if (slict_ent_key0.valid_key0 != 0x3) {
            LOG_ERROR(BSL_LS_FSAL_COMMON,(BSL_META("cbxi_slicmap_rule_get: \
            No valid rule preset at index : %d\n"),rule_number));
            return CBX_E_EMPTY;
        }
    }

    /* Populate pepper fields */
    if ((slict_ent_key0.pepper) || (slict_ent_key1.pepper)) {
        rule->pepper = slict_ent_key0.pepper;
        rule->pepper_mask = (slict_ent_key0.pepper | slict_ent_key1.pepper);
        flags |= ROBO2_SLIC_MASK_PEPPER;
    }
    /* Populate pg map: Since pg_map is programmed for don't care codition,
     * use key1.pg_map only */
    rule->pg_map = ~slict_ent_key1.pg_map;
    flags |= ROBO2_SLIC_MASK_PGMAP;

    /* Populate DMAC */
    if ((slict_ent_key0.dmac_lo16) || (slict_ent_key1.dmac_lo16)) {
        rule->dmac_lo_16 = slict_ent_key0.dmac_lo16;
        rule->dmac_lo_mask =
                        slict_ent_key0.dmac_lo16 | slict_ent_key1.dmac_lo16;
        flags |= ROBO2_SLIC_MASK_DMAC;
    }
    if ((slict_ent_key0.dmac_hi32) || (slict_ent_key1.dmac_hi32)) {
        rule->dmac_hi_32 = slict_ent_key0.dmac_hi32;
        rule->dmac_hi_mask =
                        slict_ent_key0.dmac_hi32 | slict_ent_key1.dmac_hi32;
        flags |= ROBO2_SLIC_MASK_DMAC;
    }
    /* Populate SMAC */
    if ((slict_ent_key0.smac_hi16) || (slict_ent_key1.smac_hi16)) {
        rule->smac_hi_16 = slict_ent_key0.smac_hi16;
        rule->smac_hi_mask =
                        slict_ent_key0.smac_hi16 | slict_ent_key1.smac_hi16;
        flags |= ROBO2_SLIC_MASK_SMAC;
    }
    if ((slict_ent_key0.smac_lo32) || (slict_ent_key1.smac_lo32)) {
        rule->smac_lo_32 = slict_ent_key0.smac_lo32;
        rule->smac_lo_mask =
                        slict_ent_key0.smac_lo32 | slict_ent_key1.smac_lo32;
        flags |= ROBO2_SLIC_MASK_SMAC;
    }
    /* Populate etype */
    if ((slict_ent_key0.etype) || (slict_ent_key1.etype)) {
        rule->etype = slict_ent_key0.etype;
        flags |= ROBO2_SLIC_MASK_ETYPE;
    }
    /* Populate TPID0 */
    if ((slict_ent_key0.tpid0) || (slict_ent_key1.tpid0)) {
        rule->tpid0 = slict_ent_key0.tpid0;
        flags |= ROBO2_SLIC_MASK_TPID0;
    }
    /* Populate TPID1 */
    if ((slict_ent_key0.tpid1) || (slict_ent_key1.tpid1)) {
        rule->tpid1 = slict_ent_key0.tpid1;
        flags |= ROBO2_SLIC_MASK_TPID1;
    }
    /* Populate TCI */
    if ((slict_ent_key0.valid) || (slict_ent_key1.valid)) {
        rule->tci_valid = slict_ent_key0.valid;
        rule->tci_valid_mask = (slict_ent_key0.valid | slict_ent_key1.valid);
        flags |= ROBO2_SLIC_MASK_TCI;

        if(((rule->tci_valid_mask >> ROBO2_SLIC_TCI0_SHIFT) & 0x1) &&
                        ((slict_ent_key0.tci0) || (slict_ent_key1.tci0))) {
            rule->tci0 = slict_ent_key0.tci0;
            rule->tci0_mask = (slict_ent_key0.tci0 | slict_ent_key1.tci0);
        }
        if(((rule->tci_valid_mask >> ROBO2_SLIC_TCI1_SHIFT) & 0x1) &&
                        ((slict_ent_key0.tci1) || (slict_ent_key1.tci1))) {
            rule->tci1 = slict_ent_key0.tci1;
            rule->tci1_mask = (slict_ent_key0.tci1 | slict_ent_key1.tci1);
        }
        if(((rule->tci_valid_mask >> ROBO2_SLIC_TCI2_SHIFT) & 0x1) &&
                        ((slict_ent_key0.tci2) || (slict_ent_key1.tci2))) {
            rule->tci2 = slict_ent_key0.tci2;
            rule->tci2_mask = (slict_ent_key0.tci2 | slict_ent_key1.tci2);
        }
        if(((rule->tci_valid_mask >> ROBO2_SLIC_TCI3_SHIFT) & 0x1) &&
                        ((slict_ent_key0.tci3) || (slict_ent_key1.tci3))) {
            rule->tci3 = slict_ent_key0.tci3;
            rule->tci3_mask = (slict_ent_key0.tci3 | slict_ent_key1.tci3);
        }
    }

    rule->mask_flags = flags;

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_slicmap_rule_set
 * Purpose:
 *  Set a rule at a slot number
 *  Set appropriate mask flags in soc_slic_ent_t structure passed.
 *
 * @return return code
 * @retval 0 success.
 */

int cbxi_slicmap_rule_set(int unit,
        cbxi_slic_rule_id_t rule_number, cbxi_slic_rule_t rule)
{
    slicmap_info_t *sli = &slicmap_sw_info;
    slicmap_t slict_ent_key0, slict_ent_key1;
    uint8_t etype;
    uint32 status = 0;

    CHECK_SLIC_INIT(sli);

    LOG_INFO(BSL_LS_FSAL_COMMON,
                (BSL_META("cbxi_slicmap_rule_set()..\n")));

#ifdef EMULATION_BUILD
    if (SOC_DRIVER(unit)->type == SOC_CHIP_BCM53158_B0) {
        /* reduce SLIC rules for emulation */
        if (rule_number < SLIC_PORT_BASED_VLAN) {
            return CBX_E_NONE;
        } else {
            rule_number -= SLIC_PORT_BASED_VLAN;
        }
    }
#endif

    /* Retrive SLIC info in two consecutive entries */
    CBX_IF_ERROR_RETURN(soc_robo2_slicmap_get(
                    unit, (rule_number * 2),&slict_ent_key0));
    CBX_IF_ERROR_RETURN(soc_robo2_slicmap_get(
                    unit, ((rule_number * 2) + 1) ,&slict_ent_key1));

    sal_memset(&slict_ent_key0, 0, sizeof(slicmap_t));
    sal_memset(&slict_ent_key1, 0, sizeof(slicmap_t));


    if ((rule.mask_flags) & ROBO2_SLIC_MASK_PEPPER) {
        slict_ent_key0.pepper = (rule.pepper & rule.pepper_mask);
        slict_ent_key1.pepper = (~rule.pepper & rule.pepper_mask);;
    }
    if ((rule.mask_flags) & ROBO2_SLIC_MASK_PGMAP) {
        /* Special case for pgmap: Need to program don't care
         * (key0[n]=key1[n]=0) for ports to be matched.
         * Program zero for others */
        slict_ent_key0.pg_map = 0;
        slict_ent_key1.pg_map = ~(uint16) rule.pg_map;
    }
    if ((rule.mask_flags) & ROBO2_SLIC_MASK_DMAC) {
        slict_ent_key0.dmac_lo16 = (rule.dmac_lo_16 & rule.dmac_lo_mask);
        slict_ent_key0.dmac_hi32 = (rule.dmac_hi_32 & rule.dmac_hi_mask);
        slict_ent_key1.dmac_lo16 = (~rule.dmac_lo_16 & rule.dmac_lo_mask);
        slict_ent_key1.dmac_hi32 = (~rule.dmac_hi_32 & rule.dmac_hi_mask);;
    }
    if ((rule.mask_flags) & ROBO2_SLIC_MASK_SMAC) {
        slict_ent_key0.smac_lo32 = (rule.smac_lo_32 & rule.smac_lo_mask);
        slict_ent_key0.smac_hi16 = (rule.smac_hi_16 & rule.smac_hi_mask);
        slict_ent_key1.smac_lo32 = (~rule.smac_lo_32 & rule.smac_lo_mask);;
        slict_ent_key1.smac_hi16 = (~rule.smac_hi_16 & rule.smac_hi_mask);;
    }
    if ((rule.mask_flags) & ROBO2_SLIC_MASK_ETYPE) {
        etype = (uint16_t)rule.etype;
        slict_ent_key0.etype = etype;
        slict_ent_key1.etype = ~etype;
    }
    if ((rule.mask_flags) & ROBO2_SLIC_MASK_TPID0) {
        slict_ent_key0.tpid0 = rule.tpid0;
        slict_ent_key1.tpid0 = ~(slict_ent_key0.tpid0);
    }
    if ((rule.mask_flags) & ROBO2_SLIC_MASK_TPID1) {
        slict_ent_key0.tpid1 = rule.tpid1;
        slict_ent_key1.tpid1 = ~(slict_ent_key0.tpid1);
    }
    if ((rule.mask_flags) & ROBO2_SLIC_MASK_TCI) {
        slict_ent_key0.valid = (rule.tci_valid & rule.tci_valid_mask);
        slict_ent_key1.valid = (~rule.tci_valid & rule.tci_valid_mask);
        if ((rule.tci0_mask) &&
                ((rule.tci_valid_mask >> ROBO2_SLIC_TCI0_SHIFT) & 0x1)) {
            slict_ent_key0.tci0 = (rule.tci0 & rule.tci0_mask);
            slict_ent_key1.tci0 = (~rule.tci0 & rule.tci0_mask);
        }
        if ((rule.tci1_mask) &&
                ((rule.tci_valid_mask >> ROBO2_SLIC_TCI1_SHIFT) & 0x1)) {
            slict_ent_key0.tci1 = (rule.tci1 & rule.tci1_mask);
            slict_ent_key1.tci1 = (~rule.tci1 & rule.tci1_mask);
        }
        if ((rule.tci2_mask) &&
                ((rule.tci_valid_mask >> ROBO2_SLIC_TCI2_SHIFT) & 0x1)) {
            slict_ent_key0.tci2 = (rule.tci2 & rule.tci2_mask);
            slict_ent_key1.tci2 = (~rule.tci2 & rule.tci2_mask);
        }
        if ((rule.tci3_mask) &&
                ((rule.tci_valid_mask >> ROBO2_SLIC_TCI3_SHIFT) & 0x1)) {
            slict_ent_key0.tci3 = (rule.tci3 & rule.tci3_mask);
            slict_ent_key1.tci3 = (~rule.tci3 & rule.tci3_mask);
        }
        if ((rule.tci4_mask) &&
                ((rule.tci_valid_mask >> ROBO2_SLIC_TCI4_SHIFT) & 0x1)) {
            slict_ent_key0.tci4 = (rule.tci4 & rule.tci4_mask);
            slict_ent_key1.tci4 = (~rule.tci4 & rule.tci4_mask);
        }
    }

    slict_ent_key0.valid_key0 = 0x3;
    slict_ent_key1.valid_key0 = 0x0;

    /* Write updated entry to SLICT */
    CBX_IF_ERROR_RETURN(soc_robo2_slicmap_set(
            unit, (rule_number * 2), &slict_ent_key0, &status));
    CBX_IF_ERROR_RETURN(soc_robo2_slicmap_set(
            unit, ((rule_number * 2) + 1) , &slict_ent_key1, &status));

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_slicmap_table_init
 * Purpose:
 *  Initialize SLICMAP table.
 *
 */

STATIC int
cbxi_slic_table_init(int unit)
{
    uint32_t regval, tmp;

    /* Enable TCAM */
    CBX_IF_ERROR_RETURN(REG_READ_CB_IPP_SLICTCAM_CONFIGr(unit, &regval));
    tmp = 0;
    soc_CB_IPP_SLICTCAM_CONFIGr_field_set(unit, &regval, TCAM_PWRDWNf, &tmp);
    tmp = 1;
    soc_CB_IPP_SLICTCAM_CONFIGr_field_set(unit, &regval, TCAM_RESETBf, &tmp);
    CBX_IF_ERROR_RETURN(REG_WRITE_CB_IPP_SLICTCAM_CONFIGr(unit, &regval));

    /* Enable SLIMAP table */
    CBX_IF_ERROR_RETURN(soc_robo2_slicmap_enable(unit));
    /* Reset SLICMAP table */
    CBX_IF_ERROR_RETURN(soc_robo2_slicmap_reset(unit));

    /* Enable RPSLICT */
    CBX_IF_ERROR_RETURN(soc_robo2_rpslict_enable(unit));
    /* Reset RPSLICT */
    CBX_IF_ERROR_RETURN(soc_robo2_rpslict_reset(unit));

    /* Enable FPSLICT */
    CBX_IF_ERROR_RETURN(soc_robo2_fpslict_enable(unit));
    /* Reset FPSLICT */
    CBX_IF_ERROR_RETURN(soc_robo2_fpslict_reset(unit));

    /* Enable HDT */
    CBX_IF_ERROR_RETURN(soc_robo2_hdt_enable(unit));
    /* Reset HDT */
    CBX_IF_ERROR_RETURN(soc_robo2_hdt_reset(unit));

    /* Enable SRT */
    CBX_IF_ERROR_RETURN(soc_robo2_srt_enable(unit));
    /* Reset SRT */
    CBX_IF_ERROR_RETURN(soc_robo2_srt_reset(unit));

    /* Enable STT */
    CBX_IF_ERROR_RETURN(soc_robo2_stt_enable(unit));
    /* Reset STT */
    CBX_IF_ERROR_RETURN(soc_robo2_stt_reset(unit));

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_soft_tag_init
 * Purpose:
 *  Initialize Soft Tag Table entries.
 *
 * @return return code
 * @retval 0 success.
 */

int cbxi_soft_tag_init(int unit)
{
    stt_t ent_stt;
    uint32_t status = 0;

#if 0 /* First 3 entries of Soft tag table are being used by pktio module */
    /* Alternate STAG */
    CBX_IF_ERROR_RETURN(soc_robo2_stt_get(unit, SOFT_TAG_ALT_STAG, &ent_stt));
    ent_stt.etype     = 0x9100;
    ent_stt.ntype     = CBXI_NT_STAG;
    ent_stt.ekey_type = 1; /* TPID = 1 */
    ent_stt.length    = 2; /* 2 bytes */
    ent_stt.tci       = 0; /* TCI index = 0 */
    CBX_IF_ERROR_RETURN(soc_robo2_stt_set(
                        unit, SOFT_TAG_ALT_STAG, &ent_stt, &status));

    /* Alternate CTAG */
    CBX_IF_ERROR_RETURN(soc_robo2_stt_get(unit, SOFT_TAG_ALT_CTAG, &ent_stt));
    ent_stt.etype     = 0xFFFF;
    ent_stt.ntype     = CBXI_NT_CTAG;
    ent_stt.ekey_type = 2; /* TPID = 2 */
    ent_stt.length    = 2; /* 2 bytes */
    ent_stt.tci       = 1; /* TCI index = 1 */
    CBX_IF_ERROR_RETURN(soc_robo2_stt_set(
                        unit, SOFT_TAG_ALT_STAG, &ent_stt, &status));

    /* Autoloop control */
    CBX_IF_ERROR_RETURN(soc_robo2_stt_get(unit, SOFT_TAG_AUTOLOOP, &ent_stt));
    ent_stt.etype     = 0xFF74;
    ent_stt.ntype     = CBXI_NT_UTAG0;
    ent_stt.ekey_type = 8; /* Etype field for SLIC in SLICMAP table */
    ent_stt.length    = 0;
    CBX_IF_ERROR_RETURN(soc_robo2_stt_set(
                        unit, SOFT_TAG_AUTOLOOP, &ent_stt, &status));
#endif
    /* EAPOL control */
    CBX_IF_ERROR_RETURN(soc_robo2_stt_get(unit, SOFT_TAG_EAPOL, &ent_stt));
    ent_stt.etype     = 0x888E;
    ent_stt.ntype     = CBXI_NT_UTAG1;
    ent_stt.ekey_type = 9; /* Etype field for SLIC in SLICMAP table */
    ent_stt.length    = 0;
    CBX_IF_ERROR_RETURN(soc_robo2_stt_set(
                        unit, SOFT_TAG_EAPOL, &ent_stt, &status));

    /* MIM Lite */
    CBX_IF_ERROR_RETURN(soc_robo2_stt_get(unit, SOFT_TAG_MIM_LITE, &ent_stt));
    ent_stt.etype     = 0x8842;
    ent_stt.ntype     = CBXI_NT_ITAG;
    ent_stt.ekey_type = 4; /* TPID = 4 */
    ent_stt.length    = 4;
    ent_stt.tci       = 2;
    CBX_IF_ERROR_RETURN(soc_robo2_stt_set(
                        unit, SOFT_TAG_MIM_LITE, &ent_stt, &status));

    /* LLDP */
    CBX_IF_ERROR_RETURN(soc_robo2_stt_get(unit, SOFT_TAG_LLDP, &ent_stt));
    ent_stt.etype     = 0x88CC;
    ent_stt.ntype     = CBXI_NT_UTAG2;
    ent_stt.ekey_type = ETYPE_USER_ETYPE10;
    ent_stt.length    = 0;
    CBX_IF_ERROR_RETURN(soc_robo2_stt_set(
                        unit, SOFT_TAG_LLDP, &ent_stt, &status));

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_hdr_decap_init
 * Purpose:
 *  Initialize Header Decapsulation Table entries.
 *
 * @return return code
 * @retval 0 success.
 */

int cbxi_hdr_decap_init(int unit)
{
    hdt_t ent_hdt;
    uint32_t status = 0;


    CBX_IF_ERROR_RETURN(soc_robo2_hdt_get(unit, HDT_CTAG_DECAP, &ent_hdt));
    ent_hdt.type0 = CBXI_NT_CP2SW;
    ent_hdt.which0 = 1;
    ent_hdt.type1 = CBXI_NT_CTAG;
    ent_hdt.which1 = 1;
    CBX_IF_ERROR_RETURN(soc_robo2_hdt_set(unit,
                        HDT_CTAG_DECAP, &ent_hdt, &status));

    CBX_IF_ERROR_RETURN(soc_robo2_hdt_get(unit, HDT_STAG_DECAP, &ent_hdt));
    ent_hdt.type0 = CBXI_NT_STAG;
    ent_hdt.which0 = 1;
    CBX_IF_ERROR_RETURN(soc_robo2_hdt_set(unit,
                        HDT_STAG_DECAP, &ent_hdt, &status));

    CBX_IF_ERROR_RETURN(soc_robo2_hdt_get(unit, HDT_STAG_CTAG_DECAP, &ent_hdt));
    ent_hdt.type0 = CBXI_NT_STAG;
    ent_hdt.which0 = 1;
    ent_hdt.type1 = CBXI_NT_CTAG;
    ent_hdt.which1 = 1;
    CBX_IF_ERROR_RETURN(soc_robo2_hdt_set(unit,
                        HDT_STAG_CTAG_DECAP, &ent_hdt, &status));

    CBX_IF_ERROR_RETURN(soc_robo2_hdt_get(unit, HDT_ETAG_DECAP, &ent_hdt));
    ent_hdt.type0 = CBXI_NT_ETAG;
    ent_hdt.which0 = 1;
    CBX_IF_ERROR_RETURN(soc_robo2_hdt_set(unit,
                        HDT_ETAG_DECAP, &ent_hdt, &status));

    CBX_IF_ERROR_RETURN(soc_robo2_hdt_get(unit, HDT_MIM_DECAP, &ent_hdt));
    ent_hdt.type0 = CBXI_NT_MAC;
    ent_hdt.which0 = 1;
    CBX_IF_ERROR_RETURN(soc_robo2_hdt_set(unit,
                        HDT_MIM_DECAP, &ent_hdt, &status));

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_slicid_init
 * Purpose:
 *  Initialize RPSLICT and FPSLICT entries.
 *
 * @return return code
 * @retval 0 success.
 */

int cbxi_slicid_init(int unit, cbxi_slicid_t slicid)
{
    rpslict_t ent_rpslict;
    fpslict_t ent_fpslict;
    uint32_t status = 0;

    if ((slicid < 1) || (slicid >= SLICID_MAX_INDEX)) {
        return CBX_E_PARAM;
    }

    /* Configure RPSLIC table */
    CBX_IF_ERROR_RETURN(soc_robo2_rpslict_get(unit, slicid, &ent_rpslict));
    switch (slicid) {
    case SLICID_USE_DP:
        ent_rpslict.use_dp = 2;
        ent_rpslict.default_dp  = 3;
        break;
    case SLICID_EARLY_ACK:
        ent_rpslict.early_ack = 1;
        break;
    default:
        break;
    };
    CBX_IF_ERROR_RETURN(soc_robo2_rpslict_set(
                        unit, slicid, &ent_rpslict, &status));

    /* Configure FPSLIC table */
    CBX_IF_ERROR_RETURN(soc_robo2_fpslict_get(unit, slicid, &ent_fpslict));
    switch (slicid) {
    case SLICID_MPLS_ARL:
        ent_fpslict.src_key = ARL_KEY_FID_MPLS;
        ent_fpslict.dst_key = ARL_KEY_FID_MPLS;
        break;
    case SLICID_MIM_ARL:
        ent_fpslict.src_key = ARL_KEY_FID_MIM;
        ent_fpslict.dst_key = ARL_KEY_FID_MIM;
        break;
    case SLICID_MIM_ARL_N_DECAP:
        ent_fpslict.src_key = ARL_KEY_FID_MIM;
        ent_fpslict.dst_key = ARL_KEY_FID_MIM;
        ent_fpslict.pv_decap = HDT_MIM_DECAP;
        break;
    case SLICID_EAPOL:
        break;
    case SLICID_PV_TYPE:
        ent_fpslict.default_sli = 0x20001;
        break;
    case SLICID_PV_TYPE_STAG:
        ent_fpslict.pv2li_map_en = 1;
        ent_fpslict.pv_decap = HDT_STAG_DECAP;
        break;
    case SLICID_PV_TYPE_CTAG:
    case SLICID_LWIP:
        ent_fpslict.pv2li_map_en = 1;
        ent_fpslict.pv_decap = HDT_CTAG_DECAP;
        break;
    case SLICID_LIN_TYPE_STAG:
        ent_fpslict.pv2li_map_en = 1;
        ent_fpslict.pv_decap = HDT_STAG_DECAP;
#ifdef CONFIG_VIRTUAL_PORT_SUPPORT
        /* Enable STAG + CTAG decap on LIM0  */
        ent_fpslict.li_map0_en = 1;
        ent_fpslict.li_map0 = CBXI_LIM_M2TK_PVV_DECAP;
        ent_fpslict.li_map0_decap = HDT_STAG_CTAG_DECAP;
        /* Enable only STAG decap on LIM1  */
        ent_fpslict.li_map1_en = 1;
        ent_fpslict.li_map1 = CBXI_LIM_M2TK_PVV;
        ent_fpslict.li_map1_decap = HDT_STAG_DECAP;
#endif
        break;
    case SLICID_LIN_TYPE_CTAG:
        ent_fpslict.pv2li_map_en = 1;
        ent_fpslict.pv_decap = HDT_CTAG_DECAP;
#ifdef CONFIG_VIRTUAL_PORT_SUPPORT
        /* Enable CTAG decap on LIM0  */
        ent_fpslict.li_map0_en = 1;
        ent_fpslict.li_map0 = CBXI_LIM_M2TK_PV_DECAP;
        ent_fpslict.li_map0_decap = HDT_CTAG_DECAP;
        /* No decap enabled for on LIM1  */
        ent_fpslict.li_map1_en = 1;
        ent_fpslict.li_map1 = CBXI_LIM_M2TK_PV;
        ent_fpslict.li_map1_decap = 0;
#endif
        break;
    case SLICID_PE_CS_IN_ECID_U0_P0:
    case SLICID_PE_CS_IN_ECID_U0_P1:
    case SLICID_PE_CS_IN_ECID_U0_P2:
    case SLICID_PE_CS_IN_ECID_U0_P3:
    case SLICID_PE_CS_IN_ECID_U0_P4:
    case SLICID_PE_CS_IN_ECID_U0_P5:
    case SLICID_PE_CS_IN_ECID_U0_P6:
    case SLICID_PE_CS_IN_ECID_U0_P7:
    case SLICID_PE_CS_IN_ECID_U0_P8:
    case SLICID_PE_CS_IN_ECID_U0_P9:
    case SLICID_PE_CS_IN_ECID_U0_P10:
    case SLICID_PE_CS_IN_ECID_U0_P11:
    case SLICID_PE_CS_IN_ECID_U0_P12:
    case SLICID_PE_CS_IN_ECID_U0_P13:
    case SLICID_PE_CS_IN_ECID_U0_P14:
    case SLICID_PE_CS_IN_ECID_U0_P15:
    case SLICID_PE_CS_IN_ECID_U1_P0:
    case SLICID_PE_CS_IN_ECID_U1_P1:
    case SLICID_PE_CS_IN_ECID_U1_P2:
    case SLICID_PE_CS_IN_ECID_U1_P3:
    case SLICID_PE_CS_IN_ECID_U1_P4:
    case SLICID_PE_CS_IN_ECID_U1_P5:
    case SLICID_PE_CS_IN_ECID_U1_P6:
    case SLICID_PE_CS_IN_ECID_U1_P7:
    case SLICID_PE_CS_IN_ECID_U1_P8:
    case SLICID_PE_CS_IN_ECID_U1_P9:
    case SLICID_PE_CS_IN_ECID_U1_P10:
    case SLICID_PE_CS_IN_ECID_U1_P11:
    case SLICID_PE_CS_IN_ECID_U1_P12:
    case SLICID_PE_CS_IN_ECID_U1_P13:
    case SLICID_PE_CS_IN_ECID_U1_P14:
    case SLICID_PE_CS_IN_ECID_U1_P15:
    case SLICID_PE_CS_IN_ECID_LAG0:
    case SLICID_PE_CS_IN_ECID_LAG1:
    case SLICID_PE_CS_IN_ECID_LAG2:
    case SLICID_PE_CS_IN_ECID_LAG3:
    case SLICID_PE_CS_IN_ECID_LAG4:
    case SLICID_PE_CS_IN_ECID_LAG5:
    case SLICID_PE_CS_IN_ECID_LAG6:
    case SLICID_PE_CS_IN_ECID_LAG7:
    case SLICID_PE_CS_IN_ECID_LAG8:
    case SLICID_PE_CS_IN_ECID_LAG9:
    case SLICID_PE_CS_IN_ECID_LAG10:
    case SLICID_PE_CS_IN_ECID_LAG11:
    case SLICID_PE_CS_IN_ECID_LAG12:
    case SLICID_PE_CS_IN_ECID_LAG13:
    case SLICID_PE_CS_IN_ECID_LAG14:
    case SLICID_PE_CS_IN_ECID_LAG15:
    case SLICID_PE_CS_IN_ECID_LAG16:
    case SLICID_PE_CS_IN_ECID_LAG17:
    case SLICID_PE_CS_IN_ECID_LAG18:
    case SLICID_PE_CS_IN_ECID_LAG19:
    case SLICID_PE_CS_IN_ECID_LAG20:
    case SLICID_PE_CS_IN_ECID_LAG21:
    case SLICID_PE_CS_IN_ECID_LAG22:
    case SLICID_PE_CS_IN_ECID_LAG23:
    case SLICID_PE_CS_IN_ECID_LAG24:
    case SLICID_PE_CS_IN_ECID_LAG25:
    case SLICID_PE_CS_IN_ECID_LAG26:
    case SLICID_PE_CS_IN_ECID_LAG27:
    case SLICID_PE_CS_IN_ECID_LAG28:
    case SLICID_PE_CS_IN_ECID_LAG29:
    case SLICID_PE_CS_IN_ECID_LAG30:
    case SLICID_PE_CS_IN_ECID_LAG31:
        ent_fpslict.dst_key   = 4;  /* FID_GRP_ECID */
        /* FIXME: */
        /* Access PE : Set rules on Uplink port and Decap ETAG
         * Transit PE: Set rules on Loopback port P14 and Decap ETAG*/
        ent_fpslict.slic_decap = HDT_ETAG_DECAP;
        /* default_sli is set in PE module */
        break;
    case SLICID_PE_CS_ETAG_DECAP:
        ent_fpslict.slic_decap = HDT_ETAG_DECAP;
        ent_fpslict.dst_key   = 4;  /* FID_GRP_ECID */
        /* default_sli is set in PE module */
        break;
    case SLICID_PE_IN_ECID_U0_P0:
    case SLICID_PE_IN_ECID_U0_P1:
    case SLICID_PE_IN_ECID_U0_P2:
    case SLICID_PE_IN_ECID_U0_P3:
    case SLICID_PE_IN_ECID_U0_P4:
    case SLICID_PE_IN_ECID_U0_P5:
    case SLICID_PE_IN_ECID_U0_P6:
    case SLICID_PE_IN_ECID_U0_P7:
    case SLICID_PE_IN_ECID_U0_P8:
    case SLICID_PE_IN_ECID_U0_P9:
    case SLICID_PE_IN_ECID_U0_P10:
    case SLICID_PE_IN_ECID_U0_P11:
    case SLICID_PE_IN_ECID_U0_P12:
    case SLICID_PE_IN_ECID_U0_P13:
    case SLICID_PE_IN_ECID_U0_P14:
    case SLICID_PE_IN_ECID_U0_P15:
    case SLICID_PE_IN_ECID_U1_P0:
    case SLICID_PE_IN_ECID_U1_P1:
    case SLICID_PE_IN_ECID_U1_P2:
    case SLICID_PE_IN_ECID_U1_P3:
    case SLICID_PE_IN_ECID_U1_P4:
    case SLICID_PE_IN_ECID_U1_P5:
    case SLICID_PE_IN_ECID_U1_P6:
    case SLICID_PE_IN_ECID_U1_P7:
    case SLICID_PE_IN_ECID_U1_P8:
    case SLICID_PE_IN_ECID_U1_P9:
    case SLICID_PE_IN_ECID_U1_P10:
    case SLICID_PE_IN_ECID_U1_P11:
    case SLICID_PE_IN_ECID_U1_P12:
    case SLICID_PE_IN_ECID_U1_P13:
    case SLICID_PE_IN_ECID_U1_P14:
    case SLICID_PE_IN_ECID_U1_P15:
    case SLICID_PE_IN_ECID_LAG0:
    case SLICID_PE_IN_ECID_LAG1:
    case SLICID_PE_IN_ECID_LAG2:
    case SLICID_PE_IN_ECID_LAG3:
    case SLICID_PE_IN_ECID_LAG4:
    case SLICID_PE_IN_ECID_LAG5:
    case SLICID_PE_IN_ECID_LAG6:
    case SLICID_PE_IN_ECID_LAG7:
    case SLICID_PE_IN_ECID_LAG8:
    case SLICID_PE_IN_ECID_LAG9:
    case SLICID_PE_IN_ECID_LAG10:
    case SLICID_PE_IN_ECID_LAG11:
    case SLICID_PE_IN_ECID_LAG12:
    case SLICID_PE_IN_ECID_LAG13:
    case SLICID_PE_IN_ECID_LAG14:
    case SLICID_PE_IN_ECID_LAG15:
    case SLICID_PE_IN_ECID_LAG16:
    case SLICID_PE_IN_ECID_LAG17:
    case SLICID_PE_IN_ECID_LAG18:
    case SLICID_PE_IN_ECID_LAG19:
    case SLICID_PE_IN_ECID_LAG20:
    case SLICID_PE_IN_ECID_LAG21:
    case SLICID_PE_IN_ECID_LAG22:
    case SLICID_PE_IN_ECID_LAG23:
    case SLICID_PE_IN_ECID_LAG24:
    case SLICID_PE_IN_ECID_LAG25:
    case SLICID_PE_IN_ECID_LAG26:
    case SLICID_PE_IN_ECID_LAG27:
    case SLICID_PE_IN_ECID_LAG28:
    case SLICID_PE_IN_ECID_LAG29:
    case SLICID_PE_IN_ECID_LAG30:
    case SLICID_PE_IN_ECID_LAG31:
        ent_fpslict.dst_key   = 4;  /* FID_GRP_ECID */
        /* FIXME: */
        /* Access PE : Set rules on Uplink port and Decap ETAG
         * Transit PE: Set rules on Loopback port P14 and Decap ETAG*/
        ent_fpslict.slic_decap = HDT_ETAG_DECAP;
        /* default_sli is set in PE module */
        break;
    case SLICID_PE_FWD_DWNSTR_UC:
        ent_fpslict.dst_key   = 4;  /* FID_GRP_ECID */
        /* FIXME: */
        /* Access PE : Set rule on Uplink port and Decap ETAG
         * Transit PE: Set rule on Uplink port and no Decap ETAG*/
        /* default_sli is set in PE module */
        break;
    case SLICID_PE_FWD_DWNSTR_MC:
        ent_fpslict.dst_key   = 4;  /* FID_GRP_ECID */
        ent_fpslict.slic_decap = HDT_ETAG_DECAP;
        /* FIXME: */
        /* Access PE : Set rule on Uplink port and Decap ETAG
         * Transit PE: Set rule on Uplink port and no Decap ETAG*/
/*        ent_fpslict.slic_decap = HDT_ETAG_DECAP;*/
        /* default_sli is set in PE module */
        break;
    case SLICID_PE_ETAG_DECAP_CFP:
        ent_fpslict.slic_decap = HDT_ETAG_DECAP;
        ent_fpslict.dst_key   = 4;  /* FID_GRP_ECID */
        /* default_sli is set in PE module
         * The VSI should have Looback port*/
        break;

    case SLICID_PE_ETAG_DECAP:
        ent_fpslict.slic_decap = HDT_ETAG_DECAP;
        ent_fpslict.dst_key   = 4;  /* FID_GRP_ECID */
        /* default_sli is set in PE module */
        break;
    case SLICID_PE_FWD_UPSTR:
        /* default_sli is taken from PGT.
         * PGT.default_sli is set in PE module*/
        break;
    case SLICID_PTP_ETH_PP0:
    case SLICID_PTP_ETH_PP1:
    case SLICID_PTP_ETH_PP2:
    case SLICID_PTP_ETH_PP3:
    case SLICID_PTP_ETH_PP4:
    case SLICID_PTP_ETH_PP5:
    case SLICID_PTP_ETH_PP6:
    case SLICID_PTP_ETH_PP7:
    case SLICID_PTP_ETH_PP8:
    case SLICID_PTP_ETH_PP9:
    case SLICID_PTP_ETH_PP10:
    case SLICID_PTP_ETH_PP11:
    case SLICID_PTP_ETH_PP12:
    case SLICID_PTP_ETH_PP13:
    case SLICID_PTP_ETH_PP14:
    case SLICID_PTP_ETH_PP15:
        ent_fpslict.default_sli = 0x00;
        break;
    case SLICID_PTP_UDP_PP0:
    case SLICID_PTP_UDP_PP1:
    case SLICID_PTP_UDP_PP2:
    case SLICID_PTP_UDP_PP3:
    case SLICID_PTP_UDP_PP4:
    case SLICID_PTP_UDP_PP5:
    case SLICID_PTP_UDP_PP6:
    case SLICID_PTP_UDP_PP7:
    case SLICID_PTP_UDP_PP8:
    case SLICID_PTP_UDP_PP9:
    case SLICID_PTP_UDP_PP10:
    case SLICID_PTP_UDP_PP11:
    case SLICID_PTP_UDP_PP12:
    case SLICID_PTP_UDP_PP13:
    case SLICID_PTP_UDP_PP14:
    case SLICID_PTP_UDP_PP15:
        ent_fpslict.default_sli = 0x00;
        break;
    default:
        break;
    };
    CBX_IF_ERROR_RETURN(soc_robo2_fpslict_set(
                        unit, slicid, &ent_fpslict, &status));
    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_slicid_map
 * Purpose:
 *  Map TCAM index of configured rule to SLICID
 *
 * @return return code
 * @retval 0 success.
 */

int cbxi_slicid_map(int unit, cbxi_slic_rule_id_t rule_number,
                cbxi_slicid_t slicid, cbxi_slic_trap_t slic_trap)
{
    slicmap_info_t *sli = &slicmap_sw_info;
    srt_t ent_srt;
    uint32_t status = 0;

    CHECK_SLIC_INIT(sli);

    LOG_INFO(BSL_LS_FSAL_COMMON,
                (BSL_META("cbxi_slicid_map()\n")));

#ifdef EMULATION_BUILD
    if (SOC_DRIVER(unit)->type == SOC_CHIP_BCM53158_B0) {
        /* reduce SLIC rules for emulation */
        if (rule_number < SLIC_PORT_BASED_VLAN) {
            return CBX_E_NONE;
        } else {
            rule_number -= SLIC_PORT_BASED_VLAN;
        }
    }
#endif

    CBX_IF_ERROR_RETURN(soc_robo2_srt_get(unit, rule_number, &ent_srt));

    ent_srt.slicid = (uint8_t)slicid;
    if (slic_trap != SLIC_TRAP_GROUP0) {
        ent_srt.trap_group = (uint8_t)slic_trap;
    }

    CBX_IF_ERROR_RETURN(soc_robo2_srt_set(
                        unit, rule_number, &ent_srt, &status));

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_default_rule_init
 * Purpose:
 *  Configure a switch default SLIC rule entry at given index.
 *
 * @return return code
 * @retval 0 success.
 */

int cbxi_default_rule_init(int unit, cbxi_slic_rule_id_t rule_idx)
{
    cbxi_slic_rule_t slic_rule;
    cbx_port_type_t port_type = cbxPortTypeCustomer;
    cbxi_slicid_t slicid = SLICID_DEFAULT;
    cbxi_slic_trap_t slic_trap = SLIC_TRAP_GROUP0;
    uint8_t set_rule = 1;
    uint8_t  mac_addr[6] = {0,0,0,0,0,0};
#ifndef CONFIG_EXTERNAL_HOST
    uint32_t  mac_val[6];
    char mac_string[20];
    int rv, i;
#endif

    if (rule_idx > SLIC_MAX_INDEX) {
        return CBX_E_PARAM;
    }

    sal_memset(&slic_rule, 0, sizeof(cbxi_slic_rule_t));

#ifndef CONFIG_EXTERNAL_HOST
    rv = osal_get_param(PARAMS_FILE, MAC_ADDRESS_PARAM_NAME, mac_string, 20);
    if ((rv == SOC_E_NONE) && (sal_sscanf(mac_string, "%x:%x:%x:%x:%x:%x",
        &mac_val[0], &mac_val[1], &mac_val[2], &mac_val[3], &mac_val[4], &mac_val[5]) == 6)) {
        for (i = 0; i < 6; i++) {
            mac_addr[i] = (uint8_t)mac_val[i];
        }
    }
#endif

    switch (rule_idx) {
    case SLIC_CSD_DEV1_DROP:
         /* TPID = CBXI_NT_CP2SW; TCI0 */
         slic_rule.tpid0 = USER_TPID7;
         slic_rule.tci_valid = (1 << ROBO2_SLIC_TCI0_SHIFT);
         slic_rule.tci_valid_mask = (1 << ROBO2_SLIC_TCI0_SHIFT);
         slic_rule.tci0      = 0x0080; /* Match DEVID 1 */
         slic_rule.tci0_mask = 0x0080;
         slic_rule.mask_flags |= ROBO2_SLIC_MASK_TCI;
         slic_rule.mask_flags |= ROBO2_SLIC_MASK_TPID0;

         if (unit) {
             slicid = SLICID_DEFAULT;
             slic_trap = SLIC_TRAP_GROUP3; /* Drop */
         } else {
             slicid = SLICID_DEFAULT; /* Allow on unit 1 CSD port */
         }
         /* Disable this rule on both Avengers at init
          * Enable it only on Secondary while adding cascade port */
         slic_rule.pg_map = 0x0;
         slic_rule.mask_flags |= ROBO2_SLIC_MASK_PGMAP;
         break;
    case SLIC_CSD_DEV0_FWD:
         /* TPID = CBXI_NT_CP2SW; TCI0 */
         slic_rule.tpid0 = USER_TPID7;
         slic_rule.tci_valid = (1 << ROBO2_SLIC_TCI0_SHIFT);
         slic_rule.tci_valid_mask = (1 << ROBO2_SLIC_TCI0_SHIFT);
         slic_rule.tci0      = 0x0000; /* Match DEVID 0 */
         slic_rule.tci0_mask = 0x0080;
         slic_rule.mask_flags |= ROBO2_SLIC_MASK_TCI;
         slic_rule.mask_flags |= ROBO2_SLIC_MASK_TPID0;
         if (unit) {
             slicid = SLICID_DEFAULT;
         } else {
             slicid = SLICID_DEFAULT;
             slic_trap = SLIC_TRAP_GROUP3; /* Drop in unit 0 */
         }
         /* Disable this rule on both Avengers at init
          * Enable it only on Secondary while adding cascade port */
         slic_rule.pg_map = 0x0;
         slic_rule.mask_flags |= ROBO2_SLIC_MASK_PGMAP;

         break;
    case SLIC_MPLS_UC:
         /* etype = 0x8847 */
         slic_rule.etype = ETYPE_UCMPLS;
         slic_rule.mask_flags |= ROBO2_SLIC_MASK_ETYPE;
         slicid = SLICID_MPLS_ARL;
         break;
    case SLIC_MPLS_MC:
         /* etype = 0x8848 */
         slic_rule.etype = ETYPE_MCMPLS;
         slic_rule.mask_flags |= ROBO2_SLIC_MASK_ETYPE;
         slicid = SLICID_MPLS_ARL;
         break;
    case SLIC_MIM_ENDING:
        /* TPID0 = STAG ; TPID1 = 0x88E7 */
        slic_rule.tpid0 = TPID_STAG;
        slic_rule.tpid1 = TPID_ITAG;
        slic_rule.mask_flags |= (ROBO2_SLIC_MASK_TPID0 | ROBO2_SLIC_MASK_TPID1);
        slicid = SLICID_MIM_ARL_N_DECAP;
        break;
    case SLIC_MIM:
        /* TPID0 = STAG ; TPID1 = 0x88E7 */
        slic_rule.tpid0 = TPID_STAG;
        slic_rule.tpid1 = TPID_ITAG;
        slic_rule.mask_flags |= (ROBO2_SLIC_MASK_TPID0 | ROBO2_SLIC_MASK_TPID1);
        slicid = SLICID_MIM_ARL;
        break;
    case SLIC_MIM_LITE_CUST:
        /* TPID0 = CTAG ; TPID1 = 0x8842 */
        slic_rule.tpid0 = TPID_CTAG;
        slic_rule.tpid1 = TPID_ITAG;
        slic_rule.mask_flags |= (ROBO2_SLIC_MASK_TPID0 | ROBO2_SLIC_MASK_TPID1);
        slicid = SLICID_MIM_ARL_N_DECAP;
        break;
    case SLIC_MIM_LITE_TUNNEL:
        /* TPID0 = CTAG ; TPID1 = 0x8842 */
        slic_rule.tpid0 = TPID_CTAG;
        slic_rule.tpid1 = TPID_ITAG;
        slic_rule.mask_flags |= (ROBO2_SLIC_MASK_TPID0 | ROBO2_SLIC_MASK_TPID1);
        slicid = SLICID_MIM_ARL_N_DECAP;
        break;
    case SLIC_MIM_LITE:
        /* TPID0 = CTAG ; TPID1 = 0x8842 */
        slic_rule.tpid0 = TPID_CTAG;
        slic_rule.tpid1 = TPID_ITAG;
        slic_rule.mask_flags |= (ROBO2_SLIC_MASK_TPID0 | ROBO2_SLIC_MASK_TPID1);
        slicid = SLICID_MIM_ARL;
        break;
    case SLIC_DROP_CTAG:
        slic_rule.pepper |= CBX_PORT_PEPPER_DROP_TAGGED;
        slic_rule.pepper_mask |= CBX_PORT_PEPPER_DROP_TAGGED;
        /* Check if CTAG is present */
        slic_rule.tci_valid = (1 << ROBO2_SLIC_TCI1_SHIFT);/* Only TCI1 valid */
        slic_rule.tci_valid_mask = (1 << ROBO2_SLIC_TCI1_SHIFT);
        slic_rule.mask_flags |= (ROBO2_SLIC_MASK_PEPPER | ROBO2_SLIC_MASK_TCI);
        slicid = SLICID_DEFAULT;
        slic_trap = SLIC_TRAP_GROUP2; /* Drop & trap */
        break;
    case SLIC_DROP_STAG:
        slic_rule.pepper |= CBX_PORT_PEPPER_DROP_TAGGED;
        slic_rule.pepper_mask |= CBX_PORT_PEPPER_DROP_TAGGED;
        /* Check if STAG is present */
        slic_rule.tci_valid = (1 << ROBO2_SLIC_TCI0_SHIFT);/* Only TCI0 valid */
        slic_rule.tci_valid_mask = (1 << ROBO2_SLIC_TCI0_SHIFT);
        slic_rule.mask_flags |= (ROBO2_SLIC_MASK_PEPPER | ROBO2_SLIC_MASK_TCI);
        slicid = SLICID_DEFAULT;
        slic_trap = SLIC_TRAP_GROUP2; /* Drop & trap */
        break;
    case SLIC_DROP_UTAG:
        slic_rule.pepper |= CBX_PORT_PEPPER_DROP_UNTAGGED;
        slic_rule.pepper_mask |= CBX_PORT_PEPPER_DROP_UNTAGGED;
        /* Check if both STAG & CTAG are not present */
        slic_rule.tci_valid =
                ((0 << ROBO2_SLIC_TCI1_SHIFT) | (0 << ROBO2_SLIC_TCI0_SHIFT));
        slic_rule.tci_valid_mask =
                ((1 << ROBO2_SLIC_TCI1_SHIFT) | (1 << ROBO2_SLIC_TCI0_SHIFT));
        slic_rule.mask_flags |= (ROBO2_SLIC_MASK_PEPPER | ROBO2_SLIC_MASK_TCI);
        slicid = SLICID_DEFAULT;
        slic_trap = SLIC_TRAP_GROUP2; /* Drop & trap */
        break;
    case SLIC_DROP_PRI_TAG:
        slic_rule.pepper |= CBX_PORT_PEPPER_DROP_UNTAGGED;
        slic_rule.pepper_mask |= CBX_PORT_PEPPER_DROP_UNTAGGED;
        /* Check if only CTAG is present & match to pri tag at TCI1
         * Match only VID = 0 in TCI */
        slic_rule.tci_valid =
                ((1 << ROBO2_SLIC_TCI1_SHIFT) | (0 << ROBO2_SLIC_TCI0_SHIFT));
        slic_rule.tci_valid_mask =
                ((1 << ROBO2_SLIC_TCI1_SHIFT) | (1 << ROBO2_SLIC_TCI0_SHIFT));
        slic_rule.tci1      = 0x0000;
        slic_rule.tci1_mask = 0x0FFF;
        slic_rule.mask_flags |= (ROBO2_SLIC_MASK_PEPPER | ROBO2_SLIC_MASK_TCI);
        slicid = SLICID_DEFAULT;
        slic_trap = SLIC_TRAP_GROUP2; /* Drop & trap */
        break;
    case SLIC_TAS:
        /* SMAC and TCI1 field (PCP) to be set by user */
        slic_rule.mask_flags |= ROBO2_SLIC_MASK_PGMAP;
        slic_rule.pg_map = 0;
        slicid = SLICID_EARLY_ACK;
        break;
#ifdef CONFIG_PTP
    case SLIC_PTP_ETH_PORT_0:
    case SLIC_PTP_ETH_PORT_1:
    case SLIC_PTP_ETH_PORT_2:
    case SLIC_PTP_ETH_PORT_3:
    case SLIC_PTP_ETH_PORT_4:
    case SLIC_PTP_ETH_PORT_5:
    case SLIC_PTP_ETH_PORT_6:
    case SLIC_PTP_ETH_PORT_7:
    case SLIC_PTP_ETH_PORT_8:
    case SLIC_PTP_ETH_PORT_9:
    case SLIC_PTP_ETH_PORT_10:
    case SLIC_PTP_ETH_PORT_11:
    case SLIC_PTP_ETH_PORT_12:
    case SLIC_PTP_ETH_PORT_13:
    case SLIC_PTP_ETH_PORT_14:
    case SLIC_PTP_ETH_PORT_15:
        slic_rule.dmac_hi_32   = (cbxi_ptp_eth_mac_addr[0] << 24) |
                                 (cbxi_ptp_eth_mac_addr[1] << 16) |
                                 (cbxi_ptp_eth_mac_addr[2] << 8) |
                                 (cbxi_ptp_eth_mac_addr[3]);
        slic_rule.dmac_hi_mask = 0xFFFFFFFF;
        slic_rule.dmac_lo_16   = (cbxi_ptp_eth_mac_addr[4] << 8) |
                                 (cbxi_ptp_eth_mac_addr[5]);
        slic_rule.dmac_lo_mask = 0xFFFF;
        slic_rule.mask_flags |= ROBO2_SLIC_MASK_DMAC;
        /* etype = 0x887F */
        slic_rule.etype = ETYPE_PTP;
        slic_rule.pg_map = 0;
        slic_rule.mask_flags |= (ROBO2_SLIC_MASK_ETYPE | ROBO2_SLIC_MASK_PGMAP);
        slicid = SLICID_PTP_ETH_PP0 + (rule_idx - SLIC_PTP_ETH_PORT_0);
        break;
    case SLIC_PTP_ETH_P2P_PORT_0:
    case SLIC_PTP_ETH_P2P_PORT_1:
    case SLIC_PTP_ETH_P2P_PORT_2:
    case SLIC_PTP_ETH_P2P_PORT_3:
    case SLIC_PTP_ETH_P2P_PORT_4:
    case SLIC_PTP_ETH_P2P_PORT_5:
    case SLIC_PTP_ETH_P2P_PORT_6:
    case SLIC_PTP_ETH_P2P_PORT_7:
    case SLIC_PTP_ETH_P2P_PORT_8:
    case SLIC_PTP_ETH_P2P_PORT_9:
    case SLIC_PTP_ETH_P2P_PORT_10:
    case SLIC_PTP_ETH_P2P_PORT_11:
    case SLIC_PTP_ETH_P2P_PORT_12:
    case SLIC_PTP_ETH_P2P_PORT_13:
    case SLIC_PTP_ETH_P2P_PORT_14:
    case SLIC_PTP_ETH_P2P_PORT_15:
        slic_rule.dmac_hi_32   = (cbxi_ptp_eth_p2p_mac_addr[0] << 24) |
                                 (cbxi_ptp_eth_p2p_mac_addr[1] << 16) |
                                 (cbxi_ptp_eth_p2p_mac_addr[2] << 8) |
                                 (cbxi_ptp_eth_p2p_mac_addr[3]);
        slic_rule.dmac_hi_mask = 0xFFFFFFFF;
        slic_rule.dmac_lo_16   = (cbxi_ptp_eth_p2p_mac_addr[4] << 8) |
                                 (cbxi_ptp_eth_p2p_mac_addr[5]);
        slic_rule.dmac_lo_mask = 0xFFFF;
        slic_rule.mask_flags |= ROBO2_SLIC_MASK_DMAC;
        /* etype = 0x887F */
        slic_rule.etype = ETYPE_PTP;
        slic_rule.pg_map = 0;
        slic_rule.mask_flags |= (ROBO2_SLIC_MASK_ETYPE | ROBO2_SLIC_MASK_PGMAP);
        slicid = SLICID_PTP;
        break;
    case SLIC_PTP_UDP_PORT_0:
    case SLIC_PTP_UDP_PORT_1:
    case SLIC_PTP_UDP_PORT_2:
    case SLIC_PTP_UDP_PORT_3:
    case SLIC_PTP_UDP_PORT_4:
    case SLIC_PTP_UDP_PORT_5:
    case SLIC_PTP_UDP_PORT_6:
    case SLIC_PTP_UDP_PORT_7:
    case SLIC_PTP_UDP_PORT_8:
    case SLIC_PTP_UDP_PORT_9:
    case SLIC_PTP_UDP_PORT_10:
    case SLIC_PTP_UDP_PORT_11:
    case SLIC_PTP_UDP_PORT_12:
    case SLIC_PTP_UDP_PORT_13:
    case SLIC_PTP_UDP_PORT_14:
    case SLIC_PTP_UDP_PORT_15:
        slic_rule.dmac_hi_32   = (cbxi_ptp_udp_mac_addr[0] << 24) |
                                 (cbxi_ptp_udp_mac_addr[1] << 16) |
                                 (cbxi_ptp_udp_mac_addr[2] << 8) |
                                 (cbxi_ptp_udp_mac_addr[3]);
        slic_rule.dmac_hi_mask = 0xFFFFFFFF;
        slic_rule.dmac_lo_16   = (cbxi_ptp_udp_mac_addr[4] << 8) |
                                 (cbxi_ptp_udp_mac_addr[5]);
        slic_rule.dmac_lo_mask = 0xFFFF;
        slic_rule.mask_flags |= ROBO2_SLIC_MASK_DMAC;
        /* etype = 0x0800 */
        slic_rule.etype = ETYPE_IPV4;
        slic_rule.pg_map = 0;
        slic_rule.mask_flags |= (ROBO2_SLIC_MASK_ETYPE | ROBO2_SLIC_MASK_PGMAP);
        slicid = SLICID_PTP_UDP_PP0 + (rule_idx - SLIC_PTP_UDP_PORT_0);
        break;
    case SLIC_PTP_UDP_P2P_PORT_0:
    case SLIC_PTP_UDP_P2P_PORT_1:
    case SLIC_PTP_UDP_P2P_PORT_2:
    case SLIC_PTP_UDP_P2P_PORT_3:
    case SLIC_PTP_UDP_P2P_PORT_4:
    case SLIC_PTP_UDP_P2P_PORT_5:
    case SLIC_PTP_UDP_P2P_PORT_6:
    case SLIC_PTP_UDP_P2P_PORT_7:
    case SLIC_PTP_UDP_P2P_PORT_8:
    case SLIC_PTP_UDP_P2P_PORT_9:
    case SLIC_PTP_UDP_P2P_PORT_10:
    case SLIC_PTP_UDP_P2P_PORT_11:
    case SLIC_PTP_UDP_P2P_PORT_12:
    case SLIC_PTP_UDP_P2P_PORT_13:
    case SLIC_PTP_UDP_P2P_PORT_14:
    case SLIC_PTP_UDP_P2P_PORT_15:
        slic_rule.dmac_hi_32   = (cbxi_ptp_udp_p2p_mac_addr[0] << 24) |
                                 (cbxi_ptp_udp_p2p_mac_addr[1] << 16) |
                                 (cbxi_ptp_udp_p2p_mac_addr[2] << 8) |
                                 (cbxi_ptp_udp_p2p_mac_addr[3]);
        slic_rule.dmac_hi_mask = 0xFFFFFFFF;
        slic_rule.dmac_lo_16   = (cbxi_ptp_udp_p2p_mac_addr[4] << 8) |
                                 (cbxi_ptp_udp_p2p_mac_addr[5]);
        slic_rule.dmac_lo_mask = 0xFFFF;
        slic_rule.mask_flags |= ROBO2_SLIC_MASK_DMAC;
        /* etype = 0x0800 */
        slic_rule.etype = ETYPE_IPV4;
        slic_rule.pg_map = 0;
        slic_rule.mask_flags |= (ROBO2_SLIC_MASK_ETYPE | ROBO2_SLIC_MASK_PGMAP);
        slicid = SLICID_PTP;
        break;
#endif
    case SLIC_ARP_BCAST:
        /* Trap broadcast ARP to CPU */
        /* DMAC = FF:FF:FF:FF:FF:FF ; etype = ARP */
        slic_rule.etype = ETYPE_ARP;
        slic_rule.dmac_hi_32   = 0xFFFFFFFF;
        slic_rule.dmac_hi_mask = 0xFFFFFFFF;
        slic_rule.dmac_lo_16   = 0xFFFF;
        slic_rule.dmac_lo_mask = 0xFFFF;
        slic_rule.mask_flags |= (ROBO2_SLIC_MASK_ETYPE | ROBO2_SLIC_MASK_DMAC);
        slicid = SLICID_LWIP;
        slic_trap = SLIC_TRAP_GROUP1; /* trap */
        break;
    case SLIC_ARP_UCAST:
        /* Drop & trap unicast ARP to CPU */
        /* given DMAC, etype = ARP */
        slic_rule.etype = ETYPE_ARP;
        slic_rule.dmac_hi_32   = (mac_addr[0]<<24) | (mac_addr[1]<<16) | (mac_addr[2]<<8) | (mac_addr[3]);
        slic_rule.dmac_hi_mask = 0xFFFFFFFF;
        slic_rule.dmac_lo_16   = (mac_addr[4]<<8) | (mac_addr[5]);
        slic_rule.dmac_lo_mask = 0xFFFF;
        slic_rule.mask_flags |= (ROBO2_SLIC_MASK_ETYPE | ROBO2_SLIC_MASK_DMAC);
        slicid = SLICID_LWIP;
        slic_trap = SLIC_TRAP_GROUP2; /* Drop & trap */
        break;
    case SLIC_SYS_MAC_IPV4:
        /* given DMAC; etype = IPv4 */
        slic_rule.etype = ETYPE_IPV4;
        slic_rule.dmac_hi_32  = (mac_addr[0]<<24) | (mac_addr[1]<<16) | (mac_addr[2]<<8) | (mac_addr[3]);
        slic_rule.dmac_hi_mask = 0xFFFFFFFF;
        slic_rule.dmac_lo_16   = (mac_addr[4]<<8) | (mac_addr[5]);
        slic_rule.dmac_lo_mask = 0xFFFF;
        slic_rule.mask_flags |= (ROBO2_SLIC_MASK_ETYPE | ROBO2_SLIC_MASK_DMAC);
        slicid = SLICID_LWIP;
        slic_trap = SLIC_TRAP_GROUP2; /* Drop & trap */
        break;
    case SLIC_SYS_MAC_IPV6:
        /* given DMAC; etype = IPv6 */
        slic_rule.etype = ETYPE_IPV6;
        slic_rule.dmac_hi_32   = (mac_addr[0]<<24) | (mac_addr[1]<<16) | (mac_addr[2]<<8) | (mac_addr[3]);
        slic_rule.dmac_hi_mask = 0xFFFFFFFF;
        slic_rule.dmac_lo_16   = (mac_addr[4]<<8) | (mac_addr[5]);
        slic_rule.dmac_lo_mask = 0xFFFF;
        slic_rule.mask_flags |= (ROBO2_SLIC_MASK_ETYPE | ROBO2_SLIC_MASK_DMAC);
        slicid = SLICID_LWIP;
        slic_trap = SLIC_TRAP_GROUP2; /* Drop & trap */
        break;
    case SLIC_SYS_MAC_DROP:
        /* given DMAC */
        slic_rule.dmac_hi_32   = (mac_addr[0]<<24) | (mac_addr[1]<<16) | (mac_addr[2]<<8) | (mac_addr[3]);
        slic_rule.dmac_hi_mask = 0xFFFFFFFF;
        slic_rule.dmac_lo_16   = (mac_addr[4]<<8) | (mac_addr[5]);
        slic_rule.dmac_lo_mask = 0xFFFF;
        slic_rule.mask_flags |= (ROBO2_SLIC_MASK_ETYPE | ROBO2_SLIC_MASK_DMAC);
        slicid = SLICID_DEFAULT;
        slic_trap = SLIC_TRAP_GROUP3; /* Drop */
        break;
    case SLIC_LLDP:
        /* DMAC 01-80-c2-00-00-0x */
        slic_rule.dmac_hi_32   = 0x0180C200;
        slic_rule.dmac_hi_mask = 0xFFFFFFFF;
        slic_rule.dmac_lo_16   = 0x0000;
        slic_rule.dmac_lo_mask = 0xFFF0;
        slic_rule.mask_flags |= ROBO2_SLIC_MASK_DMAC;
        /* ether type = 0x88CC */
        slic_rule.etype = ETYPE_USER_ETYPE10;
        slic_rule.mask_flags |= ROBO2_SLIC_MASK_ETYPE;
        slicid = SLICID_LLDP;
        slic_trap = SLIC_TRAP_GROUP0; /* Disabled by default - set by cbxi_slicid_map() */
        slic_rule.pg_map = 0x3FFF;
        break;
    case SLIC_EAPOL_ETYPE:
        /*User defined etype = 9 (0x888E) */
        slic_rule.etype = ETYPE_USER_ETYPE9;
        slic_rule.mask_flags |= (ROBO2_SLIC_MASK_ETYPE | ROBO2_SLIC_MASK_PGMAP);
        slicid = SLICID_EAPOL;
        slic_rule.pg_map = 0;
        slic_trap = SLIC_TRAP_GROUP2; /* Drop & trap */
        break;
    case SLIC_EAPOL_IEEE:
        /*DMAC = 01:80:C2:00:00:03    ;   ctype = 0x888E */
        slic_rule.etype        = ETYPE_USER_ETYPE9;
        slic_rule.dmac_hi_32   = 0x0180C200;
        slic_rule.dmac_hi_mask = 0xFFFFFFFF;
        slic_rule.dmac_lo_16   = 0x0003;
        slic_rule.dmac_lo_mask = 0xFFFF;
        slic_rule.pg_map = 0;
        slic_rule.mask_flags |= (ROBO2_SLIC_MASK_ETYPE |
                                 ROBO2_SLIC_MASK_DMAC  |
                                 ROBO2_SLIC_MASK_PGMAP);
        slicid = SLICID_EAPOL;
        slic_trap = SLIC_TRAP_GROUP2; /* Drop & trap */
        break;
    case SLIC_EAPOL_UCAST:
        /* etype = 0x888E ;  DMAC = User defined Unicast MAC */
        slic_rule.etype = ETYPE_USER_ETYPE9;
        /* DMAC to be set by user by calling cbxi_slicmap_rule_set */
        slicid = SLICID_EAPOL;
        slic_rule.dmac_hi_32   = 0x0;
        slic_rule.dmac_hi_mask = 0xFFFFFFFF;
        slic_rule.dmac_lo_16   = 0x0;
        slic_rule.dmac_lo_mask = 0xFFFF;
        slic_rule.pg_map = 0;
        slic_rule.mask_flags |= (ROBO2_SLIC_MASK_ETYPE |
                                 ROBO2_SLIC_MASK_DMAC  |
                                 ROBO2_SLIC_MASK_PGMAP);
        slic_trap = SLIC_TRAP_GROUP2; /* Drop & trap */
        break;
    case SLIC_EAPOL_MCAST:
        /* etype = 0x888E ;  DMAC = User defined Mcast MAC */
        slic_rule.etype = ETYPE_USER_ETYPE9;
        /* DMAC to be set by user by calling cbxi_slicmap_rule_set */
        slicid = SLICID_EAPOL;
        slic_rule.dmac_hi_32   = 0x0;
        slic_rule.dmac_hi_mask = 0xFFFFFFFF;
        slic_rule.dmac_lo_16   = 0x0;
        slic_rule.dmac_lo_mask = 0xFFFF;
        slic_rule.pg_map = 0;
        slic_rule.mask_flags |= (ROBO2_SLIC_MASK_ETYPE |
                                 ROBO2_SLIC_MASK_DMAC  |
                                 ROBO2_SLIC_MASK_PGMAP);
        slic_trap = SLIC_TRAP_GROUP2; /* Drop & trap */
        break;
    case SLIC_RSVD_MAC_1:
        /* DMAC = 01:80:C2:00:00:10 - 1F */
        slic_rule.dmac_hi_32   = 0x0180C200;
        slic_rule.dmac_hi_mask = 0xFFFFFFFF;
        slic_rule.dmac_lo_16   = 0x0010;
        slic_rule.dmac_lo_mask = 0xFFF0;
        slic_rule.mask_flags |= ROBO2_SLIC_MASK_DMAC;
        slicid = SLICID_LLDP; /* Flood to all ports */
        slic_trap = SLIC_TRAP_GROUP1; /* TRAP2CP */
        break;
    case SLIC_RSVD_MAC_2:
        /* DMAC = 01:80:C2:00:00:20 - 21 */
        slic_rule.dmac_hi_32   = 0x0180C200;
        slic_rule.dmac_hi_mask = 0xFFFFFFFF;
        slic_rule.dmac_lo_16   = 0x0020;
        slic_rule.dmac_lo_mask = 0xFFFE;
        slic_rule.mask_flags |= ROBO2_SLIC_MASK_DMAC;
        slicid = SLICID_LLDP; /* Flood to all ports */
        slic_trap = SLIC_TRAP_GROUP1; /* TRAP2CP */
        break;
    case SLIC_L2CP:
        /* DMAC = 01:80:C2:00:00:XX */
        slic_rule.dmac_hi_32   = 0x0180C200;
        slic_rule.dmac_hi_mask = 0xFFFFFFFF;
        slic_rule.dmac_lo_16   = 0x0000;
        slic_rule.dmac_lo_mask = 0xFF00;
        slic_rule.mask_flags |= ROBO2_SLIC_MASK_DMAC;
        slicid = SLICID_DEFAULT;
        slic_trap = SLIC_TRAP_GROUP2; /* Drop & TRAP2CP */
        break;
    case SLIC_EAPOL_DROP:
        slic_rule.mask_flags |= ROBO2_SLIC_MASK_PGMAP;
        slic_rule.pg_map = 0;
        slicid = SLICID_USE_DP;
        break;
    /* Port extender rules */
    case SLIC_PE_1BR_DROP:
        /* TPID0 = ETAG */
        slic_rule.mask_flags |= (ROBO2_SLIC_MASK_PGMAP |
                                                ROBO2_SLIC_MASK_TPID0);
        slic_rule.pg_map = 0;
        slic_rule.tpid0  = TPID_ETAG;
        slicid = SLICID_USE_DP;
        slic_trap = SLIC_TRAP_GROUP3; /* Drop */
        break;
    case SLIC_PE_1BR_FWD_UPSTR:

        /* TPID0 = ETAG */
        slic_rule.mask_flags |= (ROBO2_SLIC_MASK_PGMAP |
                                                ROBO2_SLIC_MASK_TPID0);
        slic_rule.pg_map = 0;
        slic_rule.tpid0  = TPID_ETAG;
        slicid = SLICID_PE_FWD_UPSTR;
        break;
     /* Specific rules on Avenger cascade ports */
     case SLIC_PE_CS_1BR_UC_ETAG:
        slic_rule.tci_valid  = (1 << ROBO2_SLIC_TCI3_SHIFT);
        slic_rule.tci_valid_mask = (1 << ROBO2_SLIC_TCI3_SHIFT);
        /* Check for only UCAST */
        slic_rule.tci3      = 0x0000;
        slic_rule.tci3_mask = 0x3000;
        slic_rule.tpid0 = TPID_ETAG;
        slic_rule.mask_flags |= (ROBO2_SLIC_MASK_TPID0 | ROBO2_SLIC_MASK_TCI);

        /* TPID0 = ETAG */
        slic_rule.mask_flags |= (ROBO2_SLIC_MASK_PGMAP |
                        ROBO2_SLIC_MASK_TCI | ROBO2_SLIC_MASK_TPID0);
        slic_rule.pg_map = 0;

        slicid = SLICID_PE_CS_ETAG_DECAP; /* Avg Cascade port removed */
        break;
    case SLIC_PE_CS_1BR_IN_ECID_U0_P0:
    case SLIC_PE_CS_1BR_IN_ECID_U0_P1:
    case SLIC_PE_CS_1BR_IN_ECID_U0_P2:
    case SLIC_PE_CS_1BR_IN_ECID_U0_P3:
    case SLIC_PE_CS_1BR_IN_ECID_U0_P4:
    case SLIC_PE_CS_1BR_IN_ECID_U0_P5:
    case SLIC_PE_CS_1BR_IN_ECID_U0_P6:
    case SLIC_PE_CS_1BR_IN_ECID_U0_P7:
    case SLIC_PE_CS_1BR_IN_ECID_U0_P8:
    case SLIC_PE_CS_1BR_IN_ECID_U0_P9:
    case SLIC_PE_CS_1BR_IN_ECID_U0_P10:
    case SLIC_PE_CS_1BR_IN_ECID_U0_P11:
    case SLIC_PE_CS_1BR_IN_ECID_U0_P12:
    case SLIC_PE_CS_1BR_IN_ECID_U0_P13:
    case SLIC_PE_CS_1BR_IN_ECID_U0_P14:
    case SLIC_PE_CS_1BR_IN_ECID_U0_P15:
    case SLIC_PE_CS_1BR_IN_ECID_U1_P0:
    case SLIC_PE_CS_1BR_IN_ECID_U1_P1:
    case SLIC_PE_CS_1BR_IN_ECID_U1_P2:
    case SLIC_PE_CS_1BR_IN_ECID_U1_P3:
    case SLIC_PE_CS_1BR_IN_ECID_U1_P4:
    case SLIC_PE_CS_1BR_IN_ECID_U1_P5:
    case SLIC_PE_CS_1BR_IN_ECID_U1_P6:
    case SLIC_PE_CS_1BR_IN_ECID_U1_P7:
    case SLIC_PE_CS_1BR_IN_ECID_U1_P8:
    case SLIC_PE_CS_1BR_IN_ECID_U1_P9:
    case SLIC_PE_CS_1BR_IN_ECID_U1_P10:
    case SLIC_PE_CS_1BR_IN_ECID_U1_P11:
    case SLIC_PE_CS_1BR_IN_ECID_U1_P12:
    case SLIC_PE_CS_1BR_IN_ECID_U1_P13:
    case SLIC_PE_CS_1BR_IN_ECID_U1_P14:
    case SLIC_PE_CS_1BR_IN_ECID_U1_P15:
        slic_rule.tci_valid  = (1 << ROBO2_SLIC_TCI2_SHIFT);
        slic_rule.tci_valid_mask = (1 << ROBO2_SLIC_TCI2_SHIFT);
        /* Actual TCI is set while configuring Extender port */
        slic_rule.tci2      = 0x0000;
        slic_rule.tci2_mask = 0x0FFF;
        slic_rule.tpid0 = TPID_ETAG;
        slic_rule.mask_flags |= (ROBO2_SLIC_MASK_TPID0 | ROBO2_SLIC_MASK_TCI);

        /* TPID0 = ETAG */
        slic_rule.mask_flags |= (ROBO2_SLIC_MASK_PGMAP |
                        ROBO2_SLIC_MASK_TCI | ROBO2_SLIC_MASK_TPID0);
        slic_rule.pg_map = 0;

        slicid = SLICID_PE_CS_IN_ECID_U0_P0 + (rule_idx - SLIC_PE_CS_1BR_IN_ECID_U0_P0);
        break;
    case SLIC_PE_CS_1BR_IN_ECID_LAG0 :
    case SLIC_PE_CS_1BR_IN_ECID_LAG1 :
    case SLIC_PE_CS_1BR_IN_ECID_LAG2 :
    case SLIC_PE_CS_1BR_IN_ECID_LAG3 :
    case SLIC_PE_CS_1BR_IN_ECID_LAG4 :
    case SLIC_PE_CS_1BR_IN_ECID_LAG5 :
    case SLIC_PE_CS_1BR_IN_ECID_LAG6 :
    case SLIC_PE_CS_1BR_IN_ECID_LAG7 :
    case SLIC_PE_CS_1BR_IN_ECID_LAG8 :
    case SLIC_PE_CS_1BR_IN_ECID_LAG9 :
    case SLIC_PE_CS_1BR_IN_ECID_LAG10:
    case SLIC_PE_CS_1BR_IN_ECID_LAG11:
    case SLIC_PE_CS_1BR_IN_ECID_LAG12:
    case SLIC_PE_CS_1BR_IN_ECID_LAG13:
    case SLIC_PE_CS_1BR_IN_ECID_LAG14:
    case SLIC_PE_CS_1BR_IN_ECID_LAG15:
    case SLIC_PE_CS_1BR_IN_ECID_LAG16:
    case SLIC_PE_CS_1BR_IN_ECID_LAG17:
    case SLIC_PE_CS_1BR_IN_ECID_LAG18:
    case SLIC_PE_CS_1BR_IN_ECID_LAG19:
    case SLIC_PE_CS_1BR_IN_ECID_LAG20:
    case SLIC_PE_CS_1BR_IN_ECID_LAG21:
    case SLIC_PE_CS_1BR_IN_ECID_LAG22:
    case SLIC_PE_CS_1BR_IN_ECID_LAG23:
    case SLIC_PE_CS_1BR_IN_ECID_LAG24:
    case SLIC_PE_CS_1BR_IN_ECID_LAG25:
    case SLIC_PE_CS_1BR_IN_ECID_LAG26:
    case SLIC_PE_CS_1BR_IN_ECID_LAG27:
    case SLIC_PE_CS_1BR_IN_ECID_LAG28:
    case SLIC_PE_CS_1BR_IN_ECID_LAG29:
    case SLIC_PE_CS_1BR_IN_ECID_LAG30:
    case SLIC_PE_CS_1BR_IN_ECID_LAG31:
        slic_rule.tci_valid  = (1 << ROBO2_SLIC_TCI2_SHIFT);
        slic_rule.tci_valid_mask = (1 << ROBO2_SLIC_TCI2_SHIFT);
        /* Actual TCI is set while configuring Extender port */
        slic_rule.tci2      = 0x0000;
        slic_rule.tci2_mask = 0x0FFF;
        slic_rule.tpid0 = TPID_ETAG;
        slic_rule.mask_flags |= (ROBO2_SLIC_MASK_TPID0 | ROBO2_SLIC_MASK_TCI);

        /* TPID0 = ETAG */
        slic_rule.mask_flags |= (ROBO2_SLIC_MASK_PGMAP |
                        ROBO2_SLIC_MASK_TCI | ROBO2_SLIC_MASK_TPID0);
        slic_rule.pg_map = 0;

        slicid = SLICID_PE_CS_IN_ECID_U0_P0 + (rule_idx - SLIC_PE_CS_1BR_IN_ECID_U0_P0);
/*        slic_trap = SLIC_TRAP_GROUP3;*/ /* Drop */
        break;

    case SLIC_PE_CS_1BR_DECAP:
        /* TPID0 = ETAG; HDT = decap ETAG */
        slic_rule.mask_flags = (ROBO2_SLIC_MASK_PGMAP |
                                                ROBO2_SLIC_MASK_TPID0);
        slic_rule.pg_map = 0;
        slic_rule.tpid0  = TPID_ETAG;
        slicid = SLICID_PE_CS_ETAG_DECAP;
        break;
    case SLIC_PE_CS_DROP:
        slic_rule.mask_flags |= ROBO2_SLIC_MASK_PGMAP;

        slic_rule.pg_map = 0x0;
        slic_trap = SLIC_TRAP_GROUP3; /* Drop */
        slicid = SLICID_DEFAULT;
        break;
     /* generic rules on Uplink port */
     case SLIC_PE_1BR_UC_ETAG:
        slic_rule.tci_valid  = (1 << ROBO2_SLIC_TCI3_SHIFT);
        slic_rule.tci_valid_mask = (1 << ROBO2_SLIC_TCI3_SHIFT);
        /* Check for only UCAST */
        slic_rule.tci3      = 0x0000;
        slic_rule.tci3_mask = 0x3000;
        slic_rule.tpid0 = TPID_ETAG;
        slic_rule.mask_flags |= (ROBO2_SLIC_MASK_TPID0 | ROBO2_SLIC_MASK_TCI);

        /* TPID0 = ETAG */
        slic_rule.mask_flags |= (ROBO2_SLIC_MASK_PGMAP |
                        ROBO2_SLIC_MASK_TCI | ROBO2_SLIC_MASK_TPID0);
        slic_rule.pg_map = 0;

        slicid = SLICID_PE_ETAG_DECAP_CFP; /* Avg Cascade port included */
#if 0 /* Use CFP to duplicate packets in case of LAG across Avengers */
        slic_trap = SLIC_TRAP_GROUP6; /* Trap to CSD without CB tag */
#endif
        break;
    case SLIC_PE_1BR_IN_ECID_U0_P0:
    case SLIC_PE_1BR_IN_ECID_U0_P1:
    case SLIC_PE_1BR_IN_ECID_U0_P2:
    case SLIC_PE_1BR_IN_ECID_U0_P3:
    case SLIC_PE_1BR_IN_ECID_U0_P4:
    case SLIC_PE_1BR_IN_ECID_U0_P5:
    case SLIC_PE_1BR_IN_ECID_U0_P6:
    case SLIC_PE_1BR_IN_ECID_U0_P7:
    case SLIC_PE_1BR_IN_ECID_U0_P8:
    case SLIC_PE_1BR_IN_ECID_U0_P9:
    case SLIC_PE_1BR_IN_ECID_U0_P10:
    case SLIC_PE_1BR_IN_ECID_U0_P11:
    case SLIC_PE_1BR_IN_ECID_U0_P12:
    case SLIC_PE_1BR_IN_ECID_U0_P13:
    case SLIC_PE_1BR_IN_ECID_U0_P14:
    case SLIC_PE_1BR_IN_ECID_U0_P15:
    case SLIC_PE_1BR_IN_ECID_U1_P0:
    case SLIC_PE_1BR_IN_ECID_U1_P1:
    case SLIC_PE_1BR_IN_ECID_U1_P2:
    case SLIC_PE_1BR_IN_ECID_U1_P3:
    case SLIC_PE_1BR_IN_ECID_U1_P4:
    case SLIC_PE_1BR_IN_ECID_U1_P5:
    case SLIC_PE_1BR_IN_ECID_U1_P6:
    case SLIC_PE_1BR_IN_ECID_U1_P7:
    case SLIC_PE_1BR_IN_ECID_U1_P8:
    case SLIC_PE_1BR_IN_ECID_U1_P9:
    case SLIC_PE_1BR_IN_ECID_U1_P10:
    case SLIC_PE_1BR_IN_ECID_U1_P11:
    case SLIC_PE_1BR_IN_ECID_U1_P12:
    case SLIC_PE_1BR_IN_ECID_U1_P13:
    case SLIC_PE_1BR_IN_ECID_U1_P14:
    case SLIC_PE_1BR_IN_ECID_U1_P15:
        slic_rule.tci_valid  = (1 << ROBO2_SLIC_TCI2_SHIFT);
        slic_rule.tci_valid_mask = (1 << ROBO2_SLIC_TCI2_SHIFT);
        /* Actual TCI is set while configuring Extender port */
        slic_rule.tci2      = 0x0000;
        slic_rule.tci2_mask = 0x0FFF;
        slic_rule.tpid0 = TPID_ETAG;
        slic_rule.mask_flags |= (ROBO2_SLIC_MASK_TPID0 | ROBO2_SLIC_MASK_TCI);

        /* TPID0 = ETAG */
        slic_rule.mask_flags |= (ROBO2_SLIC_MASK_PGMAP |
                        ROBO2_SLIC_MASK_TCI | ROBO2_SLIC_MASK_TPID0);
        slic_rule.pg_map = 0;

        slicid = SLICID_PE_IN_ECID_U0_P0 + (rule_idx - SLIC_PE_1BR_IN_ECID_U0_P0);
        break;
    case SLIC_PE_1BR_IN_ECID_LAG0 :
    case SLIC_PE_1BR_IN_ECID_LAG1 :
    case SLIC_PE_1BR_IN_ECID_LAG2 :
    case SLIC_PE_1BR_IN_ECID_LAG3 :
    case SLIC_PE_1BR_IN_ECID_LAG4 :
    case SLIC_PE_1BR_IN_ECID_LAG5 :
    case SLIC_PE_1BR_IN_ECID_LAG6 :
    case SLIC_PE_1BR_IN_ECID_LAG7 :
    case SLIC_PE_1BR_IN_ECID_LAG8 :
    case SLIC_PE_1BR_IN_ECID_LAG9 :
    case SLIC_PE_1BR_IN_ECID_LAG10:
    case SLIC_PE_1BR_IN_ECID_LAG11:
    case SLIC_PE_1BR_IN_ECID_LAG12:
    case SLIC_PE_1BR_IN_ECID_LAG13:
    case SLIC_PE_1BR_IN_ECID_LAG14:
    case SLIC_PE_1BR_IN_ECID_LAG15:
    case SLIC_PE_1BR_IN_ECID_LAG16:
    case SLIC_PE_1BR_IN_ECID_LAG17:
    case SLIC_PE_1BR_IN_ECID_LAG18:
    case SLIC_PE_1BR_IN_ECID_LAG19:
    case SLIC_PE_1BR_IN_ECID_LAG20:
    case SLIC_PE_1BR_IN_ECID_LAG21:
    case SLIC_PE_1BR_IN_ECID_LAG22:
    case SLIC_PE_1BR_IN_ECID_LAG23:
    case SLIC_PE_1BR_IN_ECID_LAG24:
    case SLIC_PE_1BR_IN_ECID_LAG25:
    case SLIC_PE_1BR_IN_ECID_LAG26:
    case SLIC_PE_1BR_IN_ECID_LAG27:
    case SLIC_PE_1BR_IN_ECID_LAG28:
    case SLIC_PE_1BR_IN_ECID_LAG29:
    case SLIC_PE_1BR_IN_ECID_LAG30:
    case SLIC_PE_1BR_IN_ECID_LAG31:
        slic_rule.tci_valid  = (1 << ROBO2_SLIC_TCI2_SHIFT);
        slic_rule.tci_valid_mask = (1 << ROBO2_SLIC_TCI2_SHIFT);
        /* Actual TCI is set while configuring Extender port */
        slic_rule.tci2      = 0x0000;
        slic_rule.tci2_mask = 0x0FFF;
        slic_rule.tpid0 = TPID_ETAG;
        slic_rule.mask_flags |= (ROBO2_SLIC_MASK_TPID0 | ROBO2_SLIC_MASK_TCI);

        /* TPID0 = ETAG */
        slic_rule.mask_flags |= (ROBO2_SLIC_MASK_PGMAP |
                        ROBO2_SLIC_MASK_TCI | ROBO2_SLIC_MASK_TPID0);
        slic_rule.pg_map = 0;

        slicid = SLICID_PE_IN_ECID_U0_P0 + (rule_idx - SLIC_PE_1BR_IN_ECID_U0_P0);
        break;
    case SLIC_PE_1BR_DECAP:
        /* TPID0 = ETAG; HDT = decap ETAG */
        slic_rule.mask_flags = (ROBO2_SLIC_MASK_PGMAP |
                                                ROBO2_SLIC_MASK_TPID0);
        slic_rule.pg_map = 0;
        slic_rule.tpid0  = TPID_ETAG;
        slicid = SLICID_PE_ETAG_DECAP;
        break;
    case SLIC_PE_1BR_FWD_DWNSTR_UC:
        slic_rule.tci_valid  = (1 << ROBO2_SLIC_TCI3_SHIFT);
        slic_rule.tci_valid_mask = (1 << ROBO2_SLIC_TCI3_SHIFT);
        /* Check only if ECID{13:12] = 00 */
        slic_rule.tci3      = 0x0000;
        slic_rule.tci3_mask = 0x3000;
        slic_rule.tpid0 = TPID_ETAG;
        slic_rule.mask_flags |= (ROBO2_SLIC_MASK_PGMAP |
                                ROBO2_SLIC_MASK_TPID0 | ROBO2_SLIC_MASK_TCI);

        slic_rule.pg_map = 0;
        slicid = SLICID_PE_ETAG_DECAP;
        break;
    case SLIC_PE_1BR_FWD_DWNSTR_MC:
        /* TPID0 = ETAG; HDT = decap ETAG */
        slic_rule.mask_flags |= (ROBO2_SLIC_MASK_PGMAP |
                                                ROBO2_SLIC_MASK_TPID0);
        slic_rule.pg_map = 0;
        slic_rule.tpid0  = TPID_ETAG;
        slicid = SLICID_PE_FWD_DWNSTR_MC;
        break;
    case SLIC_PE_UNTAG_DROP:
        slic_rule.mask_flags |= ROBO2_SLIC_MASK_PGMAP;
        slic_rule.pg_map = 0;
        slicid = SLICID_USE_DP;
        slic_trap = SLIC_TRAP_GROUP3; /* Drop */
        break;
    case SLIC_PE_UNTAG_FWD:
        slic_rule.mask_flags |= ROBO2_SLIC_MASK_PGMAP;
        slic_rule.pg_map = 0;
        slicid = SLICID_PE_FWD_UPSTR;
        break;
    /* Bridging Rules */
    case SLIC_PORT_BASED_VLAN:
        /* pg_map to be populated by individual modules */
        slic_rule.pepper &= ~CBX_PORT_PEPPER_EN_VIRTUAL_PORT;
        slic_rule.pepper &= ~CBX_PORT_PEPPER_TRUST_PORT_TCDP;
        slic_rule.pepper_mask = (CBX_PORT_PEPPER_EN_VIRTUAL_PORT |
                                        CBX_PORT_PEPPER_TRUST_PORT_TCDP);
        slic_rule.mask_flags |= (ROBO2_SLIC_MASK_PEPPER |
                                        ROBO2_SLIC_MASK_PGMAP);
        slic_rule.pg_map = 0;
        slicid = SLICID_PV_TYPE;
        break;
    case SLIC_CUSTOMER_PORT_VLAN:
        port_type = cbxPortTypeCustomer;
        slic_rule.pepper |= port_type << CBX_PORT_PEPPER_PORT_TYPE_SHIFT;
        slic_rule.pepper &= ~CBX_PORT_PEPPER_EN_VIRTUAL_PORT;
        slic_rule.pepper &= ~CBX_PORT_PEPPER_TRUST_PORT_TCDP;
        slic_rule.pepper_mask = (CBX_PORT_PEPPER_PORT_TYPE_MASK |
            CBX_PORT_PEPPER_EN_VIRTUAL_PORT | CBX_PORT_PEPPER_TRUST_PORT_TCDP);
        slic_rule.mask_flags |= (ROBO2_SLIC_MASK_PEPPER |
                                        ROBO2_SLIC_MASK_PGMAP);
        slic_rule.pg_map = 0;
        slicid = SLICID_PV_TYPE_CTAG;
        break;
    case SLIC_PROVIDER_PORT_VLAN:
        port_type = cbxPortTypeProvider;
        slic_rule.pepper |= port_type << CBX_PORT_PEPPER_PORT_TYPE_SHIFT;
        slic_rule.pepper &= ~CBX_PORT_PEPPER_EN_VIRTUAL_PORT;
        slic_rule.pepper &= ~CBX_PORT_PEPPER_TRUST_PORT_TCDP;
        slic_rule.pepper_mask = (CBX_PORT_PEPPER_PORT_TYPE_MASK |
            CBX_PORT_PEPPER_EN_VIRTUAL_PORT | CBX_PORT_PEPPER_TRUST_PORT_TCDP);
        slic_rule.mask_flags |= (ROBO2_SLIC_MASK_PEPPER |
                                        ROBO2_SLIC_MASK_PGMAP);
        slic_rule.pg_map = 0;
        slicid = SLICID_PV_TYPE_STAG;
        break;
    case SLIC_CUSTOMER_PORT_LIN:
        port_type = cbxPortTypeCustomer;
        slic_rule.pepper |= port_type << CBX_PORT_PEPPER_PORT_TYPE_SHIFT;
        slic_rule.pepper |= CBX_PORT_PEPPER_EN_VIRTUAL_PORT;
        slic_rule.pepper &= ~CBX_PORT_PEPPER_TRUST_PORT_TCDP;
        slic_rule.pepper_mask = (CBX_PORT_PEPPER_PORT_TYPE_MASK |
            CBX_PORT_PEPPER_EN_VIRTUAL_PORT | CBX_PORT_PEPPER_TRUST_PORT_TCDP);
        slic_rule.mask_flags |= (ROBO2_SLIC_MASK_PEPPER |
                                        ROBO2_SLIC_MASK_PGMAP);
        slic_rule.pg_map = 0;
        slicid = SLICID_LIN_TYPE_CTAG;
        break;
    case SLIC_PROVIDER_PORT_LIN:
        port_type = cbxPortTypeProvider;
        slic_rule.pepper |= port_type << CBX_PORT_PEPPER_PORT_TYPE_SHIFT;
        slic_rule.pepper |= CBX_PORT_PEPPER_EN_VIRTUAL_PORT;
        slic_rule.pepper &= ~CBX_PORT_PEPPER_TRUST_PORT_TCDP;
        slic_rule.pepper_mask = (CBX_PORT_PEPPER_PORT_TYPE_MASK |
            CBX_PORT_PEPPER_EN_VIRTUAL_PORT | CBX_PORT_PEPPER_TRUST_PORT_TCDP);
        slic_rule.mask_flags |= (ROBO2_SLIC_MASK_PEPPER |
                                        ROBO2_SLIC_MASK_PGMAP);
        slic_rule.pg_map = 0;
        slicid = SLICID_LIN_TYPE_STAG;
        break;
    /* Bridging rules with trusted ports */
    case SLIC_TRUST_PORT_BASED_VLAN:
        slic_rule.pepper &= ~CBX_PORT_PEPPER_EN_VIRTUAL_PORT;
        slic_rule.pepper |= CBX_PORT_PEPPER_TRUST_PORT_TCDP;
        slic_rule.pepper_mask = (CBX_PORT_PEPPER_EN_VIRTUAL_PORT |
                                        CBX_PORT_PEPPER_TRUST_PORT_TCDP);
        slic_rule.mask_flags |= (ROBO2_SLIC_MASK_PEPPER |
                                        ROBO2_SLIC_MASK_PGMAP);
        slic_rule.pg_map = 0;
        slicid = SLICID_DEFAULT;
        break;
    case SLIC_TRUST_CUSTOMER_PORT_VLAN:
        port_type = cbxPortTypeCustomer;
        slic_rule.pepper |= port_type << CBX_PORT_PEPPER_PORT_TYPE_SHIFT;
        slic_rule.pepper &= ~CBX_PORT_PEPPER_EN_VIRTUAL_PORT;
        slic_rule.pepper |= CBX_PORT_PEPPER_TRUST_PORT_TCDP;
        slic_rule.pepper_mask = (CBX_PORT_PEPPER_PORT_TYPE_MASK |
            CBX_PORT_PEPPER_EN_VIRTUAL_PORT | CBX_PORT_PEPPER_TRUST_PORT_TCDP);
        slic_rule.mask_flags |= (ROBO2_SLIC_MASK_PEPPER |
                                        ROBO2_SLIC_MASK_PGMAP);
        slic_rule.pg_map = 0;
        slicid = SLICID_PV_TYPE_CTAG;
        break;
    case SLIC_TRUST_PROVIDER_PORT_VLAN:
        port_type = cbxPortTypeProvider;
        slic_rule.pepper |= port_type << CBX_PORT_PEPPER_PORT_TYPE_SHIFT;
        slic_rule.pepper &= ~CBX_PORT_PEPPER_EN_VIRTUAL_PORT;
        slic_rule.pepper |= CBX_PORT_PEPPER_TRUST_PORT_TCDP;
        slic_rule.pepper_mask = (CBX_PORT_PEPPER_PORT_TYPE_MASK |
            CBX_PORT_PEPPER_EN_VIRTUAL_PORT | CBX_PORT_PEPPER_TRUST_PORT_TCDP);
        slic_rule.mask_flags |= (ROBO2_SLIC_MASK_PEPPER |
                                        ROBO2_SLIC_MASK_PGMAP);
        slic_rule.pg_map = 0;
        slicid = SLICID_PV_TYPE_STAG;
        break;
    case SLIC_TRUST_CUSTOMER_PORT_LIN:
        port_type = cbxPortTypeCustomer;
        slic_rule.pepper |= port_type << CBX_PORT_PEPPER_PORT_TYPE_SHIFT;
        slic_rule.pepper |= CBX_PORT_PEPPER_EN_VIRTUAL_PORT;
        slic_rule.pepper |= CBX_PORT_PEPPER_TRUST_PORT_TCDP;
        slic_rule.pepper_mask = (CBX_PORT_PEPPER_PORT_TYPE_MASK |
            CBX_PORT_PEPPER_EN_VIRTUAL_PORT | CBX_PORT_PEPPER_TRUST_PORT_TCDP);
        slic_rule.mask_flags |= (ROBO2_SLIC_MASK_PEPPER |
                                        ROBO2_SLIC_MASK_PGMAP);
        slic_rule.pg_map = 0;
        slicid = SLICID_LIN_TYPE_CTAG;
        break;
    case SLIC_TRUST_PROVIDER_PORT_LIN:
        port_type = cbxPortTypeProvider;
        slic_rule.pepper |= port_type << CBX_PORT_PEPPER_PORT_TYPE_SHIFT;
        slic_rule.pepper |= CBX_PORT_PEPPER_EN_VIRTUAL_PORT;
        slic_rule.pepper |= CBX_PORT_PEPPER_TRUST_PORT_TCDP;
        slic_rule.pepper_mask = (CBX_PORT_PEPPER_PORT_TYPE_MASK |
            CBX_PORT_PEPPER_EN_VIRTUAL_PORT | CBX_PORT_PEPPER_TRUST_PORT_TCDP);
        slic_rule.mask_flags |= (ROBO2_SLIC_MASK_PEPPER |
                                        ROBO2_SLIC_MASK_PGMAP);
        slic_rule.pg_map = 0;
        slicid = SLICID_LIN_TYPE_STAG;
        break;
    case SLIC_HOST_PORT:
        port_type = cbxPortTypeHost;
        slic_rule.pepper |= port_type << CBX_PORT_PEPPER_PORT_TYPE_SHIFT;
        slic_rule.pepper |= CBX_PORT_PEPPER_EN_VIRTUAL_PORT;
        slic_rule.pepper |= CBX_PORT_PEPPER_TRUST_PORT_TCDP;
        slic_rule.pepper_mask = (CBX_PORT_PEPPER_PORT_TYPE_MASK |
            CBX_PORT_PEPPER_EN_VIRTUAL_PORT | CBX_PORT_PEPPER_TRUST_PORT_TCDP);
        slic_rule.mask_flags |= (ROBO2_SLIC_MASK_PEPPER |
                                        ROBO2_SLIC_MASK_PGMAP);
#ifdef CONFIG_EXTERNAL_HOST
        slic_rule.pg_map = (1 << CBX_PORT_ECPU);
#else
        slic_rule.pg_map = (1 << CBX_PORT_ICPU);
#endif
        slicid = SLICID_PV_TYPE_CTAG;
        break;
    case SLIC_DEFAULT:
        break;
    default:
        set_rule = 0;
        break;
    };

    if (!set_rule) {
        return CBX_E_NONE;
    }

    CBX_IF_ERROR_RETURN(cbxi_slicmap_rule_set(unit, rule_idx, slic_rule));

    CBX_IF_ERROR_RETURN(cbxi_slicid_map(unit, rule_idx, slicid, slic_trap));

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_slictcam_init
 * Purpose:
 *  Initialize SLIC TCAM.
 *
 * @return return code
 * @retval 0 success.
 */

int cbxi_slictcam_init(void)
{
    slicmap_info_t *sli = &slicmap_sw_info;
    int unit = 0;
    uint8_t num_units = 1;
    int i = 0;

    LOG_INFO(BSL_LS_FSAL_COMMON,
                (BSL_META("cbxi_slictcam_init()..\n")));

    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = 2;
    }

    for (unit = 0; unit < num_units; unit++) {
        CBX_IF_ERROR_RETURN(cbxi_slic_table_init(unit));
    }

    sli->init = TRUE;

    for (unit = 0; unit < num_units; unit++) {
        CBX_IF_ERROR_RETURN(cbxi_soft_tag_init(unit));

        CBX_IF_ERROR_RETURN(cbxi_hdr_decap_init(unit));

        for(i = 1; i < SLICID_MAX_INDEX; i++) {
            CBX_IF_ERROR_RETURN(cbxi_slicid_init(unit, i));
        }

        for (i = 0; i < SLIC_MAX_INDEX; i++) {
            CBX_IF_ERROR_RETURN(cbxi_default_rule_init(unit, i));
        }
    }

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_slic_pgmap_set
 * Purpose:
 *  Set pg_map at given TCAM index
 *
 * @return return code
 * @retval 0 success.
 */

int cbxi_slic_pgmap_set(int unit,
                cbxi_slic_rule_id_t rule_number, pbmp_t pg_map)
{
    cbxi_slic_rule_t slic_rule;

    CBX_IF_ERROR_RETURN(cbxi_slicmap_rule_get(
                                unit, rule_number, &slic_rule));

    slic_rule.mask_flags |= ROBO2_SLIC_MASK_PGMAP;
    slic_rule.pg_map = pg_map;

    CBX_IF_ERROR_RETURN(cbxi_slicmap_rule_set(
                                unit, rule_number, slic_rule));

    return CBX_E_NONE;
}


/*
 * Function:
 *  cbxi_slic_pgmap_get
 * Purpose:
 *  Get the pg_map at given TCAM index
 *
 * @return return code
 * @retval 0 success.
 */

int cbxi_slic_pgmap_get(int unit,
                cbxi_slic_rule_id_t rule_number, pbmp_t *pg_map)
{
    cbxi_slic_rule_t slic_rule;

    CBX_IF_ERROR_RETURN(cbxi_slicmap_rule_get(
                                unit, rule_number, &slic_rule));
    if (slic_rule.mask_flags & ROBO2_SLIC_MASK_PGMAP) {
        *pg_map = slic_rule.pg_map;
    } else {
        *pg_map = 0;
    }

    return CBX_E_NONE;
}
