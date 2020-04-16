/*
 * $ID:$
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved$
 * File:
 *    avs.h
 * Description
 *    avs.h
 */

#ifndef __AVS_H__
#define __AVS_H__
typedef enum {
    PVT_TEMPERATURE = 0, /* Bit 0 - Temperature measurement */
    PVT_0P85V_0     = 1, /* Bit 1 - Voltage 0p85V<0> measurement */
    PVT_0P85V_1     = 2, /* Bit 2 - Voltage 0p85V<1> measurement */
    PVT_1V_0        = 3, /* Bit 3 - Voltage 1V<0> measurement */
    PVT_1V_1        = 4, /* Bit 4 - Voltage 1V<1> measurement */
    PVT_1p8V        = 5, /* Bit 5 - Voltage 1p8V measurement */
    PVT_3p3V        = 6, /* Bit 6 - Voltage 3p3V measurement */
    PVT_TESTMODE    = 7 /* Bit 7 - Testmode measurement */
} pvt_t;

/**
 * It was found that AVS (especially Dynamic AVS) takes a long time
 * to complete the convergence. So it was decided to:
 * (i) Execute the Dynamic AVS only once during the very first boot
 *     after the device is opened from the box.
 * (ii) Store the results of the above AVS in some persistent memory
 *     such as Flash / NVM etc.
 * (iii) In subsequent boots simply read the stored dac code, apply
 *       temperature compensation (if any) and apply the core voltage.
 *
 * The following avs_status is a data structure that contains the necessary
 * information to be stored in the persistent memory.
 *
 */
typedef struct {
    uint32_t initialized; /* Pattern: 0x55555555 --> Initialized */
    uint32_t core_voltage;/* Core Voltage resulting from Last AVS */
    uint32_t dac_code;    /* Last DAC Code that was used to achieve the Core Voltage */
    uint32_t core_temp;   /* Core Temperature from Last measurement */
} avs_status_t;

uint32 nominalDAC;

/* The PMB remotes are on a separate bus.  This means it takes longer than normal to access any of the registers.
** This applies mostly for read operations (we have to wait for the data to be ready).
** This delay needs to be after every access to a PMB register to make sure we've completed the previous before starting a new transaction.
*/
#define REMOTE_DELAY (2*ONE_MICRO_SEC) /* delay time after reading/writing PMB Remote oscillator register */

/* The sequencer needs time to do all of the measuements before any of the data is valid after being reset or changes to the DAC.
** This delay allows for a full cycle of reading all the measuments.
** Value is in milliseconds.
*/
#define DAC_WRITE_DELAY 32 /* time to wait before reading any sequencer measurement after changing DAC */


#ifndef CONFIG_EXTERNAL_HOST
/* all based on 250 MHz clk */
#define ONE_MICRO_SEC       (2*250)
#else /* CONFIG_EXTERNAL_HOST  */
#define ONE_MICRO_SEC       (1) /* Input to sal_usleep */
#endif
#define ONE_MILLI_SEC       (ONE_MICRO_SEC*1000)
#define ONE_SECOND          (ONE_MILLI_SEC*1000)

#define AVERAGE_LOOPS 10 /* the more loops the longer the process, but better the average */

#define DISABLE_SLEEP_TIME 200 /* time to wait after choosing only the item we want */

//#define NOMINAL_VOLTAGE (1.025) /* was 1.0V */
#define NOMINAL_VOLTAGE (1.030) /* was 1.0V */
#define SWING_VALUE (30)        /* how close (+ or -) we want to get to exactly 1V, in 0.1mV units */
#define MAX_ITERATIONS_1V 100   /* don't let any loops run forever */

#define SCALING_FACTOR 10000 /* make this larger to get more precision in the numbers (but be careful of overflow) */
#define S1 SCALING_FACTOR    /* WARNING: DO NOT CHANGE THIS -- it MUST match the definition used to build the library */
#define S2 SCALING_FACTOR
#define INT(x) ((int32)((x)*S1)) /* this creates an integer from a float with the precision defined above */


/*
   Indicate PVT monitor sel 011(1V_0 Monitoring) measurements data,
   validity of data and measurement done status
*/
#define BCHP_AVS_RO_REGISTERS_0_PVT_1V_0_MNTR_STATUS        0x4020120c
 /* 
   Indicate PVT monitor sel 100(1V_1 Monitoring) measurements data,
   validity of data and measurement done status 
*/
#define BCHP_AVS_RO_REGISTERS_0_PVT_1V_1_MNTR_STATUS        0x40201210
 /*
   Indicate PVT monitor sel 101(1p8V Monitoring) measurements data,
   validity of data and measurement done status
 */
#define BCHP_AVS_RO_REGISTERS_0_PVT_1p8V_MNTR_STATUS        0x40201214

#define BCHP_AVS_RO_REGISTERS_0_PVT_TESTMODE_MNTR_STATUS_done_MASK       0x00010000
#define BCHP_AVS_RO_REGISTERS_0_PVT_TESTMODE_MNTR_STATUS_valid_data_MASK 0x00000400
#define BCHP_AVS_RO_REGISTERS_0_PVT_TESTMODE_MNTR_STATUS_data_MASK       0x000003ff

/* Indicate which PVT Monitor measurements should
   be masked(skipped) in the measurement sequence */
#define BCHP_AVS_HW_MNTR_SEQUENCER_MASK_PVT_MNTR       0x4020103c 
/* Indicate which central ring oscillators should
   be masked(skipped) in the measurement sequence */
#define BCHP_AVS_HW_MNTR_SEQUENCER_MASK_CEN_ROSC_0     0x40201040 
/* Indicate which central ring oscillators should
   be masked(skipped) in the measurement sequence */
#define BCHP_AVS_HW_MNTR_SEQUENCER_MASK_CEN_ROSC_1     0x40201044 
/* Indicate which remote ring oscillators should
   be masked(skipped) in the measurement sequence */
#define BCHP_AVS_HW_MNTR_SEQUENCER_MASK_RMT_ROSC_0     0x40201048

/* Control bits for PVT monitor */
#define BCHP_AVS_PVT_MNTR_CONFIG_PVT_MNTR_CTRL               0x40201100 
/* SW must set this bit to 1 to modify DAC_CODE,
   MIN_DAC_CODE and MAX_DAC_CODE */
#define BCHP_AVS_PVT_MNTR_CONFIG_DAC_CODE_PROGRAMMING_ENABLE 0x40201110
/* Represents the input code of the DAC */
#define BCHP_AVS_PVT_MNTR_CONFIG_DAC_CODE                    0x40201114
#define BCHP_AVS_PVT_MNTR_CONFIG_DAC_CODE_dac_code_MASK      0x000003ff

/* limits on DAC code */
#define BCHP_AVS_DAC_CODE_min 150
#define BCHP_AVS_DAC_CODE_max 800

extern uint32 avenger_static_avs(int unit);
extern uint32 AvsReadVoltage(int unit, uint32 count);
extern uint32 SetAvsVoltage(int unit, uint32 nominal_voltage, uint32 dac_code);
#endif /* __AVS_H__ */
