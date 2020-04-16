/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved.$
 *
 * File: sal_alloc.h
 * Purpose: Memory allocation
 */

#ifndef _SAL_ALLOC_H
#define _SAL_ALLOC_H

#include <stdlib.h>


/*
 * SAL Memory and Cache Support
 *
 */

#ifdef SAL_MEMORY_TRACE
#define sal_alloc(A, B) sal_alloc_int(A, B, __FILE__, __LINE__)
#define sal_free(A) sal_free_int(A, __FILE__, __LINE__)
extern void *sal_alloc_int(unsigned int num_bytes, char *mem_id, const char *s, int line);
extern void sal_free_int(void *ptr, const char *s, int line);
#else
extern void *sal_alloc(unsigned int num_bytes, char *mem_id);
extern void sal_free(void *ptr);
#endif
extern void * sal_dma_alloc(unsigned int num_bytes, char *mem_id);
extern void sal_dma_free(void *ptr);
extern void *sal_calloc(unsigned int nmemb, unsigned int size );
extern void *sal_realloc(void *old_ptr,
                         unsigned int old_bytes,
                         unsigned int new_bytes );
extern void sal_print_mem_stats(void);

extern int  sal_memcmp(const void *s1, const void *s2, size_t cnt);
extern void *sal_memcpy(void *dest, const void *src, size_t cnt);
extern void *sal_memset(void *dest, int c, size_t cnt );

extern void *sal_memmove(void *dest, const void *src, size_t n);
extern void *sal_memchr(const void *s, int c, size_t n);

#endif /* !_SAL_ALLOC_H */
