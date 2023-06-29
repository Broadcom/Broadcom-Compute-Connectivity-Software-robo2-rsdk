/* * $ID: $
 * * $Copyright: (c) 2017 Broadcom Corp.
 * * All Rights Reserved$
 * *
 * * File:
 * *     avs.c
 * *
 * */

#include "shared/system.h"
#include <soc/robo2/common/memregs.h>
#include <soc/drv.h>
#include <osal/sal_time.h>
#include "soc/avs.h"

#define PERFORM_DAC_CHARGE
#define NEXT_GENERATION
/* dwl: with DISABLE_ALL_BUT defined, AvsReadVoltage() may return very wide
 * range of voltages, sometimes over 300mV.
 * dwl: with DISABLE_ALL_BUT not defined, run time is increased
 */
//#define DISABLE_ALL_BUT

/* avs_nominal_voltage = nominal_voltage * 10000 (SCALING_FACTOR) */
/* It will be read from OTP */
uint32 avs_nominal_voltage = 0;
/* avs_swing_value = swing_value * 10000 (SCALING_FACTOR) */
/* It will be read from OTP */
uint32 avs_swing_value     = 0;

   /* ADC Measures external input on pad_ADC */
#define PVT_MON_CTRL_TEST 0x200

/* TODO: Check with Ali, whether we should
 * define NEXT_GENERATION, DISABLE_ALL_BUT */

#ifndef CONFIG_EXTERNAL_HOST
#define AvsSleep(ticks)  \
        {                                                      \
            uint32 tcount = 0;                                 \
            /* one loop will take on average 8 instructions.   \
               Note it is 8bit processor */                    \
            tcount = (ticks) >> 3 ;                            \
                                                               \
            do {                                               \
                tcount = tcount - 1;                           \
            }while (tcount);                                   \
        }
#else /* External Host */
#define AvsSleep(ticks)  \
        {                                                      \
            sal_usleep(ticks);                                 \
        }
#endif

/* Get the value from PVT for the specified value from following hardware:
 * v_1p1_0 =
 * target.AVS_CPU_TOP.AVS_CPU_SYS.AVS_MONITOR1.AVS_RO_REGISTERS_0.PVT_1V_0_MNTR_STATUS.data
 * v_1p1_1 =
 * target.AVS_CPU_TOP.AVS_CPU_SYS.AVS_MONITOR1.AVS_RO_REGISTERS_0.PVT_1V_1_MNTR_STATUS.data
 * v_1p8 =
 * target.AVS_CPU_TOP.AVS_CPU_SYS.AVS_MONITOR1.AVS_RO_REGISTERS_0.PVT_1P8V_MNTR_STATUS.data
 */
uint32 AvsReadPVTregister(int unit, pvt_t which)
{
    uint32 j;
    uint32 pvt_code=0;
/* if we don't see done/valid then re-read the results this many times */
#define MAX_REREADS 10

    for (j = 0; j < MAX_REREADS; j++) {
        switch (which) {
            case PVT_1V_0:
            REG_READ_CRU_AVS_AVS_RO_REGISTERS_0_PVT_1V_0_MNTR_STATUSr(unit,
                                                                &pvt_code);
            break;
            case PVT_1V_1:
            REG_READ_CRU_AVS_AVS_RO_REGISTERS_0_PVT_1V_1_MNTR_STATUSr(unit,
                                                                &pvt_code);
            break;
            case PVT_1p8V :
            REG_READ_CRU_AVS_AVS_RO_REGISTERS_0_PVT_1P8V_MNTR_STATUSr(unit,
                                                                &pvt_code);
            break;
            default: break;
        }
        if (!((pvt_code &
           BCHP_AVS_RO_REGISTERS_0_PVT_TESTMODE_MNTR_STATUS_done_MASK) &&
           (pvt_code &
           BCHP_AVS_RO_REGISTERS_0_PVT_TESTMODE_MNTR_STATUS_valid_data_MASK))) {
            sal_printf ("   warning.......avs.c:AvsReadPVTregister which=%d"
                        " j=%d; data=0x%x, value=0x%x (%d)\n",
                        which, j, pvt_code,
            pvt_code & BCHP_AVS_RO_REGISTERS_0_PVT_TESTMODE_MNTR_STATUS_data_MASK,
            pvt_code & BCHP_AVS_RO_REGISTERS_0_PVT_TESTMODE_MNTR_STATUS_data_MASK);
        }
        /* Wait for the done and valids to be set */
        if ((pvt_code
            & BCHP_AVS_RO_REGISTERS_0_PVT_TESTMODE_MNTR_STATUS_valid_data_MASK)
            && (pvt_code
            & BCHP_AVS_RO_REGISTERS_0_PVT_TESTMODE_MNTR_STATUS_done_MASK))
            break;
    }

    /* Choose one of the three: valid, done, or both */
    /*
    if (!(pvt_code & BCHP_AVS_RO_REGISTERS_0_PVT_TESTMODE_MNTR_STATUS_valid_data_MASK)
       || !(pvt_code & BCHP_AVS_RO_REGISTERS_0_PVT_TESTMODE_MNTR_STATUS_done_MASK))
        return 0;
    */
    if (!(pvt_code
          & BCHP_AVS_RO_REGISTERS_0_PVT_TESTMODE_MNTR_STATUS_valid_data_MASK))
        return 0;
    //if (!(pvt_code & BCHP_AVS_RO_REGISTERS_0_PVT_TESTMODE_MNTR_STATUS_done_MASK))
    //    return 0;

    /* these all have the same data mask so use any one of them */
    pvt_code &= BCHP_AVS_RO_REGISTERS_0_PVT_TESTMODE_MNTR_STATUS_data_MASK;

    return pvt_code;
}

