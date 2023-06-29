/*
 * $Copyright: (c) 2017 Broadcom Corp.
 * All Rights Reserved$
 *
 * File:
 *    main.c
 */

#include "bslenable.h"
#include <fsal/cfp.h>
#include <fsal_int/init.h>
#include <fsal_int/types.h>
#include <osal/sal_time.h>

#include <soc/drv.h>
#include <string.h>
#ifdef CONFIG_USE_UART_AS_CPU_INTF
#include <soc/robo2/bcm53158/uart_spi.h>
#else
#include <soc/robo2/bcm53158/spi.h>
#endif

extern void cint_thread(void);

#define SERVER_BUF_SIZE 256
#define SERVER_CMD_SIZE 1024
#define AUTOCMD_ARGC 5
#define AUTOCMD_ARGV_SIZE 30
#define BCM_REQ_FILE "/tmp/bcm.req"
#define BCM_RES_FILE "/tmp/bcm.res"
#define BCM_RES_LOG_FILE "/tmp/bcm.res.log"
#define SAL_PRINT_LOG_FILE "/var/log/rsdk.log"
#define LSW_INIT_ERROR_FILE "/var/log/lsw-init-error.log"

typedef struct client_params_t {
    int argc;
    char* argv[];
} client_params_t;

void arguments_option(void)
{
    sal_printf("Valid arguments for rsdk option are:\n");
    sal_printf("    init                      Bcm51358 init server.\n");
    sal_printf("    cint                      Enter the cint command manually.\n");
    sal_printf("    vlan_get [vlanid]\n");
    sal_printf("    vlan_add [vlanid]\n");
    sal_printf("    vlan_port_get [vlanid]\n");
    sal_printf("    vlan_port_add [vlanid] [portid] [mode]\n");
    sal_printf("    vlan_port_remove [vlanid] [portid]\n");
    sal_printf("    port_link_status_get [portid]\n");
    sal_printf("    port_info_get [portid]\n");
    sal_printf("    port_info_set [portid] [porttype]\n");
    sal_printf("    vlan_port_pvid_set [vlanid] [portid]\n");
    sal_printf("    port_stat_get [portid] [type]\n");
}

void clean_log(char* params_file)
{
    char cmdstr[SERVER_CMD_SIZE] = { 0 };
    sprintf(cmdstr, "echo > %s", SAL_PRINT_LOG_FILE);
    system(cmdstr);
    sprintf(cmdstr, "echo > %s", BCM_REQ_FILE);
    system(cmdstr);
    sprintf(cmdstr, "echo > %s", BCM_RES_FILE);
    system(cmdstr);
    sprintf(cmdstr, "echo > %s", BCM_RES_LOG_FILE);
    system(cmdstr);
}

static int isdigitstr(char* str)
{
    return (strspn(str, "0123456789") == strlen(str));
}

void read_file(char* filepath)
{
    char line[SERVER_BUF_SIZE];
    FILE* fp = fopen(filepath, "r");
    if (fp != NULL) {
        while (fgets(line, sizeof(line), fp) != NULL) {
            if (line[strlen(line) - 1] == '\n' || line[strlen(line) - 1] == '\r') {
                line[strlen(line) - 1] = '\0';
            }
            sal_printf("%s\n", line);
        }
        fclose(fp);
    } else {
        sal_printf("can't open file:%s\n", filepath);
    }
}

int bcm_init(void)
{
    int rv = 0;
    int init_error_count = 0;
    char cmdstr[SERVER_CMD_SIZE] = { 0 };
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
        if (rv != 0) {
            init_error_count = 1;
            sal_printf("soc_robo2_essential_init(CBX_AVENGER_PRIMARY) fail: rv=%d\n", rv);
            sprintf(cmdstr, "echo %d 0 >> %s", init_error_count, LSW_INIT_ERROR_FILE);
            system(cmdstr);
            return rv;
        }
        if (rv == SOC_E_NONE) {
            if (SOC_IS_CASCADED(0)) {
                rv = soc_robo2_essential_init(CBX_AVENGER_SECONDARY);
                if (rv != 0) {
                    init_error_count = 1;
                    sal_printf("soc_robo2_essential_init(CBX_AVENGER_SECONDARY) fail: rv=%d\n", rv);
                    sprintf(cmdstr, "echo %d 0 >> %s", init_error_count, LSW_INIT_ERROR_FILE);
                    system(cmdstr);
                    return rv;
                }
            }
        }
        if (rv == SOC_E_NONE) {
            bslenable_init();
            rv = cbx_fsal_init();
            if (rv != 0) {
                init_error_count = 1;
                sal_printf("cbx_fsal_init() fail: rv=%d\n", rv);
                sprintf(cmdstr, "echo %d 0 >> %s", init_error_count, LSW_INIT_ERROR_FILE);
                system(cmdstr);
                return rv;
            }
        }
    }
    sprintf(cmdstr, "echo %d 0 >> %s", init_error_count, LSW_INIT_ERROR_FILE);
    system(cmdstr);
    return rv;
}

