/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (C) 2022 Contemporary Software
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

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <rtems.h>
#include <rtems/sysinit.h>
#include <rtems/thread.h>

#include <bsp.h>

#include <psci/smccc.h>
#include <dev/pm/pm.h>
#include <dev/pm/pm-trace.h>

typedef struct {
  uint32_t r0;
  uint32_t r1;
  uint32_t r2;
  uint32_t r3;
  uint32_t r4;
  uint32_t r5;
} pm_ret_payload;

RTEMS_SYSINIT_ITEM(
  rtems_smccc_init,
  RTEMS_SYSINIT_DRVMGR,
  RTEMS_SYSINIT_ORDER_FIRST
);

int rtems_pm_trace = PM_TRACE_DEBUG | PM_TRACE_IOCTL;
static rtems_mutex fpga_lock = RTEMS_MUTEX_INITIALIZER("pm/fpga");
static uint32_t api_feature_status[PM_API_MAX];

#ifdef BSP_RESET_SMC
#define ARM_SMCCC_CALL arm_smccc_smc
#else
#define ARM_SMCCC_CALL arm_smccc_hvc
#endif /* BSP_RESET_SMC */

#define SMCCC_ARM_ID(_id) SMCCC_FUNC_ID(SMCCC_FAST_CALL, SMCCC_32BIT_CALL, 0, _id)
#define SMCCC_STD_ID(_id) SMCCC_FUNC_ID(SMCCC_FAST_CALL, SMCCC_32BIT_CALL, 4, _id)
#define SMCCC_SIP_ID(_id) SMCCC_FUNC_ID(SMCCC_FAST_CALL, SMCCC_64BIT_CALL, 2, _id)