uint32 read_pvtmon_value(int unit, pvt_t which)
{
    uint32 i;
    uint32 pvt_code, count, sum;

#ifdef DISABLE_ALL_BUT
    uint32 mask = 0xFFFFFFFF;
        /* Disable all but the particular measurement we're interested in */
    count = ~(1<<which) & 0x7F;
    //sal_printf("----Inside read_pvtmon_value--1---which = 0x%lx\n", which);
    REG_WRITE_CRU_AVS_AVS_HW_MNTR_SEQUENCER_MASK_PVT_MNTRr(unit, &count);
    REG_WRITE_CRU_AVS_AVS_HW_MNTR_SEQUENCER_MASK_CEN_ROSC_0r(unit, &mask);
    REG_WRITE_CRU_AVS_AVS_HW_MNTR_SEQUENCER_MASK_CEN_ROSC_1r(unit, &mask);
    AvsSleep(DISABLE_SLEEP_TIME*ONE_MICRO_SEC);
#endif

    sum = count = 0;
    for (i = 0; i < AVERAGE_LOOPS; i++) {
        pvt_code = AvsReadPVTregister(unit, which);
        sum += pvt_code;
        count++;

#ifdef DISABLE_ALL_BUT
        /* allow PVT sequencer to read a new measurement */
        AvsSleep(DISABLE_SLEEP_TIME*ONE_MICRO_SEC);
#else
        /* allow PVT sequencer to loop back to this measurement */
        AvsSleep(10*ONE_MILLI_SEC);
        /* allow PVT sequencer to loop back to this measurement */
        AvsSleep(5*ONE_MILLI_SEC);
#endif
    }
    pvt_code = sum / count;

#ifdef DISABLE_ALL_BUT
    /* Re-enable all of the other measurements */
    mask = 0;
    REG_WRITE_CRU_AVS_AVS_HW_MNTR_SEQUENCER_MASK_PVT_MNTRr(unit, &mask);
    REG_WRITE_CRU_AVS_AVS_HW_MNTR_SEQUENCER_MASK_CEN_ROSC_0r(unit, &mask);
    REG_WRITE_CRU_AVS_AVS_HW_MNTR_SEQUENCER_MASK_CEN_ROSC_1r(unit, &mask);
#endif

    return pvt_code;
}

uint32 AvsGetPvtValue(int unit, pvt_t which)
{
    return read_pvtmon_value(unit, which);
}

