/*
 * $Id: auto_qos.h Exp $
 *
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom/Broadcom-Compute-Connectivity-Software-robo2-rsdk/master/Legal/LICENSE file.
 *
 * $Copyright: (c) 2020 Broadcom Inc.
 * All Rights Reserved.$
 *
 */

#ifndef _AUTO_QOS_H_
#define _AUTO_QOS_H_

typedef enum traffic_type_e {
    TrafficTypeDLF       = 0,
    TrafficTypeUnicast   = 1,
    TrafficTypeMulticast = 2,
    TrafficTypeBroadcast = 3,
    TrafficTypeMax       = 4,
} traffic_type_t;

typedef enum profile_id_e {
    Profile_reserved    = 0,
    Profile_100M_DLF    = 1,
    Profile_100M_MCAST  = 2,
    Profile_100M_BCAST  = 3,
    Profile_1G_DLF      = 4,
    Profile_1G_MCAST    = 5,
    Profile_1G_BCAST    = 6,
    Profile_2_5G_DLF    = 7,
    Profile_2_5G_MCAST  = 8,
    Profile_2_5G_BCAST  = 9,
    Profile_5G_DLF      = 10,
    Profile_5G_MCAST    = 11,
    Profile_5G_BCAST    = 12,
    Profile_10G_DLF     = 13,
    Profile_10G_MCAST   = 14,
    Profile_10G_BCAST   = 15,
    Profile_User_Def_1  = 16,
    Profile_User_Def_2  = 17,
    Profile_User_Def_3  = 18,
    Profile_User_Def_4  = 19,
    Profile_User_Def_5  = 20,
    Profile_User_Def_6  = 21,
    Profile_User_Def_7  = 22,
    Profile_User_Def_8  = 23,
    Profile_User_Def_9  = 24,
    Profile_User_Def_10 = 25,
    Profile_User_Def_11 = 26,
    Profile_User_Def_12 = 27,
    Profile_User_Def_13 = 28,
    Profile_User_Def_14 = 29,
    Profile_User_Def_15 = 30,
    Profile_User_Def_16 = 31,
} profile_id_t;

typedef struct {
    uint32 meter_cir;
    uint32 meter_cbs;
    uint32 meter_ebs;
} meter_rate_t;

typedef enum {
    RateTypexBS,
    RateTypexIR,
} rate_type_t;

#if defined(UM_KEIL_BUILD) || defined(M7_COMPILE)
extern sys_error_t bcm53158_ports_queue_schedule_set(uint8 unit,
                        uint32 pbmp, uint8 queue, uint8 weight);
#else
extern sys_error_t bcm53158_ports_queue_schedule_set(uint8 unit,
                        uint32 pbmp, uint8 queue, uint8 weight) __reentrant;
#endif
extern sys_error_t bcm53158_forwarding_profile_set(uint8 unit,
                        profile_id_t id, meter_rate_t *rate);
extern sys_error_t bcm53158_forwarding_meter_set(uint8 unit, uint32 pbmp,
                        traffic_type_t type, profile_id_t id);
extern sys_error_t bcm53158_flow_control_pause_tx_en(uint8 unit);
extern sys_error_t bcm53158_auto_qos_init(uint8 unit);

#endif /* _AUTO_QOS_H_ */
