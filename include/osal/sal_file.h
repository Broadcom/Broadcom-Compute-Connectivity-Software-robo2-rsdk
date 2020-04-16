/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved.$
 *
 * File: sal_file.h
 * Purpose: File operations
 */

#ifndef _SAL_FILE_H
#define _SAL_FILE_H

#include "sal_types.h"

#ifdef OPENRTOS
#include "FreeRTOS.h"
#endif

#include "sal_console.h"
#include "bsl.h"

/* File modes */
enum {
    SAL_FS_DIR   = 0x1,
    SAL_FS_FILE  = 0x2,
};

/* Macros to check if file is a direcory or a regular file */
#ifndef S_ISDIR
    #define S_ISDIR(MODE)   (MODE == SAL_FS_DIR)
#endif
#ifndef S_ISREG
    #define S_ISREG(MODE)   (MODE == SAL_FS_FILE)
#endif
/* File stat structure */
typedef struct sal_fstat_s {
    uint32_t st_size;
    int32_t  st_mtim;
    uint8_t  st_mode;
} sal_fstat_t;

extern FILE *sal_fopen(const char *filename, const char *mode);
extern int sal_fclose(FILE *handle);
extern int sal_fread(void *buf, int size, int num, FILE *handle);
extern int sal_fwrite(void *buf, int size, int num, FILE *handle);
extern int sal_fseek(FILE *handle, int offset, unsigned char whence);
extern int sal_ftell(FILE *handle);
extern int sal_fgetc(FILE *handle);
extern int sal_fputc(int ch, FILE *handle);
extern int sal_feof(FILE *handle);
extern int sal_fdelete(const char *filename);
extern uint32_t sal_fsize(const char *filename);
extern int sal_fstat(const char *filename, sal_fstat_t *stat);
extern int sal_frename(const char *old_path, const char *new_path);

#define sal_frewind(FILE_PTR)      sal_fseek(FILE_PTR, 0, SEEK_SET)
#define sal_fseteof(FILE_PTR)      sal_fseek(FILE_PTR, 0, SEEK_END)

#endif /* !_SAL_FILE_H */