/* A conversion is needed on the results from the register -- do the conversion */
int AvsGetPvtResult(int unit, pvt_t which)
{
    uint32 pvt_code;
    int32 result=0;

    pvt_code = AvsGetPvtValue(unit, which);

#define FORMULA_MULTIPLIER (8800U) /* from PVTMON specification v5.0 */

    /* The above supplies the value from the register. This needs to be converted
     * into a value. The formula includes floating point operations but we only
     * want to use integer arithmetic so I reduced the formula to lose as little
     * precision as possible. I left in my conversion so you can check my work.
     */
    switch (which) {
        case PVT_1V_0:
        case PVT_1V_1:
        //voltage_1p1_0 = ((v_1p1_0/1024)*877.9/.7) /* original formula from PVTMON specification */
        //voltage_1p1_0 = ((v_1p1_0/1024)*877.9/.7)/1000  /* original formula from Ali */
        //voltage_1p1_0 = ((v_1p1_0/1024)*877.9)/(1000*.7)
        //voltage_1p1_0 = ((v_1p1_0*877.9)/1024)/(1000*.7)
        //voltage_1p1_0 = (v_1p1_0*877.9)/(1024*1000*.7)
        //voltage_1p1_0 = 10*(v_1p1_0*877.9)/10*(1024*1000*.7) /* multiply top&bottom by 10 to get rid of fractions */
        //voltage_1p1_0 = (v_1p1_0*8779)/(1024*1000*7)
        // I need this to be in 1000th (millivolts) so: voltage_1p1_0 = 1000*((v_1p1_0*8779)/(1024*1000*7))
        //voltage_1p1_0 = (v_1p1_0*8779)/(1024*7)
        //voltage_1p1_0 = (v_1p1_0*8779)/7168
        // I REALLY need this to be in 10,000th (and I already did 1000th) so multiply by additional 10

            /* result = 10*(pvt_code * FORMULA_MULTIPLIER) / 7168U; */
            result = 10*(pvt_code * FORMULA_MULTIPLIER) ;
            result = result / 7168U;
            break;
        case PVT_1p8V :
        //v_1p8=v_1p8/1024*877.9/0.4 /* original formula from PVTMON specification */
        //v_1p8=v_1p8/1024*877.9/0.4/1000
        //v_1p8=(v_1p8*877.9)/1024/0.4/1000
        //v_1p8=(v_1p8*877.9)/(1024*0.4)/1000
        //v_1p8=(v_1p8*877.9)/(1024*0.4*1000)
        //v_1p8=(v_1p8*877.9)/(1024*400)
        //v_1p8=(v_1p8*877.9)/409600
        //v_1p8=(v_1p8*877.9)*10/(409600*10) /* multiply top and bottom by 10 to get rid of fractions */
        //v_1p8=(v_1p8*8779)/4096000
        // I need this to be in 1000th (millivolts) so: v_1p8=1000*(v_1p8*8779)/4096000
        //v_1p8=(v_1p8*8779)/4096
        // I REALLY need this to be in 10,000th (and I already did 1000th) so multiply by additional 10

            /* result = 10*(pvt_code * FORMULA_MULTIPLIER)/4096U; */
            result = 10*(pvt_code * FORMULA_MULTIPLIER);
            result = result/4096U;
            break;
        default: break;
    }

    return result;
}

/* Return the current voltage value generated by the PVTMON
 * Read the voltage "count" times and return average
 */
uint32 AvsReadVoltage(int unit, uint32 count)
{
    uint32_t voltage_1p1_0;
    uint32 average, j;
    uint32 sum = 0;
    uint32 max = 0;
    uint32 min = 999999999;

    AvsSleep(15*ONE_MILLI_SEC);
    for (j=0; j < count; j++) {
      voltage_1p1_0 = AvsGetPvtResult(unit, PVT_1V_0);
      sum += voltage_1p1_0;
      if (voltage_1p1_0 < min) min = voltage_1p1_0;
      if (voltage_1p1_0 > max) max = voltage_1p1_0;
      //sal_printf (".... AvsReadVoltage getting v0=%d(0.1mV) \n", voltage_1p1_0);
    }
    average = sum/count;
    //sal_printf ("........avs.c:AvsReadVoltage return %d(0.1mV), min=%d, max=%d,"
    //            " max-min=%d \n", average, min, max, max-min);
    return average;
}

