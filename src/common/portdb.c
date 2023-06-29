/*
 * $Id: portdb.c
 *
 * $Copyright: (c) 2017 Broadcom Corp.  All Rights Reserved.$
 *
 */

#include "osal/sal_types.h"
#include "osal/sal_alloc.h"
#include "osal/sal_libc.h"
#include "shared/types.h"
#include "shared/portdb.h"
#include "shared/port_capab.h"

portdb_reg_t portdb_reg;

/*
 * Function:
 *  portdb_register
 * Purpose:
 *  Registers the port database. The board initialization code calls this
 *  API after the ports are created with FSAL layer using cbx_port_create().
 *  After registration, other modules in the system can use port DB APIs.
 */
void portdb_register(portdb_t *portdb, uint8 num_entries)
{
    portdb_reg.num_entries = num_entries;
    portdb_reg.portdb = (portdb_t *)sal_alloc(num_entries * sizeof(portdb_t),
                                              "PORTDB");
    sal_memcpy(portdb_reg.portdb, portdb, (num_entries * sizeof(portdb_t)));
}

/*
 * Function:
 *  portdb_uport_count_get
 * Purpose:
 *  Gets the number of user ports on the board. This can be be used by
 *  applications for iterating through user ports on the board.
 */
uint8 portdb_uport_count_get()
{
    uint8 idx, count = 0;

    for (idx = 0; idx < portdb_reg.num_entries; idx++) {
        if ( portdb_reg.portdb[idx].uport_idx != -1) {
            count++;
        }
    }
    return count;
}

/*
 * Function:
 *  portdb_uport_phys_first_get
 * Purpose:
 *  Gets the index of first physical user port on the board. This can be used in
 *  conjunction with the portdb_uport_phys_count_get() API to iterate through
 *  all the physical user ports in the system.
 */
int8 portdb_uport_phys_first_get()
{
    uint8 idx;

    for (idx = 0; idx < portdb_reg.num_entries; idx++) {
        if (portdb_reg.portdb[idx].uport_type == USER_PORT_TYPE_PHYSICAL) {
            return (portdb_reg.portdb[idx].uport_idx);
        }
    }
    return -1;
}

/*
 * Function:
 *  portdb_uport_phys_count_get
 * Purpose:
 *  Gets the number of physical user ports on the board. This can be used in
 *  conjunction with the portdb_uport_phys_first_get() API to iterate through
 *  all the physical user ports in the system.
 */
uint8 portdb_uport_phys_count_get()
{
    uint8 idx, count = 0;

    for (idx = 0; idx < portdb_reg.num_entries; idx++) {
        if (portdb_reg.portdb[idx].uport_type == USER_PORT_TYPE_PHYSICAL) {
            count++;
        }
    }
    return count;
}

/*
 * Function:
 *  portdb_uport_phys_validate
 * Purpose:
 *  Is this a valid physical interface.
 */
uint8 portdb_uport_phys_validate(uint8 intf)
{
    uint8 first, count, last;

    count = portdb_uport_phys_count_get();
    if (count == 0) {
        return 0;
    }
    first = (uint8) portdb_uport_phys_first_get();
    last = first + count - 1;

    if ((intf >= first) && (intf <= last)) {
        return 1;
    } else {
        return 0;
    }
}

/*
 * Function:
 *  portdb_uport_lag_validate
 * Purpose:
 *  Is this a valid lag interface.
 */
uint8 portdb_uport_lag_validate(uint8 intf)
{
    uint8 first, count, last;

    count = portdb_uport_lag_count_get();
    if (count == 0) {
        return 0;
    }
    first = (uint8) portdb_uport_lag_first_get();
    last = first + count - 1;

    if ((intf >= first) && (intf <= last)) {
        return 1;
    } else {
        return 0;
    }
}

/*
 * Function:
 *  portdb_uport_intf_validate
 * Purpose:
 *  Is this a valid interface.
 */
uint8 portdb_uport_intf_validate(uint8 intf) {
    if (portdb_uport_phys_validate(intf) || portdb_uport_lag_validate(intf)) {
        return 1;
    } else {
        return 0;
    }
}


/*
 * Function:
 *  portdb_uport_lag_first_get
 * Purpose:
 *  Gets the index of first LAG user ports on the board. This can be used in
 *  conjunction with the portdb_uport_lag_count_get() API to  iterate through
 *  all the LAG user ports in the system.
 */
int8 portdb_uport_lag_first_get()
{
    uint8 idx;

    for (idx = 0; idx < portdb_reg.num_entries; idx++) {
        if ( portdb_reg.portdb[idx].uport_type == USER_PORT_TYPE_LAG) {
            return (portdb_reg.portdb[idx].uport_idx);
        }
    }
    return -1;
}