void get_request_params(char* params_file, client_params_t* params)
{
    int idx = 0;
    FILE* fp = NULL;
    char words[AUTOCMD_ARGC][AUTOCMD_ARGV_SIZE];
    fp = fopen(params_file, "r");
    if (fp != NULL) {
        for (idx = 0; idx < AUTOCMD_ARGC; idx++) {
            if (fscanf(fp, "%20s", words[idx]) != 1)
                break;
        }
        fclose(fp);
        params->argc = atoi(words[0]);
        for (idx = 0; idx < params->argc; idx++) {
            params->argv[idx] = (char*)malloc(AUTOCMD_ARGV_SIZE);
            memcpy(params->argv[idx], words[idx + 1], AUTOCMD_ARGV_SIZE);
        }
    } else {
        params->argc = 0;
    }
}

void bcm_server(void)
{
    int rv = 0;
    uint32_t idx = 0;
    char cmdstr[SERVER_CMD_SIZE] = { 0 };
    client_params_t params;
    uint32_t vlanid;
    uint32_t portid;
    uint32_t porttype;
    uint32_t mode;
    uint32_t type;
    while (1) {
        sal_sleep(2);
        get_request_params(BCM_REQ_FILE, &params);
        if (params.argc == 0) {
            continue;
        }
        for (idx = 0; idx < params.argc; idx++) {
            sal_printf("argv[%d]=%s ", idx, params.argv[idx]);
        }
        sal_printf("\n");
        if (strcmp(params.argv[0], "vlan_add") == 0) {
            sal_printf("method:vlan_add\n");
            if ((params.argc == 2) && isdigitstr(params.argv[1])) {
                vlanid = (uint32_t)(atoi(params.argv[1]));
                rv = bcm_cbx_vlan_create(vlanid);
            } else {
                rv = SOC_E_PARAM;
            }
        } else if (strcmp(params.argv[0], "vlan_get") == 0) {
            sal_printf("method:vlan_get\n");
            if ((params.argc == 2) && isdigitstr(params.argv[1])) {
                vlanid = (uint32_t)(atoi(params.argv[1]));
                rv = bcm_cbx_vlan_get(vlanid);
            } else {
                rv = SOC_E_PARAM;
            }
        } else if (strcmp(params.argv[0], "vlan_port_get") == 0) {
            sal_printf("method:vlan_port_get\n");
            if ((params.argc == 2) && isdigitstr(params.argv[1])) {
                vlanid = (uint32_t)(atoi(params.argv[1]));
                rv = bcm_cbx_vlan_port_get(vlanid);
            } else {
                rv = SOC_E_PARAM;
            }
        } else if (strcmp(params.argv[0], "vlan_port_add") == 0) {
            sal_printf("method:vlan_port_add\n");
            if ((params.argc == 4) && isdigitstr(params.argv[1]) && isdigitstr(params.argv[2]) &&
                isdigitstr(params.argv[3])) {
                vlanid = (uint32_t)(atoi(params.argv[1]));
                portid = (uint32_t)(atoi(params.argv[2]));
                mode = (uint32_t)(atoi(params.argv[3]));
                rv = bcm_cbx_vlan_port_add(vlanid, portid, mode);
            } else {
                rv = SOC_E_PARAM;
            }
        } else if (strcmp(params.argv[0], "vlan_port_remove") == 0) {
            sal_printf("method:vlan_port_remove\n");
            if ((params.argc == 3) && isdigitstr(params.argv[1]) && isdigitstr(params.argv[2])) {
                vlanid = (uint32_t)(atoi(params.argv[1]));
                portid = (uint32_t)(atoi(params.argv[2]));
                rv = bcm_cbx_vlan_port_remove(vlanid, portid);
            } else {
                rv = SOC_E_PARAM;
            }
        } else if (strcmp(params.argv[0], "port_link_status_get") == 0) {
            sal_printf("method:port_link_status_get\n");
            if ((params.argc == 2) && isdigitstr(params.argv[1])) {
                rv = bcm_port_link_status_get(atoi(params.argv[1]));
            } else {
                rv = SOC_E_PARAM;
            }
        } else if (strcmp(params.argv[0], "port_info_get") == 0) {
            sal_printf("method:port_info_get\n");
            if ((params.argc == 2) && isdigitstr(params.argv[1])) {
                rv = bcm_port_info_get(atoi(params.argv[1]));
            } else {
                rv = SOC_E_PARAM;
            }
        } else if (strcmp(params.argv[0], "port_info_set") == 0) {
            sal_printf("method:port_info_set\n");
            if ((params.argc == 3) && isdigitstr(params.argv[1]) && isdigitstr(params.argv[2])) {
                portid = (uint32_t)(atoi(params.argv[1]));
                porttype = (uint32_t)(atoi(params.argv[2]));
                rv = bcm_port_info_set(portid, porttype);
            } else {
                rv = SOC_E_PARAM;
            }
        } else if (strcmp(params.argv[0], "vlan_port_pvid_set") == 0) {
            sal_printf("method:vlan_port_pvid_set\n");
            if ((params.argc == 3) && isdigitstr(params.argv[1]) && isdigitstr(params.argv[2])) {
                vlanid = (uint32_t)(atoi(params.argv[1]));
                portid = (uint32_t)(atoi(params.argv[2]));
                rv = bcm_cbx_vlan_port_pvid_set(vlanid, portid);
            } else {
                rv = SOC_E_PARAM;
            }
        } else if (strcmp(params.argv[0], "port_stat_get") == 0) {
            sal_printf("method:port_stat_get\n");
            if ((params.argc == 3) && isdigitstr(params.argv[1]) && isdigitstr(params.argv[2])) {
                portid = (uint32_t)(atoi(params.argv[1]));
                type = (uint32_t)(atoi(params.argv[2]));
                rv = bcm_cbx_port_stat_get(portid, type);
            } else {
                rv = SOC_E_PARAM;
            }
        } else {
            rv = SOC_E_PARAM;
        }
        sprintf(cmdstr, "echo 1 %d > %s", rv, BCM_RES_FILE);
        system(cmdstr);
        sprintf(cmdstr, "echo > %s", BCM_REQ_FILE);
        system(cmdstr);
    }
}

