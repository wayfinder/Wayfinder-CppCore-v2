// **********************************************************************
//
// Copyright (c) 2001
// IONA Technologies, Inc.
// Waltham, MA, USA
//
// All Rights Reserved
//
// **********************************************************************

#ifndef JTC_THREAD_H
#define JTC_THREAD_H

#include "JTC/JTCTypes.h"
#include "JTC/JTCHandle.h"
#include "JTC/JTCCond.h"

//
// Forward declarations
//
class JTCThreadGroup;
class JTCThread;

#if defined(HAVE_POSIX_THREADS) || defined(HAVE_DCE_THREADS)
typedef void (*JTCAttrHook)(pthread_attr_t*);
#endif

//
// The run hook definition
//
typedef void (*JTCRunHook)(JTCThread*);

//
// The start hook definition
//
typedef void (*JTCStartHook)();

//
// This class encapsulates the functionality of a thread.
//
class JTCThread : public virtual JTCRefCount
{
public:

    JTCThread(
	const char* name
    );
    JTCThread(
	JTCRunnableHandle target = JTCRunnableHandle(),
	const char* name = 0
    );
    JTCThread(
	JTCThreadGroupHandle& group,
	JTCRunnableHandle target,
	const char* name = 0
    );
    JTCThread(
	JTCThreadGroupHandle& group,
	const char* name = 0
    );

    virtual
    ~JTCThread();

    //
    // Get the threads group.
    //
    JTCThreadGroupHandle
    getThreadGroup();

    //
    // Set the name of this thread.
    //
    void
    setName(
	const char*
    );

    //
    // Get the name of this thread.
    //
    const char*
    getName(
    ) const;

    //
    // Start the thread.
    //
    void
    start();

    //
    // Override this method to provide functionality to your thread.
    //
    virtual void
    run();

    //
    // Determine if the thread is currently running.
    //
    bool
    isAlive() const;

    //
    // Determine if the thread has died.
    //
    bool
    hasDied() const;

    //
    // Cause the removal of a thread that was never started
    //
    void
    destroy();

    //
    // Wait for this thread to terminate.
    //
    void
    join();

    //
    // Wait at most millis for this thread to terminate.
    //
    void
    join(
	long millis
    );

    //
    // Wait at most millis plus nanos for this thread to terminate.
    //
    void
    join(
	long millis,
	int nanos
    );
    
    //
    // Set the priority of this thread.
    //
    void
    setPriority(
	int newPri
    );

    //
    // Get the priority of this thread.
    //
    int
    getPriority(
    ) const;

    //
    // Enumerate all threads in this threads group.
    //
    static int
    enumerate(
	JTCThreadHandle* list,
	int len
    );

    //
    // The priorities must have JTC_ prepended since under WIN32
    // MIN_PRIORITY is already defined in winspool.h
    //
    enum Priority
    {
	JTC_MIN_PRIORITY  = 0,
	JTC_NORM_PRIORITY = 1,
	JTC_MAX_PRIORITY  = 2
    };

    //
    // Get a pointer to the current thread object.
    //
    static JTCThread*
    currentThread();

    //
    // Sleep for millis milliseconds, and nano nanoseconds.
    //
    static void
    sleep(
	long millis,
	int nano = 0
    );

    //
    // Yield this threads current timeslice.
    //
    static void
    yield();

    //
    // Determine the number of threads currently running in this
    // threads ThreadGroup.
    //
    static int
    activeCount();

    //
    // Get the id of this thread.
    //
    JTCThreadId
    getId(
    ) const;

    //
    // Hook functions
    //

#if defined(HAVE_POSIX_THREADS) || defined(HAVE_DCE_THREADS)
    //
    // This function sets/gets a "hook" that is used to initialize
    // custom POSIX thread attributes. This function is called before
    // every thread that is created
    //
    static void
    setAttrHook(
	JTCAttrHook attr_hook,
	JTCAttrHook* old_hook = 0
    );
#endif

    //
    // This function gets/sets a run "hook" -- that is a function that
    // is called to setup any application specific information. Some
    // third party packages, such as ObjectStore, force each thread to
    // establish fault handlers. The hook function must call thread ->
    // run() to actually run the thread.
    //
    static void
    setRunHook(
	JTCRunHook run_hook,
	JTCRunHook* old_hook = 0
    );

