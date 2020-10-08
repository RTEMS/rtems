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

#if !defined(_GNU_SOURCE)
#define _GNU_SOURCE
#endif

#include <system_error>

#include <rtems/error.hpp>
#include <rtems/thread.hpp>

#include <pthread.h>

#include <rtems.h>

#if HAVE_GET_SCHEDULER_NAME
extern "C" bool get_scheduler_name(rtems_id sid, char* name);
#endif

#if HAVE_GET_SCHEDULER_NAME
bool get_scheduler_name(rtems_id sid, char* name)
{
  name[0] = 'N';
  name[1] = 'O';
  name[2] = 'P';
  name[3] = '\0';
  return true;
}
#endif

namespace rtems
{
  namespace thread
  {
    void
    system_error_check(int ec, const char* what)
    {
      if (ec != 0) {
        throw std::system_error(ec, std::system_category(), what);
      }
    }

    attributes::attributes()
      : priority(-1),
        stack_size(RTEMS_MINIMUM_STACK_SIZE),
        attr(sched_inherit),
        policy(sched_fifo)
    {
      update();
    }

    attributes::attributes(const attributes& attr)
      : name(attr.name),
        priority(attr.priority),
        stack_size(attr.stack_size),
        scheduler(attr.scheduler),
        attr(attr.attr),
        policy(attr.policy)
    {
    }

    void
    attributes::set_name(const std::string& name_)
    {
      name = name_;
    }

    void
    attributes::set_name(const char* name_)
    {
      name = name_;
    }

    const std::string&
    attributes::get_name() const
    {
      return name;
    }

    void
    attributes::set_priority(int priority_)
    {
      priority = priority_;
    }

    int
    attributes::get_priority() const
    {
      return priority;
    }

    void
    attributes::set_stack_size(size_t size)
    {
      stack_size = size;
    }

    size_t
    attributes::get_stack_size() const
    {
      return stack_size;
    }

    void
    attributes::set_scheduler(const std::string& scheduler_)
    {
      scheduler = scheduler_;
    }

    void
    attributes::set_scheduler(const char* scheduler_)
    {
      scheduler = scheduler_;
    }

    const std::string&
    attributes::get_scheduler()
    {
      return scheduler;
    }

    attributes::sched_attr
    attributes::get_scheduler_attr() const
    {
      return attr;
    }

    void
    attributes::set_scheduler_policy(sched_policy policy_)
    {
      attr = sched_explicit;
      policy = policy_;
    }

    attributes::sched_policy
    attributes::get_scheduler_policy() const
    {
      return policy;
    }

    void
    attributes::commit()
    {
      pthread_t pid = ::pthread_self();

      system_error_check(::pthread_setname_np(pid, name.c_str()),
                         "getting name");

      int                spolicy;
      struct sched_param sched_param;

      system_error_check(::pthread_getschedparam(::pthread_self(),
                                                 &spolicy,
                                                 &sched_param),
                         "getting scheduler parameters");

      switch (policy) {
      case sched_other:
        spolicy = SCHED_OTHER;
        break;
      case sched_fifo:
        spolicy = SCHED_FIFO;
        break;
      case sched_roundrobin:
        spolicy = SCHED_RR;
        break;
      case sched_sporadic:
        spolicy = SCHED_SPORADIC;
        break;
      default:
        system_error_check(EINVAL, "get scheduler policy");
        break;
      }

      sched_param.sched_priority = priority;

      system_error_check(::pthread_setschedparam(::pthread_self(),
                                                 spolicy,
                                                 &sched_param),
                         "getting scheduler parameters");

      if (!scheduler.empty()) {
        char sname[4] = { ' ', ' ', ' ', ' ' };
        for (size_t c = 0; c < sizeof(sname); ++c) {
          if (c >= scheduler.length()) {
            break;
          }
          sname[c] = scheduler[c];
        }
        rtems_name scheduler_name = rtems_build_name(sname[0],
                                                     sname[1],
                                                     sname[2],
                                                     sname[3]);
        rtems_id scheduler_id;
        runtime_error_check(::rtems_scheduler_ident(scheduler_name,
                                                    &scheduler_id),
                            "get scheduler id");
        // runtime_error_check (::rtems_task_set_scheduler (RTEMS_SELF,
        //                                                  scheduler_id,
        //                                                  1),
        //                      "set scheduler id");
      }
    }

