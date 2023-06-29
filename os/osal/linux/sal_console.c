/*
 * * $ID: $
 * * $Copyright: (c) 2017 Broadcom Corp.
 * * All Rights Reserved$
 * *
 * * File:
 * *     sal_console.c
 * * Purpose:
 * *     Robo2 osal module. This provides abstraction for linux platform
 * *     console related service.
 * *
 * */

#include "sal_console.h"

#define  SAL_PRINT_BUF_SIZE    256

#define  SAL_PRINT_CMD_SIZE    1024
#define  SAL_PRINT_LOG_FILE    "/var/log/rsdk.log"
#define  SAL_RES_LOG_FILE "/tmp/bcm.res.log"

static char lastchar;
static const char digits[17] = "0123456789ABCDEF";
static const char ldigits[17] = "0123456789abcdef";


/*
 * Function:
 *    sal_char_avail
 * Purpose:
 *    Check if character is available for input.
 * Parameters:
 *    None
 * Returns:
 *    TRUE if character is available else FALSE.
 */
BOOL sal_char_avail(void)
{
    return 1;
}

/*
 * Function:
 *    sal_getchar
 * Purpose:
 *    Get input character, it may block if no char is available.
 * Parameters:
 *    None
 * Returns:
 *    returns input character.
 */
char sal_getchar(void)
{
    lastchar=getc(stdin);
    return lastchar;
}

char my_getchar(void * ptr)
{
    return sal_getchar();
}

/*
 * Function:
 *    sal_get_last_char
 * Purpose:
 *    Get last input character.
 * Parameters:
 *    None
 * Returns:
 *    returns last input character.
 */
char sal_get_last_char(void)
{
    return lastchar;
}

/*
 * Function:
 *    sal_putchar
 * Purpose:
 *    Output one character.
 * Parameters:
 *    c - character to write to output
 * Returns:
 *    returns character written to output.
 */
char sal_putchar(char c)
{
    putc(c,stdout);
    return c;
}
/*
 * Function:
 *    __stox(buf,num,radix,width)
 * Purpose:
 *    Convert a short int to a string
 * Parameters:
 *    buf - where to put characters
 *    num - number to convert
 *    radix - radix to convert number to (usually 10 or 16)
 *    width - width in characters
 * Returns:
 *    number of digits placed in output buffer
 */
static int __stox(char *buf,unsigned int num,unsigned int radix,int width,
                  const char *digits)
{
    char buffer[16];
    char *op;
    int retval;

    op = &buffer[0];
    retval = 0;

    do {
        *op++ = digits[num % radix];
        retval++;
        num /= radix;
    } while (num != 0);

    if (width && (width > retval)) {
        width = width - retval;
        while (width) {
            *op++ = '0';
            retval++;
            width--;
        }
    }

    while (op != buffer) {
        op--;
        *buf++ = *op;
    }

    return retval;
}



/*
 * Function:
 *    __atox(buf,num,radix,width)
 * Purpose:
 *    Convert a number to a string
 * Parameters:
 *    buf - where to put characters
 *    num - number to convert
 *    radix - radix to convert number to (usually 10 or 16)
 *    width - width in characters
 * Returns:
 *    number of digits placed in output buffer
 */
static int __atox(char *buf,unsigned int num,unsigned int radix,int width,
                  const char *digits, char pad)
{
    char buffer[16];
    char *op;
    int retval;

    op = &buffer[0];
    retval = 0;

    do {
        *op++ = digits[num % radix];
        retval++;
        num /= radix;
    } while (num != 0);

    if (width && (width > retval)) {
        width = width - retval;
        while (width) {
            *op++ = pad;
            retval++;
            width--;
        }
    }

    while (op != buffer) {
        op--;
        *buf++ = *op;
    }

    return retval;
}


/*
 * Function:
 *    __llatox(buf,num,radix,width)
 * Purpose:
 *    Convert a long number to a string
 * Parameters:
 *    buf - where to put characters
 *    num - number to convert
 *    radix - radix to convert number to (usually 10 or 16)
 *    width - width in characters
 * Returns:
 *    number of digits placed in output buffer
 */