static const uint32_t sip_api_id[PM_API_MAX] = {
  0x1,   /* PM_GET_API_VERSION */
  0x2,   /* PM_SET_CONFIGURATION */
  0x3,   /* PM_GET_NODE_STATUS */
  0x4,   /* PM_GET_OP_CHARACTERISTIC */
  0x5,   /* PM_REGISTER_NOTIFIER */
  0x6,   /* PM_REQUEST_SUSPEND */
  0x7,   /* PM_SELF_SUSPEND */
  0x8,   /* PM_FORCE_POWERDOWN */
  0x9,   /* PM_ABORT_SUSPEND */
  0xA,   /* PM_REQUEST_WAKEUP */
  0xB,   /* PM_SET_WAKEUP_SOURCE */
  0xC,   /* PM_SYSTEM_SHUTDOWN */
  0xD,   /* PM_REQUEST_NODE */
  0xE,   /* PM_RELEASE_NODE */
  0xF,   /* PM_SET_REQUIREMENT */
  0x10,  /* PM_SET_MAX_LATENCY */
  0x11,  /* PM_RESET_ASSERT */
  0x12,  /* PM_RESET_GET_STATUS */
  0x13,  /* PM_MMIO_WRITE */
  0x14,  /* PM_MMIO_READ */
  0x15,  /* PM_INIT_FINALIZE */
  0x16,  /* PM_FPGA_LOAD */
  0x17,  /* PM_FPGA_GET_STATUS */
  0x18,  /* PM_GET_CHIPID */
  0x19,  /* PM_SECURE_RSA_AES */
  0x1A,  /* PM_SECURE_SHA */
  0x1B,  /* PM_SECURE_RSA */
  0x1C,  /* PM_PINCTRL_REQUEST */
  0x1D,  /* PM_PINCTRL_RELEASE */
  0x1E,  /* PM_PINCTRL_GET_FUNCTION */
  0x1F,  /* PM_PINCTRL_SET_FUNCTION */
  0x20,  /* PM_PINCTRL_CONFIG_PARAM_GET */
  0x21,  /* PM_PINCTRL_CONFIG_PARAM_SET */
  0x22,  /* PM_IOCTL */
  0x23,  /* PM_QUERY_DATA */
  0x24,  /* PM_CLOCK_ENABLE */
  0x25,  /* PM_CLOCK_DISABLE */
  0x26,  /* PM_CLOCK_GETSTATE */
  0x27,  /* PM_CLOCK_SETDIVIDER */
  0x28,  /* PM_CLOCK_GETDIVIDER */
  0x29,  /* PM_CLOCK_SETRATE */
  0x2A,  /* PM_CLOCK_GETRATE */
  0x2B,  /* PM_CLOCK_SETPARENT */
  0x2C,  /* PM_CLOCK_GETPARENT */
  0x2D,  /* PM_SECURE_IMAGE */
  0x2E,  /* PM_FPGA_READ */
  0x2F,  /* PM_API_RESERVED_1 */
  0x30,  /* PM_PLL_SET_PARAMETER */
  0x31,  /* PM_PLL_GET_PARAMETER */
  0x32,  /* PM_PLL_SET_MODE */
  0x33,  /* PM_PLL_GET_MODE */
  0x34,  /* PM_REGISTER_ACCESS */
  0x35,  /* PM_EFUSE_ACCESS */
  0x36,  /* PM_ADD_SUBSYSTEM */
  0x37,  /* PM_DESTROY_SUBSYSTEM */
  0x38,  /* PM_DESCRIBE_NODES */
  0x39,  /* PM_ADD_NODE */
  0x3A,  /* PM_ADD_NODE_PARENT */
  0x3B,  /* PM_ADD_NODE_NAME */
  0x3C,  /* PM_ADD_REQUIREMENT */
  0x3D,  /* PM_SET_CURRENT_SUBSYSTEM */
  0x3E,  /* PM_INIT_NODE */
  0x3F,  /* PM_FEATURE_CHECK */
  0x40,  /* PM_ISO_CONTROL */
  0x41,  /* PM_ACTIVATE_SUBSYSTEM */
  0x568, /* PM_WRITE_AES_KEY */
  0x701, /* PM_LOAD_PDI */
  0x705, /* PM_GET_UID_INFO_LIST */
  0x706, /* PM_GET_META_HEADER_INFO_LIST */
  0xA01, /* GET_CALLBACK_DATA */
  0xA02, /* PM_SET_SUSPEND_MODE */
  0xA03, /* PM_GET_TRUSTZONE_VERSION */
  0xA04, /* TF_A_PM_REGISTER_SGI */
  0xB01, /* PM_BBRAM_WRITE_KEY */
  0xB02, /* PM_BBRAM_ZEROIZE */
  0xB03, /* PM_BBRAM_WRITE_USERDATA */
  0xB04, /* PM_BBRAM_READ_USERDATA */
  0xB05, /* PM_BBRAM_LOCK_USERDATA */
};

/*
 * Translate the enum to the actual value.
 */
static uint32_t pm_sip_api_id(const pm_api_id api_id) {
  if (api_id < PM_API_MAX) {
    return sip_api_id[api_id];
  }
  return 0;
}

static uint32_t pm_lower_32(uint64_t u64) {
  return (uint32_t) (u64 & 0xffffffff);
}

static uint32_t pm_upper_32(uint64_t u64) {
  return (uint32_t) (u64 >> 32);
}

static int pm_result(int result, uint32_t res_a0) {
  switch (result) {
    case SMCCC_RET_SUCCESS:
      switch (res_a0) {
        case PM_STATUS_SUCCESS:
        case PM_STATUS_DOUBLE_REQ:
          errno = 0;
          return 0;
        case PM_STATUS_NO_FEATURE:
          errno = ENOTSUP;
          break;
        case PM_STATUS_INVALID_VERSION:
          errno = ENOTSUP;
          break;
        case PM_STATUS_NO_ACCESS:
          errno = EACCES;
          break;
        case PM_STATUS_ABORT_SUSPEND:
          errno = ECANCELED;
          break;
        case PM_STATUS_MULT_USER:
          errno = EIO;
          break;
        case PM_STATUS_INTERNAL:
        case PM_STATUS_CONFLICT:
        case PM_STATUS_INVALID_NODE:
        default:
          errno = EINVAL;
          break;
      }
      break;
    case SMCCC_RET_NOT_SUPPORTED:
      errno = ENOTSUP;
      break;
    case SMCCC_RET_NOT_REQUIRED:
    default:
      errno = EINVAL;
      break;
  }
  return -1;
}

