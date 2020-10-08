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
#include <thread>
#include <mutex>

#include <rtems/thread.hpp>

using namespace std::chrono_literals;

extern "C" void rcxx_run_test(void);

void example_1();
void example_2();
void example_3();

struct test_thread
{
  test_thread();

  void start();
  bool running();
  void body(const char* title, int i, char c);

  rtems::thread::thread thread_default;
  rtems::thread::thread thread_attr;

  std::mutex mutex;

  bool finished;
};

test_thread::test_thread()
  : finished(false)
{
}

void test_thread::start()
{
  thread_default = rtems::thread::thread(&test_thread::body, this,
                                         "default",  1, 'D');

  rtems::thread::attributes attr;

  attr.set_name("RTHREAD");
  attr.set_priority(5);
  attr.set_stack_size(32 * 1024);

  thread_attr = rtems::thread::thread(attr, &test_thread::body, this,
                                      "attr", 2, 'R');
}

void test_thread::body(const char* title, int i, char c)
{
  std::cout << "Thread: start: " << title << std::endl
            << ' ' << i << ' ' << c << std::endl;

  size_t count = 5;

  while (count--) {
    std::this_thread::sleep_for(1s);
  }

  std::cout << "Thread: end: " << title << std::endl;

  std::lock_guard<std::mutex> lock(mutex);

  finished = true;
}

bool test_thread::running()
{
  std::lock_guard<std::mutex> lock(mutex);
  return finished == false;
}

void test_1()
{
  test_thread tt;
  tt.start();
  while (tt.running())
    std::this_thread::sleep_for(1s);
}

void rcxx_run_test(void)
{
  try {
    test_1();
    /* From the user manual */
    example_1();
    example_2();
    example_3();
  } catch (...) {
    std::cout << "Thread: ouch" << std::endl;
    throw;
  }
}