uint32 GetBoardDACvalue(int unit)
{
    uint32_t board_dac_code, reg;
    uint32_t VFB_pin_voltage;
    uint32_t mntr_ctrl;
    uint32_t control = PVT_MON_CTRL_TEST;

    /* Need to restore AVS_PVT_MNTR_CONFIG_PVT_MNTR_CTRL, otherwise
     * AvsReadVoltage() will return in-accurate value.
     */
    //sal_printf("1.In GetBoardDACValue\n");
    REG_READ_CRU_AVS_AVS_PVT_MNTR_CONFIG_PVT_MNTR_CTRLr(unit, &mntr_ctrl);
    //sal_printf("2.In GetBoardDACValue\n");
    REG_WRITE_CRU_AVS_AVS_PVT_MNTR_CONFIG_PVT_MNTR_CTRLr(unit, &control);
    //sal_printf("3.GetBoardDACvalue: After PVT MNTR CTRL write\n");

    /* let the PVT monitor cycle through the different samples */
    AvsSleep(30*ONE_MILLI_SEC);

    //sal_printf("GetBoardDACvalue: After AvsSleep\n");

    /* using 1p8V PVTMON's input to collect any PVTMON measurements
    reg = AvsGetPvtValue(PVT_1p8V); */
    /* use PVT_1V_0 so that this is the only voltage monitor to enable */
    reg = AvsGetPvtValue(unit, PVT_1V_0);

    //sal_printf("GetBoardDACvalue: PVT_1V_0 : 0x%lx\n", reg);

#ifdef NEXT_GENERATION
    /* matches ana_pvtmonts28hpm_all_fc_x_e module spec */
    VFB_pin_voltage = reg*880U/1024U;
    sal_printf("GetBoardDACvalue: PVT_1V_0 : 0x%lx\n", VFB_pin_voltage);
    board_dac_code = (1425U - VFB_pin_voltage)*10000U/11938U;
#else
    VFB_pin_voltage = reg*8779U/10240U;
    sal_printf("GetBoardDACvalue: PVT_1p8V : 0x%lx\n", VFB_pin_voltage);
    board_dac_code = (1421U - VFB_pin_voltage)*1000U/1191U;
#endif
    //sal_printf("4.In GetBoardDACValue\n");
    /* restore pvt_mon_ctrl with the saved value */
    REG_WRITE_CRU_AVS_AVS_PVT_MNTR_CONFIG_PVT_MNTR_CTRLr(unit, &mntr_ctrl);
    //sal_printf("5.In GetBoardDACValue\n");
    return board_dac_code;
}


/* Always give time for sequencer to sequence through all the different available data */
/* This also provides the regulator time to settle after making a DAC change */
static void AllowRegulatorToSettle(void)
{
    /* Do we need the extra delay? */
    int extra_delay = 0;
    AvsSleep((DAC_WRITE_DELAY+extra_delay)*ONE_MILLI_SEC);
#ifndef USE_SOFTWARE_TAKEOVER
#ifndef DISABLE_ALL_BUT
    AvsSleep(30*ONE_MILLI_SEC); /* 32 should be sufficient, but this pads it */
#endif
#endif
}


/* Generic function to update the PVTMON Control word:
 * -- only writes if it needs to write it.
 *
 * AVS_PVT_MNTR_CONFIG_PVT_MNTR_CTRL register fields
 * [18]    -> i_ctrl[28]    = DAC_set
 * [17]    -> i_ctrl[27]    = DAC_reset
 * [16:15] -> i_ctrl[19:18] = reserved
 * [14]    -> i_ctrl[17]    = Burnin_en
 * [13]    -> i_ctrl[16]    = Con_pad
 * [12]    -> i_ctrl[15]    = DAC_en
 * [11:10] -> i_ctrl[14:13] = ADC_insel
 * [9:7]   -> i_ctrl[12:10] = Mode
 * [6:4]   -> i_ctrl[9:7]   = RMON_sel
 * [3:0]   -> i_ctrl[6:3]   = Vtest_sel
 */
static void SetPVTctrl(int unit, uint32 value)
{
    uint32 reg_value;
    REG_READ_CRU_AVS_AVS_PVT_MNTR_CONFIG_PVT_MNTR_CTRLr(unit, &reg_value);
    if (value != reg_value) {
        sal_printf ("........avs.c:SetPVTctrl setting"
                    " AVS_PVT_MNTR_CONFIG_PVT_MNTR_CTRL from 0x%x to 0x%x \n",
                    reg_value, value);
        REG_WRITE_CRU_AVS_AVS_PVT_MNTR_CONFIG_PVT_MNTR_CTRLr(unit, &value);
    }
}


