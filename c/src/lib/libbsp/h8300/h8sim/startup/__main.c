/*
 *  Copied from newlib 1.8.2
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
