/*
 *
 * $Copyright: (c) 2017 Broadcom Corporation
 * All Rights Reserved.$
 *
 * reg_debug_cint_data.c
 *
 * Hand-coded support for FSAL routines.
 */
int reg_debug_cint_data_not_empty;

#if defined(INCLUDE_LIB_CINT)
#include <sal_types.h>
#include <types.h>
#include <cint_config.h>
#include <cint_types.h>
#include <cint_porting.h>
#include <drv_if.h>
#include <soc/robo2/common/tables.h>


extern int dump_table(soc_robo2_table_id_t tid, int idx, int len);
CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         xlreg_read,
                         int, int, unit, 0, 0,
                         int, int, port, 0, 0,
                         uint32, uint32, addr, 0, 0);

CINT_FWRAPPER_CREATE_RP5(int, int, 0, 0,
                         xlreg_write,
                         int, int, unit, 0, 0,
                         int, int, port, 0, 0,
                         uint32, uint32, addr, 0, 0,
                         uint32, uint32, valuelo, 0, 0,
                         uint32, uint32, valuehi, 0, 0);

CINT_FWRAPPER_CREATE_RP5(int, int, 0, 0,
                         drv_robo2_apb2pbus_reg_read,
                         int, int, unit, 0, 0,
                         int, int, port, 0, 0,
                         uint32, uint32, addr, 0, 0,
                         void *, void, data, 1, 0,
                         int, int, len, 0, 0);

CINT_FWRAPPER_CREATE_RP5(int, int, 0, 0,
                         drv_robo2_apb2pbus_reg_write,
                         int, int, unit, 0, 0,
                         int, int, port, 0, 0,
                         uint32, uint32, addr, 0, 0,
                         void *, void, data, 1, 0,
                         int, int, len, 0, 0);

CINT_FWRAPPER_CREATE_RP4(int, int, 0, 0,
                         drv_robo2_reg_read,
                         int, int, unit, 0, 0,
                         uint32, uint32, addr, 0, 0,
                         void *, void, data, 1, 0,
                         int, int, len, 0, 0);

CINT_FWRAPPER_CREATE_RP4(int, int, 0, 0,
                         drv_robo2_reg_write,
                         int, int, unit, 0, 0,
                         uint32, uint32, addr, 0, 0,
                         void *, void, data, 1, 0,
                         int, int, len, 0, 0);

CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         reg_read,
                         int, int, unit, 0, 0,
                         uint32, uint32, addr, 0, 0);

CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         reg_write,
                         int, int, unit, 0, 0,
                         uint32, uint32, addr, 0, 0,
                         int, int, value, 0, 0);

CINT_FWRAPPER_CREATE_RP3(int, int, 0, 0,
                         dump_table,
                         soc_robo2_table_id_t, soc_robo2_table_id_t, tid, 0, 0,
                         int, int, idx, 0, 0,
                         int, int, len, 0, 0);


CINT_FWRAPPER_CREATE_RP2(int, int, 0, 0,
                         reg_block_read,
                         int, int, unit, 0, 0,
                         uint32, uint32, block, 0, 0);

