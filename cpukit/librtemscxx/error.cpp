/* SPDX-License-Identifier: BSD-2-Clause */

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

#include <rtems/error.hpp>

namespace rtems
{
  runtime_error::runtime_error(const rtems_status_code sc)
    : std::runtime_error(::rtems_status_text(sc)),
      sc(sc)
  {
  }

  runtime_error::runtime_error(const rtems_status_code sc,
                               const std::string&      what)
    : std::runtime_error(what + ": " + ::rtems_status_text(sc)),
      sc(sc)
  {
  }

  runtime_error::runtime_error(const rtems_status_code sc,
                               const char*             what)
    : std::runtime_error(std::string(what) + ": " + ::rtems_status_text(sc)),
      sc(sc)
  {
  }

  runtime_error::~runtime_error()
  {
  }

  void
  runtime_error_check(const rtems_status_code sc)
  {
    if (sc != RTEMS_SUCCESSFUL) {
      throw runtime_error(sc);
    }
  }

  void
  runtime_error_check(const rtems_status_code sc, const std::string& what)
  {
    if (sc != RTEMS_SUCCESSFUL) {
      throw runtime_error(sc, what);
    }
  }

  void
  runtime_error_check(const rtems_status_code sc, const char* what)
  {
    if (sc != RTEMS_SUCCESSFUL) {
      throw runtime_error(sc, what);
    }
  }
};
