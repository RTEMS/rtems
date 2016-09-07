#include <cstdio>
#include <stdexcept>
#include "dl-load.h" /* make the symbol a C linkage */
void exception_dl(void)
{
  try
  {
    printf("exception_dl: throwing\n");
    throw std::runtime_error("exception_dl throw");
  }
  catch (std::exception const& e)
  {
    printf("%s: caught: %s\n", __func__, e.what());
  }
  catch (...)
  {
    printf("%s: caught: unknown\n", __func__);
  }
}
