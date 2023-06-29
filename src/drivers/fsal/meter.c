/*
 * * $ID: $
 * * $Copyright: (c) 2017 Broadcom Limited.
 * *
 * * File:
 * *     meter.c
 * * Purpose:
 * *     Robo2 meter module.
 * *
 * */
#include <bsl_log/bsl.h>
#include <sal_types.h>
#include <shared/types.h>
#include <soc/types.h>
#include <soc/robo2/common/tables.h>
#include <soc/robo2/common/memregs.h>
#include <fsal/error.h>
#include <fsal/cosq.h>
#include <fsal/meter.h>
#include <fsal/switch.h>
#include <fsal_int/port.h>
#include <fsal_int/types.h>
#include <fsal_int/meter.h>
#ifdef CONFIG_EXTERNAL_HOST
#include <shared/idxres_afl.h>
#endif /* CONFIG_EXTERNAL_HOST */
/* Internal data structures for holding meter profile info */
STATIC cbxi_meter_info_t bmu_meter_profile[CBXI_MAX_BMU_METER_PROFILE];
STATIC cbxi_meter_info_t ipp_meter_profile[CBXI_MAX_IPP_METER_PROFILE];
STATIC cbxi_meter_info_t pqm_meter_profile[CBXI_MAX_PQM_METER_PROFILE];
#ifdef CONFIG_EXTERNAL_HOST
STATIC cbxi_meter_info_t cfp_meter_profile[CBXI_MAX_CFP_METER_PROFILE];
/* Handle for CFP meter allocation */
STATIC shr_aidxres_list_handle_t  cfp_meter_list_handle[CBX_MAX_UNITS];
#endif /* CONFIG_EXTERNAL_HOST */

cbx_meter_control_t cbx_meter_ctrl = cbxMeterControlPortTC;

/*
 * Function:
 *  cbxi_meter_individual_rate_validate
 * Purpose:
 *  Validate passed meter rate.
 *
 */