/* This is used to enable the DAC value already written. */
static void AvsEnableDAC(int unit)
{
    uint32 data = 1;
    //sal_printf(".....Enabling DAC......\n");
    REG_WRITE_CRU_AVS_AVS_PVT_MNTR_CONFIG_DAC_CODE_PROGRAMMING_ENABLEr(unit,
                                                                     &data);
    //sal_printf(".....Finished Enabling DAC......\n");
    AvsSleep(ONE_MICRO_SEC); /* delay a bit before disabling again */
    //sal_printf(".....Disabling DAC......\n");
    data = 0;
    REG_WRITE_CRU_AVS_AVS_PVT_MNTR_CONFIG_DAC_CODE_PROGRAMMING_ENABLEr(unit,
                                                                     &data);
    //sal_printf(".....Finished Disabling DAC......\n");
}

/* This writes the desired DAC_CODE value and enables it. */
static void AvsSetDACcode(int unit, uint32 dac_code)
{
    //sal_printf(".....setting DAC code to 0x%lx......\n", dac_code);
    REG_WRITE_CRU_AVS_AVS_PVT_MNTR_CONFIG_DAC_CODEr(unit, &dac_code);
    //sal_printf(".....Finished setting DAC code......\n");
    AvsEnableDAC(unit);
    //sal_printf(".....Finished Enabling DAC code......\n");
}

/* this it the PVT_MNTR_CTRL word for charging the DACs */
#define PVT_MON_CTRL_FOR_DC 0x00000080

void AvsEnableDACDrivers(int unit, uint32 dac_code)
{
    uint32 val = 1;
    REG_WRITE_CRU_AVS_AVS_PVT_MNTR_CONFIG_DAC_CODEr(unit, &dac_code);
    REG_WRITE_CRU_AVS_AVS_PVT_MNTR_CONFIG_DAC_CODE_PROGRAMMING_ENABLEr(unit,
                                                                      &val);
    val = PVT_MON_CTRL_FOR_DC;
    REG_WRITE_CRU_AVS_AVS_PVT_MNTR_CONFIG_PVT_MNTR_CTRLr(unit, &val);
    AvsSleep(10*ONE_MILLI_SEC); /* allow for settling */
}

/* This version is here to handle a case where we found the voltage spiking
 * whenever we would bump the DAC up by a large amount.  This was happening
 * due to a particular voltage regulator that was quick to change when the
 * voltage was increased.  We decided to make it universal (i.e. when we
 * change the voltage up or down) because we could see this happening later
 * on some other regulator.
 */
void AvsSetDAC(int unit, uint32 dac_code)
{
    uint32 cur_dac;
    uint32 i, spread;

    REG_READ_CRU_AVS_AVS_PVT_MNTR_CONFIG_DAC_CODEr(unit, &cur_dac);
    spread = (cur_dac > dac_code)?(cur_dac - dac_code):(dac_code - cur_dac);
    sal_printf ("dwl......avs.c:AvsSetDac=%ld, cur_dac=%ld delta=%ld \n",
                dac_code, cur_dac, spread);

#define WALKING_STEPS 12 /* this is in DAC codes */

    /* We can only walk to the desired value if we have one already set
     * We "walk" to the desired voltage if we have to skip over too many
     * DAC codes to get there
     */
    if (cur_dac && spread > WALKING_STEPS) {
        unsigned steps = spread / WALKING_STEPS;
#define STEPS_DELAY (300*ONE_MICRO_SEC)

        /* Do it once just to make sure its enabled */
        AvsSetDACcode(unit, cur_dac);
        AvsSleep(STEPS_DELAY);

        for (i=0; i<steps; i++) {
            cur_dac += (dac_code > cur_dac) ? WALKING_STEPS : -WALKING_STEPS;
            AvsSetDACcode(unit, cur_dac);
            AvsSleep(STEPS_DELAY);
        }
    }
    /* Finally, set the desired DAC value */
    AvsSetDACcode(unit, dac_code);
}


#define PVT_MON_CTRL 0x00000180 /* this is the normal processing mode */


/* This is used to program the DAC to get a desired voltage:
 * This is the ONLY one that should be used to set the DAC to ensure that
 * the voltage is never read before it becomes stable
 */
void AvsSetVoltage(int unit, uint32_t dac_code)
{
    AvsSetDAC(unit, dac_code);
    SetPVTctrl(unit, PVT_MON_CTRL);
    AllowRegulatorToSettle();
}

