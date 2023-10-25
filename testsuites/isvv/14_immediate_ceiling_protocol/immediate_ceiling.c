/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2022 Critical Software SA
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <rtems/bspIo.h>
#include "../shared/utils.h"
#include "../shared/isvv_rtems_aux.h"
#include <string.h>

/**
 *
 * @brief Tests ICPP of semaphores with two configurations:
 *
 * 1. Configured WITH ICPP
 * One test case which creates a mutex WITH the priority ceiling configured,
 * and shows that our high priority task NEVER overruns the time in its critical region
 *
 * 2. Configured WITHOUT ICPP
 * One test case which creates a mutex WITHOUT the priority ceiling configured,
 * and shows that our high priority task will FREQUENTLY overrun the time in its critical region
 */

// Different periods for our periodic tasks needed for robustness and reliability as
// Execution is dependant on processor clock speed i.e 250 MHz for gr740 and 80 MHz for gr712rc
#ifdef gr740
#define T0_PERIOD 400
#define T1_PERIOD 200
#else
#define T0_PERIOD 600
#define T1_PERIOD 300
#endif

#ifdef PRIO_CEILING
#define PRIO_PROTOCOL_SWITCH RTEMS_PRIORITY_CEILING
#else
#define PRIO_PROTOCOL_SWITCH RTEMS_NO_PRIORITY_CEILING
#endif

#define MAX_ITER (256U)
#define FFT_SIZE (1024U)
#define FFT_SIZE_LOG2 (10U)
#define TC_MAX (16U)
#define FILTER_REPETITIONS (2U)
#define SNR_THRESHOLD (1000000.0f)

const uint16_t FS = 48000U;
const uint16_t FREQ[TC_MAX] = {2500U, 1500U, 15000U, 500U,
                               1000U, 800U, 440U, 8000U,
                               100U, 3500U, 12345U, 1200U,
                               20000U, 715U, 5000U, 4500U};

const float NOISE_FACTOR[TC_MAX] = {0.0001f, 0.2250f, 0.00068f, 0.30f,
                                    0.004f, 0.0123f, 0.0054f, 0.00054f,
                                    0.01f, 0.0325f, 0.012f, 0.00032f,
                                    0.075f, 0.0423f, 0.0354f, 0.00002f};

static float inSensorDataRe[FFT_SIZE];
static float inSensorDataIm[FFT_SIZE];

#define ITOA_STR_SIZE (8 * sizeof(int) + 1)

#define MAX_TLS_SIZE RTEMS_ALIGN_UP(64, RTEMS_TASK_STORAGE_ALIGNMENT)

#define TASK_ATTRIBUTES RTEMS_FLOATING_POINT

#define TASK_STORAGE_SIZE                        \
    RTEMS_TASK_STORAGE_SIZE(                     \
        MAX_TLS_SIZE + RTEMS_MINIMUM_STACK_SIZE, \
        TASK_ATTRIBUTES)

#undef TEST_PROCESSORS
#define TEST_PROCESSORS 1
#define TASK_COUNT 3
#define EVENT_FINISHED_PROCESSING RTEMS_EVENT_0

#define T1_TUNE_REPS 50
#define T2_COARSE_TUNING_REPS 2
#define T2_FINE_TUNING_REPS 1300

typedef struct
{
    rtems_id main_task;
    rtems_id task_ids[TASK_COUNT];
    rtems_id period_0_id;
    rtems_id period_1_id;
    int64_t t2_max_ticks;
    uint16_t priority_inversion_occurrences;
    float m1_data;
    rtems_id m1_data_mutex;
    float scaling_fft_factor;
} test_context;

/* create storage areas for each worker, using task construct forces
the user to create these manually*/
RTEMS_ALIGNED(RTEMS_TASK_STORAGE_ALIGNMENT)
static char task_storage[TASK_COUNT][TASK_STORAGE_SIZE];

