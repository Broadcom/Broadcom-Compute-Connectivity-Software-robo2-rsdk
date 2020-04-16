/*
 * $Id: cint_cmd_main.c $
 *
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved$
 *
 *
 */
#include <sal_console.h>
#include <sal_task.h>
#ifdef OPENRTOS
#include <FreeRTOS.h>
#endif

int cmd_cint_c_not_empty;


#ifdef INCLUDE_LIB_CINT
#include "cint_cmd_main.h"
/* fsal_stat_cint_data.c */
extern cint_data_t fsal_stat_cint_data;

/* fsal_vlan_cint_data.c */
extern cint_data_t fsal_vlan_cint_data;

/* fsal_port_cint_data.c */
extern cint_data_t fsal_port_cint_data;

#ifdef CONFIG_TIMESYNC
/* fsal_ts_cint_data.c */
extern cint_data_t fsal_ts_cint_data;
extern cint_data_t fsal_ptp_cint_data;
#endif

/* fsal_stg_cint_data.c */
extern cint_data_t fsal_stg_cint_data;

/* fsal_lag_cint_data.c */
extern cint_data_t fsal_lag_cint_data;

/* fsal_mirror_cint_data.c */
extern cint_data_t fsal_mirror_cint_data;

/* fsal_l2_cint_data.c */
extern cint_data_t fsal_l2_cint_data;

/* fsal_meter_cint_data.c */
extern cint_data_t fsal_meter_cint_data;

/* fsal_mcast_cint_data.c */
extern cint_data_t fsal_mcast_cint_data;

/* fsal_cosq_cint_data.c */
extern cint_data_t fsal_cosq_cint_data;

/* fsal_auth_cint_data.c */
extern cint_data_t fsal_auth_cint_data;

/* fsal_link_cint_data.c */
extern cint_data_t fsal_link_cint_data;

/* fsal_pktio_cint_data.c */
extern cint_data_t fsal_pktio_cint_data;

/* fsal_pktio_cint_data.c */
extern cint_data_t fsal_cfp_cint_data;

/* fsal_cable_diag_cint_data.c */
extern cint_data_t fsal_cable_diag_cint_data;

/* sal_cint_data.c */
extern cint_data_t soc_cint_data;

/* osal_cint_data.c */
extern cint_data_t osal_cint_data;

/* reg_debug_cint_data.c */
extern cint_data_t reg_debug_cint_data;

/* osal_fs_cint_data.c */
#ifdef CONFIG_FS
    extern cint_data_t osal_fs_cint_data;
#endif

/* web_server_cint_data.c */
#ifdef CONFIG_WEB_SERVER
    extern cint_data_t web_server_cint_data;
#endif

#ifdef CONFIG_PORT_EXTENDER
/* fsal_extender_cint_data.c */
extern cint_data_t fsal_extender_cint_data;
#endif

#ifdef CONFIG_VIRTUAL_PORT_SUPPORT
/* fsal_lif_cint_data.c */
extern cint_data_t fsal_lif_cint_data;
#endif

#ifdef CONFIG_DOT1AS
/* dot1as_cint_data.c */
extern cint_data_t dot1as_cint_data;
#endif

