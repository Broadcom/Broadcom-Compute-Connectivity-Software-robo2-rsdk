/*
 * * $ID: $
 * * 
 * * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 * * 
 * * $Copyright: (c) 2020 Broadcom Inc.
 * * All Rights Reserved$
 * *
 * * File:
 * *     sal_alloc.c
 * * Purpose:
 * *     Robo2 openrtos osal module. This provides abstraction for openrtos
 * *     memory related service.
 * *
 * */

#ifdef HYBRID_M7_MODE
#include <common/system.h>
#else

#include "sal_alloc.h"
#include "FreeRTOS.h"

#define SAL_DMA_ALIGN 32
#define SAL_DMA_ALIGNED(addr, align) (((uint32)(addr) + (align)) & ~((align)-1));
/*
 * Function:
 *    sal_alloc
 * Purpose:
 *    Allocate general purpose system memory.
 * Parameters:
 *    num_bytes - size of memory block to allocate
 *    mem_id - optional user description of memory block for debugging
 * Returns:
 *    Success - Pointer to memory block.
 *    Failure - Null pointer.
 * Notes:
 *    Memory allocated by this routine is not guaranteed to be safe
 *    for hardware DMA read/write. mem_id descriptior is optional and not
 *    used in OSAL implementation for freertos.
 */

#ifdef SAL_MEMORY_TRACE
void *sal_alloc_int(unsigned int num_bytes, char *mem_id, const char *s, int line)
{
    void *ret;
    ret = (void *)pvPortMalloc(num_bytes);

    sal_printf("SMT: sal_alloc at [%s:%u]: [ptr = %x] [size = %d]\n", s, line, ret, num_bytes);
    return ret;
}
#else
void *sal_alloc(unsigned int num_bytes, char *mem_id)
{
    return (void *)pvPortMalloc(num_bytes);
}
#endif

/*
 * Function:
 *    sal_free
 * Purpose:
 *    Free memory block allocated by sal_alloc.
 * Parameters:
 *    ptr - pointer to memory block returned by sal_alloc
 * Returns: None
 *
 */
#ifdef SAL_MEMORY_TRACE
void sal_free_int(void *ptr, const char *s, int line)
{
    sal_printf("SMT: sal_free at [%s:%u]: [ptr = %x]\n", s, line, ptr);
    vPortFree(ptr);
}
#else
void sal_free(void *ptr)
{
    vPortFree(ptr);
}
#endif

/*
 * Function:
 *    sal_dma_alloc
 * Purpose:
 *    Allocate shareable memory for communication with the device.
 * Parameters:
 *    num_bytes - size of memory block to allocate
 *    mem_id - optional user description of memory block for debugging
 * Returns:
 *    Pointer to memory block on success, Null pointer on failure.
 */

uint32 sal_dma_allocs = 0;
uint32 sal_dma_frees = 0;
void *sal_dma_alloc(unsigned int num_bytes, char *mem_id)
{
    uint32 *addrp = NULL;
    uint32 *userptr = NULL;
    uint32 numwords = (num_bytes + SAL_DMA_ALIGN + 4) >> 2;  /* account for our usage separately */
    addrp = (uint32 *)pvPortMalloc(numwords << 2);
    userptr = (uint32*)SAL_DMA_ALIGNED(addrp ,SAL_DMA_ALIGN);
    userptr[-1] = (uint32)addrp;
    sal_dma_allocs++;
    return (void*)userptr;
}

/*
 * Function:
 *    sal_dma_free
 * Purpose:
 *    Free memory block allocated by sal_dma_alloc.
 * Parameters:
 *    ptr - pointer to memory block returned by sal_alloc
 * Returns: None
 *
 */
void sal_dma_free(void *ptr)
{
    uint32* addrp;
    uint32* userp;
    addrp = (uint32*)(*(((uint32*)ptr) - 1));
    /* Sanitize */
    userp = (uint32*)SAL_DMA_ALIGNED(addrp, SAL_DMA_ALIGN);
#ifdef DEBUG
    if (userp != (uint32*)ptr) {
      sal_printf("ERROR ExpUser %x ActUser %x OrigAddr %x\n", (uint32)userp, (uint32)ptr, (uint32)addrp);
    }
#endif
    SAL_ASSERT(userp == (uint32*)ptr);
    sal_dma_frees++;
    vPortFree(addrp);
}

