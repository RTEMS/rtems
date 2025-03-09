/**
 * @file
 *
 * @ingroup RTEMSBSPsAArch64RaspberryPi
 *
 * @brief Support for PWM for the PI4B.
 */

/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) samarth Golwalkar<samarthgolwalkar@gmail.com>
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

#include <bsp/rpi-pwm.h>
#include <bsp/rpi-gpio.h>
#include <bsp.h>

bool rpi_pwm_init(RPI_PWM pwm_id)
{
    const bool id_is_valid = pwm_id < RPI_PWM_COUNT;
    bool status = true;
    
    if(id_is_valid) {
        // Enable PWM clock and setup divider
        // TODO: Add RPI specific PWM clock initialization
        status = true;
    } else {
        status = false;
    }
    return status;
}

bool rpi_pwm_configure(RPI_PWM pwm_id, float pwm_freq, float duty)
{
    bool status = true;
    
    if (pwm_freq <= 0.0f || duty < 0.0f || duty > 100.0f) {
        return false;
    }

    // TODO: Add RPI specific PWM configuration
    // Calculate period and duty cycle values based on freq and duty
    
    return status;
}

bool rpi_pwm_enable(RPI_PWM pwm_id)
{
    const bool id_is_valid = pwm_id < RPI_PWM_COUNT;
    bool status = true;
    
    if (id_is_valid) {
        // TODO: Enable PWM output
        status = true;
    } else {
        status = false;
    }
    return status;
}

bool rpi_pwm_disable(RPI_PWM pwm_id) 
{
    const bool id_is_valid = pwm_id < RPI_PWM_COUNT;
    bool status = true;
    
    if (id_is_valid) {
        // TODO: Disable PWM output
        status = true;
    } else {
        status = false;
    }
    return status;
}
