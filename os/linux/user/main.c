/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved$
 *
 * File:
 *    main.c
 */

#include <soc/drv.h>
#include "bslenable.h"
#include <fsal_int/types.h>
#include <fsal/cfp.h>
#include <fsal_int/init.h>
#ifdef INCLUDE_AVS
#include <soc/avs.h>
#include <soc/crmu_avs.h>
#include "sal_file.h"
#endif
#ifdef CONFIG_USE_UART_AS_CPU_INTF
#include <soc/robo2/bcm53158/uart_spi.h>
#else
#include <soc/robo2/bcm53158/spi.h>
#endif

extern void cint_thread(void);
extern void test_main(void);
extern uint8 dot1asInit(void);

#ifdef INCLUDE_AVS
/**
 * It was found that AVS (especially Dynamic AVS) takes a long time
 * to complete the convergence. So it was decided to:
 * (i) Execute the Dynamic AVS only once during the very first boot
 *     after the device is opened from the box.
 * (ii) Store the results of the above AVS in some persistent memory
 *     such as Flash / NVM etc. or a File.
 * (iii) In subsequent boots simply read the stored dac code, apply
 *       temperature compensation (if any) and apply the core voltage.
 *
 * AVS Status is a data structure that contains the necessary
 * information to be stored in the persistent memory.
 *
 * Currently it is decided to store this in a file.
 *
 */
#define AVS_STATUS_INITIALISED_PATTERN 0x55555555
avs_status_t avs_status_primary;
avs_status_t avs_status_secondary;
/* AVS File Pointer */
FILE *avsfp_primary;
FILE *avsfp_secondary;
/* Return Code for AVS / File Operation Routines*/
int ret_primary;
int ret_secondary;
#endif

#define DTCM_START	0x20000000
#define CHECK_RV(RET)	\
	if (RET)			\
		sal_printf("[%s:%u] Error: rv = %x\n", __FUNCTION__, __LINE__, RET);

int load_m7_code(char *m7_image_file)
{
	int rv;
	uint32 val, i, size;
	uint8 *val_p = (uint8 *)&val, *image;
	uint32 vtor = DTCM_START;
	FILE *fp;

	/* Open image file and read contents */
	fp = fopen(m7_image_file, "rb");
	if (fp == NULL) {
		sal_printf("Error opening file for reading: %s\n", m7_image_file);
		sal_printf("Skipping M7 boot\n");
		return -1;
	}

	/* Get file size */
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	/* Allocate buffer and read the file contents */
	image = malloc ((size + 3) & ~0x3);
	if (image == NULL) {
		sal_printf("Out of memory\n");
		sal_printf("Skipping M7 boot\n");
		fclose(fp);
		return -1;
	}

	if (size != fread(image, 1, size, fp)) {
		sal_printf("Unable to read file: %s\n", m7_image_file);
		sal_printf("Skipping M7 boot\n");
		fclose(fp);
		free(image);
		return -1;
	}

	/* Set M7 to wait after reset */
	val = 0x1;
	rv = REG_WRITE_M7SS_M7SC_CWCRr(0, val_p); CHECK_RV(rv);

	/* Set VTOR, VTVR */
	val = vtor;
	rv = REG_WRITE_M7SS_M7SC_VTORr(0, val_p); CHECK_RV(rv);
	val = 0x1;
	rv = REG_WRITE_M7SS_M7SC_VTVRr(0, val_p); CHECK_RV(rv);

	/* Release m7 from reset */
	val = 0x3FFFF;
	rv = REG_WRITE_CRU_CRU_IP_SYSTEM_RESETr(0, val_p); CHECK_RV(rv);

	/* Load the image to DTCM */
	for (i = 0; i < size/4; i++) {
		val = *((uint32 *)image + i);
		rv = soc_spi_write(0, DTCM_START + i*4, val_p, 4); CHECK_RV(rv);
	}

#ifdef TESTING
	/* Setup minimal Vector table for M7 to spin */
	val = 0x2000FE00;		/* Stack Start */
	rv = soc_spi_write(0, vtor, val_p, 4); CHECK_RV(rv);
	val = (vtor + 0x8) | 1;	/* Reset Handler vector (Thumb mode) */
	rv = soc_spi_write(0, vtor + 0x4, val_p, 4); CHECK_RV(rv);
	val = 0xe7fee7fe;		/* Reset handler code : op code for "b ." */
	rv = soc_spi_write(0, vtor + 0x8, val_p, 4); CHECK_RV(rv);
#endif

	/* Continue M7 execution */
	val = 0x0;
	rv = REG_WRITE_M7SS_M7SC_CWCRr(0, val_p); CHECK_RV(rv);

	fclose(fp);
	free(image);

	return 0;
}

