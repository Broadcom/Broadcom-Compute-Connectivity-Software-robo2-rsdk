/*
 *
 * $Copyright: (c) 2017 Broadcom Corporation
 * All Rights Reserved.$
 *
 * fsal_pktio_cint_data.c
 *
 * Hand-coded support for FSAL routines.
 */
int fsal_pktio_cint_data_not_empty;

#if defined(INCLUDE_LIB_CINT)
#include <sal_types.h>
#include <types.h>
#include <cint_config.h>
#include <cint_types.h>
#include <cint_porting.h>
#include <cosq.h>
#include <pktio.h>
#include <fsal_int/pkt.h>


extern void dump_pktio_stats(void);
#ifdef CONFIG_PACKET_DEBUG
extern void set_pkt_dump(int dir, int enable);

CINT_FWRAPPER_CREATE_VP2(set_pkt_dump,
                         int, int, dir, 0, 0,
                         int, int, enable, 0, 0);
#endif /* CONFIG_PACKET_DEBUG */

CINT_FWRAPPER_CREATE_VP0(dump_pktio_stats);
CINT_FWRAPPER_CREATE_VP1(cbx_packet_attr_t_init,
                         cbx_packet_attr_t*, cbx_packet_attr_t,
                         packet_attr, 1, 0);
CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         cbx_pktio_init,
                         cbx_pktio_params_t *, cbx_pktio_params_t,
                         pktio_params, 1, 0);
CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         cbx_pktio_detach,
                         cbx_pktio_params_t *, cbx_pktio_params_t,
                         pktio_params, 1, 0);
CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         cbx_pktio_register,
                         cbx_packet_cb_info_t, cbx_packet_cb_info_t,
                         cb_info, 0, 0,
                         cbx_packet_io_callback_t, cbx_packet_io_callback_t,
                         cb, 0, 0,
                         cbx_pktio_rx_cb_handle *, cbx_pktio_rx_cb_handle,
                         handle, 0, 0);
CINT_FWRAPPER_CREATE_RP1(int, int, 0, 0,
                         cbx_pktio_unregister,
                         cbx_pktio_rx_cb_handle, cbx_pktio_rx_cb_handle,
                         handle, 0, 0);
CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         cbx_pktio_packet_send,
                         void*, void, buffer, 1, 0,
                         int, int, buffer_size, 0, 0,
                         cbx_packet_attr_t*, cbx_packet_attr_t,
                         packet_attr, 1, 0);


static void*
__cint_maddr__cbx_pktio_params_t(void* p, int mnum, cint_struct_type_t* parent
)
{
    void* rv;
    cbx_pktio_params_t* s = (cbx_pktio_params_t *) p;
    switch(mnum)
    {
        case 0: rv = &(s->flags); break;
        default: rv = NULL; break;
    }
    return rv;
}


static void*
__cint_maddr__cbx_packet_attr_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    cbx_packet_attr_t* s = (cbx_packet_attr_t *) p;
    switch(mnum)
    {
        case 0: rv = &(s->flags); break;
        case 1: rv = &(s->vlanid); break;
        case 2: rv = &(s->tc); break;
        case 3: rv = &(s->source_index); break;
        case 4: rv = &(s->dest_index); break;
        case 5: rv = &(s->source_port); break;
        case 6: rv = &(s->dest_port); break;
        case 7: rv = &(s->mcastid); break;
        case 8: rv = &(s->trapid); break;
        case 9: rv = &(s->trap_data); break;
        default: rv = NULL; break;
    }
    return rv;
}
static void*
__cint_maddr__cbx_packet_cb_info_t(void* p, int mnum, cint_struct_type_t* parent)
{
    void* rv;
    cbx_packet_cb_info_t* s = (cbx_packet_cb_info_t *) p;
    switch(mnum)
    {
        case 0: rv = &(s->flags); break;
        case 1: rv = &(s->name); break;
        case 2: rv = &(s->priority); break;
        case 3: rv = &(s->cookie); break;
        default: rv = NULL; break;
    }
    return rv;
}
static cint_parameter_desc_t __cint_struct_members__cbx_pktio_params_t[] =
{
    {
        "uint32_t",
        "flags",
        0,
        0
    },
    CINT_ENTRY_LAST
};
static cint_parameter_desc_t __cint_struct_members__cbx_packet_attr_t[] =
{
    {
        "uint32_t",
        "flags",
        0,
        0
    },
    {
        "cbx_vlanid_t",
        "vlanid",
        0,
        0
    },
    {
        "cbx_tc_t",
        "tc",
        0,
        0
    },
    {
        "int",
        "source_index",
        0,
        0
    },
    {
        "int",
        "dest_index",
        0,
        0
    },
    {
        "cbx_portid_t",
        "source_port",
        0,
        0
    },
    {
        "cbx_portid_t",
        "dest_port",
        0,
        0
    },
    {
        "cbx_mcastid_t",
        "mcastid",
        0,
        0
    },
    {
        "cbx_trapid_t",
        "trapid",
        0,
        0
    },
    {
        "uint32_t",
        "trap_data",
        0,
        0
    },
    CINT_ENTRY_LAST
};
static cint_parameter_desc_t __cint_struct_members__cbx_packet_cb_info_t[] =
{
    {
        "uint32_t",
        "flags",
        0,
        0
    },
    {
        "const char",
        "name",
        1,
        0
    },
    {
        "uint32_t",
        "priority",
        0,
        0
    },
    {
        "void",
        "cookie",
        1,
        0
    },
    CINT_ENTRY_LAST
};

