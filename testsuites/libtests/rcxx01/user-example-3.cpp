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

#include <atomic>
#include <chrono>
#include <iostream>
#include <mutex>

#include <rtems/thread.hpp>

class ticker
{
  public:
    ticker();

    void start(const size_t life_time);
    bool finished();

    size_t seconds();

  private:
    void timer(const size_t life_time);

    rtems::thread::thread tocker;
    std::mutex            lock;
    std::atomic<bool>     done;
    bool                  running;
    std::atomic<size_t>   the_seconds;
};

ticker::ticker()
  : done(false),
    running(false),
    the_seconds(0)
{
}

void ticker::start(const size_t life_time)
{
  std::lock_guard<std::mutex> guard(lock);
  if (!running) {
    rtems::thread::attributes attr;
    attr.set_name("CLCK");
    attr.set_stack_size(8 * 1024);
    attr.set_priority(10);
    running = true;
    tocker = rtems::thread::thread(attr, &ticker::timer, this, life_time);
  }
}

bool ticker::finished()
{
  return done.load();
}

size_t ticker::seconds()
{
  return the_seconds.load();
}

void ticker::timer(const size_t life_time)
{
  while (the_seconds.load() < life_time) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    the_seconds++;
  }
  done = true;
  std::lock_guard<std::mutex> guard(lock);
  running = false;
}

void example_3()
{
  std::cout << "Start example 3" << std::endl;

  ticker my_ticker;

  my_ticker.start(5);

  while (!my_ticker.finished()) {
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << "Ticker is " << my_ticker.seconds() << std::endl;
  }

  std::cout << "End example 3" << std::endl;
}
