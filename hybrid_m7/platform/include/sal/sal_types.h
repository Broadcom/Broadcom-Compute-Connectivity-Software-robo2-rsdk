/*
 * $Copyright: (c) 2017 Broadcom Limited.
 * All Rights Reserved.$
 * File:
 *    sal_types.h
 * Description
 *    sal_types.h
 *
 */

#ifndef _SAL_TYPES_H_
#define _SAL_TYPES_H_

typedef unsigned char       uint8;
typedef unsigned short      uint16;
#ifdef CONFIG_EXTERNAL_HOST
typedef unsigned int       uint32;
#else
typedef unsigned long       uint32;
#endif
typedef unsigned long long  uint64;
#ifndef _STDINT_H
typedef unsigned char       uint8_t;
typedef unsigned short      uint16_t;
#ifdef CONFIG_EXTERNAL_HOST
typedef unsigned int       uint32_t;
#else
typedef unsigned long       uint32_t;
#endif
typedef unsigned long long  uint64_t;
#endif

typedef signed char         int8;
typedef signed short        int16;
#ifdef CONFIG_EXTERNAL_HOST
typedef signed int         int32;
#else
typedef signed long         int32;
#endif
typedef uint8               BOOL;
#define TRUE                (1)
#define FALSE               (0)

#ifndef NULL
#define NULL                (0)
#endif

#ifndef NULLPTR
#define NULLPTR (void *)0
#endif

#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned int size_t;
#endif

typedef unsigned long         sal_vaddr_t; /* Virtual address (Host address) */
#define PTR_TO_INT(x)        ((unsigned long)(x))
#define INT_TO_PTR(x)        ((void *)((sal_vaddr_t)(x)))

#define PTR_TO_UINTPTR(x)    ((sal_vaddr_t)(x))
#define UINTPTR_TO_PTR(x)    ((void *)(x))


#define COMPILER_64_IS_ZERO(src)    ((src) == 0)
#define COMPILER_64_LT(src1, src2)    ((src1) <  (src2))
#define COMPILER_64_ZERO(dst)       ((dst) = 0)
#define COMPILER_64_EQ(src1, src2)  ((src1) == (src2))
#define COMPILER_64_NE(src1, src2)  ((src1) != (src2))
#define COMPILER_64_SET(dst, src_hi, src_lo)                \
         ((dst) = (((uint64) ((uint32)(src_hi))) << 32) |   \
               ((uint64) ((uint32)(src_lo))))

#define COMPILER_64_ADD_64(dst, src)    ((dst) += (src))
#define COMPILER_64_SUB_64(dst, src)    ((dst) -= (src))

#define COMPILER_64_SHL(dst, bits)    ((dst) <<= (bits))
#define COMPILER_64_SHR(dst, bits)    ((dst) >>= (bits))
#define COMPILER_64_BITTEST(val, n)     \
    ((((uint64)val) & (((uint64) 1)<<(n))) != ((uint64) 0))
#define COMPILER_64_GE(src1, src2)    ((src1) >= (src2))
#define COMPILER_64_IS_ZERO(src)    ((src) == 0)
#define COMPILER_64_HI(src)     ((uint32) ((src) >> 32))
#define COMPILER_64_LO(src)     ((uint32) (src))

#define UNREFERENCED_PARAMETER(x)   do { x = x; } while(0)

#define min(a,b) ((a) < (b)? (a) : (b))
#define max(a,b) ((a) > (b)? (a) : (b))


#define SAL_BIT(x)  (1 << (x))
#define SAL_RBIT(x) (~(1<<(x)))

/*******************************************
 * SET ALL BITS, BEGINNING AT BIT X, TO 1  *
 *******************************************/
#define SAL_FROM_BIT(x)     (SAL_RBIT(x) + 1)

/*******************************************
 * SET ALL BITS UP TO BIT X (NOT INCLUDING *
 * BIT X) TO 1.                            *
 *******************************************/
#define SAL_UPTO_BIT(x)     (~(SAL_FROM_BIT(x)))


#define BITS2BYTES(x)        (((x) + 7) / 8)
#define BITS2WORDS(x)        (((x) + 31) / 32)

#define BYTES2BITS(x)        ((x) * 8)
#define BYTES2WORDS(x)        (((x) + 3) / 4)

#define WORDS2BITS(x)        ((x) * 32)
#define WORDS2BYTES(x)        ((x) * 4)

#define COUNTOF(ary)        ((int) (sizeof (ary) / sizeof ((ary)[0])))



/* sal_mac_addr_t mac;  Just generate a list of the macs for display */
#define SAL_MAC_ADDR_LIST(mac) \
    mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]

/* sal_mac_addr_t m1, m2;  True if equal */
#define SAL_MAC_ADDR_EQUAL(m1, m2) (!sal_memcmp(m1, m2, sizeof(sal_mac_addr_t)))

/* sal_mac_addr_t m1, m2;  like memcmp, returns -1, 0, or 1 */
#define SAL_MAC_ADDR_CMP(m1, m2) sal_memcmp(m1, m2, sizeof(sal_mac_addr_t))

/* Device bus types */
#define SAL_SPI_DEV_TYPE        0x00002 /* SPI device */

/* Device types */
#define SAL_SWITCH_DEV_TYPE     0x00100 /* Switch device */
#define SAL_ETHER_DEV_TYPE      0x00200 /* Ethernet device */
#define SAL_CPU_DEV_TYPE        0x00400 /* CPU device */
#define SAL_DEV_TYPE_MASK       0x00f00


#endif /* _SAL_TYPES_H_ */
