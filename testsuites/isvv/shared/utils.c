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

#include <rtems/bspIo.h>
#include <string.h>
#include "utils.h"
#include "isvv_rtems_aux.h"

uint8_t TEST_SET[IMAGE_WIDTH][IMAGE_HEIGHT];

void Loop(int16_t x) {
  volatile int i;
  for (i=0; i<100000* x; i++);
}

uint32_t crc32(const uint8_t *s,size_t n)
{
  uint32_t crc=0xFFFFFFFF;
  for(size_t i=0;i<n;i++) {
    char ch=s[i];
    for(size_t j=0;j<8;j++) {
      uint32_t b=(ch^crc)&1;
      crc>>=1;
      if(b) crc=crc^0xEDB88320;
      ch>>=1;
    }
  }

  return ~crc;
}

/* to print results  */
void print_test_results(void)
{
  char str[8*sizeof(uint32_t)+1];
  uint32_t crc = crc32((uint8_t *)&TEST_SET[0][0], sizeof(TEST_SET));
  print_string("Begin set {\n");
  print_string("0x");
  print_string(itoa(crc, &str[0], 16));
  // print_string("P6 \n");
  // print_string(itoa(IMAGE_WIDTH, &str[0], 10));
  // print_string(" ");
  // print_string(itoa(IMAGE_HEIGHT, &str[0], 10));
  // print_string("\n255\n");
  // for(int j= 0; j<IMAGE_HEIGHT; j++ )
  // {
  //   for(int i = 0; i<IMAGE_WIDTH; i++ )
  //   {
  //     if (TEST_SET[i][j] < 16)
  //     {
  //       print_string("0");
  //       print_string(itoa(TEST_SET[i][j], &str[0], 16));
  //     }
  //     else
  //       print_string(itoa(TEST_SET[i][j], &str[0], 16));
  //     // print_string((char)TEST_SET[i][j]);
  //     //block of code to generate output to a .ppm file
  //     // if (TEST_SET[i][j]<50) color(0,0,0);
  //     // else if( TEST_SET[i][j]<100) color(5,5,5);
  //     // else if (TEST_SET[i][j]<150) color(0,0,255);
	//     // else if (TEST_SET[i][j]<255) color(255,255,255);
  //     // else color(180,0,0);
  //   }
  // }
  print_string("\n} End set\n");
}

//function to print on unsigned char rgb format to generate the 
//output to a .ppm file
void color(uint8_t red, uint8_t green, uint8_t blue) 
{
  BSP_output_char((unsigned char)red);
  BSP_output_char((unsigned char)green);
  BSP_output_char((unsigned char)blue);
}

// return the lentgh of a string
size_t str_len (const char *str)
{
	const char *s = str;
	while (*s) s++;
	return s - str;
}

//print a string
void print_string(const char *string)
{
  char *s;
  size_t i=0;
  s = (char *) string;

  while (i<str_len(s))
  {
    BSP_output_char(s[i]);
    ++i;
  }
}

// swap two chars
void swap (char *x, char *y)
{
  char temp = *x;
  *x = *y;
  *y = temp;
}


// function to reverse the order of an array/a string
void reverse(char str[], int length)
{
  int start = 0;
  int end = length - 1;
  while (start < end)
  {
    swap(&str[start], &str[end]);
    start++;
    end--;
  }
}

// Implementation of itoa()
char* itoa(int num, char* str, int base)
{
  uint32_t i = 0;
  bool isNegative = false;
  uint32_t u32num = (uint32_t) num;
  uint32_t u32base = (uint32_t) base;

  /* Handle 0 explicitly, otherwise empty string is printed for 0 */
  if (num == 0)
  {
    str[i++] = '0';
    str[i] = '\0';
    return str;
  }

  // In standard itoa(), negative numbers are handled only with
  // base 10. Otherwise numbers are considered unsigned.
  if (num < 0 && base == 10)
  {
    // -2^31 is a special case, and needs to be treated diferently
    if (num == -2147483648) 
    {
       memcpy(str,"-2147483648", str_len("-2147483648"));
       return str;
    }
    isNegative = true;
    u32num = (uint32_t) (-num);
  }

  // Process individual digits
  while (u32num != 0)
  {
    uint32_t u32rem = u32num % u32base;
    str[i++] = (u32rem > 9)? (u32rem-10) + 'a' : u32rem + '0';
    u32num = u32num/u32base;
  }

  // If number is negative, append '-'
  if (isNegative)
    str[i++] = '-';

  str[i] = '\0'; // Append string terminator

  // Reverse the string
  reverse(str, str_len(str));

  return str;
}

