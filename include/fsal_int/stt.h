/* *
 * * $ID: $
 * * $Copyright: (c) 2017 Broadcom Limited.
 * *
 * * File:
 * *     stt.h
 * * Purpose:
 * *     This file contains definitions internal to Soft Tag Types  module.
 */

#ifndef CBX_INT_SOFT_TAG_TYPES_H
#define CBX_INT_SOFT_TAG_TYPES_H

/*
 * STT Indexes, there are 6 entries to use
 */
#define CBX_STT_INDEX_CP_TO_SWITCH     0
//#define CBX_STT_INDEX_SWITCH_TO_CP     1
#define CBX_STT_INDEX_SWITCH_TO_CP_TS  1

/*
 * STT Etypes
 */
#define CBX_STT_ETYPE_CP_TO_SWITCH       0xAAB1
#define CBX_STT_ETYPE_SWITCH_TO_CP       0xAAB2
#define CBX_STT_ETYPE_SWITCH_TO_CP_TS    0xBBCC
#define CBX_STT_ETYPE_BRCM_TAG           0xAAAA

#endif /* CBX_INT_SOFT_TAG_TYPES_H */
