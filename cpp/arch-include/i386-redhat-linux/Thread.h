/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef THREAD_H
#define THREAD_H

#include "LogFiles.h"
#include "FileSystem.h"
#include "JTC/JTC.h"
#include "Runnable.h"
#include <sys/syscall.h>

namespace isab{

void waitTerminate();
/** Implements threading. Classes that should run as threads should
 * extend this class. Threads are started with a call to the start
 * method which in turn calls the run method, and stops when the 
 * run method exits.
 */
class Thread{
  /** Inner class handling the JTC specific details. */
  class InnerThread : public JTCThread{
    /** The Thread object associated with this instance. */
    Thread* m_thread;
  public:
    /** Gets the associated Thread object. 
     * @return a pointer to the Thread object associated with this
     *         object. 
     */
    Thread* thread();
    /** Constructor that sets the associated Thread object. 
     * @param thread a pointer to the Thread object to associate with
     *               this instance of InnerThread.
     */
    InnerThread(Thread* thread);
    /** Constructor that sets the associated Thread object and gives
     * the Thread a name.
     * @param thread a pointer to the Thread object to associate with
     *               this instance of InnerThread.
     * @param name   a name for this thread.
     */
    InnerThread(Thread* thread, const char* name) ;
    /** When this InnerThread object is started, this method is the
        first to be executed. It calls the run method of either the
        Thread object associated with this object or, its Runnable
        object. */
    virtual void run();
  };

  /** A handle to the InnerThread object. Shall be used instead of
      pointers. Uses reference counting to decide when to delete the
      InnerThread object.*/
  typedef JTCHandleT<InnerThread> InnerThreadHandle;
  /** A handle to this Threads InnerThread instance. */
  InnerThreadHandle m_innerThread;
  /** A pointer to the object whos run method shall be executed
      instead of this objects. */
  Runnable* m_runnable;
  /** Does basic setup for all constructors.  
   * @param name the Thread's name. If NULL, a default name will be
   *             constructed.
   */ 
  void init(const char* name);
public:
  /** Finds the currently executing Thread. 
   * @return a pointer to the currently execuing Thread. 
   */
  static Thread* currentThread();
  /** Return values from start. */
  enum startStatus {
    UNKNOWN_FAILURE = -1, /** The start failed for unknown reasons. */ 
    INIT_FAILED, /** The Thread object didn't initialize correctly. */ 
    ALREADY_STARTED, /** This Thread has already been started. */
    GOOD_START /** Started OK.*/
  };
  /** Starts this Thread. If this method is overridden, the 
   * overriding method must ALWAYS call this method since it does 
   * all the underlying magic thread work.
   * @return GOOD_START if the Thread was started OK, ALREADY_STARTED
   *         if start had already been called on this Thread, 
   *         INIT_FAILED if the Thread object wasn't correctly
   *         initialized, and UNKNOWN_FAILURE if the start failed for
   *         unknown reasons.
   */
  int start();

  /** Stops this Thread. The canonical implemetation would be to 
   * break out of the run method's infinite while loop. The default
   * implementation will call the Runnable class's terminate 
   * method. 
   */
  virtual void terminate();
  /** This is the Threads main method. If it's not overriden by
   * descendants, nothing whatsoever will be done.
   */
  virtual void run()=0;
  /** Tests if this Thread is currently running.
   * @return true if this thread has been started and not yet 
   *         stopped.
   */
  bool isAlive();
  /** Return values for the join method. */
  enum joinStatus{
    JOIN_FAIL = 0,  /** The call failed for unknown reasons.*/
    TIME_OUT = 1,   /** The call timed out. */
    PROPER_JOIN = 2 /** The target thread has expired. */
  };
  /** Waits for this thread to stop. If this thread is alive, this
   * method blocks until either the Thread exits or the call 
   * timeouts. The timeout is the sum of the two arguments. 
   * If both parameters are less than zero the call will have no
   * timeout value. If only one is less than zero it will be
   * interpreted as zero. Due to the default values of the parameters
   * join can be called with no parameters, and act like a blocking
   * call without timeout.
   * @param millis the millisecond part of the timeout value. Default
   *               value is -1.
   * @param nanos the nanosecond part of the timeout value. Default
   *              value is -1.
   * @return JOIN_FAIL if the underlying system call failed, TIME_OUT
   *         if the join call timed out, and PROPER_JOIN if the target
   *         Thread has expired and a proper join took place. Please
   *         note that JOIN_FAIL equals 0 while TIME_OUT and
   *         PROPER_JOIN are not 0.
   */
  int join(long millis = -1, int nanos = -1);
  /** Creates a new Thread with a name.
   * @param name the Thread's name.
   */
  Thread(const char* name = NULL);
  /** Creates a new Thread which will execute the run method of a
   * Runnable object. The Thread will have a name. 
   * @param runnable the Runnable object whos run method will be
   *                 executed when this Thread objects start method is
   *                 called. 
   * @param name     the new Thread's name.
   */
  Thread(Runnable* runnable, const char* name = NULL);
  /** Virtual destructor.*/
  virtual ~Thread();
  /** Gets the threads name.
   * @return the name of the thread as a c-string.
   */
  const char* getName() const;

   int getThreadInfo(char* data, int dataLength);

  /** Return values for sleep.*/
  enum sleepStatus{
    AWOKE = 0,         /** Done sleeping. */
    FAIL,              /** System call failed. */
    ILLEGAL_ARGUMENTS, /** Called with faulty arguments. */
    INTERRUPTED        /** The callinf thread was interrupted. */
  };
  /** Sleep*/
  static int sleep(long millis, int nanos = 0);
/** Yield*/
  static void yield(){
    JTCThread::yield();
  }
};

// =====================================================================
// ==         Inlines of Thread::InnerThread                          ==

inline Thread* Thread::InnerThread::thread(){
  return m_thread;
}
inline Thread::InnerThread::InnerThread(Thread* thread) 
  : JTCThread(), m_thread(thread)
{
}
inline Thread::InnerThread::InnerThread(Thread* thread, const char* name) 
  : JTCThread(name), m_thread(thread)
{
}

// =====================================================================
// ==          Inlines of Thread                                      ==
inline bool Thread::isAlive(){
  return m_innerThread->isAlive();
}
inline const char* Thread::getName() const{
  return m_innerThread->getName();
}

inline Thread::Thread(const char* name) {
  init(name);
}

inline Thread::Thread(class Runnable* runnable, const char* name){
  init(name);
  m_runnable = runnable;
}
inline void Thread::terminate(){
  if(m_runnable != NULL){
    m_runnable->terminate();
  }
}
}
#endif



      
  
