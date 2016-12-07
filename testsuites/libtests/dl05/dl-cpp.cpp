/*
 * The base image needs this to include the RTTI data.
 */
#include <cstdio>
#include <stdexcept>
#include "dl-load.h"
void exception_base(bool throw_runtime)
{
  printf("%s: begin\n", __func__);
  try
  {
    if (throw_runtime)
      throw std::runtime_error("eb: throw std::runtime_error");
    else
      throw dl_test_throw_me("eb: throw me");
  }
  catch (std::exception const& e)
  {
    printf("%s: caught: %s\n", __func__, e.what());
  }
  catch (dl_test_throw_me const& e)
  {
    printf("%s: caught: %s\n", __func__, e.what());
  }
  catch (...)
  {
    printf("%s: caught: unknown\n", __func__);
  }
  printf("%s: end\n", __func__);
}