    //
    // This function gets/sets a start "hook" -- that is a function
    // that is called to setup any thread specific information.
    //
    static void
    setStartHook(
	JTCStartHook start_hook,
	JTCStartHook* old_hook = 0
    );

private:

    //
    // Hide copy constructor and assignment operator.
    //
    JTCThread(
	const JTCThread&
    );
    void
    operator=(
	const JTCThread&
    );

    //
    // Helper for the constructor.
    //
    void
    init(const JTCThreadGroupHandle& group, JTCRunnableHandle target,
	      const char* name);

    //
    // Private constructor for booting JTC.
    //
    JTCThread(
	JTCThreadId tid,
	bool main
    );

    static unsigned long
    get_next_thread_number();

public: // No friend decl for extern "C functions

    //
    // entrance_hook() should only be called by the thread_adapter.
    //
    //friend extern "C" void* sfm_thread_adapter(void*);
    void
    entrance_hook();

    //
    // The thread exit hook
    //
    void
    exit_hook();

private:

    //
    // Threads ID. Set in start(), and in main/adopted thread
    // constructor.  Protected by m_mutex.
    //
    JTCThreadId m_thread_id;
#ifndef WIN32
    bool m_detached;
#endif

    //
    // The thread number (immutable)
    //
    unsigned long m_thread_number;

#ifdef WIN32
    //
    // The thread handle. Protected by m_mutex.
    //
    HANDLE m_handle;
#endif

    //
    // Group of this thread (immutable)
    //
    JTCThreadGroupHandle m_group;

    //
    // Name of the thread. Protected by m_mutex.
    //
    char* m_name;

    //
    // Target of the thread (immutable)
    //
    JTCRunnableHandle m_target;

    //
    // The thread state.
    //
    enum State
    {
	new_thread,
	runnable,
	not_runnable,
	dead
    };
    State m_state;

    //
    // Mutex, associated with resumeCond, protects internal state
    // data.
    //
    JTCMutex m_mutex;

    //
    // Join condition variable.
    //
    JTCCond m_join_cond;

    //
    // Is this thread adopted?
    //
    bool m_is_adopted;
    
    //
    // Static members
    //

    //
    // TSS key.
    //
    static JTCThreadKey sm_tss_key;

#if defined(HAVE_POSIX_THREADS) || defined(HAVE_DCE_THREADS)
    static JTCAttrHook sm_attr_hook;
#endif

    static JTCRunHook sm_run_hook;
    static JTCStartHook sm_start_hook;

    //
    // The next thread number and associated mutex
    //
    static unsigned long sm_thread_number;
    static JTCMutex sm_thread_number_mutex;

    friend class JTCInitialize;
    friend class JTCAdoptCurrentThread;
};

#ifndef HAVE_JTC_NO_IOSTREAM

//
// Insert operator for the iostream library.
//
JTC_STD(ostream)& operator<<(JTC_STD(ostream)& os, const JTCThread& thr);

#endif // !HAVE_JTC_NO_IOSTREAM

//
// Forward declaration
//
class JTCTSSManager;

//
// This class is used to initialize the JTC library.
//
class JTCInitialize
{
    //
    // Number of times the library has been initialized.
    //
    static int           sm_init_count;
    
    JTCThreadHandle      m_main_thread; // The JTC main thread.
    JTCTSSManager*       m_tss_manager;
    JTCThreadGroupHandle m_root_thread_group; // The root thread group

    void initialize();
    
public:

    JTCInitialize();
    JTCInitialize(
	int& argc,
	char** argv
    );
    ~JTCInitialize();

    //
    // Wait for all running threads to terminate.
    //
    void
    waitTermination();

    //
    // Determine if JTC has been initialized.
    //
    static bool
    initialized();
};

//
// This class is used to incorporate a non-JTC thread into the
// JThreads/C++ family. If an application wants to use a non-JTC
// thread with JTC primitives (for instance, if an ORBacus method is
// called from a non-JTC thread) an instance of this class must be
// instantiated before using any JTC primitive.
//
// What this class actually does is setup a fake
// JTCThread::currentThread() object, and create another root
// ThreadGroup.
//
class JTCAdoptCurrentThread
{
    JTCThreadHandle m_adopted_thread; // The JTC adopted thread

public:

    JTCAdoptCurrentThread();
    ~JTCAdoptCurrentThread();
};

#endif
