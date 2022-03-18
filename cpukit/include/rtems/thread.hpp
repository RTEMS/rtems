/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSC++
 *
 * @brief C++ standard thread support with thread attribute control.
 *
 * Provide a way to create a thread in C++ with attributes that let
 * you control the real-time embedded parameters need to run
 * threads on RTEMS.
 *
 * The code requires the `-std=c++17` option to access `std::invoke()`.
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

#if !defined(RTEMS_THREAD_HPP)
#define RTEMS_THREAD_HPP

#include <functional>
#include <iostream>
#include <string>
#include <thread>
#include <utility>

namespace rtems
{
  namespace thread
  {
    /**
     * @brief Manage the attributes of a thread.
     */
    class attributes
    {
    public:
      /**
       * The scheduler attribute.
       */
      enum sched_attr {
        sched_inherit,    /**< Inherit the scheduler attributes
                           *   from the creating thread. */
        sched_explicit    /**< Explicitly set the scheduler to these
                           *   attributes. */
      };

      /**
       * The scheduler policies.
       */
      enum sched_policy {
        sched_other,      /**< Other scheduler policy */
        sched_fifo,       /**< FIFO scheduler policy */
        sched_roundrobin, /**< Round robin scheduler policy */
        sched_sporadic    /**< Sporadic scheduler policy */
      };

      /**
       * Construct a thread attributes object with the current settings of the
       * executing thread. The stack size is set to the configured minimum
       * stack size.
       */
      attributes();

      /*
       * Copy construct the thread attributes.
       *
       * @param attr The attributes to copy.
       */
      attributes(const attributes& attr);

      /**
       * Set the name of the thread. The thread is a classic API thread and
       * the name is only 4 characters.
       *
       * @param name The name as a string.
       */
      void set_name(const std::string& name);

      /**
       * Set the name of the thread. The thread is a classic API thread and
       * the name is only 4 characters.
       *
       * @param name The name as a string.
       */
      void set_name(const char* name);

      /**
       * Get the name of the thread.
       *
       * @retval const std::string& The name of the thread.
       */
      const std::string& get_name() const;

      /**
       * Set the priority of the thread.
       *
       * @param priority The POSIX API priority of the thread.
       */
      void set_priority(int priority);

      /**
       * Get the POSIX API priority of the thread.
       *
       * @retval int The POSIX API thread priority.
       */
      int get_priority() const;

      /**
       * Set the stack size. If the size is less than the configured minimum
       * the minimum value is used.
       *
       * @param size The stack size in bytes.
       */
      void set_stack_size(size_t size);

      /**
       * Get the stack size.
       *
       * @retval size_t The stack size in bytes.
       */
      size_t get_stack_size() const;

      /**
       * Set the scheduler name. If not set no scheduler is set.
       *
       * @parrm scheduler The name of the scheduler.
       */
      void set_scheduler(const std::string& scheduler);

      /**
       * Set the scheduler name. If not set no scheduler is set.
       */
      void set_scheduler(const char* scheduler);

      /**
       * Get scheduler name.
       */
      const std::string& get_scheduler();

      /**
       * Get the attributes' scheduler attribute for the thread.
       *
       * @return sched_attr The attributes' scheduler attribute
       */
      sched_attr get_scheduler_attr() const;

      /**
       * Set the scheduler policy for the thread. This call sets the
       * scheduler attribute to @ref sched_explicit.
       *
       * @param policy The scheduler policy.
       */
      void set_scheduler_policy(sched_policy policy);

      /**
       * Get the scheduler policy for the thread.
       */
      sched_policy get_scheduler_policy() const;

      /**
       * Commit any changes to the executing thread.
       *
       * @note only the priority and attribute of a thread can be changed. The
       * name and stack size are ignored.
       */
      void commit();

      /**
       * Update the attribute values from the executing thread. The attributes
       * are updated from the current thread when constructed and the values
       * returned are those held since then. If another thread changes the
       * attributes of the current thread those changes will not be seen until
       * this method is called. Except for the name and stack size any local
       * changes made will lost then the update call is made.
       */
      void update();

      /**
       * Copy operator.
       */
      attributes& operator=(const attributes& attr);

      /**
       * The comparison operator does not check the name or stack size
       * of a thread.
       */
      bool operator==(const attributes& attr) const;

    private:
      std::string  name;        /**< Name of the thread */
      int          priority;    /**< POSIX API priority */
      size_t       stack_size;  /**< Stack size in bytes */
      std::string  scheduler;   /**< Name of the scheduler */
      sched_attr   attr;        /**< Scheduler's attribute */
      sched_policy policy;      /**< Scheduler's policy */
      /* affinity, cpu set size is? */
    };

    template <class T>
    inline typename std::decay<T>::type
    decay_copy(T&& t) {
      return std::forward<T>(t);
    }

    /**
     * @brief Create a thread with thread attributes.
     *
     * Create a thread optionally with thread attributes. The usage of this
     * class follows the C++ standard for std::thread. The standard support
     * for creating a thread does not let you control the attributes of a
     * thread and control is important in embedded real-time
     * applications. This class lets you control a thread attributes and use
     * the extensive an excellent thread support the C++ standard provides.
     *
     * There is no indication attribute support for threads will be added to
     * the C++ standard and what it will look like. The support provided here
     * is designed to make as little impact on a code base as possible. While
     * the attributes supported are specific to RTEMS they are common to all
     * embedded operating systems.
     *
     * The support provided here is specific to GCC due to the use of some
     * non-standard interfaces to get the indices of the template argument
     * pack in new thread's context. A standards only solution would be
     * preferred.
     */
    class thread
    {
      friend void* thread_generic_entry(void* arg);

