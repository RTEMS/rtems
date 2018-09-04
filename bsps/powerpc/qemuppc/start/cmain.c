#include <bsp/bootcard.h>
#include <bsp/linker-symbols.h>

static void
__outb(int port, unsigned char v)
{
  *((volatile unsigned char *)(0x80000000 + port)) = v;
}

#if 0
/* currently unused but keep just in case */

static unsigned char
__inb(int port)
{
  return *((volatile unsigned char *)(0x80000000 + port));
}
#endif

static void
__memcpy (void *dv, void *sv, size_t len)
{
  unsigned char *d = (unsigned char *) dv;
  unsigned char *s = (unsigned char *) sv;

  while (len--)
    *d++ = *s++;
}

static void
__bzero (void *dv, size_t len)
{
  unsigned char *d = (unsigned char *) dv;

  while (len--)
    *d++ = 0;
}


/*
 * Prototype this here because it is just the entry symbol and
 * not referenced from any compileable code.
 */
void cmain (void);

void cmain (void)
{
  /*
   * init variable sections
   */
  __memcpy(
    (char *)bsp_section_data_begin,
    (char *) bsp_section_data_load_begin,
    (int)bsp_section_data_size
   );
  __bzero((char *)bsp_section_bss_begin, (int)bsp_section_bss_size);
  __bzero((char *)bsp_section_sbss_begin, (int)bsp_section_sbss_size);
  /* printk( "start of BSP\n"); */
  boot_card(0);
  /* printk( "end of BSP\n"); */
  __outb (0x92, 0x01);
  while (1)
    ;
}
