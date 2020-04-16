/*
 * * $ID: $
 * * 
 * * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 * * 
 * * $Copyright: (c) 2020 Broadcom Inc.
 * * All Rights Reserved$
 * *
 * * File:
 * *     mirror.c
 * * Purpose:
 * *     Robo2 mirror module.
 * *
 * */
#include <bsl_log/bsl.h>
#include <soc/robo2/common/tables.h>
#include <soc/robo2/common/memregs.h>
#include <fsal/error.h>
#include <shared/types.h>
#include <fsal/mirror.h>
#include <fsal/vlan.h>
#include <fsal_int/mirror.h>
#include <fsal_int/port.h>
#include <fsal_int/vlan.h>
#include <fsal_int/lin.h>

void cbxi_mirror_dest_info_dump (void);

static cbx_mirror_cntl_t      cbx_robo2_mirror_control;
static cbx_mirror_vmt_info_t  ivmt[CBXI_VMT_MAX];
static cbx_mirror_vmt_info_t  evmt[CBXI_VMT_MAX];

#define MIRROR_CNTL       cbx_robo2_mirror_control
#define MIRROR_INFO(mid)  cbx_robo2_mirror_control.mirror_info[mid]
#define MIRROR_DEST_INFO(mdid)  cbx_robo2_mirror_control.mirror_dest[mdid]

/*
 * Cause a routine to return CBX_E_INIT if Mirror subsystem is not initialized.
 */

#define MIRROR_INIT    \
          if (!MIRROR_CNTL.init) {return CBX_E_INIT;}

/*
 * Make sure mirror destid is within valid range.
 */

#define MIRROR_DESTID_CHECK(mdid) \
    if ((mdid) >= CBX_MIRROR_DEST_MAX) { \
        return CBX_E_PARAM; }
/*
 * Make sure mirror id is within valid range.
 */

#define MIRROR_ID_CHECK(mdid) \
    if ((mdid) >= CBX_MIRROR_MAX) { \
        return CBX_E_PARAM; }

/*
 * Function:
 *  cbxi_robo2_mirror_create_flags_validate
 * Purpose:
 *  Check if mirror create flag is valid.
 *
 */