static int pm_invoke(
  uint32_t api_id, uint32_t arg0, uint32_t arg1, uint32_t arg2, uint32_t arg3,
  uint32_t arg4, pm_ret_payload* res)
{
  struct arm_smccc_res res_;
  uint64_t a64_0 = ((uint64_t) arg1 << 32) | ((uint64_t) arg0);
  uint64_t a64_1 = ((uint64_t) arg3 << 32) | ((uint64_t) arg2);
  uint64_t a64_2 = (uint64_t) arg4;
  int ret = ARM_SMCCC_CALL(api_id, a64_0, a64_1, a64_2, 0, 0, 0, 0, &res_);
  res->r0 = pm_lower_32(res_.a0);
  res->r1 = pm_upper_32(res_.a0);
  res->r2 = pm_lower_32(res_.a1);
  res->r3 = pm_upper_32(res_.a1);
  res->r4 = pm_lower_32(res_.a2);
  return ret;
}

static int pm_invoke_sip(
  pm_api_id api_id, uint32_t arg0, uint32_t arg1, uint32_t arg2, uint32_t arg3,
  uint32_t arg4, pm_ret_payload* res)
{
  int ret = pm_invoke(
    SMCCC_SIP_ID(pm_sip_api_id(api_id)), arg0, arg1, arg2, arg3, arg4, res);
  return pm_result(ret, res->r0);
}

static RTEMS_UNUSED int pm_feature(uint32_t api_id) {
  pm_ret_payload res;
  int r =
    pm_invoke_sip(PM_FEATURE_CHECK, api_id, 0, 0, 0, 0, &res);
  return r;
}

int rtems_pm_feature_check(pm_api_id api_id) {
  int r = 0;
  if (api_id >= PM_API_MAX) {
    errno = EINVAL;
    return -1;
  }
  if (api_feature_status[api_id] == 0) {
    pm_ret_payload res;
    r = pm_invoke_sip(PM_FEATURE_CHECK, pm_sip_api_id(api_id), 0, 0, 0, 0, &res);
    api_feature_status[api_id] = res.r0;
  }
  if (api_feature_status[api_id] == PM_STATUS_NO_FEATURE) {
    errno = ENOTSUP;
    r = -1;
  }
  return r;
}

uint32_t rtems_pm_get_api_id(pm_api_id api_id) {
  return pm_sip_api_id(api_id);
}

int rtems_pm_max_api_id(void) {
  return PM_API_MAX;
}

int rtems_pm_api_version(pm_data_api_version* version) {
  int r = rtems_pm_feature_check(PM_GET_CHIPID);
  if (r == 0) {
    pm_ret_payload res;
    r =
      pm_invoke_sip(PM_GET_API_VERSION, 0, 0, 0, 0, 0, &res);
    version->major = res.r1 >> 16;
    version->minor = res.r1 & 0xffff;
  }
  return r;
}

int rtems_pm_chipid(pm_data_chipid* chipid) {
  int r = rtems_pm_feature_check(PM_GET_CHIPID);
  if (r == 0) {
    pm_ret_payload res;
    r =
      pm_invoke_sip(PM_GET_CHIPID, 0, 0, 0, 0, 0, &res);
    chipid->idcode = res.r1;
    chipid->version = res.r2;
  }
  return r;
}

