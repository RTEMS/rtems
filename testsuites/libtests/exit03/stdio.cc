/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2022 Chris Johns (Contemporary Software).  All rights reserved.
 * Copyright (c) 2013 embedded brains GmbH & Co. KG
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <exception>
#include <iostream>

extern "C" {
void cplusplus_stdio(void);
}

static void throw_something() {
  throw std::runtime_error("something");
}

static void throw_and_catch() {
  try {
    throw_something();
    std::cout << "nothing caught exception" << std::endl;
  } catch (std::runtime_error& e) {
    std::cout << "caught exception" << std::endl;
  }
}

void cplusplus_stdio(void) {
  std::set_terminate([](){
    std::cout << "FATAL: terminate called" << std::endl;
    std::abort();
  });
  throw_and_catch();
  std::cout << "IO on std::cout" << std::endl;
  std::cerr << "IO on std::cerr" << std::endl;
}
