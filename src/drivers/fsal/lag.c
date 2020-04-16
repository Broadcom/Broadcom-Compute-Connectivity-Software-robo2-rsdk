/*
 * * $ID: $
 * * 
 * * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 * * 
 * * $Copyright: (c) 2020 Broadcom Inc.
 * * All Rights Reserved$
 * *
 * * File:
 * *     lag.c
 * * Purpose:
 * *     Robo2 LAG module.
 * *
 * */
#include <bsl_log/bsl.h>
#include <sal_types.h>
#include <soc/robo2/common/tables.h>
#include <fsal/error.h>
#include <fsal_int/lag.h>
#include <fsal_int/port.h>
#include <fsal_int/slic.h>
#ifdef CONFIG_PORT_EXTENDER
#include <fsal_int/extender.h>
#endif
#include <fsal_int/mcast.h>
#include <fsal_int/stg.h>
#include <fsal_int/cfp.h>

/*
 * One lag control entry for each SOC device containing lag book keeping
 * info for that device.
 */

static cbx_lag_cntl_t    cbx_robo2_lag_control;
static cbx_port_t        lpgt[CBX_MAX_UNITS][CBX_MAX_PORT_PER_UNIT];
static cbx_port_t        pgt[MAX_PG];

#define LAG_CNTL       cbx_robo2_lag_control
#define LAG_INFO(tid)  cbx_robo2_lag_control.lag_info[tid]

/*
 * Cause a routine to return CBX_E_INIT if LAG subsystem is not initialized.
 */

#define LAG_INIT    \
      if (LAG_CNTL.ngroups <= 0 ||LAG_CNTL.lag_info == NULL) {\
          return CBX_E_INIT; }

/*
 * Make sure TID is within valid range.
 */

#define LAG_CHECK(tid) \
    if ((tid) >= LAG_CNTL.ngroups) { \
        return CBX_E_PARAM; }

void cbxi_lag_info_dump ( void );
void cbxi_lag_member_table_dump ( cbx_port_t  port, int lpgid );

/*
 * Return the number of bits set in a unsigned int
 */

int
_shr_popcount(unsigned int n)
{
    n = (n & 0x55555555) + ((n >> 1) & 0x55555555);
    n = (n & 0x33333333) + ((n >> 2) & 0x33333333);
    n = (n + (n >> 4)) & 0x0f0f0f0f;
    n = n + (n >> 8);

    return (n + (n >> 16)) & 0xff;
}

#ifdef CONFIG_CASCADED_MODE
/*
 * Function:
 *  cbxi_robo2_cascade_port_lpg_get
 * Purpose:
 *  Get free LPG ID to be allocated to pp.
 *
 */
int
cbxi_robo2_cascade_port_lpg_get( int unit, cbxi_pgid_t *lpgid )
{
    cbx_port_t     csd_port;

    CBX_PBMP_ITER(PBMP_CASCADE(unit), csd_port) {
        /* lpg is same for all LAG ports */
         break;
    }

    /* Obtain Local port group associated with physical cascade port */
    CBX_IF_ERROR_RETURN(cbxi_robo2_port_to_lpg(unit, csd_port, lpgid));

    return CBX_E_NONE;
}
#endif /* CONFIG_CASCADED_MODE */

/*
 * Function:
 *  cbxi_lag_psc_flags_validate
 * Purpose:
 *  Validate PSC flags for given packet type.
 *
 */
int cbxi_lag_psc_flags_validate ( cbx_lag_psc_t *psc )
{

    int            rv = CBX_E_NONE;
    uint32_t       test_flags;

    LAG_INIT;

    LOG_INFO(BSL_LS_FSAL_LAG,
             (BSL_META("FSAL API : cbxi_lag_psc_flags_validate()..\n")));

    test_flags = psc->flags;

    switch(psc->pkt_type) {
    /** other (non IPv4 or IPv6) packets */
    case cbxPacketOther:
        if((test_flags &= ~(CBX_PACKET_OTHER_PSC_PBMP))) {
            return CBX_E_PARAM;
        }
        break;
    /** IPv4 packets */
    case cbxPacketIpv4:
        if((test_flags &= ~(CBX_PACKET_IPV4_PSC_PBMP))) {
            return CBX_E_PARAM;
        }
        break;
    /** IPv6 packets */
    case cbxPacketIpv6:
        if((test_flags &= ~(CBX_PACKET_IPV6_PSC_PBMP))) {
            return CBX_E_PARAM;
        }
        break;
    default:
        break;
    }

    return rv;
}

/*
 * Function:
 *  cbxi_robo2_pp2lpg_slot_get
 * Purpose:
 *  Get free LPG ID to be allocated to pp.
 *
 */
int
cbxi_robo2_pp2lpg_slot_get( int unit, cbxi_pgid_t *lpgid )
{
    int i;
    int rv = CBX_E_FULL;

    for (i = 0; i < CBX_MAX_PORT_PER_UNIT; i++) {
        if (lpgt[unit][i] == FALSE) {
            /* Mark lpgt entry inuse and return */
            lpgt[unit][i] = TRUE;
            *lpgid = i;
            rv = CBX_E_NONE;
            break;
        }
    }
    return  rv;
}

/*
 * Function:
 *  cbxi_robo2_lpg2pg_slot_get
 * Purpose:
 *  Get free PG ID to be allocated to pp.
 *
 */
int
cbxi_robo2_lpg2pg_slot_get( int unit, cbxi_pgid_t *gpgid )
{
    int i;
    int rv = CBX_E_FULL;

    for (i = 0; i < MAX_PG; i++) {
        if (pgt[i] == FALSE) {
            /* Mark pgt entry inuse and return */
            pgt[i] = TRUE;
            *gpgid = i;
            rv = CBX_E_NONE;
            break;
        }
    }
    return  rv;
}

/*
 * Function:
 *  cbxi_lag_pg_context_clear
 * Purpose:
 *  Free LPG and PGID allocated for PP.
 *
 */
