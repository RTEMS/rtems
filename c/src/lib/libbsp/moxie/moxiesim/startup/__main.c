/*
 *  COPYRIGHT (c) 2011 Anthony Green <green@moxielogic.com>
 *
 */

typedef void (*pfunc) (void);
extern pfunc __ctors[];
extern pfunc __ctors_end[];

void __main (void)
{
  static int initialized;
  pfunc *p;

  if (initialized)
    return;

  initialized = 1;
  for (p = __ctors_end; p > __ctors; )
    (*--p) ();
}