int
cbxi_meter_individual_rate_validate(uint32_t rate, uint32_t flags) {

    /* If packet mode, convert pakets/sec to kbps */
    if (flags & CBX_METER_MODE_PACKETS) {
        rate = ((rate * (CBXI_METER_PKT_MODE_PKT_LEN * 8))/ 1000);
    }
    /* Check if meter rate is below 8kbps */
    if (rate < CBXI_METER_MIN_RATE) {
        LOG_VERBOSE(BSL_LS_FSAL_METER,
            (BSL_META("FSAL API : cbxi_meter_individual_rate_validate()...\
                      rate passed is less than minimum\n")));
            return CBX_E_PARAM;
    }
    /* Check if meter rate is above 10gbps */
    if (rate > CBXI_METER_MAX_RATE) {
        LOG_VERBOSE(BSL_LS_FSAL_METER,
            (BSL_META("FSAL API : cbxi_meter_individual_rate_validate()...\
                      rate burst passed is more than max allowed\n")));
            return CBX_E_PARAM;
    }
    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_meter_rate_validate
 * Purpose:
 *  Validate all the meter rate parameters cir,cbs,eir,ebs
 *
 */
int
cbxi_meter_rate_validate(cbx_meter_profile_params_t *meter_params) {

    if ((meter_params->ckbits_sec == 0) && (meter_params->ckbits_burst == 0) &&
        (meter_params->pkbits_sec == 0) && (meter_params->pkbits_burst == 0)) {
        return CBX_E_PARAM;
    }
    /* validate CIR */
    CBX_IF_ERROR_RETURN(
       cbxi_meter_individual_rate_validate(meter_params->ckbits_sec,
                                           meter_params->flags));
    /* validate  CBS */
    CBX_IF_ERROR_RETURN(
       cbxi_meter_individual_rate_validate(meter_params->ckbits_burst,
                                           meter_params->flags));
    /* validate  EIR */
    if (meter_params->mode != cbxMeterModeSrTCM) {
        CBX_IF_ERROR_RETURN(
           cbxi_meter_individual_rate_validate(meter_params->pkbits_sec,
                                               meter_params->flags));
    }
    /*  validate EBS */
    CBX_IF_ERROR_RETURN(
       cbxi_meter_individual_rate_validate(meter_params->pkbits_burst,
                                           meter_params->flags));
    return CBX_E_NONE;
}


/*
 * Function:
 *  cbxi_meter_compute_log2
 * Purpose:
 *  Compute log2 of the number passed
 *
 */
int
cbxi_meter_compute_log2(uint32_t rate, uint8_t *exp) {
    /* Get the exp for the rate */
    if (rate != 0) {
        rate |= rate >> 1;
        rate |= rate >> 2;
        rate |= rate >> 4;
        rate |= rate >> 8;
        *exp = _shr_popcount(rate) - 1;
    } else {
        *exp = 0;
    }
    return CBX_E_NONE;
}
/*
 * Function:
 *  cbxi_meter_div64
 * Purpose:
 *  64 bit division operation implemenation
 *
 */
int cbxi_meter_div64(uint64 x, uint32 y, uint32 *result)
{
    uint64_t rem;
    uint64_t b;
    uint64_t res, d;
    uint32_t high;

    rem =  x;
    COMPILER_64_SET(b, 0, y);
    COMPILER_64_SET(d, 0, 1);

    high = COMPILER_64_HI(rem);

    COMPILER_64_ZERO(res);
    if (high >= y) {
        /* NOTE: Follow code is used to handle 64bits result
         *  high /= y;
         *  res = (uint64) (high << 32);
         *  rem -= (uint64)((high * y) << 32);
         */
        LOG_ERROR(BSL_LS_FSAL_METER,
            (BSL_META("FSAL API : result > 32bit().. \
                                  Invalid parameter\n")));
        return CBX_E_PARAM;
    }

    while ((!COMPILER_64_BITTEST(b, 63)) &&
           (COMPILER_64_LT(b, rem)) ) {
        COMPILER_64_ADD_64(b,b);
        COMPILER_64_ADD_64(d,d);
    }

    do {
        if (COMPILER_64_GE(rem, b)) {
            COMPILER_64_SUB_64(rem, b);
            COMPILER_64_ADD_64(res, d);
        }
        COMPILER_64_SHR(b, 1);
        COMPILER_64_SHR(d, 1);
    } while (!COMPILER_64_IS_ZERO(d));

    *result = COMPILER_64_LO(res);

    return 0;
}

/*
 * Function:
 *  cbxi_meter_convert_rate_to_exp_mantissa
 * Purpose:
 *  Convert rate in kbps/packet per second to exp and mantissa
 *
 */
int
cbxi_meter_convert_rate_to_exp_mantissa(uint32_t rate,
                                        uint32_t flags,
                                        uint8_t *exp,
                                        uint16_t *mantissa) {

    uint64_t uuTemp;
    uint32_t uTemp;
    int rv = CBX_E_NONE;
    if (rate == 0) {
        *exp = 0;
        *mantissa = 0;
        return CBX_E_NONE;
    }
    /* clock speed is 125MHz */
    rate = rate * 10;
    /* rate * 1000 / (8 * 1250000000) = bytes/cycle
     * CIR * (1<<23) = (1+mant/2048)*(1<<(exp-4))
     * rate * 1024 * 1024 / (12500000) = (1+mant/2048)*(1<<(exp-4))
     */
    COMPILER_64_SET(uuTemp, 0, rate);
    COMPILER_64_SHL(uuTemp, 20);
    rv = cbxi_meter_div64(uuTemp, 1250000, &uTemp);
    if (CBX_FAILURE(rv)) {
       /* should never happen here */
       return rv;
    }

    if(uTemp <= 0xFFF) {
    /* find the exponent */
        for(*exp = 0; *exp < 12; (*exp)++) {
            if( (uTemp >> *exp) == 1 ) {
                *exp = *exp + 4;
                break;
            }
        }
        /* find mantissa */
        *mantissa = (uTemp << (15 - *exp)) & 0x7FF;
    } else {
        for(*exp=12; *exp <=31; (*exp)++) {
            if( (uTemp >> *exp) == 1 ) {
                *exp = *exp + 4;
                break;
            }
        }
        *mantissa = (uTemp >> (*exp - 15)) & 0x7FF;
    }
    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_meter_convert_burst_to_exp_mantissa
 * Purpose:
 *  Convert burst in kbps/packet per second to exp and mantissa
 *
 */
int
cbxi_meter_convert_burst_to_exp_mantissa(uint32_t burst,
                                         uint32_t flags,
                                         uint8_t *exp,
                                         uint8_t *mantissa) {
    /* convert rate to bytes/sec */
    if (flags & CBX_METER_MODE_PACKETS) {
        burst = (burst * (CBXI_METER_PKT_MODE_PKT_LEN));
    } else {
        burst = (burst * (1000/8));
    }
    /* Get exponential value e = floor(log2(xBS)) + 1   */
    CBX_IF_ERROR_RETURN(cbxi_meter_compute_log2(burst, exp));

    /* Get mantissa value m = round((xBS/(2^(e-1))) - 1) * 128  */
    if (*exp > 7) {
        *mantissa = ((burst/(1 << (*exp-7))) - 128);
    } else {
        *mantissa = ((burst * 128 /(1 << *exp)) - 128);
    }
    *exp = *exp + 1;
    /* We always want to have burst value equal to or just more than
     * user specified value. In order to do so, bump mantissa by 1
     * If mantissa becomes > 128, make it 0 and bump up exp by 1
     */
    *mantissa += 1;
    if (*mantissa == 128) {
        *mantissa = 0;
        *exp = *exp + 1;
    }
    return CBX_E_NONE;
}


/*
 * Function:
 *  cbxi_meter_convert_exp_mantissa_to_rate
 * Purpose:
 *  Convert rate in exp and mantissa to rate in kbps/packets
 *
 */
int
cbxi_meter_convert_exp_mantissa_to_rate(uint8_t exp,
                                        uint16_t mantissa,
                                        uint32_t flags,
                                        uint32_t *rate) {
#if METER_RATE_FROM_HW
    int rate1, rate2 = 0;
    rate1 =  (1 << (exp - 27 + CBXI_METER_BYTES_PER_SEC_LOG2));
    if (exp >= 21) {
       rate2 = rate1/2048;
       rate2 *= mantissa;
    } else {
       rate2 =  (mantissa * rate1)/ 2048 ;
    }
    *rate = rate1 + rate2;
    if (exp >= 38) {
       *rate = ((mantissa * (1 << (exp - 27 - 11))) + 1 << (exp -27));
    } else {
       *rate = ((mantissa * (1 << (exp -27))/ 2048) + 1 << (exp -27));
    }
    if (exp < 27) {
        *rate = (((mantissa * 1000000) / (1 << (38 - exp))) +
                                       (1000000 / (1 << (27 - exp))));
    } else {
       *rate = ((mantissa * (1000000/(1 << (38 - exp)))) +
                                       (1000000 * (1 << (exp -27))));
    }
    /* convert rate to packets/second  */
    if (flags & CBX_METER_MODE_PACKETS) {
        *rate = ((*rate * 1000) / (8 * CBXI_METER_PKT_MODE_PKT_LEN));
    }
#endif
    return CBX_E_NONE;
}

#if METER_RATE_FROM_HW
/*
 * Function:
 *  cbxi_meter_convert_burst_to_exp_mantissa
 * Purpose:
 *  Convert exp and mantissa to burst in kbps/packets
 *
 */
int
cbxi_meter_convert_exp_mantissa_to_burst(uint8_t exp,
                                         uint8_t mantissa,
                                         uint32_t flags,
                                         uint32_t *burst) {
    uint32_t rate = 0;

    if (exp > 8)  {
        *burst = ((mantissa * (1 << (exp - 1 - 7))) + (1 << (exp -1)));
    } else {
        *burst = ((mantissa * (1 << (exp - 1))/ 128) + (1 << (exp -1)));
    }

    /* convert rate to bytes/sec or packets/sec */
    if (flags & CBX_METER_MODE_PACKETS) {
        *burst = (*burst / (CBXI_METER_PKT_MODE_PKT_LEN));
    } else {
        *burst = ((*burst * 8) / 1000);
    }
    return CBX_E_NONE;
}
#endif /* METER_RATE_FROM_HW */
/*
 * Function:
 *  cbxi_meter_fill_profile_data
 * Purpose:
 *  Fill the profile entry fields as per HW format
 *
 */
int
cbxi_meter_fill_profile_data(bmu_profile_t *profile_entry,
                             cbx_meter_profile_params_t *meter_params) {
    uint32   ckbits = 0;
    uint32   pkbits = 0;
    uint32   c_burst = 0;
    uint32   p_burst = 0;

    /* initialize profile entry to 0 */
    sal_memset(profile_entry, 0, sizeof(bmu_profile_t));

    /* If mode is Coupled MEF BW - set coupling flag */
    if ((meter_params->mode == cbxMeterModeCoupledMEFBw) ||
         (meter_params->mode == cbxMeterModeSrTCM)) {
        profile_entry->cf = 1;
    }
    /* TrTcm means rfc2698 */
    if (meter_params->mode == cbxMeterModeTrTCM) {
        profile_entry->rfc2698 = 1;
    }
    /* Packet mode */
    if (meter_params->flags & CBX_METER_MODE_PACKETS) {
        profile_entry->pkt_mode = 1;
    }
    /* color blind */
    if (meter_params->flags & CBX_METER_COLOR_BLIND) {
        profile_entry->blind = 1;
    }
    if (meter_params->flags & CBX_METER_DROP_RED) {
        profile_entry->drop_on_red = 1;
    }
    if (meter_params->flags & CBX_METER_MODE_PACKETS) {
        ckbits = ((meter_params->ckbits_sec *
                (CBXI_METER_PKT_MODE_PKT_LEN * 8))/ 1000);
        pkbits = ((meter_params->pkbits_sec *
                (CBXI_METER_PKT_MODE_PKT_LEN * 8))/ 1000);
        c_burst = ((meter_params->ckbits_burst *
                (CBXI_METER_PKT_MODE_PKT_LEN * 8))/ 1000);
        p_burst = ((meter_params->pkbits_burst *
                (CBXI_METER_PKT_MODE_PKT_LEN * 8))/ 1000);
    } else {
        ckbits = meter_params->ckbits_sec;
        pkbits = meter_params->pkbits_sec;
        c_burst = meter_params->ckbits_burst;
        p_burst = meter_params->pkbits_burst;
    }
    /* Convert CIR to exp and mantissa */
    CBX_IF_ERROR_RETURN(
        cbxi_meter_convert_rate_to_exp_mantissa(ckbits,
                                     meter_params->flags,
                                     &profile_entry->cir_exp,
                                     &profile_entry->cir_mantissa));
    /* Convert CBS to exp and mantissa */
    CBX_IF_ERROR_RETURN(
        cbxi_meter_convert_burst_to_exp_mantissa(c_burst,
                                     meter_params->flags,
                                     &profile_entry->cbs_exp,
                                     (uint8_t *)&profile_entry->cbs_mantissa));

    /* if SrTcm EIR is always 0 */
    if (meter_params->mode != cbxMeterModeSrTCM) {
    /* Convert EIR to exp and mantissa */
        CBX_IF_ERROR_RETURN(
            cbxi_meter_convert_rate_to_exp_mantissa(pkbits,
                                     meter_params->flags,
                                     &profile_entry->eir_exp,
                                     &profile_entry->eir_mantissa));
    }
    /* Convert EBS to exp and mantissa */
    CBX_IF_ERROR_RETURN(
        cbxi_meter_convert_burst_to_exp_mantissa(p_burst,
                                     meter_params->flags,
                                     &profile_entry->ebs_exp,
                                     (uint8_t *)&profile_entry->ebs_mantissa));
    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_meter_profile_entry_to_param
 * Purpose:
 *  Fill the profile FSAL parameters based on profile entry in HW format
 *
 */
int
cbxi_meter_profile_entry_to_param(bmu_profile_t *profile_entry,
                             cbx_meter_profile_params_t *meter_params) {


    if(profile_entry->pkt_mode == 1) {
        meter_params->flags |= CBX_METER_MODE_PACKETS;
    }
    if(profile_entry->blind == 1) {
        meter_params->flags |= CBX_METER_COLOR_BLIND;
    }
    if(profile_entry->drop_on_red == 1) {
        meter_params->flags |= CBX_METER_DROP_RED;
    }
    if(0 == meter_params->flags) {
        meter_params->flags |= CBX_METER_TYPE_NORMAL;
    }
#if METER_RATE_FROM_HW
    /* Convert exp and mantissa to CIR */
    CBX_IF_ERROR_RETURN(
           cbxi_meter_convert_exp_mantissa_to_rate(
                                             profile_entry->cir_exp,
                                             profile_entry->cir_mantissa,
                                             meter_params->flags,
                                             &meter_params->ckbits_sec));
    /* Convert exp and mantissa to CBS */
    CBX_IF_ERROR_RETURN(
           cbxi_meter_convert_exp_mantissa_to_burst(
                                             profile_entry->cbs_exp,
                                             profile_entry->cbs_mantissa,
                                             meter_params->flags,
                                             &meter_params->ckbits_burst));

    /* Convert exp and mantissa to EIR */
    CBX_IF_ERROR_RETURN(
           cbxi_meter_convert_exp_mantissa_to_rate(
                                             profile_entry->eir_exp,
                                             profile_entry->eir_mantissa,
                                             meter_params->flags,
                                             &meter_params->pkbits_sec));
    /* Convert exp and mantissa to EBS */
    CBX_IF_ERROR_RETURN(
           cbxi_meter_convert_exp_mantissa_to_burst(
                                             profile_entry->ebs_exp,
                                             profile_entry->ebs_mantissa,
                                             meter_params->flags,
                                             &meter_params->pkbits_burst));
#endif
    return CBX_E_NONE;
}

/*
 * Function:
 *   cbxi_meter_profile_entry_compare
 * Purpose:
 *   Compare the 2 profile entries. return CBX_E_NONE if both are same and
 *   CBX_E_FAIL otherwise
 *
 */
int
cbxi_meter_profile_entry_compare(bmu_profile_t *new, bmu_profile_t *old) {
    int rv = CBX_E_FAIL;
    if(new->cbs_exp != old->cbs_exp) {
        return rv;
    }
    if(new->cbs_mantissa != old->cbs_mantissa) {
        return rv;
    }
    if(new->cir_mantissa != old->cir_mantissa) {
        return rv;
    }
    if(new->cir_exp != old->cir_exp) {
        return rv;
    }
    if(new->eir_exp != old->eir_exp) {
        return rv;
    }
    if(new->ebs_exp != old->ebs_exp) {
        return rv;
    }
    if(new->eir_mantissa != old->eir_mantissa) {
        return rv;
    }
    if(new->ebs_mantissa != old->ebs_mantissa) {
        return rv;
    }
    if(new->drop_on_red != old->drop_on_red) {
        return rv;
    }
    if(new->pkt_mode != old->pkt_mode) {
        return rv;
    }
    if(new->bktc_strict != old->bktc_strict) {
        return rv;
    }
    if(new->bkte_strict != old->bkte_strict) {
        return rv;
    }
    if(new->cf != old->cf) {
        return rv;
    }
    if(new->rfc2698 != old->rfc2698) {
        return rv;
    }
    if(new->blind != old->blind) {
        return rv;
    }
    if(new->avb_mode != old->avb_mode) {
        return rv;
    }
    return CBX_E_NONE;
}

/*
 * Function:
 *   cbxi_meter_default_profile_create
 * Purpose:
 *   Create default meter profile for BMU, IPP, PQM and CFP
 *
 */
int
cbxi_meter_default_profile_create(void) {

    bmu_profile_t profile_entry;
    uint32  status  = 0;
    cbxi_meter_info_t *profile_data = NULL;
    cbx_meter_profile_params_t meter_params;
    int     meter_type = 0;

    sal_memset(&profile_entry, 0, sizeof(bmu_profile_t));

    while (meter_type <= cbxMeterCFP) {
        switch(meter_type) {
            case cbxMeterBufferManager:
                profile_data = bmu_meter_profile;

                meter_params.mode = cbxMeterModeSrTCM;
                meter_params.flags = CBX_METER_TYPE_NORMAL |
                                                CBX_METER_COLOR_BLIND;
                meter_params.ckbits_sec = 10000000;
                meter_params.ckbits_burst = 10000000;
                meter_params.pkbits_sec = 10000000;
                meter_params.pkbits_burst = 10000000;
                meter_params.type = meter_type;

                CBX_IF_ERROR_RETURN(cbxi_meter_fill_profile_data(
                                        &profile_entry, &meter_params));

                CBX_IF_ERROR_RETURN(
                   soc_robo2_bmu_profile_set(CBX_AVENGER_PRIMARY, 0,
                                             &profile_entry, &status));
                if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
                    CBX_IF_ERROR_RETURN(
                       soc_robo2_bmu_profile_set(CBX_AVENGER_SECONDARY, 0,
                                             &profile_entry, &status));
                }
                break;

            case cbxMeterTrafficType:
                profile_data = ipp_meter_profile;
                CBX_IF_ERROR_RETURN(
                   soc_robo2_ipp_profile_set(CBX_AVENGER_PRIMARY, 0,
                                     (ipp_profile_t *)&profile_entry, &status));
                if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
                    CBX_IF_ERROR_RETURN(
                       soc_robo2_ipp_profile_set(CBX_AVENGER_SECONDARY, 0,
                                     (ipp_profile_t *)&profile_entry, &status));
                }
                break;

            case cbxMeterTrapGroup:
                profile_data = pqm_meter_profile;
                CBX_IF_ERROR_RETURN(
                   soc_robo2_pqm_profile_set(CBX_AVENGER_PRIMARY, 0,
                                     (pqm_profile_t *)&profile_entry, &status));
                if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
                    CBX_IF_ERROR_RETURN(
                       soc_robo2_pqm_profile_set(CBX_AVENGER_SECONDARY, 0,
                                     (pqm_profile_t *)&profile_entry, &status));
                }
                break;
            case cbxMeterCFP:
#ifdef CONFIG_EXTERNAL_HOST
                profile_data = cfp_meter_profile;
                CBX_IF_ERROR_RETURN(
                   soc_robo2_cfp_profile_set(CBX_AVENGER_PRIMARY, 0,
                                    (cfp_profile_t *)&profile_entry, &status));
                if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
                    CBX_IF_ERROR_RETURN(
                       soc_robo2_cfp_profile_set(CBX_AVENGER_SECONDARY, 0,
                                     (cfp_profile_t *)&profile_entry, &status));
                }
#endif /* CONFIG_EXTERNAL_HOST */
                break;
        }
        if (meter_type == cbxMeterBufferManager) {
            profile_data[0].pid |= CBXI_METER_PROFILE_DATA_VALID;
            profile_data[0].mode = cbxMeterModeSrTCM;
            profile_data[0].ckbits_sec = 10000000;
            profile_data[0].ckbits_burst = 10000000;
            profile_data[0].pkbits_sec = 10000000;
            profile_data[0].pkbits_burst = 10000000;
        } else {
            profile_data[0].pid |= CBXI_METER_PROFILE_DATA_VALID;
            profile_data[0].mode = 0;
            profile_data[0].ckbits_sec = 0;
            profile_data[0].ckbits_burst = 0;
            profile_data[0].pkbits_sec = 0;
            profile_data[0].pkbits_burst = 0;

        }
        meter_type++;
    }
    return CBX_E_NONE;
}

/*
 * Function:
 *   cbxi_meter_profile_entry_get
 * Purpose:
 *   get a profile entry from HW
 *
 */