int
cbxi_lag_pg_context_clear(int unit, cbx_port_t lpgid, cbx_port_t pgid)
{
    /* Mark lpgt and pgt entry as free slot */
    lpgt[unit][lpgid] = FALSE;
    pgt[pgid] = FALSE;
    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_robo2_ipp_pp2lpg_update
 * Purpose:
 *  Update PP2LPG for the PP passed
 *
 */
int
cbxi_robo2_ipp_pp2lpg_update(int unit, int pgid,
                             cbx_port_t port)
{

    ipp_pp2lpg_t  pp2lpg_entry;
    uint32        status  = 0;

    /* Read entry from PP2LPG  */
    CBX_IF_ERROR_RETURN(soc_robo2_ipp_pp2lpg_get(unit, port, &pp2lpg_entry));
    pp2lpg_entry.pgid = pgid;
    CBX_IF_ERROR_RETURN(soc_robo2_ipp_pp2lpg_set(unit, port,
                        &pp2lpg_entry, &status));
    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_robo2_ikft_update
 * Purpose:
 *  Update IKFT entry at the provided field index
 *
 */
int
cbxi_robo2_ikft_update( int unit, int fld_index, int key_id )
{

    ikft_t        ikft_entry;
    uint32        status  = 0;
    uint8         IKFT_FLD_LEN[] = {
        CBX_LAG_IKFT_FIELD_ID_FO_LEN, /* F0 */
        CBX_LAG_IKFT_FIELD_ID_F1_LEN, /* F1 */
        CBX_LAG_IKFT_FIELD_ID_SPG_LEN, /* F2 */
        CBX_LAG_IKFT_FIELD_ID_F3_LEN, /* F3 */
        CBX_LAG_IKFT_FIELD_ID_DSTMAC_LEN, /* F4 */
        CBX_LAG_IKFT_FIELD_ID_SRCMAC_LEN, /* F5 */
        CBX_LAG_IKFT_FIELD_ID_OUTER_TCI_LEN,  /* F6 */
        CBX_LAG_IKFT_FIELD_ID_F7_LEN, /* F7 */
        CBX_LAG_IKFT_FIELD_ID_F8_LEN, /* F8 */
        CBX_LAG_IKFT_FIELD_ID_SRCIP_LEN,  /* F9 */
        CBX_LAG_IKFT_FIELD_ID_DSTIP_LEN,  /* F10 */
        CBX_LAG_IKFT_FIELD_ID_SRCIPV6_LEN,/* F11 */
        CBX_LAG_IKFT_FIELD_ID_DSTIPV6_LEN,/* F12 */
        CBX_LAG_IKFT_FIELD_ID_F13_LEN, /* F13 */
        CBX_LAG_IKFT_FIELD_ID_F14_LEN, /* F14 */
        CBX_LAG_IKFT_FIELD_ID_L3PROTO_LEN, /* F15 */
        CBX_LAG_IKFT_FIELD_ID_L4BYTES_LEN /* F16 */
    };

    uint8         IKFT_FLD_HDR_TYPE[] = {
        CBX_LAG_IKFT_FIELD_ID_F0_HDR_TYPE, /* F0 */
        CBX_LAG_IKFT_FIELD_ID_F1_HDR_TYPE, /* F1 */
        CBX_LAG_IKFT_FIELD_ID_SPG_HDR_TYPE, /* F2 */
        CBX_LAG_IKFT_FIELD_ID_F3_HDR_TYPE, /* F3 */
        CBX_LAG_IKFT_FIELD_ID_DSTMAC_HDR_TYPE, /* F4 */
        CBX_LAG_IKFT_FIELD_ID_SRCMAC_HDR_TYPE, /* F5 */
        CBX_LAG_IKFT_FIELD_ID_OUTER_TCI_HDR_TYPE,  /* F6 */
        CBX_LAG_IKFT_FIELD_ID_F7_HDR_TYPE, /* F7 */
        CBX_LAG_IKFT_FIELD_ID_F8_HDR_TYPE, /* F8 */
        CBX_LAG_IKFT_FIELD_ID_SRCIP_HDR_TYPE,  /* F9 */
        CBX_LAG_IKFT_FIELD_ID_DSTIP_HDR_TYPE,  /* F10 */
        CBX_LAG_IKFT_FIELD_ID_SRCIPV6_HDR_TYPE,/* F11 */
        CBX_LAG_IKFT_FIELD_ID_DSTIPV6_HDR_TYPE,/* F12 */
        CBX_LAG_IKFT_FIELD_ID_F13_HDR_TYPE, /* F13 */
        CBX_LAG_IKFT_FIELD_ID_F14_HDR_TYPE, /* F14 */
        CBX_LAG_IKFT_FIELD_ID_L3PROTO_HDR_TYPE, /* F15 */
        CBX_LAG_IKFT_FIELD_ID_L4BYTES_HDR_TYPE /* F16 */
    };

    /* Field offset replicated from Avenger Arch Spec */
    uint16         IKFT_FLD_HDR_OFFSET[] = {
        CBX_LAG_IKFT_FIELD_ID_F0_OFFSET,/* F0 */
        CBX_LAG_IKFT_FIELD_ID_F1_OFFSET,/* F1 */
        CBX_LAG_IKFT_FIELD_ID_SPG_OFFSET,/* F2 */
        CBX_LAG_IKFT_FIELD_ID_F3_OFFSET,/* F3 */
        CBX_LAG_IKFT_FIELD_ID_DSTMAC_OFFSET, /* F4 */
        CBX_LAG_IKFT_FIELD_ID_SRCMAC_OFFSET, /* F5 */
        CBX_LAG_IKFT_FIELD_ID_OUTER_TCI_OFFSET, /* F6 */
        CBX_LAG_IKFT_FIELD_ID_F7_OFFSET,/* F7 */
        CBX_LAG_IKFT_FIELD_ID_F8_OFFSET,/* F8 */
        CBX_LAG_IKFT_FIELD_ID_SRCIP_OFFSET, /* F9 */
        CBX_LAG_IKFT_FIELD_ID_DSTIP_OFFSET, /* F10 */
        CBX_LAG_IKFT_FIELD_ID_SRCIPV6_OFFSET, /* F11 */
        CBX_LAG_IKFT_FIELD_ID_DSTIPV6_OFFSET, /* F12 */
        CBX_LAG_IKFT_FIELD_ID_F13_OFFSET,/* F13 */
        CBX_LAG_IKFT_FIELD_ID_F14_OFFSET,/* F14 */
        CBX_LAG_IKFT_FIELD_ID_L3PROTO_OFFSET, /* F15 */
        CBX_LAG_IKFT_FIELD_ID_L4BYTES_OFFSET  /* F16 */
    };
    /* Read entry from IKFT */
    CBX_IF_ERROR_RETURN(soc_robo2_ikft_get(unit, fld_index, &ikft_entry));
    ikft_entry.key |= (1 << key_id);
    ikft_entry.length = IKFT_FLD_LEN[fld_index];
    ikft_entry.htype = IKFT_FLD_HDR_TYPE[fld_index];
    ikft_entry.offset = IKFT_FLD_HDR_OFFSET[fld_index];
    CBX_IF_ERROR_RETURN(soc_robo2_ikft_set(unit, fld_index,
                                           &ikft_entry, &status));
    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_robo2_ikft_clear
 * Purpose:
 *  Clear IKFT entry at the provided field index
 *
 */
int
cbxi_robo2_ikft_clear( int unit, int fld_index, int key_id )
{
    ikft_t        ikft_entry;
    uint32        status  = 0;

    /* Read entry from IKFT */
    CBX_IF_ERROR_RETURN(soc_robo2_ikft_get(unit, fld_index, &ikft_entry));
    ikft_entry.key &= ~(1 << key_id);
    CBX_IF_ERROR_RETURN(soc_robo2_ikft_set(unit, fld_index,
                                           &ikft_entry, &status));
    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_robo2_lpg2pg_update
 * Purpose:
 *  Update LPG2PG entry at the given LPG index
 *
 */
int
cbxi_robo2_lpg2pg_update( int unit, int lpgid, int gpgid )
{

    lpg2pg_t      lpg2pg_entry;
    uint32        status  = 0;

    /* Read entry from LPG2PG */
    CBX_IF_ERROR_RETURN(soc_robo2_lpg2pg_get(unit, lpgid, &lpg2pg_entry));
    lpg2pg_entry.pg = gpgid;
    CBX_IF_ERROR_RETURN(soc_robo2_lpg2pg_set(unit, lpgid,
                        &lpg2pg_entry, &status));
    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_robo2_pg2lpg_update
 * Purpose:
 *  Update PG2LPG for the PG passed
 *
 */
int
cbxi_robo2_pg2lpg_update( int unit, int gpgid, int lpgid )
{

    pg2lpg_t      pg2lpg_entry;
    uint32        status  = 0;

    /* Read entry from PG2LPG */
    CBX_IF_ERROR_RETURN(soc_robo2_pg2lpg_get(unit, gpgid, &pg2lpg_entry));
    pg2lpg_entry.lpg = lpgid;
    CBX_IF_ERROR_RETURN(soc_robo2_pg2lpg_set(unit, gpgid,
                        &pg2lpg_entry, &status));
    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_robo2_lag_svt_update
 * Purpose:
 *  Update SVT for the PP passed
 *
 */
int
cbxi_robo2_lag_svt_update( int unit,
                           pbmp_t lpbmp, cbx_port_t port_in, uint32_t flags )
{
    svt_t         svt_entry;
    uint32        status  = 0;
    pbmp_t        svt_mask = 0;
    cbx_port_t    port=0;
    pbmp_t        lpbmp_local = 0;
    int           index = 0;
    int           max_index;

    max_index =  soc_robo2_svt_max_index(unit);

    lpbmp_local = lpbmp;

    if (flags == CBXI_LAG_PORT_TABLE_ENTRY_REMOVE) {
        CBX_PBMP_PORT_REMOVE(lpbmp, port_in);
        if (lpbmp == 0) {
            lpbmp = lpbmp_local;
        }
    }

    for ( index = 0; index <= max_index;) {

        CBX_PBMP_ITER(lpbmp, port) {

            if (index > max_index) {
                break;
            }

            /* Read entry from SVT */
            CBX_IF_ERROR_RETURN(soc_robo2_svt_get(unit, index, &svt_entry));
            svt_mask = svt_entry.svt_mask;

            svt_mask &= ~lpbmp;
            CBX_PBMP_PORT_ADD(svt_mask, port);

            if (flags == CBXI_LAG_PORT_TABLE_ENTRY_REMOVE) {
                CBX_PBMP_PORT_ADD(svt_mask, port_in);
            }
            svt_entry.svt_mask = svt_mask & 0xFFFF;

            /* Update SVT entry */
            CBX_IF_ERROR_RETURN(soc_robo2_svt_set(unit, index,
                                                  &svt_entry, &status));
            index++;
        }
    }
    return CBX_E_NONE;
}

#ifdef  CONFIG_CASCADED_MODE
/*
 * Function:
 *  cbxi_robo2_lag_link_svt_update
 * Purpose:
 *
 */
int
cbxi_robo2_lag_link_svt_update( int unit,
                           pbmp_t lpbmp, cbx_port_t port_in, uint32_t flags )
{
    svt_t         svt_entry;
    uint32        status  = 0;
    pbmp_t        svt_mask = 0;
    int           index = 0;
    int           max_index;

    max_index =  soc_robo2_svt_max_index(unit);

    for ( index = 0; index <= max_index;) {

        if (index > max_index) {
            break;
        }

        /* Read entry from SVT */
        CBX_IF_ERROR_RETURN(soc_robo2_svt_get(unit, index, &svt_entry));
        svt_mask = svt_entry.svt_mask;

        if (flags == CBXI_LAG_PORT_TABLE_ENTRY_ADD) {
            CBX_PBMP_PORT_ADD(svt_mask, port_in);
        } else {
            CBX_PBMP_PORT_REMOVE(svt_mask, port_in);
        }
        svt_entry.svt_mask = svt_mask & 0xFFFF;

        /* Update SVT entry */
        CBX_IF_ERROR_RETURN(soc_robo2_svt_set(unit, index,
                                              &svt_entry, &status));
        index++;
    }
    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_robo2_lag_link_svt_update2
 * Purpose:
 *
 */
int
cbxi_robo2_lag_linkfail_svt_update( int unit, pbmp_t lpbmp )
{
    svt_t         svt_entry;
    uint32        status  = 0;
    pbmp_t        svt_mask = 0;
    cbx_port_t    port=0;
    int           index = 0;
    int           max_index;

    max_index =  soc_robo2_svt_max_index(unit);

    for ( index = 0; index <= max_index;) {

        if (index > max_index) {
            break;
        }

        CBX_PBMP_ITER(lpbmp, port) {
        /* Read entry from SVT */
        CBX_IF_ERROR_RETURN(soc_robo2_svt_get(unit, index, &svt_entry));
        svt_mask = svt_entry.svt_mask;
        svt_mask &= ~lpbmp;

        CBX_PBMP_PORT_ADD(svt_mask, port);

        svt_entry.svt_mask = svt_mask & 0xFFFF;

        /* Update SVT entry */
        CBX_IF_ERROR_RETURN(soc_robo2_svt_set(unit, index,
                                              &svt_entry, &status));
        index++;
        }
    }
    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_robo2_lag_svt_update
 * Purpose:
 *  Update SVT for the PP passed
 *
 */
int
cbxi_robo2_lag_cascade_svt_update( cbx_lag_info_t *lag_info )
{
    svt_t         svt_entry;
    uint32        status  = 0;
    pbmp_t        svt_mask = 0;
    pbmp_t        lpbmp = 0;
    pbmp_t        rpbmp = 0;
    cbx_port_t    port=0;
    int           unit = CBX_AVENGER_PRIMARY;
    int           index = 0;
    int           max_index;

    for(unit =0;unit<=CBX_AVENGER_SECONDARY;unit++)
    {
        CBX_PBMP_CLEAR(lpbmp);
        CBX_PBMP_CLEAR(rpbmp);

        if(unit == CBX_AVENGER_PRIMARY) {
            CBX_PBMP_ASSIGN(lpbmp, lag_info->lag_pbmp[CBX_AVENGER_PRIMARY]);
            CBX_PBMP_ASSIGN(rpbmp, lag_info->lag_pbmp[CBX_AVENGER_SECONDARY]);
        } else {
            CBX_PBMP_ASSIGN(rpbmp, lag_info->lag_pbmp[CBX_AVENGER_PRIMARY]);
            CBX_PBMP_ASSIGN(lpbmp, lag_info->lag_pbmp[CBX_AVENGER_SECONDARY]);
        }

        if (CBX_PBMP_IS_NULL(lpbmp)) {
            /* Nothing to be done */
            continue;
        }

        max_index =  soc_robo2_svt_max_index(unit);

        for ( index = 0; index <= max_index;) {

            if((unit == CBX_AVENGER_SECONDARY) &&
               (CBX_PBMP_NOT_NULL(rpbmp))) {

                CBX_PBMP_ITER(rpbmp, port) {

                    if (index > max_index) {
                        break;
                    }

                    /* Read entry from SVT */
                    CBX_IF_ERROR_RETURN(soc_robo2_svt_get(unit, index,
                                                          &svt_entry));
                    svt_mask = svt_entry.svt_mask;

                    svt_mask &= ~lpbmp;

                    svt_entry.svt_mask = svt_mask & 0xFFFF;

                    /* Update SVT entry */
                    CBX_IF_ERROR_RETURN(soc_robo2_svt_set(unit, index,
                                                          &svt_entry, &status));
                    index++;
                }
            }

            CBX_PBMP_ITER(lpbmp, port) {

                if (index > max_index) {
                    break;
                }

                /* Read entry from SVT */
                CBX_IF_ERROR_RETURN(soc_robo2_svt_get(unit, index, &svt_entry));
                svt_mask = svt_entry.svt_mask;

                svt_mask &= ~lpbmp;
                CBX_PBMP_PORT_ADD(svt_mask, port);

                svt_entry.svt_mask = svt_mask & 0xFFFF;

                /* Update SVT entry */
                CBX_IF_ERROR_RETURN(soc_robo2_svt_set(unit, index,
                                                      &svt_entry, &status));
                index++;
            }

            if((unit == CBX_AVENGER_PRIMARY) &&
               (CBX_PBMP_NOT_NULL(rpbmp))) {

                CBX_PBMP_ITER(rpbmp, port) {

                    if (index > max_index) {
                        break;
                    }

                    /* Read entry from SVT */
                    CBX_IF_ERROR_RETURN(soc_robo2_svt_get(unit, index,
                                                          &svt_entry));
                    svt_mask = svt_entry.svt_mask;

                    svt_mask &= ~lpbmp;

                    svt_entry.svt_mask = svt_mask & 0xFFFF;

                    /* Update SVT entry */
                    CBX_IF_ERROR_RETURN(soc_robo2_svt_set(unit, index,
                                                          &svt_entry, &status));
                    index++;
                }
            }
        }
    }
    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_robo2_lag_link_cascade_svt_update
 * Purpose:
 *  Update SVT for the PP passed
 *
 */
int
cbxi_robo2_lag_link_cascade_svt_update( pbmp_t in_lpbmp,
                                        pbmp_t in_rpbmp,
                                        pbmp_t *in_csd_pbmp,
                                        cbx_lag_mode_t  lag_mode)
{
    svt_t         svt_entry;
    uint32        status  = 0;
    pbmp_t        svt_mask = 0;
    pbmp_t        lpbmp = 0;
    pbmp_t        rpbmp = 0;
    cbx_port_t    port=0;
    int           unit = CBX_AVENGER_PRIMARY;
    int           index = 0;
    int           max_index;

    for(unit =0;unit<=CBX_AVENGER_SECONDARY;unit++)
    {
        CBX_PBMP_CLEAR(lpbmp);
        CBX_PBMP_CLEAR(rpbmp);

        if(unit == CBX_AVENGER_PRIMARY) {
            CBX_PBMP_ASSIGN(lpbmp, in_lpbmp);
            CBX_PBMP_ASSIGN(rpbmp, in_rpbmp);
        } else {
            CBX_PBMP_ASSIGN(lpbmp, in_rpbmp);
            CBX_PBMP_ASSIGN(rpbmp, in_lpbmp);
        }

        if (CBX_PBMP_IS_NULL(lpbmp)) {
            /* Nothing to be done */
            continue;
        }

        max_index =  soc_robo2_svt_max_index(unit);

        for ( index = 0; index <= max_index;) {

            if((unit == CBX_AVENGER_SECONDARY) &&
               ((lag_mode == cbxLagModeNormal) ||
                   (lag_mode == cbxLagModeShared)) &&
               (CBX_PBMP_NOT_NULL(in_csd_pbmp[unit])) &&
               (CBX_PBMP_NOT_NULL(rpbmp))) {

                CBX_PBMP_ITER(rpbmp, port) {

                    if (index > max_index) {
                        break;
                    }

                    /* Read entry from SVT */
                    CBX_IF_ERROR_RETURN(soc_robo2_svt_get(unit, index,
                                                          &svt_entry));
                    svt_mask = svt_entry.svt_mask;

                    svt_mask &= ~lpbmp;

                    svt_entry.svt_mask = svt_mask & 0xFFFF;

                    /* Update SVT entry */
                    CBX_IF_ERROR_RETURN(soc_robo2_svt_set(unit, index,
                                                          &svt_entry, &status));
                    index++;
                }
            }

            CBX_PBMP_ITER(lpbmp, port) {

                if (index > max_index) {
                    break;
                }

                /* Read entry from SVT */
                CBX_IF_ERROR_RETURN(soc_robo2_svt_get(unit, index, &svt_entry));
                svt_mask = svt_entry.svt_mask;

                svt_mask &= ~lpbmp;
                CBX_PBMP_PORT_ADD(svt_mask, port);

                svt_entry.svt_mask = svt_mask & 0xFFFF;

                /* Update SVT entry */
                CBX_IF_ERROR_RETURN(soc_robo2_svt_set(unit, index,
                                                      &svt_entry, &status));
                index++;
            }

            if((unit == CBX_AVENGER_PRIMARY) &&
              ((lag_mode == cbxLagModeNormal) ||
                   (lag_mode == cbxLagModeShared)) &&
               (CBX_PBMP_NOT_NULL(in_csd_pbmp[unit])) &&
               (CBX_PBMP_NOT_NULL(rpbmp))) {

                CBX_PBMP_ITER(rpbmp, port) {

                    if (index > max_index) {
                        break;
                    }

                    /* Read entry from SVT */
                    CBX_IF_ERROR_RETURN(soc_robo2_svt_get(unit, index,
                                                          &svt_entry));
                    svt_mask = svt_entry.svt_mask;

                    svt_mask &= ~lpbmp;

                    svt_entry.svt_mask = svt_mask & 0xFFFF;

                    /* Update SVT entry */
                    CBX_IF_ERROR_RETURN(soc_robo2_svt_set(unit, index,
                                                          &svt_entry, &status));
                    index++;
                }
            }
        }
    }
    return CBX_E_NONE;
}
#endif /* CONFIG_CASCADED_MODE */

/*
 * Function:
 *  cbxi_robo2_lag_get
 * Purpose:
 *  Get LAG information for requested lagid.
 *
 */
int
cbxi_robo2_lag_get( cbx_portid_t lagid, cbx_lag_info_t *lag_info )
{

    cbx_lag_info_t  *l_info;
    cbx_port_t     lag_out;
    int            rv = CBX_E_NOT_FOUND;

    LAG_INIT;

    LOG_INFO(BSL_LS_FSAL_LAG,
             (BSL_META("FSAL API : cbxi_robo2_lag_get()..\n")));

    /* Validate the lag index passed */
    rv = cbxi_robo2_portid_validate(lagid, &lag_out);
    if (CBX_FAILURE(rv)) {
        return CBX_E_PORT;
    }

    LAG_CHECK(lag_out);

    l_info = &LAG_INFO(lag_out);

    /* Check if lag exists */
    if ((l_info->in_use == TRUE) &&
            (l_info->lagid != CBX_LAG_INVALID) &&
            ((l_info->active[CBX_AVENGER_PRIMARY] >0) ||
            (l_info->active[CBX_AVENGER_SECONDARY] >0))) {
        LOG_INFO(BSL_LS_FSAL_LAG,
                 (BSL_META("FSAL API : cbxi_robo2_lag_get()..LAG id =%d \
                           exists \n"), l_info->lagid));
        sal_memcpy(lag_info, l_info, sizeof(cbx_lag_info_t));
        return CBX_E_NONE;
    }

    return CBX_E_NOT_FOUND;
}

/*
 * Function:
 *  cbxi_robo2_lag_member_check
 * Purpose:
 *  Check if portid is member of a LAG interface.
 *
 */
int
cbxi_robo2_lag_member_check( cbx_portid_t portid, cbx_portid_t *lagid )
{

    int  i=0;
    cbx_lag_info_t  *lag_info;
    cbx_port_t     port_out=CBX_PORT_INVALID;
    int            unit = CBX_AVENGER_PRIMARY;
    int            rv = CBX_E_NOT_FOUND;
    cbxi_pgid_t    lpg=CBXI_PGID_INVALID;


    LAG_INIT;

    LOG_INFO(BSL_LS_FSAL_LAG,
             (BSL_META("FSAL API : cbxi_robo2_lag_member_check()..\n")));
    if (CBX_PORTID_IS_TRUNK(portid)) {
        return CBX_E_PORT;
    }
    /* validate the port index passed */
    rv = cbxi_robo2_port_validate(portid, &port_out, &lpg, &unit);
    if (CBX_FAILURE(rv)) {
        return CBX_E_PORT;
    }

    /* Locate first entry */
    lag_info = &LAG_INFO(0);

    /* Check if port is member of a lag */
    for (i = 0; i < LAG_CNTL.ngroups; i++) {
        if ((lag_info->in_use == TRUE) &&
                (lag_info->lagid != CBX_LAG_INVALID) &&
                ((lag_info->active[CBX_AVENGER_PRIMARY] >0) ||
                (lag_info->active[CBX_AVENGER_SECONDARY] >0))) {
            if (CBX_PBMP_MEMBER(lag_info->lag_pbmp[unit], port_out)) {
                LOG_INFO(BSL_LS_FSAL_LAG,
                    (BSL_META("FSAL API : cbxi_robo2_lag_member_check()..port \
                              is member of lag id=%d.  \n"),lag_info->lagid));
                CBX_PORTID_TRUNK_SET(*lagid, lag_info->lagid);
                return CBX_E_NONE;
            }
        }
        lag_info++;
    }

    return CBX_E_NOT_FOUND;
}

#ifdef CONFIG_PORT_EXTENDER
/*
 * Function:
 *  cbxi_shared_lag_member_add
 * Purpose:
 *   Add port to shared LAG interface membership.
 *
 */
STATIC int
cbxi_shared_lag_member_add ( cbx_lag_info_t *lag_info,
                         cbx_port_t port_out,
                         int unit )
{
    ipp_pp2lpg_t ent_pp2lpg;
    lpg2pg_t     ent_lpg2pg;
    pg2lpg_t     ent_pg2lpg;
    int          port_lpg, lag_lpg;
    int          port_gpg;
#ifdef CONFIG_CASCADED_MODE
    cbx_port_t   csd_port;
    cbx_portid_t lagid;
#endif
    uint32_t     status = 0;

    CBX_IF_ERROR_RETURN(soc_robo2_ipp_pp2lpg_get(unit, port_out, &ent_pp2lpg));
    port_lpg = ent_pp2lpg.pgid;

    CBX_IF_ERROR_RETURN(soc_robo2_lpg2pg_get(unit, port_lpg, &ent_lpg2pg));
    port_gpg = ent_lpg2pg.pg;

#if 1 /* FIXME: Static allocation of GPGs */
    port_gpg = (port_lpg + (unit) * (CBX_MAX_PORT_PER_UNIT));
#endif

    if (lag_info->active[unit] == 0) {
        if(lag_info->port_one == CBXI_PGID_INVALID) {
            lag_info->port_one = ( (unit) * (CBX_MAX_PORT_PER_UNIT)) + port_out;
            lag_info->gpgid = port_gpg;
        }

        if(unit == CBX_AVENGER_PRIMARY ) {
            lag_info->lpgid = port_lpg;
            lag_lpg = lag_info->lpgid;
        } else {
            lag_info->rlpgid = port_lpg;
            lag_lpg = lag_info->rlpgid;
        }

        /* Update LAG LIN destination */
        CBX_IF_ERROR_RETURN(cbxi_pe_lag_dest_set(lag_info));

    } else {
        if(unit == CBX_AVENGER_PRIMARY ) {
            lag_lpg = lag_info->lpgid;
        } else {
            lag_lpg = lag_info->rlpgid;
        }
    }

    /* Add port as lag member */
    CBX_PBMP_PORT_ADD(lag_info->lag_pbmp[unit], port_out);
    lag_info->active[unit]++;

    /***************  Disable interfaces before update ************************/
    CBX_IF_ERROR_RETURN(cbxi_stg_port_state_set(
                unit, CBX_STG_DEFAULT, port_lpg, cbxStgStpStateDiscard));
    if (lag_info->lpgid != CBXI_PGID_INVALID) {
        CBX_IF_ERROR_RETURN(cbxi_stg_port_state_set(CBX_AVENGER_PRIMARY,
                CBX_STG_DEFAULT, lag_info->lpgid, cbxStgStpStateDiscard));
    }
    if (lag_info->rlpgid != CBXI_PGID_INVALID) {
        CBX_IF_ERROR_RETURN(cbxi_stg_port_state_set(CBX_AVENGER_SECONDARY,
                CBX_STG_DEFAULT, lag_info->rlpgid, cbxStgStpStateDiscard));
    }
    /**************************************************************************/


    /* Ingress ECID filtering fails if PQM_PP2LPG is not mapped to same LPG */
    if(unit == CBX_AVENGER_PRIMARY ) {
        CBX_IF_ERROR_RETURN(cbxi_robo2_pqm_pp2lpg_update(
                                        unit, lag_info->lpgid, port_out));
    } else {
        CBX_IF_ERROR_RETURN(cbxi_robo2_pqm_pp2lpg_update(
                                        unit, lag_info->rlpgid, port_out));
    }

    /* Configure appropriate LPG2PG and PG2LPG mappings */
    if (lag_info->lpgid != CBXI_PGID_INVALID) {
        ent_lpg2pg.pg = lag_info->gpgid;
        CBX_IF_ERROR_RETURN(soc_robo2_lpg2pg_set(CBX_AVENGER_PRIMARY,
                                        lag_info->lpgid, &ent_lpg2pg, &status));

        ent_pg2lpg.lpg = lag_info->lpgid;
        CBX_IF_ERROR_RETURN(soc_robo2_pg2lpg_set(CBX_AVENGER_PRIMARY,
                                        lag_info->gpgid, &ent_pg2lpg, &status));

    }
#ifdef CONFIG_CASCADED_MODE
    if ((SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) &&
                        (lag_info->rlpgid != CBXI_PGID_INVALID)) {
        ent_lpg2pg.pg = lag_info->gpgid;
        CBX_IF_ERROR_RETURN(soc_robo2_lpg2pg_set(CBX_AVENGER_SECONDARY,
                                        lag_info->rlpgid, &ent_lpg2pg, &status));

        ent_pg2lpg.lpg = lag_info->rlpgid;
        CBX_IF_ERROR_RETURN(soc_robo2_pg2lpg_set(CBX_AVENGER_SECONDARY,
                                        lag_info->gpgid, &ent_pg2lpg, &status));

    }
#endif

    /* Update SVT */
    CBX_IF_ERROR_RETURN(cbxi_robo2_lag_svt_update(unit, lag_info->lag_pbmp[unit],
                                port_out, CBXI_LAG_PORT_TABLE_ENTRY_ADD));

    /* Remove LAG port from original PG2PPFOV */
    CBX_IF_ERROR_RETURN(cbxi_robo2_lpg2ppfov_update(unit, port_lpg,
                                port_out, CBXI_LAG_PORT_TABLE_ENTRY_REMOVE));
    /* Remove LAG port from original SPGPPBMP config */
    CBX_IF_ERROR_RETURN(cbxi_robo2_spg2ppconfig_update(unit, port_lpg,
                                port_out, CBXI_LAG_PORT_TABLE_ENTRY_REMOVE));

    /* Add LAG port to LAG PG2PPFOV */
    CBX_IF_ERROR_RETURN(cbxi_robo2_lpg2ppfov_update(unit, lag_lpg,
                                port_out, CBXI_LAG_PORT_TABLE_ENTRY_ADD));
    /* Add LAG port to LAG SPGPPBMP config */
    CBX_IF_ERROR_RETURN(cbxi_robo2_spg2ppconfig_update(unit, lag_lpg,
                                port_out, CBXI_LAG_PORT_TABLE_ENTRY_ADD));

#ifdef CONFIG_CASCADED_MODE
    /* Update SVT info with Cascade port if necessary */
    CBX_IF_ERROR_RETURN(cbxi_robo2_lag_cascade_svt_update(lag_info));

    if(CBX_PBMP_NOT_NULL(lag_info->lag_pbmp[CBXI_CASC_PARTNER_UNIT(unit)])) {
        /* Add Cascade ports to PPFOV */
        CBX_PBMP_ITER(PBMP_CASCADE(CBX_AVENGER_PRIMARY), csd_port) {
            CBX_IF_ERROR_RETURN(cbxi_robo2_lpg2ppfov_update(CBX_AVENGER_PRIMARY,
                    lag_info->lpgid, csd_port, CBXI_LAG_PORT_TABLE_ENTRY_ADD));
        }
        CBX_PBMP_ITER(PBMP_CASCADE(CBX_AVENGER_SECONDARY), csd_port) {
            CBX_IF_ERROR_RETURN(cbxi_robo2_lpg2ppfov_update(CBX_AVENGER_SECONDARY,
                    lag_info->rlpgid, csd_port, CBXI_LAG_PORT_TABLE_ENTRY_ADD));
        }

    }

    /* If valid, configure both lpgid and rlpgid in all member MCAST groups */
    if ((lag_info->active[unit] == 1) &&
            (lag_info->active[CBXI_CASC_PARTNER_UNIT(unit)] > 0)) {
        CBX_PORTID_TRUNK_SET(lagid, lag_info->lagid);
        CBX_IF_ERROR_RETURN(cbxi_mcast_pe_shared_lag_update(lagid, lagid));
    }

#endif /* CONFIG_CASCADED_MODE */

    /***************  Enable interfaces after update ************************/
    CBX_IF_ERROR_RETURN(cbxi_stg_port_state_set(
            unit, CBX_STG_DEFAULT, port_lpg, cbxStgStpStateForward));
    if (lag_info->lpgid != CBXI_PGID_INVALID) {
        CBX_IF_ERROR_RETURN(cbxi_stg_port_state_set(CBX_AVENGER_PRIMARY,
                CBX_STG_DEFAULT, lag_info->lpgid, cbxStgStpStateForward));
    }
    if (lag_info->rlpgid != CBXI_PGID_INVALID) {
        CBX_IF_ERROR_RETURN(cbxi_stg_port_state_set(CBX_AVENGER_SECONDARY,
                CBX_STG_DEFAULT, lag_info->rlpgid, cbxStgStpStateForward));
    }
    /**************************************************************************/


    if (lag_info->lag_params.lag_mode == cbxLagModeShared) {
        CBX_IF_ERROR_RETURN(cbxi_pe_shared_lag_port_set(
                                        unit, lag_info->lagid, port_out, TRUE));
    }


    return CBX_E_NONE;
}


/*
 * Function:
 *  cbxi_shared_lag_member_remove
 * Purpose:
 *   Remove port from shared LAG interface membership.
 *
 */
STATIC int
cbxi_shared_lag_member_remove ( cbx_lag_info_t *lag_info,
                         cbx_port_t port_out,
                         int unit )
{
    cbx_portid_t lagid;
    ipp_pp2lpg_t ent_pp2lpg;
    lpg2pg_t     ent_lpg2pg;
    pg2lpg_t     ent_pg2lpg;
    int          port_lpg, lag_lpg, port_lpg_new;
    int          port_gpg;
    int          new_port;
#ifdef CONFIG_CASCADED_MODE
    cbx_port_t   csd_port;
#endif
    uint32_t     status = 0;

    CBX_IF_ERROR_RETURN(soc_robo2_ipp_pp2lpg_get(unit, port_out, &ent_pp2lpg));
    port_lpg = ent_pp2lpg.pgid;
    port_lpg_new = port_lpg;

    CBX_IF_ERROR_RETURN(soc_robo2_lpg2pg_get(unit, port_lpg, &ent_lpg2pg));
    port_gpg = ent_lpg2pg.pg;

#if 1 /* FIXME: Static allocation of GPGs */
    port_gpg = (port_lpg + (unit) * (CBX_MAX_PORT_PER_UNIT));
#endif

    lag_info->active[unit]--;

    /***************  Disable interfaces before update ************************/
    CBX_IF_ERROR_RETURN(cbxi_stg_port_state_set(
            unit, CBX_STG_DEFAULT, port_lpg, cbxStgStpStateDiscard));
    if (lag_info->lpgid != CBXI_PGID_INVALID) {
        CBX_IF_ERROR_RETURN(cbxi_stg_port_state_set(CBX_AVENGER_PRIMARY,
                CBX_STG_DEFAULT, lag_info->lpgid, cbxStgStpStateDiscard));
    }
    if (lag_info->rlpgid != CBXI_PGID_INVALID) {
        CBX_IF_ERROR_RETURN(cbxi_stg_port_state_set(CBX_AVENGER_SECONDARY,
                CBX_STG_DEFAULT, lag_info->rlpgid, cbxStgStpStateDiscard));
    }
    /**************************************************************************/


    /* Update SVT */
    CBX_IF_ERROR_RETURN(cbxi_robo2_lag_svt_update(unit, lag_info->lag_pbmp[unit],
                                port_out, CBXI_LAG_PORT_TABLE_ENTRY_REMOVE));

    /* Remove deleted port from LAG pbmp */
    CBX_PBMP_PORT_REMOVE(lag_info->lag_pbmp[unit], port_out);

    if (unit == CBX_AVENGER_PRIMARY) {
        lag_lpg = lag_info->lpgid;
    } else {
        lag_lpg = lag_info->rlpgid;
    }

    /* Remove LAG port to LAG PG2PPFOV */
    CBX_IF_ERROR_RETURN(cbxi_robo2_lpg2ppfov_update(unit, lag_lpg,
                                port_out, CBXI_LAG_PORT_TABLE_ENTRY_REMOVE));
    /* Remove LAG port to LAG SPGPPBMP config */
    CBX_IF_ERROR_RETURN(cbxi_robo2_spg2ppconfig_update(unit, lag_lpg,
                                port_out, CBXI_LAG_PORT_TABLE_ENTRY_REMOVE));

    /* Remove all LAG ports from original port LPG2PPFOV */
    CBX_PBMP_ITER(lag_info->lag_pbmp[unit], new_port) {
        CBX_IF_ERROR_RETURN(cbxi_robo2_lpg2ppfov_update(unit, port_lpg,
                                    new_port, CBXI_LAG_PORT_TABLE_ENTRY_REMOVE));
        CBX_IF_ERROR_RETURN(cbxi_robo2_spg2ppconfig_update(unit, port_lpg,
                                    port_out, CBXI_LAG_PORT_TABLE_ENTRY_REMOVE));
    }

    /* Add LAG port to original PG2PPFOV */
    CBX_IF_ERROR_RETURN(cbxi_robo2_lpg2ppfov_update(unit, port_lpg,
                                port_out, CBXI_LAG_PORT_TABLE_ENTRY_ADD));
    /* Add LAG port to original SPGPPBMP config */
    CBX_IF_ERROR_RETURN(cbxi_robo2_spg2ppconfig_update(unit, port_lpg,
                                port_out, CBXI_LAG_PORT_TABLE_ENTRY_ADD));
#ifdef CONFIG_CASCADED_MODE
    CBX_PBMP_ITER(PBMP_CASCADE(unit), csd_port) {
        /* Remove Cascade port from Port LPG2PPFOV */
        CBX_IF_ERROR_RETURN(cbxi_robo2_lpg2ppfov_update(unit, port_lpg,
                                 csd_port, CBXI_LAG_PORT_TABLE_ENTRY_REMOVE));
    }
#endif

    /* Map PQM_PP2LPG back to original LPG */
    CBX_IF_ERROR_RETURN(cbxi_robo2_pqm_pp2lpg_update(
                                        unit, port_lpg, port_out));


    CBX_PORTID_TRUNK_SET(lagid, lag_info->lagid);

    if (lag_info->active[unit] == 0) {
#ifdef CONFIG_CASCADED_MODE
        CBX_PBMP_ITER(PBMP_CASCADE(CBX_AVENGER_PRIMARY), csd_port) {
        /* Remove Cascade port from Port LPG2PPFOV */
            CBX_IF_ERROR_RETURN(cbxi_robo2_lpg2ppfov_update(CBX_AVENGER_PRIMARY,
                lag_info->lpgid, csd_port, CBXI_LAG_PORT_TABLE_ENTRY_REMOVE));
        }
        CBX_PBMP_ITER(PBMP_CASCADE(CBX_AVENGER_SECONDARY), csd_port) {
        /* Remove Cascade port from Port LPG2PPFOV */
            CBX_IF_ERROR_RETURN(cbxi_robo2_lpg2ppfov_update(CBX_AVENGER_SECONDARY,
                lag_info->rlpgid, csd_port, CBXI_LAG_PORT_TABLE_ENTRY_REMOVE));
        }

#endif

        if(unit == CBX_AVENGER_PRIMARY) {
            lag_info->lpgid  = CBXI_PGID_INVALID;
            lag_info->gpgid  = CBXI_PGID_INVALID;
        } else {
            lag_info->rlpgid  = CBXI_PGID_INVALID;
            lag_info->rgpgid  = CBXI_PGID_INVALID;
        }

        /* Restore LPG2PG and PG2LPG mapping */
        ent_lpg2pg.pg = port_gpg;
        CBX_IF_ERROR_RETURN(soc_robo2_lpg2pg_set(
                                unit, port_lpg, &ent_lpg2pg, &status));

        ent_pg2lpg.lpg = port_lpg;
        CBX_IF_ERROR_RETURN(soc_robo2_pg2lpg_set(unit,
                                        port_gpg, &ent_pg2lpg, &status));

        /* Update LAG LIN destination */
        CBX_IF_ERROR_RETURN(cbxi_pe_lag_dest_set(lag_info));

#ifdef CONFIG_CASCADED_MODE
        if (lag_info->active[CBXI_CASC_PARTNER_UNIT(unit)] != 0) {
            /* Update Shared LAG info on all member MCAST groups if there are
             * members in partner Avenger*/
            CBX_IF_ERROR_RETURN(cbxi_mcast_pe_shared_lag_update(
                                lagid, CBXI_GLOBAL_PORT(port_out, unit)));
        } else {
            /* Just remove the port from MCAST groups,
             * Don't add any LAG back (CBX_LAG_INVALID) */
            CBX_IF_ERROR_RETURN(cbxi_mcast_pe_shared_lag_update(
                            CBX_LAG_INVALID, CBXI_GLOBAL_PORT(port_out, unit)));

            lag_info->port_one = CBXI_PGID_INVALID;
        }
#endif /* CONFIG_CASCADED_MODE */
    } else if (lag_lpg == port_lpg) {
        /* Change LAG lpgid to next member in LAG */
        CBX_PBMP_ITER(lag_info->lag_pbmp[unit], new_port) {
            CBX_IF_ERROR_RETURN(soc_robo2_ipp_pp2lpg_get(
                                        unit, new_port, &ent_pp2lpg));
            port_lpg_new = ent_pp2lpg.pgid;

            CBX_IF_ERROR_RETURN(soc_robo2_lpg2pg_get(
                                        unit, port_lpg_new, &ent_lpg2pg));
            port_gpg = ent_lpg2pg.pg;

#if 1 /* FIXME: Static allocation of GPGs */
            port_gpg = (port_lpg_new + (unit) * (CBX_MAX_PORT_PER_UNIT));
#endif

            lag_info->gpgid = port_gpg;
            if(unit == CBX_AVENGER_PRIMARY ) {
                lag_info->lpgid = port_lpg_new;
                lag_lpg = lag_info->lpgid;
            } else {
                lag_info->rlpgid = port_lpg_new;
                lag_lpg = lag_info->rlpgid;
            }
            /***********  Disable interface before update ********************/
            CBX_IF_ERROR_RETURN(cbxi_stg_port_state_set(
                unit, CBX_STG_DEFAULT, port_lpg_new, cbxStgStpStateDiscard));
            /******************************************************************/

            /* Update LAG LIN destination */
            CBX_IF_ERROR_RETURN(cbxi_pe_lag_dest_set(lag_info));

            /* Update Shared LAG info on all member MCAST groups */
            CBX_IF_ERROR_RETURN(cbxi_mcast_pe_shared_lag_update(
                                lagid, CBXI_GLOBAL_PORT(port_out, unit)));
            break;
        }

        /* Add all ports to LPG2PPFOV */
        CBX_PBMP_ITER(lag_info->lag_pbmp[unit], new_port) {
            CBX_IF_ERROR_RETURN(cbxi_robo2_lpg2ppfov_update(unit, port_lpg_new,
                                    new_port, CBXI_LAG_PORT_TABLE_ENTRY_ADD));
            CBX_IF_ERROR_RETURN(cbxi_robo2_spg2ppconfig_update(unit, port_lpg_new,
                                    port_out, CBXI_LAG_PORT_TABLE_ENTRY_ADD));
        }

    /* Configure appropriate LPG2PG and PG2LPG mappings */
    if (lag_info->lpgid != CBXI_PGID_INVALID) {
        ent_lpg2pg.pg = lag_info->gpgid;
        CBX_IF_ERROR_RETURN(soc_robo2_lpg2pg_set(CBX_AVENGER_PRIMARY,
                                        lag_info->lpgid, &ent_lpg2pg, &status));

        ent_pg2lpg.lpg = lag_info->lpgid;
        CBX_IF_ERROR_RETURN(soc_robo2_pg2lpg_set(CBX_AVENGER_PRIMARY,
                                        lag_info->gpgid, &ent_pg2lpg, &status));

    }
#ifdef CONFIG_CASCADED_MODE
    if ((SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) &&
                        (lag_info->rlpgid != CBXI_PGID_INVALID)) {
        ent_lpg2pg.pg = lag_info->gpgid;
        CBX_IF_ERROR_RETURN(soc_robo2_lpg2pg_set(CBX_AVENGER_SECONDARY,
                                        lag_info->rlpgid, &ent_lpg2pg, &status));

        ent_pg2lpg.lpg = lag_info->rlpgid;
        CBX_IF_ERROR_RETURN(soc_robo2_pg2lpg_set(CBX_AVENGER_SECONDARY,
                                        lag_info->gpgid, &ent_pg2lpg, &status));

    }
#endif


#ifdef CONFIG_CASCADED_MODE
        if(CBX_PBMP_NOT_NULL(lag_info->lag_pbmp[CBXI_CASC_PARTNER_UNIT(unit)])) {
            /* Add Cascade ports to PPFOV */
            CBX_PBMP_ITER(PBMP_CASCADE(unit), csd_port) {
                CBX_IF_ERROR_RETURN(cbxi_robo2_lpg2ppfov_update(
                    unit, lag_lpg, csd_port, CBXI_LAG_PORT_TABLE_ENTRY_ADD));
            }
        }
#endif /* CONFIG_CASCADED_MODE */
    }


#ifdef CONFIG_CASCADED_MODE
    /* Update SVT info with Cascade port if necessary */
    CBX_IF_ERROR_RETURN(cbxi_robo2_lag_cascade_svt_update(lag_info));
#endif /* CONFIG_CASCADED_MODE */

    /***************  Enable interfaces after update ************************/
    CBX_IF_ERROR_RETURN(cbxi_stg_port_state_set(
            unit, CBX_STG_DEFAULT, port_lpg, cbxStgStpStateForward));
    CBX_IF_ERROR_RETURN(cbxi_stg_port_state_set(
            unit, CBX_STG_DEFAULT, port_lpg_new, cbxStgStpStateForward));

    if (lag_info->lpgid != CBXI_PGID_INVALID) {
        CBX_IF_ERROR_RETURN(cbxi_stg_port_state_set(CBX_AVENGER_PRIMARY,
                CBX_STG_DEFAULT, lag_info->lpgid, cbxStgStpStateForward));
    }
    if (lag_info->rlpgid != CBXI_PGID_INVALID) {
        CBX_IF_ERROR_RETURN(cbxi_stg_port_state_set(CBX_AVENGER_SECONDARY,
                CBX_STG_DEFAULT, lag_info->rlpgid, cbxStgStpStateForward));
    }
    /**************************************************************************/



    if (lag_info->lag_params.lag_mode == cbxLagModeShared) {
        CBX_IF_ERROR_RETURN(cbxi_pe_shared_lag_port_set(
                                    unit, lag_info->lagid, port_out, FALSE));
    }

    return CBX_E_NONE;
}
#endif /* CONFIG_PORT_EXTENDER */

/*
 * Function:
 *  cbxi_robo2_lag_member_remove
 * Purpose:
 *   Remove portid from LAG interface membership.
 *
 */
STATIC int
cbxi_lag_member_remove ( cbx_lag_info_t *lag_info,
                         cbx_port_t port_out,
                         int unit )
{
    uint32_t       flag=0;
    int            rv = CBX_E_NONE;
    cbxi_pgid_t    lpgid = CBXI_PGID_INVALID;
    cbxi_pgid_t    gpgid = CBXI_PGID_INVALID;
    cbxi_pgid_t    tlpgid;
#ifdef CONFIG_CASCADED_MODE
    int            tunit;
    cbx_port_t     csd_port;
    cbxi_pgid_t    csd_lpgid = CBXI_PGID_INVALID;
    lpg2pg_t       oldlpg2pg_entry;
#endif

    flag = CBXI_LAG_PORT_TABLE_ENTRY_REMOVE;

    LOG_INFO(BSL_LS_FSAL_LAG,
             (BSL_META("FSAL API : cbxi_lag_member_remove()..\n")));

    if ((lag_info->in_use == FALSE) ||
            (lag_info->active[unit] == 0)) {
        LOG_ERROR(BSL_LS_FSAL_LAG,
                  (BSL_META("FSAL API : cbxi_lag_member_remove()..invalid \
                             LAG \n")));
        return CBX_E_PARAM;
    }

    /* Check if port is member of LAG */
    if (!(CBX_PBMP_MEMBER(lag_info->lag_pbmp[unit], port_out))) {
        LOG_ERROR(BSL_LS_FSAL_LAG,
                  (BSL_META("FSAL API : cbxi_lag_member_remove()..port is \
               not member of lag id=%d.  \n"),lag_info->lagid));
        return CBX_E_PARAM;
    }

#ifdef CONFIG_PORT_EXTENDER
    if (lag_info->lag_params.lag_mode == cbxLagModeShared) {
        CBX_IF_ERROR_RETURN(cbxi_shared_lag_member_remove(lag_info, port_out, unit));
        return CBX_E_NONE;
    }
#endif

    lag_info->active[unit]--;

    if(lag_info->active[unit] != 0) {
        /* Get LPGID to allocate for the deleted port */
        rv = cbxi_robo2_pp2lpg_slot_get(unit, &lpgid);
        if (CBX_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_FSAL_LAG,
                  (BSL_META("FSAL API : cbxi_lag_memmber_remove()..Failed to \
                             find free LPGID slot \n")));
            return rv;
        }

        /* Get PGID to allocate for the deleted port */
        rv = cbxi_robo2_lpg2pg_slot_get(unit, &gpgid);
        if (CBX_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_FSAL_LAG,
                   (BSL_META("FSAL API : cbxi_lag_memmber_remove()..Failed to \
                                 find free PGID slot \n")));
            return rv;
        }
    } else {
        if(unit == CBX_AVENGER_PRIMARY) {
            lpgid = lag_info->lpgid;
#ifdef CONFIG_CASCADED_MODE
            tunit = CBX_AVENGER_SECONDARY;
            if(lag_info->lgpgid != CBXI_PGID_INVALID) {
                gpgid = lag_info->lgpgid;
            }
#else
            gpgid = lag_info->gpgid;
#endif /* CONFIG_CASCADED_MODE */
        } else {
            lpgid = lag_info->rlpgid;
#ifdef CONFIG_CASCADED_MODE
            tunit = CBX_AVENGER_PRIMARY;
            if(lag_info->rgpgid != CBXI_PGID_INVALID) {
                gpgid = lag_info->rgpgid;
            }
#else
            gpgid = lag_info->gpgid;
#endif /* CONFIG_CASCADED_MODE */
        }

#ifdef CONFIG_CASCADED_MODE
    if (lag_info->active[tunit] != 0) {
        /* Get PGID to allocate for the deleted port */
        rv = cbxi_robo2_lpg2pg_slot_get(unit, &gpgid);
        if (CBX_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_FSAL_LAG,
                   (BSL_META("FSAL API : cbxi_lag_memmber_remove()..Failed to \
                                 find free PGID slot \n")));
            return rv;
        }
    }
#endif /* CONFIG_CASCADED_MODE */
    }

    /* Add deleted port to PG2PPFOV corresponding to port LPG */
    rv = cbxi_robo2_lpg2ppfov_update(
             unit, lpgid, port_out, CBXI_LAG_PORT_TABLE_ENTRY_ADD);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_LAG,
                  (BSL_META("FSAL API : cbxi_lag_memmber_remove()..Failed to \
                             update PG2PPFOV entry \n")));
        return rv;
    }

#ifdef CONFIG_CASCADED_MODE
    CBX_PBMP_ITER(PBMP_CASCADE(unit), csd_port) {
        rv = cbxi_robo2_lpg2ppfov_update(unit, lpgid,
                                         csd_port, flag);
        if (CBX_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_FSAL_LAG,
                      (BSL_META("FSAL API : cbx_lag_member_remove()..Failed to\
                                  update PG2PPFOV entry \n")));
            return rv;
        }
    }
#endif /* CONFIG_CASCADED_MODE */

    /* Add deleted port to SPGPPBMP corresponding to port LPG */
    rv = cbxi_robo2_spg2ppconfig_update(unit, lpgid,
                                     port_out, CBXI_LAG_PORT_TABLE_ENTRY_ADD);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_LAG,
                  (BSL_META("FSAL API : cbxi_lag_memmber_remove()..Failed to \
                             update SPGPPBMP config entry \n")));
        return rv;
    }


    /* Update newly allocated LPGID in IPP PP2LPG */
    rv = cbxi_robo2_ipp_pp2lpg_update(unit, lpgid, port_out);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_LAG,
                  (BSL_META("FSAL API : cbxi_lag_memmber_remove()..Failed to \
                             update PP2PGID entry \n")));
        return rv;
    }

    /* Update newly allocated LPGID in PQM PP2LPG */
    rv = cbxi_robo2_pqm_pp2lpg_update(unit, lpgid, port_out);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_LAG,
                  (BSL_META("FSAL API : cbxi_lag_memmber_remove()..Failed to \
                             update PP2PGID entry \n")));
        return rv;
    }