/*
 * Function:
 *    sal_calloc
 * Purpose:
 *    Allocate general purpose system memory and clears the contents of
 *    allocated memory.
 * Parameters:
 *    nmemb - number of member elements
 *    size - size of each member element.
 * Returns:
 *    Success - Pointer to memory block.
 *    Failure - Null pointer.
 * Notes:
 *    Memory allocated by this routine is not guaranteed to be safe
 *    for hardware DMA read/write. mem_id descriptior is optional and not
 *    used in OSAL implementation for freertos.
 */

void *sal_calloc(unsigned int nmemb, unsigned int size )
{
    void *ptr = NULL;

    ptr =  pvPortMalloc((nmemb*size));

    if (ptr) {

        /* zero the memory */
        sal_memset(ptr, 0, (nmemb*size));
    }

    return ptr;
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
    void *ptr = NULL;
    size_t  len = 0;

    if (new_bytes != 0) {
        ptr =  pvPortMalloc(new_bytes);

        if (ptr) {
            if (old_ptr != NULL) {
                /* take only smallest subset */
                len = (old_bytes > new_bytes) ? (new_bytes) : (old_bytes);
                sal_memcpy(ptr, old_ptr, len);
                vPortFree(old_ptr);
            }
            /* Old freed, return new */
            return ptr;
        } else {
            /* malloc failed, return old pointer */
            return old_ptr;
        }
    }
    if ((new_bytes == 0) && (old_ptr != NULL)) {
        vPortFree(old_ptr);
    }
    return NULL;
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
    uint32 total = 0, lwm = 0, avail = 0;
    avail = xPortGetFreeHeapSize();
    lwm = xPortGetMinimumEverFreeHeapSize();
    total = configTOTAL_HEAP_SIZE;
    sal_printf("Heap Statistics: \n");
    sal_printf(" Total heapmem bytes: %d\n", total);
    sal_printf(" Current inuse bytes: %d\n", (total - avail));
    sal_printf(" Current  free bytes: %d\n", avail);
    sal_printf(" Low watermark bytes: %d\n", lwm);
    sal_printf(" Num  DMA Mem  Alloc: %d\n", sal_dma_allocs);
    sal_printf(" Num  DMA Mem   Free: %d\n", sal_dma_frees);
}

#endif //HYBRID_M7_MODE

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

    if (s1 == NULL && s2 == NULL)
        return 0;

    if (s1 == NULL || s2 == NULL)
        return -1;

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

    if (s == NULL || d == NULL)
        return NULL;

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

    if (dest == NULL)
        return NULL;

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
    unsigned char *d;
    const unsigned char *s;
    d = (unsigned char *) dest;
    s = (const unsigned char *) src;
    int cnt = n;

    if (s == d)
        return d;

    if (s == NULL || d == NULL)
        return NULL;

    if (s > d) {
        while (cnt) {
            *d++ = *s++;
            cnt--;
        }
    } else {
        while (cnt) {
            d[cnt-1] = s[cnt-1];
            cnt--;
        }
    }

    return dest;
}

/*
 * Function:
 *    sal_memchr
 * Purpose:
 *    scans the first n bytes of the memory area pointed to by s
 *    for the character c.
 * Parameters:
 *    s - pointer to memory region
 *    c - character to look for
 *    n - size of the memory region to scan
 * Returns:
 *    Returns pointer to the matching byte or NULL if  the  character does
 *    not occur in the given memory area.
 */
void *sal_memchr(const void *s, int c, size_t n)
{
    int i;

    if (s != NULL) {
        for (i = 0; i < n; i++)
            if (*((unsigned char *)s + i) == (c & 0xFF))
                return (void *)((char *)s + i);
    }

    return NULL;
}


