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

#include <chrono>
#include <iostream>

#include <rtems/thread.hpp>

static void wait_for(std::string me, size_t seconds, size_t announce)
{
  size_t count = 0;
  while (count < seconds) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    if ((count % announce) == 0)
      std::cout << me << ": " << count << std::endl;
    count++;
  }
}

void example_2()
{
  std::cout << "Start example 2" << std::endl;

  rtems::thread::attributes attr;
  attr.set_stack_size(16 * 1024);

  attr.set_priority(100);
  attr.set_name("T1");
  rtems::thread::thread t1(attr, wait_for, "T1", 10, 1);

  attr.set_priority(101);
  attr.set_name("T2");
  rtems::thread::thread t2(attr, wait_for, "T2", 10, 2);

  attr.set_priority(attr.get_priority());
  attr.set_name("T3");
  rtems::thread::thread t3(attr, wait_for, "T3", 15, 3);

  t1.join();
  t2.join();
  t3.join();

  std::cout << "End example 2" << std::endl;
}
