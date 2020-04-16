/* *
 * * $ID: $
 * * 
 * * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 * * 
 * * $Copyright: (c) 2020 Broadcom Inc.
 * *
 * * File:
 * *     init.h
 * * Purpose:
 * *     This file contains definitions internal to init module.
 */

#ifndef CBX_INT_INIT_H
#define CBX_INT_INIT_H

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

#ifdef __cplusplus
}
#endif


#endif /* CBX_INT_INIT_H */
