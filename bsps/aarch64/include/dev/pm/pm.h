/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (C) 2022 Contemporary Software (Chris Johns)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _RTEMS_DEV_PM_PM_H
#define _RTEMS_DEV_PM_PM_H

#include <stdbool.h>
#include <stdint.h>

#include "pm-image.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * PM API id code are translated into the actual codes in the driver.
 */
typedef enum {
  PM_GET_API_VERSION,                /* 0x1 */
  PM_SET_CONFIGURATION,              /* 0x2 */
  PM_GET_NODE_STATUS,                /* 0x3 */
  PM_GET_OP_CHARACTERISTIC,          /* 0x4 */
  PM_REGISTER_NOTIFIER,              /* 0x5 */
  PM_REQUEST_SUSPEND,                /* 0x6 */
  PM_SELF_SUSPEND,                   /* 0x7 */
  PM_FORCE_POWERDOWN,                /* 0x8 */
  PM_ABORT_SUSPEND,                  /* 0x9 */
  PM_REQUEST_WAKEUP,                 /* 0xA */
  PM_SET_WAKEUP_SOURCE,              /* 0xB */
  PM_SYSTEM_SHUTDOWN,                /* 0xC */
  PM_REQUEST_NODE,                   /* 0xD */
  PM_RELEASE_NODE,                   /* 0xE */
  PM_SET_REQUIREMENT,                /* 0xF */
  PM_SET_MAX_LATENCY,                /* 0x10 */
  PM_RESET_ASSERT,                   /* 0x11 */
  PM_RESET_GET_STATUS,               /* 0x12 */
  PM_MMIO_WRITE,                     /* 0x13 */
  PM_MMIO_READ,                      /* 0x14 */
  PM_INIT_FINALIZE,                  /* 0x15 */
  PM_FPGA_LOAD,                      /* 0x16 */
  PM_FPGA_GET_STATUS,                /* 0x17 */
  PM_GET_CHIPID,                     /* 0x18 */
  PM_SECURE_RSA_AES,                 /* 0x19 */
  PM_SECURE_SHA,                     /* 0x1A */
  PM_SECURE_RSA,                     /* 0x1B */
  PM_PINCTRL_REQUEST,                /* 0x1C */
  PM_PINCTRL_RELEASE,                /* 0x1D */
  PM_PINCTRL_GET_FUNCTION,           /* 0x1E */
  PM_PINCTRL_SET_FUNCTION,           /* 0x1F */
  PM_PINCTRL_CONFIG_PARAM_GET,       /* 0x20 */
  PM_PINCTRL_CONFIG_PARAM_SET,       /* 0x21 */
  PM_IOCTL,                          /* 0x22 */
  PM_QUERY_DATA,                     /* 0x23 */
  PM_CLOCK_ENABLE,                   /* 0x24 */
  PM_CLOCK_DISABLE,                  /* 0x25 */
  PM_CLOCK_GETSTATE,                 /* 0x26 */
  PM_CLOCK_SETDIVIDER,               /* 0x27 */
  PM_CLOCK_GETDIVIDER,               /* 0x28 */
  PM_CLOCK_SETRATE,                  /* 0x29 */
  PM_CLOCK_GETRATE,                  /* 0x2A */
  PM_CLOCK_SETPARENT,                /* 0x2B */
  PM_CLOCK_GETPARENT,                /* 0x2C */
  PM_SECURE_IMAGE,                   /* 0x2D */
  PM_FPGA_READ,                      /* 0x2E */
  PM_API_RESERVED_1,                 /* 0x2F */
  PM_PLL_SET_PARAMETER,              /* 0x30 */
  PM_PLL_GET_PARAMETER,              /* 0x31 */
  PM_PLL_SET_MODE,                   /* 0x32 */
  PM_PLL_GET_MODE,                   /* 0x33 */
  PM_REGISTER_ACCESS,                /* 0x34 */
  PM_EFUSE_ACCESS,                   /* 0x35 */
  PM_ADD_SUBSYSTEM,                  /* 0x36 */
  PM_DESTROY_SUBSYSTEM,              /* 0x37 */
  PM_DESCRIBE_NODES,                 /* 0x38 */
  PM_ADD_NODE,                       /* 0x39 */
  PM_ADD_NODE_PARENT,                /* 0x3A */
  PM_ADD_NODE_NAME,                  /* 0x3B */
  PM_ADD_REQUIREMENT,                /* 0x3C */
  PM_SET_CURRENT_SUBSYSTEM,          /* 0x3D */
  PM_INIT_NODE,                      /* 0x3E */
  PM_FEATURE_CHECK,                  /* 0x3F */
  PM_ISO_CONTROL,                    /* 0x40 */
  PM_ACTIVATE_SUBSYSTEM,             /* 0x41 */
  PM_WRITE_AES_KEY,                  /* 0x568 */
  PM_LOAD_PDI,                       /* 0x701 */
  PM_GET_UID_INFO_LIST,              /* 0x705 */
  PM_GET_META_HEADER_INFO_LIST,      /* 0x706 */
  GET_CALLBACK_DATA,                 /* 0xA01 */
  PM_SET_SUSPEND_MODE,               /* 0xA02 */
  PM_GET_TRUSTZONE_VERSION,          /* 0xA03 */
  TF_A_PM_REGISTER_SGI,              /* 0xA04 */
  PM_BBRAM_WRITE_KEY,                /* 0xB01 */
  PM_BBRAM_ZEROIZE,                  /* 0xB02 */
  PM_BBRAM_WRITE_USERDATA,           /* 0xB03 */
  PM_BBRAM_READ_USERDATA,            /* 0xB04 */
  PM_BBRAM_LOCK_USERDATA,            /* 0xB05 */
  PM_API_MAX
} pm_api_id;

