/*
 * DO NOT EDIT THIS FILE!
 * This file is auto-generated from the registers file.
 * Edits to this file will be lost when it is regenerated.
 *
 * $Id: $
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved.$
 *
 * File:	arl_scan.c
 * Purpose:	Robo2 ARL Scanner Access Routines
 */

#include <sal_types.h>
#include <sal_console.h>
#include <soc/types.h>
#include <soc/defs.h>
#include <soc/drv.h>
#include <soc/register.h>
#include <soc/error.h>
#include <soc/robo2/common/allenum.h>
#include <soc/robo2/common/regacc.h>
#include <soc/robo2/common/memregs.h>
#include <soc/robo2/common/arl_scan.h>
#include <soc/robo2/common/tables.h>

/*
 * Function:
 *    soc_robo2_arl_scan_entry_validate
 * Description:
 *    Validate the ARL scan entry contents
 * Parameters:
 *    IN entry : Pointer to object of soc_robo2_arl_scan_entry_t
 * Result:
 *    SOC_E_NONE  on success
 *    SOC_E_PARAM  on failure
 */
int
soc_robo2_arl_scan_entry_validate(soc_robo2_arl_scan_entry_t *entry)
{

    if (entry == NULL) {
        return SOC_E_PARAM;
    }
    if (entry->valid_key > 3) {
        LOG_ERROR(BSL_LS_SOC_CRAL,("soc_robo2_arl_scan_entry_validate:"
                "Failed validating valid_key, range(0-3)\n"));
        return SOC_E_PARAM;
    }
    if (entry->dst_key > 2047) {
        LOG_ERROR(BSL_LS_SOC_CRAL,("soc_robo2_arl_scan_entry_validate:"
                "Failed validating dst_key, range(0-2047)\n"));
        return SOC_E_PARAM;
    }
    if (entry->dst_type_key > 3) {
        LOG_ERROR(BSL_LS_SOC_CRAL,("soc_robo2_arl_scan_entry_validate:"
                "Failed validating dst_type_key, range(0-3)\n"));
        return SOC_E_PARAM;
    }
    if (entry->hit_key > 1) {
        LOG_ERROR(BSL_LS_SOC_CRAL,("soc_robo2_arl_scan_entry_validate:"
                "Failed validating hit_key, range(0-1)\n"));
        return SOC_E_PARAM;
    }
    if (entry->fid_key > 4095) {
        LOG_ERROR(BSL_LS_SOC_CRAL,("soc_robo2_arl_scan_entry_validate:"
                "Failed validating fid_key, range(0-4095)\n"));
        return SOC_E_PARAM;
    }
    if (entry->fwd_ctrl_key > 3) {
        LOG_ERROR(BSL_LS_SOC_CRAL,("soc_robo2_arl_scan_entry_validate:"
                "Failed validating fwd_ctrl_key, range(0-3)\n"));
        return SOC_E_PARAM;
    }
    if (entry->valid_mask > 3) {
        LOG_ERROR(BSL_LS_SOC_CRAL,("soc_robo2_arl_scan_entry_validate:"
                "Failed validating valid_mask, range(0-3)\n"));
        return SOC_E_PARAM;
    }
    if (entry->dst_mask > 2047) {
        LOG_ERROR(BSL_LS_SOC_CRAL,("soc_robo2_arl_scan_entry_validate:"
                "Failed validating dst_mask, range(0-2047)\n"));
        return SOC_E_PARAM;
    }
    if (entry->dst_type_mask > 3) {
        LOG_ERROR(BSL_LS_SOC_CRAL,("soc_robo2_arl_scan_entry_validate:"
                "Failed validating dst_type_mask, range(0-3)\n"));
        return SOC_E_PARAM;
    }
    if (entry->hit_mask > 1) {
        LOG_ERROR(BSL_LS_SOC_CRAL,("soc_robo2_arl_scan_entry_validate:"
                "Failed validating hit_mask, range(0-1)\n"));
        return SOC_E_PARAM;
    }
    if (entry->fid_mask > 4095) {
        LOG_ERROR(BSL_LS_SOC_CRAL,("soc_robo2_arl_scan_entry_validate:"
                "Failed validating fid_mask, range(0-4095)\n"));
        return SOC_E_PARAM;
    }
    if (entry->fwd_ctrl_mask > 3) {
        LOG_ERROR(BSL_LS_SOC_CRAL,("soc_robo2_arl_scan_entry_validate:"
                "Failed validating fwd_ctrl_mask, range(0-3)\n"));
        return SOC_E_PARAM;
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *    soc_robo2_arl_scanner_wait_for
 * Description:
 *    This routine will poll scanner for the given duration
 *    Will return SOC_E_NONE if scanner ready is set
 *    Returns SOC_E_TIMEOUT if scanner ready is not set in the duration.
 * Parameters:
 *    IN unit     : chip identification
 *    IN duration : timeout period in ms
 *   OUT result   : Scanner result bitmap
 * Result:
 *    SOC_E_NONE  on success
 *    SOC_E_XXXX  on failures
 */
int
soc_robo2_arl_scanner_wait_for(int unit, int32 duration, uint32 *result)
{
    uint32 regval = 0;
    uint32 usecs = 0;
    int rc = 0;

    if (result == NULL) {
        LOG_ERROR(BSL_LS_SOC_CRAL,("soc_robo2_arl_scanner_wait_for:"
            "Null value for result \n"));
        return SOC_E_PARAM;
    }

    /* Make sure scanner is ready is set */
    do {
        rc = REG_READ_CB_ITM_ARL_SCAN_STATUSr(unit, &regval);
        if (rc != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_CRAL,("soc_robo2_arl_scanner_wait_for:"
                    " Failed reading CB_ITM_arl_scan_status\n"));
            return rc;
        }
        if (regval & (SCANNER_STATUS_READY |
                        SCANNER_STATUS_SEARCH_VALID)) {
            *result = regval;
            return SOC_E_NONE;
        }
        if (duration <= 0) {break;}
        usecs = (duration > 1000) ? (1000) : duration;
        sal_usleep(usecs);
        duration -= usecs;
    } while(1);

    return SOC_E_TIMEOUT;
}

/*
 * Function:
 *    soc_robo2_arl_update_scan_data_reg
 * Description:
 *    Update data reg for scanner
 * Parameters:
 *    IN unit  : chip identification
 *    IN entry : Pointer to object of soc_robo2_arl_scan_entry_t
 * Result:
 *    SOC_E_NONE  on success
 *    SOC_E_XXXX  on failures
 */
int
soc_robo2_arl_update_scan_data_reg(int unit,  soc_robo2_arl_scan_entry_t *entry)
{
    uint32 regval = 0;
    uint32 tmpval = 0;
    int rc = 0;

    rc = soc_robo2_arl_scan_entry_validate(entry);
    if (rc != SOC_E_NONE) {
        return rc;
    }

    if (SOC_DRIVER(unit)->type == SOC_CHIP_BCM53158_B0) {
        tmpval = entry->valid_key;
        SOC_REG_FIELD_SET(unit, CB_ITM_ARLFM_SCAN_DATA_PART2r_ROBO2, &regval,
                        VALIDf, &tmpval);
        tmpval = entry->dst_key;
        SOC_REG_FIELD_SET(unit, CB_ITM_ARLFM_SCAN_DATA_PART2r_ROBO2, &regval,
                        DSTf, &tmpval);
        tmpval = entry->dst_type_key;
        SOC_REG_FIELD_SET(unit, CB_ITM_ARLFM_SCAN_DATA_PART2r_ROBO2, &regval,
                        DST_TYPEf, &tmpval);
        tmpval = entry->hit_key;
        SOC_REG_FIELD_SET(unit, CB_ITM_ARLFM_SCAN_DATA_PART2r_ROBO2, &regval,
                        HITf, &tmpval);
        /* Program the register */
        rc = REG_WRITE_CB_ITM_ARLFM_SCAN_DATA_PART2r(unit, &regval);
        if (rc != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_CRAL,("soc_robo2_arl_update_scan_data_reg:"
            "Failed writing CB_ITM_ARLFM_SCAN_DATA_PART2 rc(%d)\n", rc));
            return rc;
        }
        SOC_REG_FIELD_SET(unit, CB_ITM_ARLFM_SCAN_DATA_PART0r_ROBO2, &regval,
                        MAC_LOf, &entry->mac_lo_key);
        /* Program the register */
        rc = REG_WRITE_CB_ITM_ARLFM_SCAN_DATA_PART0r(unit, &regval);
        if (rc != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_CRAL,("soc_robo2_arl_update_scan_data_reg:"
            "Failed writing CB_ITM_ARLFM_SCAN_DATA_PART0 rc(%d)\n", rc));
            return rc;
        }
        tmpval = entry->fid_key;
        SOC_REG_FIELD_SET(unit, CB_ITM_ARLFM_SCAN_DATA_PART1r_ROBO2, &regval,
                        FIDf, &tmpval);
        tmpval = entry->upd_ctrl_key;
        SOC_REG_FIELD_SET(unit, CB_ITM_ARLFM_SCAN_DATA_PART1r_ROBO2, &regval,
                        UPD_CTRLf, &tmpval);
        tmpval = entry->mac_hi_key;
        SOC_REG_FIELD_SET(unit, CB_ITM_ARLFM_SCAN_DATA_PART1r_ROBO2, &regval,
                        MAC_HIf, &tmpval);
        tmpval = entry->fwd_ctrl_key;
        SOC_REG_FIELD_SET(unit, CB_ITM_ARLFM_SCAN_DATA_PART1r_ROBO2, &regval,
                        FWD_CTRLf, &tmpval);
        /* Program the register */
        rc = REG_WRITE_CB_ITM_ARLFM_SCAN_DATA_PART1r(unit, &regval);
        if (rc != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_CRAL,("soc_robo2_arl_update_scan_data_reg:"
            "Failed writing CB_ITM_ARLFM_SCAN_DATA_PART1 rc(%d)\n", rc));
            return rc;
        }
        tmpval = entry->valid_mask;
        SOC_REG_FIELD_SET(unit, CB_ITM_ARLFM_SCAN_MASK_PART2r_ROBO2, &regval,
                        VALIDf, &tmpval);
        tmpval = entry->dst_mask;
        SOC_REG_FIELD_SET(unit, CB_ITM_ARLFM_SCAN_MASK_PART2r_ROBO2, &regval,
                        DSTf, &tmpval);
        tmpval = entry->dst_type_mask;
        SOC_REG_FIELD_SET(unit, CB_ITM_ARLFM_SCAN_MASK_PART2r_ROBO2, &regval,
                        DST_TYPEf, &tmpval);
        tmpval = entry->hit_mask;
        SOC_REG_FIELD_SET(unit, CB_ITM_ARLFM_SCAN_MASK_PART2r_ROBO2, &regval,
                        HITf, &tmpval);
        /* Program the register */
        rc = REG_WRITE_CB_ITM_ARLFM_SCAN_MASK_PART2r(unit, &regval);
        if (rc != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_CRAL,("soc_robo2_arl_update_scan_data_reg:"
            "Failed writing CB_ITM_ARLFM_SCAN_MASK_PART2 rc(%d)\n", rc));
            return rc;
        }
        SOC_REG_FIELD_SET(unit, CB_ITM_ARLFM_SCAN_MASK_PART0r_ROBO2, &regval,
                        MAC_LOf, &entry->mac_lo_mask);
        /* Program the register */
        rc = REG_WRITE_CB_ITM_ARLFM_SCAN_MASK_PART0r(unit, &regval);
        if (rc != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_CRAL,("soc_robo2_arl_update_scan_data_reg:"
            "Failed writing CB_ITM_ARLFM_SCAN_MASK_PART0 rc(%d)\n", rc));
            return rc;
        }
        tmpval = entry->fid_mask;
        SOC_REG_FIELD_SET(unit, CB_ITM_ARLFM_SCAN_MASK_PART1r_ROBO2, &regval,
                        FIDf, &tmpval);
        tmpval = entry->upd_ctrl_mask;
        SOC_REG_FIELD_SET(unit, CB_ITM_ARLFM_SCAN_MASK_PART1r_ROBO2, &regval,
                        UPD_CTRLf, &tmpval);
        tmpval = entry->mac_hi_mask;
        SOC_REG_FIELD_SET(unit, CB_ITM_ARLFM_SCAN_MASK_PART1r_ROBO2, &regval,
                        MAC_HIf, &tmpval);
        tmpval = entry->fwd_ctrl_mask;
        SOC_REG_FIELD_SET(unit, CB_ITM_ARLFM_SCAN_MASK_PART1r_ROBO2, &regval,
                        FWD_CTRLf, &tmpval);
        /* Program the register */
        rc = REG_WRITE_CB_ITM_ARLFM_SCAN_MASK_PART1r(unit, &regval);
        if (rc != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_CRAL,("soc_robo2_arl_update_scan_data_reg:"
            "Failed writing CB_ITM_ARLFM_SCAN_MASK_PART1 rc(%d)\n", rc));
            return rc;
        }
    }
    if (SOC_DRIVER(unit)->type == SOC_CHIP_BCM53158_A0) {
        tmpval = entry->valid_key;
        SOC_REG_FIELD_SET(unit, CB_ITM_ARLFM0_SCAN_DATA_PART0r_ROBO2, &regval,
                        VALIDf, &tmpval);
        tmpval = entry->dst_key;
        SOC_REG_FIELD_SET(unit, CB_ITM_ARLFM0_SCAN_DATA_PART0r_ROBO2, &regval,
                        DSTf, &tmpval);
        tmpval = entry->dst_type_key;
        SOC_REG_FIELD_SET(unit, CB_ITM_ARLFM0_SCAN_DATA_PART0r_ROBO2, &regval,
                        DST_TYPEf, &tmpval);
        tmpval = entry->hit_key;
        SOC_REG_FIELD_SET(unit, CB_ITM_ARLFM0_SCAN_DATA_PART0r_ROBO2, &regval,
                        HITf, &tmpval);
        /* Program the register */
        rc = REG_WRITE_CB_ITM_ARLFM0_SCAN_DATA_PART0r(unit, &regval);
        if (rc != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_CRAL,("soc_robo2_arl_update_scan_data_reg:"
            "Failed writing CB_ITM_ARLFM0_SCAN_DATA_PART0 rc(%d)\n", rc));
            return rc;
        }
        SOC_REG_FIELD_SET(unit, CB_ITM_ARLFM1_SCAN_DATA_PART0r_ROBO2, &regval,
                        MAC_LOf, &entry->mac_lo_key);
        /* Program the register */
        rc = REG_WRITE_CB_ITM_ARLFM1_SCAN_DATA_PART0r(unit, &regval);
        if (rc != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_CRAL,("soc_robo2_arl_update_scan_data_reg:"
            "Failed writing CB_ITM_ARLFM1_SCAN_DATA_PART0 rc(%d)\n", rc));
            return rc;
        }
        tmpval = entry->fid_key;
        SOC_REG_FIELD_SET(unit, CB_ITM_ARLFM1_SCAN_DATA_PART1r_ROBO2, &regval,
                        FIDf, &tmpval);
        tmpval = entry->upd_ctrl_key;
        SOC_REG_FIELD_SET(unit, CB_ITM_ARLFM1_SCAN_DATA_PART1r_ROBO2, &regval,
                        UPD_CTRLf, &tmpval);
        tmpval = entry->mac_hi_key;
        SOC_REG_FIELD_SET(unit, CB_ITM_ARLFM1_SCAN_DATA_PART1r_ROBO2, &regval,
                        MAC_HIf, &tmpval);
        tmpval = entry->fwd_ctrl_key;
        SOC_REG_FIELD_SET(unit, CB_ITM_ARLFM1_SCAN_DATA_PART1r_ROBO2, &regval,
                        FWD_CTRLf, &tmpval);
        /* Program the register */
        rc = REG_WRITE_CB_ITM_ARLFM1_SCAN_DATA_PART1r(unit, &regval);
        if (rc != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_CRAL,("soc_robo2_arl_update_scan_data_reg:"
            "Failed writing CB_ITM_ARLFM1_SCAN_DATA_PART1 rc(%d)\n", rc));
            return rc;
        }
        tmpval = entry->valid_mask;
        SOC_REG_FIELD_SET(unit, CB_ITM_ARLFM0_SCAN_MASK_PART0r_ROBO2, &regval,
                        VALIDf, &tmpval);
        tmpval = entry->dst_mask;
        SOC_REG_FIELD_SET(unit, CB_ITM_ARLFM0_SCAN_MASK_PART0r_ROBO2, &regval,
                        DSTf, &tmpval);
        tmpval = entry->dst_type_mask;
        SOC_REG_FIELD_SET(unit, CB_ITM_ARLFM0_SCAN_MASK_PART0r_ROBO2, &regval,
                        DST_TYPEf, &tmpval);
        tmpval = entry->hit_mask;
        SOC_REG_FIELD_SET(unit, CB_ITM_ARLFM0_SCAN_MASK_PART0r_ROBO2, &regval,
                        HITf, &tmpval);
        /* Program the register */
        rc = REG_WRITE_CB_ITM_ARLFM0_SCAN_MASK_PART0r(unit, &regval);
        if (rc != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_CRAL,("soc_robo2_arl_update_scan_data_reg:"
            "Failed writing CB_ITM_ARLFM0_SCAN_MASK_PART0 rc(%d)\n", rc));
            return rc;
        }
        SOC_REG_FIELD_SET(unit, CB_ITM_ARLFM1_SCAN_MASK_PART0r_ROBO2, &regval,
                        MAC_LOf, &entry->mac_lo_mask);
        /* Program the register */
        rc = REG_WRITE_CB_ITM_ARLFM1_SCAN_MASK_PART0r(unit, &regval);
        if (rc != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_CRAL,("soc_robo2_arl_update_scan_data_reg:"
            "Failed writing CB_ITM_ARLFM1_SCAN_MASK_PART0 rc(%d)\n", rc));
            return rc;
        }
        tmpval = entry->fid_mask;
        SOC_REG_FIELD_SET(unit, CB_ITM_ARLFM1_SCAN_MASK_PART1r_ROBO2, &regval,
                        FIDf, &tmpval);
        tmpval = entry->upd_ctrl_mask;
        SOC_REG_FIELD_SET(unit, CB_ITM_ARLFM1_SCAN_MASK_PART1r_ROBO2, &regval,
                        UPD_CTRLf, &tmpval);
        tmpval = entry->mac_hi_mask;
        SOC_REG_FIELD_SET(unit, CB_ITM_ARLFM1_SCAN_MASK_PART1r_ROBO2, &regval,
                        MAC_HIf, &tmpval);
        tmpval = entry->fwd_ctrl_mask;
        SOC_REG_FIELD_SET(unit, CB_ITM_ARLFM1_SCAN_MASK_PART1r_ROBO2, &regval,
                        FWD_CTRLf, &tmpval);
        /* Program the register */
        rc = REG_WRITE_CB_ITM_ARLFM1_SCAN_MASK_PART1r(unit, &regval);
        if (rc != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_CRAL,("soc_robo2_arl_update_scan_data_reg:"
            "Failed writing CB_ITM_ARLFM1_SCAN_MASK_PART1 rc(%d)\n", rc));
            return rc;
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *    soc_robo2_arl_scanner_operation
 * Description:
 *    Access the scanner for requested operation
 * Parameters:
 *    IN unit  : chip identification
 *    IN entry : Pointer to object of soc_robo2_arl_scan_entry_t
 *    IN cmd   : Scanner operation requested
 *   OUT index : Scanner result for insert/search operations
 * Result:
 *    SOC_E_NONE  on success
 *    SOC_E_XXXX  on failures
 */
