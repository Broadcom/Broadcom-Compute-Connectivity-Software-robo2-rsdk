/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved$
 *
 * File:
 *    sal_sscanf.c
 * Description:
 *    Sscanf Implementation
 */

#include <sal_types.h>
#include <sal_alloc.h>
#include <stdarg.h>


#define SSCANF_FORMAT_INTEGER      0x00000001
#define SSCANF_FORMAT_UNSIGNED     0x00000002
#define SSCANF_FORMAT_DECIMAL      0x00000004
#define SSCANF_FORMAT_HEX          0x00000008
#define SSCANF_FORMAT_OCTAL        0x00000010
#define SSCANF_FORMAT_STRING       0x00000020
#define SSCANF_FORMAT_CHAR         0x00000040
#define SSCANF_TWOS_COMPLEMENT     0x00000080
#define SSCANF_SUPPRESS_ASSIGN     0x00000100
#define SSCANF_SET_NOT_MATCHING    0x00000200
#define SSCANF_SET_MATCHING        0x00000400
#define SSCANF_GET_PARSED_LEN      0x00000800
#define SSCANF_SIZE_8              0x00001000
#define SSCANF_SIZE_16             0x00002000
#define SSCANF_SIZE_32             0x00004000
#define SSCANF_SIZE_64             0x00008000

#define IS_WHITE_SPACE(c) ((c == ' ') || (c == '\t') || (c == '\n') || \
                            (c == '\f') || (c == '\r'))

#define SKIP_WHITE_SPACE(p) do {                                               \
                                if (!IS_WHITE_SPACE(*p)) {break;} else {p++;}  \
                            } while (*p != '\0')

char *hexset="0123456789abcdefABCDEF";
int hexset_len = 22; /* 16 + 6 */
int decset_len = 10; 
int octset_len = 8; 
int binset_len = 2; 
 
static int
get_field_len(uint32 flags)
{
    int outlen = 0;
    if (flags & SSCANF_SIZE_16) {
        outlen = 2;
    } else if (flags & SSCANF_SIZE_32) {
        outlen = 4;
    } else if (flags & SSCANF_SIZE_64) {
        outlen = 8;
    } else {
        outlen = 1;
    }
    return outlen;
}

static int
extract_bytes(const char *input, uint32 flags, const char *charset, int csetlen, int outlen, char* output)
{
    const char *p, *r, *re;
    char *q;
    uint64 value = 0;
    int is_numeric = 0;
    int base =0, len=0, i, j;
    int bytes = 0;

    q = output;
    for (p = input; *p != '\0'; q++, p++) {
        if (IS_WHITE_SPACE(*p)) {
            continue;
        } else {
            if (flags & SSCANF_FORMAT_INTEGER) {
                if (*p == '-') {
                    flags |= SSCANF_TWOS_COMPLEMENT;
                    p++; bytes++;
                }
                if (p[0] == '0') {
                    if ((p[1] == 'x') || (p[1] == 'X')) {
                        flags |= SSCANF_FORMAT_HEX;
                        csetlen = hexset_len;
                    } //else {
                      //  flags |= SSCANF_FORMAT_OCTAL;
                      //  csetlen = octset_len;
                    //}
                } else {
                    flags |= SSCANF_FORMAT_DECIMAL;
                    csetlen = decset_len;
                }
            }


            if (flags & SSCANF_FORMAT_UNSIGNED) {
                is_numeric = 1;
                base = 10;
            } else if (flags & SSCANF_FORMAT_DECIMAL) {
                is_numeric = 1;
                base = 10;
            } else if (flags & SSCANF_FORMAT_OCTAL) {
                is_numeric = 1;
                if (p[0] == '0') {
                    p++; bytes+=1;
                }
                base = 8;
            } else if (flags & SSCANF_FORMAT_HEX) {
                is_numeric = 1;
                if ((p[0] == '0') && 
                        ((p[1] == 'x') || (p[1] == 'X'))) {
                    p+=2; bytes+=2;
                }
                base = 16;
            }

            if (is_numeric) {
                len = get_field_len(flags);
                /* If the outlen is 255 (initialized max value) extract all the bytes
                   otherwise extract only the given number of bytes  */ 
                if (outlen == 255) {
                    outlen = 16;
                } 
                for (i=0; i<outlen; bytes++, i++) {
                    if (IS_WHITE_SPACE(p[i])) {
                        break;
                    }
                    for (r = charset, re = r + csetlen; (r < re) && (*r != p[i]); r++);
                    if (r < re) {
                        value = value * base;
                        if ((p[i] >= 'A') && (p[i] <= 'F')) {
                            value += 10 + (p[i] - 'A');
                        } else if ((p[i] >= 'a') && (p[i] <= 'f')) {
                            value += 10 + (p[i] - 'a');
                        } else {
                            value += (p[i] - '0');
                        }
                    } else {
                        /* Non numeric */
                        break;
                    }
                }
                if (i >= 16) {
                    value = -1;
                }
                if (!(flags & SSCANF_SUPPRESS_ASSIGN)) {
                    if (flags & SSCANF_TWOS_COMPLEMENT)
                        value = ~value + 1;
                    for (j=0; j<len; j++) {
                        q[j] = (value >> (j << 3)) & 0xFF;
                    }
                    return bytes;
                }
            } else if (flags & SSCANF_FORMAT_CHAR) {
                if (!(flags & SSCANF_SUPPRESS_ASSIGN)) {
                    *q = *p;
                }
            } else if (flags & SSCANF_FORMAT_STRING) {
                for (i=0; i<outlen; i++) {
                    if (IS_WHITE_SPACE(p[i])) {
                        q[i] = '\0';
                        return i;
                    }
                    q[i] = p[i];
                }
                /* max length reached */
                q[i] = '\0';
                return i;
            } else if (flags & SSCANF_SET_NOT_MATCHING) {
                for (i=0; i<outlen; i++) {
                    for (r = charset, re = r + csetlen; (r < re) && (*r != p[i]); r++);
                    if (r < re) {
                        /* match found, hence break */
                        q[i] = '\0';
                        return i;
                    } else {
                        q[i] = p[i];
                    }
                }
                /* max length reached */
                q[i] = '\0';
                return i;
            } else if (flags & SSCANF_SET_MATCHING) {
                for (i=0; i<outlen; i++) {
                    for (r = charset, re = r + csetlen; (r < re) && (*r != p[i]); r++);
                    if (r >= re) {
                        /* No match found, hence break */
                        q[i] = '\0';
                        return i;
                    } else {
                        q[i] = p[i];
                    }
                }
                /* max length reached */
                q[i] = '\0';
                return i;
            }
        }
    }
    /* done */
    return (q - output);
}

