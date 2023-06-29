/*
 * $Copyright: (c) 2017 Broadcom Limited.
 * All Rights Reserved.$
 *
 * File:
 *     sal_get_set_params.c
 * Purpose:
 *     api to get and set parameters to NVM
 */

#include "sal_get_set_param.h"
#include "sal_console.h"
#include "sal_alloc.h"
#include "sal_libc.h"
#include "sal_file.h"

#define MAX_LINE_LENGTH    128

typedef struct param_default_val_s {
    const char *param_name;
    char *param_val;
} param_default_val_t;

param_default_val_t def_vals[] = {
    {
        MAC_ADDRESS_PARAM_NAME,
        "00:11:22:33:44:55",
    },
    {
        LOGIN_PASSWORD_PARAM_NAME,
        "5E884898DA28047151D0E56F8DC6292773603D0D6AABBDD62A11EF721D1542D8"
    },
};

static char *get_default_value(const char *param) {
    int i;

    for (i = 0; i < sizeof(def_vals)/sizeof(param_default_val_t); i++) {
        if (sal_strncmp(param,
                        def_vals[i].param_name,
                        sal_strlen(def_vals[i].param_name)) == 0) {
            return def_vals[i].param_val;
        }
    }

    return NULL;
}

#if defined(CONFIG_FS) || defined(WEB_SERVER_SIM) || defined(CONFIG_EXTERNAL_HOST)
static int write_params_upto_argv0(const char *param, char *buf, int buf_len, FILE *fp);

soc_error_t osal_get_param(const char *file, const char *param, char *value, uint32_t value_len) {
    FILE *fp;
    char *buf;
    char *val = NULL;
    int i, size, param_len;

    /* parameter check */
    if ((file == NULL) || 
        (param == NULL) || 
        (value == NULL) ||
        (value_len == 0))
        return SOC_E_PARAM;
    
    param_len = sal_strlen(param);
    if (param_len <= 0)
        return SOC_E_PARAM;
    
    size = sal_fsize(file);
    if (size <= 0) {
        /* No params file found - look for defaults */
        val = get_default_value(param);
        if (val != NULL) {
            //sal_printf ("Warn: Returning default value for parameter : %s\n", param);
            sal_strncpy(value, val, value_len);
            return SOC_E_NONE;
        } else {
            sal_printf ("Error: No params file found!\n");
            return SOC_E_UNAVAIL;
        }
    }

    /* Allocate file size buffer */
    buf = sal_alloc(size+1, "getparam");
    if (buf == NULL) {
        sal_printf ("Error: Memory allocation failed : %d\n", size);
        return SOC_E_MEMORY;
    }

    /* Clear buffer */
    sal_memset(buf, 0, size+1);
    
    /* Read file to buffer */
    fp = sal_fopen(file, "r");
    if (fp == NULL) {
        sal_printf ("Error: Failed to open file: %s\n", file);
        sal_free(buf);
        return SOC_E_UNAVAIL;
    }
    sal_fread(buf, 1, size, fp);
    sal_fclose(fp);
    
    if (param[0] == '*') {
        sal_printf("%s\n", buf);
        sal_free(buf);
        return SOC_E_NONE;
    }
    
    for (i = 0; i < (size - (param_len + 2)); i++) {
        if ((sal_strncmp(param, &buf[i], param_len) == 0)) {
            if ((i == 0) || (buf[i-1] == '\n')) { /* Check param begins on a new line */
                val = &buf[i + param_len];
                if (*val == ':') {
                    /* Skip : and spaces */
                    while (*val == ' ' || *val == ':')
                        val++;
                    break;
                }
            }
            val = NULL; /* Continue looking */
        }
    }
    
    /* Get default value if no entry exists in file */
    if (val == NULL) {
        val = get_default_value(param);
    } else {
        /* Add null terminator to the value if, param is found in file */
        i = 0;
        do {
            if (val[i] == '\n')
                val[i] = '\0';
            i++;
        } while ((val + i) <= (buf + size));
    } 

    if (val == NULL) {
        sal_printf ("Param '%s' not set\n", param);
        sal_free(buf);
        return SOC_E_NOT_FOUND;
    }
    
    sal_strncpy(value, val, value_len);

    sal_free(buf);

    return SOC_E_NONE;
}

