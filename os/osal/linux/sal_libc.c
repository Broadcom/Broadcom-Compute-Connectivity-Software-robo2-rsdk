/*
 * * $ID: $
 * * 
 * * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 * * 
 * * $Copyright: (c) 2020 Broadcom Inc.
 * * All Rights Reserved$
 * *
 * * File:
 * *     sal_libc.c
 * * Purpose:
 * *     Robo2 openrtos osal module. This provides abstraction for LINUX
 * *     strings related service.
 * *
 * */

#include "sal_libc.h"
#include "sal_alloc.h"

/*
 * Function:
 *    sal_strlen
 * Purpose:
 *    Compute length of given string.
 * Parameters:
 *    str - string to compute the length
 * Returns:
 *    length of the string.
 */
size_t sal_strlen(const char *str)
{
    const char *s_orig;

    if (str == NULLPTR) {
        return 0;
    }
    s_orig = str;

    while (*str != 0) {
        str++;
    }

    return (str - s_orig);
}

/*
 * Function:
 *    sal_strcpy
 * Purpose:
 *    Copy string contained in src location to dest location.
 * Parameters:
 *    dest - pointer to buffer to store the string.
 *    src - pointer to buffer containing source string to copy.
 * Returns:
 *    pointer to buffer that contains destination string.
 */
char *sal_strcpy(char *dest,const char *src)
{
    char *ptr = dest;

    while (*src) *ptr++ = *src++;
    *ptr = '\0';

    return dest;
}

/*
 * Function:
 *    sal_strncpy
 * Purpose:
 *    Copies exactly cnt bytes from src location to dest location.If the length
 *    of the src string is less than cnt bytes the rest of the characters are
 *    filled with null byte.
 * Parameters:
 *    dest - pointer to destination string buffer.
 *    src - pointer to source string buffer.
 *    cnt - number of bytes to copy from source string.
 * Returns:
 *    pointer to buffer that contains destination string.
 */
char *sal_strncpy(char *dest,const char *src,size_t cnt)
{
    char *ptr = dest;

    while (*src && (cnt > 0)) {
        *ptr++ = *src++;
        cnt--;
    }

    while (cnt > 0) {
        *ptr++ = 0;
        cnt--;
    }

    return dest;
}
/*
 * Function:
 *    sal_strdup
 * Purpose:
 *    Copies from src location to dest location.
 * Parameters:
 *    rc - pointer to destination string buffer.
 *    s - pointer to source string buffer.
 * Returns:
 *    pointer to buffer that contains destination string.
 */


char *
sal_strdup(const char *s)
{
    int len = sal_strlen(s);
    char *rc = sal_alloc(len + 1, "sal_strdup");
    if (rc != NULL) {
        /* coverity[secure_coding] */
    sal_strcpy(rc, s);
    }
    return rc;
}


/*
 * Function:
 *    sal_strcmp
 * Purpose:
 *    Compares src string with dest string.
 * Parameters:
 *    dest - pointer to destination string buffer.
 *    src - pointer to source string buffer.
 * Returns:
 *    return integer that is less than, equal to or greater than zero. If src
 *    string is found to be less than, matching or greater than dest string.
 */
int sal_strcmp(const char *dest,const char *src)
{
    while (*src && *dest) {
        if (*dest < *src) return -1;
        if (*dest > *src) return 1;
        dest++;
        src++;
    }

    if (*dest && !*src) return 1;
    if (!*dest && *src) return -1;
    return 0;
}

/*
 * Function:
 *    sal_strncmp
 * Purpose:
 *    Compares exactly cnt bytes from src string with dest string.
 * Parameters:
 *    dest - pointer to destination string buffer.
 *    src - pointer to source string buffer.
 *    cnt - number of bytes to compare from source string.
 * Returns:
 *    return integer that is less than, equal to or greater than zero. If src
 *    string is found to be less than, matching or greater than dest string.
 */
int
sal_strncmp(const char *dest, const char *src, size_t cnt)
{
    const unsigned char *d;
    const unsigned char *s;

    d = (const unsigned char *) dest;
    s = (const unsigned char *) src;

    while (cnt) {
        if (*d < *s) return -1;
        if (*d > *s) return 1;
        if (!*d && !*s) return 0;
        d++;
        s++;
        cnt--;
    }

    return 0;
}

