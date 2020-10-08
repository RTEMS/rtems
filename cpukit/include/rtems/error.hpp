/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSC++
 *
 * @brief RTEMS Error exception.
 *
 * Provide an error exception for RTEMS errors.
 */

/*
 * Copyright (C) 2020 Chris Johns (http://contemporary.software)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#if !defined(RTEMS_ERROR_HPP)
#define RTEMS_ERROR_HPP

#include <stdexcept>
#include <string>

#include <rtems/error.h>

namespace rtems
{
  class runtime_error :
    public std::runtime_error
  {
    const rtems_status_code sc;
  public:
    runtime_error(const rtems_status_code sc);
    runtime_error(const rtems_status_code sc, const std::string& what);
    runtime_error(const rtems_status_code sc, const char* what);
    ~runtime_error();
  };

  /**
   * Throw a rtems::runtime_error exception if the RTEMS status code is
   * not RTEMS_SUCCESSFUL.
   */
  void runtime_error_check(const rtems_status_code sc);
  void runtime_error_check(const rtems_status_code sc, const std::string& what);
  void runtime_error_check(const rtems_status_code sc, const char* what);
};

#endif