int
cbxi_meter_profile_entry_get(int unit, cbx_meter_type_t type,
                             int index, bmu_profile_t *entry) {

    switch(type) {
        case cbxMeterBufferManager:
            return(soc_robo2_bmu_profile_get(unit, index, entry));
            break;

        case cbxMeterTrafficType:
            return(soc_robo2_ipp_profile_get(unit, index,
                                             (ipp_profile_t *)entry));
            break;

        case cbxMeterTrapGroup:
            return(soc_robo2_pqm_profile_get(unit, index,
                                             (pqm_profile_t *)entry));
            break;
        case cbxMeterCFP:
#ifdef CONFIG_EXTERNAL_HOST
            return(soc_robo2_cfp_profile_get(unit, index,
                                             (cfp_profile_t *)entry));
#endif /* CONFIG_EXTERNAL_HOST */
            break;
        default:
            return CBX_E_PARAM;
            break;
    }
    return CBX_E_NONE;
}

/*
 * Function:
 *   cbxi_meter_profile_entry_set
 * Purpose:
 *   set a profile entry in HW
 *
 */
int
cbxi_meter_profile_entry_set(int unit, cbx_meter_type_t type,
                             int index, bmu_profile_t *entry) {

    uint32  status  = 0;

    switch(type) {
        case cbxMeterBufferManager:
            return(soc_robo2_bmu_profile_set(unit, index, entry, &status));
            break;

        case cbxMeterTrafficType:
            return(soc_robo2_ipp_profile_set(unit, index,
                                   (ipp_profile_t *)entry, &status));
            break;

        case cbxMeterTrapGroup:
            return(soc_robo2_pqm_profile_set(unit, index,
                                   (pqm_profile_t *)entry, &status));
            break;
        case cbxMeterCFP:
#ifdef CONFIG_EXTERNAL_HOST
            return(soc_robo2_cfp_profile_set(unit, index,
                                   (cfp_profile_t *)entry, &status));
#endif /* CONFIG_EXTERNAL_HOST */
            break;
        default:
            return CBX_E_PARAM;
            break;
    }
    return CBX_E_NONE;
}

/*
 * Function:
 *   cbxi_meter_bmu_mtr_config_update
 * Purpose:
 *   Update BMU MTR CONFIG register
 *
 */
int
cbxi_meter_bmu_mtr_config_update(int unit, bmu_profile_t *profile_entry) {
    uint32  meter_config;
    uint32  fval = 0;
    CBX_IF_ERROR_RETURN(
          REG_READ_CB_BMU_MTR_CONFIGr(unit, &meter_config));
    if (profile_entry->drop_on_red == 1) {
        fval = 1;
        soc_CB_BMU_MTR_CONFIGr_field_set(unit, &meter_config,
                                       DROP_ON_REDf, &fval);
    }
    if (profile_entry->rfc2698 == 1) {
        fval = 1;
        soc_CB_BMU_MTR_CONFIGr_field_set(unit, &meter_config,
                                       RFC2698f, &fval);
    }
    if (profile_entry->blind == 1) {
        fval = 1;
        soc_CB_BMU_MTR_CONFIGr_field_set(unit, &meter_config,
                                       BLINDf, &fval);
    }
    CBX_IF_ERROR_RETURN(
              REG_WRITE_CB_BMU_MTR_CONFIGr(unit, &meter_config));
    return CBX_E_NONE;
}

/*
 * Function:
 *   cbxi_meter_profile_update
 * Purpose:
 *   Write profile entry to HW
 *
 */
int
cbxi_meter_profile_update(bmu_profile_t *profile_entry,
                       cbx_meter_profile_params_t *meter_params,
                       cbx_meter_type_t type,
                       cbx_meter_profile_t *profile_index,
                       int              flag) {

    cbxi_meter_info_t  *profile_data = NULL;
    int           max_index = 0;
    int           i = 0;
    int           index = CBX_METER_INVALID;
    int           unit = CBX_AVENGER_PRIMARY;
    int           rv = CBX_E_NONE;
    bmu_profile_t existing_entry;

    /* Based on type get profile management data structure and maximum
       number of profiles allowed for the type */
    switch(type) {
        case cbxMeterBufferManager:
            profile_data = bmu_meter_profile;
            max_index = CBXI_MAX_BMU_METER_PROFILE;
            break;

        case cbxMeterTrafficType:
            profile_data = ipp_meter_profile;
            max_index = CBXI_MAX_IPP_METER_PROFILE;
            break;

        case cbxMeterTrapGroup:
            profile_data = pqm_meter_profile;
            max_index = CBXI_MAX_PQM_METER_PROFILE;
            break;
        case cbxMeterCFP:
#ifdef CONFIG_EXTERNAL_HOST
            profile_data = cfp_meter_profile;
            max_index = CBXI_MAX_CFP_METER_PROFILE;
#else
            return CBX_E_PARAM;
#endif /* CONFIG_EXTERNAL_HOST */
            break;
        default:
            return CBX_E_PARAM;
            break;
    }
    /* For create - either find an exisiting matching profile or allocate
       a new index */
    if (flag & CBXI_METER_PROFILE_CREATE) {
        for(i = 0; i < max_index; i++) {
            if (profile_data[i].pid & CBXI_METER_PROFILE_DATA_VALID) {
            /* read profile entry from HW and compare with entry to be added */
                CBX_IF_ERROR_RETURN(
                    cbxi_meter_profile_entry_get(unit, type,
                                                 i, &existing_entry));
                    rv = cbxi_meter_profile_entry_compare(profile_entry,
                                                          &existing_entry);
                    if (CBX_SUCCESS(rv)) {
                        *profile_index = i;
                        return rv;
                    }
            } else {
                if(index == CBX_METER_INVALID) {
                    index = i;
                }
            }

        }
        if(index == CBX_METER_INVALID) {
            return CBX_E_FULL;
        }
    /* for update - get profile index from profile passed */
    } else if(flag & CBXI_METER_PROFILE_UPDATE) {
       index = *profile_index & 0xFF;
       if (index >= max_index) {
            return CBX_E_PARAM;
       }
    } else {
        return CBX_E_PARAM;
    }
    /* Add entry to HW at free_index */
    unit = CBX_AVENGER_PRIMARY;
    CBX_IF_ERROR_RETURN(
            cbxi_meter_profile_entry_set(unit, type, index, profile_entry));
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        unit = CBX_AVENGER_SECONDARY;
        CBX_IF_ERROR_RETURN(
            cbxi_meter_profile_entry_set(unit, type, index, profile_entry));
    }
    /* for BMU meters drop on red, rfc 2698, color blind */
    if (type == cbxMeterBufferManager) {
        unit = CBX_AVENGER_PRIMARY;
        CBX_IF_ERROR_RETURN(cbxi_meter_bmu_mtr_config_update(unit,
                                                             profile_entry));
        if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
            unit = CBX_AVENGER_SECONDARY;
            CBX_IF_ERROR_RETURN(cbxi_meter_bmu_mtr_config_update(unit,
                                                             profile_entry));
        }
    }
    /* Set valid bit in the profile management data structure */
    profile_data[index].pid |= CBXI_METER_PROFILE_DATA_VALID;
    profile_data[index].mode =
                        ((meter_params->mode & 0xf) << CBXI_METER_MODE_SHIFT);
    profile_data[index].ckbits_sec = meter_params->ckbits_sec;
    profile_data[index].ckbits_burst = meter_params->ckbits_burst;
    profile_data[index].pkbits_sec = meter_params->pkbits_sec;
    profile_data[index].pkbits_burst = meter_params->pkbits_burst;
    /* Set profile index */
    *profile_index = (cbx_meter_profile_t)index;
    return CBX_E_NONE;
}

/*
 * Function:
 *   cbxi_meter_profile_set
 * Purpose:
 *   Internal function to update profile entry in HW. If flag is set to
 *   CREATE, create a new profile entry. If set to UPDATE, modify the existing
 *   entry
 *
 */
int
cbxi_meter_profile_set(cbx_meter_profile_params_t *meter_params,
                       cbx_meter_profile_t      *profileid,
                       int                      flag) {

    int rv = CBX_E_NONE;
    bmu_profile_t profile_entry;

    if ( NULL == meter_params ) {
        LOG_ERROR(BSL_LS_FSAL_METER,
            (BSL_META("FSAL API : cbxi_meter_profile_set().. \
                                   Invalid parameter\n")));
        return CBX_E_PARAM;
    }
    /* validate CIR,CBS,EIR,EBS */
    rv = cbxi_meter_rate_validate(meter_params);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_METER,
            (BSL_META("FSAL API : cbxi_meter_profile_set().. \
                                   Invalid rate passed\n")));
        return rv;
    }
    /* Shaping is supported only for BMU meters when
       flowcontrol mode is meter */
    if (meter_params->flags & CBX_METER_SHAPING) {
       if (meter_params->type != cbxMeterBufferManager) {
            LOG_ERROR(BSL_LS_FSAL_METER,
                (BSL_META("FSAL API : cbxi_meter_profile_set().. \
                            Shaping is supported only for BMU\n")));
            return CBX_E_PARAM;
        } else {
#if 0
            rv = cbx_cosq_flowcontrol_get(&fc);
            if ((rv != CBX_E_NONE) || (fc != cbxFlowcontrolMeter)) {
                LOG_ERROR(BSL_LS_FSAL_METER,
                    (BSL_META("FSAL API : cbxi_meter_profile_set().. \
                            Shaping is supported only for BMU\n")));
                return CBX_E_PARAM;
            }
#endif
        }
    }
    /* Fill in the profile entry from profile parameters passed */
    rv = cbxi_meter_fill_profile_data(&profile_entry, meter_params);
    if (CBX_FAILURE(rv)) {
        LOG_VERBOSE(BSL_LS_FSAL_METER,
            (BSL_META("FSAL API : cbxi_meter_profile_set().. \
                             could not create profile data\n")));
        return rv;
    }
    /* Get a profile index and write to HW */
    rv = cbxi_meter_profile_update(&profile_entry, meter_params,
                                   meter_params->type,
                                   profileid, flag);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_METER,
            (BSL_META("FSAL API : cbxi_meter_profile_set().. \
                                could not add profile to HW\n")));
        return rv;
    }
    /* Shaping is supported only for BMU meters when
       flowcontrol mode is meter */
    if (meter_params->flags & CBX_METER_SHAPING) {
       if (meter_params->type != cbxMeterBufferManager) {
            LOG_ERROR(BSL_LS_FSAL_METER,
                (BSL_META("FSAL API : cbxi_meter_profile_set().. \
                            Shaping is supported only for BMU\n")));
            return CBX_E_PARAM;
        }
    }
    return rv;
}

/*
 * Function:
 *      cbxi_meter_index_validate
 * Purpose:
 *      validate IPP/BMU/PQM/CFP meter index passed
 */