static cint_enum_map_t __cint_enum_map__soc_robo2_table_id_t[] =
{
    {"tct",    ROBO2_TABLE_TCT_ID                 },
    {"rpt",    ROBO2_TABLE_RPT_ID                 },
    {"pgt",    ROBO2_TABLE_PGT_ID                 },
    {"pmipage",    ROBO2_TABLE_PMIPAGE_ID             },
    {"gfcd_config",    ROBO2_TABLE_GFCD_CONFIG_ID         },
    {"apt",    ROBO2_TABLE_APT_ID                 },
    {"gfcd_cntr",    ROBO2_TABLE_GFCD_CNTR_ID           },
    {"spg_pp_bmp_config",    ROBO2_TABLE_SPG_PP_BMP_CONFIG_ID   },
    {"dgt",    ROBO2_TABLE_DGT_ID                 },
    {"gspg2ig",    ROBO2_TABLE_GSPG2IG_ID             },
    {"epp_calendar",    ROBO2_TABLE_EPP_CALENDAR_ID        },
    {"ght",    ROBO2_TABLE_GHT_ID        },
    {"lin2vsi",    ROBO2_TABLE_LIN2VSI_ID             },
    {"cfp_profile",    ROBO2_TABLE_CFP_PROFILE_ID         },
    {"cfp_bucket",    ROBO2_TABLE_CFP_BUCKET_ID          },
    {"calendar",    ROBO2_TABLE_CALENDAR_ID            },
    {"pg2lpg",    ROBO2_TABLE_PG2LPG_ID              },
    {"arlfm",    ROBO2_TABLE_ARLFM_ID              },
    {"arlfm0",    ROBO2_TABLE_ARLFM0_ID              },
    {"ipp_bucket",    ROBO2_TABLE_IPP_BUCKET_ID          },
    {"fcd_config",    ROBO2_TABLE_FCD_CONFIG_ID          },
    {"stat",    ROBO2_TABLE_STAT_ID                },
    {"eeecfg",    ROBO2_TABLE_EEECFG_ID              },
    {"hdt",    ROBO2_TABLE_HDT_ID                 },
    {"estget",    ROBO2_TABLE_ESTGET_ID              },
    {"stt",    ROBO2_TABLE_STT_ID                 },
    {"pet",    ROBO2_TABLE_PET_ID                 },
    {"istget",    ROBO2_TABLE_ISTGET_ID              },
    {"qdschcfg",    ROBO2_TABLE_QDSCHCFG_ID            },
    {"pqm_rscale",    ROBO2_TABLE_PQM_RSCALE_ID          },
    {"mtr2tcb",    ROBO2_TABLE_MTR2TCB_ID             },
    {"dli2ldpg",    ROBO2_TABLE_DLI2LDPG_ID            },
    {"tc2qd",    ROBO2_TABLE_TC2QD_ID               },
    {"spldt",    ROBO2_TABLE_SPLDT_ID               },
    {"cfp_rscale",    ROBO2_TABLE_CFP_RSCALE_ID          },
    {"ghtdt",    ROBO2_TABLE_GHTDT_ID               },
    {"bmu_bucket",    ROBO2_TABLE_BMU_BUCKET_ID          },
    {"lilt",    ROBO2_TABLE_LILT_ID                },
    {"svt",    ROBO2_TABLE_SVT_ID                 },
    {"slicmap",    ROBO2_TABLE_SLICMAP_ID             },
    {"m2tk",    ROBO2_TABLE_M2TK_ID                },
    {"art",    ROBO2_TABLE_ART_ID                 },
    {"lpg2ig",    ROBO2_TABLE_LPG2IG_ID              },
    {"ipp_profile",    ROBO2_TABLE_IPP_PROFILE_ID         },
    {"qfcqdaddr",    ROBO2_TABLE_QFCQDADDR_ID           },
    {"mtgt",    ROBO2_TABLE_MTGT_ID                },
    {"ipp_pptr",    ROBO2_TABLE_IPP_PPTR_ID            },
    {"set",    ROBO2_TABLE_SET_ID                 },
    {"srt",    ROBO2_TABLE_SRT_ID                 },
    {"ipp_pmon",    ROBO2_TABLE_IPP_PMON_ID            },
    {"vtct",    ROBO2_TABLE_VTCT_ID                },
    {"cfpcam",    ROBO2_TABLE_CFPCAM_ID              },
    {"fcd_cntr",    ROBO2_TABLE_FCD_CNTR_ID            },
    {"fpslict",    ROBO2_TABLE_FPSLICT_ID             },
    {"kst",    ROBO2_TABLE_KST_ID                 },
    {"pqm_bucket",    ROBO2_TABLE_PQM_BUCKET_ID          },
    {"ghst",    ROBO2_TABLE_GHST_ID                },
    {"lpg2pg",    ROBO2_TABLE_LPG2PG_ID              },
    {"epgt",    ROBO2_TABLE_EPGT_ID                },
    {"bmu_rscale",    ROBO2_TABLE_BMU_RSCALE_ID          },
    {"lpg2ppfov",    ROBO2_TABLE_LPG2PPFOV_ID           },
    {"vsit",    ROBO2_TABLE_VSIT_ID                },
    {"qfccredit",    ROBO2_TABLE_QFCCREDIT_ID           },
    {"tet",    ROBO2_TABLE_TET_ID                 },
    {"ipp_pp2lpg",    ROBO2_TABLE_IPP_PP2LPG_ID          },
    {"pqm_pptr",    ROBO2_TABLE_PQM_PPTR_ID            },
    {"bmu_pmon",    ROBO2_TABLE_BMU_PMON_ID            },
    {"etct",    ROBO2_TABLE_ETCT_ID                },
    {"arlfm1_ght_h0",    ROBO2_TABLE_ARLFM1_GHT_H0_ID       },
    {"cfp_pmon",    ROBO2_TABLE_CFP_PMON_ID            },
    {"max_config",    ROBO2_TABLE_MAX_CONFIG_ID          },
    {"bmu_profile",    ROBO2_TABLE_BMU_PROFILE_ID         },
    {"pv2li",    ROBO2_TABLE_PV2LI_ID               },
    {"ipp_rscale",    ROBO2_TABLE_IPP_RSCALE_ID          },
    {"pqm_pmon",    ROBO2_TABLE_PQM_PMON_ID            },
    {"ikft",    ROBO2_TABLE_IKFT_ID                },
    {"arlfm1_ght_h1",    ROBO2_TABLE_ARLFM1_GHT_H1_ID       },
    {"epmt",    ROBO2_TABLE_EPMT_ID                },
    {"epp_bmu_tx_reorder_req_lmt",    ROBO2_TABLE_EPP_BMU_TX_REORDER_REQ_LMT_ID},
    {"bmu_pptr",    ROBO2_TABLE_BMU_PPTR_ID            },
    {"ert",    ROBO2_TABLE_ERT_ID                 },
    {"action",    ROBO2_TABLE_ACTION_ID              },
    {"rpslict",    ROBO2_TABLE_RPSLICT_ID             },
    {"slictecc",    ROBO2_TABLE_SLICTECC_ID            },
    {"litet",    ROBO2_TABLE_LITET_ID               },
    {"tecc",    ROBO2_TABLE_TECC_ID                },
    {"cfp_pptr",    ROBO2_TABLE_CFP_PPTR_ID            },
    {"wrt",    ROBO2_TABLE_WRT_ID                 },
    {"wpt",    ROBO2_TABLE_WPT_ID                 },
    {"dliet",    ROBO2_TABLE_DLIET_ID               },
    {"pglct",    ROBO2_TABLE_PGLCT_ID               },
    {"lpg2ap",    ROBO2_TABLE_LPG2AP_ID              },
    {"pqm_profile",    ROBO2_TABLE_PQM_PROFILE_ID         },
    {"cpmt",    ROBO2_TABLE_CPMT_ID                },
    {"pqm_pp2lpg",    ROBO2_TABLE_PQM_PP2LPG_ID          },
    {"ofarlfm",    ROBO2_TABLE_OFARLFM_ID                },
    CINT_ENTRY_LAST
};

