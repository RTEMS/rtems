#include <rtems.h>
#include <rtems/mips/idtcpu.h>
#include <stdlib.h>
#include <string.h>

#include <libcpu/isr_entries.h>

void mips_install_isr_entries( void )
{
#if __mips == 1
  memcpy( (void *)UT_VEC, exc_utlb_code, 40 );  /* utlbmiss vector */
  memcpy( (void *)DB_VEC, exc_dbg_code, 40 );
  memcpy( (void *)E_VEC, exc_norm_code, 40 );   /* exception vevtor */

#elif __mips == 32
  memcpy( (void *)T_VEC, exc_tlb_code, 40 );    /* tlbmiss vector */
  memcpy( (void *)X_VEC, exc_xtlb_code, 40 );   /* xtlbmiss vector */
  memcpy( (void *)C_VEC, exc_cache_code, 40 );  /* cache error vector */
  memcpy( (void *)E_VEC, exc_norm_code, 40 );   /* exception vector */

#elif __mips == 3
  memcpy( (void *)T_VEC, exc_tlb_code, 40 );    /* tlbmiss vector */
  memcpy( (void *)X_VEC, exc_xtlb_code, 40 );   /* xtlbmiss vector */
  memcpy( (void *)C_VEC, exc_cache_code, 40 );  /* cache error vector */
  memcpy( (void *)E_VEC, exc_norm_code, 40 );   /* exception vector */
#endif

  rtems_cache_flush_entire_data();
}