/*
 * Function:
 *      sal_strcasecmp
 * Purpose:
 *      Compare two strings ignoring the case of the characters.
 * Parameters:
 *      s1 - first string to compare
 *      s2 - second string to compare
 * Returns:
 *      0 if s1 and s2 are identical.
 *      negative integer if s1 < s2.
 *      positive integer if s1 > s2.
 * Notes
 *      See man page of strcasecmp for more info.
 */

int
sal_strcasecmp(const char *s1, const char *s2)
{
    unsigned char c1, c2;

    do {
        c1 = tolower(*s1++);
        c2 = tolower(*s2++);
    } while (c1 == c2 && c1 != 0);

    return c1 - c2;
}

/*
 * Function:
 *      sal_strncasecmp
 * Purpose:
 *      Compare two strings ignoring the case of the characters.
 * Parameters:
 *      s1 - first string to compare
 *      s2 - second string to compare
 *      n - maximum number of characters to compare
 * Returns:
 *      0 if s1 and s2 are identical up to n characters.
 *      negative integer if s1 < s2 up to n characters.
 *      positive integer if s1 > s2 up to n characters.
 * Notes
 *      See man page of strncasecmp for more info.
 */

int
sal_strncasecmp(const char *s1, const char *s2, size_t n)
{
    unsigned char c1, c2;

    if (n == 0) {
        return 0;
    }
    do {
        c1 = tolower(*s1++);
        c2 = tolower(*s2++);
    } while (--n && c1 == c2 && c1 != 0);

    return c1 - c2;
}



/*
 * Function:
 *    sal_strstr
 * Purpose:
 *    Finds the first occurrence of substring s2 in string s1.
 * Parameters:
 *    s1 - pointer to string buffer.
 *    s2 - pointer to substring buffer to locate in s1.
 * Returns:
 *    return pointer to the first occurrence of start of substring s2 in the
 *    string s1 if found, else NULL.
 */
char *
sal_strstr(const char *s1, const char *s2)
{
    if (*s1 == '\0') {
        if (*s2) {
            return (char *) NULL;
        } else {
            return (char *) s1;
        }
    }

    while(*s1) {
        int i;

        for (i=0; ; i++) {
            if (s2[i] == '\0') {
                return (char *) s1;
            }

            if (s2[i] != s1[i]) {
                break;
            }
        }
        s1++;
    }

    return (char *) NULL;
}

/*
 * Function:
 *    sal_strchr
 * Purpose:
 *    Finds the first occurrence of character c in string dest.
 * Parameters:
 *    dest - pointer to string buffer.
 *    c - character to located in dest string.
 * Returns:
 *    return pointer to the first occurrence of character c in the
 *    dest string if found, else NULL.
 */
char *sal_strchr(const char *dest,int c)
{
    while (*dest) {
        if (*dest == c) return (char *) dest;
        dest++;
    }
    return NULL;
}
/*
 * Function:
 *    sal_strrchr
 * Purpose:
 *    Finds the las occurrence of character c in string dest.
 * Parameters:
 *    dest - pointer to string buffer.
 *    c - character to located in dest string.
 * Returns:
 *    return pointer to the last occurrence of character c in the
 *    dest string if found, else NULL.
 */
char *sal_strrchr(const char *dest, int c)
{
    char* ret= NULL;
    do {
        if( *dest == (char)c )
            ret = (char *) dest;
    } while(*dest++);
    return ret;

}


/*
 * Function:
 *    sal_strcat
 * Purpose:
 *    Concatenates the src string to the end of the dest string.
 * Parameters:
 *    dest - pointer to destination string buffer.
 *    src - pointer to source string buffer.
 * Returns:
 *    return pointer to the resulting concatenated string dest.
 */