typedef enum {
  PM_STATUS_SUCCESS = 0,
  PM_STATUS_INVALID_VERSION = 4,
  PM_STATUS_NO_FEATURE = 19,
  PM_STATUS_INTERNAL = 2000,
  PM_STATUS_CONFLICT = 2001,
  PM_STATUS_NO_ACCESS = 2002,
  PM_STATUS_INVALID_NODE = 2003,
  PM_STATUS_DOUBLE_REQ = 2004,
  PM_STATUS_ABORT_SUSPEND = 2005,
  PM_STATUS_MULT_USER = 2008,
} pm_ret_status ;

typedef enum {
  PM_QID_INVALID = 0,
  PM_QID_CLOCK_GET_NAME = 1,
  PM_QID_CLOCK_GET_TOPOLOGY = 2,
  PM_QID_CLOCK_GET_FIXEDFACTOR_PARAMS = 3,
  PM_QID_CLOCK_GET_PARENTS = 4,
  PM_QID_CLOCK_GET_ATTRIBUTES = 5,
  PM_QID_PINCTRL_GET_NUM_PINS = 6,
  PM_QID_PINCTRL_GET_NUM_FUNCTIONS = 7,
  PM_QID_PINCTRL_GET_NUM_FUNCTION_GROUPS = 8,
  PM_QID_PINCTRL_GET_FUNCTION_NAME = 9,
  PM_QID_PINCTRL_GET_FUNCTION_GROUPS = 10,
  PM_QID_PINCTRL_GET_PIN_GROUPS = 11,
  PM_QID_CLOCK_GET_NUM_CLOCKS = 12,
  PM_QID_CLOCK_GET_MAX_DIVISOR = 13,
  PM_QID_PLD_GET_PARENT = 14,
} pm_query_id;