      /**
       * Base state class to interface to derived template of the thread
       * state from the generic entry point for the thread.
       */
      struct state_base
      {
        virtual ~state_base();
        virtual const attributes get_attributes() = 0;
        virtual void run() = 0;
      };

      /**
       * The state is passed to the new thread context as a unique
       * pointer. This handles the hand over and clean up.
       */
      using state_ptr = std::unique_ptr<state_base>;

    public:

      /**
       * Template check to see if the first argument of a thread is a set of
       * attributes.
       */
      template <typename A, class DecayA = typename std::decay<A>::type>
      using enable_if_attributes = typename std::enable_if
        <std::is_same<DecayA, attributes>::value>::type;

      /**
       * We need our own id type so the thread class can access the pthread
       * handle to initialise it.
       */
      class id {
      public:
        id() noexcept : id_(0) { }
        explicit id(pthread_t id_) : id_(id_) { }
      private:
        pthread_t id_;

        friend class thread;
        friend bool operator==(thread::id l, thread::id r) noexcept;

        template<class CharT, class Traits>
        friend std::basic_ostream<CharT, Traits>&
        operator<<(std::basic_ostream<CharT, Traits>& out, thread::id id_);
      };

      /**
       * The default thread constructions.
       */
      thread() noexcept = default;

      /**
       * The std::thread equivalent constructor. The attributes will be the
       * same as the executing thread with a default thread name and the
       * configured minimum stack size.
       */
      template<typename F, typename... Args>
      explicit thread(F&& func, Args&&... args);

      /**
       * Create a thread with the provided attributes. The entry point and
       * optional arguments are the same as std::thread.
       */
      template <typename A, typename F, typename ...Args,
                class = enable_if_attributes<A>>
      explicit thread(A&& attr, F&& func, Args&&... args)
        : id_(0) {
        start_thread(
          make_state(attr,
                     make_invoker(decay_copy(std::forward<F>(func)),
                                  decay_copy(std::forward<Args>(args))...))
        );
      }

      /**
       * Move the thread id to this instance.
       */
      thread& operator=(thread&& thread_);

      void swap(thread& thread_) noexcept;

      bool joinable() const noexcept;

      void join();

      void detach();

      /*
       * Constrain use. These are not available.
       */
      thread(thread&) = delete;
      thread(const thread&) = delete;
      thread(const thread&&) = delete;
      thread& operator=(const thread&) = delete;

      std::thread::id get_id() const noexcept;

    private:

      id id_;

      /**
       * Invoke the thread's entry point with the parameter pack in the new
       * thread's context. This object holds the parameters copied onto the
       * new thread's stack making them available to entry point routine.
       */
      template<typename Parms>
      struct invoker {
        Parms p;

        template<size_t Index>
        static std::__tuple_element_t<Index, Parms>&& declval();

        template<size_t... Ind>
        auto invoke(std::_Index_tuple<Ind...>)
          noexcept(noexcept(std::invoke(declval<Ind>()...)))
          -> decltype(std::invoke(declval<Ind>()...)) {
          return std::invoke(std::get<Ind>(std::move(p))...);
        }

        using indices =
          typename std::_Build_index_tuple<std::tuple_size<Parms>::value>::__type;

        void run() {
          invoke(indices());
        }
      };

      /**
       * The state holds the invoker with the parameters. The generic entry
       * point calls the virtual methods to get the attributes and to run the
       * new thread in the new thread's context.
       */
      template<typename Invoker>
      struct state : state_base {
        const attributes attr;
        Invoker          i;

        state(const attributes& attr, Invoker&& i)
          : attr(attr),
            i(std::forward<Invoker>(i)) {
        }

        const attributes get_attributes() override {
          return attr;
        }

        void run() override {
          i.run();
        }
      };

      /**
       * Make the state. This dynamic memory is managed by the unique pointer
       * and is passed to the generic thread entry point.
       */
      template<typename Invoker>
      static state_ptr
      make_state(const attributes& attr, Invoker&& i) {
        using state_impl = state<Invoker>;
        return state_ptr{ new state_impl(attr, std::forward<Invoker>(i)) };
      }

      /**
       * Decay the parameters so they can be correctly packed into the
       * parameter tuple.
       */
      template<typename... T>
      using decayed_tuple = std::tuple<typename std::decay<T>::type...>;

      /**
       * Make the invoker with the parameters.
       */
      template<typename F, typename... Args>
      static invoker<decayed_tuple<F, Args...>>
      make_invoker(F&& func, Args&&... args)
      {
        return {
          decayed_tuple<F, Args...> {
            std::forward<F>(func), std::forward<Args>(args)...
          }
        };
      }

      /**
       * Create and start the thread.
       */
      void start_thread(state_ptr s);
    };

    template<typename F, typename... Args>
    thread::thread(F&& func, Args&&... args)
      : id_(0)  {
      attributes attr;
      start_thread(
        make_state(attr,
                   make_invoker(decay_copy(std::forward<F>(func)),
                                decay_copy(std::forward<Args>(args))...))
      );
    }

    inline std::thread::id thread::get_id() const noexcept {
      return std::thread::id(id_.id_);
    }

    inline bool
    operator==(thread::id l, thread::id r) noexcept {
      return l.id_ == r.id_;
    }

    inline bool
    operator!=(thread::id l, thread::id r) noexcept {
      return !(l == r);
    }

    template<class C, class T>
    inline std::basic_ostream<C, T>&
    operator<<(std::basic_ostream<C, T>& out, thread::id id_) {
      return out << std::thread::id(id_.id_);
    }
  };
};

#endif
