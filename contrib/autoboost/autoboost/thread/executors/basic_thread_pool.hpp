// Copyright (C) 2013-2014 Vicente J. Botet Escriba
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// 2013/09 Vicente J. Botet Escriba
//    Adapt to autoboost from CCIA C++11 implementation
//    first implementation of a simple pool thread using a vector of threads and a sync_queue.

#ifndef AUTOBOOST_THREAD_EXECUTORS_BASIC_THREAD_POOL_HPP
#define AUTOBOOST_THREAD_EXECUTORS_BASIC_THREAD_POOL_HPP

#include <autoboost/thread/detail/config.hpp>
#include <autoboost/thread/detail/delete.hpp>
#include <autoboost/thread/detail/move.hpp>
#include <autoboost/thread/scoped_thread.hpp>
#include <autoboost/thread/sync_queue.hpp>
#include <autoboost/thread/executors/work.hpp>
#include <autoboost/thread/csbl/vector.hpp>

#include <autoboost/config/abi_prefix.hpp>

namespace autoboost
{
namespace executors
{
  class basic_thread_pool
  {
  public:
    /// type-erasure to store the works to do
    typedef  executors::work work;
  private:
    /// the kind of stored threads are scoped threads to ensure that the threads are joined.
    /// A move aware vector type
    typedef scoped_thread<> thread_t;
    typedef csbl::vector<thread_t> thread_vector;

    /// the thread safe work queue
    sync_queue<work > work_queue;
    /// A move aware vector
    thread_vector threads;

  public:
    /**
     * Effects: try to execute one task.
     * Returns: whether a task has been executed.
     * Throws: whatever the current task constructor throws or the task() throws.
     */
    bool try_executing_one()
    {
      work task;
      try
      {
        if (work_queue.try_pull_front(task) == queue_op_status::success)
        {
          task();
          return true;
        }
        return false;
      }
      catch (std::exception& )
      {
        return false;
      }
      catch (...)
      {
        return false;
      }
    }
    /**
     * Effects: schedule one task or yields
     * Throws: whatever the current task constructor throws or the task() throws.
     */
    void schedule_one_or_yield()
    {
        if ( ! try_executing_one())
        {
          this_thread::yield();
        }
    }
  private:

    /**
     * The main loop of the worker threads
     */
    void worker_thread()
    {
      try
      {
        for(;;)
        {
          work task;
          queue_op_status st = work_queue.wait_pull_front(task);
          if (st == queue_op_status::closed) return;
          task();
        }
      }
      catch (std::exception& )
      {
        return;
      }
      catch (...)
      {
        return;
      }
    }
#if defined(AUTOBOOST_NO_CXX11_RVALUE_REFERENCES)
    template <class AtThreadEntry>
    void worker_thread1(AtThreadEntry& at_thread_entry)
    {
      at_thread_entry(*this);
      worker_thread();
    }
#endif
    void worker_thread2(void(*at_thread_entry)(basic_thread_pool&))
    {
      at_thread_entry(*this);
      worker_thread();
    }
    template <class AtThreadEntry>
    void worker_thread3(AUTOBOOST_THREAD_FWD_REF(AtThreadEntry) at_thread_entry)
    {
      at_thread_entry(*this);
      worker_thread();
    }
    static void do_nothing_at_thread_entry(basic_thread_pool&) {}

  public:
    /// basic_thread_pool is not copyable.
    AUTOBOOST_THREAD_NO_COPYABLE(basic_thread_pool)