typedef enum {
  PM_RESET_START = 1000,
  PM_RESET_PCIE_CFG = PM_RESET_START,
  PM_RESET_PCIE_BRIDGE = 1001,
  PM_RESET_PCIE_CTRL = 1002,
  PM_RESET_DP = 1003,
  PM_RESET_SWDT_CRF = 1004,
  PM_RESET_AFI_FM5 = 1005,
  PM_RESET_AFI_FM4 = 1006,
  PM_RESET_AFI_FM3 = 1007,
  PM_RESET_AFI_FM2 = 1008,
  PM_RESET_AFI_FM1 = 1009,
  PM_RESET_AFI_FM0 = 1010,
  PM_RESET_GDMA = 1011,
  PM_RESET_GPU_PP1 = 1012,
  PM_RESET_GPU_PP0 = 1013,
  PM_RESET_GPU = 1014,
  PM_RESET_GT = 1015,
  PM_RESET_SATA = 1016,
  PM_RESET_ACPU3_PWRON = 1017,
  PM_RESET_ACPU2_PWRON = 1018,
  PM_RESET_ACPU1_PWRON = 1019,
  PM_RESET_ACPU0_PWRON = 1020,
  PM_RESET_APU_L2 = 1021,
  PM_RESET_ACPU3 = 1022,
  PM_RESET_ACPU2 = 1023,
  PM_RESET_ACPU1 = 1024,
  PM_RESET_ACPU0 = 1025,
  PM_RESET_DDR = 1026,
  PM_RESET_APM_FPD = 1027,
  PM_RESET_SOFT = 1028,
  PM_RESET_GEM0 = 1029,
  PM_RESET_GEM1 = 1030,
  PM_RESET_GEM2 = 1031,
  PM_RESET_GEM3 = 1032,
  PM_RESET_QSPI = 1033,
  PM_RESET_UART0 = 1034,
  PM_RESET_UART1 = 1035,
  PM_RESET_SPI0 = 1036,
  PM_RESET_SPI1 = 1037,
  PM_RESET_SDIO0 = 1038,
  PM_RESET_SDIO1 = 1039,
  PM_RESET_CAN0 = 1040,
  PM_RESET_CAN1 = 1041,
  PM_RESET_I2C0 = 1042,
  PM_RESET_I2C1 = 1043,
  PM_RESET_TTC0 = 1044,
  PM_RESET_TTC1 = 1045,
  PM_RESET_TTC2 = 1046,
  PM_RESET_TTC3 = 1047,
  PM_RESET_SWDT_CRL = 1048,
  PM_RESET_NAND = 1049,
  PM_RESET_ADMA = 1050,
  PM_RESET_GPIO = 1051,
  PM_RESET_IOU_CC = 1052,
  PM_RESET_TIMESTAMP = 1053,
  PM_RESET_RPU_R50 = 1054,
  PM_RESET_RPU_R51 = 1055,
  PM_RESET_RPU_AMBA = 1056,
  PM_RESET_OCM = 1057,
  PM_RESET_RPU_PGE = 1058,
  PM_RESET_USB0_CORERESET = 1059,
  PM_RESET_USB1_CORERESET = 1060,
  PM_RESET_USB0_HIBERRESET = 1061,
  PM_RESET_USB1_HIBERRESET = 1062,
  PM_RESET_USB0_APB = 1063,
  PM_RESET_USB1_APB = 1064,
  PM_RESET_IPI = 1065,
  PM_RESET_APM_LPD = 1066,
  PM_RESET_RTC = 1067,
  PM_RESET_SYSMON = 1068,
  PM_RESET_AFI_FM6 = 1069,
  PM_RESET_LPD_SWDT = 1070,
  PM_RESET_FPD = 1071,
  PM_RESET_RPU_DBG1 = 1072,
  PM_RESET_RPU_DBG0 = 1073,
  PM_RESET_DBG_LPD = 1074,
  PM_RESET_DBG_FPD = 1075,
  PM_RESET_APLL = 1076,
  PM_RESET_DPLL = 1077,
  PM_RESET_VPLL = 1078,
  PM_RESET_IOPLL = 1079,
  PM_RESET_RPLL = 1080,
  PM_RESET_GPO3_PL_0 = 1081,
  PM_RESET_GPO3_PL_1 = 1082,
  PM_RESET_GPO3_PL_2 = 1083,
  PM_RESET_GPO3_PL_3 = 1084,
  PM_RESET_GPO3_PL_4 = 1085,
  PM_RESET_GPO3_PL_5 = 1086,
  PM_RESET_GPO3_PL_6 = 1087,
  PM_RESET_GPO3_PL_7 = 1088,
  PM_RESET_GPO3_PL_8 = 1089,
  PM_RESET_GPO3_PL_9 = 1090,
  PM_RESET_GPO3_PL_10 = 1091,
  PM_RESET_GPO3_PL_11 = 1092,
  PM_RESET_GPO3_PL_12 = 1093,
  PM_RESET_GPO3_PL_13 = 1094,
  PM_RESET_GPO3_PL_14 = 1095,
  PM_RESET_GPO3_PL_15 = 1096,
  PM_RESET_GPO3_PL_16 = 1097,
  PM_RESET_GPO3_PL_17 = 1098,
  PM_RESET_GPO3_PL_18 = 1099,
  PM_RESET_GPO3_PL_19 = 1100,
  PM_RESET_GPO3_PL_20 = 1101,
  PM_RESET_GPO3_PL_21 = 1102,
  PM_RESET_GPO3_PL_22 = 1103,
  PM_RESET_GPO3_PL_23 = 1104,
  PM_RESET_GPO3_PL_24 = 1105,
  PM_RESET_GPO3_PL_25 = 1106,
  PM_RESET_GPO3_PL_26 = 1107,
  PM_RESET_GPO3_PL_27 = 1108,
  PM_RESET_GPO3_PL_28 = 1109,
  PM_RESET_GPO3_PL_29 = 1110,
  PM_RESET_GPO3_PL_30 = 1111,
  PM_RESET_GPO3_PL_31 = 1112,
  PM_RESET_RPU_LS = 1113,
  PM_RESET_PS_ONLY = 1114,
  PM_RESET_PL = 1115,
  PM_RESET_PS_PL0 = 1116,
  PM_RESET_PS_PL1 = 1117,
  PM_RESET_PS_PL2 = 1118,
  PM_RESET_PS_PL3 = 1119,
  PM_RESET_END = PM_RESET_PS_PL3
} zynqmp_pm_reset;

