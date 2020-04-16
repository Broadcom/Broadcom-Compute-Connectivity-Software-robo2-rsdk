/* *
 * * $ID: $
 * * 
 * * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 * * 
 * * $Copyright: (c) 2020 Broadcom Inc.
 * *
 * * File:
 * *     types.h
 * * Purpose:
 * *     This file contains internal FSAL module type definitions.
 */

#ifndef CBX_INT_TYPES_H
#define CBX_INT_TYPES_H

#define CBX_AVENGER_PRIMARY    0
#define CBX_AVENGER_SECONDARY  1
#define CBX_MAX_UNITS          2
#define CBX_MAX_PORT_PER_UNIT  16
#define CBX_MAX_LAG_PER_UNIT   (CBX_MAX_PORT_PER_UNIT - 2)
#define CBXI_GLOBAL_PORT(_port, _unit)   (_port + (unit * CBX_MAX_PORT_PER_UNIT))


#define CBX_PBMP_PORT_MAX       _SHR_PBMP_PORT_MAX

#define CBX_PBMP_CLEAR(pbm)     _SHR_PBMP_CLEAR(pbm)
#define CBX_PBMP_MEMBER(bmp, port)  _SHR_PBMP_MEMBER((bmp), (port))
#define CBX_PBMP_ITER(bmp, port)  _SHR_PBMP_ITER((bmp), (port))
#define CBX_PBMP_COUNT(pbm, count)  _SHR_PBMP_COUNT(pbm, count)

#define CBX_PBMP_IS_NULL(pbm)   _SHR_PBMP_IS_NULL(pbm)
#define CBX_PBMP_NOT_NULL(pbm)  _SHR_PBMP_NOT_NULL(pbm)
#define CBX_PBMP_EQ(pbm_a, pbm_b)  _SHR_PBMP_EQ(pbm_a, pbm_b)
#define CBX_PBMP_NEQ(pbm_a, pbm_b)  _SHR_PBMP_NEQ(pbm_a, pbm_b)

/* Assignment operators */
#define CBX_PBMP_ASSIGN(dst, src)  _SHR_PBMP_ASSIGN(dst, src)
#define CBX_PBMP_AND(pbm_a, pbm_b)  _SHR_PBMP_AND(pbm_a, pbm_b)
#define CBX_PBMP_OR(pbm_a, pbm_b)  _SHR_PBMP_OR(pbm_a, pbm_b)
#define CBX_PBMP_XOR(pbm_a, pbm_b)  _SHR_PBMP_XOR(pbm_a, pbm_b)
#define CBX_PBMP_REMOVE(pbm_a, pbm_b)  _SHR_PBMP_REMOVE(pbm_a, pbm_b)
#define CBX_PBMP_NEGATE(pbm_a, pbm_b)  _SHR_PBMP_NEGATE(pbm_a, pbm_b)

/* Port PBMP operators */
#define CBX_PBMP_PORT_SET(pbm, port)  _SHR_PBMP_PORT_SET(pbm, port)
#define CBX_PBMP_PORT_ADD(pbm, port)  _SHR_PBMP_PORT_ADD(pbm, port)
#define CBX_PBMP_PORT_REMOVE(pbm, port)  _SHR_PBMP_PORT_REMOVE(pbm, port)
#define CBX_PBMP_PORT_FLIP(pbm, port)  _SHR_PBMP_PORT_FLIP(pbm, port)

typedef uint32_t cbxi_pgid_t; /* INTERNAL: This is a generic typedef for pgid
                               * which can be used to represent local pgid(0-15)
                               * or Global port groap (0-31) */

#define CBXI_PGID_INVALID      0xffffffff

#define CBXI_CASC_PARTNER_UNIT(u)       (u ^ 0x1)



/* Logical Interface ID(LIID) and Local Logical Interface (LLIID / LLI) checks*/

/*******************  LIID format *******************************/
/*
| 17 |16 | 15 | 14 | 13 | 12 | 11 | 10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
| T=0|          RSVD              |             N[10:0]                        |
| T=1|         PG[4:0]       |                 VSI[11:0]                       |
*/
/* LIN type LIID: LIID[17] = 0;                      ; LIID[10:0] = N[10:0] */
/* PV type LIID:  LIID[17] = 1; LIID[16:12] = PG[4:0]; LIID[11:0] = VSI[11:0] */
#define CBXI_LIID_TYPE_PV         1
#define CBXI_LIID_TYPE_LIN        0
#define CBXI_LIID_TYPE_SHIFT     17
#define CBXI_LIID_PG_SHIFT       12

#define CBXI_PV_LIID_SET(_pg, _vsi) \
        ((CBXI_LIID_TYPE_PV << CBXI_LIID_TYPE_SHIFT) | (_pg << CBXI_LIID_PG_SHIFT) | (_vsi))
#define CBXI_LIN_LIID_SET(_lin) \
        ((CBXI_LIID_TYPE_LIN << CBXI_LIID_TYPE_SHIFT) | (_lin))

#define CHECK_LIID_IS_PV(_liid) \
        (((_liid > 0) && ((_liid >> CBXI_LIID_TYPE_SHIFT) == CBXI_LIID_TYPE_PV)) ? 1 : 0)
#define CHECK_LIID_IS_LIN(_liid) \
        (((_liid > 0) && ((_liid >> CBXI_LIID_TYPE_SHIFT) == CBXI_LIID_TYPE_LIN)) ? 1 : 0)


/*******************  LLI format *****************/
/*
| 11 | 10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
| T=0|           N[10:0]                          |
| T=1|       RSVD             |   PG[4:0]         |
*/
/* LIN type LLI: LLI[11] = 0; LLI[10:0] = N[10:0] */
/* PV type LLI:  LLI[11] = 1; LLI[4:0]  = PG[4:0]; */

#define CBXI_LLI_TYPE_PV         1
#define CBXI_LLI_TYPE_LIN        0
#define CBXI_LLI_TYPE_SHIFT     11
#define CBXI_LLI_PG_SHIFT        0

#define CBXI_PV_LLI_SET(_pg) \
        ((CBXI_LLI_TYPE_PV << CBXI_LLI_TYPE_SHIFT) | (_pg << CBXI_LLI_PG_SHIFT))
#define CBXI_LIN_LLI_SET(_lin) \
        ((CBXI_LLI_TYPE_LIN << CBXI_LLI_TYPE_SHIFT) | (_lin))

#define CHECK_LLI_IS_PV(_lli) \
        (((_lli > 0) && ((_lli >> CBXI_LLI_TYPE_SHIFT) == CBXI_LLI_TYPE_PV)) ? 1 : 0)
#define CHECK_LLI_IS_LIN(_lli) \
        (((_lli > 0) && ((_lli >> CBXI_LLI_TYPE_SHIFT) == CBXI_LLI_TYPE_LIN)) ? 1 : 0)

#endif  /* !_CBX_INT_TYPES_H */
