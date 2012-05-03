static void
__outb(int port, unsigned char v)
{
  *((volatile unsigned char *)(0x80000000 + port)) = v;
}

static unsigned char
__inb(int port)
{
  return *((volatile unsigned char *)(0x80000000 + port));
}

static void
__memcpy (unsigned char *d, unsigned char *s, int len)
{
  while (len--)
    *d++ = *s++;
}

static void
__bzero (unsigned char *d, int len)
{
  while (len--)
    *d++ = 0;
}

extern unsigned char __sdata2_load[], __sdata2_start[], __sdata2_end[];
extern unsigned char __data_load[], __data_start[], __data_end[];
extern unsigned char __sdata_load[], __sdata_start[], __sdata_end[];
extern unsigned char __sbss2_start[], __sbss2_end[];
extern unsigned char __sbss_start[], __sbss_end[];
extern unsigned char __bss_start[], __bss_end[];

extern void boot_card(void *);


void cmain (void)
{
  /*
   * init variable sections
   */
  __memcpy (__sdata2_start, __sdata2_load, __sdata2_end - __sdata2_start);
  __memcpy (__sdata_start , __sdata_load , __sdata_end  - __sdata_start);
  __memcpy (__data_start  , __data_load  , __data_end   - __data_start);
  __bzero (__sbss2_start  , __sbss2_end - __sbss2_start);
  __bzero (__sbss_start   , __sbss_end  - __sbss_start);
  __bzero (__bss_start    , __bss_end   - __bss_start);
  /* printk( "start of BSP\n"); */
  boot_card(0);
  /* printk( "end of BSP\n"); */
  __outb (0x92, 0x01);
  while (1)
    ;
}
