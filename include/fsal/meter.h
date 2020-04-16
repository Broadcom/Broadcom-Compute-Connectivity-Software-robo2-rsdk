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
 */

#ifndef CBX_API_METER_H_
#define CBX_API_METER_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @defgroup cbx_meter CBX METER
 * Forwarding Services Abstraction Layer (FSAL) Meter operations.
 *
 * <h3>Meter Types</h3>
 *
 * <h4>BMU Meters</h4>
 * - 128 meters provide 8 per TC limits per each of 16 ports: 128 total.
 *   32 profiles
 *   BMU meters can operate in two mode:
 *       1) per port per traffic class color (DP) marking
 *       2) per port shaping to generate flow control (pause) back to the source port
 *
 * <h4>IPP Meters</h4>
 * - 4 meters per port for unicast, multicast, broadcast and DLF
 *   32 profiles
 *
 * <h4>PQM Meters</h4>
 * - 16 Trap Groups each have their own meter each with their own profile
 *
 * <h4>CFP Meters</h4>
 * - 511 Meters (selected by action)
 *   64 profile
 *
 * <h3>Meter Actions</h3>
 * Policing affects the Drop Precedence (DP) or may drop the packet.
 *
 * <h3>Ingress Metering</h3>
 * Avenger provides a set of meters per port.
 *  These meters may be configured to rate limit arriving traffic
 *  Buffer Manager meters on a per Traffic Class basis.
 *  Traffic type metering:
 *    - Unicast,
 *    - Multicast,
 *    - Broadcast and
 *    - DLF.
 *
 * <h3>Buffer Manager Metering</h3>
 * Buffer Manager metering is performed based on the initial TC assignment,
 * prior to flow processing or ARL lookup with the exception of
 * DLF metering, which is done after the CFP & ARL lookup.
 *
 * <h3>Meter Performance Monitoring</h3>
 * Meter Performance Monitoring provides visibility into the precise behavior of a meter.
 * Each performance monitor has a configured meter_id which it monitors.
 * A monitor operates on a single meter.
 * Once enabled the performance monitor has the following counters.
 * All counter count both packet (32b) and bytes (38b).
 * The number of performance monitors is defined when the block is used.
 *
 *  @{
 */

/**
 * @typedef cbx_meter_profile_t
 * Meter Profile Identifier
 */
typedef uint32_t cbx_meter_profile_t;

/**
 * @def CBX_METER_INVALID
 * Invalid Meter
 */
#define CBX_METER_INVALID   0xffffffff

/** @def CBX_METER_MAX
 * Maximum meters
 */
#define CBX_METER_MAX  128

/**
 * Metering Types
 */
typedef enum cbx_meter_type_e {
    cbxMeterBufferManager,     /**< BMU Meters */
    cbxMeterTrafficType,       /**< PP Meters */
    cbxMeterTrapGroup,         /**< PQM Meters */
    cbxMeterCFP,               /**< CFP Meters */
} cbx_meter_type_t;

/**
 * Meter Algorithms
 * Coupling:
 * - excessive tokens from the committed bucket are filled into the excessive bucket
 */
typedef enum cbx_meter_mode_e {
    cbxMeterModeSrTCM,           /**< RFC2697 */
    cbxMeterModeTrTCM,           /**< RFC2698 */
    cbxMeterModeMEF,             /**< RFC4115 */
    cbxMeterModeMEFBw,           /**< MEF Bandwidth Profile algorithm */
    cbxMeterModeCoupledMEFBw,    /**< MEF Bandwidth Profile with coupling */
} cbx_meter_mode_t;

/** @{
 * Meter Flags
 */
#define  CBX_METER_TYPE_NORMAL        0x00000001
#define  CBX_METER_COLOR_BLIND        0x00000002
#define  CBX_METER_DROP_RED           0x00000004
#define  CBX_METER_SHAPING            0x00000008
#define  CBX_METER_MODE_PACKETS       0x00000010
/** @} */

/**
 * Meter configuration paramters
 * - configuration information about a meter
 */
typedef struct cbx_meter_profile_params_t {
    uint32_t flags;             /**< Flags:  CBX_METER_XX */
    cbx_meter_type_t type;      /**< Policer type. */
    cbx_meter_mode_t mode;      /**< Policer mode. */
    uint32_t ckbits_sec;        /**< Committed rate in kbps or packets per second. */
    uint32_t ckbits_burst;      /**< Committed burst size in kbits or number of packets. */
    uint32_t pkbits_sec;        /**< Peak rate in kbps or packets per second. */
    uint32_t pkbits_burst;      /**< Peak burst size in kbits or number of packets. */
} cbx_meter_profile_params_t;

/**
 * Meter binding paramters
 * - meter bindings configuration
 */
typedef struct cbx_meter_params_t {
    cbx_portid_t        portid;
    cbx_traffic_type_t  traffic_type;
    cbx_tc_t            tc;
    int                 trap_group_index;
} cbx_meter_params_t;

/**
 * @typedef cbx_meter_pm_counter_t
 * Meter Performance Monitoring Counter
 */
typedef struct cbx_meter_pm_counter_t {
    uint64_t packet;
    uint64_t byte;
} cbx_meter_pm_counter_t;

/**
 * @typedef cbx_meter_pm_params_t
 * Meter Performance Monitoring
 */
typedef struct cbx_meter_pm_params_t {
    cbx_meter_pm_counter_t cbx_meter_pm_green;
    /**< Frames which arrive with DP=0 */
    cbx_meter_pm_counter_t cbx_meter_pm_yellow;
    /**< Frames which arrive with DP=1 */
    cbx_meter_pm_counter_t cbx_meter_pm_red;
    /**< Frames which arrive with DP=2 */
    cbx_meter_pm_counter_t cbx_meter_pm_reserved;
    /**< Frames which arrive with DP=3 */
    cbx_meter_pm_counter_t cbx_meter_pm_g2y;
    /**< Frame which arrive with DP=0
         and meter marks frame as DP=1 */
    cbx_meter_pm_counter_t cbx_meter_pm_y2r;
    /**< Frame which arrive with DP=1
         and meter marks frame as DP=2 */
    cbx_meter_pm_counter_t cbx_meter_pm_g2r;
    /**< Frame which arrive with DP=0
         and meter marks frame as DP=2 */
    cbx_meter_pm_counter_t cbx_meter_pm_y2g;
    /**< Frame which arrive with DP=1
         and meter marks frame as DP=0
         (only color blind mode)       */
    cbx_meter_pm_counter_t cbx_meter_pm_r2g;
    /**< Frame which arrive with DP=2
         and meter marks frame as DP=0
         (only color blind mode)       */
    cbx_meter_pm_counter_t cbx_meter_pm_r2y;
    /**< Frame which arrive with DP=2
         and meter marks frame as DP=1
         (only color blind mode)       */
    cbx_meter_pm_counter_t cbx_meter_pm_g2d;
    /**< Frame which arrive with DP=0
         and meter marks frame as DP=3 */
    cbx_meter_pm_counter_t cbx_meter_pm_y2d;
    /**< Frame which arrive with DP=1
         and meter marks frame as DP=3 */
    cbx_meter_pm_counter_t cbx_meter_pm_r2d;
    /**< Frame which arrive with DP=2
         and meter marks frame as DP=3 */
} cbx_meter_pm_params_t;


/**
 * Create a meter profile
 * This routine is used to create a meter profile.
 *
 * @param meter_profile_params  (IN)  Meter profile parameters.
 * @param profileid             (OUT) Handle of the meter profile created
 *                                 CBX_METER_INVALID: Meter could not be created
 *
 * @return return code
 * @retval CBX_E_NONE Success
 *         CBX_E_XXXX Failure
 */

int cbx_meter_profile_create ( cbx_meter_profile_params_t *meter_profile_params,
                               cbx_meter_profile_t        *profileid );

/**
 * Destroy a meter previously created by cbx_meter_create()
 *
 * @param profileid   (IN)  Handle of the meter profile to be destroyed
 *
 * @return return code
 * @retval CBX_E_NONE Success
 *         CBX_E_XXXX Failure
 *
 * @note This routine destroys a previously created meter profile.
 */

int cbx_meter_profile_destroy ( cbx_meter_profile_t profileid );

/**
 * Get a meter profile
 * This routine is used to get a meter profile.
 *
 * @param profileid     (IN)  Meter Profile Identifier
 * @param meter_params  (OUT) Meter parameters.
 *
 * @return return code
 * @retval CBX_E_NONE Success
 *         CBX_E_XXXX Failure
 */

int cbx_meter_profile_get ( cbx_meter_profile_t         profileid,
                            cbx_meter_profile_params_t *meter_params);

/**
 * Set a meter profile
 * This routine is used to set a meter profile.
 *
 * @param profileid     (IN)  Meter Profile Identifier
 * @param meter_params  (IN)  Meter parameters.
 *
 * @return return code
 * @retval CBX_E_NONE Success
 *         CBX_E_XXXX Failure
 */

int cbx_meter_profile_set ( cbx_meter_profile_t         profileid,
                            cbx_meter_profile_params_t *meter_params);

/**
 * Create a meter
 * This routine is used to create a meter.
 *
 * @param profileid     (IN)  Meter Profile Identifier
 * @param meter_params  (IN)  Meter binding parameters.
 * @param meterid       (OUT) Handle of the meter created
 *                            CBX_METER_INVALID: Meter could not be created
 *
 * @return return code
 * @retval CBX_E_NONE Success
 *         CBX_E_XXXX Failure
 */

int cbx_meter_create ( cbx_meter_profile_t profileid,
                       cbx_meter_params_t *meter_params,
                       cbx_meterid_t      *meterid );


/**
 * Destroy a meter previously created by cbx_meter_create()
 *
 * @param meterid    (IN)  Handle of the meter to be destroyed
 *
 * @return return code
 * @retval CBX_E_NONE Success
 *         CBX_E_XXXX Failure
 *
 * @note This routine destroys a previously created meter.
 */

int cbx_meter_destroy ( cbx_meterid_t meterid );


/**
 * Meter Performance monitoring enable
 *
 * @param meterid    (IN)  Handle of the meter to enable Performance Monitoring
 *
 * @return return code
 * @retval CBX_E_NONE Success
 *         CBX_E_XXXX Failure
 */

int cbx_meter_pm_enable ( cbx_meterid_t meterid );

/**
 * Meter Performance monitoring disable
 *
 * @param meterid    (IN)  Handle of the meter to disable Performance Monitoring
 *
 * @return return code
 * @retval CBX_E_NONE Success
 *         CBX_E_XXXX Failure
 */
int cbx_meter_pm_disable ( cbx_meterid_t meterid );

/**
 * Meter Performance monitoring get counters
 *
 * @param meterid      (IN)  Handle of the meter with Performance Monitoring
 * @param pm_params    (OUT) Performance Monitoring parameters
 *
 * @return return code
 * @retval CBX_E_NONE Success
 *         CBX_E_XXXX Failure
 */
int cbx_meter_pm_get ( cbx_meterid_t          meterid,
                       cbx_meter_pm_params_t *pm_params );


/**
 * @}
 */


#ifdef __cplusplus
}
#endif

#endif