int
soc_robo2_arl_scanner_operation(int unit,  soc_robo2_arl_scan_entry_t *entry,
         soc_robo2_arl_scanner_op_t cmd, uint32 *index)
{
    uint32 regval = 0;
    uint32 status = 0;
    uint32 tmpval = 0;
    uint32 flag = 0;
    int rc = 0;

    if ((cmd <= SCANNER_MIN_CMD) || (cmd >= SCANNER_MAX_CMD)) {
        LOG_ERROR(BSL_LS_SOC_CRAL,("soc_robo2_arl_scanner_operation:"
            "Invalid cmd (%d)\n", cmd));
    }

    /* No need to validate entry for resume command as the entry is validated
     * in first read command */
    if (cmd != SCANNER_RESUME) {
        rc = soc_robo2_arl_scan_entry_validate(entry);
        if (rc != SOC_E_NONE) {
            return rc;
        }
    }

    /* Make sure scanner is ready for operation */
    rc = soc_robo2_arl_scanner_wait_for(unit, 100, &status);
    if ((rc != SOC_E_NONE) ||
        ((cmd != SCANNER_RESUME) && (status & SCANNER_STATUS_SEARCH_VALID))) {
        if (!(status & SCANNER_STATUS_READY)) {
            LOG_ERROR(BSL_LS_SOC_CRAL,("soc_robo2_arl_scanner_operation:"
            "Failed before initiating scanner access rc(%d)\n", rc));
            return SOC_E_FAIL;
        }
    }

    /* Set the fields */
    if (cmd != SCANNER_RESUME) {
        rc = soc_robo2_arl_update_scan_data_reg(unit, entry);
        if (rc != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_CRAL,("soc_robo2_arl_scanner_operation:"
            "Failed updating scanner data registers\n", rc));
            return rc;
        }
    }


    regval = 0;
    /* Initiate the cmd */
    tmpval = cmd;
    SOC_REG_FIELD_SET(
        unit, CB_ITM_ARL_SCAN_CONFIGr_ROBO2, &regval, CMDf, &tmpval);
    rc = REG_WRITE_CB_ITM_ARL_SCAN_CONFIGr(unit, &regval);
    if (rc != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_CRAL,("soc_robo2_arl_scanner_operation:"
                "Failed initiating arl access rc(%d)\n", rc));
        return SOC_E_NONE;
    }

    /* Wait for scanner to complete operation */
    rc = soc_robo2_arl_scanner_wait_for(unit, 10000, &status);
    if (rc != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_CRAL,("soc_robo2_arl_scanner_operation:"
                "Failed after initiating scanner access rc(%d)\n", rc));
        return rc;
    }
    switch (cmd) {
    case SCANNER_READ:
    case SCANNER_RESUME:
        flag = SCANNER_STATUS_SEARCH_VALID;
        break;
    case SCANNER_INSERT:
        flag = SCANNER_STATUS_INSERT_VALID;
        break;
    case SCANNER_COUNT:
        /*flag = SCANNER_STATUS_COUNT_VALID;*/
        flag = SCANNER_STATUS_READY;
        break;
    case SCANNER_DELETE:
    default:
        flag = SCANNER_STATUS_READY;
    }
    LOG_VERBOSE(BSL_LS_SOC_CRAL,
         ("soc_robo2_arl_scanner_operation:"
         "status(%x) flag(%x)\n", status, flag));
    if (status & flag) {
        *index = status & SCANNER_STATUS_RESULT_MASK;
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *    soc_robo2_arl_delete_entry
 * Description:
 *    Delete the given entry
 * Parameters:
 *    IN unit  : chip identification
 *    IN entry : Pointer to object of soc_robo2_arl_scan_entry_t
 *   OUT count : Number of entries matching
 * Result:
 *    SOC_E_NONE  on success
 *    SOC_E_XXXX  on failures
 */
int
soc_robo2_arl_delete_entry(int unit,  soc_robo2_arl_scan_entry_t *entry,
        uint32 *count)
{
    int rc = 0;
    if (entry == NULL) {
        LOG_ERROR(BSL_LS_SOC_CRAL, ("soc_robo2_arl_delete_entry:"
             "entry(%p) \n", entry));
        return SOC_E_PARAM;
    }

    rc = soc_robo2_arl_scanner_operation(unit, entry, SCANNER_DELETE, count);
    LOG_VERBOSE(BSL_LS_SOC_CRAL, ("soc_robo2_arl_delete_entry: rc(%d)"
            "Number of entries deleted = %d\n", rc, *count));
    return rc;
}

/*
 * Function:
 *    soc_robo2_arl_count_entry
 * Description:
 *    Count the number of entries matching the given key/mask entry
 * Parameters:
 *    IN unit  : chip identification
 *    IN entry : Pointer to object of soc_robo2_arl_scan_entry_t
 *   OUT count : Number of entries matching
 * Result:
 *    SOC_E_NONE  on success
 *    SOC_E_XXXX  on failures
 */
int
soc_robo2_arl_count_entry(int unit,  soc_robo2_arl_scan_entry_t *entry,
         uint32 *count)
{
    int rc = 0;
    if ((entry == NULL) || (count == NULL)) {
        LOG_ERROR(BSL_LS_SOC_CRAL, ("soc_robo2_arl_count_entry:"
             "entry(%p) count(%p)\n", entry, count));
        return SOC_E_PARAM;
    }

    rc = soc_robo2_arl_scanner_operation(unit, entry, SCANNER_COUNT, count);
    LOG_VERBOSE(BSL_LS_SOC_CRAL, ("soc_robo2_arl_count_entry:"
         "rc(%d) count(%d)\n", rc, *count));
    return rc;
}

/*
 * Function:
 *    soc_robo2_arl_insert_entry
 * Description:
 *    Insert the given entry and return the index of insertion
 * Parameters:
 *    IN unit  : chip identification
 *    IN entry : Pointer to object of soc_robo2_arl_scan_entry_t
 *   OUT index : Index where the entry was inserted
 * Result:
 *    SOC_E_NONE  on success
 *    SOC_E_XXXX  on failures
 */
int
soc_robo2_arl_insert_entry(int unit,  soc_robo2_arl_scan_entry_t *entry,
         uint32 *index)
{
    int rc = 0;
    if ((entry == NULL) || (index == NULL)) {
        LOG_ERROR(BSL_LS_SOC_CRAL, ("soc_robo2_arl_insert_entry:"
             "entry(%p) index(%p)\n", entry, index));
        return SOC_E_PARAM;
    }

    rc = soc_robo2_arl_scanner_operation(unit, entry, SCANNER_INSERT, index);
    LOG_VERBOSE(BSL_LS_SOC_CRAL, ("soc_robo2_arl_insert_entry:"
         "rc(%d) index(%d)\n", rc, *index));
    return rc;
}

/*
 * Function:
 *    soc_robo2_arl_search_first_entry
 * Description:
 *    Search the ARL table for the entry matching given key/mask
 *    Return the first entry, use search_next_entry to get other results
 * Parameters:
 *    IN unit  : chip identification
 *    IN entry : Pointer to object of soc_robo2_arl_scan_entry_t
 *   OUT index : Index of the matching entry
 * Result:
 *    SOC_E_NONE  on success
 *    SOC_E_XXXX  on failures
 */
int
soc_robo2_arl_search_first_entry(int unit,  soc_robo2_arl_scan_entry_t *entry,
         uint32 *index)
{
    int rc = 0;
    if ((entry == NULL) || (index == NULL)) {
        LOG_ERROR(BSL_LS_SOC_CRAL, ("soc_robo2_arl_search_first_entry:"
             "entry(%p) index(%p)\n", entry, index));
        return SOC_E_PARAM;
    }

    rc = soc_robo2_arl_scanner_operation(unit, entry, SCANNER_READ, index);
    LOG_VERBOSE(BSL_LS_SOC_CRAL,("soc_robo2_arl_search_first_entry:"
         "rc(%d) index(%d)\n", rc, *index));
    return rc;
}

/*
 * Function:
 *    soc_robo2_arl_search_next_entry
 * Description:
 *    Resume searching the ARL table for the entry matching given key/mask
 *    return the next match
 * Parameters:
 *    IN unit  : chip identification
 *    IN entry : Pointer to object of soc_robo2_arl_scan_entry_t
 *   OUT index : Index of the matching entry
 * Result:
 *    SOC_E_NONE  on success
 *    SOC_E_XXXX  on failures
 */
int
soc_robo2_arl_search_next_entry(int unit,  soc_robo2_arl_scan_entry_t *entry,
         uint32 *index)
{
    int rc = 0;
    if ((entry == NULL) || (index == NULL)) {
        LOG_ERROR(BSL_LS_SOC_CRAL, ("soc_robo2_arl_search_next_entry:"
             "entry(%p) index(%p)\n", entry, index));
        return SOC_E_PARAM;
    }

    rc = soc_robo2_arl_scanner_operation(unit, entry, SCANNER_RESUME, index);
    LOG_VERBOSE(BSL_LS_SOC_CRAL, ("soc_robo2_arl_search_next_entry:"
         "rc(%d) index(%d)\n", rc, *index));
    return rc;
}