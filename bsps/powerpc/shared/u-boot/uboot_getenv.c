/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
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

#include <bsp.h>

/*
 * If the BSP variant does not have UBoot, then disable all contents
 * of this file.
 */
#if defined(HAS_UBOOT)

#include <stdint.h>
#include <string.h>

/* Assumed to be provided by BSP */
extern const uint8_t *uboot_environment;
extern const size_t uboot_environment_size;

/*
 * The U-Boot source code appears to use the CRC32 code from zlib.
 * But I cannot find a way to get the crc32() in zlib code to
 * generate the CRC found in the Flash on the Icecube board.
 * So for now, always return TRUE.
 */
static int bsp_uboot_environ_check_crc(void)
{
#if 0
  unsigned long crc;
  unsigned long max;

  for (max=0 ; max <= 0x20000 ; max+=4 ) {
    crc = crc32( 0, NULL, 0 );
    crc = crc32( crc, &uboot_environment[4], max);
    printk( "crc=0x%08lx need %0x max=%d\n", crc,
            *(int *)uboot_environment, max  );
  }
#endif
  return 1;
}

const char *bsp_uboot_getenv(
  const char *name
)
{
  char   lhs[64];
  size_t i, j;

  if ( !bsp_uboot_environ_check_crc() )
    return NULL;

  for ( i=4 ; i<uboot_environment_size ; i++ ) {
    memset( lhs, '\0', sizeof(lhs) );
    for( j=0 ; uboot_environment[i] != '=' && j<sizeof(lhs) ; i++, j++ ) {
      lhs[j] = uboot_environment[i];
    }
    if ( !strncmp( name, lhs, sizeof(lhs) ) ) {
      return (const char *)&uboot_environment[i+1];
    }

    for ( i++ ; uboot_environment[i] && i<uboot_environment_size ; i++ )
      ;
    if ( !uboot_environment[i+1] )
      return NULL;
  }
  return NULL;
}

#endif /* defined(HAS_UBOOT) */