int cbxi_robo2_mirror_create_flags_validate( uint32_t mirror_flags )
{

    /* Only one type of mirror group is permitted  */
    if (mirror_flags & CBX_MIRROR_MODE_PORT_INGRESS ) {

        /*  mirror group is port ingress others not permitted  */
        if ( (mirror_flags & CBX_MIRROR_MODE_PORT_EGRESS ) ||
                (mirror_flags & CBX_MIRROR_MODE_VLAN_INGRESS ) ||
                (mirror_flags & CBX_MIRROR_MODE_VLAN_EGRESS )) {
            return CBX_E_PARAM;
        }
    } else if (mirror_flags & CBX_MIRROR_MODE_PORT_EGRESS ) {

        /*  mirror group is port egress others not permitted  */
        if ( (mirror_flags & CBX_MIRROR_MODE_PORT_INGRESS ) ||
                (mirror_flags & CBX_MIRROR_MODE_VLAN_INGRESS ) ||
                (mirror_flags & CBX_MIRROR_MODE_VLAN_EGRESS )) {
            return CBX_E_PARAM;
        }
    } else if (mirror_flags & CBX_MIRROR_MODE_VLAN_INGRESS ) {

        /*  mirror group is vlan ingress others not permitted  */
        if ( (mirror_flags & CBX_MIRROR_MODE_VLAN_EGRESS ) ||
                (mirror_flags & CBX_MIRROR_MODE_PORT_INGRESS ) ||
                (mirror_flags & CBX_MIRROR_MODE_PORT_EGRESS )) {
            return CBX_E_PARAM;
        }
    } else if (mirror_flags & CBX_MIRROR_MODE_VLAN_EGRESS ) {

        /*  mirror group is vlan egress others not permitted  */
        if ( (mirror_flags & CBX_MIRROR_MODE_VLAN_INGRESS ) ||
                (mirror_flags & CBX_MIRROR_MODE_PORT_INGRESS ) ||
                (mirror_flags & CBX_MIRROR_MODE_PORT_EGRESS )) {
            return CBX_E_PARAM;
        }
    } else {
        return CBX_E_PARAM;
    }

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_robo2_mirror_port_mode_validate
 * Purpose:
 *  Check if mirror port mode flag is valid.
 *
 */
int cbxi_robo2_mirror_port_mode_validate( uint32_t port_mode,
        uint32_t mirror_flags)
{
    int rv = CBX_E_NONE;

    /* validate port mode and mirror group mode */
    switch (port_mode) {
    case cbxMirrorModeIngress:
        if ( !(mirror_flags & CBX_MIRROR_MODE_PORT_INGRESS )) {
            rv =  CBX_E_PARAM;
        }
        break;
    case cbxMirrorModeEgress:
        if ( !(mirror_flags & CBX_MIRROR_MODE_PORT_EGRESS )) {
            rv =  CBX_E_PARAM;
        }
        break;
    default:
        rv =  CBX_E_PARAM;
        break;
    }

    return rv;
}

/*
 * Function:
 *  cbxi_robo2_mirror_vlan_mode_validate
 * Purpose:
 *  Check if mirror vlan mode flag is valid.
 *
 */
int cbxi_robo2_mirror_vlan_mode_validate( uint32_t vlan_mode,
        uint32_t mirror_flags)
{
    int rv = CBX_E_NONE;

    /* validate vlan mode and mirror group mode */
    switch (vlan_mode) {
    case cbxMirrorModeIngress:
        if ( !(mirror_flags & CBX_MIRROR_MODE_VLAN_INGRESS )) {
            rv =  CBX_E_PARAM;
        }
        break;
    case cbxMirrorModeEgress:
        if ( !(mirror_flags & CBX_MIRROR_MODE_VLAN_EGRESS )) {
            rv =  CBX_E_PARAM;
        }
        break;
    default:
        rv =  CBX_E_PARAM;
        break;
    }

    return rv;
}

/*
 * Function:
 *  cbxi_robo2_mtgt_slot_get
 * Purpose:
 *  Get free ID to be used for mirror entry in MTGT.
 *
 */
STATIC int
cbxi_robo2_mtgt_slot_get( int unit, cbx_mirrorid_t  *mirrorid )
{
    int i;
    int rv = CBX_E_FULL;
    cbx_mirror_info_t  *mirror_info;

    mirror_info = MIRROR_CNTL.mirror_info;

    /* Find free mirror entry */
    for (i = 1; i < CBX_MIRROR_MAX; i++) {
        if (mirror_info[i].in_use == FALSE) {
            /* Mark mirror entry inuse and return */
            mirror_info[i].in_use = TRUE;
            *mirrorid = i;
            SHR_BITSET(MIRROR_CNTL.mirror_bitmap, *mirrorid);
            rv = CBX_E_NONE;
            break;
        }
    }
    return rv;
}

/*
 * Function:
 *  cbxi_robo2_mirror_dest_slot_get
 * Purpose:
 *  Get free ID to be used for mirror destination entry in DLIET.
 *
 */
STATIC int
cbxi_robo2_mirror_dest_slot_get(int unit,
                                cbx_mirror_destid_t  *mirror_destid )
{
    int i;
    int rv = CBX_E_FULL;
    cbx_mirror_dest_info_t  *mrrdest_info;

    mrrdest_info = MIRROR_CNTL.mirror_dest;

    /* Find free mirror destination entry */
    for (i = 1; i < CBX_MIRROR_DEST_MAX; i++) {
        if (mrrdest_info[i].in_use == FALSE) {
            /* Mark mirror destination entry inuse and return */
            mrrdest_info[i].in_use = TRUE;
            *mirror_destid = i;
            SHR_BITSET(MIRROR_CNTL.mirror_dest_bitmap, *mirror_destid);
            rv = CBX_E_NONE;
            break;
        }
    }
    return rv;
}

/*
 * Function:
 *  cbxi_robo2_ivmt_slot_get
 * Purpose:
 *  Get free ID to be used for adding entry to IVMT.
 *
 */
STATIC int
cbxi_robo2_ivmt_slot_get( int unit, cbx_vlanid_t vlanid, int *ivmt_index )
{
    int i;
    int rv = CBX_E_FULL;

    for (i = 0; i < CBXI_VMT_MAX; i++) {
        if (ivmt[i].in_use == FALSE) {
            /* Mark ivmt entry inuse and return */
            ivmt[i].in_use = TRUE;
            ivmt[i].vlanid = vlanid;
            *ivmt_index = i;
            rv = CBX_E_NONE;
            break;
        }
    }
    return  rv;
}

/*
 * Function:
 *  cbxi_robo2_ivmt_index_get
 * Purpose:
 *  Get IVMT index associated with the given VLAN ID.
 *
 */
STATIC int
cbxi_robo2_ivmt_index_get( int unit, cbx_vlanid_t vlanid, int *ivmt_index )
{
    int i;
    int rv = CBX_E_PARAM;

    for (i = 0; i < CBXI_VMT_MAX; i++) {
        if ((ivmt[i].in_use == TRUE) &&
                (ivmt[i].vlanid == vlanid)) {
            *ivmt_index = i;
            rv = CBX_E_NONE;
            break;
        }
    }
    return  rv;
}

/*
 * Function:
 *  cbxi_robo2_evmt_slot_get
 * Purpose:
 *  Get free ID to be used for adding entry to EVMT.
 *
 */
STATIC int
cbxi_robo2_evmt_slot_get( int unit, cbx_vlanid_t vlanid, int *evmt_index )
{
    int i;
    int rv = CBX_E_FULL;

    for (i = 0; i < CBXI_VMT_MAX; i++) {
        if (evmt[i].in_use == FALSE) {
            /* Mark evmt entry inuse and return */
            evmt[i].in_use = TRUE;
            evmt[i].vlanid = vlanid;
            *evmt_index = i;
            rv = CBX_E_NONE;
            break;
        }
    }
    return  rv;
}

/*
 * Function:
 *  cbxi_robo2_evmt_index_get
 * Purpose:
 *  Get EVMT index associated with the given VLAN ID.
 *
 */
STATIC int
cbxi_robo2_evmt_index_get( int unit, cbx_vlanid_t vlanid, int *evmt_index )
{
    int i;
    int rv = CBX_E_PARAM;

    for (i = 0; i < CBXI_VMT_MAX; i++) {
        if ((evmt[i].in_use == TRUE) &&
                (evmt[i].vlanid == vlanid)) {
            *evmt_index = i;
            rv = CBX_E_NONE;
            break;
        }
    }
    return  rv;
}

/*
 * Function:
 *  cbxi_robo2_SIA_mirror_config_update
 * Purpose:
 *  Update SIA mirror config register
 *
 */
STATIC int
cbxi_robo2_SIA_mirror_config_update( int unit,
                                     uint32_t   port_vsi,
                                     uint32_t   flag )
{
    uint32        reg_val = 0;
    uint32        fld_val = 0;
    vsit_t        vsit_entry;
    lpg2ppfov_t   ppfov_entry;
    int           lpgid = 0;

    /* Read SIA mirror config register */
    CBX_IF_ERROR_RETURN(REG_READ_CB_SIA_MIRR_SI_CONFIGr(unit, &reg_val));

    CBX_IF_ERROR_RETURN(soc_CB_SIA_MIRR_SI_CONFIGr_field_get(unit,
                        &reg_val,
                        ENABLEf,
                        &fld_val));
    switch (flag) {
    case CBXI_MIRROR_PORT_ADD:
        /* Program egress mirrored port in SIA mirror config register */
        fld_val |= 1<<port_vsi;
        break;
    case CBXI_MIRROR_PORT_REMOVE:
        /* Clear egress mirrored port in SIA mirror config register */
        fld_val &= ~(1<<port_vsi);
        break;
    case CBXI_MIRROR_VLAN_ADD:
        /* Retrive VSIT table entry */
        CBX_IF_ERROR_RETURN(
            soc_robo2_vsit_get(unit, port_vsi, &vsit_entry));
        /* Program vsi pgmap in SIA mirror config register */
        CBX_PBMP_ITER(vsit_entry.pg_map, lpgid) {
            ppfov_entry.ppfov = 0;
            /* Read entry from LPP2PGFOV */
             CBX_IF_ERROR_RETURN(soc_robo2_lpg2ppfov_get(unit, lpgid,
                                                         &ppfov_entry));
             fld_val |= ppfov_entry.ppfov;
        }
        break;
    case CBXI_MIRROR_VLAN_REMOVE:
        /* Retrive VSIT table entry */
        CBX_IF_ERROR_RETURN(
            soc_robo2_vsit_get(unit, port_vsi, &vsit_entry));
        /* Clear vsi pgmap in SIA mirror config register */
        CBX_PBMP_ITER(vsit_entry.pg_map, lpgid) {
            ppfov_entry.ppfov = 0;
            /* Read entry from LPP2PGFOV */
             CBX_IF_ERROR_RETURN(soc_robo2_lpg2ppfov_get(unit, lpgid,
                                                         &ppfov_entry));
             fld_val &= ~(ppfov_entry.ppfov);
        }
        break;
    default:
        return CBX_E_PARAM;
        break;
    }

    CBX_IF_ERROR_RETURN(soc_CB_SIA_MIRR_SI_CONFIGr_field_set(unit,
                        &reg_val,
                        ENABLEf,
                        &fld_val));

    CBX_IF_ERROR_RETURN(REG_WRITE_CB_SIA_MIRR_SI_CONFIGr(unit, &reg_val));

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_robo2_ivmt_update
 * Purpose:
 *  Update IVMT entry at the given index
 *
 */
STATIC int
cbxi_robo2_ivmt_update( int unit,
                        int ivmt_index,
                        cbx_vlanid_t     vlanid,
                        cbx_mirrorid_t   mirrorid )
{
    uint32        reg_val = 0;
    uint32        vsi_val = 0;
    uint32        mirror_val = 0;
    cbx_vlan_t    vsi;

    /* Get VLAN to VSI mapping, UM+ mode (1:1 mapping) VLAN and VSI */
    if (vlanid == 0) {
        vsi = 0;
    } else {
        CBX_IF_ERROR_RETURN(cbxi_vlan_vsit_index_get(vlanid, &vsi));
    }
    vsi_val = vsi;

    if (mirrorid == CBX_MIRROR_INVALID) {
        /* Invalidate mirror id and VSI as VSI mirroring is disabled */
        mirror_val = 0;
        vsi_val  = 0;
    } else {
        mirror_val = mirrorid;
    }

    CBX_IF_ERROR_RETURN(soc_CB_IPP_IVMT_Ar_field_set(unit, &reg_val,
                        MIRRORf, &mirror_val));
    CBX_IF_ERROR_RETURN(soc_CB_IPP_IVMT_Ar_field_set(unit, &reg_val,
                        VSIf, &vsi_val));
    CBX_IF_ERROR_RETURN(REG_WRITE_CB_IPP_IVMT_Ar(unit, ivmt_index, &reg_val));

    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_robo2_evmt_update
 * Purpose:
 *  Update EVMT entry at the given index
 *
 */
STATIC int
cbxi_robo2_evmt_update( int unit, int evmt_index,
                        cbx_vlanid_t     vlanid,
                        cbx_mirrorid_t   mirrorid )
{
    uint32        reg_val = 0;
    uint32        tc_val = 0;
    uint32        dp_val = 0;
    uint32        drop_prob_val = 0;
    uint32        vsi_val = 0;
    uint32        mirror_val = 0;
    mtgt_t        mtgt_entry;
    cbx_vlan_t    vsi;
    uint32_t      flag = 0;
    int           rv = CBX_E_NONE;

    /* Get VLAN to VSI mapping, UM+ mode (1:1 mapping) VLAN and VSI */
    if (vlanid == 0) {
        vsi = 0;
    } else {
        CBX_IF_ERROR_RETURN(cbxi_vlan_vsit_index_get(vlanid, &vsi));
    }
    vsi_val = vsi;

    if (mirrorid == CBX_MIRROR_INVALID) {
        /* Invalidate mirror id,VSI and other parameters, VSI mirroring
         * disabled
         */
        tc_val = 0;
        dp_val = 0;
#if 0 /* Not used for now */
        truncate_val = 0;
#endif
        drop_prob_val = 0;
        mirror_val = 0;
        vsi_val = 0;
    } else {
        CBX_IF_ERROR_RETURN(soc_robo2_mtgt_get(unit, mirrorid, &mtgt_entry));
        tc_val = mtgt_entry.tc;
        dp_val = mtgt_entry.dp;
#if 0 /* Not used for now */
        truncate_val = mtgt_entry.truncate;
#endif
        drop_prob_val = mtgt_entry.drop_prob;
        mirror_val = mirrorid;
    }

    switch (evmt_index) {
    case 0:
        /* Clear EVMT VSI0 Robo fields */
        CBX_IF_ERROR_RETURN(soc_CB_EPP_EVMT_VSI0r_field_set(unit, &reg_val,
                            VSIf, &vsi_val));
        CBX_IF_ERROR_RETURN(REG_WRITE_CB_EPP_EVMT_VSI0r(unit, &reg_val));

        /* Clear EVMT VSI0 DATA fields */
        CBX_IF_ERROR_RETURN(soc_CB_EPP_EVMT_VSI0_DATAr_field_set(unit, &reg_val,
                            MDPf, &dp_val));
        CBX_IF_ERROR_RETURN(soc_CB_EPP_EVMT_VSI0_DATAr_field_set(unit, &reg_val,
                            MGPf, &mirror_val));
        CBX_IF_ERROR_RETURN(soc_CB_EPP_EVMT_VSI0_DATAr_field_set(unit, &reg_val,
                            MTCf, &tc_val));
        CBX_IF_ERROR_RETURN(soc_CB_EPP_EVMT_VSI0_DATAr_field_set(unit, &reg_val,
                            SAMPLEf, &drop_prob_val));
        CBX_IF_ERROR_RETURN(REG_WRITE_CB_EPP_EVMT_VSI0_DATAr(unit, &reg_val));
        break;
    case 1:
        /* Clear EVMT VSI1 Robo fields */
        CBX_IF_ERROR_RETURN(soc_CB_EPP_EVMT_VSI1r_field_set(unit, &reg_val,
                            VSIf, &vsi_val));
        CBX_IF_ERROR_RETURN(REG_WRITE_CB_EPP_EVMT_VSI1r(unit, &reg_val));
        /* Clear EVMT VSI1 DATA fields */
        CBX_IF_ERROR_RETURN(soc_CB_EPP_EVMT_VSI1_DATAr_field_set(unit, &reg_val,
                            MDPf, &dp_val));
        CBX_IF_ERROR_RETURN(soc_CB_EPP_EVMT_VSI1_DATAr_field_set(unit, &reg_val,
                            MGPf, &mirror_val));
        CBX_IF_ERROR_RETURN(soc_CB_EPP_EVMT_VSI1_DATAr_field_set(unit, &reg_val,
                            MTCf, &tc_val));
        CBX_IF_ERROR_RETURN(soc_CB_EPP_EVMT_VSI1_DATAr_field_set(unit, &reg_val,
                            SAMPLEf, &drop_prob_val));
        CBX_IF_ERROR_RETURN(REG_WRITE_CB_EPP_EVMT_VSI1_DATAr(unit, &reg_val));
        break;
    case 2:
        /* Clear EVMT VSI2 Robo fields */
        CBX_IF_ERROR_RETURN(soc_CB_EPP_EVMT_VSI2r_field_set(unit, &reg_val,
                            VSIf, &vsi_val));
        CBX_IF_ERROR_RETURN(REG_WRITE_CB_EPP_EVMT_VSI2r(unit, &reg_val));
        /* Clear EVMT VSI2 DATA fields */
        CBX_IF_ERROR_RETURN(soc_CB_EPP_EVMT_VSI2_DATAr_field_set(unit, &reg_val,
                            MDPf, &dp_val));
        CBX_IF_ERROR_RETURN(soc_CB_EPP_EVMT_VSI2_DATAr_field_set(unit, &reg_val,
                            MGPf, &mirror_val));
        CBX_IF_ERROR_RETURN(soc_CB_EPP_EVMT_VSI2_DATAr_field_set(unit, &reg_val,
                            MTCf, &tc_val));
        CBX_IF_ERROR_RETURN(soc_CB_EPP_EVMT_VSI2_DATAr_field_set(unit, &reg_val,
                            SAMPLEf, &drop_prob_val));
        CBX_IF_ERROR_RETURN(REG_WRITE_CB_EPP_EVMT_VSI2_DATAr(unit, &reg_val));
        break;
    case 3:
        /* Clear EVMT VSI3 Robo fields */
        CBX_IF_ERROR_RETURN(soc_CB_EPP_EVMT_VSI3r_field_set(unit, &reg_val,
                            VSIf, &vsi_val));
        CBX_IF_ERROR_RETURN(REG_WRITE_CB_EPP_EVMT_VSI3r(unit, &reg_val));
        /* Clear EVMT VSI3 DATA fields */
        CBX_IF_ERROR_RETURN(soc_CB_EPP_EVMT_VSI3_DATAr_field_set(unit, &reg_val,
                            MDPf, &dp_val));
        CBX_IF_ERROR_RETURN(soc_CB_EPP_EVMT_VSI3_DATAr_field_set(unit, &reg_val,
                            MGPf, &mirror_val));
        CBX_IF_ERROR_RETURN(soc_CB_EPP_EVMT_VSI3_DATAr_field_set(unit, &reg_val,
                            MTCf, &tc_val));
        CBX_IF_ERROR_RETURN(soc_CB_EPP_EVMT_VSI3_DATAr_field_set(unit, &reg_val,
                            SAMPLEf, &drop_prob_val));
        CBX_IF_ERROR_RETURN(REG_WRITE_CB_EPP_EVMT_VSI3_DATAr(unit, &reg_val));
        break;
    default:
        rv = CBX_E_PARAM;
        break;
    }

    if (rv == CBX_E_NONE) {
        if (mirrorid != CBX_MIRROR_INVALID) {
            flag = CBXI_MIRROR_VLAN_ADD;
        } else {
            flag = CBXI_MIRROR_VLAN_REMOVE;
        }
        CBX_IF_ERROR_RETURN(cbxi_robo2_SIA_mirror_config_update( unit,
                            (uint32_t)vsi,
                            flag ));
    }
    return rv;
}

/*
 * Function:
 *  cbxi_mirror_table_init
 * Purpose:
 *  Global Initialization for all mirror related tables.
 *
 */
STATIC int
cbxi_mirror_table_init( void )
{

    uint32        num_units = 1;
    int           unit = 0;
    int           i = 0;

    /* Check if it is cascade set-up */
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = CBX_MAX_UNITS;
    }

    for ( unit = 0; unit < num_units; unit++ ) {
#if 0
        /* Enable and reset MTGT */
        CBX_IF_ERROR_RETURN(soc_robo2_mtgt_enable(unit));
        CBX_IF_ERROR_RETURN(soc_robo2_mtgt_reset(unit));
#endif

        /* Clear IVMT,EVMT registers and dli2ldpg table */
        for ( i = 0; i < CBXI_VMT_MAX; i++ ) {
            CBX_IF_ERROR_RETURN(cbxi_robo2_ivmt_update(unit, i,
                                0, CBX_MIRROR_INVALID));
            CBX_IF_ERROR_RETURN(cbxi_robo2_evmt_update(unit, i,
                                0, CBX_MIRROR_INVALID));
        }
    }

    return CBX_E_NONE;
}

/**************************************************************************
 *                 MIRROR FSAL API IMPLEMENTATION                           *
 **************************************************************************/

/**
 * Initialize mirror destination parameters.
 *
 * @param mirror_dest mirror destination parameter object
 */
void cbx_mirror_dest_t_init( cbx_mirror_dest_t *mirror_dest )
{
    if ( NULL != mirror_dest ) {
        sal_memset( mirror_dest, 0, sizeof ( cbx_mirror_dest_t ));
    }
    return;
}

/**
 * Initialize mirror parameters.
 *
 * @param mirror_params mirror parameter object
 */
void cbx_mirror_params_t_init( cbx_mirror_params_t *mirror_params )
{
    if ( NULL != mirror_params ) {
        sal_memset( mirror_params, 0, sizeof ( cbx_mirror_params_t ));
    }
    return;
}

/**
 * Initialize mirror port parameters.
 *
 * @param mirror_port_params mirror port parameter object
 */
void cbx_mirror_port_params_t_init(cbx_mirror_port_params_t *mirror_port_params)
{
    if ( NULL != mirror_port_params ) {
        sal_memset( mirror_port_params, 0, sizeof ( cbx_mirror_port_params_t ));
    }
    return;
}

/**
 * Initialize mirror vlan parameters.
 *
 * @param mirror_vlan_params mirror vlan parameter object
 */
void cbx_mirror_vlan_params_t_init(cbx_mirror_vlan_params_t *mirror_vlan_params)
{
    if ( NULL != mirror_vlan_params ) {
        sal_memset( mirror_vlan_params, 0, sizeof ( cbx_mirror_vlan_params_t ));
    }
    return;
}

/*
 * Function:
 *      cbx_mirror_cntl_t_init
 * Purpose:
 *      Initialize mirror control structure parameters.
 * Parameters:
 *      mirror_cntl - Pointer to mirror control structure.
 * Returns:
 *      None
 */

void cbx_mirror_cntl_t_init( cbx_mirror_cntl_t *mirror_cntl )
{
    if ( NULL != mirror_cntl ) {
        sal_memset( mirror_cntl, 0, sizeof ( cbx_mirror_cntl_t ));
    }
    return;
}

/*
 * Function:
 *      cbx_mirror_init
 * Purpose:
 *      Initializes the mirror module
 * Parameters:
 *      None.
 * Returns:
 *      CBX_E_NONE - success (or already initialized)
 *      CBX_E_MEMORY - failed to allocate required memory.
 */
int
cbx_mirror_init(void)
{

    int unit = CBX_AVENGER_PRIMARY;
    uint32 num_units = 1;
    int alloc_size=0;
    cbx_mirror_info_t  *mirror_info;
    cbx_mirror_dest_info_t  *mrrdest_info;
    int  rv,i=0;
    rv = CBX_E_NONE;

    LOG_INFO(BSL_LS_FSAL_MIRROR,
             (BSL_META("FSAL API : cbx_mirror_init()..\n")));

    cbx_mirror_cntl_t_init(&MIRROR_CNTL);

    /* Enable all tables related to mirror module */
    rv = cbxi_mirror_table_init();
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_MIRROR,
            (BSL_META("FSAL API : cbx_mirror_init()..Table init failed\n")));
        return rv;
    }

    /* Allocate mirror group info structure */
    alloc_size = CBX_MIRROR_MAX * sizeof(cbx_mirror_info_t);
    mirror_info = (cbx_mirror_info_t *) sal_alloc(alloc_size, "mirror_info");
    if (mirror_info == NULL) {
        return (CBX_E_MEMORY);
    }

    MIRROR_CNTL.mirror_info = mirror_info;

    /* Allocate mirror group bitmap */
    alloc_size = SHR_BITALLOCSIZE(CBX_MIRROR_MAX+ 1);
    if (MIRROR_CNTL.mirror_bitmap == NULL) {
        MIRROR_CNTL.mirror_bitmap = sal_alloc(alloc_size, "MIRROR-bitmap");
    }

    sal_memset(MIRROR_CNTL.mirror_bitmap, 0, alloc_size);

    /* Allocate mirror destination info structure */
    alloc_size = CBX_MIRROR_DEST_MAX * sizeof(cbx_mirror_dest_info_t);
    mrrdest_info = (cbx_mirror_dest_info_t *) sal_alloc(alloc_size,
                   "mirror_dest_info");
    if (mrrdest_info == NULL) {
        return (CBX_E_MEMORY);
    }

    MIRROR_CNTL.mirror_dest = mrrdest_info;

    /* Allocate mirror destination bitmap */
    alloc_size = SHR_BITALLOCSIZE(CBX_MIRROR_DEST_MAX+ 1);
    if (MIRROR_CNTL.mirror_dest_bitmap == NULL) {
        MIRROR_CNTL.mirror_dest_bitmap = sal_alloc(alloc_size,
                                         "MIRRORDEST-bitmap");
    }

    sal_memset(MIRROR_CNTL.mirror_dest_bitmap, 0, alloc_size);

    /* Mark all mirror entries unused */
    mirror_info = MIRROR_CNTL.mirror_info;

    /* Check if it is cascade set-up */
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = CBX_MAX_UNITS;
    }

    for (i = 0; i < CBX_MIRROR_MAX; i++) {
        mirror_info[i].in_use = FALSE;
        mirror_info[i].flags = 0;
        mirror_info[i].mirrorid = CBX_MIRROR_INVALID;
        mirror_info[i].mirror_destid = CBX_MIRROR_DEST_INVALID;
        for(unit=0;unit<num_units;unit++) {
            CBX_PBMP_CLEAR(mirror_info[i].ingport_pbmp[unit]);
            CBX_PBMP_CLEAR(mirror_info[i].egrport_pbmp[unit]);
        }

#ifdef MANAGED_MODE
        /* Allocate ingress vlan bitmap */
        alloc_size = SHR_BITALLOCSIZE(CBX_VLAN_MAX+ 1);
        if (mirror_info[i].ingvlan_bitmap == NULL) {
            mirror_info[i].ingvlan_bitmap = sal_alloc(alloc_size,
                                            "MIRROR-INGVLAN_bitmap");
        }

        /* Allocate egress vlan bitmap */
        alloc_size = SHR_BITALLOCSIZE(CBX_VLAN_MAX+ 1);
        if (mirror_info[i].egrvlan_bitmap == NULL) {
            mirror_info[i].egrvlan_bitmap = sal_alloc(alloc_size,
                                            "MIRROR-EGRVLAN_bitmap");
        }
#endif /* MANAGED_MODE */
    }

    /* Mark first mirror entry used as mirror 0 indicates no mirror */
    mirror_info[0].in_use = TRUE;

    /* Mark all mirror destination entries unused */
    mrrdest_info = MIRROR_CNTL.mirror_dest;
    for (i = 0; i < CBX_MIRROR_DEST_MAX; i++) {
        mrrdest_info[i].in_use = FALSE;
        /* Clear mirror destination usage reference count */
        mrrdest_info[i].ref_count = 0;
    }

    /* Mark all ivmt and evmt entries unused */
    for (i = 0; i < CBXI_VMT_MAX; i++) {
        sal_memset(&ivmt[i], 0, sizeof(cbx_mirror_vmt_info_t));
        sal_memset(&evmt[i], 0, sizeof(cbx_mirror_vmt_info_t));
        ivmt[i].in_use = FALSE;
        evmt[i].in_use = FALSE;
    }

    /* Mark mirror init complete */
    MIRROR_CNTL.init = TRUE;

    return CBX_E_NONE;
}