    /**
     * \b Effects: creates a thread pool that runs closures on \c thread_count threads.
     *
     * \b Throws: Whatever exception is thrown while initializing the needed resources.
     */
    basic_thread_pool(unsigned const thread_count = thread::hardware_concurrency())
    {
      try
      {
        threads.reserve(thread_count);
        for (unsigned i = 0; i < thread_count; ++i)
        {
#if 1
          thread th (&basic_thread_pool::worker_thread, this);
          threads.push_back(thread_t(autoboost::move(th)));
#else
          threads.push_back(thread_t(&basic_thread_pool::worker_thread, this)); // do not compile
#endif
        }
      }
      catch (...)
      {
        close();
        throw;
      }
    }
    /**
     * \b Effects: creates a thread pool that runs closures on \c thread_count threads
     * and executes the at_thread_entry function at the entry of each created thread. .
     *
     * \b Throws: Whatever exception is thrown while initializing the needed resources.
     */
#if defined(AUTOBOOST_NO_CXX11_RVALUE_REFERENCES)
    template <class AtThreadEntry>
    basic_thread_pool( unsigned const thread_count, AtThreadEntry& at_thread_entry)
    {
      try
      {
        threads.reserve(thread_count);
        for (unsigned i = 0; i < thread_count; ++i)
        {
          thread th (&basic_thread_pool::worker_thread1<AtThreadEntry>, this, at_thread_entry);
          threads.push_back(thread_t(autoboost::move(th)));
          //threads.push_back(thread_t(&basic_thread_pool::worker_thread, this)); // do not compile
        }
      }
      catch (...)
      {
        close();
        throw;
      }
    }
#endif
    basic_thread_pool( unsigned const thread_count, void(*at_thread_entry)(basic_thread_pool&))
    {
      try
      {
        threads.reserve(thread_count);
        for (unsigned i = 0; i < thread_count; ++i)
        {
          thread th (&basic_thread_pool::worker_thread2, this, at_thread_entry);
          threads.push_back(thread_t(autoboost::move(th)));
          //threads.push_back(thread_t(&basic_thread_pool::worker_thread, this)); // do not compile
        }
      }
      catch (...)
      {
        close();
        throw;
      }
    }
    template <class AtThreadEntry>
    basic_thread_pool( unsigned const thread_count, AUTOBOOST_THREAD_FWD_REF(AtThreadEntry) at_thread_entry)
    {
      try
      {
        threads.reserve(thread_count);
        for (unsigned i = 0; i < thread_count; ++i)
        {
          thread th (&basic_thread_pool::worker_thread3<AtThreadEntry>, this, autoboost::forward<AtThreadEntry>(at_thread_entry));
          threads.push_back(thread_t(autoboost::move(th)));
          //threads.push_back(thread_t(&basic_thread_pool::worker_thread, this)); // do not compile
        }
      }
      catch (...)
      {
        close();
        throw;
      }
    }
    /**
     * \b Effects: Destroys the thread pool.
     *
     * \b Synchronization: The completion of all the closures happen before the completion of the \c basic_thread_pool destructor.
     */
    ~basic_thread_pool()
    {
      // signal to all the worker threads that there will be no more submissions.
      close();
      // joins all the threads as the threads were scoped_threads
    }

    /**
     * \b Effects: join all the threads.
     */
    void join()
    {
      for (unsigned i = 0; i < threads.size(); ++i)
      {
        threads[i].join();
      }
    }

    /**
     * \b Effects: close the \c basic_thread_pool for submissions.
     * The worker threads will work until there is no more closures to run.
     */
    void close()
    {
      work_queue.close();
    }

    /**
     * \b Returns: whether the pool is closed for submissions.
     */
    bool closed()
    {
      return work_queue.closed();
    }

    /**
     * \b Requires: \c Closure is a model of \c Callable(void()) and a model of \c CopyConstructible/MoveConstructible.
     *
     * \b Effects: The specified \c closure will be scheduled for execution at some point in the future.
     * If invoked closure throws an exception the \c basic_thread_pool will call \c std::terminate, as is the case with threads.
     *
     * \b Synchronization: completion of \c closure on a particular thread happens before destruction of thread's thread local variables.
     *
     * \b Throws: \c sync_queue_is_closed if the thread pool is closed.
     * Whatever exception that can be throw while storing the closure.
     */

#if defined(AUTOBOOST_NO_CXX11_RVALUE_REFERENCES)
    template <typename Closure>
    void submit(Closure & closure)
    {
      work_queue.push_back(work(closure));
    }
#endif
    void submit(void (*closure)())
    {
      work_queue.push_back(work(closure));
    }

    template <typename Closure>
    void submit(AUTOBOOST_THREAD_RV_REF(Closure) closure)
    {
      work_queue.push_back(work(autoboost::forward<Closure>(closure)));
    }

    /**
     * \b Requires: This must be called from an scheduled task.
     *
     * \b Effects: reschedule functions until pred()
     */
    template <typename Pred>
    bool reschedule_until(Pred const& pred)
    {
      do {
        if ( ! try_executing_one())
        {
          return false;
        }
      } while (! pred());
      return true;
    }

  };
}
using executors::basic_thread_pool;

}

#include <autoboost/config/abi_suffix.hpp>

#endif
