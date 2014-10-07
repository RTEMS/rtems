/*
 * From newlib ==> libc/sys/sysnecv850/crt1.c
 *
 * Obtained from newlib: 29 May 2012
 * Warnings fixed: 7 October 2014
 */

void __main(void);
typedef void (*pfunc) (void);
extern pfunc __ctors[];
extern pfunc __ctors_end[];

void __main(void)
{
  static int initialized;
  if (! initialized)
    {
      pfunc *p;

      initialized = 1;
      for (p = __ctors_end; p > __ctors; )
        (*--p) ();

    }
}