// TASK 0 - (highest priority w.r.t. the other two) - a SC S/W task
// Checks data after request for recent evidence of solar flare
static void solar_flare_analysis(rtems_task_argument arg)
{
    test_context *ctx = (test_context *)arg;

    uint16_t flare_count = 0;
    uint32_t start_time, end_time, time_in_critical_region;

    ctx->period_0_id = CreateRateMonotonic();

    while (1)
    {
        WaitPeriod(ctx->period_0_id, T0_PERIOD);

        // Timer to see how long it takes to enter and exit critical region
        start_time = rtems_clock_get_ticks_since_boot();
        ObtainMutex(ctx->m1_data_mutex);

        if (ctx->m1_data >= SNR_THRESHOLD)
            flare_count++;

        ReleaseMutex(ctx->m1_data_mutex);

        end_time = rtems_clock_get_ticks_since_boot();
        time_in_critical_region = end_time - start_time;

        // If time spent in critical region is greater than bound for T2 then we know priority inversion has occurred
        if (time_in_critical_region > ctx->t2_max_ticks)
            ctx->priority_inversion_occurrences++;
    }

    // Although this periodic task is executed ad infinitum it is good practice to finalize tasks
    rtems_task_exit();
}

// Task 1 - (medium priority w.r.t. the other two) - a COMS task
// A communication task which periodically receives and has to decode a message
// To simulate this, we are running the lucas-lehmer primality test
static void decode_communications(rtems_task_argument arg)
{
    uint32_t start_time, end_time, elapsed_time;
    test_context *ctx = (test_context *)arg;

    uint32_t res = 0;

    ctx->period_1_id = CreateRateMonotonic();

    while (1)
    {
        WaitPeriod(ctx->period_1_id, T1_PERIOD);

        start_time = rtems_clock_get_ticks_since_boot();

        for (uint16_t i = 0; i < T1_TUNE_REPS; i++)
        {
            for (uint8_t p = 2; p < 100; p++)
            {
                if (is_prime(p) && is_mersenne_prime(p))
                {
                    res = p;
                }
            }
        }

        end_time = rtems_clock_get_ticks_since_boot();
        elapsed_time = end_time - start_time;

        // If the execution time is greater than the period, and hence implicit deadline, then
        // this test cannot sufficiently be completed with the resources and parameters must be changed
        if (elapsed_time >= T1_PERIOD)
            print_string("[ERROR] Execution time of T1 is greater than period. Please reduce T1_TUNE_REPS \n");
    }

    (void)res;
    // Although this periodic task is executed ad infinitum it is good practice to finalize tasks
    rtems_task_exit();
}

// Computes the signal-to-noise ratio for our given data at a given position
static float compute_snr(float scaling_factor, uint32_t iter)
{
    for (uint32_t i = 0; i < FFT_SIZE; i++)
    {
        inSensorDataRe[i] = 0.4995f * cos_aprox(i * 2.0f * PI * FREQ[iter % TC_MAX] / FS) + (noise_generator(0) * NOISE_FACTOR[iter % TC_MAX]);
        inSensorDataIm[i] = 0.4995f * sin_aprox(i * 2.0f * PI * FREQ[iter % TC_MAX] / FS) + (noise_generator(0) * NOISE_FACTOR[iter % TC_MAX]);
    }

    for (uint32_t i = 0; i < FFT_SIZE; i++)
    {
        // Real & Imaginary
        inSensorDataRe[i] = inSensorDataRe[i] * blackman_harris(i, FFT_SIZE) / scaling_factor;
        inSensorDataIm[i] = inSensorDataIm[i] * blackman_harris(i, FFT_SIZE) / scaling_factor;
    }

    fft(&inSensorDataRe[0], &inSensorDataIm[0], FFT_SIZE_LOG2);

    // Look for the peak Value and its index (no DC)
    float maxValue = 0.0;
    int32_t maxValueIndex = -1;
    for (uint32_t i = 3; i < (FFT_SIZE / 2); i++)
    {
        if (inSensorDataRe[i] > maxValue)
        {
            maxValue = inSensorDataRe[i];
            maxValueIndex = i;
        }
    }

    // Calculates the signal and noise power (no DC)
    float sig = 0.0f;
    float noise = 0.0f;

    for (uint32_t i = 3; i < (FFT_SIZE / 2); i++)
    {
        if ((i > maxValueIndex - 8) && (i < maxValueIndex + 8))
        {
            sig += (2 * inSensorDataRe[i]);
        }
        else
        {
            noise += (2 * inSensorDataRe[i]);
        }
    }

    if (!(sig > 0.0f))
    {
        sig = 0.0000000001f;
    }
    if (!(noise > 0.0f))
    {
        noise = 0.0000000001f;
    }

    return sig / noise;
}