static cint_struct_type_t __cint_reg_debug_structures[] =
{
    CINT_ENTRY_LAST
};


static cint_enum_type_t __cint_reg_debug_enums[] =
{
    { "soc_robo2_table_id_t", __cint_enum_map__soc_robo2_table_id_t },
    CINT_ENTRY_LAST
};


static cint_parameter_desc_t __cint_reg_debug_typedefs[] =
{
    CINT_ENTRY_LAST
};


static cint_constants_t __cint_reg_debug_constants[] =
{
    CINT_ENTRY_LAST
};

static cint_function_pointer_t __cint_reg_debug_function_pointers[] =
{
    CINT_ENTRY_LAST
};

static cint_function_t __cint_reg_debug_functions[] =
    {
        CINT_FWRAPPER_ENTRY(drv_robo2_apb2pbus_reg_read),
        CINT_FWRAPPER_ENTRY(drv_robo2_apb2pbus_reg_write),
        CINT_FWRAPPER_ENTRY(xlreg_read),
        CINT_FWRAPPER_ENTRY(xlreg_write),
        CINT_FWRAPPER_ENTRY(drv_robo2_reg_read),
        CINT_FWRAPPER_ENTRY(drv_robo2_reg_write),
        CINT_FWRAPPER_ENTRY(reg_read),
        CINT_FWRAPPER_ENTRY(reg_write),
        CINT_FWRAPPER_ENTRY(dump_table),
        CINT_FWRAPPER_ENTRY(reg_block_read),
        CINT_ENTRY_LAST
    };

cint_data_t reg_debug_cint_data =
    {
        NULL,
        __cint_reg_debug_functions,
        __cint_reg_debug_structures,
        __cint_reg_debug_enums,
        __cint_reg_debug_typedefs,
        __cint_reg_debug_constants,
        __cint_reg_debug_function_pointers
    };

#endif /* INCLUDE_LIB_CINT */
