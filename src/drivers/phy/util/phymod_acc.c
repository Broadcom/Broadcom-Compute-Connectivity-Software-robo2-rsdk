/*
 * $Id: phymod_acc.c,v 1.1.2.6 2013/07/10 07:18:49 mlarsen Exp $
 * $Copyright: (c) 2017 Broadcom Corp.
 * All Rights Reserved$
 */

#include <phymod/phymod_acc.h>

int
phymod_acc_check(const phymod_access_t *pa)
{
    if (pa == 0 ||
        PHYMOD_ACC_BUS(pa) == 0 ||
        PHYMOD_ACC_BUS(pa)->read == 0 ||
        PHYMOD_ACC_BUS(pa)->write == 0) {
        return -1;
    }
    return 0;
}

int
phymod_bus_read(const phymod_access_t *pa, uint32_t reg, uint32_t *data)
{
    /* Read raw PHY data */
    return PHYMOD_ACC_BUS(pa)->read(PHYMOD_ACC_USER_ACC(pa),
                                    PHYMOD_ACC_BUS_ADDR(pa),
                                    reg, data);
}

int
phymod_bus_write(const phymod_access_t *pa, uint32_t reg, uint32_t data)
{
    /* Write raw PHY data */
    return PHYMOD_ACC_BUS(pa)->write(PHYMOD_ACC_USER_ACC(pa),
                                     PHYMOD_ACC_BUS_ADDR(pa),
                                     reg, data);
}

int
phymod_is_write_disabled(const phymod_access_t *pa, uint32_t *data)
{
    if (pa && PHYMOD_ACC_BUS(pa)) {
        if((PHYMOD_ACC_BUS(pa)->is_write_disabled) == NULL) { 
            *data = 0; 
        } else {
            return PHYMOD_ACC_BUS(pa)->is_write_disabled(PHYMOD_ACC_USER_ACC(pa),
                                         data);
        }
    } else {
        *data = 0;
    }
    
    return 0;
}
