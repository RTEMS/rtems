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

#include <stdio.h>

#include <rtems.h>

#include <psci/smccc.h>

#include <bsp.h>

typedef struct {
  uint32_t r0;
  uint32_t r1;
  uint32_t r2;
  uint32_t r3;
} ret_payload;

#define SMCCC_STD_FIND_FEATURES SMCCC_FUNC_ID(SMCCC_FAST_CALL, SMCCC_32BIT_CALL, 4, 10)

#define SMCCC_VERSION_1_0 0x10000

static uint32_t smccc_version = SMCCC_VERSION_1_0;

#ifdef BSP_RESET_SMC
#define ARM_SMCCC_CALL arm_smccc_smc
#else
#define ARM_SMCCC_CALL arm_smccc_hvc
#endif /* BSP_RESET_SMC */

static int smccc_call(
  uint32_t api_id, uint32_t arg0, uint32_t arg1, uint32_t arg2)
{
  struct arm_smccc_res res_;
  return ARM_SMCCC_CALL(api_id, arg0, arg1, arg2, 0, 0, 0, 0, &res_);
}

int32_t smccc_arch_features(uint32_t smccc_func_id){
  if (smccc_version == SMCCC_VERSION_1_0) {
    return SMCCC_RET_NOT_SUPPORTED;
  }
  return smccc_call(SMCCC_ARCH_FEATURES, smccc_func_id, 0, 0);
}

/*
 * The SMCCC handler for Spectre variant 2: Branch target injection.
 * (CVE-2017-5715)
 */
int
smccc_arch_workaround_1(void)
{
  if (smccc_version != SMCCC_VERSION_1_0) {
    rtems_panic("SMCCC arch workaround 1 called with an invalid SMCCC interface");
  }
  return smccc_call(SMCCC_ARCH_WORKAROUND_1, 0, 0, 0);
}

int smccc_arch_workaround_2(int enable) {
  if (smccc_version != SMCCC_VERSION_1_0) {
    rtems_panic("SMCCC arch workaround 2 called with an invalid SMCCC interface");
  }
  return smccc_call(SMCCC_ARCH_WORKAROUND_2, enable, 0, 0);
}

void rtems_smccc_init(void) {
  smccc_version = 1;
  int32_t features = smccc_call(SMCCC_STD_FIND_FEATURES, SMCCC_VERSION, 0, 0);
  if (features != SMCCC_RET_NOT_SUPPORTED) {
    uint32_t ret = smccc_call(SMCCC_VERSION, 0, 0, 0);
    if (ret > 0) {
      smccc_version = ret;
    }
    printf("SMCCC version %u.%u\n",
      SMCCC_VERSION_MAJOR(smccc_version), SMCCC_VERSION_MINOR(smccc_version));
  }
}