int
cbxi_meter_index_validate(int unit,
                         cbx_meter_type_t type, int meter_index) {
    int max_index = 0;

    switch(type) {
        case cbxMeterBufferManager:
            max_index =  soc_robo2_bmu_pptr_max_index(unit);
            break;

        case cbxMeterTrafficType:
            max_index =  soc_robo2_ipp_pptr_max_index(unit);
            break;

        case cbxMeterTrapGroup:
            max_index =  soc_robo2_pqm_pptr_max_index(unit);
            break;
        case cbxMeterCFP:
#ifdef CONFIG_EXTERNAL_HOST
            max_index =  soc_robo2_cfp_pptr_max_index(unit);
#else
            return CBX_E_PARAM;
#endif /* CONFIG_EXTERNAL_HOST */
            break;
        default:
            return CBX_E_PARAM;
            break;
    }
    if (meter_index > max_index) {
        return CBX_E_PARAM;

    }
    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_meter_bmu_config
 * Purpose:
 *  Set meter id selection tp spg,tc for BMU meters
 *  packet mode
 *
 */
int
cbxi_meter_bmu_config(void) {
    uint32    fval = 0; /* use SPG and TC for meter index */
    int       unit;
    uint32    meter_config;
    int       num_units = 1;
    /* Get whether we are in a cascade set-up or not */
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = 2;
    }
    /* set meter id selection based on SPG and TC */
    for (unit = 0; unit < num_units; unit++) {
        fval = 1;
        CBX_IF_ERROR_RETURN(
              REG_READ_CB_BMU_MTR_CONFIGr(unit, &meter_config));
        soc_CB_BMU_MTR_CONFIGr_field_set(unit, &meter_config,
                                       METER_ID_SELf, &fval);
        fval = 0; /* use meter output as color */
        soc_CB_BMU_MTR_CONFIGr_field_set(unit, &meter_config,
                                       METER_DP_SELf, &fval);
        CBX_IF_ERROR_RETURN(
              REG_WRITE_CB_BMU_MTR_CONFIGr(unit, &meter_config));
    }
    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_meter_mtr_control_set
 * Purpose:
 *  Set Packet length to be used when metering is set to be in
 *  packet mode
 *
 */
int
cbxi_meter_mtr_control_set(void) {
    uint32    fval = CBXI_METER_PKT_MODE_PKT_LEN;
    int       unit = CBX_AVENGER_PRIMARY;
    uint32    meter_control;
    uint32    mtr_ctrl = 0;
    int       num_units = 1;
    /* Get whether we are in a cascade set-up or not */
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = 2;
    }
    soc_CB_PQM_MTR_CONTROLr_field_set(unit, &mtr_ctrl,
                                       PKT_MODE_LENf, &fval);
    fval = 1;
    soc_CB_PQM_MTR_CONTROLr_field_set(unit, &mtr_ctrl,
                                       REFRESH_ENf, &fval);
    /* set MTR_CONTROL */
    for (unit = 0; unit < num_units; unit++) {
        CBX_IF_ERROR_RETURN(
              REG_READ_CB_PQM_MTR_CONTROLr(unit, &meter_control));
        meter_control |= mtr_ctrl;
        CBX_IF_ERROR_RETURN(
              REG_WRITE_CB_PQM_MTR_CONTROLr(unit, &meter_control));

        CBX_IF_ERROR_RETURN(REG_READ_CB_BMU_MTR_CONTROLr(unit, &meter_control));
        meter_control |= mtr_ctrl;
        CBX_IF_ERROR_RETURN(
              REG_WRITE_CB_BMU_MTR_CONTROLr(unit, &meter_control));

        CBX_IF_ERROR_RETURN(REG_READ_CB_IPP_MTR_CONTROLr(unit, &meter_control));
        meter_control |= mtr_ctrl;
        CBX_IF_ERROR_RETURN(
              REG_WRITE_CB_IPP_MTR_CONTROLr(unit, &meter_control));
#ifdef CONFIG_EXTERNAL_HOST
        CBX_IF_ERROR_RETURN(REG_READ_CB_CFP_MTR_CONTROLr(unit, &meter_control));
        meter_control |= mtr_ctrl;
        CBX_IF_ERROR_RETURN(
              REG_WRITE_CB_CFP_MTR_CONTROLr(unit, &meter_control));
#endif /* CONFIG_EXTERNAL_HOST */
    }
    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_meter_control_bucket_set
 * Purpose:
 *  Set the value for committed and excess bucket
 *  packet mode
 *
 */
int
cbxi_meter_control_bucket_set(int unit, int index,
                              cbx_meter_type_t type,
                              bmu_bucket_t *bucket_entry) {

    uint32    status  = 0;

    switch(type) {
        case cbxMeterBufferManager:
            return(soc_robo2_bmu_bucket_set(unit, index,
                                            bucket_entry, &status));
            break;

        case cbxMeterTrafficType:
            return(soc_robo2_ipp_bucket_set(unit, index,
                                       (ipp_bucket_t *)bucket_entry, &status));
            break;

        case cbxMeterTrapGroup:
            return(soc_robo2_pqm_bucket_set(unit, index,
                                       (pqm_bucket_t *)bucket_entry, &status));
            break;

        case cbxMeterCFP:
#ifdef CONFIG_EXTERNAL_HOST
            return(soc_robo2_cfp_bucket_set(unit, index,
                                       (cfp_bucket_t *)bucket_entry, &status));
#else
            return CBX_E_PARAM;
#endif /* CONFIG_EXTERNAL_HOST */
            break;

        default:
            return CBX_E_PARAM;
            break;
    }
    return CBX_E_NONE;
}
/*
 * Function:
 *      cbxi_meter_op
 * Purpose:
 *      Allocate/free a meter index and update PPTR pointer table. Also
 *      increment/decrement profile usage count
 */
int
cbxi_meter_op(int unit,
              cbx_port_t port_out,
              cbx_meter_profile_t profileid,
              cbx_meter_params_t *meter_params,
              cbx_meterid_t      *meterid,
              int flag) {

    cbx_meter_type_t type;
    cbx_meter_mode_t mode = 0;
    int              profile_index;
    uint32           meter_index = 0;
    bmu_pptr_t       pptr_entry;
    bmu_bucket_t     bucket_entry;
    cbxi_meter_info_t  *profile_data = NULL;
    int              max_index = 0;
    uint32           status  = 0;
    int              valid = 0;
    int              i = 0;
    bmu_profile_t    profile_entry;
    mtr2tcb_t        entry;
    cbx_flowcontrol_t fc;

    if (flag & CBXI_METER_CREATE) {
        /*  Extract profile type and profile index from profile id */
        type = (profileid & CBXI_METER_TYPE_MASK) >> CBXI_METER_TYPE_SHIFT;
        profile_index = (profileid & 0xFF);
        mode = (profileid & CBXI_METER_MODE_MASK) >> CBXI_METER_MODE_SHIFT;
        bucket_entry.bkte = CBXI_METER_INITIAL_BUCKET_COUNT;
        bucket_entry.bktc = CBXI_METER_INITIAL_BUCKET_COUNT;
    } else if(flag & CBXI_METER_DESTROY) {
        /*  Extract profile type and profile index, meter index and
            unit number from meterid */
        valid = ((*meterid & CBXI_METER_INDEX_VALID)
                                          >> CBXI_METER_INDEX_VALID_SHIFT);
        if (valid == 0) {
            return CBX_E_PARAM;
        }
        type = (*meterid & CBXI_METER_TYPE_MASK) >> CBXI_METER_TYPE_SHIFT;
        profile_index = ((*meterid & CBXI_METER_PROFILE_INDEX_MASK) >>
                                    CBXI_METER_PROFILE_INDEX_SHIFT);
        meter_index= (*meterid & CBXI_METER_INDEX_MASK);
        unit = ((*meterid & CBXI_METER_UNIT_MASK) >> CBXI_METER_UNIT_SHIFT);
        bucket_entry.bkte = 0;
        bucket_entry.bktc = 0;
   } else {
       return CBX_E_PARAM;
   }

    /* Update PPTR table entry with profile pointer index */
    if(flag & CBXI_METER_DESTROY) {
        pptr_entry.pointer = CBXI_METER_DEFAULT_PROFILE;  /* default profile */
    } else {
        pptr_entry.pointer = profile_index;
    }

    switch(type) {
        case cbxMeterBufferManager:
            profile_data = bmu_meter_profile;
            max_index = CBXI_MAX_BMU_METER_PROFILE;
            if (flag & CBXI_METER_CREATE) {
                if (meter_params->tc.int_pri > CBXI_METER_MAX_INT_PRI) {
                    return CBX_E_PARAM;
                }
                meter_index = ((port_out * CBXI_METER_BMU_PER_PORT_METERS) +
                         meter_params->tc.int_pri);
            }
            CBX_IF_ERROR_RETURN(soc_robo2_bmu_pptr_set(unit, meter_index,
                                        &pptr_entry, &status));
            CBX_IF_ERROR_RETURN(cbx_cosq_flowcontrol_get(&fc));
            if (fc == cbxFlowcontrolMeter) {
                CBX_IF_ERROR_RETURN(
                    soc_robo2_mtr2tcb_get(unit, meter_index, &entry));
                entry.tc_bmp = 0;
                if ((flag & CBXI_METER_CREATE)) {
                    entry.tc_bmp =
                        1 << (meter_index % CBXI_METER_BMU_PER_PORT_METERS);
                }
                CBX_IF_ERROR_RETURN(soc_robo2_mtr2tcb_set(unit, meter_index,
                                                          &entry, &status));
            }
            break;

        case cbxMeterTrafficType:
            profile_data = ipp_meter_profile;
            max_index = CBXI_MAX_IPP_METER_PROFILE;
            if (flag & CBXI_METER_CREATE) {
                if (meter_params->traffic_type >
                                         CBXI_METER_IPP_PER_PORT_METERS) {
                    return CBX_E_PARAM;
                }
                meter_index = ((port_out * CBXI_METER_IPP_PER_PORT_METERS) +
                          meter_params->traffic_type);
            }
            CBX_IF_ERROR_RETURN(soc_robo2_ipp_pptr_set(unit, meter_index,
                                        (ipp_pptr_t *)&pptr_entry, &status));
            break;

        case cbxMeterTrapGroup:
            profile_data = pqm_meter_profile;
            max_index = CBXI_MAX_PQM_METER_PROFILE;
            if (flag & CBXI_METER_CREATE) {
                meter_index = meter_params->trap_group_index;
            }
            if (meter_index > CBXI_METER_TRAP_GROUP_MAX_INDEX) {
                return CBX_E_PARAM;
            }
            CBX_IF_ERROR_RETURN(soc_robo2_pqm_pptr_set(unit, meter_index,
                                        (pqm_pptr_t *)&pptr_entry, &status));
            break;

        case cbxMeterCFP:
#ifdef CONFIG_EXTERNAL_HOST
            profile_data = cfp_meter_profile;
            max_index = CBXI_MAX_CFP_METER_PROFILE;
            if (flag & CBXI_METER_CREATE) {
                /* Allocate a meter index */
                CBX_IF_ERROR_RETURN(shr_aidxres_list_alloc(
                                    cfp_meter_list_handle[unit],
                                    &meter_index));
                CBX_IF_ERROR_RETURN(soc_robo2_cfp_pptr_set(unit, meter_index,
                                        (cfp_pptr_t *)&pptr_entry, &status));
            }
#else
            return CBX_E_PARAM;
#endif /* CONFIG_EXTERNAL_HOST */
            break;
        default:
            LOG_VERBOSE(BSL_LS_FSAL_METER,
                (BSL_META("FSAL API : cbxi_meter_op().. \
                              Invalid profile id/meter id passed\n")));
            return CBX_E_PARAM;
            break;
    }
    if (profile_index >= max_index) {
        LOG_VERBOSE(BSL_LS_FSAL_METER,
            (BSL_META("FSAL API : cbxi_meter_op().. \
                          Invalid profile id passed\n")));
        return CBX_E_PARAM;
    }
    CBX_IF_ERROR_RETURN(cbxi_meter_control_bucket_set(unit, meter_index,
                                                      type, &bucket_entry));

    /* Increment/decrement reference count for profile usage */
    if(flag & CBXI_METER_DESTROY) {
#ifdef CONFIG_EXTERNAL_HOST
        /* free meter index */
        if (type == cbxMeterCFP) {
            CBX_IF_ERROR_RETURN(shr_aidxres_list_free(
                                    cfp_meter_list_handle[unit],
                                    meter_index));
        }
#endif /* CONFIG_EXTERNAL_HOST */
        if (profile_data[profile_index].pid > 0) {
            profile_data[profile_index].pid -= 1;
        } else {
            return CBX_E_PARAM;
        }
        for (i = 0; i < max_index; i++)  {
            /* Check profile usage count */
            if (profile_data[i].pid & CBXI_METER_PROFILE_DATA_REF_COUNT) {
                return CBX_E_NONE;
            }
        }
        /* Enable bypass mode */
        flag = CBXI_METER_CONTROL_OP_ENABLE;
        CBX_IF_ERROR_RETURN(cbxi_meter_control_bypass_set(unit, type, flag));
        /* clear drop on red, refc2698 and blind setting for BMU meters */
        if(type == cbxMeterBufferManager) {
            profile_entry.drop_on_red = 0;
            profile_entry.rfc2698 = 0;
            profile_entry.blind = 0;
            CBX_IF_ERROR_RETURN(cbxi_meter_bmu_mtr_config_update(unit,
                                                       &profile_entry));
        }
    } else {
        profile_data[profile_index].pid += 1;
        *meterid = meter_index;
        *meterid |= type << CBXI_METER_TYPE_SHIFT;
        *meterid |= unit << CBXI_METER_UNIT_SHIFT;
        *meterid |= CBXI_METER_INDEX_VALID;
        *meterid |= mode << CBXI_METER_MODE_SHIFT;
        *meterid |=  profile_index << CBXI_METER_PROFILE_INDEX_SHIFT;
        /* Disable bypass mode */
        flag = CBXI_METER_CONTROL_OP_DISABLE;
        CBX_IF_ERROR_RETURN(cbxi_meter_control_bypass_set(unit, type, flag));
    }
    return CBX_E_NONE;
}

/*
 * Function:
 *      cbxi_meter_control_bypass_set
 * Purpose:
 *      Enable/Disable meter bypass mode so that color info is carried to
 *      next stage
 */
int
cbxi_meter_control_bypass_set(int unit, cbx_meter_type_t type, int flag) {
    uint32 meter_control = 0;
    uint32 fval = 0;
    /* Based on type read MTR control register */
    switch(type) {
        case cbxMeterBufferManager:
            CBX_IF_ERROR_RETURN(
                   REG_READ_CB_BMU_MTR_CONTROLr(unit, &meter_control));
            break;

        case cbxMeterTrafficType:
            CBX_IF_ERROR_RETURN(
                   REG_READ_CB_IPP_MTR_CONTROLr(unit, &meter_control));
            break;

        case cbxMeterTrapGroup:
            CBX_IF_ERROR_RETURN(
                   REG_READ_CB_PQM_MTR_CONTROLr(unit, &meter_control));
            break;
        case cbxMeterCFP:
#ifdef CONFIG_EXTERNAL_HOST
            CBX_IF_ERROR_RETURN(
                   REG_READ_CB_CFP_MTR_CONTROLr(unit, &meter_control));
#else
            return CBX_E_PARAM;
#endif /* CONFIG_EXTERNAL_HOST */
            break;
        default:
            return CBX_E_PARAM;
            break;
    }
    /* disable bypass mode  */
    soc_CB_PQM_MTR_CONTROLr_field_get(unit, &meter_control,
                                       BYPASS_MODEf, &fval);
    if (flag == CBXI_METER_CONTROL_OP_ENABLE) {
        if (fval == 1) {
            return CBX_E_NONE;
        } else {
            fval = 0;
        }
    } else {
        if (fval == 1) {
            fval = 0;
        } else {
            return CBX_E_NONE;
        }

    }
    soc_CB_PQM_MTR_CONTROLr_field_set(unit, &meter_control,
                                       BYPASS_MODEf, &fval);

    /* Write updated MTR_CONTROL to HW */
    switch(type) {
        case cbxMeterBufferManager:
            CBX_IF_ERROR_RETURN(
                    REG_WRITE_CB_BMU_MTR_CONTROLr(unit, &meter_control));
            break;

        case cbxMeterTrafficType:
            CBX_IF_ERROR_RETURN(
                    REG_WRITE_CB_IPP_MTR_CONTROLr(unit, &meter_control));
            break;

        case cbxMeterTrapGroup:
            CBX_IF_ERROR_RETURN(
                    REG_WRITE_CB_PQM_MTR_CONTROLr(unit, &meter_control));
            break;
        case cbxMeterCFP:
#ifdef CONFIG_EXTERNAL_HOST
            CBX_IF_ERROR_RETURN(
                    REG_WRITE_CB_CFP_MTR_CONTROLr(unit, &meter_control));
#else
            return CBX_E_PARAM;
#endif /* CONFIG_EXTERNAL_HOST */
            break;
    }
    return CBX_E_NONE;
}

/*
 * Function:
 *      cbxi_meter_pm_op
 * Purpose:
 *      Enable/Disable/check performance monitoring for IPP/BMU/PQM/CFP meter
 *      index passed
 */
int
cbxi_meter_pm_op(int unit, cbx_meter_type_t type,
                 uint32 meter_index, int flag) {
    uint32 meter_control = 0;
    uint32 fval = 0;
    uint32 index = 0;
    /* Based on type read MTR control register */
    switch(type) {
        case cbxMeterBufferManager:
            CBX_IF_ERROR_RETURN(
                   REG_READ_CB_BMU_MTR_CONTROLr(unit, &meter_control));
            break;

        case cbxMeterTrafficType:
            CBX_IF_ERROR_RETURN(
                   REG_READ_CB_IPP_MTR_CONTROLr(unit, &meter_control));
            break;

        case cbxMeterTrapGroup:
            CBX_IF_ERROR_RETURN(
                   REG_READ_CB_PQM_MTR_CONTROLr(unit, &meter_control));
            break;
        case cbxMeterCFP:
#ifdef CONFIG_EXTERNAL_HOST
            CBX_IF_ERROR_RETURN(
                   REG_READ_CB_CFP_MTR_CONTROLr(unit, &meter_control));
#else
            return CBX_E_PARAM;
#endif /* CONFIG_EXTERNAL_HOST */
            break;
        default:
            return CBX_E_PARAM;
            break;
    }
    if (flag == CBXI_METER_PM_CHECK) {
        CBX_IF_ERROR_RETURN(
             soc_CB_BMU_MTR_CONTROLr_field_get(unit, &meter_control,
                                               PMON_ENf, &fval));
        CBX_IF_ERROR_RETURN(
             soc_CB_BMU_MTR_CONTROLr_field_get(unit, &meter_control,
                                               PMON_SELf, &index));
        if ((0 == fval) || (meter_index != index)) {
            return CBX_E_PARAM;
        }
    } else if (flag == CBXI_METER_PM_ENABLE) {
        fval = 1;
    } else {
        meter_index = 0;
    }
    /* enable PM for the meter index passed */
    CBX_IF_ERROR_RETURN(soc_CB_BMU_MTR_CONTROLr_field_set(unit, &meter_control,
                                       PMON_ENf, &fval));
    CBX_IF_ERROR_RETURN(soc_CB_BMU_MTR_CONTROLr_field_set(unit, &meter_control,
                                       PMON_SELf, &meter_index));
    /* Write updated MTR_CONTROL to HW */
    switch(type) {
        case cbxMeterBufferManager:
            CBX_IF_ERROR_RETURN(
                    REG_WRITE_CB_BMU_MTR_CONTROLr(unit, &meter_control));
            break;

        case cbxMeterTrafficType:
            CBX_IF_ERROR_RETURN(
                    REG_WRITE_CB_IPP_MTR_CONTROLr(unit, &meter_control));
            break;

        case cbxMeterTrapGroup:
            CBX_IF_ERROR_RETURN(
                    REG_WRITE_CB_PQM_MTR_CONTROLr(unit, &meter_control));
            break;
        case cbxMeterCFP:
#ifdef CONFIG_EXTERNAL_HOST
            CBX_IF_ERROR_RETURN(
                    REG_WRITE_CB_CFP_MTR_CONTROLr(unit, &meter_control));
#else
            return CBX_E_PARAM;
#endif /* CONFIG_EXTERNAL_HOST */
            break;
    }
    return CBX_E_NONE;
}

/*
 * Function:
 *      cbxi_meter_read_pm_reg
 * Purpose:
 *      Retrieve PM counter value at the index passed for IPP/BMU/PQM/CFP
 */
int
cbxi_meter_read_pm_reg(int unit, cbx_meter_type_t type,
                       int index, bmu_pmon_t *counter) {
    /* Based on type read PM counter at the index specified */
    switch(type) {
        case cbxMeterBufferManager:
            CBX_IF_ERROR_RETURN(soc_robo2_bmu_pmon_get(unit, index, counter));
            break;

        case cbxMeterTrafficType:
            CBX_IF_ERROR_RETURN(
                soc_robo2_ipp_pmon_get(unit, index, (ipp_pmon_t *)counter));
            break;

        case cbxMeterTrapGroup:
            CBX_IF_ERROR_RETURN(
                soc_robo2_pqm_pmon_get(unit, index, (pqm_pmon_t *)counter));
            break;
        case cbxMeterCFP:
#ifdef CONFIG_EXTERNAL_HOST
            CBX_IF_ERROR_RETURN(
                soc_robo2_cfp_pmon_get(unit, index, (cfp_pmon_t *)counter));
#else
            return CBX_E_PARAM;
#endif /* CONFIG_EXTERNAL_HOST */
            break;
        default:
            return CBX_E_PARAM;
            break;
    }
    return CBX_E_NONE;
}

/*
 * Function:
 *      cbxi_meter_pm_counter_pair_get
 * Purpose:
 *      Retrieve PM packet and byte counter values for IPP/BMU/PQM/CFP
 *      for the counter type passed
 */
int
cbxi_meter_pm_counter_pair_get(int unit, cbx_meter_type_t type,
                               int index, cbx_meter_pm_counter_t *pm_counter) {
    bmu_pmon_t  counter;
    int         max_index;

    max_index = soc_robo2_bmu_pmon_max_index(unit);
    if ((index + 1) > max_index) {
        return CBX_E_PARAM;
    }
    CBX_IF_ERROR_RETURN(cbxi_meter_read_pm_reg(unit, type, index, &counter));
    COMPILER_64_SET(pm_counter->packet, counter.count_msbs, counter.count_lsbs);
    CBX_IF_ERROR_RETURN(
              cbxi_meter_read_pm_reg(unit, type, index + 1, &counter));
    COMPILER_64_SET(pm_counter->byte, counter.count_msbs, counter.count_lsbs);
    return CBX_E_NONE;
}

/*
 * Function:
 *      cbxi_meter_pm_get
 * Purpose:
 *      Retrieve PM packet and byte counter values for IPP/BMU/PQM/CFP meter
 *      index passed
 */
int
cbxi_meter_pm_get(int unit, cbx_meter_type_t type,
                 cbx_meter_pm_params_t *pm_params) {


    /*  Get packet and byte count of frames which arrive with DP=0  */
    CBX_IF_ERROR_RETURN(
           cbxi_meter_pm_counter_pair_get(unit, type,
                                          CBXI_METER_PM_GREEN_CTR_INDEX,
                                          &pm_params->cbx_meter_pm_green));

    /*  Get packet and byte count of frames which arrive with DP=1 */
    CBX_IF_ERROR_RETURN(
           cbxi_meter_pm_counter_pair_get(unit, type,
                                          CBXI_METER_PM_YELLOW_CTR_INDEX,
                                          &pm_params->cbx_meter_pm_yellow));

    /*  Get packet and byte count of frames which arrive with DP=2 */
    CBX_IF_ERROR_RETURN(
           cbxi_meter_pm_counter_pair_get(unit, type,
                                          CBXI_METER_PM_RED_CTR_INDEX,
                                          &pm_params->cbx_meter_pm_red));

    /*  Get packet and byte count of  frames which arrive with DP=3 */
    CBX_IF_ERROR_RETURN(
           cbxi_meter_pm_counter_pair_get(unit, type,
                                         CBXI_METER_PM_RESERVED_CTR_INDEX,
                                         &pm_params->cbx_meter_pm_reserved));

    /*  Get packet and byte count of frames which arrive with DP=0
         and meter marks frame as DP=1 */
    CBX_IF_ERROR_RETURN(
           cbxi_meter_pm_counter_pair_get(unit, type,
                                          CBXI_METER_PM_G2Y_CTR_INDEX,
                                          &pm_params->cbx_meter_pm_g2y));

    /*  Get packet and byte count of frames which arrive with DP=0
         and meter marks frame as DP=2 */
    CBX_IF_ERROR_RETURN(
           cbxi_meter_pm_counter_pair_get(unit, type,
                                          CBXI_METER_PM_G2R_CTR_INDEX,
                                          &pm_params->cbx_meter_pm_g2r));

    /*  Get packet and byte count of frames which arrive with DP=1
         and meter marks frame as DP=2  */
    CBX_IF_ERROR_RETURN(
           cbxi_meter_pm_counter_pair_get(unit, type,
                                          CBXI_METER_PM_Y2R_CTR_INDEX,
                                          &pm_params->cbx_meter_pm_y2r));

    /*  Get packet and byte count of frames which arrive with DP=0
         and meter marks frame as DP=3 */
    CBX_IF_ERROR_RETURN(
           cbxi_meter_pm_counter_pair_get(unit, type,
                                          CBXI_METER_PM_G2D_CTR_INDEX,
                                          &pm_params->cbx_meter_pm_g2d));

    /*  Get packet and byte count of frames which arrive with DP=1
         and meter marks frame as DP=3 */
    CBX_IF_ERROR_RETURN(
           cbxi_meter_pm_counter_pair_get(unit, type,
                                          CBXI_METER_PM_Y2D_CTR_INDEX,
                                          &pm_params->cbx_meter_pm_y2d));

    /*  Get packet and byte count of frames which arrive with DP=2
         and meter marks frame as DP=3 */
    CBX_IF_ERROR_RETURN(
           cbxi_meter_pm_counter_pair_get(unit, type,
                                          CBXI_METER_PM_R2D_CTR_INDEX,
                                          &pm_params->cbx_meter_pm_r2d));

    /*  Get packet and byte count of frames which arrive with DP=1
         and meter marks frame as DP=0  */
    CBX_IF_ERROR_RETURN(
           cbxi_meter_pm_counter_pair_get(unit, type,
                                          CBXI_METER_PM_Y2G_CTR_INDEX,
                                          &pm_params->cbx_meter_pm_y2g));

    /*  Get packet and byte count of frames which arrive with DP=2
         and meter marks frame as DP=0  */
    CBX_IF_ERROR_RETURN(
           cbxi_meter_pm_counter_pair_get(unit, type,
                                          CBXI_METER_PM_R2G_CTR_INDEX,
                                          &pm_params->cbx_meter_pm_r2g));

    /*  Get packet and byte count of frames which arrive with DP=2
         and meter marks frame as DP=1  */
    CBX_IF_ERROR_RETURN(
           cbxi_meter_pm_counter_pair_get(unit, type,
                                          CBXI_METER_PM_R2Y_CTR_INDEX,
                                          &pm_params->cbx_meter_pm_r2y));

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_meter_table_init
 * Purpose:
 *  Global Initialization for all meter related tables.
 *
 */
int
cbxi_meter_table_init() {

    uint32        num_units = 1;
    int           i = 0;

    /* Get whether we are in a cascade set-up or not */
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = 2;
    }
    for (i = 0; i < num_units; i++) {

        /* Enable and reset BMU rscale table */
        CBX_IF_ERROR_RETURN(soc_robo2_bmu_rscale_enable(i));
        CBX_IF_ERROR_RETURN(soc_robo2_bmu_rscale_reset(i));

        /* Enable and reset BMU profile table */
        CBX_IF_ERROR_RETURN(soc_robo2_bmu_profile_enable(i));
        CBX_IF_ERROR_RETURN(soc_robo2_bmu_profile_reset(i));

        /* Enable and reset BMU profile pointer table */
        CBX_IF_ERROR_RETURN(soc_robo2_bmu_pptr_enable(i));
        CBX_IF_ERROR_RETURN(soc_robo2_bmu_pptr_reset(i));

        /* Enable and reset BMU performance monitor table */
        CBX_IF_ERROR_RETURN(soc_robo2_bmu_pmon_enable(i));
        CBX_IF_ERROR_RETURN(soc_robo2_bmu_pmon_reset(i));

        /* Enable and reset BMU bucket table */
        CBX_IF_ERROR_RETURN(soc_robo2_bmu_bucket_enable(i));
        CBX_IF_ERROR_RETURN(soc_robo2_bmu_bucket_reset(i));

        /* Enable and reset BMU meter to tc table */
        CBX_IF_ERROR_RETURN(soc_robo2_mtr2tcb_enable(i));
        CBX_IF_ERROR_RETURN(soc_robo2_mtr2tcb_reset(i));

        /* Enable and reset IPP rscale table */
        CBX_IF_ERROR_RETURN(soc_robo2_ipp_rscale_enable(i));
        CBX_IF_ERROR_RETURN(soc_robo2_ipp_rscale_reset(i));

        /* Enable and reset IPP profile table */
        CBX_IF_ERROR_RETURN(soc_robo2_ipp_profile_enable(i));
        CBX_IF_ERROR_RETURN(soc_robo2_ipp_profile_reset(i));

        /* Enable and reset IPP profile pointer table */
        CBX_IF_ERROR_RETURN(soc_robo2_ipp_pptr_enable(i));
        CBX_IF_ERROR_RETURN(soc_robo2_ipp_pptr_reset(i));

        /* Enable and reset IPP performance monitor table */
        CBX_IF_ERROR_RETURN(soc_robo2_ipp_pmon_enable(i));
        CBX_IF_ERROR_RETURN(soc_robo2_ipp_pmon_reset(i));

        /* Enable and reset IPP bucket table */
        CBX_IF_ERROR_RETURN(soc_robo2_ipp_bucket_enable(i));
        CBX_IF_ERROR_RETURN(soc_robo2_ipp_bucket_reset(i));

        /* Enable and reset PQM rscale table */
        CBX_IF_ERROR_RETURN(soc_robo2_pqm_rscale_enable(i));
        CBX_IF_ERROR_RETURN(soc_robo2_pqm_rscale_reset(i));

        /* Enable and reset PQM profile table */
        CBX_IF_ERROR_RETURN(soc_robo2_pqm_profile_enable(i));
        CBX_IF_ERROR_RETURN(soc_robo2_pqm_profile_reset(i));

        /* Enable and reset PQM profile pointer table */
        CBX_IF_ERROR_RETURN(soc_robo2_pqm_pptr_enable(i));
        CBX_IF_ERROR_RETURN(soc_robo2_pqm_pptr_reset(i));

        /* Enable and reset PQM performance monitor table */
        CBX_IF_ERROR_RETURN(soc_robo2_pqm_pmon_enable(i));
        CBX_IF_ERROR_RETURN(soc_robo2_pqm_pmon_reset(i));

        /* Enable and reset PQM bucket table */
        CBX_IF_ERROR_RETURN(soc_robo2_pqm_bucket_enable(i));
        CBX_IF_ERROR_RETURN(soc_robo2_pqm_bucket_reset(i));

#ifdef CONFIG_EXTERNAL_HOST
        /* Enable and reset PQM rscale table */
        CBX_IF_ERROR_RETURN(soc_robo2_cfp_rscale_enable(i));
        CBX_IF_ERROR_RETURN(soc_robo2_cfp_rscale_reset(i));

        /* Enable and reset PQM profile table */
        CBX_IF_ERROR_RETURN(soc_robo2_cfp_profile_enable(i));
        CBX_IF_ERROR_RETURN(soc_robo2_cfp_profile_reset(i));

        /* Enable and reset PQM profile pointer table */
        CBX_IF_ERROR_RETURN(soc_robo2_cfp_pptr_enable(i));
        CBX_IF_ERROR_RETURN(soc_robo2_cfp_pptr_reset(i));

        /* Enable and reset PQM performance monitor table */
        CBX_IF_ERROR_RETURN(soc_robo2_cfp_pmon_enable(i));
        CBX_IF_ERROR_RETURN(soc_robo2_cfp_pmon_reset(i));

        /* Enable and reset PQM bucket table */
        CBX_IF_ERROR_RETURN(soc_robo2_cfp_bucket_enable(i));
        CBX_IF_ERROR_RETURN(soc_robo2_cfp_bucket_reset(i));
#endif /* CONFIG_EXTERNAL_HOST */
    }
    return CBX_E_NONE;
}

#ifdef CONFIG_EXTERNAL_HOST
/*
 * Function:
 *  cbxi_meter_cfp_index_management_init
 * Purpose:
 *  Initialize CFP meter index allocation mechanism
 */
int
cbxi_meter_cfp_index_management_init() {
    int     unit = CBX_AVENGER_PRIMARY;
    int     max_index;
    int     num_units = 1;
    int     i = 0;
    /* Get whether we are in a cascade set-up or not */
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = 2;
    }
    max_index =  soc_robo2_bmu_pptr_max_index(unit);

    for (i = 0; i < num_units; i++) {
        /*  create the list */
        CBX_IF_ERROR_RETURN(shr_aidxres_list_create(
                 &cfp_meter_list_handle[i],
                 0, max_index, 0, max_index,
                 8, "cfp_meter"));
        /* Reserve meter index 0 as it is not used */
        CBX_IF_ERROR_RETURN(shr_aidxres_list_reserve_block(
                         cfp_meter_list_handle[i], 0, 1));
    }
    return CBX_E_NONE;
}
#endif /* CONFIG_EXTERNAL_HOST */