static int __llatox(char *buf,unsigned long long num,unsigned int radix,
                    int width,const char *digits)
{
    char buffer[16];
    char *op;
    int retval;

    op = &buffer[0];
    retval = 0;

#if CPUCFG_REGS32
    /*
     * To avoid pulling in the helper library that isn't necessarily
     * compatible with PIC code, force radix to 16, use shifts and masks
     */
    do {
        *op++ = digits[num & 0x0F];
        retval++;
        num >>= 4;
    } while (num != 0);
#else
    do {
        *op++ = digits[num % radix];
        retval++;
        num /= radix;
    } while (num != 0);
#endif

    if (width && (width > retval)) {
        width = width - retval;
        while (width) {
            *op++ = '0';
            retval++;
            width--;
        }
    }

    while (op != buffer) {
        op--;
        *buf++ = *op;
    }

    return retval;
}



/*
 * Function:
 *    sal_vsnprintf(outbuf,buf_size,template,arglist)
 * Purpose:
 *    Format a string into the output buffer
 * Parameters:
 *    output - output buffer
 *    template - template string
 *    arglist - parameters
 * Returns:
 *    number of characters copied
 */
int sal_vsnprintf(char *outbuf,size_t buf_size,const char *templat,va_list marker)
{
    char *optr;
    const char *iptr;
    char pad = ' ';
    unsigned char *tmpptr;
    long x;
    unsigned long ux;
    unsigned long long ulx;
	unsigned short uh;
    int i;
    long long ll;
	int half = 0;
	int buffersize = buf_size;
    int islong;
    int width = 0;
    int width2 = 0;
	char *start = outbuf;
    optr = outbuf;
    iptr = templat;

    while (*iptr && ((buffersize-(optr-start)> 1 ) )) {
        if (*iptr != '%') {
            *optr++ = *iptr++;
            continue;
        }

        iptr++;

        if (*iptr == '#') {
            iptr++;
        }
        if (*iptr == '0') {
            pad = '0';
            iptr++;
        }
        width = 0;
        while (*iptr && isdigit(*iptr)) {
            width += (*iptr - '0');
            iptr++;
            if (isdigit(*iptr)) width *= 10;
        }
        if (*iptr == '.') {
            iptr++;
            /* Support for dynamic format specifiers */
            if (*iptr == '*') {
                width = va_arg(marker, unsigned int);
                iptr++;
            } else {
		        width2 = 0;
		        while (*iptr && isdigit(*iptr)) {
		            width2 += (*iptr - '0');
		            iptr++;
		            if (isdigit(*iptr)) width2 *= 10;
                }
	        }
	    }
		if (*iptr == 'h') {
			iptr++;
			half = 1;
		}

        islong = 0;
        if (*iptr == 'l') {
            islong++;
            iptr++;
        }
        if (*iptr == 'l') {
            islong++;
            iptr++;
        }

        switch (*iptr) {
        case 'I':
            tmpptr = (unsigned char *) va_arg(marker,unsigned char *);
            optr += __atox(optr,*tmpptr++,10,0,digits,pad);
            *optr++ = '.';
            optr += __atox(optr,*tmpptr++,10,0,digits,pad);
            *optr++ = '.';
            optr += __atox(optr,*tmpptr++,10,0,digits,pad);
            *optr++ = '.';
            optr += __atox(optr,*tmpptr++,10,0,digits,pad);
            break;
        case 's':
            tmpptr = (unsigned char *) va_arg(marker,unsigned char *);
            if (!tmpptr) tmpptr = (unsigned char *) "(null)";
            if ((width == 0) && (width2 == 0)) {
                width = buffersize - (optr-start)-1;
            } else if (width2 != 0) {
                width = width2;
            }
            if (width > (buffersize-(optr-start))) {
                width = buffersize - (optr-start)-1;
            }
            while (width && *tmpptr) {
                *optr++ = *tmpptr++;
                width--;
            }
            *optr = '\0';
            break;
        case 'a':
            tmpptr = (unsigned char *) va_arg(marker,unsigned char *);
            for (x = 0; x < 5; x++) {
                optr += __atox(optr,*tmpptr++,16,2,digits,pad);
                *optr++ = '-';
            }
            optr += __atox(optr,*tmpptr++,16,2,digits,pad);
            break;
        case 'd':
            switch (islong) {
            case 0:
            case 1:
                i = va_arg(marker,int);
                if (i < 0) {
                    *optr++='-';
                    i = -i;
                }
                optr += __atox(optr,i,10,width,digits,pad);
                break;
            case 2:
                ll = va_arg(marker,long long int);
                if (ll < 0) {
                    *optr++='-';
                    ll = -ll;
                }
                optr += __llatox(optr,ll,10,width,digits);
                break;
            }
            break;
        case 'u':
            switch (islong) {
            case 0:
            case 1:
				if (half){
                    ux = va_arg(marker,unsigned int);
                    uh = (unsigned int) (unsigned short) ux;
                    optr += __stox(optr,uh,10,width,digits);
                    break;
                }
                ux = va_arg(marker,unsigned int);
                optr += __atox(optr,ux,10,width,digits,pad);
                break;
            case 2:
                ulx = va_arg(marker,unsigned long long);
                optr += __llatox(optr,ulx,10,width,digits);
                break;
            }
            break;
        case 'X':
        case 'x':
            switch (islong) {
            case 0:
            case 1:
				if (half){
                    ux = va_arg(marker,unsigned int);
                    uh = (unsigned int) (unsigned short) ux;
                    optr += __stox(optr,uh,16,width,
					           (*iptr == 'X') ? digits : ldigits);
                    break;
                }
                ux = va_arg(marker,unsigned int);
                optr += __atox(optr,ux,16,width,
                               (*iptr == 'X') ? digits : ldigits, pad);
                break;
            case 2:
                ulx = va_arg(marker,unsigned long long);
                optr += __llatox(optr,ulx,16,width,
                                 (*iptr == 'X') ? digits : ldigits);
                break;
            }
            break;
        case 'p':
        case 'P':
#ifdef __long64
            lx = va_arg(marker,long long);
            optr += __llatox(optr,lx,16,16,
                             (*iptr == 'P') ? digits : ldigits);
#else
            x = va_arg(marker,long);
            optr += __atox(optr,x,16,8,
                           (*iptr == 'P') ? digits : ldigits, pad);
#endif
            break;
        case 'w':
            x = va_arg(marker,unsigned int);
            x &= 0x0000FFFF;
            optr += __atox(optr,x,16,4,digits,pad);
            break;
        case 'b':
            x = va_arg(marker,unsigned int);
            x &= 0x0000FF;
            optr += __atox(optr,x,16,2,digits,pad);
            iptr++;
            break;
        case 'Z':
            x = va_arg(marker,unsigned int);
            tmpptr = va_arg(marker,unsigned char *);
            while (x) {
                optr += __atox(optr,*tmpptr++,16,2,digits,pad);
                x--;
            }
            break;
        case 'c':
            x = va_arg(marker, int);
            *optr++ = x & 0xff;
            break;

        default:
            *optr++ = *iptr;
            break;
        }
        iptr++;
    }

    *optr = '\0';

    return (optr - outbuf);
}

