/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved$
 *
 * Filename:
 *     drv_if.h
 * Purpose:
 *     Interface to the HAL 
 */

#ifndef _DRV_IF_H
#define _DRV_IF_H


#define DRV_REG_READ(unit, addr, data, len)\
         drv_robo2_reg_read(unit, addr, data, len)

#define DRV_REG_WRITE(unit, addr, data, len)\
         drv_robo2_reg_write(unit, addr, data, len)

#define DRV_APB2PBUS_REG_READ(unit, port, addr, data, len)\
         drv_robo2_apb2pbus_reg_read(unit, port, addr, data, len)

#define DRV_APB2PBUS_REG_WRITE(unit, port, addr, data, len)\
         drv_robo2_apb2pbus_reg_write(unit, port, addr, data, len)

#define DRV_REG_LENGTH_GET(unit, reg)\
         drv_robo2_reg_length_get(unit, reg)

#define DRV_REG_ADDR(unit, reg, port, index)\
         drv_robo2_reg_addr_get(unit, reg, port, index)

#define DRV_REG_FIELD_SET(unit, reg, regbuf, field, fldbuf)\
         drv_robo2_reg_field_set(unit, reg, regbuf, field, fldbuf)

#define DRV_REG_FIELD_GET(unit, reg, regbuf, field, fldbuf)\
         drv_robo2_reg_field_get(unit, reg, regbuf, field, fldbuf)

#define DRV_MEM_LENGTH_GET(unit, mem, data)\
         drv_robo2_mem_length_get(unit, mem, data)

#define DRV_MEM_WIDTH_GET(unit, mem, data)\
         drv_robo2_mem_width_get(unit, mem, data)

#define DRV_MEM_READ(unit, mem, entry_id, count, entry)\
         drv_robo2_mem_read(unit, mem, entry_id, count, entry)

#define DRV_MEM_WRITE(unit, mem, entry_id, count, entry)\
         drv_robo2_mem_write(unit, mem, entry_id, count, entry)

#define DRV_MEM_FIELD_GET(unit, mem, field_index, entry, fld_data)\
         drv_robo2_mem_field_get(unit, mem, field_index, entry, fld_data)

#define DRV_MEM_FIELD_SET(unit, mem, field_index, entry, fld_data)\
         drv_robo2_mem_field_set(unit, mem, field_index, entry, fld_data)



extern int drv_robo2_apb2pbus_reg_read(int unit, int port, uint32 addr, void *data, int len);
extern int drv_robo2_apb2pbus_reg_write(int unit, int port, uint32 addr, void *data, int len);
extern int drv_robo2_reg_read(int unit, uint32 addr, void *data, int len);
extern int drv_robo2_reg_write(int unit, uint32 addr, void *data, int len);
extern int reg_read(int unit, uint32 addr);
extern int reg_write(int unit, uint32 addr, int value);
extern int xlreg_read(int unit, int port, uint32 addr);
extern int xlreg_write(int unit, int port, uint32 addr, uint32 valuelo, uint32 valuehi);
extern int drv_robo2_reg_length_get(int unit, uint32 reg);
extern uint32 drv_robo2_reg_addr_get(int unit, uint32 reg, int port, int index);
extern int drv_robo2_reg_field_set(int unit, uint32 reg, uint32 *regbuf, uint32 field, uint32 *fldbuf);
extern int drv_robo2_reg_field_get(int unit, uint32 reg, uint32 *regbuf, uint32 field, uint32 *fldbuf);
extern int drv_robo2_mem_length_get(int unit, uint32 mem, uint32 *data);
extern int drv_robo2_mem_width_get(int unit, uint32 mem, uint32 *data);
extern int drv_robo2_mem_read(int unit, uint32 mem, uint32 entry_id, uint32 count, uint32 *entry);
extern int drv_robo2_mem_write(int unit, uint32 mem, uint32 entry_id, uint32 count, uint32 *entry);
extern int drv_robo2_mem_field_get(int unit, uint32 mem, uint32 field_index, uint32 *entry, uint32 *fld_data);
extern int drv_robo2_mem_field_set(int unit, uint32 mem, uint32 field_index, uint32 *entry, uint32 *fld_data);
extern char* drv_robo2_reg_name_get(int unit, uint32 reg);
extern int reg_block_read(int unit, uint32 block);

#endif