// Function to calc mandelbrot set
void mandelbrot_tile(uint8_t tile, uint8_t tiles)
{
  float zx, zy, x_zero, tempx, y_zero;
  uint16_t iteration, y_min_tile, y_max_tile;

  // setting first and last point of the tile
  y_min_tile = ( IMAGE_HEIGHT/tiles ) * (tile - 1);
  y_max_tile = tile < tiles ? (IMAGE_HEIGHT / tiles) * tile : IMAGE_HEIGHT;

  // scanning every point in the tile
  for (uint16_t x = 0; x < IMAGE_WIDTH; x++)
  {
    x_zero = (x * SCALE_X)+ LEFT;
    for (uint16_t y = y_min_tile; y < y_max_tile; y++) 
    {
      //scaling to lie in the mandelbrot scale
      y_zero = (y * SCALE_Y) + TOP;

      //iterations need to be reset before calculations for each point
      zx = 0;
      zy = 0;
      iteration = 0;

      // Calculate whether belongs to the Mandelbrot set or not 
      while ((zx * zx + zy * zy < 4) && (iteration < MAXITERATION))
      {
        // Calculate Mandelbrot function
        // z = z*z + c where z is a complex number
        tempx = zx * zx - zy * zy + x_zero;
        zy = 2 * zx * zy + y_zero;
        zx = tempx;

        iteration = iteration + 1;
      }
      TEST_SET[x][y]=iteration;
    }
  }
}

// Dumb implementation of primality test
bool is_prime(uint64_t p)
{
  if (2 == p)
    return TRUE;
  else if ((1 >= p) || (0 == (p % 2)))
    return FALSE;
  else
  {
    bool prime = TRUE;
    const uint32_t to = p / 2;
    for (uint32_t i = 3; i <= to; i += 2)
    {
      if (0 == (p % i)) {
        prime = false;
        break;
      }
    }
    return prime;
  }
}

bool is_mersenne_prime(uint64_t p)
{
  if (2 == p)
    return TRUE;
  else if (p >= 64)
  {
    // Using this to make sure we don't run into any undefined behaviour
    // When left-shifting bits by greater than the set amount
    // The object of this function isn't necessarily to get the correct value for any given integer
    // Just to stress the the CPU while utilising lots of execution time to assert robustness
    return false;
  }
  else
  {
    const uint64_t m_p = (1LLU << p) - 1;
    uint64_t s = 4;
    uint64_t i;
    for (i = 3; i <= p; i++)
    {
      s = (s * s - 2) % m_p;
    }
    return 0 == s;
  }
}

// Dumb implementation of power function for small numbers
uint64_t power (uint16_t num, uint16_t exp) {
    uint64_t total = 1;
    for (uint16_t i = 0; i < exp; i++) {
      total *= num;
    }
    return total;
}

uint32_t isqrt(uint32_t val) {
  uint32_t temp, g = 0U, b = 0x8000U, bshft = 15U;
  do {
    if (val >= (temp = (((g << 1) + b)<<bshft--))) {
	  g += b;
	  val -= temp;
	}
  } while (b >>= 1);
  return g;
}


//-----------------------------------------------------------------------------
// Functions related with signal processing
//-----------------------------------------------------------------------------

static void swapf(float *x, float *y){
  float temp = *x;
  *x = *y;
  *y = temp;
}