/*
 * Function:
 *      cbx_meter_profile_params_t_init
 * Purpose:
 *      Initialize meter profile entry parameters.
 */

void
cbxi_meter_profile_params_t_init(cbx_meter_profile_params_t *meter_params) {
    if ( NULL != meter_params ) {
        sal_memset(meter_params, 0, sizeof (cbx_meter_profile_params_t));
    }
    return;
}


/*
 * Function:
 *      cbx_meter_params_t_init
 * Purpose:
 *      Initialize Port entry parameters.
 */

void
cbxi_meter_params_t_init(cbx_meter_params_t *meter_params) {
    if ( NULL != meter_params ) {
        sal_memset(meter_params, 0, sizeof (cbx_meter_params_t));
    }
    return;
}

/*
 * Function:
 *      cbx_meter_params_t_init
 * Purpose:
 *      Initialize Port entry parameters.
 */

void
cbxi_meter_pm_params_t_init(cbx_meter_pm_params_t *pm_params) {
    if ( NULL != pm_params ) {
        sal_memset(pm_params, 0, sizeof (cbx_meter_pm_params_t));
    }
    return;
}

/*
 * Function:
 *      cbxi_meter_id_create
 * Purpose:
 *      Given meter type and index create meter id
 */
int
cbxi_meter_id_create(int unit, int index, cbx_meter_type_t type,
                     cbx_meterid_t *meterid) {

    cfp_pptr_t       pptr_entry;
    *meterid = index;
    *meterid |= type << CBXI_METER_TYPE_SHIFT;
    *meterid |= unit << CBXI_METER_UNIT_SHIFT;
    *meterid |= CBXI_METER_INDEX_VALID;
    CBX_IF_ERROR_RETURN(soc_robo2_cfp_pptr_get(unit, index,
                                        &pptr_entry));
#ifdef CONFIG_EXTERNAL_HOST
    *meterid |= ((cfp_meter_profile[pptr_entry.pointer].mode) <<
                  CBXI_METER_MODE_SHIFT);
    *meterid |=  (pptr_entry.pointer) << CBXI_METER_PROFILE_INDEX_SHIFT;
#endif
    return CBX_E_NONE;
}

