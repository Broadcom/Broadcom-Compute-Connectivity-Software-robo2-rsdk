/*
 * $Id: pbmp.h,v 1.22 2012/01/16 18:06:25 dayad Exp $
 * $Copyright: (c) 2017 Broadcom Corp.
 * All Rights Reserved.$
 *
 * Port Bitmap Macros
 */

#ifndef _SHR_PBMP_H
#define _SHR_PBMP_H


/*
 * Port Bitmap Macros
 *
 * These macros are never used directly (well, except in the C code
 * support routines that are used to help implement the macros).
 * For many macro here, there are BCM_* and SOC_* versions that are
 * defined in <bcm/types.h> and <soc/types.h>. SDK and customer code
 * should use those macros, not these.
 *
 * This header requires that the uint32 type be defined before inclusion.
 * Using <sal/types.h> is the simplest (and recommended) way of doing
 * this.
 *
 * There following macros are made available.  All have names starting
 * with _SHR_, which have been elided from this list:
 *
 * Constants or Expressions suitable for assignment:
 * PBMP_PORT_MAX		the maximum number of ports supported
 * PBMP_WORD_MAX		the maximum number of words in a bitmap
 * PBMP_WORD_GET(b, w)		return word w (0..WORD_MAX-1) from bitmap b
 * PBMP_FMT_LEN			length of char[] array for PBMP_FMT
 * PBMP_FMT(b,s)		%s'able format of bitmap b into string s
 *				returns s
 *
 * Predicates: (return 0 or 1, suitable for using in if statements)
 * PBMP_MEMBER(b, p)		is port p a member of bitmap b?
 * PBMP_IS_NULL(b)		is bitmap b empty?
 * PBMP_NOT_NULL(b)		is bitmap b not empty?
 * PBMP_EQ(b1, b2)		are bitmaps b1 and b2 equal?
 * PBMP_NEQ(b1, b2)		are bitmaps b1 and b2 not equal?
 * PBMP_PORT_VALID(p)		is p a valid port number? (0..PORT_MAX)
 *
 * Statements: (cannot be used as a predicate)
 * PBMP_CLEAR(b)		zero out a bitmap
 * PBMP_COUNT(b, c)		store how many bits are on in b into integer c
 * PBMP_ITER(b, p) {...}	iterate over bitmap b, setting p to each bit
 * PBMP_ASSIGN(b1, b2)		copy bitmap b2 into b1
 * PBMP_AND(b1, b2)		and bitmap b2 into b1
 * PBMP_OR(b1, b2)		or bitmap b2 into b1
 * PBMP_XOR(b1, b2)		exclusive or bitmap b2 into b1
 * PBMP_REMOVE(b1, b2)		remove bits in bitmap b2 from b1
 * PBMP_NEGATE(b1, b2)		copy the bitwise negation of bitmap b2 into b1
 * PBMP_PORT_SET(b, p)		clear bitmap b, then turn bit p on
 * PBMP_PORT_ADD(b, p)		turn bit p on in bitmap b
 * PBMP_PORT_REMOVE(b, p)	turn bit p off in bitmap b
 * PBMP_PORT_FLIP(b, p)		flip the sense of bit p on in bitmap b
 * PBMP_WORD_SET(b, w, v)	set word w (0..WORD_MAX-1) from bitmap b to v
 *
 * Internal forms: (should not be used outside of this header file)
 * PBMP_WENT(p)			word index for bit p
 * PBMP_WBIT(p)			word bitmask for bit p
 * PBMP_BMCLEAR(b)		clear a bitmap
 * PBMP_BMNULL(b)		is bitmap b empty?
 * PBMP_BMEQ(b1, b2)		are bitmaps b1 and b2 equal?
 * PBMP_BMOP(b1, b2, op)	execute op on a word basis on bitmaps b1, b2
 * PBMP_ENTRY(b, p)		the word of bitmap b that holds bit p
 *
 */

/* No. of ports will not exceed 32 in Avenger. uint32 is sufficient for pbmp */

typedef uint32 _shr_pbmp_t;

#ifndef	_SHR_PBMP_PORT_MAX
#define	_SHR_PBMP_PORT_MAX		32
#endif
#define	_SHR_PBMP_WORD_MAX		1
#define	_SHR_PBMP_WBIT(port)		(1U<<(port))
#define	_SHR_PBMP_WORD_GET(pbm, word)	(pbm)
#define	_SHR_PBMP_WORD_SET(pbm, word, val)	((pbm) = (val))

#define _SHR_PBMP_CLEAR(pbm)		((pbm) = 0)
#define _SHR_PBMP_MEMBER(bmp, port)	(((bmp) & _SHR_PBMP_WBIT(port)) != 0)
#define	_SHR_PBMP_COUNT(bmp, count)	(count = _shr_popcount(bmp))
#define _SHR_PBMP_ITER(bmp, port) \
        for ((port) = 0; (port) < 32; (port)++) \
            if (_SHR_PBMP_MEMBER((bmp), (port)))

#define _SHR_PBMP_IS_NULL(pbm)           ((pbm) == 0)
#define _SHR_PBMP_NOT_NULL(pbm)          ((pbm) != 0)
#define _SHR_PBMP_EQ(pbm_a, pbm_b)       ((pbm_a) == (pbm_b))
#define _SHR_PBMP_NEQ(pbm_a, pbm_b)      ((pbm_a) != (pbm_b))

/* Assignment operators */
#define _SHR_PBMP_ASSIGN(dst, src)       (dst) = (src)
#define _SHR_PBMP_AND(pbm_a, pbm_b)      ((pbm_a) &= (pbm_b))
#define _SHR_PBMP_OR(pbm_a, pbm_b)       ((pbm_a) |= (pbm_b))
#define _SHR_PBMP_XOR(pbm_a, pbm_b)      ((pbm_a) ^= (pbm_b))
#define _SHR_PBMP_REMOVE(pbm_a, pbm_b)   ((pbm_a) &= ~(pbm_b))
#define _SHR_PBMP_NEGATE(pbm_a, pbm_b)   ((pbm_a) = ~(pbm_b))

/* Port PBMP operators */
#define _SHR_PBMP_PORT_SET(pbm, port)    ((pbm) = (1U << (port)))
#define _SHR_PBMP_PORT_ADD(pbm, port)    ((pbm) |= (1U << (port)))
#define _SHR_PBMP_PORT_REMOVE(pbm, port) ((pbm) &= ~(1U << (port)))
#define _SHR_PBMP_PORT_FLIP(pbm, port)	 ((pbm) ^= (1U << (port)))

#define _SHR_PBMP_PORTS_RANGE_ADD(bm, first_port, range) \
    do {                                                 \
        uint32 _mask_ = ((1 << range) - 1);              \
        _mask_ <<= first_port;                           \
        bm |= _mask_;                                    \
    } while (0); 


extern char		*_shr_pbmp_format(_shr_pbmp_t, char *);
extern int		_shr_pbmp_decode(char *, _shr_pbmp_t *);

#define	_SHR_PBMP_FMT(bm, buf)		_shr_pbmp_format(bm, buf)
#define	_SHR_PBMP_FMT_LEN		((_SHR_PBMP_WORD_MAX*8)+3)

#define	_SHR_PBMP_PORT_VALID(p)		((p) >= 0 && (p) < _SHR_PBMP_PORT_MAX)

#endif	/* !_SHR_PBMP_H */