uint32 FindNominalVoltage(int unit, uint32 board_dac_code)
{
    uint32 i;
    uint32 voltage = 0, cur_dac_code = board_dac_code;
    char result = 'X'; /*display this if we fail to reach 1V */
    BOOL success = FALSE;
    int32 step_size;

    sal_printf("\ndwl FindNominalVoltage with board_dac_code=%d,"
               " avs_nominal_voltage=%d, swing=%d\n",
               board_dac_code, avs_nominal_voltage, avs_swing_value);

    for (i = 0; i < MAX_ITERATIONS_1V; i++) {
        AvsSetVoltage(unit, cur_dac_code);
        voltage = AvsReadVoltage(unit, 3);
        //sal_printf("\n   iteration %d, cur_dac_code=%d => voltage=%d\n",
        // i, cur_dac_code, voltage);

        /* CLOSE ENOUGH? If I'm in the ball park, then we're done ... */
        if ((voltage >= (avs_nominal_voltage - avs_swing_value)) &&
            (voltage <= (avs_nominal_voltage + avs_swing_value))) {
             success = TRUE;
             result = '1';
             break;
        }

        /* This method "jumps" to the new voltage by calculating what the DAC
         * should be for the desired voltage.
         * Dynamically calculate a step size based on how far away we are from
         * the target
         * Note that current voltage may be above 1V so result can be negative
         *
         * calculated based on the minimum resolution of the DAC and the system
         * design
         */
#define MIN_VOLTAGE_STEP_SIZE INT(.00088)
         /* multiplied top and bottom by 10 to get better precision */
        step_size = ((signed)avs_nominal_voltage - (signed)voltage)*10 / 88;

        /* this should never happen as the "close enough" test above should
         * have caught this
         */
        //if (!step_size) { result='2'; break; }
        cur_dac_code += step_size;

        /* Don't let the DAC exceed the limits - this also works for setting
         * less than zero because it wraps.
         *
         * This can happen if we can still affect the voltage with DAC changes
         * but reach a DAC value of zero before it reaches 1V.
         *
         * We don't want to EVER set the DAC value to zero, so give it something
         * above that.
         *
         * Note: returning a "small" value also causes the board to reset
         * -- so use original value.
         */
        if ((cur_dac_code > BCHP_AVS_DAC_CODE_max)
            || (cur_dac_code < BCHP_AVS_DAC_CODE_min)) {
             sal_printf("FindNominal_Voltage Break Iteration[0x%lx]"
                        "voltage[0x%lx]\n", i, voltage);
             sal_printf("FindNominal_Voltage Break cur_dac_code[0x%lx]\n", cur_dac_code);
             cur_dac_code = board_dac_code; /* return this instead */
             break;
        }

        sal_printf("FindNominal_Voltage Iteration[0x%lx],  voltage[0x%lx],"
                   " step_size[%ld], cur_dac[0x%lx]\n",
                   i, voltage, step_size, cur_dac_code);
    }

    /* If the IR drop on this board is such that
     * we could not change the voltage to a point that was close enough to 1V
     * then we might have a corrupt first DAC value.
     * Try to determine which DAC value we should use.
     */
    if (!success) {
        result = 'O';
        cur_dac_code = board_dac_code;
    }

    /* We have need for returning the voltage to the nominal value
     * on an error situation.
     * This saves the DAC value we used to read that nominal voltage.
     */
    nominalDAC = cur_dac_code;
    sal_printf("\nFind Nominal Voltage - Final Result: %c\n", result);

    return cur_dac_code;
}

#define TEST (0)
// dwl set USE_DEFINED_NOMINAL_VOLTAGE for debug on high, nom, low voltages
#define USE_DEFINED_NOMINAL_VOLTAGE (1)
/* Nominal voltage: 1.0005 Swing value=0.0010 */
/* Nominal voltage * 10000 = 10005, swing value * 10000 = 10 */
//#define AVS_OTP_VALUE 0x251c0032
//#define AVS_OTP_VALUE 0x29040064
#define AVS_OTP_VALUE 0x2710004b

#define OTP_ROW_FOR_NOMINAL_VALUE 0x10
#if TEST
#define NUM_OTP_ROWS 23
static uint32 otp_read_by_row(uint8 unit, uint32 otp_row);
#endif