/**
 * Function:
 *      cbx_mirror_dest_create
 * Purpose:
 * Create a mirror destination
 * This routine is used to create a mirror destination.
 *
 * @param mirror_dest   (IN)  Mirror destination parameters.
 * @param mirror_destid (OUT) Handle of the mirror destination created
 *                      CBX_MIRROR_DEST_INVALID: Mirror destination could
 *                      not be created
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_mirror_dest_create ( cbx_mirror_dest_t   *mirror_dest,
                             cbx_mirror_destid_t *mirror_destid )
{
    int             unit = CBX_AVENGER_PRIMARY;
    cbx_mirror_destid_t  mrrdest_index = CBX_MIRROR_DEST_INVALID;
    cbx_port_t      port_out = CBX_PORT_INVALID;
    cbxi_pgid_t     lpg = CBXI_PGID_INVALID;
    int             rv = CBX_E_NONE;
    int             dlinid = 0;
    cbx_mirror_dest_info_t  *mrrdest_info;
#ifdef CONFIG_CASCADED_MODE
    int             csd_unit = CBX_AVENGER_PRIMARY;
    cbx_portid_t    csd_portid  = CBX_PORT_INVALID;
    cbx_port_t      csd_portout = CBX_PORT_INVALID;
    cbxi_pgid_t     csd_lpg = CBXI_PGID_INVALID;
#endif /* CONFIG_CASCADED_MODE */

    MIRROR_INIT;

    /* validate the port index passed */
    rv = cbxi_robo2_port_validate(mirror_dest->port, &port_out, &lpg, &unit);
    if (CBX_FAILURE(rv)) {
        return CBX_E_PORT;
    }

    /* Allocate free slot for mirror destination */
    rv = cbxi_robo2_mirror_dest_slot_get(unit, &mrrdest_index);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_MIRROR,
                (BSL_META("FSAL API : cbx_mirror_dest_create()..Failed to get \
                              mirror destination slot get\n")));
        return rv;
    }

    /*  Allocate DLI.N index
     *  Get GPG for the given dest port
     *  Program the dli2dpg table at DLI.N index with the GPG value
     *  PPFOV and SVT table entries for the dest port are already
     *  programmed in port module.
     */
    CBX_IF_ERROR_RETURN(cbxi_lin_slot_get(&dlinid));
    CBX_IF_ERROR_RETURN(cbxi_lin_dest_set(unit,dlinid, lpg));