/**************************************************************************
 *                 METER FSAL API IMPLEMENTATION                           *
 **************************************************************************/

/*
 * Function:
 *      cbx_meter_init
 * Purpose:
 *      Initialize meter module
 * Parameters:
 *      None.
 * Returns:
 *      CBX_E_NONE - success
 *      CBX_E_XXX -  Error
 */

int
cbx_meter_init(void) {
    int       rv = CBX_E_NONE;

    LOG_INFO(BSL_LS_FSAL_METER,
            (BSL_META("FSAL API : cbx_meter_init()..\n")));

    /* Enable all the tables related to meter module */
    rv = cbxi_meter_table_init();
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_METER,
            (BSL_META("FSAL API : cbx_meter_init()..Table init failed\n")));
        return rv;
    }
    /* Set pkt_mode_len field of  MTR_CONTROL register */
    rv = cbxi_meter_mtr_control_set();
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_METER,
            (BSL_META("FSAL API : cbx_meter_init()..Failed to set \
                        MTR control \n")));
        return rv;
    }
    /* Set meter id selection criteria for BMU meters  */
    rv = cbxi_meter_bmu_config();
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_METER,
            (BSL_META("FSAL API : cbx_meter_init()..Failed to set \
                        MTR config for BMU meters\n")));
        return rv;
    }
    /* Create default meter profile */
    rv = cbxi_meter_default_profile_create();
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_METER,
            (BSL_META("FSAL API : cbxi_meter_init()..Failed to \
                        create default meter profiles\n")));
        return rv;
    }