uint32 avenger_static_avs(int unit)
{
    uint32 board_dac_code;
    uint32 cur_dac_code;
    uint32 avs_otp_value = 0;

#if TEST
    uint32 i;
    for (i = 0; i < NUM_OTP_ROWS; i++) {
        avs_otp_value = otp_read_by_row(unit, i);
        sal_printf("OTP_ROW[%ld] = 0x%lx\n", i, avs_otp_value);
    }
#endif

#if USE_DEFINED_NOMINAL_VOLTAGE
    avs_otp_value = AVS_OTP_VALUE;
#else
    sal_printf("---Reading AVS Nominal Value---\n");
    avs_otp_value = otp_read_by_row(unit, OTP_ROW_FOR_NOMINAL_VALUE);
    sal_printf("AVS Nominal Value from OTP: 0x%lx\n", avs_otp_value);
#endif

    sal_printf("Nominal_Voltage 0x%lx\n", avs_nominal_voltage);
    avs_swing_value = avs_otp_value & 0xFFFF;
    avs_nominal_voltage = (avs_otp_value >> 16) & 0xFFFF;
    sal_printf("dwl avenger_static_avs, otp_value=0x%x, nominal_voltage=%d,"
               " swing=%d (0.1mV units)\n", avs_otp_value,
                avs_nominal_voltage, avs_swing_value);
    sal_printf("Nominal_Voltage 0x%lx\n", avs_nominal_voltage);
    if ((avs_nominal_voltage < 8500) || (avs_nominal_voltage > 11000)) {
        return 0;
    }

    if (avs_nominal_voltage == 10185) {
        return 0;
    }

    board_dac_code = GetBoardDACvalue(unit);

    sal_printf("Board DAC Code 0x%lx\n", board_dac_code);
    /*sal_printf("dwl Board DAC Code = %d \n", board_dac_code);*/

    /* We need to charge up the DAC before we enable it so its ready to be read */
#ifdef PERFORM_DAC_CHARGE
    AvsEnableDACDrivers(unit, board_dac_code);
#endif

    cur_dac_code = FindNominalVoltage(unit, board_dac_code);

    sal_printf("\ndwl avenger_static_avs() done DAC code=%d, resulting AVS"
               " voltage readings below:\n", cur_dac_code);
    AvsReadVoltage(unit, 3);
    return 0;
}

#if TEST
#define REG_CRU_OTPC_OTPC_MODE_REG    CRU_OTPC_OTPC_MODE_REG
#define REG_CRU_OTPC_OTPC_COMMAND     CRU_OTPC_OTPC_COMMAND
#define REG_CRU_OTPC_OTPC_CMD_START   CRU_OTPC_OTPC_CMD_START
#define REG_CRU_OTPC_OTPC_CPU_STATUS  CRU_OTPC_OTPC_CPU_STATUS
#define REG_CRU_OTPC_OTPC_CPU_DATA    CRU_OTPC_OTPC_CPU_DATA
#define REG_CRU_OTPC_OTPC_CPUADDR_REG CRU_OTPC_OTPC_CPUADDR_REG
static uint32 otp_read_by_row(uint8 unit, uint32 otp_row)
{
    uint32 otp_raw;
    uint32 rv;
    uint32 reg_value;
    uint32 timeout_count;

    /* 1. Enable CPU Mode access for OTP interface */
    rv = 1;
    REG_WRITE_CRU_OTPC_OTPC_MODE_REGr(unit, &rv);

    /* 2. Write OTP Read command in cpu command register */
    rv = 0;
    REG_WRITE_CRU_OTPC_OTPC_COMMANDr(unit, &rv);

    /* 3.  Write OTP Row Address in CPU address register */
    REG_WRITE_CRU_OTPC_OTPC_CPUADDR_REGr(unit, &otp_row);

    /* 4. Write CPU command start bit = 1 in command start register */
    rv = 1;
    REG_WRITE_CRU_OTPC_OTPC_CMD_STARTr(unit, &rv);

    /* 5. Write CPU command start bit = 0 in CPU command start register */
    rv = 0;
    REG_WRITE_CRU_OTPC_OTPC_CMD_STARTr(unit, &rv);

    /* 6. Wait for the command to complete by checking command done bit
     * in CPU status register
     */
    timeout_count = 0;
    do {
        REG_READ_CRU_OTPC_OTPC_CPU_STATUSr(unit, &reg_value);
        timeout_count = timeout_count + 1;
        if (timeout_count > 3000) {
            sal_printf("OTP read by row : timeout");
            return 0;
        }
    } while ((reg_value & 0x2) == 0);

    /* 7. OTP Row content would be ready in CPU read data register when command
     * is completed
     */
    REG_READ_CRU_OTPC_OTPC_CPU_DATAr(unit, &otp_raw);

#if TEST
    sal_printf("OTP read by row : otp_raw[%ld] 0x%lx\n", otp_row, otp_raw);
#endif
    return otp_raw;
}
#endif