/*
 * Function:
 *    sal_vsprintf(outbuf,template,arglist)
 * Purpose:
 *    Format a string into the output buffer
 * Parameters:
 *    output - output buffer
 *    template - template string
 *    arglist - parameters
 * Returns:
 *    number of characters copied
 */
int sal_vsprintf(char *outbuf,const char *templat,va_list marker)
{
	int rv;
	rv = sal_vsnprintf(outbuf,(size_t) SAL_PRINT_BUF_SIZE ,templat, marker);
	return rv;
}
/*
 * Function:
 *    sal_printf(const char *fmt, ...)
 * Purpose:
 *    printf routine for the SAL layer
 * Parameters:
 *    fmt - printf-style format string
 *    ... - printf-style argument list
 * Returns:
 *    rv  - returns no of characters printed
 */
int sal_printf(const char *fmt, ...)
{
    int rv;
    va_list arg_ptr;
    char buf[SAL_PRINT_BUF_SIZE];
    char cmdstr[SAL_PRINT_CMD_SIZE] = { 0 };

    va_start(arg_ptr, fmt);
    rv = sal_vsprintf(buf, fmt, arg_ptr);
    va_end(arg_ptr);

    printf("%s",buf);
    sprintf(cmdstr, "echo -n \"%s\"  >> %s", buf, SAL_PRINT_LOG_FILE);
    system(cmdstr);
    return rv;
}

