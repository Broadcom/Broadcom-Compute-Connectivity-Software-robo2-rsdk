/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 */

/*
 * $ sal_alloc.h $
 */

extern int  sal_memcmp(const void *s1, const void *s2, size_t cnt);
extern void *sal_memcpy(void *dest, const void *src, size_t cnt);
extern void *sal_memset(void *dest, int c, size_t cnt);
extern void *sal_memmove(void *dest, const void *src, size_t n);
extern void *sal_memchr(const void *s, int c, size_t n);