typedef enum {
  PM_ABORT_REASON_WAKEUP_EVENT = 100,
  PM_ABORT_REASON_POWER_UNIT_BUSY = 101,
  PM_ABORT_REASON_NO_POWERDOWN = 102,
  PM_ABORT_REASON_UNKNOWN = 103,
} zynqmp_pm_abort_reason;

typedef enum {
  PM_SUSPEND_POWER_REQUEST = 201,
  PM_SUSPEND_ALERT = 202,
  PM_SUSPEND_SYSTEM_SHUTDOWN = 203,
} pm_suspend_reason;

typedef enum {
  PM_REQUEST_ACK_NO = 1,
  PM_REQUEST_ACK_BLOCKING = 2,
  PM_REQUEST_ACK_NON_BLOCKING = 3,
} pm_request_ack;

typedef enum {
  PM_NODE_UNKNOWN = 0,
  PM_NODE_APU = 1,
  PM_NODE_APU_0 = 2,
  PM_NODE_APU_1 = 3,
  PM_NODE_APU_2 = 4,
  PM_NODE_APU_3 = 5,
  PM_NODE_RPU = 6,
  PM_NODE_RPU_0 = 7,
  PM_NODE_RPU_1 = 8,
  PM_NODE_PLD = 9,
  PM_NODE_FPD = 10,
  PM_NODE_OCM_BANK_0 = 11,
  PM_NODE_OCM_BANK_1 = 12,
  PM_NODE_OCM_BANK_2 = 13,
  PM_NODE_OCM_BANK_3 = 14,
  PM_NODE_TCM_0_A = 15,
  PM_NODE_TCM_0_B = 16,
  PM_NODE_TCM_1_A = 17,
  PM_NODE_TCM_1_B = 18,
  PM_NODE_L2 = 19,
  PM_NODE_GPU_PP_0 = 20,
  PM_NODE_GPU_PP_1 = 21,
  PM_NODE_USB_0 = 22,
  PM_NODE_USB_1 = 23,
  PM_NODE_TTC_0 = 24,
  PM_NODE_TTC_1 = 25,
  PM_NODE_TTC_2 = 26,
  PM_NODE_TTC_3 = 27,
  PM_NODE_SATA = 28,
  PM_NODE_ETH_0 = 29,
  PM_NODE_ETH_1 = 30,
  PM_NODE_ETH_2 = 31,
  PM_NODE_ETH_3 = 32,
  PM_NODE_UART_0 = 33,
  PM_NODE_UART_1 = 34,
  PM_NODE_SPI_0 = 35,
  PM_NODE_SPI_1 = 36,
  PM_NODE_I2C_0 = 37,
  PM_NODE_I2C_1 = 38,
  PM_NODE_SD_0 = 39,
  PM_NODE_SD_1 = 40,
  PM_NODE_DP = 41,
  PM_NODE_GDMA = 42,
  PM_NODE_ADMA = 43,
  PM_NODE_NAND = 44,
  PM_NODE_QSPI = 45,
  PM_NODE_GPIO = 46,
  PM_NODE_CAN_0 = 47,
  PM_NODE_CAN_1 = 48,
  PM_NODE_EXTERN = 49,
  PM_NODE_APLL = 50,
  PM_NODE_VPLL = 51,
  PM_NODE_DPLL = 52,
  PM_NODE_RPLL = 53,
  PM_NODE_IOPLL = 54,
  PM_NODE_DDR = 55,
  PM_NODE_IPI_APU = 56,
  PM_NODE_IPI_RPU_0 = 57,
  PM_NODE_GPU = 58,
  PM_NODE_PCIE = 59,
  PM_NODE_PCAP = 60,
  PM_NODE_RTC = 61,
  PM_NODE_LPD = 62,
  PM_NODE_VCU = 63,
  PM_NODE_IPI_RPU_1 = 64,
  PM_NODE_IPI_PL_0 = 65,
  PM_NODE_IPI_PL_1 = 66,
  PM_NODE_IPI_PL_2 = 67,
  PM_NODE_IPI_PL_3 = 68,
  PM_NODE_PL = 69,
  PM_NODE_GEM_TSU = 70,
  PM_NODE_SWDT_0 = 71,
  PM_NODE_SWDT_1 = 72,
  PM_NODE_CSU = 73,
  PM_NODE_PJTAG = 74,
  PM_NODE_TRACE = 75,
  PM_NODE_TESTSCAN = 76,
  PM_NODE_PMU = 77,
  PM_NODE_MAX = 78,
} pm_node_id;