#ifdef CONFIG_EXTERNAL_HOST
     /* Index management CFP meter */
    rv = cbxi_meter_cfp_index_management_init();
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_METER,
            (BSL_META("FSAL API : cbxi_meter_init()..Failed to \
                   create meter index allocation list for CFP meters\n")));
        return rv;
    }

#endif /* CONFIG_EXTERNAL_HOST */

    return rv;
}

/**
 * Function :
 *    cbx_meter_profile_create -  Create a meter profile
 * Purpose  :
 *   This routine is used to create a meter profile.
 *
 * Parameters:
 *      meter_params  (IN)  Meter parameters.
 *      profileid     (OUT) Handle of the meter created
 *                         CBX_METER_INVALID: Meter could not be created
 *
 * Returns:
 *      CBX_E_NONE Success
 *      CBX_E_XXXX Failure
 */
int
cbx_meter_profile_create(cbx_meter_profile_params_t *meter_params,
                         cbx_meter_profile_t        *profileid) {

    int rv = CBX_E_NONE;
    int flag = CBXI_METER_PROFILE_CREATE;

    *profileid = CBX_METER_INVALID;
    /* Create new profile or reuse an existing profile matching this */
    rv = cbxi_meter_profile_set(meter_params, profileid, flag);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_METER,
            (BSL_META("FSAL API : cbx_meter_profile_create().. \
                                   failed to create the profile\n")));
        return rv;
    }
    /* create profile id - MMTTXXXX MM mdoe TT type XXXX profile index  */
    *profileid |= ((meter_params->mode & 0xf) << CBXI_METER_MODE_SHIFT);
    *profileid |= ((meter_params->type & 0xf) << CBXI_METER_TYPE_SHIFT);
    return rv;
}

/**
 * Function :
 *     cbx_meter_profile_destroy
 * Purpose  :
 *     Destroy a meter previously created by cbx_meter_create()
 *
 * Parameters:
 *     profileid (IN)  Handle of the profile to be destroyed
 *
 * Returns:
 *      CBX_E_NONE Success
 *      CBX_E_XXXX Failure
 *
 */

int
cbx_meter_profile_destroy(cbx_meter_profile_t profileid) {

    int     unit = CBX_AVENGER_PRIMARY;
    int     rv = CBX_E_NONE;
    cbx_meter_type_t type;
    bmu_profile_t    profile_entry;
    int              profile_index;
    cbxi_meter_info_t  *profile_data = bmu_meter_profile;

    /* extract meter type and profile index from the profileid */
    type = (profileid & CBXI_METER_TYPE_MASK) >> CBXI_METER_TYPE_SHIFT;
    profile_index = (profileid & 0xFF);
    sal_memset(&profile_entry, 0, sizeof(bmu_profile_t));

    /* Get internal profile manaagement data structure */
    switch(type) {
        case cbxMeterBufferManager:
            profile_data = bmu_meter_profile;
            break;

        case cbxMeterTrafficType:
            profile_data = ipp_meter_profile;
            break;

        case cbxMeterTrapGroup:
            profile_data = pqm_meter_profile;
            break;
        case cbxMeterCFP:
#ifdef CONFIG_EXTERNAL_HOST
            profile_data = cfp_meter_profile;
#else
            return CBX_E_PARAM;
#endif /* CONFIG_EXTERNAL_HOST */
            break;
        default:
            LOG_ERROR(BSL_LS_FSAL_METER,
              (BSL_META("FSAL API : cbx_meter_profile_destroy().. \
                              Invalid profile id passed\n")));
            return CBX_E_PARAM;
            break;
    }
    if ((profile_data[profile_index].pid & CBXI_METER_PROFILE_DATA_VALID) == 0){
        LOG_ERROR(BSL_LS_FSAL_METER,
          (BSL_META("FSAL API : cbx_meter_profile_destroy().. \
                          Invalid profile id passed\n")));
        return CBX_E_PARAM;
    }
    /* Check profile usage count */
    if (profile_data[profile_index].pid & CBXI_METER_PROFILE_DATA_REF_COUNT) {
        LOG_ERROR(BSL_LS_FSAL_METER,
          (BSL_META("FSAL API : cbx_meter_profile_destroy().. \
                                        Profile is in use\n")));
        return CBX_E_PARAM;

    }
    /* Set profile entry in HW */
    rv = cbxi_meter_profile_entry_set(unit, type,
                                      profile_index, &profile_entry);
    if (CBX_FAILURE(rv)) {
       LOG_ERROR(BSL_LS_FSAL_METER,
            (BSL_META("FSAL API : cbx_meter_profile_destroy().. \
                              Invalid profile id passed\n")));
       return rv;
    }
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        unit = CBX_AVENGER_SECONDARY;
        rv = cbxi_meter_profile_entry_set(unit, type,
                                      profile_index, &profile_entry);
        if (CBX_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_FSAL_METER,
                (BSL_META("FSAL API : cbx_meter_profile_destroy().. \
                              Invalid profile id passed\n")));
            return rv;
        }
    }
    profile_data[profile_index].pid = 0;
    profile_data[profile_index].ckbits_sec = 0;
    profile_data[profile_index].ckbits_burst = 0;
    profile_data[profile_index].pkbits_sec = 0;
    profile_data[profile_index].pkbits_burst = 0;
    return CBX_E_NONE;
}

/**
 * Function :
 *     cbx_meter_profile_get
 * Purpose  :
 *     This routine is used to get a meter profile.
 *
 * Parameters:
 *      profileid     (IN)  Meter Profile Identifier
 *      meter_params  (OUT) Meter parameters.
 *
 * Returns:
 *      CBX_E_NONE Success
 *      CBX_E_XXXX Failure
 */

int
cbx_meter_profile_get (cbx_meter_profile_t         profileid,
                       cbx_meter_profile_params_t *meter_params) {
    int                rv = CBX_E_NONE;
    cbx_meter_type_t   type;
    cbx_meter_mode_t   mode;
    int                profile_index;

    cbxi_meter_info_t  *profile_data = bmu_meter_profile;
    int                max_index = 0;
    bmu_profile_t      profile_entry;

    cbxi_meter_profile_params_t_init(meter_params);

    /* Get meter type, mode and profile index from the profile id passed */
    type = (profileid & CBXI_METER_TYPE_MASK) >> CBXI_METER_TYPE_SHIFT;
    mode = (profileid & CBXI_METER_MODE_MASK) >> CBXI_METER_MODE_SHIFT;
    profile_index = (profileid & 0xFF);

    switch(type) {
        case cbxMeterBufferManager:
            profile_data = bmu_meter_profile;
            max_index = CBXI_MAX_BMU_METER_PROFILE;
            if (profile_index >= max_index) {
                return CBX_E_PARAM;
            }
            break;

        case cbxMeterTrafficType:
            profile_data = ipp_meter_profile;
            max_index = CBXI_MAX_IPP_METER_PROFILE;
            if (profile_index >= max_index) {
                return CBX_E_PARAM;
            }
            break;

        case cbxMeterTrapGroup:
            profile_data = pqm_meter_profile;
            max_index = CBXI_MAX_PQM_METER_PROFILE;
            if (profile_index >= max_index) {
                return CBX_E_PARAM;
            }
            break;
        case cbxMeterCFP:
#ifdef CONFIG_EXTERNAL_HOST
            profile_data = cfp_meter_profile;
            max_index = CBXI_MAX_CFP_METER_PROFILE;
            if (profile_index >= max_index) {
                return CBX_E_PARAM;
            }
#else
            return CBX_E_PARAM;
#endif /* CONFIG_EXTERNAL_HOST */
            break;
        default:
            LOG_ERROR(BSL_LS_FSAL_METER,
                (BSL_META("FSAL API : cbx_meter_profile_get().. \
                              Invalid profile id passed\n")));
            return CBX_E_PARAM;
            break;
    }
    /* Check whether it is a valid profile index */
    if (!(profile_data[profile_index].pid & CBXI_METER_PROFILE_DATA_VALID)) {
        LOG_ERROR(BSL_LS_FSAL_METER,
            (BSL_META("FSAL API : cbx_meter_profile_get().. \
                          Invalid profile id passed\n")));
        return CBX_E_PARAM;
    }
    /* read profile entry from HW */
    rv = cbxi_meter_profile_entry_get(CBX_AVENGER_PRIMARY, type,
                                      profile_index, &profile_entry);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_METER,
            (BSL_META("FSAL API : cbx_meter_profile_get().. \
                                 failed to read profile\n")));
        return rv;
    }
    /* fill in profile parameters */
    meter_params->mode = mode;
    meter_params->type = type;
    meter_params->ckbits_sec = profile_data[profile_index].ckbits_sec;
    meter_params->ckbits_burst = profile_data[profile_index].ckbits_burst;
    meter_params->pkbits_sec = profile_data[profile_index].pkbits_sec;
    meter_params->pkbits_burst = profile_data[profile_index].pkbits_burst;
    rv = cbxi_meter_profile_entry_to_param(&profile_entry, meter_params);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_METER,
            (BSL_META("FSAL API : cbx_meter_profile_get().. \
                 failed to convert HW profile entry to FSAL profile entry\n")));
        return rv;
    }
#if 0
    rv = cbx_cosq_flowcontrol_get(&fc);
    if ((rv == CBX_E_NONE) && (fc == cbxFlowcontrolMeter)) {
        meter_params->flags |= CBX_METER_SHAPING;
    }
#endif
    return rv;
}

/**
 * Function :
 *      cbx_meter_profile_set
 * Purpose  :
 *      This routine is used to set a meter profile.
 *
 * Parameters:
 *      profileid     (IN)  Meter Profile Identifier
 *      meter_params  (IN)  Meter parameters.
 *
 * Returns:
 *      CBX_E_NONE Success
 *      CBX_E_XXXX Failure
 */

int
cbx_meter_profile_set(cbx_meter_profile_t         profileid,
                       cbx_meter_profile_params_t *meter_params) {

    int              rv = CBX_E_NONE;
    int              flag = CBXI_METER_PROFILE_UPDATE;


    rv = cbxi_meter_profile_set(meter_params, &profileid, flag);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_METER,
            (BSL_META("FSAL API : cbx_meter_profile_create().. \
                                   failed to create the profile\n")));
        return rv;
    }
    return rv;
}

/**
 * Function :
 *      cbx_meter_create -  Create a meter
 * Purpose  :
 *      This routine is used to create a meter.
 *
 * Parameters:
 *      meter_params  (IN)  Meter parameters.
 *      meterid       (OUT) Handle of the meter created
 *                         CBX_METER_INVALID: Meter could not be created
 *
 * Returns:
 *      CBX_E_NONE Success
 *      CBX_E_XXXX Failure
 */