#ifdef CONFIG_CASCADED_MODE
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        if(unit == CBX_AVENGER_PRIMARY ) {
            CBX_IF_ERROR_RETURN(cbxi_cascade_port_get(CBX_AVENGER_SECONDARY,
                                &csd_portid));

            /* validate the port index */
            CBX_IF_ERROR_RETURN(cbxi_robo2_port_validate(csd_portid,
                                         &csd_portout, &csd_lpg, &csd_unit));

            /* Get cascade port, and find lpg for the cascade port */
            CBX_IF_ERROR_RETURN(cbxi_lin_dest_set(csd_unit,
                                                  dlinid, csd_lpg));
        } else {
            CBX_IF_ERROR_RETURN(cbxi_cascade_port_get(CBX_AVENGER_PRIMARY,
                                &csd_portid));

            /* validate the port index */
            CBX_IF_ERROR_RETURN(cbxi_robo2_port_validate(csd_portid,
                                         &csd_portout, &csd_lpg, &csd_unit));

            /* Get cascade port, and find lpg for the cascade port */
            CBX_IF_ERROR_RETURN(cbxi_lin_dest_set(csd_unit,
                                                  dlinid, csd_lpg));
        }
    }
#endif /* CONFIG_CASCADED_MODE */

    /* Save mirror dest parameters to mirror dest info entry */
    mrrdest_info = &MIRROR_DEST_INFO(mrrdest_index);
    sal_memcpy(&mrrdest_info->mirror_dest, mirror_dest,
               sizeof(cbx_mirror_dest_t));

    mrrdest_info->mirror_destid = mrrdest_index;
    mrrdest_info->dlinid = dlinid;
    mrrdest_info->ref_count = 0;
    *mirror_destid = mrrdest_index;
    cbxi_mirror_dest_info_dump();
    return CBX_E_NONE;
}


