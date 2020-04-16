/*
 * idxres_afl.h
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 *
 * Description:
 *    Module: Indexed resource management, using banked lists
 */
#ifndef _SHR_IDXRES_AFL_
#define _SHR_IDXRES_AFL_

#include <types.h>

typedef uint32 shr_aidxres_element_t;

struct _shr_aidxres_list_s;

typedef struct _shr_aidxres_list_s *shr_aidxres_list_handle_t;

/*
 *   Function
 *      shr_aidxres_list_create
 *   Purpose
 *      Create an aligned/contiguous_blocked banked free list
 *   Parameters
 *      (out) shr_aidxres_list_handle_t *list = place to put list handle
 *      (in) shr_aidxres_element_t first = number of first entry to manage
 *      (in) shr_aidxres_element_t last = number of last entry to manage
 *      (in) shr_aidxres_element_t validLow = low valid entry value
 *      (in) shr_aidxres_element_t validHigh = high valid entry value
 *      (in) shr_aidxres_element_t block_factor = max block power of two
 *      (in) char *name = name for the list (used for sal_alloc)
 *   Returns
 *      bcm_error_t = BCM_E_NONE if list created successfully
 *                    BCM_E_* as appropriate otherwise
 *   Notes
 *      The validLow and validHigh values are used to specify the valid range
 *      of entries for querying 'free/used' status of an entry; any value not
 *      in this range is considered an invalid argument, but values that are
 *      not between first and last will be permanently 'used' and not allowed
 *      by the free operation nor ever provided by the allocate operation.
 *      The blocking factor is the actual power of two that is to be used when
 *      computing maximum block size.  Blocks will be able to be manipulated up
 *      to 2^blocking_factor, but note that blocking_factor must be less than
 *      or equal to the number of bits used for bank index (so in 8b mode, this
 *      must be 7 or less, in 16b mode it must be 15 or less, and in 32b mode
 *      it must be 31 or less).
 */
extern int
shr_aidxres_list_create(shr_aidxres_list_handle_t *list,
                        shr_aidxres_element_t first,
                        shr_aidxres_element_t last,
                        shr_aidxres_element_t valid_low,
                        shr_aidxres_element_t valid_high,
                        shr_aidxres_element_t block_factor,
                        char *name);

/*
 *   Function
 *      shr_aidxres_list_destroy
 *   Purpose
 *      Destroy a list
 *   Parameters
 *      (in) shr_aidxres_list_handle_t list = the list handle
 *   Returns
 *      bcm_error_t = BCM_E_NONE if list created successfully
 *                    BCM_E_* as appropriate otherwise
 *   Notes
 *      This destroys the list, but does not claim the semaphore first, so the
 *      caller must take care not to destroy the list while it's being used.
 *      It is possible that some OSes will not permit the destruction of a lock
 *      that is in use, so maybe that at least helps.  It is also willing to
 *      destroy the list even if there are still allocated entries.
 */
extern int
shr_aidxres_list_destroy(shr_aidxres_list_handle_t list);

/*
 *   Function
 *      shr_aidxres_list_alloc
 *   Purpose
 *      Allocate the next available single element from a list
 *   Parameters
 *      (in) shr_aidxres_list_handle_t list = list from which to allocate
 *      (out) shr_aidxres_element_t *element = where to put alloced elem num
 *   Returns
 *      bcm_error_t = BCM_E_NONE if element allocated successfully
 *                    BCM_E_* as appropriate otherwise
 *   Notes
 *      As for the idxres list alloc call, this returns *one* element.
 */
extern int
shr_aidxres_list_alloc(shr_aidxres_list_handle_t list,
                       shr_aidxres_element_t *element);

/*
 *   Function
 *      shr_aidxres_list_free
 *   Purpose
 *      Free an element or block of elements back to a list
 *   Parameters
 *      (in) shr_aidxres_list_handle_t list = list from which elem was alloced
 *      (in) shr_aidxres_element_t element = element number to free
 *                                           (or first element in block)
 *   Returns
 *      bcm_error_t = BCM_E_NONE if element freed successfully
 *                    BCM_E_* as appropriate otherwise
 *   Notes
 *      Freeing an entry already in the list is checked, as well as freeing an
 *      entry outside of the list-managed range.  Elements can be freed using
 *      either free call, no matter which alloc call was used to obtain them.
 */
extern int
shr_aidxres_list_free(shr_aidxres_list_handle_t list,
                      shr_aidxres_element_t element);

/*
 *   Function
 *      shr_aidxres_list_reserve_block
 *   Purpose
 *      Reserve a block in a list
 *   Parameters
 *      (in) shr_aidxres_list_handle_t list = list handle
 *      (in) shr_aidxres_element_t first = first element in block to reserve
 *      (in) shr_aidxres_element_t count = number of elements in block
 *   Returns
 *      bcm_error_t = BCM_E_NONE if elements reserved successfully
 *                    BCM_E_* as appropriate otherwise
 *   Notes
 *      This is truly an inefficient way to manage top and bottom reservations
 *      unless they are not known at list creation time, as this does not do
 *      anything to adjust the physical size of the list's workspace; it merely
 *      takes the requested range out of the available elements.
 *      Elements reserved in this manner can be returned using free; they are
 *      allocated a a single block.
 *      The block to be reserved must satisfy all allocation rules for blocks
 *      (for example, alignment and size) that apply to the list.
 */
extern int
shr_aidxres_list_reserve_block(shr_aidxres_list_handle_t list,
                               shr_aidxres_element_t first,
                               shr_aidxres_element_t count);

/*
 *  This variable controls sanity checking behaviour, assuming the feature is
 *  enabled in the idxres_afl.c file.
 *
 *  Note that CREATE only checks on RETURN; there is no check to perform for
 *  CREATE on ENTRY.  There is no point in doing checks before DESTROY.
 */
extern uint32 _aidxres_sanity_settings;
#define _AIDXRES_SANITY_POINT_ENTRY  0x00000001
#define _AIDXRES_SANITY_POINT_RETURN 0x00000002
#define _AIDXRES_SANITY_FUNC_ALLOC   0x00000010
#define _AIDXRES_SANITY_FUNC_FREE    0x00000020
#define _AIDXRES_SANITY_FUNC_CREATE  0x00000040
#define _AIDXRES_SANITY_DUMP_FAULTS  0x00010000

#endif /* ndef _SHR_IDXRES_AFL_ */

