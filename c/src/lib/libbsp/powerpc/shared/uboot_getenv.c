/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

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