/*
 * This is minor variation of FindNominalVoltage(). It reads the current
 * DAC code and gradually changes it until the desired nominal voltage
 * is reached. This is used by dynamic AVS to change the voltage.
 *
 * If called with dac_code=0, that means it is first time, and the function
 * will read the PVT mon value.
 */
uint32 SetAvsVoltage(int unit, uint32 nominal_voltage, uint32 dac_code)
{
    uint32 i;
    uint32 voltage = 0, cur_dac_code, board_dac_code;
    char result = 'X'; /*display this if we fail to reach 1V */
    BOOL success = FALSE;
    int32 step_size;

    if (dac_code == 0) {
      board_dac_code = GetBoardDACvalue(unit);
    }
    else {
      board_dac_code = dac_code;
    }
    cur_dac_code = board_dac_code;

    //sal_printf("\nSetAvsVoltage with dac_code=%d, board_dac_code=%d,"
    //           " nominal_voltage=%d, swing=%d\n", dac_code, board_dac_code,
    //           nominal_voltage, SWING_VALUE);

#ifdef PERFORM_DAC_CHARGE
    AvsEnableDACDrivers(unit, board_dac_code);
#endif

    //sal_printf("SetAvsVoltage - Finished Enabling DAC Drivers.\n");

    for (i = 0; i < MAX_ITERATIONS_1V; i++) {
        AvsSetVoltage(unit, cur_dac_code);
        voltage = AvsReadVoltage(unit, 3);
        //sal_printf("\ndwl iteration %d, cur_dac_code=%d, voltage=%d\n",
        //           i, cur_dac_code, voltage);

        /* CLOSE ENOUGH? If I'm in the ball park, then we're done ... */
        if ((voltage >= (nominal_voltage - SWING_VALUE)) &&
            (voltage <= (nominal_voltage + SWING_VALUE))) {
             success = TRUE;
             result = '1';
             break;
        }

        /* This method "jumps" to the new voltage by calculating what the
         * DAC should be for the desired voltage.
         *
         * Dynamically calculate a step size based on how far away we are
         * from the target.
         * note that current voltage may be above 1V so result can be negative
         */
        /* calculated based on the minimum resolution of the DAC and the
         * system design
         */
#define MIN_VOLTAGE_STEP_SIZE INT(.00088)
         /* multiplied top and bottom by 10 to get better precision */
        step_size = ((signed)nominal_voltage - (signed)voltage)*10 / 88;

        /* this should never happen as the "close enough" test above should
         * have caught this
         */
        //if (!step_size) { result='2'; break; }
        cur_dac_code += step_size;

        /* Don't let the DAC exceed the limits -
         * this also works for setting less than zero because it wraps
         *
         * This can happen if we can still affect the voltage with DAC changes
         * but reach a DAC value of zero before it reaches 1V.
         *
         * We don't want to EVER set the DAC value to zero, so give it
         * something above that.
         *
         * Note: returning a "small" value also causes the board to reset
         * -- so use original value.
         */
        if ((cur_dac_code > BCHP_AVS_DAC_CODE_max)
            || (cur_dac_code < BCHP_AVS_DAC_CODE_min)) {
             sal_printf("SetAvs_Voltage Break Iteration[0x%lx]"
                        " voltage[0x%lx]\n", i, voltage);
             sal_printf("SetAvs_Voltage Break cur_dac_code[0x%lx]\n",
                        cur_dac_code);
             cur_dac_code = board_dac_code; /* return this instead */
             break;
             // fixme: should revert to static avs?
        }

        //sal_printf("SetAvs_Voltage Iteration[0x%lx],  voltage[0x%lx], step_size[%ld],"
        //           " cur_dac[0x%lx]\n", i, voltage, step_size, cur_dac_code);
    }

    /* If the IR drop on this board is such that
     *  we could not change the voltage to a point that was close enough to 1V
     *  then we might have a corrupt first DAC value.
     *  Try to determine which DAC value we should use.
     */
    if (!success) {
        result = 'O';
        cur_dac_code = board_dac_code;
    }

    /* We have need for returning the voltage to the nominal value on an error situation.
     * This saves the DAC value we used to read that nominal voltage.
     */
    nominalDAC = cur_dac_code;
    sal_printf("SetAvsVoltage - Final Result: %c\n", result);
    return cur_dac_code;
}
