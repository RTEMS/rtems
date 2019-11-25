#include "dl-load.h" /* make the symbol a C linkage */

#include <stdexcept>

#include <rtems/test.h>

#define printf(...) rtems_printf(&rtems_test_printer, __VA_ARGS__);

void exception_dl(bool throw_runtime)
{
  printf("exception_dl: begin\n");
  try
  {
    printf("exception_dl: throwing...\n");
    if (throw_runtime)
      throw std::runtime_error("throw std::runtime_error object");
    else
      throw dl_test_throw_me("throw dl_test_throw_me object");
  }
  catch (dl_test_throw_me const& e)
  {
    printf("%s: caught: %s\n", __func__, e.what());
  }
  catch (std::exception const& e)
  {
    printf("%s: caught: %s\n", __func__, e.what());
  }
  catch (...)
  {
    printf("%s: caught: unknown\n", __func__);
  }
  printf("exception_dl: end\n");
}