typedef enum {
  PM_TAPDELAY_INPUT = 0,
  PM_TAPDELAY_OUTPUT = 1,
} pm_tap_delay_type;

typedef enum {
  PM_DLL_RESET_ASSERT = 0,
  PM_DLL_RESET_RELEASE = 1,
  PM_DLL_RESET_PULSE = 2,
} pm_dll_reset_type;

typedef enum {
  PM_PINCTRL_CONFIG_SLEW_RATE = 0,
  PM_PINCTRL_CONFIG_BIAS_STATUS = 1,
  PM_PINCTRL_CONFIG_PULL_CTRL = 2,
  PM_PINCTRL_CONFIG_SCHMITT_CMOS = 3,
  PM_PINCTRL_CONFIG_DRIVE_STRENGTH = 4,
  PM_PINCTRL_CONFIG_VOLTAGE_STATUS = 5,
  PM_PINCTRL_CONFIG_TRI_STATE = 6,
  PM_PINCTRL_CONFIG_MAX = 7,
} pm_pinctrl_config_param;

typedef enum {
  PM_PINCTRL_SLEW_RATE_FAST = 0,
  PM_PINCTRL_SLEW_RATE_SLOW = 1,
} pm_pinctrl_slew_rate;

typedef enum {
  PM_PINCTRL_BIAS_DISABLE = 0,
  PM_PINCTRL_BIAS_ENABLE = 1,
} pm_pinctrl_bias_status;

typedef enum {
  PM_PINCTRL_BIAS_PULL_DOWN = 0,
  PM_PINCTRL_BIAS_PULL_UP = 1,
} pm_pinctrl_pull_ctrl;

