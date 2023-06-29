/*
 * $Copyright: (c) 2017 Broadcom Limited.
 */

/**
 * @file
 * Coronado Bridge
 * Forwarding Services Abstraction Layer (FSAL) API
 * CBX SWITCH
 */

#ifndef CBX_API_SWITCH_H_
#define CBX_API_SWITCH_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @defgroup cbx_switch CBX SWITCH
 * Forwarding Services Abstraction Layer (FSAL) Switch operations.
 *
 *  @{
 */

/**
 * @typedef cbx_switch_t
 * CBX switch
 */

/** @def CBX_SWITCH_RND_MAX
 * Maximum random number array for initialization
 */
#define CBX_SWITCH_RND_MAX  32

/**
 * Switch Software mode
 */
typedef enum cbx_switch_sw_mode_e {
    cbxSwitchSWModeUnmanaged,
    cbxSwitchSWModeUnmanagedPlus,
    cbxSwitchSWModeManaged,
} cbx_switch_sw_mode_t;

/**
 * Switch Vlan mode
 */
typedef enum cbx_switch_vlan_mode_e {
    cbxSwitchVlanModeShared,
    cbxSwitchVlanModeIndependent,
} cbx_switch_vlan_mode_t;

/**
 * Switch Port mode
 */
typedef enum cbx_switch_port_type_e {
    cbxSwitchPortTypeNormal,
    cbxSwitchPortTypeDisabled,
    cbxSwitchPortTypeIMP,
    cbxSwitchPortTypeCascade,
} cbx_switch_port_type_t;

/**
 * Switch Port Max speed
 */
typedef enum cbx_switch_port_max_speed_e {
    cbxSwitchPortSpeed100,
    cbxSwitchPortSpeed1000,
    cbxSwitchPortSpeed2500,
    cbxSwitchPortSpeed10000,
} cbx_switch_port_max_speed_t;

/**
 * Switch port setup configuration
 *
typedef struct cbx_switch_port_config_t {
    cbx_switch_port_type_t       port_type;
    cbx_switch_port_max_speed_t  port_max_speed;
} cbx_switch_port_config_t;

 *
 * Switch parameter struct
 * information about a switch
 */
typedef struct cbx_switch_params_t {
    cbx_switch_sw_mode_t     sw_mode;      /**< Switch SW mode of operation */
    cbx_switch_vlan_mode_t   vlan_mode;    /**< VLAN Shared/Independent learning */
    int                      default_vlan; /**< default VLAN=1 option */
    uint32_t     rnd[CBX_SWITCH_RND_MAX];  /**< random numbers for ARL and LIM */
} cbx_switch_params_t;


#if 0
cbx_switch_tags[CBX_SWITCH_TPID_MAX];    /**< programmable TPID fields */
cbx_switch_range[CBX_SWITCH_RANGE_MAX];  /**< programmable ranges      */
#endif

/**
 * Initialize switch entry parameters.
 *
 * @param  switch_params  parameter object
 */
void cbx_switch_params_t_init( cbx_switch_params_t *switch_params );


/**
 * Swtich Control placholder
 */
typedef int cbx_switch_control_t;

/**
 * Switch Initialization
 * and all other APIs take cb as first parameter
 * - this is the First CBX API
 *    prior to this the CM needs to be configured
 * - switch_init initializes all the FSAL modules
 * - default setup of SLIC, Key Building, Traps, etc.
 *
 * @param switch_params   (IN)  Switch configuration parameters
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_switch_init ( cbx_switch_params_t *switch_params );


/**
 * Detach a Switch previously initialized by cbx_switch_init()
 *
 * @param switch_params   (IN)  Switch configuration parameters
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 *
 * @note This routine destroys a previously initialized switch.
 */

int cbx_switch_detach ( cbx_switch_params_t *switch_params );


/**
 * Switch control get
 * This routine is used to get Switch control parameters.
 *
 * @param control    (IN)  Switch control
 * @param value      (OUT) value
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_switch_control_get ( cbx_switch_control_t  control,
                             int                  *value );

/**
 * Switch control set
 * This routine is used to set Switch control parameters.
 *
 * @param control    (IN)  Switch control
 * @param value      (IN)  value
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_switch_control_set ( cbx_switch_control_t control,
                             int                  value );



/**
 * @}
 */


#ifdef __cplusplus
}
#endif

#endif