static int
__cint_avenger_data_init(void)
{
    int rc = 0;
    rc = cint_interpreter_add_data(&fsal_stat_cint_data, NULL);
    if (rc != CINT_E_NONE) { sal_printf("Failed adding stat data (%d)\n", rc); return rc; }
    rc = cint_interpreter_add_data(&fsal_lag_cint_data, NULL);
    if (rc != CINT_E_NONE) { sal_printf("Failed adding lag data (%d)\n", rc); return rc; }
    rc = cint_interpreter_add_data(&fsal_vlan_cint_data, NULL);
    if (rc != CINT_E_NONE) { sal_printf("Failed adding vlan data (%d)\n", rc); return rc; }
    rc = cint_interpreter_add_data(&fsal_port_cint_data, NULL);
    if (rc != CINT_E_NONE) { sal_printf("Failed adding port data (%d)\n", rc); return rc; }
#ifdef CONFIG_TIMESYNC
    rc = cint_interpreter_add_data(&fsal_ts_cint_data, NULL);
    if (rc != CINT_E_NONE) { sal_printf("Failed adding ts data (%d)\n", rc); return rc; }
#endif
#ifdef CONFIG_PTP
    rc = cint_interpreter_add_data(&fsal_ptp_cint_data, NULL);
    if (rc != CINT_E_NONE) { sal_printf("Failed adding PTP data (%d)\n", rc); return rc; }
#endif
    rc = cint_interpreter_add_data(&fsal_stg_cint_data, NULL);
    if (rc != CINT_E_NONE) { sal_printf("Failed adding stg data (%d)\n", rc); return rc; }
    rc = cint_interpreter_add_data(&fsal_mirror_cint_data, NULL);
    if (rc != CINT_E_NONE) { sal_printf("Failed adding mirror data (%d)\n", rc); return rc; }
    rc = cint_interpreter_add_data(&fsal_l2_cint_data, NULL);
    if (rc != CINT_E_NONE) { sal_printf("Failed adding l2 data (%d)\n", rc); return rc; }
    rc = cint_interpreter_add_data(&fsal_meter_cint_data, NULL);
    if (rc != CINT_E_NONE) { sal_printf("Failed adding meter data (%d)\n", rc); return rc; }
    rc = cint_interpreter_add_data(&fsal_mcast_cint_data, NULL);
    if (rc != CINT_E_NONE) { sal_printf("Failed adding mcast data (%d)\n", rc); return rc; }
    rc = cint_interpreter_add_data(&fsal_cosq_cint_data, NULL);
    if (rc != CINT_E_NONE) { sal_printf("Failed adding cosq data (%d)\n", rc); return rc; }
    rc = cint_interpreter_add_data(&fsal_auth_cint_data, NULL);
    if (rc != CINT_E_NONE) { sal_printf("Failed adding authentication data (%d)\n", rc); return rc; }
    rc = cint_interpreter_add_data(&fsal_link_cint_data, NULL);
    if (rc != CINT_E_NONE) { sal_printf("Failed adding link data (%d)\n", rc); return rc; }
    rc = cint_interpreter_add_data(&fsal_pktio_cint_data, NULL);
    if (rc != CINT_E_NONE) { sal_printf("Failed adding pktio data (%d)\n", rc); return rc; }
    rc = cint_interpreter_add_data(&fsal_cfp_cint_data, NULL);
    if (rc != CINT_E_NONE) { sal_printf("Failed adding cfp data (%d)\n", rc); return rc; }
    rc = cint_interpreter_add_data(&fsal_cable_diag_cint_data, NULL);
    if (rc != CINT_E_NONE) { sal_printf("Failed adding cable diag data (%d)\n", rc); return rc; }
    rc = cint_interpreter_add_data(&soc_cint_data, NULL);
    if (rc != CINT_E_NONE) { sal_printf("Failed adding soc data (%d)\n", rc); return rc; }
    rc = cint_interpreter_add_data(&reg_debug_cint_data, NULL);
    if (rc != CINT_E_NONE) { sal_printf("Failed adding debug data (%d)\n", rc); return rc; }
    rc = cint_interpreter_add_data(&osal_cint_data, NULL);
    if (rc != CINT_E_NONE) { sal_printf("Failed adding osal data (%d)\n", rc); return rc; }
#ifdef CONFIG_FS
    rc = cint_interpreter_add_data(&osal_fs_cint_data, NULL);
    if (rc != CINT_E_NONE) { sal_printf("Failed adding fs data (%d)\n", rc); return rc; }
#endif
#ifdef CONFIG_WEB_SERVER
    rc = cint_interpreter_add_data(&web_server_cint_data, NULL);
    if (rc != CINT_E_NONE) { sal_printf("Failed adding web server (%d)\n", rc); return rc; }
#endif
#ifdef CONFIG_PORT_EXTENDER
    rc = cint_interpreter_add_data(&fsal_extender_cint_data, NULL);
    if (rc != CINT_E_NONE) { sal_printf("Failed adding port extender data (%d)\n", rc); return rc; }
#endif
#ifdef CONFIG_VIRTUAL_PORT_SUPPORT
    rc = cint_interpreter_add_data(&fsal_lif_cint_data, NULL);
    if (rc != CINT_E_NONE) { sal_printf("Failed adding port extender data (%d)\n", rc); return rc; }
#endif
#ifdef CONFIG_DOT1AS
    rc = cint_interpreter_add_data(&dot1as_cint_data, NULL);
    if (rc != CINT_E_NONE) { sal_printf("Failed adding dot1as data (%d)\n", rc); return rc; }
#endif
    return 0;
}