#ifdef CONFIG_CASCADED_MODE
    /* Read existing lpg2pg map for removed port from LPG2PG table */
    CBX_IF_ERROR_RETURN(soc_robo2_lpg2pg_get(unit, lpgid,
                        &oldlpg2pg_entry));

    if (gpgid != oldlpg2pg_entry.pg) {
        /* Get cascade port lpg */
        CBX_IF_ERROR_RETURN(cbxi_robo2_cascade_port_lpg_get( unit, &csd_lpgid));

        /* Update PG2LPGID to cascade port */
        rv = cbxi_robo2_pg2lpg_update(unit, oldlpg2pg_entry.pg, csd_lpgid);
        if (CBX_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_FSAL_LAG,
                      (BSL_META("FSAL API: cbxi_lag_member_remove()..Failed to \
                                 update PG2LPGID entry with cascade LPG \n")));
            return rv;
        }
    }
#endif /* CONFIG_CASCADED_MODE */

    /* Update LPG2PGID */
    rv = cbxi_robo2_lpg2pg_update(unit, lpgid, gpgid);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_LAG,
                  (BSL_META("FSAL API : cbxi_lag_member_remove()..Failed to \
                             update LPG2PGID entry \n")));
        return rv;
    }

    /* Update PG2LPGID */
    rv = cbxi_robo2_pg2lpg_update(unit, gpgid, lpgid);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_LAG,
                  (BSL_META("FSAL API : cbxi_lag_member_remove()..Failed to \
                             update PG2LPGID entry \n")));
        return rv;
    }

    tlpgid = (unit == CBX_AVENGER_PRIMARY) ? lag_info->lpgid:lag_info->rlpgid;
    if (lpgid != tlpgid) {
        /* Remove deleted port from PG2PPFOV corresponding to LAG LPG */
        rv = cbxi_robo2_lpg2ppfov_update(unit, tlpgid, port_out, flag);
        if (CBX_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_FSAL_LAG,
                      (BSL_META("FSAL API : cbxi_lag_memmber_remove()..\
                                 Failed to update PG2PPFOV entry \n")));
            return rv;
        }

        /* Remove deleted port to SPGPPBMP corresponding to LAG LPG */
        rv = cbxi_robo2_spg2ppconfig_update(unit, tlpgid,
                                             port_out, flag);
        if (CBX_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_FSAL_LAG,
                      (BSL_META("FSAL API : cbxi_lag_memmber_remove()..\
                                 Failed to update SPGPPBMP config entry \n")));
            return rv;
        }
   }

    /* Update SVT */
    rv = cbxi_robo2_lag_svt_update(unit,
                                   lag_info->lag_pbmp[unit], port_out, flag);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_LAG,
                  (BSL_META("FSAL API : cbxi_lag_memmber_remove()..Failed to \
                             update SVT entry \n")));
        return rv;
    }

    /* Restore PGT for deleted port */
    rv = cbxi_port_pgt_context_recover(unit, port_out, lpgid);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_LAG,
                  (BSL_META("FSAL API : cbxi_lag_memmber_remove()..Failed to \
                             restore PGT for deleted port \n")));
        return rv;
    }
    /* Remove deleted port from LAG pbmp */
    CBX_PBMP_PORT_REMOVE(lag_info->lag_pbmp[unit], port_out);