static cint_struct_type_t __cint_fsal_pktio_structures[] =
{
    {
    "cbx_pktio_params_t",
    sizeof(cbx_pktio_params_t),
    __cint_struct_members__cbx_pktio_params_t,
    __cint_maddr__cbx_pktio_params_t
    },
    {
    "cbx_packet_attr_t",
    sizeof(cbx_packet_attr_t),
    __cint_struct_members__cbx_packet_attr_t,
    __cint_maddr__cbx_packet_attr_t
    },
    {
    "cbx_packet_cb_info_t",
    sizeof(cbx_packet_cb_info_t),
    __cint_struct_members__cbx_packet_cb_info_t,
    __cint_maddr__cbx_packet_cb_info_t
    },
    CINT_ENTRY_LAST
};


static cint_enum_type_t __cint_fsal_pktio_enums[] =
{
    CINT_ENTRY_LAST
};


static cint_parameter_desc_t __cint_fsal_pktio_typedefs[] =
{
    {
        "uint32_t",
        "cbx_trapid_t",
        0,
        0
    },
    {
        "void",
        "cbx_pktio_rx_cb_handle",
        1,
        0
    },
    CINT_ENTRY_LAST
};


static cint_constants_t __cint_fsal_pktio_constants[] =
{
   { "CBX_PACKET_RX_UCAST", CBX_PACKET_RX_UCAST },
   { "CBX_PACKET_RX_MCAST", CBX_PACKET_RX_MCAST },
   { "CBX_PACKET_RX_DLF", CBX_PACKET_RX_DLF },
   { "CBX_PACKET_RX_LEARN", CBX_PACKET_RX_LEARN },
   { "CBX_PACKET_RX_MIRROR", CBX_PACKET_RX_MIRROR },
   { "CBX_PACKET_RX_TRAP", CBX_PACKET_RX_TRAP },
   { "CBX_PACKET_RX_LIN", CBX_PACKET_RX_LIN },
   { "CBX_PACKET_TX_FWD_RAW", CBX_PACKET_TX_FWD_RAW },
   { "CBX_PACKET_TX_FWD_UCAST", CBX_PACKET_TX_FWD_UCAST },
   { "CBX_PACKET_TX_FWD_MCAST", CBX_PACKET_TX_FWD_MCAST },
   { "CBX_PACKET_TX_FWD_SWITCH", CBX_PACKET_TX_FWD_SWITCH },
   { "CBX_PKTIO_CALLBACK_ALL", CBX_PKTIO_CALLBACK_ALL },
   { "CBX_PKTIO_CALLBACK_TRAP", CBX_PKTIO_CALLBACK_TRAP },
   { "CBX_PKTIO_CALLBACK_LLC", CBX_PKTIO_CALLBACK_LLC },
   { "CBX_PKTIO_CALLBACK_TCP", CBX_PKTIO_CALLBACK_TCP },
   { "CBX_PKTIO_CALLBACK_UDP", CBX_PKTIO_CALLBACK_UDP },
   { "CBX_RX_NOT_HANDLED", CBX_RX_NOT_HANDLED },
   { "CBX_RX_HANDLED", CBX_RX_HANDLED },
    CINT_ENTRY_LAST
};

static cint_function_pointer_t __cint_function_pointers[2];
static int
__cint_fpointer__pktio_cb(void* buffer, int* buffer_size, cbx_packet_attr_t* packet_attr , cbx_packet_cb_info_t* cb_info)
{
  int returnval;

  cint_interpreter_callback(__cint_function_pointers+0, 4, 1, &buffer, &buffer_size, &packet_attr, &cb_info, &returnval);

  return returnval;
}
static cint_parameter_desc_t __cint_parameters_pktio_cb[] =
{
        {
            "int",
            "r",
            0,
            0
        },
    {
        "void",
        "buffer",
        1,
        0
    },
    {
        "int",
        "buffer_size",
        1,
        0
    },
    {
        "cbx_packet_attr_t",
        "packet_attr",
        1,
        0
    },
    {
        "cbx_packet_cb_info_t",
        "cb_info",
        1,
        0
    },
    CINT_ENTRY_LAST
};

static cint_function_pointer_t __cint_fsal_pktio_function_pointers[] =
{
    {
        "cbx_packet_io_callback_t",
        (cint_fpointer_t) __cint_fpointer__pktio_cb,
        __cint_parameters_pktio_cb,
    },
    CINT_ENTRY_LAST
};

static cint_function_t __cint_fsal_pktio_functions[] =
    {
        CINT_FWRAPPER_ENTRY(cbx_packet_attr_t_init),
        CINT_FWRAPPER_ENTRY(cbx_pktio_init),
        CINT_FWRAPPER_ENTRY(cbx_pktio_detach),
        CINT_FWRAPPER_ENTRY(cbx_pktio_register),
        CINT_FWRAPPER_ENTRY(cbx_pktio_unregister),
        CINT_FWRAPPER_ENTRY(cbx_pktio_packet_send),
        CINT_FWRAPPER_ENTRY(dump_pktio_stats),
#ifdef CONFIG_PACKET_DEBUG
        CINT_FWRAPPER_ENTRY(set_pkt_dump),
#endif /* CONFIG_PACKET_DEBUG */
        CINT_ENTRY_LAST
    };

cint_data_t fsal_pktio_cint_data =
    {
        NULL,
        __cint_fsal_pktio_functions,
        __cint_fsal_pktio_structures,
        __cint_fsal_pktio_enums,
        __cint_fsal_pktio_typedefs,
        __cint_fsal_pktio_constants,
        __cint_fsal_pktio_function_pointers
    };

#endif /* INCLUDE_LIB_CINT */