// Task 2 - (lowest priority w.r.t. the other two) - a SC S/W task
// Gathers signal data from sensors, computes the signal to noise ratio, then writes to a shared buffer
static void gather_meteorological_data(rtems_task_argument arg)
{
    test_context *ctx = (test_context *)arg;

    uint32_t iter = 0;

    while (iter < MAX_ITER)
    {
        ObtainMutex(ctx->m1_data_mutex);

        ctx->m1_data = compute_snr(ctx->scaling_fft_factor, iter);

        ReleaseMutex(ctx->m1_data_mutex);

        iter++;
    }

    print_string("Finished Processing Data \n");
    SendEvents(ctx->main_task, EVENT_FINISHED_PROCESSING);
    rtems_task_exit();
}

static void Init(rtems_task_argument arg)
{
    (void)arg;
    test_context ctx;

    char str[ITOA_STR_SIZE];

    uint32_t t1_start_time, t1_end_time, t1_elapsed_time;
    uint32_t t2_start_time, t2_end_time, t2_elapsed_time_in_critical_region;

    // With the ICPP we set the ceiling priority to the highest priority of any task that may hold it,
    // In this case it should be T0 with priority PRIO_VERY_HIGH.
    rtems_status_code sc;
    sc = rtems_semaphore_create(rtems_build_name('M', '1', 'M', 'X'), 1,
                                RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY |
                                    PRIO_PROTOCOL_SWITCH,
                                PRIO_VERY_HIGH, &ctx.m1_data_mutex);
    ASSERT_SUCCESS(sc);

    /*
     * Due to the logic of the ICPP, and the fact that the Init task automatically has a priority
     * of 1 (PRIO_VERY_ULTRA_HIGH, the highest possible for any user defined task) we must either decrease
     * the priority of the Init task or increase the priority of the ceiling to be equal to the Init task,
     * and configure the priorities of our defined tasks (T0, T1, T2)
     *
     * For consistency, we will keep our defined tasks with the same priority as in Test Case 13 and reduce the priority
     * of the Init task, which only needs to acquire the semaphore to find the time T2 spends in its critical region
     *
     * See (https://docs.rtems.org/branches/master/c-user/key_concepts.html#immediate-ceiling-priority-protocol-icpp)
     */

    rtems_task_priority task_prio;
    sc = rtems_task_set_priority(RTEMS_SELF, PRIO_VERY_HIGH, &task_prio);
    ASSERT_SUCCESS(sc);

    // FFT Initialization
    (void)memset(&inSensorDataRe[0], 0, FFT_SIZE);
    (void)memset(&inSensorDataIm[0], 0, FFT_SIZE);

    // Calculates mean of Blackman-Harris terms
    ctx.scaling_fft_factor = 0.0;
    for (uint32_t i = 0; i < FFT_SIZE; i++)
    {
        ctx.scaling_fft_factor += blackman_harris(i, FFT_SIZE);
    }

    ctx.scaling_fft_factor = ctx.scaling_fft_factor / ((float)FFT_SIZE);

    // Find length of time T1 takes to execute
    uint32_t res = 0;

    t1_start_time = rtems_clock_get_ticks_since_boot();

    for (uint16_t i = 0; i < T1_TUNE_REPS; i++)
    {
        for (uint8_t p = 2; p < 100; p++)
        {
            if (is_prime(p) && is_mersenne_prime(p))
            {
                res = p;
            }
        }
    }

    t1_end_time = rtems_clock_get_ticks_since_boot();
    t1_elapsed_time = t1_end_time - t1_start_time;

    print_string("[DEBUG] e(T1) is: ");
    print_string(itoa(t1_elapsed_time, &str[0], 10));
    print_string("\n");

    (void)res;

    // Find the amount of time it takes for T2 to complete its critical region on
    // an empirical basis of the max, for any of our given data, to find the upper bound
    // that T0 should wait, enabling us to detect priority inversion
    ctx.t2_max_ticks = -1;

    for (uint8_t i = 0; i < TC_MAX; i++)
    {
        t2_start_time = rtems_clock_get_ticks_since_boot();

        ObtainMutex(ctx.m1_data_mutex);

        ctx.m1_data = compute_snr(ctx.scaling_fft_factor, i);

        ReleaseMutex(ctx.m1_data_mutex);

        t2_end_time = rtems_clock_get_ticks_since_boot();
        t2_elapsed_time_in_critical_region = t2_end_time - t2_start_time;

        ctx.t2_max_ticks = MAX(ctx.t2_max_ticks, t2_elapsed_time_in_critical_region);
    }

    print_string("[DEBUG] MAX eCR(T2) is: ");
    print_string(itoa(ctx.t2_max_ticks, &str[0], 10));
    print_string("\n");

    // Build up our task configs
    rtems_task_config configs[TASK_COUNT];
    for (uint8_t i = 0; i < TASK_COUNT; i++)
    {
        configs[i] = (rtems_task_config){
            .name = rtems_build_name('T', 'S', 'K', (char)i),
            .storage_size = TASK_STORAGE_SIZE,
            .storage_area = &task_storage[i][0],
            .maximum_thread_local_storage_size = MAX_TLS_SIZE,
            .initial_modes = RTEMS_DEFAULT_MODES,
            .attributes = RTEMS_FLOATING_POINT};
    }

    // Initialize relevant test variables
    ctx.main_task = rtems_task_self();
    ctx.priority_inversion_occurrences = 0;
    ctx.period_0_id = INVALID_ID;
    ctx.period_1_id = INVALID_ID;

    // Explicitly set the priorities of our tasks
    configs[2].initial_priority = PRIO_VERY_LOW;
    ctx.task_ids[2] = DoCreateTask(configs[2]);

    configs[1].initial_priority = PRIO_NORMAL;
    ctx.task_ids[1] = DoCreateTask(configs[1]);

    configs[0].initial_priority = PRIO_VERY_HIGH;
    ctx.task_ids[0] = DoCreateTask(configs[0]);

    // Start Tasks
    StartTask(ctx.task_ids[2], gather_meteorological_data, &ctx);
    StartTask(ctx.task_ids[1], decode_communications, &ctx);
    StartTask(ctx.task_ids[0], solar_flare_analysis, &ctx);

    // Blocking wait for relevant event until T2 is finished
    ReceiveAllEvents(EVENT_FINISHED_PROCESSING);

    print_string("Total Priority Inversion occurrences: ");
    print_string(itoa(ctx.priority_inversion_occurrences, &str[0], 10));
    print_string("\n");

    rtems_rate_monotonic_delete(ctx.period_0_id);
    rtems_rate_monotonic_delete(ctx.period_1_id);
    rtems_fatal(RTEMS_FATAL_SOURCE_EXIT, 0);
}

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_PROCESSORS TEST_PROCESSORS