int main(int argc, char **argv)
{
    int rv = 0;

#ifdef CONFIG_USE_UART_AS_CPU_INTF
    /* Init UART Spi Interface */
    rv = soc_uart_spi_init(0);
#else
    /* Init Spi Interface */
    rv = soc_spi_init(0);
#endif
    if (rv == 0) {
        /* Init Chip */
        rv = soc_robo2_essential_init(CBX_AVENGER_PRIMARY);
        if (rv == SOC_E_NONE) {
            if (SOC_IS_CASCADED(0)) {
                rv = soc_robo2_essential_init(CBX_AVENGER_SECONDARY);
            }
        }

#ifdef HYBRID_M7_MODE
        /* Load M7 code */
        if (argc == 2) {
            sal_printf("Loading M7 code\n");
            load_m7_code(argv[1]);
            (void) cbx_fsal_m7_hybrid_mode_init();
        }
#endif

#ifdef INCLUDE_AVS
    sal_printf("Reading Primary Avenger OTP...\n");
    sal_printf("Starting Dynamic AVS Using ROs on Primary Avenger...\n");

    /*
     * 1. Read the Initialised Pattern to know if this area is erased and
     *    written with Pattern. Generally after erase, the memory is filled
     *    with 0xFFFFFFFF.
     * 2. If the Pattern is "Initialized" to the chosen pattern (here
     *    0x55555555) then this is NOT the first boot, so read values from
     *    Flash and apply the dac code.
     * 3. Otherwise, i.e. If this is the first boot, then invoke the AVS.
     * 4. Store the last set core voltage and dac code.
     */
    avsfp_primary = sal_fopen("avs_status_file_primary", "r");
    if (avsfp_primary == NULL) {
        sal_printf("Error opening Primary AVS file for reading!! Create One.\n");
        avsfp_primary = sal_fopen("avs_status_file_primary", "w");
        if (avsfp_primary == NULL) {
            sal_printf("Error creating Primary AVS file!!\n");
        }
        else {
            avs_init(CBX_AVENGER_PRIMARY);
            if(avs_start(CBX_AVENGER_PRIMARY) < 0) {
                sal_printf("Error!! Dynamic AVS Failed on Primary."
                           " Reverting to Static AVS.\n");
                avenger_static_avs(CBX_AVENGER_PRIMARY);
            }

            avs_status_primary.core_voltage = AvsReadVoltage(CBX_AVENGER_PRIMARY, 5);
            avs_status_primary.dac_code     = currentDAC; /* Last DAC Code */

            /* ROBO-992: Incorporating suggestions from AVENGER-628
             * 20mV needs to be added to final AVS voltage to leave enough
             * margin for aging */
            /* 20mV * SCALING_FACTOR = 200 */
            avs_status_primary.core_voltage += 200;
            currentDAC = SetAvsVoltage(CBX_AVENGER_PRIMARY,
                                        avs_status_primary.core_voltage,
                                                avs_status_primary.dac_code);
            avs_status_primary.core_voltage = AvsReadVoltage(CBX_AVENGER_PRIMARY, 3);
            avs_status_primary.dac_code     = currentDAC; /* Last DAC Code */


            sal_printf("AVS3 Successful!! Core Voltage After AVS on"
                       " Primary Avenger: %d\n", avs_status_primary.core_voltage);
            avs_status_primary.initialized = AVS_STATUS_INITIALISED_PATTERN;
            ret_primary = sal_fwrite(&avs_status_primary.initialized, 4, 4, avsfp_primary);
            if(ret_primary != 4) {
                sal_printf("Error Writing Back AVS info into File!!\n");
            }
            /* Close the file */
            ret_primary = sal_fclose(avsfp_primary);
            if(ret_primary != 0) {
                sal_printf("Error Closing File!!\n");
            }
        }
    }
    else {
        /*
         * Not the First Boot Out of the Box. So Just apply saved Core Voltage.
         */
        ret_primary = sal_fread(&avs_status_primary.initialized, 4, 4, avsfp_primary);
        if(ret_primary != 4) {
            sal_printf("Error Reading AVS info from File!!\n");
        }
        else {
            sal_printf("Applying Saved Core Voltage:%d with saved DAC Code:"
                       " %d.\n", avs_status_primary.core_voltage, avs_status_primary.dac_code);
            SetAvsVoltage(CBX_AVENGER_PRIMARY, avs_status_primary.core_voltage,
                          avs_status_primary.dac_code);
            sal_printf("Core Voltage After applying saved AVS results on"
                       " Primary Avenger: %d\n",
                       AvsReadVoltage(CBX_AVENGER_PRIMARY, 5));
            /* Close the file */
            ret_primary = sal_fclose(avsfp_primary);
            if(ret_primary != 0) {
                sal_printf("Error Closing File!!\n");
            }
        }
    }

    /* Invoke AVS on Secondary Avenger if the Primary Avenger is in cascaded Mode */
    if(SOC_IS_CASCADED(CBX_AVENGER_PRIMARY)) {
        sal_printf("Reading Secondary Avenger OTP...\n");
        sal_printf("Starting Dynamic AVS Using ROs on Secondary Avenger...\n");

        /*
         * 1. Read the Initialised Pattern to know if this area is erased and
         *    written with Pattern. Generally after erase, the memory is filled
         *    with 0xFFFFFFFF.
         * 2. If the Pattern is "Initialized" to the chosen pattern (here
         *    0x55555555) then this is NOT the first boot, so read values from
         *    Flash and apply the dac code.
         * 3. Otherwise, i.e. If this is the first boot, then invoke the AVS.
         * 4. Store the last set core voltage and dac code.
         */
        avsfp_secondary = sal_fopen("avs_status_file_secondary", "r");
        if (avsfp_secondary == NULL) {
            sal_printf("Error opening Secondary AVS file for reading!! Create One.\n");
            avsfp_secondary = sal_fopen("avs_status_file_secondary", "w");
            if (avsfp_secondary == NULL) {
                sal_printf("Error creating Secondary AVS file!!\n");
            }
            else {
                avs_init(CBX_AVENGER_SECONDARY);
                if(avs_start(CBX_AVENGER_SECONDARY) < 0) {
                    sal_printf("Error!! Dynamic AVS Failed on Secondary."
                               " Reverting to Static AVS.\n");
                    avenger_static_avs(CBX_AVENGER_SECONDARY);
                }

                avs_status_secondary.core_voltage = AvsReadVoltage(CBX_AVENGER_SECONDARY, 5);
                avs_status_secondary.dac_code     = currentDAC; /* Last DAC Code */

                /* ROBO-992: Incorporating suggestions from AVENGER-628
                 * 20mV needs to be added to final AVS voltage to leave enough
                 * margin for aging */
                /* 20mV * SCALING_FACTOR = 200 */
                avs_status_secondary.core_voltage += 200;
                currentDAC = SetAvsVoltage(CBX_AVENGER_SECONDARY,
                                        avs_status_secondary.core_voltage,
                                              avs_status_secondary.dac_code);
                avs_status_secondary.core_voltage = AvsReadVoltage(CBX_AVENGER_SECONDARY, 3);
                avs_status_secondary.dac_code     = currentDAC; /* Last DAC Code */

                sal_printf("AVS3 Successful!! Core Voltage After AVS on"
                           " Secondary Avenger: %d\n", avs_status_secondary.core_voltage);
                avs_status_secondary.initialized = AVS_STATUS_INITIALISED_PATTERN;
                ret_secondary = sal_fwrite(&avs_status_secondary.initialized, 4, 4, avsfp_secondary);
                if(ret_secondary != 4) {
                    sal_printf("Error Writing Back AVS info into File!!\n");
                }
                /* Close the file */
                ret_secondary = sal_fclose(avsfp_secondary);
                if(ret_secondary != 0) {
                    sal_printf("Error Closing File!!\n");
                }
            }
        }
        else {
            /*
             * Not the First Boot Out of the Box. So Just apply saved Core Voltage.
             */
            ret_secondary = sal_fread(&avs_status_secondary.initialized, 4, 4, avsfp_secondary);
            if(ret_secondary != 4) {
                sal_printf("Error Reading AVS info from File!!\n");
            }
            else {
                sal_printf("Applying Saved Core Voltage:%d with saved DAC Code:"
                           " %d.\n", avs_status_secondary.core_voltage, avs_status_secondary.dac_code);
                SetAvsVoltage(CBX_AVENGER_SECONDARY, avs_status_secondary.core_voltage,
                              avs_status_secondary.dac_code);
                sal_printf("Core Voltage After applying saved AVS results on"
                           " Secondary Avenger: %d\n",
                           AvsReadVoltage(CBX_AVENGER_SECONDARY, 5));
                /* Close the file */
                ret_secondary = sal_fclose(avsfp_secondary);
                if(ret_secondary != 0) {
                    sal_printf("Error Closing File!!\n");
                }
            }
        }
    }
#endif

        if (rv == SOC_E_NONE) {
            bslenable_init();
            cbx_fsal_init();
#ifdef CONFIG_DOT1AS
            if (dot1asInit() != 0) {
                sal_printf("DOT1AS initialization failed\n");
            } 
#endif
#ifdef INCLUDE_CINT
            cint_thread();
#endif
#ifdef INCLUDE_TEST
            test_main();
#endif
        }
#ifdef CONFIG_USE_UART_AS_CPU_INTF
        soc_uart_spi_terminate(0);
#else
        soc_spi_terminate(0);
#endif
    }
    return rv;
}

