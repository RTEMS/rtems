/*
 * The base image needs this to include the RTTI data.
 */
#include <cstdio>
#include <stdexcept>
#include "dl-load.h"
void exception_base(bool istrue)
{
  printf("exception_base called\n");
  if (istrue)
  {
    throw std::runtime_error("dummy call to link in symbols");
  }
}