int
sal_sscanf(const char *input, const char *format, ...)
{
    int i;
    char pattern[256], *pat;
    const char *cur_ptr, *f, *charset = NULL;
    int csetlen = 0, maxlen = 255;
    uint32 flags = 0;
    int num_parsed = 0;
    int is_numeric;
    va_list args;
    char *next_ptr;
    int bytes = 0;

    if ((!format) || (!input)) {return 0;}

    cur_ptr = input;
    va_start(args, format);
    //flags |= SSCANF_SIZE_32; 
    is_numeric = 1;

    for (f = format; *f != '\0'; f++) {
        flags = SSCANF_SIZE_32; 
        SKIP_WHITE_SPACE(f);
        if (*f == '%') {
            f++;
            next_ptr = va_arg(args, void*); /* default */
            if (*f == '*') {
                flags |= SSCANF_SUPPRESS_ASSIGN;
                f++;
            }
            if ((*f >= '0') && (*f <= '9')) {
                maxlen = 0;
                while (*f != '\0') {
                    if ((*f < '0') || (*f > '9')) {
                        break;
                    } else {
                        maxlen = maxlen * 10 + *f - '0';
                    }
                    f++;
                }
            }
            if (*f == '[') {
                sal_memset(&pattern[0], 0, sizeof(pattern));
                pat = &pattern[0];
                is_numeric = 0;
                if (*++f == '^') {
                    flags |= SSCANF_SET_NOT_MATCHING;
                } else {
                    flags |= SSCANF_SET_MATCHING;
                }
                while (*++f != '\0') {
                    if (*f == '%') { break;}
                    *pat++ = *f;
                }
                for (i=(pat-pattern); i > 0; i--) {
                    if (pattern[i] == ']') {
                        pattern[i]='\0';
                        csetlen = i;
                        charset = &pattern[0];
                        break;
                    }
                }
                if (i <= 0) {
                    /* No matching set termination found */
                    va_end(args);
                    return num_parsed;
                }
            } else if (*f == 'n') {
                flags |= SSCANF_GET_PARSED_LEN;
                is_numeric = 0;
            } else if (*f == 's') {
                flags |= SSCANF_FORMAT_STRING;
                is_numeric = 0;
            } else if (*f == 'c') {
                flags |= SSCANF_FORMAT_CHAR;
                is_numeric = 0;
            } else if (*f == 'h') {
                 flags &= ~SSCANF_SIZE_32;
                 if (*++f  == 'h') {
                     flags |= SSCANF_SIZE_8;
                     f++;
                 } else {
                     flags |= SSCANF_SIZE_16;
                 }
            } else if (*f == 'l') {
                 if (*++f  == 'l') {
                     flags |= SSCANF_SIZE_64;
                     f++;
                 } else {
                     flags |= SSCANF_SIZE_32;
                 }
            }
            if (is_numeric) {
                charset = hexset; 
                if (*f == 'u') {
                    flags |= SSCANF_FORMAT_INTEGER | SSCANF_FORMAT_UNSIGNED;
                    csetlen = decset_len;
                } else if ((*f == 'x') || (*f == 'X')) {
                    flags |= SSCANF_FORMAT_HEX;
                    csetlen = hexset_len;
                } else if (*f == 'o') {
                    flags |= SSCANF_FORMAT_OCTAL;
                    csetlen = octset_len;
                } else if (*f == 'd') {
                    flags |= SSCANF_FORMAT_DECIMAL;
                    csetlen = decset_len;
                } else {
                    /* Unknown format spec */
                    va_end(args);
                    return num_parsed;
                }
            }
            if (flags & SSCANF_GET_PARSED_LEN) {
                *(int*)next_ptr = (int)(cur_ptr - input);
            } else {
                bytes = extract_bytes(cur_ptr, flags, charset, csetlen, maxlen, next_ptr);
                if (bytes <= 0) {
                    /*Extraction failed */
                    va_end(args);
                    return num_parsed;
                }
                cur_ptr += bytes;
                num_parsed++;
            }
        } else {
            if (*f != *cur_ptr) {
                /* fixed string mismatch */
                va_end(args);
                return num_parsed;
            } else {
                cur_ptr++;
            }
        }
    }
    va_end(args);
    return num_parsed;
}