#ifdef CONFIG_CASCADED_MODE
    if (lag_info->lag_params.lag_mode == cbxLagModeNormal) {
        rv = cbxi_robo2_lag_cascade_svt_update( lag_info );
        if (CBX_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_FSAL_LAG,
                      (BSL_META("FSAL API : cbx_lag_member_remove()..Failed to \
                                  update SVT entry in cascaded mode \n")));
            return rv;
        }
    }
#endif /* CONFIG_CASCADED_MODE */

    LOG_INFO(BSL_LS_FSAL_LAG,
             (BSL_META("FSAL API : cbxi_lag_member_remove()..LAG id =%d \
                       portid=%d, lpgid=%d, gpgid=%d \n"),
              lag_info->lagid, port_out,
              lpgid, gpgid));
    /* Debug routines */
    cbxi_lag_info_dump ();
    cbxi_lag_member_table_dump (port_out, lpgid);

    if (lag_info->active[unit] == 0) {
        if(unit == CBX_AVENGER_PRIMARY) {
            lag_info->lpgid  = CBXI_PGID_INVALID;
            lag_info->gpgid  = CBXI_PGID_INVALID;
        } else {
            lag_info->rlpgid  = CBXI_PGID_INVALID;
            lag_info->rgpgid  = CBXI_PGID_INVALID;
        }
    }

#ifdef CONFIG_PORT_EXTENDER
    /* Update LAG LIN destination */
    CBX_IF_ERROR_RETURN(cbxi_pe_lag_dest_set(lag_info));
#endif

    return rv;
}

