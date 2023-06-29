/*
 * * $ID: $
 * * $Copyright: (c) 2017 Broadcom Corp.
 * * All Rights Reserved$
 * *
 * * File:
 * *     sal_alloc.c
 * *
 * */


#include <assert.h>
#include <sal_types.h>
#include <sal_alloc.h>
#include <stdlib.h>
#include <string.h>


/* To do: use real data segment limits for bad pointer detection */
#define BAD_PTR(p)						\
        (PTR_TO_UINTPTR(p) < 0x1000UL ||			\
         PTR_TO_UINTPTR(p) > ~0xfffUL)
#define CORRUPT(p)						\
	(p[-1] != 0xaaaaaaaa ||					\
	 p[p[-2]] != 0xbbbbbbbb)

static unsigned long	sal_alloc_bytes;
static unsigned long	sal_free_bytes;

extern int sal_printf(const char *fmt, ...);


void sal_get_alloc_counters(unsigned long	*alloc_bytes_count,unsigned long	*free_bytes_count)
{
    *alloc_bytes_count = sal_alloc_bytes;
    *free_bytes_count = sal_free_bytes;

}

/*
 * Function:
 *	sal_alloc
 * Purpose:
 *	Allocate general purpose system memory.
 * Parameters:
 *	sz - size of memory block to allocate.
 *	s - optional user description of memory block for debugging.
 * Returns:
 *	Pointer to memory block
 * Notes:
 *	Memory allocated by this routine is not guaranteed to be safe
 *	for hardware DMA read/write.
 */

#ifdef SAL_MEMORY_TRACE
void *sal_alloc_int(unsigned int num_bytes, char *mem_id, const char *s, int line)
{
    void *ret;
    ret = (void *)malloc(num_bytes);
    
    sal_printf("SMT: sal_alloc at [%s:%u]: [ptr = %x] [size = %d]\n", s, line, ret, num_bytes);
    return ret;
}
#else
void *sal_alloc(unsigned int sz, char *s)
{
    return malloc(sz);
}
#endif


/*
 * Function:
 *	sal_free
 * Purpose:
 *	Free memory block allocate by sal_alloc
 * Parameters:
 *	addr - Address returned by sal_alloc
 */

#ifdef SAL_MEMORY_TRACE
void sal_free_int(void *ptr, const char *s, int line)
{
    sal_printf("SMT: sal_free at [%s:%u]: [ptr = %x]\n", s, line, ptr);
    free(ptr);
}
#else
void sal_free(void *ptr)
{
    free(ptr);
}
#endif


/*
 * Function:
 *	sal_dma_alloc
 * Purpose:
 *	Allocate memory that can be DMA'd into/out of.
 * Parameters:
 *	sz - number of bytes to allocate
 *	s - string associated with allocate
 * Returns:
 *	Pointer to allocated memory or NULL if out of memory.
 * Notes:
 *	Memory allocated by this routine is not guaranteed to be safe
 *	for hardware DMA read/write. This is for use only on sim platform.
 */

void *
sal_dma_alloc(size_t sz, char *s)
{
    uint32	*p;

    /*
     * Round up size to accommodate corruption detection sentinels.
     * Place sentinels at the beginning and end of the data area to
     * detect memory corruption.  These are verified on free.
     */
    sz = (sz + 3) & ~3;

    if ((p = malloc(sz + 12)) == 0) {
        return p;
    }

    assert(INT_TO_PTR(PTR_TO_INT(p)) == p);

    p[0] = sz / 4;
    p[1] = 0xaaaaaaaa;
    p[2 + sz / 4] = 0xbbbbbbbb;

    return (void *) &p[2];
}

/*
 * Function:
 *	sal_dma_free
 * Purpose:
 *	Free memory allocated by sal_dma_alloc
 * Parameters:
 *	addr - pointer to memory to free.
 * Returns:
 *	Nothing.
 */

void
sal_dma_free(void *addr)
{
    uint32	*p	= (uint32 *)addr;
    uint32  *ap = p; /* Originally Allocated Pointer */

    /*
     * Verify sentinels on free.  If this assertion fails, it means that
     * memory corruption was detected.
     */

    /*    coverity[conditional (1): FALSE]    */
    /*    coverity[conditional (2): FALSE]    */
    assert(!BAD_PTR(p));	/* Use macro to beautify assert message */
    /*    coverity[conditional (3): FALSE]    */
    /*    coverity[conditional (4): FALSE]    */
    assert(!CORRUPT(p));	/* Use macro to beautify assert message */


    /* Adjust for Corruption detection Sentinels */
    ap--;
    ap--;

    ap[1] = 0;			/* Detect redundant frees */
    /*    coverity[address_free : FALSE]    */
    free(ap);
}

