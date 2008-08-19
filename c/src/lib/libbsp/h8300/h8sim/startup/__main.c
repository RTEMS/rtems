/*
 *  Copied from newlib 1.8.2
 *
 *  $Id$
 */

void __main (void)
{
  static int initialized;
  if (! initialized)
    {
      typedef void (*pfunc) (void);
      extern pfunc __ctors[];
      extern pfunc __ctors_end[];
      pfunc *p;

      initialized = 1;
      for (p = __ctors_end; p > __ctors; )
	(*--p) ();

    }
}