/*
 * Function:
 *  cbxi_lag_kst_table_update
 * Purpose:
 *  KST table initialization for lag LBH entries.
 *
 */
STATIC int
cbxi_lag_kst_table_update( int unit )
{

    int           index = 0;
    kst_t         kst_entry;
    uint32        status  = 0;

    for (index=CBXI_SLIC_LAG_CFP_KEYSET_OTHER;
         index<=CBXI_SLIC_LAG_CFP_KEYSET_IPV6;
         index++) {

        /* KST_entry   lbh_en  lbhid   Description
         * 0           1        8      non-IP
         * 1           1        9      IPv4
         * 2           1       10      IPv6
         */
        CBX_IF_ERROR_RETURN(soc_robo2_kst_get(unit, index, &kst_entry));

        /* Add LBH entries for default packet types IPv4, IPv6 and non-IP */
        switch(index) {
        /* non-IP packets */
        case CBXI_SLIC_LAG_CFP_KEYSET_OTHER:
            kst_entry.lbh_key = CBX_LAG_LBH_OTHER_KEY_ID;
            break;
        /* IPv4 packets */
        case CBXI_SLIC_LAG_CFP_KEYSET_IPV4:
            kst_entry.lbh_key = CBX_LAG_LBH_IPv4_KEY_ID;
            break;
        /* IPv6 packets */
        case CBXI_SLIC_LAG_CFP_KEYSET_IPV6:
            kst_entry.lbh_key = CBX_LAG_LBH_IPv6_KEY_ID;
            break;
        default:
            return CBX_E_BADID;
            break;
        }
        kst_entry.lbh_en = 1; /* Set lbh_en bit for the entry */
        CBX_IF_ERROR_RETURN(soc_robo2_kst_set(unit, index,
                                              &kst_entry, &status));
    }
    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_lag_fpslict_table_update
 * Purpose:
 *  FPSLICT table update for lag LBH entries.
 *
 */
STATIC int
cbxi_lag_fpslict_table_update( int unit, int fpslict_index )
{

    fpslict_t     fpslict_entry;
    uint32        status  = 0;

    CBX_IF_ERROR_RETURN(soc_robo2_fpslict_get(unit, fpslict_index,
                        &fpslict_entry));
    fpslict_entry.other_keyset = CBXI_SLIC_LAG_CFP_KEYSET_OTHER;
    fpslict_entry.ipv4_keyset  = CBXI_SLIC_LAG_CFP_KEYSET_IPV4;
    fpslict_entry.ipv6_keyset  = CBXI_SLIC_LAG_CFP_KEYSET_IPV6;

    CBX_IF_ERROR_RETURN(soc_robo2_fpslict_set(unit, fpslict_index,
                        &fpslict_entry, &status));
    return CBX_E_NONE;
}

/*
 * Function:
 *  cbxi_lag_table_init
 * Purpose:
 *  Global Initialization for all lag related tables.
 *
 */
STATIC int
cbxi_lag_table_init( void )
{

    uint32        num_units = 1;
    int           i = 0;

    /* Check if it is cascade set-up */
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = CBX_MAX_UNITS;
    }

    for ( i = 0; i < num_units; i++ ) {

        /* Enable and reset KST */
        CBX_IF_ERROR_RETURN(soc_robo2_kst_enable(i));
        CBX_IF_ERROR_RETURN(soc_robo2_kst_reset(i));

        /* KST table entries for lag LBH configured with fixed keys
         * during init.
         */
        CBX_IF_ERROR_RETURN(cbxi_lag_kst_table_update(i));


        /* Enable and reset IKFT */
        CBX_IF_ERROR_RETURN(soc_robo2_ikft_enable(i));
        CBX_IF_ERROR_RETURN(soc_robo2_ikft_reset(i));
    }
    return CBX_E_NONE;
}


void cbxi_lag_info_dump ( )
{
#ifdef LAG_DEBUG_VERBOSE
    int  i=0;
    int  unit = CBX_AVENGER_PRIMARY;
    cbx_lag_info_t  *lag_info;

    /* Locate first entry */
    lag_info = &LAG_INFO(0);

    sal_printf("\nS.No  LAGID   in_use  active  LPGID    RLPGID\
    GPGID    lag_pbmp0    lag_pbmp1\n");
    sal_printf("================================================================================\n");
    for (i = 0; i < LAG_CNTL.ngroups; i++) {
        if ((lag_info->in_use == TRUE) &&
                (lag_info->lagid != CBX_LAG_INVALID)) {
            sal_printf("%d    0x%x       %d       %d     0x%x       0x%x          0x%x     0x%x       0x%x\n",
                       i,lag_info->lagid, lag_info->in_use,
                       lag_info->active[0], lag_info->lpgid,
                       lag_info->rlpgid, lag_info->gpgid,
                       lag_info->lag_pbmp[0], lag_info->lag_pbmp[1]);

        }
        lag_info++;
    }
    sal_printf("================================================================================\n");
#endif /* LAG_DEBUG_VERBOSE */
    return;
}

void cbxi_lag_member_table_dump ( cbx_port_t  port, int lpgid )
{
#ifdef LAG_DEBUG_VERBOSE
    int  unit = CBX_AVENGER_PRIMARY;
    lpg2pg_t       lpg2pg_entry;
    int rv =0;

    /* Read entry from LPG2PG */
    rv = soc_robo2_lpg2pg_get(unit, lpgid, &lpg2pg_entry);

    sal_printf("Port =%d, LPG = 0x%x GPG = 0x%x \n",
               port, lpgid, lpg2pg_entry.pg);

    sal_printf("/******** Start LAG member table dumps *******");

    /* Dump IPP-PP2LPG table entry */
    dump_table(ROBO2_TABLE_IPP_PP2LPG_ID, port, 1);

    /* Dump PQM-PP2LPG table entry */
    dump_table(ROBO2_TABLE_PQM_PP2LPG_ID, port, 1);

    /* Dump LPG2PPFOV table entry */
    dump_table(ROBO2_TABLE_LPG2PPFOV_ID, lpgid, 1);

    /* Dump LPG2PG table entry */
    dump_table(ROBO2_TABLE_LPG2PG_ID, lpgid, 1);

    /* Dump PG2LPG table entry */
    dump_table(ROBO2_TABLE_PG2LPG_ID, lpg2pg_entry.pg, 1);
#if 0
    /* Dump all SVT table entries */
    dump_table(ROBO2_TABLE_SVT_ID, 0,
               soc_robo2_svt_max_index(unit));
#endif

    sal_printf("/******** End LAG member table dumps *******");
#endif /* LAG_DEBUG_VERBOSE */

    return;
}


/*
 * Function:
 *  cbxi_robo2_lag_link_status_get
 * Purpose:
 *  Get Link status for requested lagid.
 *  Return Link status UP even if any one of the member port is up.
 *
 */
int
cbxi_robo2_lag_link_status_get( cbx_portid_t lagid, uint32_t *link_sts)
{
    cbx_lag_info_t *l_info;
    cbx_port_t     port;
    cbx_port_t     lag_out;
    int            unit = CBX_AVENGER_PRIMARY;
    uint32         num_units = 1;
    int            rv;

    /* Validate the lag index passed */
    rv = cbxi_robo2_portid_validate(lagid, &lag_out);
    if (CBX_FAILURE(rv)) {
        return CBX_E_PORT;
    }

    LAG_CHECK(lag_out);

    l_info = &LAG_INFO(lag_out);

    /* Check if it is cascade set-up */
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = CBX_MAX_UNITS;
    }

    for(unit = 0; unit < num_units; unit++) {
        CBX_PBMP_ITER(l_info->lag_pbmp[unit], port) {
             CBX_IF_ERROR_RETURN(cbxi_port_link_status_get(
                                                unit, port, link_sts));
             if (*link_sts) {
                 return CBX_E_NONE;
             }
        }
    }
    return CBX_E_NONE;
}

#ifdef CONFIG_CASCADED_MODE
/*
 * Function:
 *  cbxi_robo2_lag_link_status_update
 * Purpose:
 *
 */
int
cbxi_robo2_lag_link_status_update( cbx_port_t gport,
                                   int  link_status,
                                   cbx_portid_t lagid )
{
    cbx_lag_info_t *l_info;
    cbx_lag_info_t *lag_info;
    cbx_port_t     port_out;
    cbx_port_t     port;
    cbxi_pgid_t    port_lpg;
    cbx_port_t     lag_out;
    int            unit = CBX_AVENGER_PRIMARY;
    int            tunit = CBX_AVENGER_PRIMARY;
    uint32         num_units = 1;
    int            update_flag = FALSE;
    uint32_t       flags = 0;
    pbmp_t         lpbmp;
    pbmp_t         rpbmp;
    pbmp_t         csd_pbmp[CBX_MAX_UNITS];
    pbmp_t         pstatus_pbmp;
    uint32_t       port_status;
    int            i;
    int            rv;

    /* validate the port index passed */
    rv = cbxi_robo2_port_validate(gport, &port_out, &port_lpg, &unit);
    if (CBX_FAILURE(rv)) {
        return CBX_E_PORT;
    }

    /* Validate the lag index passed */
    rv = cbxi_robo2_portid_validate(lagid, &lag_out);
    if (CBX_FAILURE(rv)) {
        return CBX_E_PORT;
    }

    LAG_CHECK(lag_out);

    l_info = &LAG_INFO(lag_out);

    /* Check if it is cascade set-up */
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = CBX_MAX_UNITS;
    }

    CBX_PBMP_CLEAR(pstatus_pbmp);
    CBX_PBMP_ASSIGN(lpbmp, l_info->lag_pbmp[CBX_AVENGER_PRIMARY]);
    CBX_PBMP_ASSIGN(rpbmp, l_info->lag_pbmp[CBX_AVENGER_SECONDARY]);

    if(link_status) {
        if(unit == CBX_AVENGER_PRIMARY) {
            CBX_PBMP_PORT_ADD(lpbmp, port_out);
        } else {
            CBX_PBMP_PORT_ADD(rpbmp, port_out);
        }
        flags = CBXI_LAG_PORT_TABLE_ENTRY_ADD;
        update_flag = TRUE;
    } else {
        if(unit == CBX_AVENGER_PRIMARY) {
            CBX_PBMP_PORT_REMOVE(lpbmp, port_out);
        } else {
            CBX_PBMP_PORT_REMOVE(rpbmp, port_out);
        }
        flags = CBXI_LAG_PORT_TABLE_ENTRY_REMOVE;
        update_flag = TRUE;
    }

    for(tunit = 0; tunit < num_units; tunit++) {
        CBX_PBMP_ITER(l_info->lag_pbmp[tunit], port) {
             CBX_IF_ERROR_RETURN(cbxi_port_link_status_get(
                                            tunit, port, &port_status));
             if (port_status) {
                 CBX_PBMP_PORT_ADD(pstatus_pbmp, port);
             }
        }
        if(tunit == CBX_AVENGER_PRIMARY) {
            lpbmp &= pstatus_pbmp;
        } else {
            rpbmp &= pstatus_pbmp;
        }
        CBX_PBMP_CLEAR(pstatus_pbmp);
        port = 0;
        CBX_PBMP_CLEAR(csd_pbmp[tunit]);
        CBX_PBMP_ITER(PBMP_CASCADE(tunit), port) {
             CBX_IF_ERROR_RETURN(cbxi_port_link_status_get(
                                            tunit, port, &port_status));
             if (port_status) {
                 CBX_PBMP_PORT_ADD(csd_pbmp[tunit], port);
             }
        }
        port = 0;
    }
    sal_printf("\n unit =%d csd_pbmp[0]=0x%x, csd_pbmp[1]=0x%x \n",
                    unit, csd_pbmp[0], csd_pbmp[1] );

    if(update_flag == TRUE) {
        rv = cbxi_robo2_lag_link_svt_update(unit, l_info->lag_pbmp[unit],
                                            port_out, flags);
        if (CBX_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_FSAL_LAG,
                      (BSL_META("FSAL API:cbxi_robo2_lag_link_status_update()..\
                                 Failed to update svt entry \n")));
            return rv;
        }
        rv = cbxi_robo2_lag_link_cascade_svt_update( lpbmp, rpbmp, csd_pbmp,
                                                l_info->lag_params.lag_mode );
        if (CBX_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_FSAL_LAG,
                      (BSL_META("FSAL API:cbxi_robo2_lag_link_status_update()..\
                                 Failed to update cascade svt entry \n")));
            return rv;
        }

        sal_printf("\n unit =%d lpg2ppfov update port_out=%d, port_lpg=%d link_status=%d\n",
                    unit, port_out, port_lpg, link_status);
        rv = cbxi_robo2_lpg2ppfov_update(unit, port_lpg,
                                         port_out, flags);
        if (CBX_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_FSAL_LAG,
                      (BSL_META("FSAL API:cbxi_robo2_lag_link_status_update()..\
                                  update LPG2PPFOV entry \n")));
            return rv;
        }

        /* Iterate over all normal LAG's and update cascade port ppfov */
        if(l_info->lag_params.lag_mode == cbxLagModeCascade) {
            lag_info = &LAG_INFO(0);
            for (i = 0; i < LAG_CNTL.ngroups; i++) {
                if ((lag_info->in_use == TRUE) &&
                    (lag_info->lagid != CBX_LAG_INVALID)) {

                    if(lag_info->active[CBX_AVENGER_PRIMARY] >0) {
                        rv = cbxi_robo2_lpg2ppfov_update(unit, lag_info->lpgid,
                                                     port_out, flags);
                        if (CBX_FAILURE(rv)) {
                            LOG_ERROR(BSL_LS_FSAL_LAG,
                                     (BSL_META("FSAL API:..\
                                         cbxi_robo2_lag_link_status_update()..\
                                         update LPG2PPFOV entry \n")));
                            return rv;
                        }
                    }
                    if(lag_info->active[CBX_AVENGER_SECONDARY] >0) {
                        rv = cbxi_robo2_lpg2ppfov_update(unit, lag_info->rlpgid,
                                                     port_out, flags);
                        if (CBX_FAILURE(rv)) {
                            LOG_ERROR(BSL_LS_FSAL_LAG,
                                   (BSL_META("FSAL API:..\
                                         cbxi_robo2_lag_link_status_update()..\
                                              update LPG2PPFOV entry \n")));
                            return rv;
                        }
                    }

                    if(((lag_info->active[CBX_AVENGER_PRIMARY] >0) ||
                      (lag_info->active[CBX_AVENGER_SECONDARY] >0)) &&
                      ((CBX_PBMP_IS_NULL(csd_pbmp[CBX_AVENGER_PRIMARY])) &&
                      (CBX_PBMP_IS_NULL(csd_pbmp[CBX_AVENGER_SECONDARY])))) {
                        CBX_PBMP_CLEAR(lpbmp);
                        CBX_PBMP_CLEAR(rpbmp);
                        if(unit == CBX_AVENGER_PRIMARY) {
                            CBX_PBMP_ASSIGN(lpbmp,
                                    lag_info->lag_pbmp[CBX_AVENGER_PRIMARY]);
                            CBX_PBMP_ASSIGN(rpbmp,
                                    lag_info->lag_pbmp[CBX_AVENGER_SECONDARY]);
                        } else {
                            CBX_PBMP_ASSIGN(rpbmp,
                                    lag_info->lag_pbmp[CBX_AVENGER_PRIMARY]);
                            CBX_PBMP_ASSIGN(lpbmp,
                                    lag_info->lag_pbmp[CBX_AVENGER_SECONDARY]);
                        }
   sal_printf("\n unit =%d lpbmp=0x%x rpbmp=0x%x, csd_pbmp[0]=0x%x csd_pbmp[1]=0x%x\n",
                    unit, lpbmp, rpbmp,csd_pbmp[0],csd_pbmp[1] );
                        rv = cbxi_robo2_lag_linkfail_svt_update( unit, lpbmp );
                        if (CBX_FAILURE(rv)) {
                            LOG_ERROR(BSL_LS_FSAL_LAG,
                                   (BSL_META("FSAL API:..\
                                         cbxi_robo2_lag_link_status_update()..\
                                              update svt entry \n")));
                            return rv;
                        }
                    }
                }
                lag_info++;
            }
        }
    }

    return CBX_E_NONE;
}
#endif /* CONFIG_CASCADED_MODE */


