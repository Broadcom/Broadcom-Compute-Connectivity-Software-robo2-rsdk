/*
 * * $ID: $
 * * $Copyright: (c) 2017 Broadcom Corp.
 * * All Rights Reserved$
 * *
 * * File:
 * *     sal_fs.c
 * * Purpose:
 * *     Openrtos osal module for file system apis. This provides abstraction
 * *     for openrtos file system related service.
 * *
 * */

#include "sal_fs.h"

/* Stubs for file system apis */
/*
 * Function:
 *    sal_fs_init
 * Purpose:
 *    Formats the file system to the specified type
 */
int sal_fs_init(fs_dev_type_t dev, uint32_t start_addr, uint32_t size, fs_type_t *format)
{
    return -1;
}

/*
 * Function:
 *    sal_fs_format
 * Purpose:
 *    Formats the file system to the specified type
 */
int sal_fs_format(fs_dev_type_t dev, fs_type_t type)
{
    return -1;
}

/*
 * Function:
 *    sal_fs_getfreespace
 * Purpose:
 *    Retrieves the file system's disk usage 
 */
int sal_fs_getfreespace(fs_dev_type_t dev, fs_space_t *pspace)
{
    return -1;
}

/*
 * Function:
 *    sal_fs_mkdir
 * Purpose:
 *    Create a directory in the file system
 */
int sal_fs_mkdir(const char *dirname)
{
    return -1;
}

/*
 * Function:
 *    sal_fs_rmdir
 * Purpose:
 *    Delete a directory in the file system
 */
int sal_fs_rmdir(const char *dirname)
{
    return -1;
}

/*
 * Function:
 *    sal_fs_chdir
 * Purpose:
 *    Change directory to path specified
 */
int sal_fs_chdir(const char *dirname)
{
    return -1;
}

/*
 * Function:
 *    sal_fs_getcwd
 * Purpose:
 *    Get current working directory
 */
int sal_fs_getcwd(char *buffer, uint8_t maxlen)
{
    return -1;
}

/*
 * Function:
 *    sal_fs_ls
 * Purpose:
 *    List the contents for a directory.
 *    sub directories are indicated with a '\'
 */
int sal_fs_ls(const char *dirname)
{
    return -1;
}