/**
 * Function:
 *      cbx_mirror_dest_destroy
 * Purpose:
 * Destroy a mirror destination previously created by cbx_mirror_dest_create()
 *
 * @param mirror_destid    (IN) Handle of the mirror destination to be destroyed
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 *
 * @note This routine destroys a previously created mirror destination.
 */

int cbx_mirror_dest_destroy ( cbx_mirror_destid_t mirror_destid )
{
    cbx_mirror_dest_info_t  *mrrdest_info;

    MIRROR_INIT;
    MIRROR_DESTID_CHECK(mirror_destid);
    mrrdest_info = &MIRROR_DEST_INFO(mirror_destid);

    if (mrrdest_info->in_use == FALSE) {
        LOG_ERROR(BSL_LS_FSAL_MIRROR,
                  (BSL_META("FSAL API : cbx_mirror_dest_destroy..invalid \
                             mirror destination \n")));
        return CBX_E_PARAM;
    }

    /* Check Mirror destination usage reference count */
    if (mrrdest_info->ref_count != 0) {
        LOG_ERROR(BSL_LS_FSAL_MIRROR,
                  (BSL_META("FSAL API : cbx_mirror_dest_destroy..invalid \
                             mirror destination ref count \n")));
        return CBX_E_INTERNAL;
    }

    /* Free DLINID mapping */
    CBX_IF_ERROR_RETURN(cbxi_lin_slot_release(mrrdest_info->dlinid));

    /* Clear mirror destination bitmap and free the slot */
    SHR_BITCLR(MIRROR_CNTL.mirror_dest_bitmap, mirror_destid);
    mrrdest_info->in_use = FALSE;
    mrrdest_info->dlinid = 0;
    mrrdest_info->mirror_destid = CBX_MIRROR_DEST_INVALID;
    sal_memset(&mrrdest_info->mirror_dest, 0,
               sizeof(cbx_mirror_dest_t));
    cbxi_mirror_dest_info_dump();
    return CBX_E_NONE;
}