/**************************************************************************
 *                 LAG FSAL API IMPLEMENTATION                           *
 **************************************************************************/

/*
 * Function:
 *      cbx_lag_params_t_init
 * Purpose:
 *      Initialize LAG entry parameters.
 * Parameters:
 *      lag_params - Pointer to LAG parameter structure.
 * Returns:
 *      None
 */

void cbx_lag_params_t_init( cbx_lag_params_t *lag_params )
{
    if ( NULL != lag_params ) {
        sal_memset( lag_params, 0, sizeof ( cbx_lag_params_t ));
    }
    return;
}

/*
 * Function:
 *      cbx_lag_psc_t_init
 * Purpose:
 *      Initialize lag port selection criteria.
 * Parameters:
 *      lag_psc      port selection criteria object
 * Returns:
 *      None
 */

void cbx_lag_psc_t_init ( cbx_lag_psc_t *lag_psc )
{
    if ( NULL != lag_psc ) {
        sal_memset( lag_psc, 0, sizeof ( cbx_lag_psc_t ));
    }
    return;
}

/*
 * Function:
 *      cbx_lag_cntl_t_init
 * Purpose:
 *      Initialize LAG control structure parameters.
 * Parameters:
 *      lag_cntl - Pointer to LAG control structure.
 * Returns:
 *      None
 */

void cbx_lag_cntl_t_init( cbx_lag_cntl_t *lag_cntl )
{
    if ( NULL != lag_cntl ) {
        sal_memset( lag_cntl, 0, sizeof ( cbx_lag_cntl_t ));
    }
    return;
}

/*
 * Function:
 *      cbx_lag_init
 * Purpose:
 *      Initializes the lag tables to empty (no lags configured)
 * Parameters:
 *      None.
 * Returns:
 *      CBX_E_NONE - success (or already initialized)
 *      CBX_E_MEMORY - failed to allocate required memory.
 */
int
cbx_lag_init(void)
{

    int  alloc_size=0;
    int  rv,i=0;
    int  unit=0;
    cbx_lag_info_t  *lag_info;
    cbx_portid_t  lagid;
    uint32 num_units = 1;
    int max_index = 0;

    rv = CBX_E_NONE;

    LOG_INFO(BSL_LS_FSAL_LAG,
             (BSL_META("FSAL API : cbx_lag_init()..\n")));

    cbx_lag_cntl_t_init(&LAG_CNTL);

    /* Get the number of allowed turnk groups in this unit */
    LAG_CNTL.ngroups = CBX_LAG_MAX;

    /* Get the max port number allowed in a given trunk. */
    LAG_CNTL.nports = 8;

    /* Enable all the tables related to lag module */
    rv = cbxi_lag_table_init();
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_LAG,
                  (BSL_META("FSAL API : cbx_lag_init()..Table init failed\n")));
        return rv;
    }

    /* Check if it is cascade set-up */
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = CBX_MAX_UNITS;
    }

    if (LAG_CNTL.lag_info != NULL) {
        sal_free(LAG_CNTL.lag_info);
        LAG_CNTL.lag_info = NULL;
    }

    if (LAG_CNTL.ngroups > 0) {
        alloc_size = LAG_CNTL.ngroups * sizeof(cbx_lag_info_t);
        lag_info = (cbx_lag_info_t *) sal_alloc(alloc_size, "lag_info");
        if (lag_info == NULL) {
            return (CBX_E_MEMORY);
        }
        LAG_CNTL.lag_info = lag_info;

        for (lagid = 0; lagid < LAG_CNTL.ngroups; lagid++) {
            lag_info->lagid  = CBX_LAG_INVALID;
            lag_info->in_use = FALSE;
            lag_info->lpgid  = CBXI_PGID_INVALID;
            lag_info->rlpgid = CBXI_PGID_INVALID;
            lag_info->gpgid  = CBXI_PGID_INVALID;
            lag_info->lgpgid  = CBXI_PGID_INVALID;
            lag_info->rgpgid  = CBXI_PGID_INVALID;
            lag_info->port_one  = CBXI_PGID_INVALID;
            for (unit=0; unit < num_units; unit++) {
                /* Initialize lag local pbmp */
                CBX_PBMP_CLEAR(lag_info->lag_pbmp[unit]);
                lag_info->active[unit] = 0;
            }
            lag_info++;
        }
        cbx_lag_params_t_init(&lag_info->lag_params);

        for (unit=0; unit < num_units; unit++) {

            /* Initialize lag local SW LPG table */
            for (i=0; i<CBX_MAX_PORT_PER_UNIT; i++) {
                /* PP 2 PG is 1:1 maping without LAG, mark all lpgt entries
                 * as used. All LPGs are used by PPs on powerup initially.
                 */
                /* Port 14 and 15 never used in secondary Avenger */
                if ((unit == CBX_AVENGER_SECONDARY) &&
                    (i >= 14 ))
                {
                    lpgt[unit][i] = TRUE;
                } else {
                    lpgt[unit][i] = FALSE;
                }
            }
        }

        /* Initialize lag local SW global PG table */
        for (i = 0; i < MAX_PG; i++) {
            /* LPG 2 PG mapping is 1:1 maping without LAG, mark all pgt entries
             * as used. All PGs are used by PPs on powerup initially.
             */
            /* PG 30 and 31 never used, secondary has only 14 ports */
            if (i >= 30 )
            {
                pgt[i] = TRUE;
            } else {
                pgt[i] = FALSE;
            }
        }

        /* Initialize lag psc structure */
        cbx_lag_psc_t_init(&LAG_CNTL.psc[cbxPacketOther]);
        cbx_lag_psc_t_init(&LAG_CNTL.psc[cbxPacketIpv4]);
        cbx_lag_psc_t_init(&LAG_CNTL.psc[cbxPacketIpv6]);

        /* Initialize psc field pbmp */
        CBX_PBMP_CLEAR(LAG_CNTL.pscfld_pbmp[cbxPacketOther]);
        CBX_PBMP_CLEAR(LAG_CNTL.pscfld_pbmp[cbxPacketIpv4]);
        CBX_PBMP_CLEAR(LAG_CNTL.pscfld_pbmp[cbxPacketIpv6]);

        /* Initialize default global PSC Flags */
        /* Add fields F4-DMAC F5-SMAC F6-OUTER_VID for key=1 */
        LAG_CNTL.psc[cbxPacketOther].pkt_type = cbxPacketOther;
        LAG_CNTL.psc[cbxPacketOther].flags = (CBX_LAG_PSC_SRCMAC |
                                              CBX_LAG_PSC_DSTMAC |
                                              CBX_LAG_PSC_SPG |
                                              CBX_LAG_PSC_VLAN);

        CBX_PBMP_PORT_ADD(LAG_CNTL.pscfld_pbmp[cbxPacketOther],
                          CBX_LAG_IKFT_FIELD_ID_SPG);
        CBX_PBMP_PORT_ADD(LAG_CNTL.pscfld_pbmp[cbxPacketOther],
                          CBX_LAG_IKFT_FIELD_ID_DSTMAC);
        CBX_PBMP_PORT_ADD(LAG_CNTL.pscfld_pbmp[cbxPacketOther],
                          CBX_LAG_IKFT_FIELD_ID_SRCMAC);
        CBX_PBMP_PORT_ADD(LAG_CNTL.pscfld_pbmp[cbxPacketOther],
                          CBX_LAG_IKFT_FIELD_ID_OUTER_TCI);

        for (unit=0; unit < num_units; unit++) {
            /* coverity[overrun-local] */
            CBX_PBMP_ITER(LAG_CNTL.pscfld_pbmp[cbxPacketOther], i) {
                /* Enable key 1 for non-IP packets  */
                /* coverity[overrun-call] */
                rv = cbxi_robo2_ikft_update(unit, i, CBX_LAG_LBH_OTHER_KEY_ID);
                if (CBX_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_FSAL_LAG,
                       (BSL_META("FSAL API : cbx_lag_init()..Failed to update \
                                     IKFT non-IP packet key entry \n")));
                    return rv;
                }
            }
        }

        /* Add fields F10-L3SA F11-L3DA F17-DPORT_IP_PROTO
         * F18-SPORT for key=2
         */
        LAG_CNTL.psc[cbxPacketIpv4].pkt_type = cbxPacketIpv4;
        LAG_CNTL.psc[cbxPacketIpv4].flags = (CBX_LAG_PSC_SRCIP |
                                             CBX_LAG_PSC_DSTIP |
                                             CBX_LAG_PSC_SPG |
                                             CBX_LAG_PSC_L3PROTO |
                                             CBX_LAG_PSC_L4BYTES);

        CBX_PBMP_PORT_ADD(LAG_CNTL.pscfld_pbmp[cbxPacketIpv4],
                          CBX_LAG_IKFT_FIELD_ID_SPG);
        CBX_PBMP_PORT_ADD(LAG_CNTL.pscfld_pbmp[cbxPacketIpv4],
                          CBX_LAG_IKFT_FIELD_ID_SRCIP);
        CBX_PBMP_PORT_ADD(LAG_CNTL.pscfld_pbmp[cbxPacketIpv4],
                          CBX_LAG_IKFT_FIELD_ID_DSTIP);
        CBX_PBMP_PORT_ADD(LAG_CNTL.pscfld_pbmp[cbxPacketIpv4],
                          CBX_LAG_IKFT_FIELD_ID_L3PROTO);
        CBX_PBMP_PORT_ADD(LAG_CNTL.pscfld_pbmp[cbxPacketIpv4],
                          CBX_LAG_IKFT_FIELD_ID_L4BYTES);

        for (unit=0; unit < num_units; unit++) {
            /* coverity[overrun-local] */
            CBX_PBMP_ITER(LAG_CNTL.pscfld_pbmp[cbxPacketIpv4], i) {
                /* Enable key 2 for all IPvf4 packets */
                /* coverity[overrun-call] */
                rv = cbxi_robo2_ikft_update(unit, i, CBX_LAG_LBH_IPv4_KEY_ID);
                if (CBX_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_FSAL_LAG,
                       (BSL_META("FSAL API : cbx_lag_init()..Failed to update \
                                      IKFT IPv4 key entry \n")));
                    return rv;
                }
            }
        }


        /* Add fields  F10-L3SA F11-L3DA F17-DPORT_IP_PROTO
         * F18-SPORT for key=3
         */
        LAG_CNTL.psc[cbxPacketIpv6].pkt_type = cbxPacketIpv6;
        LAG_CNTL.psc[cbxPacketIpv6].flags = (CBX_LAG_PSC_SRCIPV6 |
                                             CBX_LAG_PSC_DSTIPV6 |
                                             LAG_CNTL.psc[cbxPacketIpv4].flags);

        CBX_PBMP_PORT_ADD(LAG_CNTL.pscfld_pbmp[cbxPacketIpv6],
                          CBX_LAG_IKFT_FIELD_ID_SPG);
        CBX_PBMP_PORT_ADD(LAG_CNTL.pscfld_pbmp[cbxPacketIpv6],
                          CBX_LAG_IKFT_FIELD_ID_SRCIP);
        CBX_PBMP_PORT_ADD(LAG_CNTL.pscfld_pbmp[cbxPacketIpv6],
                          CBX_LAG_IKFT_FIELD_ID_SRCIPV6);
        CBX_PBMP_PORT_ADD(LAG_CNTL.pscfld_pbmp[cbxPacketIpv6],
                          CBX_LAG_IKFT_FIELD_ID_DSTIP);
        CBX_PBMP_PORT_ADD(LAG_CNTL.pscfld_pbmp[cbxPacketIpv6],
                          CBX_LAG_IKFT_FIELD_ID_DSTIPV6);
        CBX_PBMP_PORT_ADD(LAG_CNTL.pscfld_pbmp[cbxPacketIpv6],
                          CBX_LAG_IKFT_FIELD_ID_L3PROTO);
        CBX_PBMP_PORT_ADD(LAG_CNTL.pscfld_pbmp[cbxPacketIpv6],
                          CBX_LAG_IKFT_FIELD_ID_L4BYTES);

        for (unit=0; unit < num_units; unit++) {
            /* coverity[overrun-local] */
            CBX_PBMP_ITER(LAG_CNTL.pscfld_pbmp[cbxPacketIpv6], i) {
                /* Enable key 3 for IPv6 packets */
                /* coverity[overrun-call] */
                rv = cbxi_robo2_ikft_update(unit, i, CBX_LAG_LBH_IPv6_KEY_ID);
                if (CBX_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_FSAL_LAG,
                       (BSL_META("FSAL API : cbx_lag_init()..Failed to update \
                                     IKFT IPv6 key entry \n")));
                    return rv;
                }
            }
        }
        /* All FPSLICT entries should have LBH keys irrespective of rules they
         * correspond to, as Load balancing is done on all types of traffic */
        for (unit=0; unit < num_units; unit++) {
            max_index = soc_robo2_fpslict_max_index(unit);
            for(i = 1; i < max_index; i++) {
                CBX_IF_ERROR_RETURN(cbxi_lag_fpslict_table_update(unit, i));
            }
        }
    }

    return CBX_E_NONE;
}

/**
 * Function:
 *      cbx_lag_create
 * Purpose:
 *      Create a lag
 * This routine is used to create a Link Aggregration Group.
 * It allocates an entry in the Port Group Table (PGT)
 * It sets this IPP Table:
 *  - Physical Port to Port Group (PP2PG) table
 * It sets these PQM Tables:
 *  - Port Group to Physical Port Fan-Out Vector (PG2PPFOV) table
 *  - Physical Port to Port Group Identifier (PP2PGID) table
 *  - Squelch Vector Table (SVT)
 * These tables are also used by the Port module.
 * The set of Ports and LAGs make up the Global Port Number space
 *
 * @param lag_params  (IN)  Lag parameters.
 * @param lagid       (OUT) Handle of the Lag created
 *                          CBX_LAG_INVALID: Lag could not be created
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_lag_create ( cbx_lag_params_t*lag_params,
                     cbx_portid_t     *lagid )
{

    int  rv = CBX_E_FULL;
    int  i=0;
    cbx_lag_info_t  *lag_info;

    LAG_INIT;

    LOG_INFO(BSL_LS_FSAL_LAG,
             (BSL_META("FSAL API : cbx_lag_create()..\n")));

    /* Locate first entry */
    lag_info = &LAG_INFO(0);

    for (i = 0; i < LAG_CNTL.ngroups; i++) {
        if ((lag_info->in_use == FALSE) &&
                (lag_info->lagid == CBX_LAG_INVALID)) {
            lag_info->lagid = i;
            lag_info->in_use = TRUE;
            sal_memcpy(&(lag_info->lag_params), lag_params,
                       sizeof(cbx_lag_params_t));

            /* Form lag portid */
            CBX_PORTID_TRUNK_SET(*lagid, lag_info->lagid);
            rv = CBX_E_NONE;
            break;
        }
        lag_info++;
    }

#ifdef CONFIG_PORT_EXTENDER
    if (lag_info->lag_params.lag_mode == cbxLagModeShared) {
        CBX_IF_ERROR_RETURN(cbxi_pe_shared_lag_set(*lagid, TRUE));
    }
#endif

    return rv;
}

/**
 * Function:
 *      cbx_lag_destroy
 * Purpose:
 *      Destroy a lag previously created by cbx_lag_create()
 *
 * @param lagid    (IN)  Handle of the lag to be destroyed
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 *
 * @note This routine destroys a previously created Link Aggregration Group.
 */