/*
 * Function:
 *  portdb_uport_lag_count_get
 * Purpose:
 *  Gets the number of LAG user ports on the board. This can be used in
 *  conjunction with the portdb_uport_lag_first_get() API to  iterate through
 *  all the LAG user ports in the system.
 */
uint8 portdb_uport_lag_count_get()
{
    uint8 idx, count = 0;

    for (idx = 0; idx < portdb_reg.num_entries; idx++) {
        if ( portdb_reg.portdb[idx].uport_type == USER_PORT_TYPE_LAG) {
            count++;
        }
    }
    return count;
}

/*
 * Function:
 *  portdb_uport_name_get
 * Purpose:
 *  Gets the user port name for the given user port index.
 */
int portdb_uport_name_get(uint8 uport_idx, char *uport_name)
{
    uint8 idx;

    for (idx = 0; idx < portdb_reg.num_entries; idx++) {
        if ( portdb_reg.portdb[idx].uport_idx == uport_idx) {
            sal_strcpy(uport_name, portdb_reg.portdb[idx].uport_name);
            return 0;
        }
    }
    return -1;
}

/*
 * Function:
 *  portdb_uport_to_fport_get
 * Purpose:
 *  Gets the FSAL port identifier for the given user port index.
 */
int portdb_uport_to_fport_get(uint8 uport_idx, cbx_portid_t *fport)
{
    uint8 idx;

    for (idx = 0; idx < portdb_reg.num_entries; idx++) {
        if ( portdb_reg.portdb[idx].uport_idx == uport_idx) {
            *fport = portdb_reg.portdb[idx].fport;
            return 0;
        }
    }
    return -1;
}

/*
 * Function:
 *  portdb_fport_to_uport_get
 * Purpose:
 *  Gets the user port identifier for the given FSAL port index.
 */
int portdb_fport_to_uport_get(cbx_portid_t fport, uint8 *uport_idx)
{
    uint8 idx;

    for (idx = 0; idx < portdb_reg.num_entries; idx++) {
        if ( portdb_reg.portdb[idx].fport == fport) {
            *uport_idx = portdb_reg.portdb[idx].uport_idx;
            return 0;
        }
    }
    return -1;
}

/*
 * Function:
 *  portdb_lport_to_ext_phy_addr_get
 * Purpose:
 *  Gets the PHY address for a given logical port(switch port). The PHY drivers
 *  for external PHYs can use this routine.
 */
int portdb_lport_to_ext_phy_addr_get(uint8 lport_idx, uint8 *ext_phy_addr)
{
    uint8 idx;

    for (idx = 0; idx < portdb_reg.num_entries; idx++) {
        if (portdb_reg.portdb[idx].lport_idx == lport_idx) {
            *ext_phy_addr = portdb_reg.portdb[idx].ext_phy_addr;
            return 0;
        }
    }
    return -1;
}

/*
 * Function:
 *  portdb_uport_capab_get
 * Purpose:
 *  Gets the port capabilities for a given user port.
 */
int portdb_uport_capab_get(uint8_t uport_idx, uint32_t *attrib)
{
    uint8 idx;

    for (idx = 0; idx < portdb_reg.num_entries; idx++) {
        if ( portdb_reg.portdb[idx].uport_idx == uport_idx) {
            *attrib = portdb_reg.portdb[idx].port_capab;
            return 0;
        }
    }
    return -1;
}

/*
 * Function:
 *  portdb_uport_phys_type_get
 * Purpose:
 *  Gets the physical port type for a given user port.
 *      PORT_CAPAB_GPHY
 *      PORT_CAPAB_SGMII
 *      PORT_CAPAB_QSGMII
 *      PORT_CAPAB_XFI
 */
int portdb_uport_phys_type_get(uint8_t uport_idx, uint32_t *phys_type)
{
    int      rv;
    uint32_t attrib;

    rv = portdb_uport_capab_get(uport_idx, &attrib);
    if (rv == 0) {
        if (attrib & PORT_CAPAB_GPHY) {
            *phys_type = PORT_CAPAB_GPHY;
        } else if (attrib & PORT_CAPAB_SGMII) {
            *phys_type = PORT_CAPAB_SGMII;
        } else if (attrib & PORT_CAPAB_QSGMII) {
            *phys_type = PORT_CAPAB_QSGMII;
        } else if (attrib & PORT_CAPAB_XFI) {
            *phys_type = PORT_CAPAB_XFI;
        } else {
            rv = -1;
        }
    }
    return rv;
}