/**
 * Function:
 *      cbx_mirror_create
 * Purpose:
 * Create a mirror
 * This routine is used to create a mirror.
 *
 * @param mirror_params (IN)  Mirror parameters.
 * @param mirror_destid (IN)  Mirror destination identifier
 * @param mirrorid      (OUT) Handle of the mirror created
 *                            CBX_MIRROR_INVALID: Mirror could not be created
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_mirror_create ( cbx_mirror_params_t *mirror_params,
                        cbx_mirror_destid_t  mirror_destid,
                        cbx_mirrorid_t      *mirrorid )
{
    int             unit = CBX_AVENGER_PRIMARY;
    uint32          status  = 0;
    cbx_mirrorid_t  mtgt_index = CBX_MIRROR_INVALID;
    cbx_mirror_dest_info_t  *mrrdest_info;
    cbx_mirror_info_t  *mirror_info;
    mtgt_t          mtgt_entry;
    int             rv = CBX_E_NONE;

    MIRROR_INIT;
    MIRROR_DESTID_CHECK(mirror_destid);

    /* Validate mirror group create flags */
    rv = cbxi_robo2_mirror_create_flags_validate(mirror_params->flags);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_MIRROR,
                  (BSL_META("FSAL API : cbx_mirror_create()..mirror create \
                              flags validate failed\n")));
        return rv;
    }

    /* Allocate free slot for mirror group */
    rv = cbxi_robo2_mtgt_slot_get(unit, &mtgt_index);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_MIRROR,
                  (BSL_META("FSAL API : cbx_mirror_create()..Failed to get \
                              mirror group slot get\n")));
        return rv;
    }


    /* Read entry from MTGT */
    CBX_IF_ERROR_RETURN(soc_robo2_mtgt_get(unit, mtgt_index, &mtgt_entry));

    /* Update MTGT entry */
    mtgt_entry.tc = mirror_params->mirror_tc;
    mtgt_entry.dp = mirror_params->mirror_dp;
    /* Extract truncate flag */
    mtgt_entry.truncate = !!(mirror_params->flags & CBX_MIRROR_TRUNCATE);
    mtgt_entry.drop_prob = mirror_params->sample_rate;

    /* Exrtract dlin from mirror destination */
    mrrdest_info = &MIRROR_DEST_INFO(mirror_destid);
    if (mrrdest_info->in_use == FALSE) {
        LOG_ERROR(BSL_LS_FSAL_MIRROR,
                  (BSL_META("FSAL API : cbx_mirror_create()..Invalid mirror \
                              destination\n")));
        return CBX_E_PARAM;
    }
    mtgt_entry.dli_n = mrrdest_info->dlinid;
    rv = soc_robo2_mtgt_set(unit, mtgt_index,
                            &mtgt_entry, &status);

#ifdef CONFIG_CASCADED_MODE
    /* Configure mtgt entry for cascade set-up */
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        rv = soc_robo2_mtgt_set(CBX_AVENGER_SECONDARY, mtgt_index,
                                &mtgt_entry, &status);
    }
#endif /* CONFIG_CASCADED_MODE */

    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_MIRROR,
                  (BSL_META("FSAL API : cbx_mirror_create()..Failed to free \
                              mirror group slot \n")));
        /* Free mirror ID slot for mirror group */
        mirror_info = &MIRROR_INFO(mtgt_index);
        mirror_info->in_use = FALSE;
        SHR_BITCLR(MIRROR_CNTL.mirror_bitmap, mtgt_index);
        return rv;
    }

    /* Increment mirror destination usage reference count */
    mrrdest_info->ref_count++;

    mirror_info = &MIRROR_INFO(mtgt_index);
    mirror_info->flags = mirror_params->flags;
    mirror_info->mirror_destid = mirror_destid;
    mirror_info->mirrorid = mtgt_index;
    *mirrorid = mtgt_index;

    return CBX_E_NONE;
}


/**
 * Function:
 *      cbx_mirror_destroy
 * Purpose:
 * Destroy a mirror previously created by cbx_mirror_create()
 *
 * @param mirrorid    (IN)  Handle of the mirror to be destroyed
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 *
 * @note This routine destroys a previously created mirror.
 */

int cbx_mirror_destroy ( cbx_mirrorid_t mirrorid )
{
    int            unit = CBX_AVENGER_PRIMARY;
    uint32         status  = 0;
    mtgt_t         mtgt_entry;
    cbx_mirror_info_t  *mirror_info;
    cbx_mirror_dest_info_t  *mrrdest_info;

    MIRROR_INIT;
    MIRROR_ID_CHECK(mirrorid);

    mirror_info = &MIRROR_INFO(mirrorid);

    if (mirror_info->in_use == FALSE) {
        LOG_ERROR(BSL_LS_FSAL_MIRROR,
                  (BSL_META("FSAL API : cbxi_mirror_remove..invalid \
                             mirror group \n")));
        return CBX_E_PARAM;
    }

    /* Clear mirror group's bitmap and free the slot */
    SHR_BITCLR(MIRROR_CNTL.mirror_bitmap, mirrorid);
    mirror_info->in_use = FALSE;
    mirror_info->mirrorid = CBX_MIRROR_INVALID;

    mrrdest_info = &MIRROR_DEST_INFO(mirror_info->mirror_destid);

    /* Decrement mirror destination usage reference count */
    mrrdest_info->ref_count--;

    /* Destory entry from MTGT table */
    sal_memset(&mtgt_entry, 0, sizeof(mtgt_t));
    CBX_IF_ERROR_RETURN(soc_robo2_mtgt_set(unit, mirrorid,
                                           &mtgt_entry, &status));

#ifdef CONFIG_CASCADED_MODE
    /* Destroy entry from MTGT table for cascade set-up */
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        sal_memset(&mtgt_entry, 0, sizeof(mtgt_t));
        CBX_IF_ERROR_RETURN(soc_robo2_mtgt_set(unit, mirrorid,
                                               &mtgt_entry, &status));
    }
#endif /* CONFIG_CASCADED_MODE */

    return CBX_E_NONE;
}

/**
 * Function:
 *      cbx_mirror_port_add
 * Purpose:
 * Mirror port add
 * This routine is used to add a Mirror to a port
 *
 * @param mirrorid             (IN)  Mirror Identifier
 * @param portid               (IN)  Port Identifier
 * @param mirror_port_params   (IN)  Mirror port parameters.
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_mirror_port_add ( cbx_mirrorid_t            mirrorid,
                          cbx_portid_t              portid,
                          cbx_mirror_port_params_t *mirror_port_params )
{
    int             unit = CBX_AVENGER_PRIMARY;
    cbx_port_t      port_out = CBX_PORT_INVALID;
    cbxi_pgid_t     lpg = CBXI_PGID_INVALID;
    pgt_t           pgt_entry;
    epgt_t          epgt_entry;
    mtgt_t          mtgt_entry;
    uint32          status  = 0;
    int             rv = CBX_E_NONE;
    cbx_mirror_info_t  *mirror_info;

    MIRROR_INIT;
    MIRROR_ID_CHECK(mirrorid);

    mirror_info = &MIRROR_INFO(mirrorid);

    if (mirror_info->in_use == FALSE) {
        LOG_ERROR(BSL_LS_FSAL_MIRROR,
                  (BSL_META("FSAL API : cbxi_mirror_port_add..invalid \
                             mirror group \n")));
        return CBX_E_PARAM;
    }

    /* LAG port is not permitted */
    if (CBX_PORTID_IS_TRUNK(portid)) {
        return CBX_E_PORT;
    }

    /* validate the port index passed */
    rv = cbxi_robo2_port_validate(portid, &port_out, &lpg, &unit);
    if (CBX_FAILURE(rv)) {
        return CBX_E_PORT;
    }

    rv = cbxi_robo2_mirror_port_mode_validate( mirror_port_params->mode,
            mirror_info->flags);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_MIRROR,
                  (BSL_META("FSAL API : cbxi_mirror_port_add..invalid \
                             port mode\n")));
        return CBX_E_PARAM;
    }

    /* Update ingress PGT */
    if (mirror_port_params->mode == cbxMirrorModeIngress) {
        if (CBX_PBMP_MEMBER(mirror_info->ingport_pbmp[unit], port_out)) {
            LOG_INFO(BSL_LS_FSAL_MIRROR,
                     (BSL_META("FSAL API : cbx_mirror_port_add()..port \
                     is member of mirror id=%d.  \n"), mirror_info->mirrorid));
            return CBX_E_EXISTS;
        }

        /* Read entry from ingress PGT */
        CBX_IF_ERROR_RETURN(soc_robo2_pgt_get( unit, lpg, &pgt_entry));

        pgt_entry.mirror = mirrorid;

        /* Update ingress PGT entry */
        CBX_IF_ERROR_RETURN(soc_robo2_pgt_set( unit, lpg, &pgt_entry, &status));
        CBX_PBMP_PORT_ADD(mirror_info->ingport_pbmp[unit], port_out);

    } else {
        if (CBX_PBMP_MEMBER(mirror_info->egrport_pbmp[unit], port_out)) {
            LOG_INFO(BSL_LS_FSAL_MIRROR,
                     (BSL_META("FSAL API : cbx_mirror_port_add()..port \
                     is member of mirror id=%d.  \n"), mirror_info->mirrorid));
            return CBX_E_EXISTS;
        }

        CBX_IF_ERROR_RETURN(cbxi_robo2_SIA_mirror_config_update( unit,
                            (uint32_t)port_out,
                            CBXI_MIRROR_PORT_ADD ));

        /* Read entry from egress PGT */
        CBX_IF_ERROR_RETURN(soc_robo2_epgt_get( unit, lpg, &epgt_entry));

        /* Read entry from MTGT */
        CBX_IF_ERROR_RETURN(soc_robo2_mtgt_get(unit, mirrorid, &mtgt_entry));

        epgt_entry.mirror    = mirrorid;
        epgt_entry.mirror_tc = mtgt_entry.tc;
        epgt_entry.sample    = mtgt_entry.drop_prob;
        epgt_entry.mirror_dp = mtgt_entry.dp;

        /* Update egress PGT entry */
        CBX_IF_ERROR_RETURN(soc_robo2_epgt_set( unit, lpg, &epgt_entry,
                                                &status));
        CBX_PBMP_PORT_ADD(mirror_info->egrport_pbmp[unit], port_out);
    }

    return rv;
}