typedef enum {
  PM_PINCTRL_INPUT_TYPE_CMOS = 0,
  PM_PINCTRL_INPUT_TYPE_SCHMITT = 1,
} pm_pinctrl_schmitt_cmos;

typedef enum {
  PM_OPERATING_CHARACTERISTIC_POWER = 1,
  PM_OPERATING_CHARACTERISTIC_ENERGY = 2,
  PM_OPERATING_CHARACTERISTIC_TEMPERATURE = 3,
} pm_opchar_type;

typedef enum {
  PM_PINCTRL_DRIVE_STRENGTH_2MA = 0,
  PM_PINCTRL_DRIVE_STRENGTH_4MA = 1,
  PM_PINCTRL_DRIVE_STRENGTH_8MA = 2,
  PM_PINCTRL_DRIVE_STRENGTH_12MA = 3,
} pm_pinctrl_drive_strength;

typedef enum {
  PM_PINCTRL_TRI_STATE_DISABLE = 0,
  PM_PINCTRL_TRI_STATE_ENABLE = 1,
} pm_pinctrl_tri_state;

typedef enum {
  PM_SHUTDOWN_TYPE_SHUTDOWN = 0,
  PM_SHUTDOWN_TYPE_RESET = 1,
  PM_SHUTDOWN_TYPE_SETSCOPE_ONLY = 2,
} pm_shutdown_type;

typedef enum {
  PM_SHUTDOWN_SUBTYPE_SUBSYSTEM = 0,
  PM_SHUTDOWN_SUBTYPE_PS_ONLY = 1,
  PM_SHUTDOWN_SUBTYPE_SYSTEM = 2,
} pm_shutdown_subtype;

typedef enum {
  PM_TAPDELAY_NAND_DQS_IN = 0,
  PM_TAPDELAY_NAND_DQS_OUT = 1,
  PM_TAPDELAY_QSPI = 2,
  PM_TAPDELAY_MAX = 3,
} pm_tap_delay_signal_type;

typedef enum {
  PM_TAPDELAY_BYPASS_DISABLE = 0,
  PM_TAPDELAY_BYPASS_ENABLE = 1,
} pm_tap_delay_bypass_ctrl;

typedef enum {
  PM_SGMII_DISABLE = 0,
  PM_SGMII_ENABLE = 1,
} pm_sgmii_mode;

typedef enum {
  PM_CONFIG_REG_WRITE = 0,
  PM_CONFIG_REG_READ = 1,
} pm_register_access_id;

typedef enum {
  PM_RESET_REASON_EXT_POR = 0,
  PM_RESET_REASON_SW_POR = 1,
  PM_RESET_REASON_SLR_POR = 2,
  PM_RESET_REASON_ERR_POR = 3,
  PM_RESET_REASON_DAP_SRST = 7,
  PM_RESET_REASON_ERR_SRST = 8,
  PM_RESET_REASON_SW_SRST = 9,
  PM_RESET_REASON_SLR_SRST = 10,
} pm_reset_reason;

typedef enum {
  PM_FEATURE_INVALID = 0,
  PM_FEATURE_OVERTEMP_STATUS = 1,
  PM_FEATURE_OVERTEMP_VALUE = 2,
  PM_FEATURE_EXTWDT_STATUS = 3,
  PM_FEATURE_EXTWDT_VALUE = 4,
} pm_feature_config_id;

typedef enum {
  PM_SD_CONFIG_EMMC_SEL = 1, /* To set SD_EMMC_SEL in CTRL_REG_SD and SD_SLOTTYPE */
  PM_SD_CONFIG_BASECLK = 2, /* To set SD_BASECLK in SD_CONFIG_REG1 */
  PM_SD_CONFIG_8BIT = 3, /* To set SD_8BIT in SD_CONFIG_REG2 */
  PM_SD_CONFIG_FIXED = 4, /* To set fixed config registers */
} pm_sd_config_type;

