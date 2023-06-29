/*
 * $Copyright: (c) 2017 Broadcom Limited.
 */

/**
 * @file
 * Forwarding Services Abstraction Layer (FSAL) API
 */

#ifndef CBX_API_CDIAG_H_
#define CBX_API_CDIAG_H_

#ifdef __cplusplus
extern "C" {
#endif


typedef enum cbx_cdiag_result_e {
    cbxCdiagResultNill         = 0,
    cbxCdiagResultBusy         = 1,
    cbxCdiagResultInvalid      = 2,
    cbxCdiagResultPass         = 3,
    cbxCdiagResultFail         = 4
} cbx_cdiag_result_t;

typedef enum cbx_cdiag_state_e {
    cbxCableStateInvalid       = 0,
    cbxCableStateOK            = 1,
    cbxCableStateOpen          = 2,
    cbxCableStateIntraShort    = 3,
    cbxCableStateInterShort    = 4,
    cbxCableStateBusy          = 5
} cbx_cdiag_state_t;

typedef struct cbx_port_cable_diag_s {
    cbx_cdiag_result_t         port_cdiag_result;
    cbx_cdiag_state_t          pair_state[4];
    uint32_t                   pair_len[4];
} cbx_port_cable_diag_t;



/**
 * Run cable diag on given port.
 *
 * @param portid       (IN)  portid on which Cable diag has to run
 *                           This always represent physical port.
 * @param port_result  (OUT) Port cable diag result
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_port_cable_diag (
                cbx_portid_t portid, cbx_port_cable_diag_t *port_result);

/**
 * @}
 */


#ifdef __cplusplus
}
#endif

#endif
