/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 */

/* 
 * types.h
 */

#ifndef _TYPES_H_
#define _TYPES_H_

typedef unsigned char       uint8;
typedef unsigned short      uint16;
typedef unsigned long       uint32;

typedef signed char         int8;
typedef signed short        int16;
typedef signed long         int32;

typedef uint8               BOOL;
#define TRUE                (1)
#define FALSE               (0)

#ifndef NULL
#define NULL                (0)
#endif

#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned int size_t;
#endif

/**
 * 8 bit datatype
 *
 * This typedef defines the 8-bit type used throughout uIP.
 *
 * \hideinitializer
 */
typedef uint8 u8_t;
typedef uint8 uint8_t;
#ifndef M7_COMPILE
typedef char int8_t;
#endif /* ifndef M7_COMPILE */

/**
 * 16 bit datatype
 *
 * This typedef defines the 16-bit type used throughout uIP.
 *
 * \hideinitializer
 */
typedef uint16 u16_t;
typedef uint16 uint16_t;
typedef int16  int16_t;

/**
 * 32 bit datatype
 *
 * This typedef defines the 32-bit type used throughout uIP.
 *
 * \hideinitializer
 */
typedef uint32 u32_t;
typedef uint32 uint32_t;
typedef int32  int32_t;

#define UNREFERENCED_PARAMETER(x)   do { x = x; } while(0)

#define min(a,b) ((a) < (b)? (a) : (b))
#define max(a,b) ((a) > (b)? (a) : (b))

#endif /* _TYPES_H_ */