int bcm_client(int argc, char** argv)
{
    int rv = 0;
    uint32_t idx = 0;
    uint32_t try_times = 0;
    char buf[SERVER_BUF_SIZE] = { 0 };
    char cmdstr[SERVER_CMD_SIZE] = { 0 };
    client_params_t req_params;
    client_params_t res_params;
    int wait = 1;
    while (wait) {
        get_request_params(BCM_REQ_FILE, &req_params);
        if (req_params.argc != 0) {
            sal_sleep(2);
            try_times += 1;
        } else {
            wait = 0;
        }
        if (try_times > 10) {
            return SOC_E_BUSY;
        }
    }
    sprintf(cmdstr, "echo > %s", BCM_RES_FILE);
    system(cmdstr);
    for (idx = 0; idx < argc; idx++) {
        if (idx == 0) {
            sprintf(buf, "%d", argc - 1);
            continue;
        }
        strcat(buf, " ");
        strcat(buf, argv[idx]);
    }
    sprintf(cmdstr, "echo \"%s\" > %s", buf, BCM_REQ_FILE);
    system(cmdstr);
    try_times = 0;
    while (try_times < 10) {
        get_request_params(BCM_RES_FILE, &res_params);
        if (res_params.argc == 0) {
            try_times += 1;
            sal_printf("try_times = %d\n", try_times);
            sal_sleep(2);
        } else {
            sprintf(cmdstr, "echo > %s", BCM_RES_FILE);
            system(cmdstr);
            rv = atoi(res_params.argv[0]);
            read_file(BCM_RES_LOG_FILE);
            sprintf(cmdstr, "echo > %s", BCM_RES_LOG_FILE);
            system(cmdstr);
            return rv;
        }
    }
    sal_printf("Initialization is incomplete or blocked. Please check and try again.\n");
    return SOC_E_TIMEOUT;
}

int main(int argc, char** argv)
{
    if (argc == 1) {
        arguments_option();
        return 0;
    }
    int rv = 0;
    if (strcmp(argv[1], "cint") == 0) {
        rv = bcm_init();
        if (rv != 0) {
            sal_printf("bcm_init: rv=%d\n", rv);
            return rv;
        }
#ifdef INCLUDE_CINT
        cint_thread();
#endif
    } else if (strcmp(argv[1], "init") == 0) {
        clean_log(SAL_PRINT_LOG_FILE);
        sal_printf("bcm server is initing,serverstate=0\n");
        rv = bcm_init();
        if (rv != 0) {
            sal_printf("bcm_init fail: rv=%d\n", rv);
            return rv;
        }
        sal_printf("bcm server is running,serverstate=1\n");
        bcm_server();
#ifdef CONFIG_USE_UART_AS_CPU_INTF
        soc_uart_spi_terminate(0);
#else
        soc_spi_terminate(0);
#endif
    } else {
        rv = bcm_client(argc, argv);
        sal_printf("%s: rv=%d\n", argv[1], rv);
        if (rv == SOC_E_PARAM) {
            arguments_option();
        }
    }
    return rv;
}