/**
 * Function:
 *      cbx_mirror_port_remove
 * Purpose:
 * Mirror port remove
 * This routine is used to remove a Mirror from a port
 *
 * @param mirrorid    (IN) Mirror Identifier
 * @param portid      (IN) Port Identifier
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_mirror_port_remove ( cbx_mirrorid_t mirrorid,
                             cbx_portid_t   portid )
{
    int             unit = CBX_AVENGER_PRIMARY;
    cbx_port_t      port_out = CBX_PORT_INVALID;
    cbxi_pgid_t     lpg= CBXI_PGID_INVALID;
    pgt_t           pgt_entry;
    epgt_t          epgt_entry;
    uint32          status  = 0;
    int             rv = CBX_E_NONE;
    cbx_mirror_info_t  *mirror_info;

    MIRROR_INIT;
    MIRROR_ID_CHECK(mirrorid);
    mirror_info = &MIRROR_INFO(mirrorid);

    if (mirror_info->in_use == FALSE) {
        LOG_ERROR(BSL_LS_FSAL_MIRROR,
                  (BSL_META("FSAL API : cbxi_mirror_port_remove..invalid \
                             mirror group \n")));
        return CBX_E_PARAM;
    }

    /* LAG port is not permitted */
    if (CBX_PORTID_IS_TRUNK(portid)) {
        return CBX_E_PORT;
    }

    /* validate the port index passed */
    rv = cbxi_robo2_port_validate(portid, &port_out, &lpg, &unit);
    if (CBX_FAILURE(rv)) {
        return CBX_E_PORT;
    }

    if (mirror_info->flags & CBX_MIRROR_MODE_PORT_INGRESS) {
        /* Check if port is ingress mirrored */
        if (!(CBX_PBMP_MEMBER(mirror_info->ingport_pbmp[unit], port_out))) {
            LOG_INFO(BSL_LS_FSAL_MIRROR,
                     (BSL_META("FSAL API : cbx_mirror_port_remove()..port \
                  is not member of mirror id=%d.  \n"), mirror_info->mirrorid));
            return CBX_E_NOT_FOUND;
        }

        /* Read entry from ingress PGT */
        CBX_IF_ERROR_RETURN(soc_robo2_pgt_get( unit, lpg, &pgt_entry));

        pgt_entry.mirror = 0;/* clear mirror id */

        /* Update ingress PGT entry */
        CBX_IF_ERROR_RETURN(soc_robo2_pgt_set( unit, lpg, &pgt_entry, &status));
        CBX_PBMP_PORT_REMOVE(mirror_info->ingport_pbmp[unit], port_out);

    } else if (mirror_info->flags & CBX_MIRROR_MODE_PORT_EGRESS) {
        if (!(CBX_PBMP_MEMBER(mirror_info->egrport_pbmp[unit], port_out))) {
            LOG_INFO(BSL_LS_FSAL_MIRROR,
                     (BSL_META("FSAL API : cbx_mirror_port_remove()..port \
                  is not member of mirror id=%d.  \n"), mirror_info->mirrorid));
            return CBX_E_NOT_FOUND;
        }

        CBX_IF_ERROR_RETURN(cbxi_robo2_SIA_mirror_config_update( unit,
                            (uint32_t)port_out,
                            CBXI_MIRROR_PORT_REMOVE ));

        /* Read entry from egress PGT */
        CBX_IF_ERROR_RETURN(soc_robo2_epgt_get( unit, lpg, &epgt_entry));

        epgt_entry.mirror = 0;/* clear mirror id */
        epgt_entry.mirror_tc = 0;
        epgt_entry.sample    = 0;
        epgt_entry.mirror_dp = 0;

        /* Update egress PGT entry */
        CBX_IF_ERROR_RETURN(soc_robo2_epgt_set( unit, lpg, &epgt_entry,
                                                &status));
        CBX_PBMP_PORT_REMOVE(mirror_info->egrport_pbmp[unit], port_out);
    } else {
        LOG_ERROR(BSL_LS_FSAL_MIRROR,
                  (BSL_META("FSAL API : cbxi_mirror_port_remove..invalid \
                             port mode\n")));
        rv = CBX_E_PARAM;
    }

    return rv;
}

