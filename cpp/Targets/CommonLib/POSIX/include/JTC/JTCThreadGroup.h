// **********************************************************************
//
// Copyright (c) 2001
// IONA Technologies, Inc.
// Waltham, MA, USA
//
// All Rights Reserved
//
// **********************************************************************

#ifndef JTC_THREAD_GROUP_H
#define JTC_THREAD_GROUP_H

#include "JTC/JTCMonitor.h"
#include "JTC/JTCHandle.h"

//
// This class represents a grouping of threads.
//
class JTCThreadGroup : public JTCMonitor, public virtual JTCRefCount
{
public:

    //
    // Constructor and destructor.
    //
    JTCThreadGroup(
	const char* name
    );

    JTCThreadGroup(
	const JTCThreadGroupHandle& parent,
	const char* name
    );

    virtual
    ~JTCThreadGroup();

    //
    // Get the name of this ThreadGroup.
    //
    const char*
    getName() const;

    //
    // Return parent of this ThreadGroup.
    //
    JTCThreadGroupHandle
    getParent() const;

    //
    // Is this a daemon Threadgroup?
    //
    bool
    isDaemon() const;

    //
    // Set the daemon status of a ThreadGroup.
    //
    void
    setDaemon(
	bool daemon
    );

    //
    // Called on an uncaught exception.
    //
    virtual void
    uncaughtException(
	JTCThreadHandle t,
	const JTCException& e
    );

    //
    // Called on an uncaught unknown exception.
    //
    virtual void
    uncaughtException(
	JTCThreadHandle t
    );

    //
    // Get maximum priority of this ThreadGroup.
    //
    int
    getMaxPriority() const;

    //
    // Has this ThreadGroup been destroyed?
    //
    bool
    isDestroyed() const;

    //
    // Destroy this ThreadGroup.
    //
    void
    destroy();

    //
    // Set maximum priority of threads in this ThreadGroup, and our
    // child ThreadGroups.
    //
    void
    setMaxPriority(
	int pri
    );

    //
    // Is this ThreadGroup a parent of the argument?
    //
    bool
    parentOf(
	const JTCThreadGroupHandle& g
    );

    //
    // Number of active threads in this group, and child ThreadGroups.
    //
    int
    activeCount() const;

    //
    // Number of active ThreadGroups in this group, and child ThreadGroups.
    //
    int
    activeGroupCount() const;

    //
    // Enumerate the set of threads in this ThreadGroup, and child
    // ThreadGroups, if recurse is true.
    //
    int
    enumerate(
	JTCThreadHandle* list,
	int len,
	bool recurse = true
    ) const;

    //
    // Enumerate the set of ThreadGroups in this ThreadGroup, and child
    // ThreadGroups, if recurse is true.
    //
    int
    enumerate(
	JTCThreadGroupHandle* list,
	int len,
	bool recurse = true
    ) const;

#ifndef HAVE_JTC_NO_IOSTREAM

    //
    // Display the set of threads and child ThreadGroups
    // in this ThreadGroup to cout.
    //
    void
    list();

    //
    // Display the set of threads and child ThreadGroups
    // in this ThreadGroup to stream os, with given indent level.
    //
    void
    list(
	JTC_STD(ostream)& os,
	int indent
    );

#endif // !HAVE_JTC_NO_IOSTREAM

private:

    //
    // Hide copy constructor and assignment operator.
    //
    JTCThreadGroup(
	const JTCThreadGroup&
    );
    void operator=(
	const JTCThreadGroup&
    );

    //
    // This class represents a collection of ThreadGroupHandles.
    //
    class JTCThreadGroupSet
    {
    public:

        //
        // Constructor/destructor.
        //
        JTCThreadGroupSet()
	    : m_handles(0),
	      m_length(0)
	{
	}

        ~JTCThreadGroupSet()
	{
	    delete [] m_handles;
	}

        //
        // Allocate n members for the set.
        //
        void
	allocate(
	    int n
	);

        //
        // How many members in the set.
        //
        int
	length() const;

        //
        // Return a reference to the ith member of the set.
        //
        JTCThreadGroupHandle&
	operator[](
	    int i
	);

    private:

	//
        // Hide copy constructor and assignment operator.
	//
        JTCThreadGroupSet(
	    const JTCThreadGroupSet&
	);
        JTCThreadGroupSet&
	operator=(
	    const JTCThreadGroupSet&
	);
        
        JTCThreadGroupHandle* m_handles; // The set of handles.
        int m_length; // Length of the set.
    };
    
    //
    // Copy current set of groups.
    //
    void
    create_group_snapshot(
	JTCThreadGroupSet& set
    ) const;

    //
    // Add and remove ThreadGroup.  Add and remove threads.
    //
    void
    add(
	const JTCThreadGroupHandle& g
    );

    void
    remove(
	const JTCThreadGroupHandle& g
    );

    void
    add(
	const JTCThreadHandle& t
    );

    void
    remove(
	const JTCThreadHandle& t
    );

    //
    // Copy current set of threads, and ThreadGroups.
    //
    int
    enumerate(
	JTCThreadHandle* list,
	int len,
	int n,
	bool recurse
    ) const;

    int
    enumerate(
	JTCThreadGroupHandle* list,
	int len,
	int n,
	bool recurse
    ) const;

    //
    // Constructor helper.
    //
    void
    init(
	const JTCThreadGroupHandle& parent,
	const char* name
    );

    JTCThreadGroupHandle m_parent; // Parent of this ThreadGroup.
    char* m_name; // Name of ThreadGroup.
    int m_max_priority; // Maximum priority of this ThreadGroup.
    bool m_destroyed; // Has this ThreadGroup been destroyed?

    int m_nthreads; // Number of threads in this ThreadGroup.
    JTCThreadHandle* m_threads; // Pointer to each thread in this ThreadGroup.
    int m_threads_capacity; // Capacity of the above array.

    int m_ngroups; // Number of child ThreadGroups.
    JTCThreadGroupHandle* m_groups; // Pointer to each child ThreadGroup.
    int m_groups_capacity; // Capacity of the above array.

    bool m_isdaemon; // Is this a daemon ThreadGroup?

    //
    // Needed for add/remove
    //
    friend class JTCThread;

    //
    // Only JTCInitialize can create the root thread group
    //
    friend class JTCInitialize;
    JTCThreadGroup();
};

#ifndef HAVE_JTC_NO_IOSTREAM

//
// Insert operator for the iostream library.
//
JTC_STD(ostream)&
operator<<(
    JTC_STD(ostream)& os,
    const JTCThreadGroup& g
);

#endif // !HAVE_JTC_NO_IOSTREAM

#endif