#define CONFIGURE_MAXIMUM_TASKS (TASK_COUNT + 1)

#define CONFIGURE_MAXIMUM_PERIODS 2

#define CONFIGURE_MAXIMUM_SEMAPHORES 1

#define CONFIGURE_MINIMUM_TASK_STACK_SIZE RTEMS_MINIMUM_STACK_SIZE + CPU_STACK_ALIGNMENT

#define CONFIGURE_EXTRA_TASK_STACKS RTEMS_MINIMUM_STACK_SIZE

#define CONFIGURE_IDLE_TASK_STORAGE_SIZE TASK_STORAGE_SIZE
#define CONFIGURE_INIT_TASK_CONSTRUCT_STORAGE_SIZE 2 * TASK_STORAGE_SIZE

#define CONFIGURE_MINIMUM_TASKS_WITH_USER_PROVIDED_STORAGE \
    CONFIGURE_MAXIMUM_TASKS

#define CONFIGURE_MICROSECONDS_PER_TICK 1000

#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 0

#define CONFIGURE_DISABLE_NEWLIB_REENTRANCY

#define CONFIGURE_APPLICATION_DISABLE_FILESYSTEM

#define CONFIGURE_MAXIMUM_THREAD_LOCAL_STORAGE_SIZE MAX_TLS_SIZE

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT_TASK_ATTRIBUTES (RTEMS_SYSTEM_TASK | TASK_ATTRIBUTES)

#define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_DEFAULT_MODES

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
