/*
 *  M68K Cache Manager Support
 */

#if (defined(__mc68020__) && !defined(__mcpu32__))
# define M68K_INSTRUCTION_CACHE_ALIGNMENT 16
#elif defined(__mc68030__)
# define M68K_INSTRUCTION_CACHE_ALIGNMENT 16
# define M68K_DATA_CACHE_ALIGNMENT 16
#elif ( defined(__mc68040__) || defined (__mc68060__) )
# define M68K_INSTRUCTION_CACHE_ALIGNMENT 16
# define M68K_DATA_CACHE_ALIGNMENT 16
#elif ( defined(__mcf5200__) )
# define M68K_INSTRUCTION_CACHE_ALIGNMENT 16
# if ( defined(__mcf528x__) )
#  define M68K_DATA_CACHE_ALIGNMENT 16
# endif
#elif ( defined(__mcf5300__) )
# define M68K_INSTRUCTION_CACHE_ALIGNMENT 16
# define M68K_DATA_CACHE_ALIGNMENT 16
#elif defined(__mcfv4e__)
# define M68K_INSTRUCTION_CACHE_ALIGNMENT 16
# define M68K_DATA_CACHE_ALIGNMENT 16
#endif

#if defined(M68K_DATA_CACHE_ALIGNMENT)
#define CPU_DATA_CACHE_ALIGNMENT M68K_DATA_CACHE_ALIGNMENT
#endif

#if defined(M68K_INSTRUCTION_CACHE_ALIGNMENT)
#define CPU_INSTRUCTION_CACHE_ALIGNMENT M68K_INSTRUCTION_CACHE_ALIGNMENT
#endif

/*
 *  Since the cacr is common to all mc680x0, provide macros
 *  for masking values in that register.
 */

/*
 *  Used to clear bits in the cacr.
 */
#define _CPU_CACR_AND(mask)                                        \
  {                                                                \
  register unsigned long _value = mask;                            \
  register unsigned long _ctl = 0;                                 \
  __asm__ volatile ( "movec %%cacr, %0;           /* read the cacr */  \
                  andl %2, %0;                /* and with _val */  \
                  movec %1, %%cacr"           /* write the cacr */ \
   : "=d" (_ctl) : "0" (_ctl), "d" (_value) : "%%cc" );            \
  }


/*
 *  Used to set bits in the cacr.
 */
#define _CPU_CACR_OR(mask)                                         \
  {                                                                \
  register unsigned long _value = mask;                            \
  register unsigned long _ctl = 0;                                 \
  __asm__ volatile ( "movec %%cacr, %0;           /* read the cacr */  \
                  orl %2, %0;                 /* or with _val */   \
                  movec %1, %%cacr"           /* write the cacr */ \
   : "=d" (_ctl) : "0" (_ctl), "d" (_value) : "%%cc" );            \
  }


/*
 * CACHE MANAGER: The following functions are CPU-specific.
 * They provide the basic implementation for the rtems_* cache
 * management routines. If a given function has no meaning for the CPU,
 * it does nothing by default.
 */
#if ( (defined(__mc68020__) && !defined(__mcpu32__)) || defined(__mc68030__) )

#if defined(__mc68030__)

/* Only the mc68030 has a data cache; it is writethrough only. */

static inline void _CPU_cache_flush_1_data_line(const void * d_addr) {}
static inline void _CPU_cache_flush_entire_data(void) {}

static inline void _CPU_cache_invalidate_1_data_line(
  const void * d_addr
)
{
  void * p_address = (void *) _CPU_virtual_to_physical( d_addr );
  __asm__ volatile ( "movec %0, %%caar" :: "a" (p_address) );      /* write caar */
  _CPU_CACR_OR(0x00000400);
}

static inline void _CPU_cache_invalidate_entire_data(void)
{
  _CPU_CACR_OR( 0x00000800 );
}

static inline void _CPU_cache_freeze_data(void)
{
  _CPU_CACR_OR( 0x00000200 );
}

static inline void _CPU_cache_unfreeze_data(void)
{
  _CPU_CACR_AND( 0xFFFFFDFF );
}

static inline void _CPU_cache_enable_data(void)
{
  _CPU_CACR_OR( 0x00000100 );
}

static inline void _CPU_cache_disable_data(void)
{
  _CPU_CACR_AND( 0xFFFFFEFF );
}
#endif


/* Both the 68020 and 68030 have instruction caches */

static inline void _CPU_cache_invalidate_1_instruction_line(
  const void * d_addr
)
{
  void * p_address = (void *) _CPU_virtual_to_physical( d_addr );
  __asm__ volatile ( "movec %0, %%caar" :: "a" (p_address) ); /* write caar */
  _CPU_CACR_OR( 0x00000004 );
}

static inline void _CPU_cache_invalidate_entire_instruction(void)
{
  _CPU_CACR_OR( 0x00000008 );
}

static inline void _CPU_cache_freeze_instruction(void)
{
  _CPU_CACR_OR( 0x00000002);
}

static inline void _CPU_cache_unfreeze_instruction(void)
{
  _CPU_CACR_AND( 0xFFFFFFFD );
}

static inline void _CPU_cache_enable_instruction(void)
{
  _CPU_CACR_OR( 0x00000001 );
}

static inline void _CPU_cache_disable_instruction(void)
{
  _CPU_CACR_AND( 0xFFFFFFFE );
}


#elif ( defined(__mc68040__) || defined (__mc68060__) )

/* Cannot be frozen */
static inline void _CPU_cache_freeze_data(void) {}
static inline void _CPU_cache_unfreeze_data(void) {}
static inline void _CPU_cache_freeze_instruction(void) {}
static inline void _CPU_cache_unfreeze_instruction(void) {}

static inline void _CPU_cache_flush_1_data_line(
  const void * d_addr
)
{
  void * p_address = (void *) _CPU_virtual_to_physical( d_addr );
  __asm__ volatile ( "cpushl %%dc,(%0)" :: "a" (p_address) );
}

static inline void _CPU_cache_invalidate_1_data_line(
  const void * d_addr
)
{
  void * p_address = (void *) _CPU_virtual_to_physical( d_addr );
  __asm__ volatile ( "cinvl %%dc,(%0)" :: "a" (p_address) );
}

static inline void _CPU_cache_flush_entire_data(void)
{
  __asm__ volatile ( "cpusha %%dc" :: );
}

static inline void _CPU_cache_invalidate_entire_data(void)
{
  __asm__ volatile ( "cinva %%dc" :: );
}

static inline void _CPU_cache_enable_data(void)
{
  _CPU_CACR_OR( 0x80000000 );
}

static inline void _CPU_cache_disable_data(void)
{
  _CPU_CACR_AND( 0x7FFFFFFF );
}

static inline void _CPU_cache_invalidate_1_instruction_line(
  const void * i_addr )
{
  void * p_address = (void *)  _CPU_virtual_to_physical( i_addr );
  __asm__ volatile ( "cinvl %%ic,(%0)" :: "a" (p_address) );
}

static inline void _CPU_cache_invalidate_entire_instruction(void)
{
  __asm__ volatile ( "cinva %%ic" :: );
}

static inline void _CPU_cache_enable_instruction(void)
{
  _CPU_CACR_OR( 0x00008000 );
}

static inline void _CPU_cache_disable_instruction(void)
{
  _CPU_CACR_AND( 0xFFFF7FFF );
}
#endif