int sal_res_printf(const char *fmt, ...)
{
    int rv;
    va_list arg_ptr;
    char buf[SAL_PRINT_BUF_SIZE];
    char cmdstr[SAL_PRINT_CMD_SIZE] = { 0 };

    va_start(arg_ptr, fmt);
    rv = sal_vsprintf(buf, fmt, arg_ptr);
    va_end(arg_ptr);

    printf("%s",buf);
    sprintf(cmdstr, "echo -n \"%s\"  >> %s", buf, SAL_RES_LOG_FILE);
    system(cmdstr);
    return rv;
}

/*
 * Function:
 *    sal_vprintf(const char *fmt, ...)
 * Purpose:
 *    printf routine for the SAL layer
 * Parameters:
 *    fmt - printf-style format string
 *    args- argument list
 * Returns:
 *    rv   - returns no of characters printed
 */
int sal_vprintf(const char *fmt, va_list args)
{
	int rv;
    char buf[SAL_PRINT_BUF_SIZE];

    rv = sal_vsprintf(buf, fmt, args);
    printf("%s",buf);
	return rv;
}



/*
 * Function:
 *    sal_sprintf(buf,template,params..)
 * Purpose:
 *    format messages from template into a buffer.
 * Parameters:
 *    buf - output buffer
 *    template - template string
 *    params... parameters
 * Returns:
 *    number of bytes copied to buffer
 */
int sal_sprintf(char *buf,const char *templat,...)
{
    va_list marker;
    int count;

    va_start(marker,templat);
    count = sal_vsprintf(buf,templat,marker);
    va_end(marker);

    return count;
}

/*
 * Function:
 *    sal_snprintf(buf,buf_size,template,params..)
 * Purpose:
 *    format messages from template into a buffer.
 * Parameters:
 *    buf - output buffer
 *    template - template string
 *    params... parameters
 * Returns:
 *    number of bytes copied to buffer
 */
int sal_snprintf(char *buf, size_t buf_size,const char *templat,...)
{
    va_list marker;
    int count;

    va_start(marker,templat);
    count = sal_vsnprintf(buf,buf_size,templat,marker);
    va_end(marker);

    return count;
}




/*
 * Function:
 *    sal_debugf(fmt,params..)
 * Purpose:
 *    debug routine for the SAL layer
 * Parameters:
 *    fmt - printf-style format string
 *    ... - printf-style argument list
 * Returns:
 *    None
 */
void sal_debugf(const char *fmt, ...)
{
    va_list arg_ptr;
    char buf[SAL_PRINT_BUF_SIZE];

    va_start(arg_ptr, fmt);
    sal_vsprintf(buf, fmt, arg_ptr);
    va_end(arg_ptr);

    printf("\n%s",buf);
}

/*
 * Function:
 *    sal_assert(expr,file,line)
 * Purpose:
 *    debug routine for the SAL layer
 * Parameters:
 *    expr - expression resulting in assertion
 *    file - file in which assertion occured
 *    line - line in the file at which assertion occured
 * Returns:
 *    None
 */
void sal_assert(const char *expr, const char *file, uint16 line)
{
    sal_printf("ERROR: Assertion failed: (%s) at %s:%u\n", expr, file, line);
    for(;;);
}

void sal_backspace(int count)
{
    while (count--) {
        sal_putchar('\x08');
        sal_putchar('\x20');
        sal_putchar('\x08');
    }
}
