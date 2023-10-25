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

/* 
 * Calculates the mandebroot set
*/
#include <rtems.h>
#include "trig_tables.h"

#ifdef gr740
  #define TEST_PROCESSORS 4
#endif
#ifdef gr712rc
  #define TEST_PROCESSORS 2
#endif

#define MAXITERATION 255
#define IMAGE_WIDTH 2250
#define IMAGE_HEIGHT 2250

#define LEFT -2.00 //x max value
#define SIDE_X 2.47 //x range (xmax-xmin)
#define TOP 1.12 //y max value
#define SIDE_Y -2.24 //y range (ymax-ymin)
#define SCALE_X (SIDE_X / IMAGE_WIDTH)
#define SCALE_Y (SIDE_Y / IMAGE_HEIGHT)

#define ASSERT_SUCCESS(sc)                            \
  do {                                                \
    if ((sc) != RTEMS_SUCCESSFUL)                     \
    {                                                 \
      print_string(rtems_status_text(sc));              \
      print_string("\n");                               \
      rtems_fatal(RTEMS_FATAL_SOURCE_EXIT, __LINE__); \
    }                                                 \
  } while (0)
  
#define ASSERT_SUCCESS_WITHOUT_FAILING(sc)            \
  do {                                                \
    if ((sc) != RTEMS_SUCCESSFUL)                     \
    {                                                 \
      print_string(rtems_status_text(sc));              \
      print_string("\n");                               \
    }                                                 \
  } while (0)

#define MAX(x, y) (((x) > (y)) ? (x) : (y))

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

void Loop(int16_t x);

uint32_t crc32(const uint8_t *s,size_t n);

void print_test_results(void);

void color(uint8_t red, uint8_t green, uint8_t blue);

size_t str_len (const char *str);

void print_string(const char *string);

void swap (char *x, char *y);

void reverse(char str[], int length);

char* itoa(int num, char* str, int base);

void mandelbrot_tile(uint8_t tile, uint8_t tiles);

bool is_prime(uint64_t p);

bool is_mersenne_prime(uint64_t p);

uint64_t power(uint16_t num, uint16_t exp);

uint32_t isqrt(uint32_t val);

float sin_aprox(float arg);

float cos_aprox(float arg);

float blackman_harris(int n, int N);

void fft(float *inReal, float *inImag, uint32_t log2_N);

float noise_generator(uint32_t seed);