soc_error_t osal_set_param(const char *file, const char *param, const char *value) {
    FILE *fp;
    char c;
    char *buf;
    int i, size, param_val_len, alloc_len;

    /* parameter check */
    if ((file == NULL) || 
        (param == NULL) || 
        (value == NULL))
        return SOC_E_PARAM;
    
    if ((sal_strlen(param) <= 0) || (sal_strlen(value) <= 0))
        return SOC_E_PARAM;
    
    param_val_len = sal_strlen(param) + sal_strlen(value);
    if (param_val_len > (MAX_LINE_LENGTH - 2)) {
        sal_printf ("Param name/value too long. Limit combined size to %d bytes\n", MAX_LINE_LENGTH - 2);
        return SOC_E_PARAM;
    }

    size = sal_fsize(file);
    
    if (size <= 0) { /* New file */
        fp = sal_fopen(file, "w");
        if (fp == NULL) {
            sal_printf ("Error: Failed to create file: %s\n", file);
            return -1;
        }
        alloc_len = param_val_len + 3;
        buf = sal_alloc(alloc_len, "setparam");
        if (buf == NULL) {
            sal_printf ("Error: Memory allocation failed : %d\n", alloc_len);
            sal_fclose(fp);
            return SOC_E_MEMORY;
        }
        sal_snprintf(buf, alloc_len, "%s:%s\n", param, value);
        sal_fwrite(buf, 1, alloc_len - 1, fp);
        sal_free(buf);
        sal_fclose(fp);
    } else {
        /* Allocate file size buffer */
        buf = sal_alloc(size, "setparam");
        if (buf == NULL) {
            sal_printf ("Error: Memory allocation failed : %d\n", size);
            return SOC_E_MEMORY;
        }
        /* Read file to buffer */
        fp = sal_fopen(file, "r");
        if (fp == NULL) {
            sal_printf ("Error: Failed to open file: %s\n", file);
            sal_free(buf);
            return SOC_E_UNAVAIL;
        }
        sal_fread(buf, 1, size, fp);
        sal_fclose(fp);
        /* Open file for writing */
        fp = sal_fopen(file, "w");
        if (fp == NULL) {
            sal_printf ("Error: Failed to create file: %s\n", file);
            sal_free(buf);
            return SOC_E_UNAVAIL;
        }
        /* write params uptp param 
         - returns the offset in buf to after argv0:value if param is already present
         - else returns buf length if argv0 is not present in params file */
        i = write_params_upto_argv0(param, buf, size, fp);

        /* Write param param */
        sal_fwrite((char *)param, 1, sal_strlen(param), fp);
        c = ':'; sal_fwrite(&c, 1, 1, fp);
        sal_fwrite((char *)value, 1, sal_strlen(value), fp);
        c = '\n'; sal_fwrite(&c, 1, 1, fp);
        
        /* Write the remaining params */
        if (i < size)
            sal_fwrite(&buf[i], 1, size - i, fp);
        sal_free(buf);
        sal_fclose(fp);
    }
    
    return SOC_E_NONE;
}

static int write_params_upto_argv0(const char *param, char *buf, int buf_len, FILE *fp) {
    int i, len, match;
    
    len = sal_strlen(param);
    match = 0;
    for (i = 0; i < buf_len - (len+2); i++) {
        if (sal_strncmp(param, &buf[i], len) == 0) {
            /* Check for : after param match */
            if (buf[i+len] == ':') {
                /* Check for param beginning at a new line */
                if ((i == 0) || (buf[i-1] == '\n')) {
                    match = 1;
                    break;
                }
            }
        }
    }
    
    if (match) {
        /* Write data upto param */
        sal_fwrite(&buf[0], 1, i, fp);
        /* Skip to next param */
        while (i < buf_len) {
            if (buf[i++] == '\n') {
                break;
            }
        }
    } else  {
        /* Write all the data if no match is found */
        sal_fwrite(&buf[0], 1, buf_len, fp);
        i = buf_len;
    }
   
    return i;
}
#else   /* CONFIG_FS */

soc_error_t osal_get_param(const char *file, const char *param, char *value, uint32_t value_len) {
    char *val;
    
    if ((param == NULL) ||
        (value == NULL) ||
        (value_len == 0))
        return SOC_E_PARAM;

    val = get_default_value(param);
    if (val != NULL) {
        //sal_printf ("Warn: Returning default value for parameter : %s\n", param);
        sal_strncpy(value, val, value_len);
        return SOC_E_NONE;
    } else
        return SOC_E_NOT_FOUND;
}

soc_error_t osal_set_param(const char *file, const char *param, const char *value) {
    return SOC_E_UNAVAIL;
}

#endif  /* CONFIG_FS */