/**
 * Function:
 *      cbx_mirror_vlan_add
 * Purpose:
 * Mirror vlan add
 * This routine is used to add a Mirror to a vlan
 *
 * @param mirrorid             (IN)  Mirror Identifier
 * @param vlanid               (IN)  VLAN Identifier
 * @param mirror_vlan_params   (IN)  Mirror port parameters.
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_mirror_vlan_add ( cbx_mirrorid_t            mirrorid,
                          cbx_vlanid_t              vlanid,
                          cbx_mirror_vlan_params_t *mirror_vlan_params )
{
    int             unit = CBX_AVENGER_PRIMARY;
    int             rv = CBX_E_NONE;
    int             vmt_index = CBX_MIRROR_INVALID;
    cbx_mirror_info_t  *mirror_info;

    MIRROR_INIT;
    MIRROR_ID_CHECK(mirrorid);
    CHECK_VID(vlanid);
    mirror_info = &MIRROR_INFO(mirrorid);

    if (mirror_info->in_use == FALSE) {
        LOG_ERROR(BSL_LS_FSAL_MIRROR,
                  (BSL_META("FSAL API : cbxi_mirror_vlan_add..invalid \
                             mirror group \n")));
        return CBX_E_PARAM;
    }

    rv = cbxi_robo2_mirror_vlan_mode_validate( mirror_vlan_params->mode,
            mirror_info->flags);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_MIRROR,
                  (BSL_META("FSAL API : cbxi_mirror_vlan_add..invalid \
                             vlan mode\n")));
        return CBX_E_PARAM;
    }

    /* Update ingress VMT table */
    if (mirror_vlan_params->mode == cbxMirrorModeIngress) {
#ifdef MANAGED_MODE
        if (SHR_BITGET(mirror_info->ingvlan_bitmap, vlanid)) {
#else
        rv = cbxi_robo2_ivmt_index_get( unit, vlanid, &vmt_index);
        if (CBX_SUCCESS(rv)) {
#endif /* MANAGED_MODE */
            LOG_INFO(BSL_LS_FSAL_MIRROR,
                     (BSL_META("FSAL API : cbx_mirror_vlan_add()..vlan \
                    is member of mirror id=%d.  \n"), mirror_info->mirrorid));
            return CBX_E_EXISTS;
        }

        CBX_IF_ERROR_RETURN(cbxi_robo2_ivmt_slot_get( unit, vlanid,
                            &vmt_index));

        rv = cbxi_robo2_ivmt_update( unit, vmt_index,
                                     vlanid, mirrorid);
        if (CBX_FAILURE(rv)) {
            /* Free IVMT slot */
            ivmt[vmt_index].in_use = FALSE;
            return CBX_E_FAIL;
        }

#ifdef CONFIG_CASCADED_MODE
        if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
            rv = cbxi_robo2_ivmt_update( CBX_AVENGER_SECONDARY, vmt_index,
                                         vlanid, mirrorid);
            if (CBX_FAILURE(rv)) {
                /* Free IVMT slot */
                ivmt[vmt_index].in_use = FALSE;
                return CBX_E_FAIL;
            }
        }
#endif /* CONFIG_CASCADED_MODE */
#ifdef MANAGED_MODE
        SHR_BITSET(mirror_info->ingvlan_bitmap, vlanid);
#endif /* MANAGED_MODE */

    } else {
#ifdef MANAGED_MODE
        if (SHR_BITGET(mirror_info->egrvlan_bitmap, vlanid)) {
#else
        rv = cbxi_robo2_evmt_index_get( unit, vlanid, &vmt_index);
        if (CBX_SUCCESS(rv)) {
#endif /* MANAGED_MODE */
            LOG_INFO(BSL_LS_FSAL_MIRROR,
                     (BSL_META("FSAL API : cbx_mirror_vlan_add()..vlan \
                    is member of mirror id=%d.  \n"), mirror_info->mirrorid));
            return CBX_E_EXISTS;
        }

        CBX_IF_ERROR_RETURN(cbxi_robo2_evmt_slot_get( unit, vlanid,
                            &vmt_index));

        rv = cbxi_robo2_evmt_update( unit, vmt_index,
                                     vlanid, mirrorid);
        if (CBX_FAILURE(rv)) {
            /* Free EVMT slot */
            evmt[vmt_index].in_use = FALSE;
            return CBX_E_FAIL;
        }

#ifdef CONFIG_CASCADED_MODE
        if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
            rv = cbxi_robo2_evmt_update( CBX_AVENGER_SECONDARY, vmt_index,
                                         vlanid, mirrorid);
            if (CBX_FAILURE(rv)) {
                /* Free IVMT slot */
                evmt[vmt_index].in_use = FALSE;
                return CBX_E_FAIL;
            }
        }
#endif /* CONFIG_CASCADED_MODE */
#ifdef MANAGED_MODE
        SHR_BITSET(mirror_info->egrvlan_bitmap, vlanid);
#endif /* MANAGED_MODE */
    }

    return rv;
}

/**
 * Function:
 *      cbx_mirror_vlan_remove
 * Purpose:
 * Mirror vlan remove
 * This routine is used to remove a Mirror from a vlan
 *
 * @param mirrorid   (IN)  Mirror Identifier
 * @param vlanid     (IN)  VLAN Identifier
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_mirror_vlan_remove ( cbx_mirrorid_t mirrorid,
                             cbx_vlanid_t   vlanid )
{
    int             unit = CBX_AVENGER_PRIMARY;
    int             rv = CBX_E_NONE;
    int             vmt_index = CBX_MIRROR_INVALID;
    cbx_mirror_info_t  *mirror_info;

    MIRROR_INIT;
    MIRROR_ID_CHECK(mirrorid);
    CHECK_VID(vlanid);
    mirror_info = &MIRROR_INFO(mirrorid);

    if (mirror_info->in_use == FALSE) {
        LOG_ERROR(BSL_LS_FSAL_MIRROR,
                  (BSL_META("FSAL API : cbxi_mirror_vlan_remove..invalid \
                             mirror group \n")));
        return CBX_E_PARAM;
    }

    /* Update ingress VMT table */
    if (mirror_info->flags & CBX_MIRROR_MODE_VLAN_INGRESS) {
        CBX_IF_ERROR_RETURN(cbxi_robo2_ivmt_index_get( unit, vlanid,
                            &vmt_index));
        /* Free IVMT slot */
        ivmt[vmt_index].in_use = FALSE;
#ifdef MANAGED_MODE
        SHR_BITCLR(mirror_info->ingvlan_bitmap, vlanid);
#endif /* MANAGED_MODE */

        CBX_IF_ERROR_RETURN(cbxi_robo2_ivmt_update( unit, vmt_index,
                            vlanid, CBX_MIRROR_INVALID));
#ifdef CONFIG_CASCADED_MODE
        if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
            CBX_IF_ERROR_RETURN(cbxi_robo2_ivmt_update(CBX_AVENGER_SECONDARY,
                                vmt_index, vlanid, CBX_MIRROR_INVALID));
        }
#endif /* CONFIG_CASCADED_MODE */
    } else if (mirror_info->flags & CBX_MIRROR_MODE_VLAN_EGRESS) {
        CBX_IF_ERROR_RETURN(cbxi_robo2_evmt_index_get( unit, vlanid,
                            &vmt_index));
        /* Free EVMT slot */
        evmt[vmt_index].in_use = FALSE;
#ifdef MANAGED_MODE
        SHR_BITCLR(mirror_info->egrvlan_bitmap, vlanid);
#endif /* MANAGED_MODE */

        CBX_IF_ERROR_RETURN(cbxi_robo2_evmt_update(unit, vmt_index,
                            vlanid, CBX_MIRROR_INVALID));

#ifdef CONFIG_CASCADED_MODE
        if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
            CBX_IF_ERROR_RETURN(cbxi_robo2_evmt_update(CBX_AVENGER_SECONDARY,
                                vmt_index, vlanid, CBX_MIRROR_INVALID));
        }
#endif /* CONFIG_CASCADED_MODE */
    } else {
        LOG_ERROR(BSL_LS_FSAL_MIRROR,
                  (BSL_META("FSAL API : cbxi_mirror_vlan_remove..invalid \
                             vlan mode\n")));
        rv = CBX_E_PARAM;
    }
    return rv;
}

void cbxi_mirror_dest_info_dump ( )
{
#ifdef MIRROR_DEBUG_VERBOSE
    int  i=0;
    int  unit = CBX_AVENGER_PRIMARY;
    cbx_mirror_dest_info_t  *mrrdest_info;

    /* Locate first entry */
    mrrdest_info = &MIRROR_DEST_INFO(0);

    sal_printf("\nS.No  DESTID   in_use  ref_count  DLNID    \n");
    sal_printf("=======================================================\n");
    for (i = 0; i < CBX_MIRROR_DEST_MAX; i++) {
        if (mrrdest_info->in_use == TRUE) {
            sal_printf("%d    0x%x       %d       %d     %d\n",
                       i,mrrdest_info->mirror_destid,
                       mrrdest_info->in_use,
                       mrrdest_info->ref_count,
                       mrrdest_info->dlinid);

        }
        mrrdest_info++;
    }
    sal_printf("=======================================================\n");
#endif /* MIRROR_DEBUG_VERBOSE */
    return;
}
