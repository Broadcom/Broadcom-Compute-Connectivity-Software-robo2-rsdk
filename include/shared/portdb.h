/*
 * $Id: portdb.h
 *
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.  All Rights Reserved.$
 *
 */

#ifndef _PORTDB_H_
#define _PORTDB_H_


/* User port types */
enum user_port_type {
    USER_PORT_TYPE_NONE,
    USER_PORT_TYPE_PHYSICAL,
    USER_PORT_TYPE_LAG
};

/* port database - provides the mappings for port identifiers */
typedef struct portdb_s {
    int8         uport_idx;      /* user port id */
    char         uport_name[10]; /* user port name */
    uint8        uport_type;     /* user port type */
    int8         lport_idx;      /* logical port id - switch port id */
    cbx_portid_t fport;          /* fsal port id */
    uint8        ext_phy_addr;   /* external phy address */
    uint32       port_capab;     /* port capabilities flag */  
} portdb_t;

/* port Data Base registration details */
typedef struct portdb_reg_s {
    portdb_t *portdb;  /* pointer to port DB structure */
    uint8 num_entries; /* number of port DB entries */
} portdb_reg_t;

extern portdb_reg_t portdb_reg;
/* APIs */
extern void portdb_register(portdb_t *portdb, uint8 num_entries);
extern uint8 portdb_uport_phys_validate(uint8 intf);
extern uint8 portdb_uport_lag_validate(uint8 intf);
extern uint8 portdb_uport_intf_validate(uint8 intf);
extern uint8 portdb_uport_count_get();
extern int8 portdb_uport_phys_first_get();
extern uint8 portdb_uport_phys_count_get();
extern int8 portdb_uport_lag_first_get();
extern uint8 portdb_uport_lag_count_get();
extern int portdb_uport_name_get(uint8 uport_idx, char *uport_name);
extern int portdb_uport_to_fport_get(uint8 uport_idx, cbx_portid_t *fport);
extern int portdb_fport_to_uport_get(cbx_portid_t fport, uint8 *uport_idx);
extern int portdb_lport_to_ext_phy_addr_get(uint8 lport_idx,
                                            uint8 *ext_phy_addr);
extern int portdb_uport_capab_get(uint8_t uport_idx, uint32_t *attrib);
extern int portdb_uport_phys_type_get(uint8_t uport_idx, uint32_t *phys_type);
#endif
