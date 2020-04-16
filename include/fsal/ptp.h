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

#ifndef CBX_API_PTP_H_
#define CBX_API_PTP_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @defgroup cbx_ptp CBX PTP
 * Forwarding Services Abstraction Layer (FSAL) PTP operations.
 *
 *
 * Precision Time Protocol
 * IEEE-1588
 *
 * Avenger Implementation
 *
 * UM+ mode PTP feature provides:
 * 1588v2/802.1as PTP End-to-End Transparent Clock
 *    (update of correction field for residence time)
 *
 * MM mode PTP feature provides full support
 *
 *
 * Packet format:
 *  IEEE 1588 PTP frames including event messages
 *  - directly over Ethernet (using Ethertype = 0x88F7) and
 *  - over UDP (using destination port = 319 or 320)
 *
 * Note that PTP frames require additional processing when IEEE 1588 1-step
 * timing is enabled. The hardware adjusts the correction field
 *
 * PTP is identified during ingress processing.
 * When supporting one-step time stamping, the PTP flow is assigned
 * its own DLI which uses the following directives.
 *
 * Two directives are used to adjust the correction field for 1588 PTP frames.
 * -  Update PTP correction field
 *   o  This directive uses the ingress timestamp TLV
 *     The relative time is carried in the TLV and added to the correction field
 *   o  The 48b ingress timestamp is subtracted from the correction field
 *   The directive provides the option to locate the correction field
 *   and UDP checksum based on the parser results or to take the location from the directive
 *   o  If UDP the UDP checksum is updated
 *
 * -  Prepend egress timestamp TLV
 *   o  This directive creates an egress TLV which is used to
 *      instruct the egress MAC/PHY on supporting the time stamp operation;
 *  both 1-step and 2-step are supported
 *   o  The directive includes control fields passed directly to the MAC:
 *     1-step or 2-step mode : {do_tstamp, osts}
 *     Whether the UDP checksum : do_chksum
 *     Whether sign extension is required : tsi_sign_ext
 *   o  The egress editor calculates the offsets for the location
 *       of the correction field and the UDP checksum OR
 *       the offsets are provided in the directive
 *       These are also passed to the MAC
 *
 * When 1588 is enabled in Avenger, all frames are time stamped on ingress.
 * The 48b reference time is carried with the frame to the egress.
 * The ingress timestamp TLV is dropped in the egress in all cases.
 *
 *
 * Avenger Programming:
 *  Egress PGT
 *  ptp_mode[1:0]
 *   0b00 = PTP edits disabled
 *   0b01 = PTP enabled, use PTP control set 0
 *   0b10 = PTP enabled, use PTP control set 1
 *  imp_mode[1:0]
 *   0b00 = all IMP headers are non-timestamp format
 *   0b01 = IMP headers are non-timestamp format
 *          except PTP frames; these use the timestamp format
 *   0b10 = all headers use timestamp format
 *
 *  @{
 */

/**
 * @typedef cbx_ptp_t
 * PTP Identifier
 */

/**
 * @def CBX_PTP_INVALID
 * Invalid PTP
 */
#define CBX_PTP_INVALID   0xffffffff

/**
 * PTP port admin state
 */
#define CBX_PTP_PORT_ENABLE   1
#define CBX_PTP_PORT_DISABLE  0

/**
 *  PTP parameter structure
 */
typedef struct cbx_ptp_params_t {
    uint32_t         flags;            /**< Flags:  CBX_PTP_XX */
} cbx_ptp_params_t;

/**
 *  PTP callback information structure
 */
typedef struct cbx_ptp_info_t {
    uint32_t         flags;            /**< Flags:  CBX_PTP_CALLBACK_XX */
    const char      *name;             /**< Registered callback name string */
} cbx_ptp_info_t;

/**
 *  PTP clock modes
 */
typedef enum cbx_ptp_clock_mode_e {
    cbxPtpClkOneStep,
    cbxPtpClkTwoStep,
} cbx_ptp_clk_mode_t;

/* Hardware per-port timestamp FIFO */
#define CBX_TS_FIFO_DEPTH 4

typedef struct {
  uint32_t seq_id;     /* Sequence ID associated with this timestamp */
  uint32_t nanoseconds;/* Nanoseconds value of the timestamp */
  uint8_t  is_ts_read; /* Flag indicates this time stamp is already read */
} CBX_TS_INFO_t;

typedef struct {
  uint8_t cur_ts_idx;    /* Current timestmap index */
  CBX_TS_INFO_t ts_info[CBX_TS_FIFO_DEPTH];  /* Timestmap FIFO */
} CBX_TS_FIFO_t;

/**
 * PTP Callback prototype
 *
 * Function prototype definition for user supplied PTP callback.
 * Callback is performed in the context of the Pktio RX thread.
 * Used to handle PTP events and messages
 *
 * @param portid        (IN)  Port Identifier.
 * @param value         (IN)  PTP value
 */
typedef void ( *cbx_ptp_callback_t )
( cbx_portid_t       portid,
  uint32_t           value );


/**
 * Initializes the PTP subsystem
 * - setup of Precision Time Protocol module
 *
 * @param ptp_params   (IN)  PTP configuration parameters
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_ptp_init ( void );


/**
 * Detach PTP previously initialized by cbx_ptp_init()
 * - shut down the PTP subsystem
 *
 * @param ptp_params   (IN)  PTP parameters.
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_ptp_detach ( cbx_ptp_params_t *ptp_params );


/**
 * PTP register
 * This routine is used to register a PTP callback routine.
 * If multiple handlers are registered, they are invoked on
 * in the same order as they were registered.
 *
 * @param ptp_info (IN)  PTP callback information
 * @param cb       (IN)  Callback function pointer
 *                       see: cbx_ptp_callback_t
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_ptp_register ( cbx_ptp_info_t      ptp_info,
                       cbx_ptp_callback_t  cb );

/**
 * PTP unregister
 * This routine is used to unregister a PTP callback routine.
 *
 * @param cb           (IN)  PTP callback routine to unregister
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */

int cbx_ptp_unregister ( cbx_ptp_callback_t  cb );

/**
 * cbx_ptp_clock_mode_set
 * Sets the PTP clock mode to either one step or two step.
 * Default mode is two step. Change of mode has to be done
 * done before enabling the PTP on ports.
 *
 * @param clk_mode   (IN)  PTP clock mode
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */
int cbx_ptp_clock_mode_set(cbx_ptp_clk_mode_t clk_mode);

/**
 * cbx_ptp_clock_mode_get
 * Gets the PTP clock mode.
 *
 * @param clk_mode   (IN/OUT)  PTP clock mode
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */
int cbx_ptp_clock_mode_get(cbx_ptp_clk_mode_t *clk_mode);;

/**
 * cbx_ptp_port_admin_set
 * Enables/Disables the port PTP configuration. This will configure the
 * port to do PTP correction field updates and timestamping of packets
 *
 * @param portid  (IN) Port Identifier.
 * @param admin   (IN) Admin value(CBX_PTP_PORT_ENABLE/CBX_PTP_PORT_DISABLE)
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */
int cbx_ptp_port_admin_set(cbx_portid_t portid, uint8_t admin);

/**
 * cbx_ptp_port_admin_get
 * Gets the admin state of port PTP configurtion
 *
 * @param portid  (IN)  Port Identifier.
 * @param admin   (IN/OUT) Admin value(CBX_PTP_PORT_ENABLE/CBX_PTP_PORT_DISABLE)
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */
int cbx_ptp_port_admin_get(cbx_portid_t portid, uint8_t *admin);

/**
 * cbx_ptp_timestamp_get
 * Gets the timestamp and sequence id of egress PTP packet on the port
 *
 * @param portid     (IN)  Port Identifier.
 * @param timestamp  (IN/OUT) Timestamp  value
 * @param seq_id     (IN/OUT) Sequence id of the packet
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */
int cbx_ptp_timestamp_get(cbx_portid_t portid, uint32_t *timestamp,
                          uint32_t *seq_id);


/**
 * cbx_ptp_seqid_timestamp_get
 * Gets the timestamp of egress PTP packet for the matching sequence id.
 *
 * @param portid     (IN)  Port Identifier.
 * @param seq_id     (IN) Sequence id of the packet
 * @param timestamp  (IN/OUT) Timestamp  value
 *
 * @return return code
 * @retval CBX_E_NONE Success
 * @retval CBX_E_XXXX Failure
 */
int cbx_ptp_seqid_timestamp_get(cbx_portid_t portid, uint32_t seq_id,
                                uint32_t *timestamp);

/**
 * @ }
 */


#ifdef __cplusplus
}
#endif

#endif
