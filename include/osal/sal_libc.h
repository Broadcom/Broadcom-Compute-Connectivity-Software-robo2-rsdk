/*
 * $Id: sal.h,v 1.29 2015/04/08 09:38:56 justinch Exp $
 *
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved.$
 *
 */

#ifndef _SAL_LIBC_H_
#define _SAL_LIBC_H_

#include <stdlib.h>
#include "sal_types.h"

#ifndef _CTYPE_H
#define isdigit(d)      (((d) >= '0') && ((d) <= '9'))
#define isxdigit(c)      ((((c) >= '0') && ((c) <= '9')) || \
                            (((c) >= 'a') && ((c) <= 'f')) || \
                            (((c) >= 'A') && ((c) <= 'F')))
#define isspace(x)      (((x) == ' ') || ((x) == '\t'))
#endif

extern size_t sal_strlen(const char *str);
extern char *sal_strcpy(char *dest,const char *src);
extern char *sal_strncpy(char *dest,const char *src,size_t cnt);
extern int sal_strcmp(const char *dest,const char *src);
extern int sal_strncmp(const char *dest, const char *src, size_t cnt);
extern int sal_strncasecmp(const char *s1, const char *s2, size_t n);
extern int sal_strcasecmp(const char *s1, const char *s2);
extern char *sal_strstr(const char *s1, const char *s2);
extern char *sal_strchr(const char *dest,int c);
extern char *sal_strrchr(const char *dest,int c);
extern char *sal_strcat(char *dest,const char *src);
extern char *sal_strncat(char *dest,const char *src,size_t n);
extern int sal_atoi(const char *dest);
extern int sal_xtoi(const char *dest);
extern int sal_ctoi(const char *s, char **end);
extern int sal_strtol(const char *nptr, char **endptr, int base);
extern unsigned int sal_strtoul(const char *nptr, const char **endptr, int base);
extern uint64_t sal_strtoull(const char *nptr, const char **endptr, int base);
extern char * sal_strdup(const char *s);
extern int sal_isalnum(const char c);

uint32_t sal_strspn(const char *s, const char *accept);
uint32_t sal_strcspn(const char *s, const char *reject);
char *sal_strtok(char *s, const char *delim);

void sal_srand(uint32 seed);
int sal_rand(void);

/* string lib related */
extern char sal_toupper(char c);
extern char sal_tolower(char c);
#define toupper     sal_toupper
#define tolower     sal_tolower

/* Random number generator (0 ~ 32767 to be uniform for all platforms) */
extern void sal_srand(uint32 seed);
extern void sal_set_errno(int value);

#endif /* _SAL_LIBC_H_ */