typedef enum {
  PM_GEM_CONFIG_SGMII_MODE = 1, /* To set GEM_SGMII_MODE in GEM_CLK_CTRL register */
  PM_GEM_CONFIG_FIXED = 2, /* To set fixed config registers */
} pm_gem_config_type;

typedef enum {
  PM_USB_CONFIG_FIXED = 1, /* To set fixed config registers */
} pm_usb_config_type;

typedef enum {
  PM_OSPI_MUX_SEL_DMA = 0,
  PM_OSPI_MUX_SEL_LINEAR = 1,
} pm_ospi_mux_select_type;

/*
 * Firmware FPGA Manager flags:
 * PM_FPGA_FULL:	  FPGA full reconfiguration
 * PM_FPGA_PARTIAL: FPGA partial reconfiguration
 */
typedef enum {
  PM_FPGA_FULL = 0 << 0,
  PM_FPGA_PARTIAL	= 1 << 0,
  PM_FPGA_AUTHENTICATION_DDR = 1 << 1,
  PM_FPGA_AUTHENTICATION_OCM = 1 << 2,
  PM_FPGA_ENCRYPTION_USERKEY = 1 << 3,
  PM_FPGA_ENCRYPTION_DEVKEY = 1 << 4,
} pm_fpga_flag;

typedef struct {
  int major;
  int minor;
} pm_data_api_version;

typedef struct {
  int idcode;
  int version;
} pm_data_chipid;

typedef struct {
  uint32_t node;
  uint32_t cmd;
  uint32_t arg1;
  uint32_t arg2;
} pm_data_ioctl;

typedef struct {
  uint32_t qid;
  uint32_t arg1;
  uint32_t arg2;
  uint32_t arg3;
} pm_data_query;

typedef struct {
  bool write_not_read;
  uint32_t id;
  uint32_t arg1;
  uint32_t arg2;
  uint32_t status;
} pm_data_control;

typedef struct {
  uint32_t id;
  uint32_t arg1;
  void* address;
} pm_data_fpga;

extern uint32_t smccc_version;


#ifdef __aarch64__
  #define RTEMS_PM_SUPPORTED 1
#else
  #define RTEMS_PM_SUPPORTED 0
#endif

extern struct rtems_shell_cmd_tt bsp_pm_load_shell_command;

int rtems_pm_api_version(pm_data_api_version* version);
int rtems_pm_chipid(pm_data_chipid* chipid);

int rtems_pm_feature_check(pm_api_id api_id);
uint32_t rtems_pm_get_api_id(pm_api_id api_id);
int rtems_pm_max_api_id(void);

/*
 * Direct calls for use by other drivers. You need to query the driver to get
 * the device handle.
 *
 * The flags and status are ignore on ACAP (Versal). ACAP's PMC only supports
 * loading a PDI.
 */
int rtems_pm_fpga_load(
  const void* image, size_t size, uint32_t flags, uint32_t* status);
int rtems_pm_fpga_get_status(uint32_t* status);

/*
 * Veral ACAP Image loading
 */
int rtems_pm_acap_load(const void* image, size_t size, uint32_t* status);

/*
 * Refer to Embedded Energy Management Interface [EEMI API Reference
 * Guide](UG1200).
 *
 * https://docs.xilinx.com/r/2021.1-English/oslib_rm/XilPM-Zynq-UltraScale-MPSoC-APIs
 */
int rtems_pm_ioctl(pm_data_ioctl* ioctl);
int rtems_pm_query_data(pm_data_query* query);

int rtems_pm_mmio_read(const uint32_t address, uint32_t* value);
int rtems_pm_mmio_write(
  const uint32_t address, const uint32_t mask, const uint32_t value);

int rtems_pm_clock_control(pm_data_control* ctrl);
int rtems_pm_clock_div_control(pm_data_control* ctrl);
int rtems_pm_clock_rate_control(pm_data_control* ctrl);
int rtems_pm_clock_parent_control(pm_data_control* ctrl);
int rtems_pm_pll_mode(pm_data_control* ctrl);
int rtems_pm_pll_fraction_data(pm_data_control* ctrl);

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_DEV_PM_PM_H */