static int
__cint_event_handler(void* cookie, cint_interpreter_event_t event)
{
    switch(event)
        {
        case cintEventReset:
            {
                /*
                 * Interpreter has reset. Re-register our data
                 */
                __cint_avenger_data_init();
                break;
            }
        default:
            {
                /* Nothing */
                break;
            }
        }
    return 0;
}

#if CINT_CONFIG_INCLUDE_XINCLUDE == 1
static void
__cint_init_path(void)
{
    char *path;
#if defined(VXWORKS)
    path = var_get("CINT_INCLUDE_PATH");
#elif !defined(__KERNEL__)
    path = getenv("CINT_INCLUDE_PATH");
#else
    path = NULL;
#endif

    if (path) {
        cint_interpreter_include_set(path);
    }
}
#endif
int
cmd_cint_initialize(void)
{
    static int init = 0;
    if(init == 0) {
        cint_interpreter_init();
        cint_interpreter_event_register(__cint_event_handler, NULL);
#if CINT_CONFIG_INCLUDE_XINCLUDE == 1
        __cint_init_path();
#endif
        __cint_avenger_data_init();
#ifdef INCLUDE_LIB_C_UNIT
        c_unit_initialize(c_unit_context_set);
#endif
        init = 1;
    }
    return 0;
}
typedef struct cint_main_params_t{
    int argc;
    char *argv[];
} cint_main_params_t;

#ifdef DEBUG_CINT_WRAPPER
void cint_main (void *context)
{
    cint_main_params_t *params= (struct cint_main_params_t *)context;
    cmd_cint_initialize();
    if(params->argc == 2) {
#if CINT_CONFIG_FILE_IO == 1
        FILE* fp;
        if((fp = fopen(params->argv[1], "r")) != NULL) {
            sal_printf("opened the file %s",params->argv[1]);
            cint_interpreter_parse(fp, NULL, (params->argc - 2), &params->argv[2]);
            fclose(fp);
        } else {
            sal_printf("\nunable to open the file %s   argc=%d",params->argv[1],params->argc);
        }
#endif
    } else {
        cint_interpreter_parse(NULL, "cint> ", params->argc, params->argv);
    }
#ifdef CONFIG_EXTERNAL_HOST
/*
 * If the CINT is exited, exit from application. It needs to be properly fixed
 * so that all the threads exited properly.
 */
exit(0);
#endif
}
#endif /* DEBUG_CINT_WRAPPER */

sal_thread_t thread;
cint_main_params_t cint_params;

void cint_thread(void)
{
    cint_params.argc = 0;
    cint_params.argv[0] = NULL;

#ifdef OPENRTOS
    thread = sal_thread_create("CINT", 4096,
                                1, cint_main,(void *) &cint_params);
    if (thread == NULL) {
        sal_printf("Failed creating taskone\n");
        return;
    }
    vTaskStartScheduler();
#else
    thread = sal_thread_create("CINT", 8192,
                                 1, cint_main,(void *) &cint_params);
    if (thread == 0) {
        sal_printf("Failed creating cint\n");
        return;
    }
#endif
    /* Should not come here */
    while(1);

}
#endif /* INCLUDE_LIB_CINT */