char *sal_strcat(char *dest,const char *src)
{
    char *ptr = dest;

    while (*ptr) ptr++;
    while (*src) *ptr++ = *src++;
    *ptr = '\0';

    return dest;
}
char*
sal_strncat(char *dest, const char *src, size_t n)
{
    size_t dest_len = sal_strlen(dest);
	size_t i;

	for (i = 0 ; i < n && src[i] != '\0' ; i++)
        dest[dest_len + i] = src[i];
    dest[dest_len + i] = '\0';

    return dest;
}

/*
 * sal_ctoi
 *
 *   Converts a C-style constant to integer.
 *   Also supports '0b' prefix for binary.
 */

int
sal_ctoi(const char *s, char **end)
{
    unsigned int        n, neg;
    int base = 10;

    if (s == 0) {
        if (end != 0) {
            end = 0;
        }
        return 0;
    }

    s += (neg = (*s == '-'));

    if (*s == '0') {
        s++;
        if (*s == 'x' || *s == 'X') {
            base = 16;
            s++;
        } else if (*s == 'b' || *s == 'B') {
            base = 2;
            s++;
        } else {
            base = 8;
        }
    }

    for (n = 0; ((*s >= 'a' && *s < 'a' + base - 10) ||
                 (*s >= 'A' && *s < 'A' + base - 10) ||
                 (*s >= '0' && *s <= '9')); s++) {
        n = n * base + ((*s <= '9' ? *s : *s + 9) & 15);
    }

    if (end != 0) {
        *end = (char *) s;
    }

    return (int) (neg ? -n : n);
}


/*
 * Function:
 *    sal_atoi
 * Purpose:
 *    Converts string pointed to by dest to integer value.
 * Parameters:
 *    dest - pointer to string buffer to convert to integer value.
 * Returns:
 *    return converted integer value.
 */
int sal_atoi(const char *dest)
{
    int x = 0;
    int digit;

    while (*dest) {
        if ((*dest >= '0') && (*dest <= '9')) {
            digit = *dest - '0';
        } else {
            break;
        }
        x *= 10;
        x += digit;
        dest++;
    }
    return x;
}

/*
 * Function:
 *    sal_xtoi
 * Purpose:
 *    Converts hexadecimal string to integer value.
 * Parameters:
 *    dest - pointer to hexadecimal string to convert to integer.
 * Returns:
 *    return converted integer value.
 */
int sal_xtoi(const char *dest)
{
    int x = 0;
    int digit;

    if ((*dest == '0') && (*(dest+1) == 'x')) dest += 2;

    while (*dest) {
        if ((*dest >= '0') && (*dest <= '9')) {
            digit = *dest - '0';
        } else if ((*dest >= 'A') && (*dest <= 'F')) {
            digit = 10 + *dest - 'A';
        } else if ((*dest >= 'a') && (*dest <= 'f')) {
            digit = 10 + *dest - 'a';
        } else {
            break;
        }
        x *= 16;
        x += digit;
        dest++;
    }

    return x;
}

char
sal_tolower(char c)
{
    if ((c >= 'A') && (c <= 'Z'))
        return 'a' + c - 'A';
    return c;
}

char
sal_toupper(char c)
{
    if ((c >= 'a') && (c <= 'z'))
        c -= 32;
    return c;
}


/*
 * A simple random number generator without floating pointer operations
 */
static  int rand_is_seeded = 0;
static  uint32 rand_c_value, rand_t_value, time_seed;
#define RAND_MAGIC_1 0x0007444BUL
#define RAND_MAGIC_2 0x8874A9C2UL
#define RAND_MAGIC_3 69069UL

void
sal_srand(uint32 seed)
{
    time_seed = (seed << 21) + (seed << 14) + (seed << 7);
    rand_c_value = ((time_seed + RAND_MAGIC_1) << 1) + 1;
    rand_t_value = (time_seed << (time_seed & 0xF)) + RAND_MAGIC_2;
    rand_is_seeded = 1;
}

int sal_rand(void) {
    time_seed = rand_c_value*time_seed + rand_t_value;
    return time_seed;
}

/*
 * Function:
 *    sal_strtol
 * Purpose:
 *    Converts string to integer value accroding to given base.
 * Parameters:
 *    nptr - pointer to string to convert to integer.
 *    endptr - pointer to invalid string location in nptr.
 *    base - base to convert string to integer.
 * Returns:
 *    return converted integer value.
 */