int cbx_lag_destroy ( cbx_portid_t lagid )
{

    cbx_lag_info_t  *lag_info;
    cbx_port_t      lag_out;
    cbx_port_t      port;
    int  rv = CBX_E_NONE;
    int  unit = CBX_AVENGER_PRIMARY;
    uint32 num_units = 1;

    LAG_INIT;

    LOG_INFO(BSL_LS_FSAL_LAG,
             (BSL_META("FSAL API : cbx_lag_destroy()..\n")));

    /* Check if portid is of LAG */
    if (!CBX_PORTID_IS_TRUNK(lagid)) {
        return CBX_E_PORT;
    }

    /* Validate the lag index passed */
    rv = cbxi_robo2_portid_validate(lagid, &lag_out);
    if (CBX_FAILURE(rv)) {
        return CBX_E_PORT;
    }

    LAG_CHECK(lag_out);

    lag_info = &LAG_INFO(lag_out);

    /* Check if lag is a valid */
    if (lag_info->in_use == FALSE) {
        LOG_ERROR(BSL_LS_FSAL_LAG,
                  (BSL_META("FSAL API : cbx_lag_destroy()..invalid \
                             LAG \n")));
        return CBX_E_PARAM;
    }

    /* Check if it is cascade set-up */
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = CBX_MAX_UNITS;
    }

    for (unit=0; unit < num_units; unit++) {
        CBX_PBMP_ITER(lag_info->lag_pbmp[unit], port) {
            /* Cleanup PP2LPG, LPG2PG, SVT and PG2PPFOV
             * entries for this lag group
             */
            /* Coverity fix ensure port does not exceed CBX_MAX_PORT_PER_UNIT */
            if (port < CBX_MAX_PORT_PER_UNIT) {
                CBX_IF_ERROR_RETURN(cbxi_lag_member_remove (lag_info, port,
                                    unit));
            }
        }
    }
#ifdef CONFIG_PORT_EXTENDER
    if (lag_info->lag_params.lag_mode == cbxLagModeShared) {
        /* Update LAG LIN destination */
        CBX_IF_ERROR_RETURN(cbxi_pe_lag_dest_set(lag_info));

        CBX_IF_ERROR_RETURN(cbxi_pe_shared_lag_set(lagid, FALSE));
    }
#endif

    /* Clear lag info parameters */
    lag_info->lagid = CBX_LAG_INVALID;
    lag_info->in_use = FALSE;
    lag_info->lpgid  = CBXI_PGID_INVALID;
    lag_info->rlpgid = CBXI_PGID_INVALID;
    lag_info->gpgid  = CBXI_PGID_INVALID;
    lag_info->lgpgid  = CBXI_PGID_INVALID;
    lag_info->rgpgid  = CBXI_PGID_INVALID;
    lag_info->port_one  = CBXI_PGID_INVALID;
    for (unit=0; unit < num_units; unit++) {
        CBX_PBMP_CLEAR(lag_info->lag_pbmp[unit]);
        lag_info->active[unit] = 0;
    }

    sal_memset(&(lag_info->lag_params), 0, sizeof(cbx_lag_params_t));

    return (rv);
}

/**
 * Function:
 *    cbx_lag_member_add
 * Purpose :
 *    Add port member to lag interface.
 * This routine is used to add a port to a LAG.
 *
 * @param lagid     (IN)  LAG Identifier
 * @param portid    (IN)  Port Identifier
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_lag_member_add ( cbx_portid_t lagid, cbx_portid_t portid )
{

    int  i;
    cbx_lag_info_t  *lag_info;
    cbx_port_t     port_out;
    cbx_port_t     lag_out;
    lpg2pg_t       lpg2pg_entry;
    int            unit = CBX_AVENGER_PRIMARY;
    int            lpgid;
    int            lpgupdt_flag=0;
    uint32_t       flag;
    uint32         status  = 0;
    int            rv = CBX_E_NONE;
    cbxi_pgid_t    port_lpg;
    pgt_t          ent_pgt;
#ifdef CONFIG_CASCADED_MODE
    uint8_t        pe_en = 0;
    cbx_port_t     csd_port;
#if LAG_NOT_REQUIRED
    int            tunit = CBX_AVENGER_PRIMARY;
#endif
#endif

    flag = CBXI_LAG_PORT_TABLE_ENTRY_ADD;

    LAG_INIT;

    LOG_INFO(BSL_LS_FSAL_LAG,
             (BSL_META("FSAL API : cbx_lag_member_add()..\n")));

    /* Check if portid is of LAG */
    if (!CBX_PORTID_IS_TRUNK(lagid)) {
        return CBX_E_PORT;
    }

    /* validate the port index passed */
    rv = cbxi_robo2_port_validate(portid, &port_out, &port_lpg, &unit);
    if (CBX_FAILURE(rv)) {
        return CBX_E_PORT;
    }

    /* validate the lag index passed */
    rv = cbxi_robo2_portid_validate(lagid, &lag_out);
    if (CBX_FAILURE(rv)) {
        return CBX_E_PORT;
    }

    LAG_CHECK(lag_out);

    /* Locate first entry */
    lag_info = &LAG_INFO(0);

    /* Check if port is member of another lag */
    for (i = 0; i < LAG_CNTL.ngroups; i++) {
        if ((lag_info->in_use == TRUE) &&
                (lag_info->lagid != CBX_LAG_INVALID) &&
                ((lag_info->active[CBX_AVENGER_PRIMARY] >0) ||
                (lag_info->active[CBX_AVENGER_SECONDARY] >0))) {
            if (CBX_PBMP_MEMBER(lag_info->lag_pbmp[unit], port_out)) {
                LOG_ERROR(BSL_LS_FSAL_LAG,
                          (BSL_META("FSAL API : cbx_lag_member_add()..port is \
                       already member of lag id=%d.  \n"),lag_info->lagid));
                return CBX_E_PARAM;
            }
        }
        lag_info++;
    }

    lag_info = &LAG_INFO(lag_out);

    /* Check if lag is a valid */
    if (lag_info->in_use == FALSE) {
        LOG_ERROR(BSL_LS_FSAL_LAG,
                  (BSL_META("FSAL API : cbx_lag_member_add()..invalid \
                             LAG \n")));
        return CBX_E_PARAM;
    }

#ifdef CONFIG_PORT_EXTENDER
    if (lag_info->lag_params.lag_mode == cbxLagModeShared) {
        CBX_IF_ERROR_RETURN(cbxi_shared_lag_member_add(lag_info, port_out, unit));
        return CBX_E_NONE;
    }
#endif

    /* Save port PGT properties, use PGID returned for the port
     * from cbxi_robo2_port_validate()
     */
    rv = cbxi_port_pgt_context_save(unit, port_out, port_lpg);
    if (CBX_FAILURE(rv)) {
        return CBX_E_PORT;
    }

    /* Debug routines */
    cbxi_lag_info_dump ();
    cbxi_lag_member_table_dump (port_out, port_lpg);

    if (lag_info->active[unit] == 0) {

        if(lag_info->port_one == CBXI_PGID_INVALID) {
            lag_info->port_one = ( (unit) * (CBX_MAX_PORT_PER_UNIT)) + port_out;
        }

        /* Update LPGID/RPGID to PGID value returned from
         * cbxi_robo2_port_validate API
         */
        if(unit == CBX_AVENGER_PRIMARY ) {
            lag_info->lpgid = port_lpg;
            lpgid = lag_info->lpgid;
            lpgupdt_flag = TRUE;
        } else {
            lag_info->rlpgid = port_lpg;
            lpgid = lag_info->rlpgid;
            lpgupdt_flag = TRUE;
        }

        if (lag_info->lag_params.lag_mode != cbxLagModeShared) {
            /* Temporary fix to bypass SLICTCAM */
            CBX_IF_ERROR_RETURN(soc_robo2_pgt_get(unit, lpgid, &ent_pgt));
            ent_pgt.default_slicid = 250;
            CBX_IF_ERROR_RETURN(soc_robo2_pgt_set(unit, lpgid, &ent_pgt, &status));
        }
        /* Read entry from LPG2PG */
        CBX_IF_ERROR_RETURN(soc_robo2_lpg2pg_get(unit, lpgid, &lpg2pg_entry));

        if (unit == CBX_AVENGER_PRIMARY) {
            lag_info->lgpgid = lpg2pg_entry.pg;
        } else {
            lag_info->rgpgid = lpg2pg_entry.pg;
        }

        if(lag_info->port_one < CBX_MAX_PORT_PER_UNIT) {
            lag_info->gpgid = lag_info->lgpgid;
            if( lag_info->rgpgid != CBXI_PGID_INVALID) {
                /* Free rgpgid */
                pgt[lag_info->rgpgid] = FALSE;
            }
        } else {
            lag_info->gpgid = lag_info->rgpgid;
            if( lag_info->lgpgid != CBXI_PGID_INVALID) {
                /* Free rgpgid */
                pgt[lag_info->lgpgid] = FALSE;
            }
        }

        /* Add port as lag member */
        CBX_PBMP_PORT_ADD(lag_info->lag_pbmp[unit], port_out);

    } else {
        if(unit == CBX_AVENGER_PRIMARY) {
            /* Update LPGID/RPGID to PGID value returned from
             * cbxi_robo2_port_validate API
             */
            if(lag_info->lpgid == CBXI_PGID_INVALID) {
                lag_info->lpgid = port_lpg;
                lpgupdt_flag = TRUE;
            }
            lpgid = lag_info->lpgid;
        } else {
            if(lag_info->rlpgid == CBXI_PGID_INVALID) {
                lag_info->rlpgid = port_lpg;
                lpgupdt_flag = TRUE;
            }
            lpgid = lag_info->rlpgid;
        }

        /* Add port as lag member */
        CBX_PBMP_PORT_ADD(lag_info->lag_pbmp[unit], port_out);
    }

    lag_info->active[unit]++;

    /* Update SVT */
    rv = cbxi_robo2_lag_svt_update(unit,
                                   lag_info->lag_pbmp[unit], port_out, flag);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_LAG,
                  (BSL_META("FSAL API : cbx_lag_member_add()..Failed to update \
                              SVT entry \n")));
        return rv;
    }

#ifdef CONFIG_CASCADED_MODE
        CBX_IF_ERROR_RETURN(soc_robo2_pgt_get(unit, port_lpg, &ent_pgt));
        ent_pgt.spgid = lag_info->gpgid;
        CBX_IF_ERROR_RETURN(soc_robo2_pgt_set(unit, port_lpg, &ent_pgt, &status));

    if (lag_info->lag_params.lag_mode == cbxLagModeNormal) {
        rv = cbxi_robo2_lag_cascade_svt_update( lag_info );
        if (CBX_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_FSAL_LAG,
                      (BSL_META("FSAL API : cbx_lag_member_add()..Failed to \
                                  update SVT entry in cascaded mode \n")));
            return rv;
        }
    }
#endif /* CONFIG_CASCADED_MODE */


    if (lag_info->lag_params.lag_mode != cbxLagModeShared) {
        /* Allocate PG for this port - PG is same as LPGID  */
        rv = cbxi_robo2_ipp_pp2lpg_update(unit, lpgid, port_out);
        if (CBX_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_FSAL_LAG,
                      (BSL_META("FSAL API : cbx_lag_member_add()..Failed to update \
                                  PP2LPG entry \n")));
            return rv;
        }

        /* Update PP2PGID */
        rv = cbxi_robo2_pqm_pp2lpg_update(unit, lpgid, port_out);
        if (CBX_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_FSAL_LAG,
                      (BSL_META("FSAL API : cbx_lag_member_add()..Failed to update \
                                  PP2PGID entry \n")));
            return rv;
        }
    }

    /* Update SPGPPBMP config */
    rv = cbxi_robo2_spg2ppconfig_update(unit, lpgid,
                                     port_out, flag);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_LAG,
                  (BSL_META("FSAL API : cbx_lag_member_add()..Failed to update \
                              spg2ppconfig entry \n")));
        return rv;
    }

    /* Update PG2PPFOV */
    rv = cbxi_robo2_lpg2ppfov_update(unit, lpgid,
                                     port_out, flag);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_LAG,
                  (BSL_META("FSAL API : cbx_lag_member_add()..Failed to update \
                              PG2PPFOV entry \n")));
        return rv;
    }

#ifdef CONFIG_CASCADED_MODE
#ifdef LAG_NOT_REQUIRED
    if(unit == CBX_AVENGER_PRIMARY ) {
        tunit = CBX_AVENGER_SECONDARY;
    } else {
        tunit = CBX_AVENGER_PRIMARY;
    }
    if(CBX_PBMP_NOT_NULL(lag_info->lag_pbmp[tunit])) {
#endif
#ifdef CONFIG_PORT_EXTENDER
        if ((cbxi_check_port_extender_mode() == CBX_E_NONE)) {
            pe_en = 1;
        }
#endif /* CONFIG_PORT_EXTENDER */

        if (pe_en != 1) {
            /* In PE mode, Cascade ports are not added to LAG by default */
            CBX_PBMP_ITER(PBMP_CASCADE(unit), csd_port) {
                rv = cbxi_robo2_lpg2ppfov_update(unit, lpgid,
                                             csd_port, flag);
                if (CBX_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_FSAL_LAG,
                          (BSL_META("FSAL API : cbx_lag_member_add()..Failed to\
                                      update PG2PPFOV entry \n")));
                    return rv;
                }
            }
        }
#ifdef  LAG_NOT_REQUIRED
    }
#endif
#endif /* CONFIG_CASCADED_MODE */

    if(lpgupdt_flag == TRUE) {
        /* Update LPG2PGID */
        rv = cbxi_robo2_lpg2pg_update(unit, lpgid, lag_info->gpgid);
        if (CBX_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_FSAL_LAG,
                      (BSL_META("FSAL API : cbx_lag_member_add()..Failed to \
                                 update LPG2PGID entry \n")));
            return rv;
        }

        /* Update PG2LPGID */
        rv = cbxi_robo2_pg2lpg_update(unit, lag_info->gpgid, lpgid);
        if (CBX_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_FSAL_LAG,
                      (BSL_META("FSAL API : cbx_lag_member_add()..Failed to \
                                 update PG2LPGID entry \n")));
            return rv;
        }
#ifdef CONFIG_CASCADED_MODE
            if((lag_info->lgpgid != CBXI_PGID_INVALID) &&
              ( lag_info->lgpgid != lag_info->gpgid)) {
                /* Free lgpgid */
                pgt[lag_info->lgpgid] = FALSE;
            }
            if((lag_info->rgpgid != CBXI_PGID_INVALID) &&
              ( lag_info->rgpgid != lag_info->gpgid)) {
                /* Free rgpgid */
                pgt[lag_info->rgpgid] = FALSE;
            }
#endif /* CONFIG_CASCADED_MODE */
    } else {
        /* Mark lpgt entry as free slot */

        if (lag_info->lag_params.lag_mode != cbxLagModeShared) {
            lpgt[unit][port_lpg] = FALSE;

            sal_memset(&lpg2pg_entry, 0, sizeof(lpg2pg_t));

            /* Get PG corresponding to the port_lpg */
            CBX_IF_ERROR_RETURN(soc_robo2_lpg2pg_get(unit, port_lpg,
                                &lpg2pg_entry));

            pgt[lpg2pg_entry.pg] = FALSE;
        }
        /* Remove LAG port from original PG2PPFOV */
        rv = cbxi_robo2_lpg2ppfov_update(
                 unit, port_lpg, port_out, CBXI_LAG_PORT_TABLE_ENTRY_REMOVE);
        if (CBX_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_FSAL_LAG,
                  (BSL_META("FSAL API : cbx_lag_member_add()..Failed to \
                             update PG2PPFOV entry \n")));
            return rv;
        }
        /* Remove LAG port from original SPGPPBMP config */
        rv = cbxi_robo2_spg2ppconfig_update(
                 unit, port_lpg, port_out, CBXI_LAG_PORT_TABLE_ENTRY_REMOVE);
        if (CBX_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_FSAL_LAG,
                  (BSL_META("FSAL API : cbx_lag_member_add()..Failed to \
                             update SPGPPBMP config entry \n")));
            return rv;
        }
    }

#ifdef CONFIG_PORT_EXTENDER
    /* Update LAG LIN destination */
    CBX_IF_ERROR_RETURN(cbxi_pe_lag_dest_set(lag_info));
