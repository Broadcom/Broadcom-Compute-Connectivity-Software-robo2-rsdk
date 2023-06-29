/*
 * $Copyright: (c) 2017 Broadcom Corporation
 * All Rights Reserved.$
 *
 * i2c.c
 *
 * i2c driver implementation.
 */

#include <i2c/i2c.h>
#include "sal_console.h"
#include <soc/robo2/common/memregs.h>
#include <soc/drv.h>
#include <sal_time.h>

static int i2c_debug = 0;
#define i2c_printf(format, args...)              \
do {                                             \
    if (i2c_debug > 0) {                         \
        sal_printf((format), ##args);            \
    }                                            \
} while(0)

#define I2C_DEBUG(str)                                             \
do {                                                               \
    i2c_printf("%s(%d): %s.\n", __FUNCTION__, __LINE__, (str));    \
}while(0)

#define I2C_RESULT(result, str)                  \
do {                                             \
    if ((result) != 0) {                         \
        I2C_DEBUG(str);                          \
        sal_mutex_give(i2c_ctrl[unit].mutexid);  \
        return (result);                         \
    }                                            \
}while(0)

/*
 * Transaction error codes defined in Master command register (0x30)
 */
#define MSTR_STS_XACT_SUCCESS              0
#define MSTR_STS_LOST_ARB                  1
#define MSTR_STS_NACK_FIRST_BYTE           2
#define MSTR_STS_NACK_NON_FIRST_BYTE       3

/*
 * SMBUS protocol values defined in register 0x30
 */
#define SMBUS_PROT_QUICK_CMD               0
#define SMBUS_PROT_SEND_BYTE               1
#define SMBUS_PROT_RECV_BYTE               2
#define SMBUS_PROT_WR_BYTE                 3
#define SMBUS_PROT_RD_BYTE                 4
#define SMBUS_PROT_WR_WORD                 5
#define SMBUS_PROT_RD_WORD                 6
#define SMBUS_PROT_BLK_WR                  7
#define SMBUS_PROT_BLK_RD                  8
#define SMBUS_PROT_PROC_CALL               9
#define SMBUS_PROT_BLK_WR_BLK_RD_PROC_CALL 10

#define SMB_MAX_RETRIES                    65535
#define I2C_SMBUS_BLOCK_MAX                32
#define I2C_CHIP_MAX                       128

#define I2C_MASTER_TX_FIFO_SIZE            64
#define I2C_MASTER_RX_FIFO_SIZE            64
#define I2C_SLAVE_TX_FIFO_SIZE             64
#define I2C_SLAVE_RX_FIFO_SIZE             64

/*
 * Enum to specify clock speed. The user will provide it during initialization.
 * If needed, it can be changed dynamically
 */
typedef enum iproc_smb_clk_freq {
    I2C_SPEED_100KHz = 0,
    I2C_SPEED_400KHz = 1,
    I2C_SPEED_INVALID = 255
} smb_clk_freq_t;

/* Per unit i2c control structure*/
typedef struct _i2c_ctr {
    sal_mutex_t mutexid;
    int32 master_init_done;
    int32 master_fifo_bytes;
}i2c_ctr;

i2c_ctr i2c_ctrl[2];

static int32 i2c_dump_regs(int32 unit)
{
    uint32_t regval;

    i2c_printf("\n----------------------------------------------\n");
    i2c_printf("Dumping SMBus registers... \n");
    REG_READ_M7SS_SMBUS_SMBUS_CONFIGr(unit, &regval);
    i2c_printf("SMB_CFG_REG=0x%08X\n", regval);
    REG_READ_M7SS_SMBUS_SMBUS_TIMING_CONFIGr(unit, &regval);
    i2c_printf("SMB_TIMGCFG_REG=0x%08X\n", regval);
    REG_READ_M7SS_SMBUS_SMBUS_ADDRESSr(unit, &regval);
    i2c_printf("SMB_ADDR_REG=0x%08X\n", regval);
    REG_READ_M7SS_SMBUS_SMBUS_MASTER_FIFO_CONTROLr(unit, &regval);
    i2c_printf("SMB_MSTRFIFOCTL_REG=0x%08X\n", regval);
    REG_READ_M7SS_SMBUS_SMBUS_SLAVE_FIFO_CONTROLr(unit, &regval);
    i2c_printf("SMB_SLVFIFOCTL_REG=0x%08X\n", regval);
    REG_READ_M7SS_SMBUS_SMBUS_BIT_BANG_CONTROLr(unit, &regval);
    i2c_printf("SMB_BITBANGCTL_REG=0x%08X\n", regval);
    REG_READ_M7SS_SMBUS_SMBUS_MASTER_COMMANDr(unit, &regval);
    i2c_printf("SMB_MSTRCMD_REG=0x%08X\n", regval);
    REG_READ_M7SS_SMBUS_SMBUS_SLAVE_COMMANDr(unit, &regval);
    i2c_printf("SMB_SLVCMD_REG=0x%08X\n", regval);
    REG_READ_M7SS_SMBUS_SMBUS_EVENT_ENABLEr(unit, &regval);
    i2c_printf("SMB_EVTEN_REG=0x%08X\n", regval);
    REG_READ_M7SS_SMBUS_SMBUS_EVENT_STATUSr(unit, &regval);
    i2c_printf("SMB_EVTSTS_REG=0x%08X\n", regval);
    REG_READ_M7SS_SMBUS_SMBUS_MASTER_DATA_WRITEr(unit, &regval);
    i2c_printf("SMB_MSTRDATAWR_REG=0x%08X\n", regval);
    REG_READ_M7SS_SMBUS_SMBUS_MASTER_DATA_READr(unit, &regval);
    i2c_printf("SMB_MSTRDATARD_REG=0x%08X\n", regval);
    REG_READ_M7SS_SMBUS_SMBUS_SLAVE_DATA_WRITEr(unit, &regval);
    i2c_printf("SMB_SLVDATAWR_REG=0x%08X\n", regval);
    REG_READ_M7SS_SMBUS_SMBUS_SLAVE_DATA_READr(unit, &regval);
    i2c_printf("SMB_SLVDATARD_REG=0x%08X\n", regval);
    i2c_printf("----------------------------------------------\n\n");
    return 0;
}

static int32 i2c_set_clk_freq(int32 unit, smb_clk_freq_t freq)
{
    uint32_t regval, val;

    switch (freq) {
        case I2C_SPEED_100KHz:
            val = 0;
            break;
        case I2C_SPEED_400KHz:
            val = 1;
            break;
        default:
            return -1;
    }
    REG_READ_M7SS_SMBUS_SMBUS_TIMING_CONFIGr(unit, &regval);
    soc_M7SS_SMBUS_SMBUS_TIMING_CONFIGr_field_set(unit, &regval, MODE_400f,
                                                  &val);
    REG_WRITE_M7SS_SMBUS_SMBUS_TIMING_CONFIGr(unit, &regval);
    return 0;
}

/*
 * Function to init I2C master device
 */
int32 i2c_master_init(int32 unit)
{
    uint32_t regval = 0;
    uint32_t val;

    i2c_printf("%s: Entering\n", __FUNCTION__);

    if (0 != i2c_ctrl[unit].master_init_done) {
        return 0;
    }
    i2c_ctrl[unit].mutexid = sal_mutex_create("i2c access");

    /*
     * Flush Tx, Rx FIFOs. Note we are setting the Rx FIFO threshold to 0.
     * May be OK since we are setting RX_EVENT and RX_FIFO_FULL interrupts
     */
    val = 1;
    soc_M7SS_SMBUS_SMBUS_MASTER_FIFO_CONTROLr_field_set(unit, &regval,
                                               MASTER_RX_FIFO_FLUSHf, &val);
    soc_M7SS_SMBUS_SMBUS_MASTER_FIFO_CONTROLr_field_set(unit, &regval,
                                               MASTER_TX_FIFO_FLUSHf, &val);
    REG_WRITE_M7SS_SMBUS_SMBUS_MASTER_FIFO_CONTROLr(unit, &regval);

    /*
     * Enable SMbus block. Note, we are setting MASTER_RETRY_COUNT to zero
     * since there will be only one master
     */
    REG_READ_M7SS_SMBUS_SMBUS_CONFIGr(unit, &regval);
    val = 1;
    soc_M7SS_SMBUS_SMBUS_CONFIGr_field_set(unit, &regval, SMB_ENf, &val);
    REG_WRITE_M7SS_SMBUS_SMBUS_CONFIGr(unit, &regval);

    REG_READ_M7SS_SMBUS_SMBUS_CONFIGr(unit, &regval);
    val = 1;
    soc_M7SS_SMBUS_SMBUS_CONFIGr_field_set(unit, &regval, RESETf, &val);
    REG_WRITE_M7SS_SMBUS_SMBUS_CONFIGr(unit, &regval);
    val = 0;
    soc_M7SS_SMBUS_SMBUS_CONFIGr_field_set(unit, &regval, RESETf, &val);
    REG_WRITE_M7SS_SMBUS_SMBUS_CONFIGr(unit, &regval);

    /* Wait a minimum of 50 Usec, as per SMB hw doc. But we wait longer*/
    sal_usleep(100);
    i2c_set_clk_freq(unit, I2C_SPEED_100KHz);
    regval = 0x0;
    REG_WRITE_M7SS_SMBUS_SMBUS_EVENT_ENABLEr(unit, &regval);
    REG_READ_M7SS_SMBUS_SMBUS_EVENT_STATUSr(unit, &regval);
    REG_WRITE_M7SS_SMBUS_SMBUS_EVENT_STATUSr(unit, &regval);
    i2c_ctrl[unit].master_init_done = 1;
    i2c_ctrl[unit].master_fifo_bytes = 0;
    i2c_printf("%s: Init successful\n", __FUNCTION__);
    if (i2c_debug > 1) {
        i2c_dump_regs(unit);
    }
    return 0;
}

/*
 * Function to ensure that the previous transaction was completed before
 * initiating a new transaction. It can also be used in polling mode to
 * check status of completion of a command
 */
static int32 i2c_master_startbusy_wait(int32 unit, uint32_t *status)
{
    uint32_t regval = 0, i = 0, val = 0;

    /*
     * Check if an operation is in progress. During probe it won't be.
     * But when shutdown/remove was called we want to make sure that
     * the transaction in progress completed
     */
    while (i < SMB_MAX_RETRIES) {
        REG_READ_M7SS_SMBUS_SMBUS_MASTER_COMMANDr(unit, &regval);
        soc_M7SS_SMBUS_SMBUS_MASTER_COMMANDr_field_get(unit, &regval,
                                       MASTER_START_BUSY_COMMANDf ,&val);
        if (!val) {
            break;
        }
        i++;
    }

    if (i >= SMB_MAX_RETRIES) {
        i2c_printf("%s: START_BUSY bit didn't clear, exiting\n", __FUNCTION__);
        return -1;
    }

    soc_M7SS_SMBUS_SMBUS_MASTER_COMMANDr_field_get(unit, &regval,
                                                   MASTER_STATUSf ,&val);
    *status = val;
    i2c_ctrl[unit].master_fifo_bytes = 0;

    return 0;
}

static int32 i2c_master_write_addr(int unit, uint8_t chip_addr, BOOL read,
                                   BOOL last)
{
    int32 result = 0;
    uint32_t value = 0;
    uint32_t data = 0;

    if (i2c_ctrl[unit].master_fifo_bytes > 0) {
        i2c_printf("%s: Master TX FIFO is not empty! %d bytes!", __FUNCTION__,
                   i2c_ctrl[unit].master_fifo_bytes);
        return -1;
    }
    value = chip_addr << 1;
    if (TRUE == read) {
        value |= 0x1;
    }
    if (TRUE == last) {
        data = 0x01;
        soc_M7SS_SMBUS_SMBUS_MASTER_DATA_WRITEr_field_set(unit, &value,
                                                MASTER_WR_STATUSf, &data);
    }
    REG_WRITE_M7SS_SMBUS_SMBUS_MASTER_DATA_WRITEr(unit, &value);
    i2c_ctrl[unit].master_fifo_bytes = 1;

    return result;
}

static int32 i2c_master_write_data(int32 unit, uint8_t *buffer, uint32_t length)
{
    int32 result = 0;
    int32 i = 0;
    uint32_t value = 0;
    uint32_t field = 0;

    if ((i2c_ctrl[unit].master_fifo_bytes + length) > I2C_MASTER_TX_FIFO_SIZE) {
        i2c_printf("%s: Try to send %d bytes!", __FUNCTION__,
                   (i2c_ctrl[unit].master_fifo_bytes + length));
        return -1;
    }

    for (i = 0; i < length; i++) {
        value = buffer[i];
        if (i == (length - 1)) {
            field = 0x01;
            soc_M7SS_SMBUS_SMBUS_MASTER_DATA_WRITEr_field_set(unit, &value,
                                                   MASTER_WR_STATUSf, &field);
        }
        REG_WRITE_M7SS_SMBUS_SMBUS_MASTER_DATA_WRITEr(unit, &value);
        i2c_ctrl[unit].master_fifo_bytes = i2c_ctrl[unit].master_fifo_bytes + 1;
    }

    return result;
}

static int32 i2c_master_write_cmd(int32 unit, uint8_t cmd, uint32_t rd_length)
{
    uint32_t field = 0;
    uint32_t value = 0;

    field = cmd;
    soc_M7SS_SMBUS_SMBUS_MASTER_COMMANDr_field_set(unit, &value,
                                           SMBUS_PROTOCOLf, &field);
    field = rd_length;
    soc_M7SS_SMBUS_SMBUS_MASTER_COMMANDr_field_set(unit, &value,
                                           RD_BYTE_COUNTf, &field);
    field = 1;
    soc_M7SS_SMBUS_SMBUS_MASTER_COMMANDr_field_set(unit, &value,
                                        MASTER_START_BUSY_COMMANDf, &field);
    REG_WRITE_M7SS_SMBUS_SMBUS_MASTER_COMMANDr(unit, &value);

    return 0;
}

static int32 i2c_master_read_data(int32 unit, uint8_t *buffer, uint32_t length)
{
    int32 result = 0;
    uint32_t value = 0, step = 0;

    for (step = 0; step < length; step++) {
        REG_READ_M7SS_SMBUS_SMBUS_MASTER_DATA_READr(unit, &value);
        buffer[step] = value & 0xff;
    }

    return result;
}

/*
 * Probe the given I2C chip address.  Returns 0 if a chip responded,
 * not 0 on failure.
 */
int32 i2c_master_probe (int32 unit, uint8_t chip)
{
    uint32_t status;
    int32_t  result;

    i2c_printf("%s: Entering\n", __FUNCTION__);
    sal_mutex_take(i2c_ctrl[unit].mutexid, sal_mutex_FOREVER);
    result = i2c_master_startbusy_wait(unit, &status);
    if (result != 0) {
        i2c_printf("i2c_master_startbusy_wait failed, reset controller");
        i2c_ctrl[unit].master_init_done = 0;
        result = i2c_master_init(unit);
        I2C_RESULT(result, "i2c_master_init failed");
    }
    result = i2c_master_write_addr(unit, chip, FALSE, TRUE);
    I2C_RESULT(result, "i2c_master_write_addr failed");
    result = i2c_master_write_cmd(unit, SMBUS_PROT_QUICK_CMD, 0);
    I2C_RESULT(result, "i2c_master_write_cmd failed");
    result = i2c_master_startbusy_wait(unit, &status);
    I2C_RESULT(result, "i2c_master_startbusy_wait failed");
    if (status != 0) {
        i2c_printf("i2c_master_startbusy_wait status 0x%x\n", status);
        sal_mutex_give(i2c_ctrl[unit].mutexid);
        return -1;
    }
    sal_mutex_give(i2c_ctrl[unit].mutexid);

    return result;
}

int32 i2c_master_write (int32 unit, I2C_MUX_SEL mux, uint8_t chip,
                        uint8_t *buffer, uint32_t length)
{
    uint32_t status;
    uint32_t data;
    int32  result;

    i2c_printf("%s: Entering\n", __FUNCTION__);
    if (buffer == NULL) {
        i2c_printf("%s: Invalid buffer!!!", buffer);
        return -1;
    }
    if ((length + 1) > I2C_MASTER_TX_FIFO_SIZE) {
        i2c_printf("%s: Invalid length %d!!!", length);
        return -1;
    }
    sal_mutex_take(i2c_ctrl[unit].mutexid, sal_mutex_FOREVER);
    data = mux;
    REG_WRITE_CRU_CRU_I2C_MUX_SELr(unit, &data);
    result = i2c_master_startbusy_wait(unit, &status);
    if (result != 0) {
        i2c_printf("i2c_master_startbusy_wait failed, reset controller");
        i2c_ctrl[unit].master_init_done = 0;
        result = i2c_master_init(unit);
        I2C_RESULT(result, "i2c_master_init failed");
    }
    result = i2c_master_write_addr(unit, chip, FALSE, FALSE);
    I2C_RESULT(result, "i2c_master_write_addr failed");
    result = i2c_master_write_data(unit, buffer, length);
    I2C_RESULT(result, "i2c_master_write_data failed");
    result = i2c_master_write_cmd(unit, SMBUS_PROT_BLK_WR, 0);
    I2C_RESULT(result, "i2c_master_write_cmd failed");
    result = i2c_master_startbusy_wait(unit, &status);
    I2C_RESULT(result, "i2c_master_startbusy_wait failed");
    if (status != 0) {
        i2c_printf("i2c_master_startbusy_wait status 0x%x\n", status);
        sal_mutex_give(i2c_ctrl[unit].mutexid);
        return -1;
    }
    sal_mutex_give(i2c_ctrl[unit].mutexid);
    return result;
}

int32 i2c_master_read (int32 unit, I2C_MUX_SEL mux, uint8_t chip,
                       uint8_t *buffer, uint32_t length)
{
    uint32_t status;
    uint32_t data;
    int32  result;
    BOOL last = FALSE;

    i2c_printf("%s: Entering\n", __FUNCTION__);
    if (buffer == NULL) {
        i2c_printf("%s: Invalid buffer!!!", buffer);
        return -1;
    }
    if ((length + 1) > I2C_MASTER_RX_FIFO_SIZE) {
        i2c_printf("%s: Invalid length %d!!!", length);
        return -1;
    }
    sal_mutex_take(i2c_ctrl[unit].mutexid, sal_mutex_FOREVER);
    data = mux;
    REG_WRITE_CRU_CRU_I2C_MUX_SELr(unit, &data);
    result = i2c_master_startbusy_wait(unit, &status);
    if (result != 0) {
        i2c_printf("i2c_master_startbusy_wait failed, reset controller");
        i2c_ctrl[unit].master_init_done = 0;
        result = i2c_master_init(unit);
        I2C_RESULT(result, "i2c_master_init failed");
    }
    if (0 == length) {
        last = TRUE;
    }
    else {
        last = FALSE;
    }
    result = i2c_master_write_addr(unit, chip, TRUE, last);
    I2C_RESULT(result, "i2c_master_write_addr failed");
    result = i2c_master_write_cmd(unit, SMBUS_PROT_BLK_RD, length);
    I2C_RESULT(result, "i2c_master_write_cmd failed");
    result = i2c_master_startbusy_wait(unit, &status);
    I2C_RESULT(result, "i2c_master_startbusy_wait failed");
    if (status != 0) {
        i2c_printf("i2c_master_startbusy_wait status 0x%x\n", status);
        sal_mutex_give(i2c_ctrl[unit].mutexid);
        return -1;
    }
    result = i2c_master_read_data(unit, buffer, length);
    I2C_RESULT(result, "i2c_master_read_data failed");
    sal_mutex_give(i2c_ctrl[unit].mutexid);

    return result;
}
