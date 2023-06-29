/*
 * $Copyright: (c) 2017 Broadcom Corp.
 * All Rights Reserved.$
 *
 * File: sal_fs.h
 * Purpose: File system operations
 */

#ifndef _SAL_FS_H
#define _SAL_FS_H

#include "sal_types.h"
#include <stdbool.h>

#ifdef OPENRTOS
#include "FreeRTOS.h"
#endif

#include "sal_console.h"
#include "bsl.h"

/* File info fdate mask and shift values */
#define DATE_YEAR_MASK      0xFE00
#define DATE_YEAR_SHIFT     9
#define DATE_MONTH_MASK     0x01E0
#define DATE_MONTH_SHIFT    5
#define DATE_DAY_MASK       0x001F
#define DATE_DAY_SHIFT      0

extern bool fs_init_complete;

#define FS_INIT_CHECK() ((fs_init_complete) ? 0 : sal_fs_init_all())

#define FS_INIT_CHECK_RETURN()      \
    do {                            \
        int ret;                    \
        if (!(fs_init_complete)) {  \
            ret = sal_fs_init_all();\
            if (ret != 0) {         \
                return -1;          \
            }                       \
        }                           \
    } while (0)

/** File system type */
typedef enum fs_type {
    FS_FAT12 = 1,   /* Fat 12 file system */
    FS_FAT16 = 2,   /* Fat 16 file system */
    FS_FAT32 = 3,   /* Fat 32 file system */
} fs_type_t;

/** MTD Device for mounting the file system (flash or ram) */
typedef enum fs_dev_type {
    FS_DEV_FLASH_WEB_ROOT,   /* flash - web_root*/
    FS_DEV_FLASH_CONFIG,     /* flash - config */
    FS_DEV_FLASH_MAX,
} fs_dev_type_t;

/** File system space info */
typedef struct fs_space
{
    uint32_t  total;       /* Total file system size */
    uint32_t  free;        /* Free space */
    uint32_t  used;        /* Used space */
    uint32_t  bad;         /* Size marked as bad */
    uint32_t  total_high;  /* Total high watermark */
    uint32_t  free_high;   /* Free space high watermark */
    uint32_t  used_high;   /* Used space high watermark */
    uint32_t  bad_high;    /* Bad space high watermark */
} fs_space_t;

extern int sal_fs_init_all(void);
extern int sal_fs_init(fs_dev_type_t dev, uint32_t start_addr, uint32_t size, fs_type_t *format);
extern int sal_fs_format(fs_dev_type_t dev, fs_type_t type);
extern int sal_fs_getfreespace(fs_dev_type_t dev, fs_space_t *pspace);
extern int sal_fs_mkdir(const char *dirname);
extern int sal_fs_rmdir(const char *dirname);
extern int sal_fs_chdir(const char *dirname);
extern int sal_fs_getcwd(char *buffer, uint8_t maxlen);
extern int sal_fs_ls(const char *dirname);

extern char *sal_fs_error_str(int error);

#define FS_FLASH    1
#define FS_RAM      1

#endif /* !_SAL_FS_H */
