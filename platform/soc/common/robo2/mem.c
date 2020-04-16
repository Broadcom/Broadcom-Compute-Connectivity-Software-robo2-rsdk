/*
 * $ID:$
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved$
 *
 * Filename:
 *     mem.c
 * Description:
 *     HAL for memory access
 */

#include <sal_types.h>
#include <sal_console.h> /* [VKC] needed for NULL def !! */
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/memory.h>
#include <soc/mem.h>

/**
 * @par Function : drv_robo2_mem_length_get
 *
 * @par  Purpose :
 *      Get the number of entries of the selected memory.
 *
 * @params [in]  unit  :   unit id
 * @params [in]  mem   :   memory entry type.
 * @params [out] data  :   total number entries of this memory type.
 *
 * @returns SOC_E_XXX
 *
 */
int
drv_robo2_mem_length_get(int unit, uint32 mem, uint32 *data)
{
    return SOC_E_UNAVAIL;
}

/**
 * @par Function : drv_robo2_mem_width_get
 *
 * @par  Purpose :
 *      Get the width of an entry in the given memory
 *
 * @params [in]  unit  :   unit id
 * @params [in]  mem   :   memory entry type.
 * @params [out] data  :   total number entries of this memory type.
 *
 * @returns SOC_E_XXX
 *
 */
int
drv_robo2_mem_width_get(int unit, uint32 mem, uint32 *data)
{
    return SOC_E_UNAVAIL;
}


/**
 * @par Function : drv_robo2_mem_read
 *
 * @par Purpose :
 *      Get the width of selected memory.
 *      The value returned in data is width in bits.
 *
 * @params [in]   unit       :  unit id
 * @params [in]   mem        :  the memory type to access.
 * @params [in]   entry_id   :  Starting index of entry to read
 * @params [in]   count      :  Number of entries to read
 * @params [out]  entry_data :  pointer to a buffer of 32-bit words
 *                              to contain the read result.
 *
 * @returns SOC_E_XXX
 *
 */
int
drv_robo2_mem_read(int unit, uint32 mem,
                   uint32 entry_id, uint32 count, uint32 *entry)
{
    return SOC_E_UNAVAIL;
}


/**
 * @par Function : drv_robo2_mem_write
 *
 * @par Purpose :
 *      write given entry into the given table at the given index
 *      Supports writing more than one entry
 *
 * @params [in]   unit       :  unit id
 * @params [in]   mem        :  the memory type to access.
 * @params [in]   entry_id   :  Starting index of entry to read
 * @params [in]   count      :  Number of entries to read
 * @params [in]  entry_data :  pointer to a buffer of 32-bit words
 *                              to contain the read result.
 *
 * @returns SOC_E_XXX
 *
 */
int
drv_robo2_mem_write(int unit, uint32 mem,
                   uint32 entry_id, uint32 count, uint32 *entry)
{
    return SOC_E_UNAVAIL;
}

/**
 * @par Function : drv_robo2_mem_field_get
 *
 * @par Purpose :
 *      Extract the value of a field from a memory entry value.
 *
 * @parms [in]   unit        :  unit id
 * @parms [in]   mem         :  memory indication.
 * @parms [in]   field_index :  field type.
 * @parms [in]   entry       :  entry value pointer.
 * @parms [out]  fld_data    :  field value pointer.
 *
 * @returns SOC_E_XXX
 */
int
drv_robo2_mem_field_get(int unit, uint32 mem, uint32 field_index, 
                        uint32 *entry, uint32 *fld_data)
{
    return SOC_E_UNAVAIL;
}


/**
 * @par Function : drv_robo2_mem_field_set
 *
 * @par Purpose :
 *      Set the value of a field in a memory entry value.
 *
 * @parms [in]  unit        :  unit id
 * @parms [in]  mem         :  memory indication.
 * @parms [in]  field_index :  field type.
 * @parms [in]  entry       :  entry value pointer.
 * @parms [in]  fld_data    :  field value pointer.
 *
 * @returns SOC_E_XXX
 */
int
drv_robo2_mem_field_set(int unit, uint32 mem, uint32 field_index, 
                        uint32 *entry, uint32 *fld_data)
{
    return SOC_E_UNAVAIL;
}


