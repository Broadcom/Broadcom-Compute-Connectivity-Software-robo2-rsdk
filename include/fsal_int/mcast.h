/* *
 * * $ID: $
 * * $Copyright: (c) 2017 Broadcom Limited.
 * *
 * * File:
 * *     mcast.h
 * * Purpose:
 * *     This file contains definitions internal to L2 multicast module.
 */

#ifndef CBX_INT_MCAST_H
#define CBX_INT_MCAST_H

#include <shared/types.h>
#include <shared/bitop.h>
#include <fsal/mcast.h>
#include <fsal_int/types.h>

/* Member update flags */
#define CBXI_MCAST_MEMBER_REMOVE     0x1
#define CBXI_MCAST_MEMBER_ADD        0x2
#define CBXI_MCAST_MEMBER_UPDATE     0x4
#define CBXI_MCAST_MEMBER_LILT_REL   0x8 /* Release the lin slot for PE Cascade port */


#define CHECK_MCAST_INIT(cmi)         \
    if (!cmi->init)             \
        return CBX_E_INIT

#define CHECK_MCAST(cmi, mcast)     \
    if ((mcast) < cmi->mcast_min || (mcast) > cmi->mcast_max)         \
        return CBX_E_BADID

#define CBXI_MCAST_LOWER_16_MASK                0xFFFF
#define CBXI_MCAST_SHIFT_16                     16

/* Macros to form Packed DLI  */
#define CBXI_MCAST_PACKED_DLI_PV_GET(gpgid)     (CBXI_PV_LLI_SET(gpgid))
#define CBXI_MCAST_PACKED_DLI_LIN_GET(n_val)    (CBXI_LIN_LLI_SET(n_val))

#define CHECK_PACKED_DLI_PV(dliid)              (CHECK_LLI_IS_PV(dliid))
#define CHECK_PACKED_DLI_LIN(dliid)             (CHECK_LLI_IS_LIN(dliid))
#define CHECK_NORMAL_DLI_PV(dliid)              (CHECK_LIID_IS_PV(dliid))
#define CHECK_NORMAL_DLI_LIN(dliid)             (CHECK_LIID_IS_LIN(dliid))

/* LILT table is divided into 3 parts:
 * 1) M arrays of packed DLIIDs.  each array having 16 DLIIDs @ 8 entries
 * 2) N arrays of DLIIDs. each array having 16 DLIID list elements @ 16
 *   entries
 * 3) balance of table with free pool of DLIID list elements
 *
 * For FSAL v1, M = 512, N = 0, free pool = 0
 * Maintain a bitmap of 512 arrays to provide a free slot of array in LILT in
 * case of cbxMcastLiltModeArray.
 * */

#define CBXI_LILT_ARRAY_MAX             512

typedef struct cbx_mcast_sw_info_s {
    int           init;        /* TRUE if MCAST module has been initialized */
    uint32_t      mcast_min;   /* MCAST table min index */
    uint32_t      mcast_max;   /* MCAST table max index */
    SHR_BITDCL    *mcast_bitmap; /* Bitmap of allocated MCAST ids */
    SHR_BITDCL    *lilt_bitmap; /* Bitmap of allocated LILT ndx arrays */
    int           mcast_count;   /* Number MCASTs allocated */
} cbx_mcast_sw_info_t;

extern int cbxi_mcast_init(void);
extern int cbxi_mcast_id_check(cbx_mcastid_t mcastid);
extern int cbxi_mcast_entry_set(int unit, uint32_t  mcast,
                        cbxi_pgid_t lpg_new, uint32_t lilt_ent, int flags);
#endif