    void
    attributes::update()
    {
      char buf[32];
      system_error_check(::pthread_getname_np(::pthread_self(),
                                              buf,
                                              sizeof (buf)),
                         "getting name");
      name = buf;

      int                spolicy;
      struct sched_param sched_param;
      system_error_check(::pthread_getschedparam(::pthread_self(),
                                                 &spolicy,
                                                 &sched_param),
                         "getting scheduler parameters");

      switch (spolicy) {
      case SCHED_OTHER:
        policy = sched_other;
        break;
      case SCHED_FIFO:
        policy = sched_fifo;
        break;
      case SCHED_RR:
        policy = sched_roundrobin;
        break;
      case SCHED_SPORADIC:
        policy = sched_sporadic;
        break;
      default:
        system_error_check(EINVAL, "get scheduler policy");
        break;
      }
      priority = sched_param.sched_priority;

      pthread_attr_t pattr;
      system_error_check(::pthread_getattr_np(::pthread_self(), &pattr),
                         "getting thread attributes");
      system_error_check(::pthread_attr_getstacksize(&pattr, &stack_size),
                         "getting stack size");
      int inheritsched = 0;
      system_error_check(::pthread_attr_getinheritsched(&pattr, &inheritsched),
                         "getting inherited sheduler attribute");
      switch (inheritsched) {
      case PTHREAD_INHERIT_SCHED:
        attr = sched_inherit;
        break;
      case PTHREAD_EXPLICIT_SCHED:
        attr = sched_explicit;
        break;
      default:
        system_error_check(EINVAL, "get scheduler attribute");
        break;
      }

      rtems_id scheduler_id;
      runtime_error_check(::rtems_task_get_scheduler(RTEMS_SELF, &scheduler_id));
#if HAVE_GET_SCHEDULER_NAME
      char name[5];
      if (!get_scheduler_name(scheduler_id, &name[0])) {
        system_error_check(ENOENT, "get scheduler name");
      }
      scheduler = name;
#endif
    }

    attributes&
    attributes::operator=(const attributes& other)
    {
      name = other.name;
      priority = other.priority;
      stack_size = other.stack_size;
      attr = other.attr;
      policy = other.policy;
      return *this;
    }

    bool
    attributes::operator==(const attributes& other) const
    {
      return
        name == other.name &&
        priority == other.priority &&
        stack_size == other.stack_size &&
        attr == other.attr &&
        policy == other.policy;
    }

    void*
    thread_generic_entry(void* arg)
    {
      thread::state_ptr s{ static_cast<thread::state_base*>(arg) };
      try {
        s->run();
      } catch (...) {
        std::terminate();
      }
      return nullptr;
    }

    thread&
    thread::operator=(thread&& thread_)
    {
      if (joinable()) {
        std::terminate();
      }
      swap(thread_);
      return *this;
    }

    void
    thread::swap(thread& thread_) noexcept
    {
      std::swap(id_, thread_.id_);
    }

    bool
    thread::joinable() const noexcept
    {
      return !(id_ == id());
    }

    void
    thread::join()
    {
      if (!joinable()) {
        system_error_check(EINVAL, "join");
      }
      system_error_check(::pthread_join(id_.id_, nullptr), "join");
      id_ = id();
    }

    void
    thread::detach()
    {
      if (!joinable()) {
        system_error_check(EINVAL, "detach");
      }
      system_error_check(::pthread_detach(id_.id_), "detach");
      id_ = id();
    }

    thread::state_base::~state_base() = default;

    void
    thread::start_thread(thread::state_ptr s)
    {
      const attributes attr = s->get_attributes();

      pthread_attr_t pattr;

      system_error_check(::pthread_attr_init(&pattr),
                         "attribute init");

      struct sched_param param;
      param.sched_priority = attr.get_priority();
      system_error_check(::pthread_attr_setschedparam(&pattr, &param),
                         "set sched param");

      int spolicy;
      switch (attr.get_scheduler_policy()) {
      case attributes::sched_other:
        spolicy = SCHED_OTHER;
        break;
      case attributes::sched_roundrobin:
        spolicy = SCHED_RR;
        break;
      case attributes::sched_sporadic:
        spolicy = SCHED_SPORADIC;
        break;
      default:
        spolicy = SCHED_FIFO;
        break;
      }
      system_error_check(::pthread_attr_setschedpolicy(&pattr, spolicy),
                         "set scheduler policy");

      if (attr.get_scheduler_attr() == attributes::sched_inherit) {
        ::pthread_attr_setinheritsched(&pattr, PTHREAD_INHERIT_SCHED);
      }
      else {
        ::pthread_attr_setinheritsched(&pattr, PTHREAD_EXPLICIT_SCHED);
      }

      system_error_check(::pthread_attr_setstacksize(&pattr,
                                                     attr.get_stack_size()),
                         "set stack size");

      /*
       * Hold the new thread in the state's run handler until the rest
       * of the thread is set up after the create call.
       */
      system_error_check(::pthread_create(&id_.id_,
                                          &pattr,
                                          thread_generic_entry,
                                          s.get()),
                         "create thread");

      system_error_check(::pthread_setname_np(id_.id_,
                                              attr.get_name().c_str()),
                         "setting thread name");

      ::pthread_attr_destroy(&pattr);

      s.release();
    };
  };
};