int
cbx_meter_create(cbx_meter_profile_t profileid,
                 cbx_meter_params_t *meter_params,
                 cbx_meterid_t      *meterid) {

    int unit = CBX_AVENGER_PRIMARY;
    int rv = CBX_E_NONE;
    cbx_port_t port_out;
    cbxi_pgid_t lpg;
    *meterid = CBX_METER_INVALID;
    cbx_meterid_t meterid1 = 0;
    cbx_meter_type_t type = 0;
    if ( NULL == meter_params ) {
        LOG_ERROR(BSL_LS_FSAL_METER,
            (BSL_META("FSAL API : cbx_meter_create().. \
                                  Invalid parameter\n")));
        return CBX_E_PARAM;
    }
    /* validate port */
    if ((CBX_PORTID_IS_TRUNK(meter_params->portid)) ||
        (CBX_PORTID_IS_VLAN_PORT(meter_params->portid))) {
        LOG_ERROR(BSL_LS_FSAL_METER,
            (BSL_META("FSAL API : cbx_meter_create()..Invalid  \
                               portid passed \n")));
        return CBX_E_PORT;
    }
    /* validate the port index passed */
    rv = cbxi_robo2_port_validate(
                        meter_params->portid, &port_out, &lpg, &unit);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_METER,
            (BSL_META("FSAL API : cbx_meter_create()..Invalid  \
                                  port index passed \n")));
        return CBX_E_PORT;
    }
    /* Create the meter using LPG */
    type = (profileid & CBXI_METER_TYPE_MASK) >> CBXI_METER_TYPE_SHIFT;
    if ((type == cbxMeterCFP) || (type == cbxMeterTrapGroup)) {
        unit = 0;
    }
    rv = cbxi_meter_op(unit, lpg, profileid,
                   meter_params, meterid, CBXI_METER_CREATE);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_METER,
            (BSL_META("FSAL API : cbx_meter_create()... \
                                   invalid parameter \n")));
        return CBX_E_PARAM;
    }
    /*  Meter Id format : MMMM TTTT PPPP PPPP UVXX XXXX XXXX XXXX
     *  TT meter type, PP profile index, XXXX meter index, U unit number
     *  V is valid bit
     */
    *meterid |= unit << CBXI_METER_UNIT_SHIFT;
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        if ((type == cbxMeterCFP) || (type == cbxMeterTrapGroup)) {
            rv = cbxi_meter_op(CBX_AVENGER_SECONDARY, 0, profileid,
                           meter_params, &meterid1, CBXI_METER_CREATE);
            if (CBX_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_FSAL_METER,
                    (BSL_META("FSAL API : cbx_meter_create()... \
                                       invalid parameter \n")));
                return CBX_E_PARAM;
            }
        }
    }
    return CBX_E_NONE;
}

/**
 * Function :
 *    cbx_meter_destroy
 * Purpose  :
 *    Destroy a meter previously created by cbx_meter_create()
 *
 * Parameters:
 *    meterid    (IN)  Handle of the meter to be destroyed
 *
 * Returns:
 *    CBX_E_NONE Success
 *    CBX_E_XXXX Failure
 *
 */
int
cbx_meter_destroy(cbx_meterid_t meterid) {

    int rv = CBX_E_NONE;
    cbx_meterid_t meterid1 = 0;
    cbx_meter_type_t type = 0;
    rv = cbxi_meter_op(0, 0, 0, NULL, &meterid, CBXI_METER_DESTROY);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_METER,
            (BSL_META("FSAL API : cbx_meter_destroy()... \
                                   invalid parameter \n")));
        return CBX_E_PARAM;
    }
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        type = (meterid & CBXI_METER_TYPE_MASK) >> CBXI_METER_TYPE_SHIFT;
        if ((type == cbxMeterCFP) || (type == cbxMeterTrapGroup)) {
            meterid1 = (meterid | (CBX_AVENGER_SECONDARY << CBXI_METER_UNIT_SHIFT));
            rv = cbxi_meter_op(CBX_AVENGER_SECONDARY, 0, 0,
                           NULL, &meterid1, CBXI_METER_DESTROY);
            if (CBX_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_FSAL_METER,
                    (BSL_META("FSAL API : cbx_meter_destroy()... \
                                       invalid parameter \n")));
                return CBX_E_PARAM;
            }
        }
    }
    return rv;
}

/**
 * Function :
 *      cbx_meter_pm_enable
 * Purpose  :
 *      Meter performance monitoring enable
 *
 * Parameters:
 *      meterid    (IN)  Handle of the meter to enable Performance Monitoring
 *
 * Returns:
 *      CBX_E_NONE Success
 *      CBX_E_XXXX Failure
 */
int
cbx_meter_pm_enable(cbx_meterid_t meterid) {

    int unit = CBX_AVENGER_PRIMARY;
    int rv = CBX_E_NONE;
    cbx_meter_type_t type;
    uint32  meter_index = 0;
    int     valid = 0;

    /* Get meter type,  meter index and unit from the meter id passed */
    valid = ((meterid & CBXI_METER_INDEX_VALID)
                                      >> CBXI_METER_INDEX_VALID_SHIFT);
    if (valid == 0) {
        return CBX_E_PARAM;
    }
    type = (meterid & CBXI_METER_TYPE_MASK) >> CBXI_METER_TYPE_SHIFT;
    meter_index = meterid & CBXI_METER_INDEX_MASK;
    unit = ((meterid & CBXI_METER_UNIT_MASK) >> CBXI_METER_UNIT_SHIFT);

    /* validate meter index */
    rv = cbxi_meter_index_validate(unit, type, meter_index);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_METER,
            (BSL_META("FSAL API : cbx_meter_pm_enable()... \
                                   invalid meter index \n")));
        return CBX_E_PARAM;
    }
    /* Enable PM for the meter */
    rv = cbxi_meter_pm_op(unit, type, meter_index, CBXI_METER_PM_ENABLE);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_METER,
            (BSL_META("FSAL API : cbx_meter_pm_enable()... \
                                   invalid parameter \n")));
        return CBX_E_PARAM;
    }
    return rv;
}

/**
 * Function :
 *      cbx_meter_pm_disable
 * Purpose  :
 *      Meter Performance monitoring disable
 *
 * Parameters:
 *      meterid    (IN)  Handle of the meter to disable Performance Monitoring
 *
 * Returns:
 *      CBX_E_NONE Success
 *      CBX_E_XXXX Failure
 */
int
cbx_meter_pm_disable(cbx_meterid_t meterid) {

    int unit = CBX_AVENGER_PRIMARY;
    int rv = CBX_E_NONE;
    cbx_meter_type_t type;
    uint32  meter_index;
    int     valid = 0;
    /* Get meter type,  meter index and unit from the meter id passed */
    valid = ((meterid & CBXI_METER_INDEX_VALID)
                                      >> CBXI_METER_INDEX_VALID_SHIFT);
    if (valid == 0) {
        return CBX_E_PARAM;
    }
    type = (meterid & CBXI_METER_TYPE_MASK) >> CBXI_METER_TYPE_SHIFT;
    meter_index= meterid & CBXI_METER_INDEX_MASK;
    unit = ((meterid & CBXI_METER_UNIT_MASK) >> CBXI_METER_UNIT_SHIFT);

    /* validate meter index */
    rv = cbxi_meter_index_validate(unit, type, meter_index);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_METER,
            (BSL_META("FSAL API : cbx_meter_pm_enable()... \
                                   invalid meter index \n")));
        return CBX_E_PARAM;
    }

    /* Disable PM for the meter */
    rv = cbxi_meter_pm_op(unit, type, meter_index, CBXI_METER_PM_DISABLE);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_METER,
            (BSL_META("FSAL API : cbx_meter_pm_disable()... \
                                   invalid parameter \n")));
        return CBX_E_PARAM;
    }
    return rv;
}

/**
 * Function :
 *      cbx_meter_pm_get
 * Purpose  :
 *      Meter Performance monitoring get counters
 *
 * Parameters:
 *      meterid      (IN)  Handle of the meter with Performance Monitoring
 *      pm_params_t  (OUT) Performance Monitoring parameters
 *
 * Returns:
 *      CBX_E_NONE Success
 *      CBX_E_XXXX Failure
 */
int
cbx_meter_pm_get(cbx_meterid_t          meterid,
                 cbx_meter_pm_params_t *pm_params) {

    int unit = CBX_AVENGER_PRIMARY;
    int rv = CBX_E_NONE;
    int meter_index = 0;
    cbx_meter_type_t type;
    int     valid = 0;
    cbxi_meter_pm_params_t_init(pm_params);

    /* Get meter type,  meter index and unit from the meter id passed */
    valid = ((meterid & CBXI_METER_INDEX_VALID)
                                      >> CBXI_METER_INDEX_VALID_SHIFT);
    if (valid == 0) {
        return CBX_E_PARAM;
    }
    type = (meterid & CBXI_METER_TYPE_MASK) >> CBXI_METER_TYPE_SHIFT;
    meter_index= meterid & CBXI_METER_INDEX_MASK;
    unit = ((meterid & CBXI_METER_UNIT_MASK) >> CBXI_METER_UNIT_SHIFT);

    /* validate meter index */
    rv = cbxi_meter_index_validate(unit, type, meter_index);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_METER,
            (BSL_META("FSAL API : cbx_meter_pm_enable()... \
                                   invalid meter index \n")));
        return CBX_E_PARAM;
    }
    /* Check whether PM is enabled for this meter index */
    rv = cbxi_meter_pm_op(unit, type, meter_index, CBXI_METER_PM_CHECK);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_METER,
            (BSL_META("FSAL API : cbx_meter_pm_get().. \
                                  Invalid parameter\n")));
        return CBX_E_PARAM;
    }
    /* Read PM counters for the meter */
    rv = cbxi_meter_pm_get(unit, type, pm_params);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_METER,
            (BSL_META("FSAL API : cbx_meter_pm_get().. \
                                  Invalid parameter\n")));
        return CBX_E_PARAM;

    }
    return CBX_E_NONE;
}

/**
 * Function :
 *      cbx_meter_control_set
 * Purpose  :
 *      Meter control type set
 *
 * Parameters:
 *      meter_ctrl   (IN)  Meter control type
 *
 * Returns:
 *      CBX_E_NONE Success
 *      CBX_E_XXXX Failure
 */
int
cbx_meter_control_set(cbx_meter_control_t meter_ctrl) {

    cbx_flowcontrol_t fc;
    uint32         meter_config;
    uint32         fval;

    if (meter_ctrl == cbx_meter_ctrl) {
        return CBX_E_NONE;
    }
    CBX_IF_ERROR_RETURN(cbx_cosq_flowcontrol_get(&fc));
    if (fc != cbxFlowcontrolNone) {
        LOG_ERROR(BSL_LS_FSAL_METER,
            (BSL_META("FSAL API : cbx_meter_control_set()... \
            Not allowed when flowcontrol mode is not cbxFlowcontrolNone \n")));
        return CBX_E_FAIL;
    }
    CBX_IF_ERROR_RETURN(
        REG_READ_CB_BMU_MTR_CONFIGr(CBX_AVENGER_PRIMARY, &meter_config));
    switch (meter_ctrl) {
        case cbxMeterControlPortTC:
            fval = 1;
            soc_CB_BMU_MTR_CONFIGr_field_set(CBX_AVENGER_PRIMARY, &meter_config,
                                             METER_ID_SELf, &fval);
        break;
        case cbxMeterControlPort:
            fval = 2;
            soc_CB_BMU_MTR_CONFIGr_field_set(CBX_AVENGER_PRIMARY, &meter_config,
                                             METER_ID_SELf, &fval);
        break;
        default:
            return CBX_E_NONE;
        break;
    }
    CBX_IF_ERROR_RETURN(
              REG_WRITE_CB_BMU_MTR_CONFIGr(CBX_AVENGER_PRIMARY, &meter_config));
    cbx_meter_ctrl = meter_ctrl;
    return CBX_E_NONE;
}

/**
 * Function :
 *      cbx_meter_control_get
 * Purpose  :
 *      Meter control type get
 *
 * Parameters:
 *      meter_ctrl   (IN/OUT)  Meter control type
 *
 * Returns:
 *      CBX_E_NONE Success
 *      CBX_E_XXXX Failure
 */
int
cbx_meter_control_get(cbx_meter_control_t *meter_ctrl) {
    if (meter_ctrl == NULL) {
        return CBX_E_PARAM;
    }
    *meter_ctrl = cbx_meter_ctrl;
    return CBX_E_NONE;
}