int sal_strtol(const char *nptr, char **endptr, int base)
{
    int x = 0;
    int digit;
    BOOL negative = FALSE;

    while (isspace(*nptr)) {
        nptr++;
    }

    if (*nptr == '-') {
        negative = TRUE;
        nptr++;
    }

    if ((base == 0) || (base == 16)) {
        if ((*nptr == '0') && ((*(nptr+1) == 'x') || (*(nptr+1) == 'X'))) {
            base = 16;
            nptr += 2;
        } else if (base == 0) {
            base = 10;
        }
    }

    while (*nptr) {
        if ((*nptr >= '0') && (*nptr <= '9')) {
            digit = *nptr - '0';
        } else if ((*nptr >= 'A') && (*nptr <= 'F')) {
            digit = 10 + *nptr - 'A';
        } else if ((*nptr >= 'a') && (*nptr <= 'f')) {
            digit = 10 + *nptr - 'a';
        } else {
            break;
        }

        if (digit >= base) {
            break;
        }
        x *= base;
        x += digit;
        nptr++;
    }

    if (endptr != NULL) {
        *endptr = (char *)nptr;
    }

    if (negative) {
        x = -x;
    }

    return x;
}

/*
 * Function:
 *    sal_strtoul
 * Purpose:
 *    Converts string to unsigned integer value accroding to given base.
 * Parameters:
 *    nptr - pointer to string to convert to unsigned integer.
 *    endptr - pointer to invalid string location in nptr.
 *    base - base to convert string to integer.
 * Returns:
 *    return converted unsinged integer value.
 */
unsigned int sal_strtoul(const char *nptr, const char **endptr, int base)
{
    unsigned int x = 0;
    int digit;
    BOOL negative = FALSE;

    while (isspace(*nptr)) {
        nptr++;
    }

    if (*nptr == '-') {
        negative = TRUE;
        nptr++;
    }

    if ((base == 0) || (base == 16)) {
        if ((*nptr == '0') && ((*(nptr+1) == 'x') || (*(nptr+1) == 'X'))) {
            base = 16;
            nptr += 2;
        } else if (base == 0) {
            base = 10;
        }
    }

    while (*nptr) {
        if ((*nptr >= '0') && (*nptr <= '9')) {
            digit = *nptr - '0';
        } else if ((*nptr >= 'A') && (*nptr <= 'F')) {
            digit = 10 + *nptr - 'A';
        } else if ((*nptr >= 'a') && (*nptr <= 'f')) {
            digit = 10 + *nptr - 'a';
        } else {
            break;
        }

        if (digit >= base) {
            break;
        }
        x *= base;
        x += digit;
        nptr++;
    }

    if (endptr != NULL) {
        *endptr = (char *)nptr;
    }

    if (negative) {
        x = -x;
    }

    return x;
}


/* Untested */
/*
 * Function:
 *    sal_strtoull
 * Purpose:
 *    Converts string to unsigned long long (uint64) value accroding to given base.
 * Parameters:
 *    nptr - pointer to string to convert to unsigned integer.
 *    endptr - pointer to invalid string location in nptr.
 *    base - base to convert string to integer.
 * Returns:
 *    return converted unsinged long long value.
 */
uint64_t sal_strtoull(const char *nptr, const char **endptr, int base)
{
    uint64_t x = 0;
    int digit;
    BOOL negative = FALSE;

    while (isspace(*nptr)) {
        nptr++;
    }

    if (*nptr == '-') {
        negative = TRUE;
        nptr++;
    }

    if ((base == 0) || (base == 16)) {
        if ((*nptr == '0') && ((*(nptr+1) == 'x') || (*(nptr+1) == 'X'))) {
            base = 16;
            nptr += 2;
        } else if (base == 0) {
            base = 10;
        }
    }

    while (*nptr) {
        if ((*nptr >= '0') && (*nptr <= '9')) {
            digit = *nptr - '0';
        } else if ((*nptr >= 'A') && (*nptr <= 'F')) {
            digit = 10 + *nptr - 'A';
        } else if ((*nptr >= 'a') && (*nptr <= 'f')) {
            digit = 10 + *nptr - 'a';
        } else {
            break;
        }

        if (digit >= base) {
            break;
        }
        x *= base;
        x += digit;
        nptr++;
    }

    if (endptr != NULL) {
        *endptr = (char *)nptr;
    }

    if (negative) {
        x = -x;
    }

    return x;
}