static uint32_t bits_reverse(uint32_t val, int width) {
  uint32_t result = 0;
  for (int i = 0; i < width; i++, val >>= 1)
    result = (result << 1) | (val & 1U);
  return result;
}

float cos_aprox(float arg) {
  // arg must be between 0 and 2*PI
  uint32_t idx = (uint32_t)(arg/(2*PI)*FFT_TABLE_SIZE);
  float    ro  = (arg/(2*PI)*FFT_TABLE_SIZE)-(float)idx; 
  // using liner interpolation in the case that the given arg does not match exactly with a direct table entry
  return cosTable[idx%FFT_TABLE_SIZE] + ro*(cosTable[(idx+1)%FFT_TABLE_SIZE]-cosTable[idx%FFT_TABLE_SIZE]);
}

float sin_aprox(float arg) {
  // arg must be between 0 and 2*PI
  uint32_t idx = (uint32_t)(arg/(2*PI)*FFT_TABLE_SIZE);
  float    ro  = (arg/(2*PI)*FFT_TABLE_SIZE)-(float)idx;    
  // using liner interpolation in the case that the given arg does not match exactly with a direct table entry
  return sinTable[idx%FFT_TABLE_SIZE] + ro*(sinTable[(idx+1)%FFT_TABLE_SIZE]-sinTable[idx%FFT_TABLE_SIZE]);
}

float blackman_harris(int n, int N){
  float a0, a1, a2, a3, seg1, seg2, seg3, w_n;
  a0 = 0.35875f;
  a1 = 0.48829f;
  a2 = 0.14128f;
  a3 = 0.01168f;

  seg1 = a1 * (float) cos_aprox((float)(2*PI*n)/(float) (N - 1));
  seg2 = a2 * (float) cos_aprox((float)(4*PI*n)/(float) (N - 1));
  seg3 = a3 * (float) cos_aprox((float)(6*PI*n)/(float) (N - 1));

  w_n = a0 - seg1 + seg2 - seg3;

  return w_n;
}

void fft( float *inReal, float *inImag, uint32_t log2_N ){
  const uint32_t N = 1 << log2_N;
    
  // Bits-reversed
  for (uint32_t i = 0; i < N; i++) {
    uint32_t j = bits_reverse(i, log2_N);
    if (j > i) {
      swapf(&inReal[i], &inReal[j]);
      swapf(&inImag[i], &inImag[j]);
    }
  }   
    
  // Cooley-Tukey radix-2 FFT
  for (uint32_t size = 2; size <= N; size *= 2) {
    for (uint32_t i = 0; i < N; i += size) {
      uint32_t k=0;
      for (uint32_t j = i; j < i + size/2; j++) {               
        float a_real        = inReal[j];
        float a_imag        = inImag[j];
        float b_real        = inReal[j+size/2];
        float b_imag        = inImag[j+size/2];
        float twiddle_real  = cos_aprox(2.0*PI*(float)k/(float)N);
        float twiddle_imag  = sin_aprox(2.0*PI*(float)k/(float)N);

        float bias_real     =  b_real * twiddle_real + b_imag * twiddle_imag;
        float bias_imag     = -b_real * twiddle_imag + b_imag * twiddle_real;
        inReal[j+size/2]    = a_real - bias_real;
        inImag[j+size/2]    = a_imag - bias_imag;
        inReal[j]           = a_real + bias_real;
        inImag[j]           = a_imag + bias_imag;
        k += N/size;
      }
    }
  }
}


float noise_generator(uint32_t seed) {
  static uint32_t lfsr = 0xE3F5828U;
  if (seed != 0) { lfsr = seed; }
  /* taps:  32,22,2,1; feedback polynomial    : x^32 + x^22 + x^2 + x^1 + 1 */
  uint32_t bit = ((lfsr >> 0) ^ (lfsr >> 10) ^ (lfsr >> 30) ^ (lfsr >> 31)) & 1u;
  lfsr = (lfsr >> 1) | (bit << 31);
  return (((float)lfsr / (float)UINT32_MAX) - 0.5f);
}

