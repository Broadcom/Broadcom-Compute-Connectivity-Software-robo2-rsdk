/*
 * * $ID: $
 * * 
 * * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 * * 
 * * $Copyright: (c) 2020 Broadcom Inc.
 * * All Rights Reserved$
 * *
 * * File:
 * *     sal_file.c
 * * Purpose:
 * *     Openrtos sal module for file apis. This provides abstraction for openrtos
 * *     file related service.
 * *
 * */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "sal_file.h"

/*
 * Function:
 *    sal_fopen
 * Purpose:
 *    Open a file and return the descriptor
 * Parameters:
 *    filename - path to file
 *    mode - file open mode string
 * Returns:
 *    Non-Zero FILE descriptor on success
 *    NULL on failure
 */
FILE *sal_fopen(const char *filename, const char *mode)
{
    return fopen(filename, mode);
}

/*
 * Function:
 *    sal_fclose
 * Purpose:
 *    Close a file
 * Parameters:
 *    handle - file descriptor handle
 * Returns:
 *    0 on success
 *    EOF on failure
 */
int sal_fclose(FILE *handle)
{
    return fclose(handle);
}

/*
 * Function:
 *    sal_fread
 * Purpose:
 *    Read from a file
 * Parameters:
 *    buf - buffer to read into
 *    size - size of the element to read
 *    num - number of elements to read
 *    handle - file descriptor handle
 * Returns:
 *    number of elements read
 */
int sal_fread(void *buf, int size, int num, FILE *handle)
{
    return fread(buf, size, num, handle);
}

/*
 * Function:
 *    sal_fwrite
 * Purpose:
 *    Write to a file
 * Parameters:
 *    buf - buffer to write from
 *    size - size of the element to write
 *    num - number of elements to write
 *    handle - file descriptor handle
 * Returns:
 *    number of elements written
 */
int sal_fwrite(void *buf, int size, int num, FILE *handle)
{
    return fwrite(buf, size, num, handle);
}

/*
 * Function:
 *    sal_fseek
 * Purpose:
 *    Seek to a specified position in the file
 * Parameters:
 *    handle - file descriptor handle
 *    offset - Seek offset from 'whence'
 *    whence - Position to where the offset will be added
 *             (SEEK_CUR, SEEK_SET, SEEK_END)
 * Returns:
 *    0 on success
 *    -1 on failure
 */
int sal_fseek(FILE *handle, int offset, unsigned char whence)
{
    return fseek(handle, offset, whence);
}

/*
 * Function:
 *    sal_ftell
 * Purpose:
 *    Retrieve the current position in the file
 * Parameters:
 *    handle - file descriptor handle
 * Returns:
 *    Position in file from start
 *    -1 on error
 */
int sal_ftell(FILE *handle)
{
    return ftell(handle);
}

/*
 * Function:
 *    sal_fgetc
 * Purpose:
 *    Read one byte from the file
 * Parameters:
 *    handle - file descriptor handle
 * Returns:
 *    Character read from file on success
 *    EOF on failure
 */
int sal_fgetc(FILE *handle)
{
    return fgetc(handle);
}

/*
 * Function:
 *    sal_fputc
 * Purpose:
 *    Write one byte to the file
 * Parameters:
 *    ch - character to write
 *    handle - file descriptor handle
 * Returns:
 *    Character written to file on success
 *    EOF on failure
 */
int sal_fputc(int ch, FILE *handle)
{
    return fputc(ch, handle);
}

/*
 * Function:
 *    sal_feof
 * Purpose:
 *    End of file check
 * Parameters:
 *    handle - file descriptor handle
 * Returns:
 *    0 if end of file reached
 *    -1 on error or not at end of file
 */
int sal_feof(FILE *handle)
{
    return feof(handle);
}

/*
 * Function:
 *    sal_fdelete
 * Purpose:
 *    Delete a file
 * Parameters:
 *    filename - path to the file
 * Returns:
 *    0 on success
 *    -1 on failure
 */
int sal_fdelete(const char *filename)
{
    return remove(filename);
}

/*
 * Function:
 *    sal_fsize
 * Purpose:
 *    Retrieve the size of a file
 * Parameters:
 *    filename - path to the file
 * Returns:
 *    file size on success
 *    -1 on error
 */
uint32_t sal_fsize(const char *filename)
{
    int ret;
    struct stat st;

    ret = stat(filename, &st);

    return (ret == 0) ? st.st_size : -1;
}

int sal_fstat(const char *filename, sal_fstat_t *sal_stat)
{
    int ret;
    struct stat st;

    ret = stat(filename, &st);
    if (ret == 0)
    {
        sal_stat->st_size = st.st_size;
        sal_stat->st_mtim = st.st_mtim.tv_sec;
        sal_stat->st_mode = S_ISDIR(st.st_mode) ? SAL_FS_DIR : SAL_FS_FILE;
    }

    return ret;
}

/*
 * Function:
 *    sal_frename
 * Purpose:
 *    Rename a file or directory
 * Parameters:
 *    old_path - path to existing file or directory
 *    new_path - path to new file or directory
 * Returns:
 *    0 on success
 *    -1 on error
 */
int sal_frename(const char *old_path, const char *new_path)
{
    return rename(old_path, new_path);
}