/*
 * Function:
 *	sal_calloc
 * Purpose:
 *  Allocate general purpose system memory and clears the contents of
 *  allocated memory.
 * Parameters:
 *	nmemb - number of member elements
 *	size - size of each member element.
 * Returns:
 *	Pointer to memory block
 * Notes:
 *	Memory allocated by this routine is not guaranteed to be safe
 *	for hardware DMA read/write.
 */

void *
sal_calloc(unsigned int nmemb, unsigned int size )
{
    return calloc(nmemb, size);
}

/*
 * Function:
 *    sal_realloc
 * Purpose:
 *    Reallocate general purpose system memory for newly requested size
 *    and release old memory.
 * Parameters:
 *    old_ptr - Pointer to memory block allocated previously
 *    old_bytes - size of old memory block allocated
 *    new_bytes - size of memory block to allocate
 * Returns:
 *    Success - Pointer to reallocated memory block.
 *    Failure - Null pointer.
 * Notes:
 *    Memory allocated by this routine is not guaranteed to be safe
 *    for hardware DMA read/write. mem_id descriptior is optional and not
 *    used in OSAL implementation for freertos.
 */

void *sal_realloc(void *old_ptr,
                  unsigned int old_bytes,
                  unsigned int new_bytes )
{
    return realloc(old_ptr, new_bytes);

}

/*
 * Function:
 *    sal_print_mem_stats
 * Purpose:
 *    Print memory usage statistics
 * Parameters:
 *    None
 * Returns:
 *    None
 * Notes:
 *    Prints the Total available heap size, watermarks, available size as applicable
 */
void sal_print_mem_stats(void)
{
   
}

/*
 * Function:
 *    sal_memcmp
 * Purpose:
 *    Compares the n bytes of the memory contained in s1 and s2 and each entry
 *    interpreted as unsigned char.
 * Parameters:
 *    s1 -pointer to first entry to compare, entry interpreted as unsigned char
 *    s2 -pointer to second entry to compare, entry interpreted as unsigned char
 *    cnt- number of bytes to compare

 * Returns:
 *    Function returns an integer less than, equal to, or greater than zero.
 */
int  sal_memcmp(const void *s1, const void *s2, size_t cnt)
{

    const unsigned char *d;
    const unsigned char *s;

    d = (const unsigned char *) s1;
    s = (const unsigned char *) s2;

    while (cnt) {
        if (*d < *s) return -1;
        if (*d > *s) return 1;
        d++;
        s++;
        cnt--;
    }

    return 0;

}
/*
 * Function:
 *    sal_memcpy
 * Purpose:
 *    Copies count bytes of the memory from src memory location to dest memory
 *    location.
 * Parameters:
 *    dest - pointer to destination memory location
 *    src - pointer to source memory location
 *    cnt - number of bytes to copy from source to destination

 * Returns:
 *    Function returns pointer to destination memory location.
 */
void *sal_memcpy(void *dest, const void *src, size_t cnt)
{
    unsigned char *d;
    const unsigned char *s;
    d = (unsigned char *) dest;
    s = (const unsigned char *) src;

    while (cnt) {
        *d++ = *s++;
        cnt--;
    }

    return dest;
}

/*
 * Function:
 *    sal_memset
 * Purpose:
 *    Fills cnt bytes of the memory pointed to by dest pointer with constant
 *    bytes c value.
 * Parameters:
 *    dest - pointer to destination memory location
 *    c - constant value to fill the memory
 *    cnt- number of bytes to fill

 * Returns:
 *    Function returns pointer to destination memory location.
 */
void *sal_memset(void *dest, int c, size_t cnt)
{

    unsigned char *d;

    d = dest;

    while (cnt) {
        *d++ = (unsigned char) c;
        cnt--;
    }

    return dest;
}

/*
 * Function:
 *    sal_memmove
 * Purpose:
 *    Copies count bytes of the memory from src memory location to dest memory
 *    location. The src and dest memory regions may be overlapping
 * Parameters:
 *    dest - pointer to destination memory location
 *    src - pointer to source memory location
 *    cnt - number of bytes to copy from source to destination

 * Returns:
 *    Function returns pointer to destination memory location.
 */
void *sal_memmove(void *dest, const void *src, size_t n)
{
   return memmove(dest, src, n);
} 