/*
 * Function:
 *    sal_isalnum
 * Purpose:
 *    Checks to see if a character is alpha-numeric (0-9, a-z, A-Z).
 * Parameters:
 *    c - character to check
 * Returns:
 *    0 if the character is alpha-numeric, non-zero otherwise.
 */
int sal_isalnum(const char c)
{
    if (((c >= '0') && (c <= '9')) ||
        ((c >= 'a') && (c <= 'z')) ||
        ((c >= 'A') && (c <= 'Z')))
    {
        return 0;
    } else {
        return 1;
    }
}

/*
 * Function:
 *    sal_strspn
 * Purpose:
 *    Calculates the length of the initial segment of the string 
 *    provided, which consists entirely of characters in accept
 * Parameters:
 *    s - input string
 *    accept - array of accepted characters in string (null terminated)
 * Returns:
 *    returns length of initial segment of string containing only accpeted
 *    characters
 */
uint32_t sal_strspn(const char *s, const char *accept) {
    int i, j;
    uint32_t ret = 0, s_len, accept_len;
    
    if (s == NULL || accept == NULL)
        return 0;

    s_len = sal_strlen(s);
    accept_len = sal_strlen(accept);
    for (i = 0; i < s_len; i++) {
        for (j = 0; j < accept_len; j++) {
            if (s[i] == accept[j]) {
                break;
            }
        }
        if (j == accept_len)
            return ret;
        ret++;
    }
    
    return ret;
}

/*
 * Function:
 *    sal_strcspn
 * Purpose:
 *    Calculates the length of the initial segment of the string 
 *    provided, which consists entirely of characters not in the 
 *    reject list
 * Parameters:
 *    s - input string
 *    reject - array of rejected characters in string (null terminated)
 * Returns:
 *    returns length of initial segment of string containing only characters
 *    that are not in the reject list
 */
uint32_t sal_strcspn(const char *s, const char *reject) {
    int i, j;
    uint32_t ret = 0, s_len, reject_len;
    
    if (s == NULL || reject == NULL)
        return 0;

    s_len = sal_strlen(s);
    reject_len = sal_strlen(reject);
    for (i = 0; i < s_len; i++) {
        for (j = 0; j < reject_len; j++) {
            if (s[i] == reject[j]) {
                break;
            }
        }
        if (j != reject_len)
            return ret;
        ret++;
    }
    
    return ret;
}

int sal_printf(const char *format, ...);
/*
 * Function:
 *    sal_strtok
 * Purpose:
 *    Extract tokens from strings. This function parses a string into 
 *    a sequence of tokens. On the first call to strtok() the string
 *    to be parsed should be specified in 's'. In each subsequent call
 *    that should parse the same string, s should be NULL
 *    Note: This function modifies the input string
 * Parameters:
 *    s - input string
 *    delim - list of delimiters to split string on
 * Returns:
 *    Returns a pointer to the next token, or NULL if there are no
 *    more tokens
 */
char *sal_strtok(char *s, const char *delim)
{
    static char *next_tok = NULL;
    char *str;
    int pos, len;
    
    /* Invalid delimiter list */
    if (delim == NULL)
        return NULL;

    /* Use last saved token ptr if s == NULL */
    str = s;
    if (s == NULL)
        str = next_tok;
    
    /* Nothing to parse */
    if (str == NULL)
        return NULL;
    
    len = sal_strlen(str);
    pos = sal_strcspn(str, delim);
    
    /* Delimiter not found */
    if (len == pos) {
        next_tok = NULL;
    } else {
        str[pos] = '\0';
        next_tok = &str[pos+1];
    }
    
    return str;
}
