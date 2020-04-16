/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 */

/**
 * @file
 * Coronado Bridge
 * Forwarding Services Abstraction Layer (FSAL) API
 * CBX LINK
 */

#ifndef CBX_API_LINK_H_
#define CBX_API_LINK_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @defgroup cbx_link CBX LINK
 * Forwarding Services Abstraction Layer (FSAL) Link Scan operations.
 *
 * Link Scan provides link monitoring, notification, and recovery
 *
 * The CBX API provides a mechanism to scan the ports,
 * monitoring for link-up and link-down events.
 * This is accomplished using a thread to constantly monitor the
 * PHYs for link status, and then calling user-supplied callbacks.
 *
 * The cbx_port_link_status_get API call retrieves the current link status
 * from the link_scan task, and not by reading the PHY itself,
 * allowing multiple applications to recover the current link status
 * without the possibility of missing an event.
 *
 * For this reason, the use of the supplied link_scan is recommended,
 * and link status should always be retrieved using the
 * appropriate API calls and not by inspecting the PHY registers directly.
 *
 * Link scanning works with the cbx_port_update API function.
 * It calls the function on link-up and link-down events,
 * to configure the MAC to operate properly with the current PHY mode,
 * which includes programming the MAC to match autonegotiated settings.
 *
 * When using link_scan, users of the CBX library should not call cbx_port_update.
 * The link_scan process ensures that user supplied callbacks are called
 * once for each link-up and link-down event recognized on each port.
 *
 *  @{
 */


/**
 * Link Scan parameters struct
 * information about Link Scan
 */
typedef struct cbx_link_scan_params_t {
    uint32_t               flags;       /**< Flags:  CBX_LINK_XX */
    int                    usec;        /**< Link Scan interval in usec */
} cbx_link_scan_params_t;

/**
 *  Link Scan callback information structure
 */
typedef struct cbx_link_scan_info_t {
    uint32_t         flags;            /**< Flags:  CBX_LINK_CALLBACK_XX */
    char            *name;             /**< Registered callback name string */
} cbx_link_scan_info_t;

/**
 * Link Scan Callback prototype
 *
 * Function prototype definition for user supplied link scan callback.
 * Callback is performed in the context of the link_scan thread.
 *
 * @param portid        (IN)  Port Identifier.
 * @param value         (IN)  Port link status
 */
typedef void ( *cbx_link_scan_callback_t )
( cbx_portid_t       portid,
  uint32_t           value );


/**
 * Initialize Link scan module
 * This routine is used to init link scan.
 *
 * The link_scan thread is created during this call.
 * Link scanning is not performed until enabled using cbx_link_scan_enable_set
 *
 * @param link_scan_params  (IN)  Link scan parameters.
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_link_scan_init( cbx_link_scan_params_t *link_scan_params );


/**
 * Detach a Link scan previously initialized by cbx_link_scan_init()
 * Terminates the link_scan thread, and frees all memory associated
 * with link_scan on the device.
 * All registered handlers are unregistered requiring them
 * to re-register if cbx_link_scan_init is called to reinitialize link_scan.
 *
 * @param link_scan_params  (IN)  Link scan parameters.
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 *
 * @note This routine destroys a previously initialized link scan.
 */

int cbx_link_scan_detach ( cbx_link_scan_params_t *link_scan_params );


/**
 * Link scan register
 * This routine is used to register a Link Scan callback routine.
 * Register a handler invoked on a link up or link down transition.
 * If multiple handlers are registered, they are invoked on a
 * link transition in the same order as they were registered.
 *
 * @param link_scan_info (IN)  Link Scan callback information
 * @param cb             (IN)  Callback function pointer
 *                             see: cbx_link_scan_callback_t
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_link_scan_register ( cbx_link_scan_info_t      link_scan_info,
                             cbx_link_scan_callback_t  cb );


/**
 * Link scan unregister
 * This routine is used to unregister a Link Scan callback routine.
 *
 * @param cb           (IN)  Link Scan callback routine to unregister
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_link_scan_unregister ( cbx_link_scan_callback_t  cb );


/**
 * Link scan enable get
 * Retrieve the link scan enable and polling interval.
 *
 * cbx_link_scan_enable_get returns the current scan interval.
 * A returned scan interval of 0 indicates link_scan is disabled.
 *
 * @param link_scan_params  (OUT) Link Scan parameters.
 *                              usec is the minimum time between software link scan cycles
 *                              in micro-seconds. 0 indicates link_scan is disabled.
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_link_scan_enable_get ( cbx_link_scan_params_t  *link_scan_params );


/**
 * Link scan enable set
 * Enable link scan and set the polling interval.
 *
 * Calling cbx_link_scan_enable_set with a non-zero scanning interval
 * sets the scan interval.
 * If the specified interval is zero, the link_scan task is stopped.
 *
 * @param link_scan_params  (IN)  Link Scan parameters.
 *                             usec is the minimum time between software link scan cycles
 *                             in micro-seconds. 0 indicates link_scan is disabled.
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_link_scan_enable_set ( cbx_link_scan_params_t  *link_scan_params );


/**
 * Link scan dump
 * Display information about link_scan callbacks.
 *
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_link_scan_dump ( void );


/**
 * @}
 */


#ifdef __cplusplus
}
#endif

#endif

