/* *
 * * $ID: $
 * * $Copyright: (c) 2017 Broadcom Limited.
 * *
 * * File:
 * *     init.h
 * * Purpose:
 * *     This file contains definitions internal to init module.
 */

#ifndef CBX_INT_INIT_H
#define CBX_INT_INIT_H
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/**
 * CBX Fsal init
 *
 * @param  None
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */
extern int cbx_fsal_init(void);

/**
 * CBX fsal m7 hybrid mode init
 *
 * @param  None
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */
extern int cbx_fsal_m7_hybrid_mode_init(void);

/**
 * CBX Fsal uninit
 *
 * @param  None
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */
extern int cbx_fsal_uninit(void);

/**
 * CBX Fsal Get cfp id
 *
 * @param  pointer to cfp id to be returned
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */
extern int cbx_fsal_get_cfp_id(cbx_cfpid_t *cfpid);
extern int bcm_cbx_vlan_get(uint32_t vlanid);
extern int bcm_cbx_vlan_create(uint32_t vlanid);
extern int bcm_cbx_vlan_port_get(uint32_t vlanid);
extern int bcm_cbx_vlan_port_add(uint32_t vlanid, uint32_t portid, uint32_t mode);
extern int bcm_cbx_vlan_port_remove(uint32_t vlanid, uint32_t portid);
extern int bcm_port_info_get(uint32_t portid);
extern int bcm_port_info_set(uint32_t portid, uint32_t porttype);
extern int bcm_cbx_vlan_port_pvid_set(uint16 vlanid, uint32_t portid);
extern int bcm_cbx_port_stat_get(uint32_t portid, uint32_t type);


#ifdef __cplusplus
}
#endif


#endif /* CBX_INT_INIT_H */