int rtems_pm_fpga_load(
  const void* image, size_t size, uint32_t flags, uint32_t* status) {
  pm_ret_payload res;
  const uint64_t addr = (intptr_t) image;
  int r;
  rtems_cache_flush_multiple_data_lines(image, size);
  r = pm_invoke_sip(
    PM_FPGA_LOAD, pm_lower_32(addr), pm_upper_32(addr), size, flags, 0, &res);
  *status = res.r0;
  return r;
}

int rtems_pm_fpga_get_status(uint32_t* status) {
  int r = rtems_pm_feature_check(PM_FPGA_GET_STATUS);
  if (r == 0) {
    pm_ret_payload res;
    r = pm_invoke_sip(PM_FPGA_GET_STATUS, 0, 0, 0, 0, 0, &res);
    *status = res.r1;
  } else {
    *status = -1;
  }
  return r;
}

int rtems_pm_acap_load(const void* image, size_t size, uint32_t* status) {
  pm_ret_payload res;
  const void* ihdrtab;
  const uint64_t addr = (intptr_t) image;
  int r;
  ihdrtab = rtems_pm_acap_image_header(image);
  if (ihdrtab == NULL) {
    printf("error: ACAP image: corrupt image\n");
    errno = EIO;
    return -1;
  }
  rtems_cache_flush_multiple_data_lines(image, size);
  /*
   * Only support DDR. The modes are set here:
   *  https://github.com/Xilinx/embeddedsw/blob/master/lib/sw_services/xilloader/src/xloader.h#L229
   */
  #define PDI_SRC_DDR 0xf
  rtems_mutex_lock(&fpga_lock);
  r = pm_invoke_sip(
    PM_LOAD_PDI, PDI_SRC_DDR, pm_lower_32(addr), pm_upper_32(addr), 0, 0, &res);
  rtems_mutex_unlock(&fpga_lock);
  *status = res.r0;
  return r;
}

int rtems_pm_ioctl(pm_data_ioctl* ioctl) {
  (void) ioctl;
  return -1;
}

int rtems_pm_query_data(pm_data_query* query) {
  (void) query;
  return -1;
}

int rtems_pm_mmio_read(const uint32_t address, uint32_t* value) {
  pm_ret_payload res;
  int r;
  rtems_mutex_lock(&fpga_lock);
  r = pm_invoke_sip(
    PM_MMIO_READ, address, 0, 0, 0, 0, &res);
  rtems_mutex_unlock(&fpga_lock);
  *value = res.r1;
  return r;
}

int rtems_pm_mmio_write(
  const uint32_t address, const uint32_t mask, const uint32_t value) {
  pm_ret_payload res;
  int r;
  rtems_mutex_lock(&fpga_lock);
  r = pm_invoke_sip(
    PM_MMIO_WRITE, address, mask, value, 0, 0, &res);
  rtems_mutex_unlock(&fpga_lock);
  return r;
}

int rtems_pm_clock_control(pm_data_control* ctrl) {
  pm_ret_payload res;
  int r;
  rtems_mutex_lock(&fpga_lock);
  r = pm_invoke_sip(
    ctrl->id, ctrl->arg1, ctrl->arg2, 0, 0, 0, &res);
  rtems_mutex_unlock(&fpga_lock);
  ctrl->status = res.r0;
  return r;
}

int rtems_pm_clock_div_control(pm_data_control* ctrl) {
  (void) ctrl;
  return -1;
}

int rtems_pm_clock_rate_control(pm_data_control* ctrl) {
  (void) ctrl;
  return -1;
}

int rtems_pm_clock_parent_control(pm_data_control* ctrl) {
  (void) ctrl;
  return -1;
}

int rtems_pm_pll_mode(pm_data_control* ctrl) {
  (void) ctrl;
  return -1;
}

int rtems_pm_pll_fraction_data(pm_data_control* ctrl) {
  (void) ctrl;
  return -1;
}