#endif

    LOG_INFO(BSL_LS_FSAL_LAG,
             (BSL_META("FSAL API : cbx_robo2_lag_member_add()..LAG id =%d \
                       portid=%d, lpgid=%d, gpgid=%d \n"),
              lag_info->lagid, port_out,
              lag_info->lpgid, lag_info->gpgid));
    /* Debug routines */
    cbxi_lag_info_dump ();
    cbxi_lag_member_table_dump (port_out, port_lpg);

    return rv;
}

/**
 * Function:
 *    cbx_lag_member_remove
 * Purpose :
 *    Delete port member from lag interface.
 * This routine is used to remove a port from a LAG.
 *
 * @param lagid     (IN)  LAG Identifier
 * @param portid    (IN)  Port Identifier
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_lag_member_remove ( cbx_portid_t lagid, cbx_portid_t portid )
{

    cbx_lag_info_t  *lag_info;
    cbx_port_t     port_out;
    int            unit = CBX_AVENGER_PRIMARY;
    int            rv = CBX_E_NONE;
    cbx_port_t     lag_out;
    cbxi_pgid_t    port_lpg;


    LAG_INIT;

    LOG_INFO(BSL_LS_FSAL_LAG,
             (BSL_META("FSAL API : cbx_lag_member_remove()..\n")));

    /* Check if portid is of LAG */
    if (!CBX_PORTID_IS_TRUNK(lagid)) {
        return CBX_E_PORT;
    }

    /* Validate the lag index passed */
    rv = cbxi_robo2_portid_validate(lagid, &lag_out);
    if (CBX_FAILURE(rv)) {
        return CBX_E_PORT;
    }

    LAG_CHECK(lag_out);

    /* validate the port index passed */
    rv = cbxi_robo2_port_validate(portid, &port_out, &port_lpg, &unit);
    if (CBX_FAILURE(rv)) {
        return CBX_E_PORT;
    }

    /* Debug routines */
    cbxi_lag_info_dump ();
    cbxi_lag_member_table_dump (port_out, port_lpg);

    lag_info = &LAG_INFO(lag_out);

    /* Delete port from lag members */
    rv = cbxi_lag_member_remove (lag_info, port_out, unit);

#ifdef CONFIG_PORT_EXTENDER
    if (lag_info->lag_params.lag_mode == cbxLagModeShared) {
        CBX_IF_ERROR_RETURN(cbxi_pe_shared_lag_port_set(unit, lag_out, port_out, FALSE));
    }
#endif


    return rv;
}

/**
 * Function:
 *    cbx_lag_psc_get
 * Purpose :
 *    Get port selection criterion for lag interface.
 * LAG PSC get
 * This routine is used to get the Port Selection Criteria value.
 *
 * @param lagid     (IN)  LAG Identifier
 *                        when PSC is global, use CBX_LAG_ALL
 * @param psc       (OUT) PSC object
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_lag_psc_get ( cbx_portid_t lagid, cbx_lag_psc_t *psc )
{

    int            rv = CBX_E_NOT_FOUND;

    LAG_INIT;

    LOG_INFO(BSL_LS_FSAL_LAG,
             (BSL_META("FSAL API : cbx_lag_psc_get()..\n")));

    /* PSC setttings are global, per LAG configuration is invalid */
    if (lagid != CBX_LAG_ALL) {
        return CBX_E_PARAM;
    }

    switch(psc->pkt_type) {
    /** IPv4 packets */
    case cbxPacketIpv4:
        psc->flags = LAG_CNTL.psc[cbxPacketIpv4].flags;
        rv = CBX_E_NONE;
        break;
    /** IPv6 packets */
    case cbxPacketIpv6:
        psc->flags = LAG_CNTL.psc[cbxPacketIpv6].flags;
        rv = CBX_E_NONE;
        break;
    /** other (non IPv4 or IPv6) packets */
    case cbxPacketOther:
        psc->flags = LAG_CNTL.psc[cbxPacketOther].flags;
        rv = CBX_E_NONE;
        break;
    default:
        break;
    }

    return rv;
}

/**
 * Function:
 *    cbx_lag_psc_set
 * Purpose :
 *    Set port selection criterion for lag interface.
 * LAG PSC set
 * This routine is used to set the Port Selection Criteria value.
 *
 * @param lagid     (IN)  LAG Identifier
 *                        when PSC is global, use CBX_LAG_ALL
 * @param psc       (IN)  PSC object
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_lag_psc_set ( cbx_portid_t lagid, cbx_lag_psc_t *psc )
{

    int            rv = CBX_E_NOT_FOUND;
    int            unit = CBX_AVENGER_PRIMARY;
    uint32         num_units = 1;
    pbmp_t         pscfld_mask = 0;
    int            psc_fld = 0;
    int            key_id = 0;
    pbmp_t         oldpscfld_pbmp = 0;

    LAG_INIT;

    LOG_INFO(BSL_LS_FSAL_LAG,
             (BSL_META("FSAL API : cbx_lag_psc_set()..\n")));

    /* Configure PSC global setttings, per LAG PSC configuration is invalid. */
    if (lagid != CBX_LAG_ALL) {
        return CBX_E_PARAM;
    }

    rv =  cbxi_lag_psc_flags_validate (psc);
    if (CBX_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_FSAL_LAG,
                  (BSL_META("FSAL API : cbx_lag_psc_set()..Flags\
                             validate failed \n")));
        return rv;
    }

    CBX_PBMP_CLEAR(pscfld_mask);

    /* Setup LBH entries in IKFT table */
    switch(psc->pkt_type) {
    /** other (non IPv4 or IPv6) packets */
    case cbxPacketOther:
        oldpscfld_pbmp = LAG_CNTL.pscfld_pbmp[cbxPacketOther];
        LAG_CNTL.psc[cbxPacketOther].flags = psc->flags;
        rv = CBX_E_NONE;
        key_id = CBX_LAG_LBH_OTHER_KEY_ID;
        if (LAG_CNTL.psc[cbxPacketOther].flags & CBX_LAG_PSC_SPG) {
            CBX_PBMP_PORT_ADD(pscfld_mask, CBX_LAG_IKFT_FIELD_ID_SPG);
        } if (LAG_CNTL.psc[cbxPacketOther].flags & CBX_LAG_PSC_VLAN) {
            CBX_PBMP_PORT_ADD(pscfld_mask, CBX_LAG_IKFT_FIELD_ID_OUTER_TCI);
        } if (LAG_CNTL.psc[cbxPacketOther].flags & CBX_LAG_PSC_DSTMAC) {
            CBX_PBMP_PORT_ADD(pscfld_mask, CBX_LAG_IKFT_FIELD_ID_DSTMAC);
        } if (LAG_CNTL.psc[cbxPacketOther].flags & CBX_LAG_PSC_SRCMAC) {
            CBX_PBMP_PORT_ADD(pscfld_mask, CBX_LAG_IKFT_FIELD_ID_SRCMAC);
        }
        LAG_CNTL.pscfld_pbmp[cbxPacketOther] = pscfld_mask;
        break;
    /** IPv4 packets */
    case cbxPacketIpv4:
        oldpscfld_pbmp = LAG_CNTL.pscfld_pbmp[cbxPacketIpv4];
        LAG_CNTL.psc[cbxPacketIpv4].flags = psc->flags;
        rv = CBX_E_NONE;
        key_id = CBX_LAG_LBH_IPv4_KEY_ID;
        if (LAG_CNTL.psc[cbxPacketIpv4].flags & CBX_LAG_PSC_SPG) {
            CBX_PBMP_PORT_ADD(pscfld_mask, CBX_LAG_IKFT_FIELD_ID_SPG);
        } if (LAG_CNTL.psc[cbxPacketIpv4].flags & CBX_LAG_PSC_SRCIP) {
            CBX_PBMP_PORT_ADD(pscfld_mask, CBX_LAG_IKFT_FIELD_ID_SRCIP);
        } if (LAG_CNTL.psc[cbxPacketIpv4].flags & CBX_LAG_PSC_DSTIP) {
            CBX_PBMP_PORT_ADD(pscfld_mask, CBX_LAG_IKFT_FIELD_ID_DSTIP);
        } if (LAG_CNTL.psc[cbxPacketIpv4].flags & CBX_LAG_PSC_L3PROTO) {
            CBX_PBMP_PORT_ADD(pscfld_mask, CBX_LAG_IKFT_FIELD_ID_L3PROTO);
        } if (LAG_CNTL.psc[cbxPacketIpv4].flags & CBX_LAG_PSC_L4BYTES) {
            CBX_PBMP_PORT_ADD(pscfld_mask, CBX_LAG_IKFT_FIELD_ID_L4BYTES);
        }
        LAG_CNTL.pscfld_pbmp[cbxPacketIpv4] = pscfld_mask;
        break;
    /** IPv6 packets */
    case cbxPacketIpv6:
        oldpscfld_pbmp = LAG_CNTL.pscfld_pbmp[cbxPacketIpv6];
        LAG_CNTL.psc[cbxPacketIpv6].flags = psc->flags;
        rv = CBX_E_NONE;
        key_id = CBX_LAG_LBH_IPv6_KEY_ID;
        if (LAG_CNTL.psc[cbxPacketIpv6].flags & CBX_LAG_PSC_SPG) {
            CBX_PBMP_PORT_ADD(pscfld_mask, CBX_LAG_IKFT_FIELD_ID_SPG);
        } if (LAG_CNTL.psc[cbxPacketIpv6].flags & CBX_LAG_PSC_SRCIP) {
            CBX_PBMP_PORT_ADD(pscfld_mask, CBX_LAG_IKFT_FIELD_ID_SRCIP);
        } if (LAG_CNTL.psc[cbxPacketIpv6].flags & CBX_LAG_PSC_SRCIPV6) {
            CBX_PBMP_PORT_ADD(pscfld_mask, CBX_LAG_IKFT_FIELD_ID_SRCIPV6);
        } if (LAG_CNTL.psc[cbxPacketIpv6].flags & CBX_LAG_PSC_DSTIP) {
            CBX_PBMP_PORT_ADD(pscfld_mask, CBX_LAG_IKFT_FIELD_ID_DSTIP);
        } if (LAG_CNTL.psc[cbxPacketIpv6].flags & CBX_LAG_PSC_DSTIPV6) {
            CBX_PBMP_PORT_ADD(pscfld_mask, CBX_LAG_IKFT_FIELD_ID_DSTIPV6);
        } if (LAG_CNTL.psc[cbxPacketIpv6].flags & CBX_LAG_PSC_L3PROTO) {
            CBX_PBMP_PORT_ADD(pscfld_mask, CBX_LAG_IKFT_FIELD_ID_L3PROTO);
        } if (LAG_CNTL.psc[cbxPacketIpv6].flags & CBX_LAG_PSC_L4BYTES) {
            CBX_PBMP_PORT_ADD(pscfld_mask, CBX_LAG_IKFT_FIELD_ID_L4BYTES);
        }
        LAG_CNTL.pscfld_pbmp[cbxPacketIpv6] = pscfld_mask;
        break;
    default:
        break;
    }

    /* Check if it is cascade set-up */
    if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        num_units = CBX_MAX_UNITS;
    }

    /* Clear old IKFT fields */
    if (CBX_PBMP_NOT_NULL(oldpscfld_pbmp)) {
        /* coverity[overrun-local] */
        CBX_PBMP_ITER(oldpscfld_pbmp, psc_fld) {
            /* coverity[overrun-call] */
            for(unit=0; unit<num_units; unit++) {
                rv = cbxi_robo2_ikft_clear(unit, psc_fld, key_id);
                if (CBX_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_FSAL_LAG,
                              (BSL_META("FSAL API : cbx_lag_psc_set()..Failed\
                                         to update IKFT entry \n")));
                    return rv;
                }
            }
        }
    }

    psc_fld = 0;

    if (CBX_PBMP_NOT_NULL(pscfld_mask)) {
        /* coverity[overrun-local] */
        CBX_PBMP_ITER(pscfld_mask, psc_fld) {
            /* coverity[overrun-call] */
            for(unit=0; unit<num_units; unit++) {
            rv = cbxi_robo2_ikft_update(unit, psc_fld, key_id);
            if (CBX_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_FSAL_LAG,
                          (BSL_META("FSAL API : cbx_lag_psc_set()..Failed\
                                         to update IKFT entry \n")));
                return rv;
                }
            }
        }
    }

    return rv;
}

/**
 * Function:
 *    cbx_lag_member_get
 * Purpose :
 *    Get port member information for lag interface.
 * LAG Member get
 * This routine is used to get information including array of ports on a LAG.
 *
 * @param lagid            (IN)     LAG Identifier
 * @param lag_member_info  (IN/OUT) LAG Information structure
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */
int cbx_lag_member_get( cbx_portid_t lagid, cbx_lag_member_info_t *lag_info )
{

    int  size;
    cbx_lag_info_t  *l_info;
    cbx_port_t     port_out;
    cbx_port_t     lag_out;
    int            unit = CBX_AVENGER_PRIMARY;
    uint32         num_units = 1;
    cbx_portid_t   *portid_array;
    int            rv = CBX_E_NOT_FOUND;

    LAG_INIT;

    LOG_INFO(BSL_LS_FSAL_LAG,
             (BSL_META("FSAL API : cbxi_robo2_lag_member_get()..\n")));

    /* Check if portid is of LAG */
    if (!CBX_PORTID_IS_TRUNK(lagid)) {
        return CBX_E_PORT;
    }

    /* Validate the lag index passed */
    rv = cbxi_robo2_portid_validate(lagid, &lag_out);
    if (CBX_FAILURE(rv)) {
        return CBX_E_PORT;
    }

    LAG_CHECK(lag_out);

    l_info = &LAG_INFO(lag_out);

    if (l_info->lagid == CBX_LAG_INVALID) {
        return (CBX_E_PARAM);
    }

    if ((lag_info->max_portid > 0) && (lag_info->portid_array == NULL)) {
        return CBX_E_PARAM;
    }
    portid_array = lag_info->portid_array;

    /* Clear valid port count and port array */
    lag_info->valid_portid = 0;
    for(size=0; size<lag_info->max_portid; size++) {
        *portid_array = CBX_PORT_INVALID;
        portid_array++;
    }

    size = 0;
    portid_array = lag_info->portid_array;

    /* Check if lag exists */
    if ((l_info->in_use == TRUE) &&
            ((l_info->active[CBX_AVENGER_PRIMARY] >0) ||
            (l_info->active[CBX_AVENGER_SECONDARY] >0))) {
        LOG_INFO(BSL_LS_FSAL_LAG,
                 (BSL_META("FSAL API : cbx_lag_member_get()..LAG id =%d \
                            exists \n"), l_info->lagid));
        /*
         * Read the old trunk member pbmp
         */
        CBX_PBMP_COUNT(l_info->lag_pbmp[CBX_AVENGER_PRIMARY], size);
        lag_info->valid_portid = size;
        CBX_PBMP_COUNT(l_info->lag_pbmp[CBX_AVENGER_SECONDARY], size);
        lag_info->valid_portid += size;
        if (lag_info->valid_portid > lag_info->max_portid ) {
            LOG_ERROR(BSL_LS_FSAL_LAG,
                      (BSL_META("FSAL API : cbx_lag_member_get()..insufficient \
                  portid array! \n")));
            return CBX_E_PARAM;
        }

        /* Check if it is cascade set-up */
        if (SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
            num_units = CBX_MAX_UNITS;
        }

        for(unit=0; unit<num_units; unit++) {
            CBX_PBMP_ITER(l_info->lag_pbmp[unit], port_out) {
                *portid_array = (unit * CBX_MAX_PORT_PER_UNIT) + port_out;
                portid_array++;
            }
            rv = CBX_E_NONE;
        }
    }

    return rv;
}

/**
 * LAG reseed
 * Function:
 *    cbx_lag_psc_reseed
 * Purpose :
 *    Re-seed port selection criteria
 * LAG reseed
 * This routine is used to re-seed the Port Selection Criteria value.
 *
 * @param lagid     (IN)  LAG Identifier
 *                        when PSC is global, use CBX_LAG_ALL
 * @parm  mode      (IN)  LAG Reseed mode
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_lag_psc_reseed ( cbx_portid_t lagid, cbx_lag_reseed_mode_t mode )
{
    return CBX_E_UNAVAIL;
}
