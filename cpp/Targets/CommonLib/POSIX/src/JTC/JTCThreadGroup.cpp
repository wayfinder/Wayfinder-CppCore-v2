// **********************************************************************
//
// Copyright (c) 2001
// IONA Technologies, Inc.
// Waltham, MA, USA
//
// All Rights Reserved
//
// **********************************************************************

#include "JTC/JTCThreadGroup.h"
#include "JTC/JTCThread.h"
#include "JTC/JTCSyncT.h"
#include "JTC/JTCRunnable.h" // For HandleI w/ gc
#include "JTC/JTCHandleI.h"

#include <string.h>
#include <assert.h>

#ifndef HAVE_NO_EXPLICIT_TEMPLATES
template class JTCHandleT<JTCThreadGroup>;
#else
#  ifdef HAVE_PRAGMA_DEFINE
#    pragma define(JTCHandleT<JTCThreadGroup>)
#  endif
#endif

#ifndef HAVE_JTC_NO_IOSTREAM
#   ifdef HAVE_STD_IOSTREAM
using namespace std;
#   endif
#endif // !HAVE_JTC_NO_IOSTREAM

// ----------------------------------------------------------------------
// JTCThreadGroup::JTCThreadGroupSet public member implementation
// ----------------------------------------------------------------------

//
// Allocate n members.
//
void
JTCThreadGroup::JTCThreadGroupSet::allocate(
    int n
)
{
    m_length = n;
    m_handles = new JTCThreadGroupHandle[n];
}

//
// Return the number of elements in the set.
//
int
JTCThreadGroup::JTCThreadGroupSet::length() const
{
    return m_length;
}

//
// Return a reference to the i'th element.
//
JTCThreadGroupHandle&
JTCThreadGroup::JTCThreadGroupSet::operator[](
    int i
)
{
    return m_handles[i];
}

// ----------------------------------------------------------------------
// JTCThreadGroup private member implementation
// ----------------------------------------------------------------------

//
// Create a snapshot of current child ThreadGroups.
//
void
JTCThreadGroup::create_group_snapshot(
    JTCThreadGroupSet& set
) const
{
    set.allocate(m_ngroups);
    for (int i = 0; i < m_ngroups; ++i)
    {
	set[i] = m_groups[i];
    }
}

//
// Add a ThreadGroup to my child list.
//
void
JTCThreadGroup::add(
    const JTCThreadGroupHandle& g
)
{
    JTCSyncT<JTCMonitor> sync(*this);
    if (m_destroyed)
    {
	throw JTCIllegalThreadStateException();
    }
    //
    // No children yet?
    //
    if (m_groups == 0)
    {
	m_groups = new JTCThreadGroupHandle[4];
	m_groups_capacity = 4;
    }
    //
    // Time to grow array?
    //
    else if (m_ngroups == m_groups_capacity)
    {
        //
        // Allocate memory and then copy...  This is more exception
        // safe.
        //
	int new_groups_length = m_ngroups*2;
	JTCThreadGroupHandle* new_groups =
	    new JTCThreadGroupHandle[new_groups_length];
	for (int i = 0; i < m_ngroups; ++i)
	{
	    new_groups[i] = m_groups[i];
	}
        
	delete[] m_groups;
	m_groups = new_groups;
	m_groups_capacity = new_groups_length;
    }

    //
    // Add the group to the child list.
    //
    m_groups[m_ngroups] = g;
    ++m_ngroups;
}

//
// Remove a ThreadGroup from my child list.
//
void
JTCThreadGroup::remove(
    const JTCThreadGroupHandle& g
)
{
    JTCSyncT<JTCMonitor> sync(*this);
    if (m_destroyed)
    {
	return;
    }
    
    for (int i = 0; i < m_ngroups; ++i)
    {
	if (m_groups[i] == g)
	{
	    --m_ngroups;
	    for (int j = i; j < m_ngroups; ++j)
	    {
		m_groups[j] = m_groups[j+1];
	    }
	    m_groups[m_ngroups] = 0;
	}
    }

    //
    // If we remove the last thread then notify the ThreadGroups
    // monitor.
    //
    if (m_nthreads == 0)
    {
	notifyAll();
    }

    //
    // If we've gotten rid of all threads and ThreadGroups then
    // destroy the ThreadGroup.
    //
    if (m_isdaemon && m_nthreads == 0 && m_ngroups == 0)
    {
	destroy();
    }
}

//
// End internal compilation error code
//

//
// Add a thread to my thread set.
//
void
JTCThreadGroup::add(
    const JTCThreadHandle& t
)
{
    JTCSyncT<JTCMonitor> sync(*this);
    if (m_destroyed)
    {
	throw JTCIllegalThreadStateException();
    }


    //
    // No threads yet?
    //
    if (m_threads == 0)
    {
	m_threads = new JTCThreadHandle[4];
	m_threads_capacity = 4;
    }
    //
    // Time to grow the array?
    //
    else if (m_nthreads == m_threads_capacity)
    {

        //
        // Allocate and then copy.  This is more exception safe.
        //
	int new_thread_length = m_nthreads*2;
	JTCThreadHandle* new_threads = new JTCThreadHandle[new_thread_length];
	for (int i = 0; i < m_nthreads; ++i)
	{
	    new_threads[i] = m_threads[i];
	}
        
	delete[] m_threads;

	m_threads = new_threads;
	m_threads_capacity = new_thread_length;
    }
    //
    // Add thread.
    //
    m_threads[m_nthreads] = t;
    ++m_nthreads;
}

//
// Remove a thread from my thread set.
//
void
JTCThreadGroup::remove(
    const JTCThreadHandle& t
)
{
    JTCSyncT<JTCMonitor> sync(*this);
    if (m_destroyed)
    {
	return;
    }
    for (int i = 0; i < m_nthreads; ++i)
    {
	if (m_threads[i] == t)
	{
	    --m_nthreads;
	    for (int j = i; j < m_nthreads; ++j)
	    {
		m_threads[j]  = m_threads[j+1];
	    }
	    m_threads[m_nthreads] = JTCThreadHandle();
	}
    }
    if (m_nthreads == 0)
    {
    	notifyAll();
    }
    if (m_isdaemon && m_nthreads == 0 && m_ngroups == 0)
    {
	destroy();
    }
}

//
// Copy threads from my thread set into the provided array.
//
int
JTCThreadGroup::enumerate(
    JTCThreadHandle* list,
    int len,
    int n,
    bool recurse
) const
{
    JTCThreadGroupSet groups_snapshot;

    {
	JTCSyncT<JTCMonitor> sync(*this);

	if (m_destroyed)
	{
	    return 0;
	}

        //
        // Copy threads into the provided list.
        //
	int nt = m_nthreads;
	if (nt > len - n)
	{
	    nt = len - n;
	}
	if (nt > 0)
	{
	    for (int i = 0; i < nt; ++i)
	    {
		list[i+n] = m_threads[i];
	    }
	    n += nt;
	}
	create_group_snapshot(groups_snapshot);
    }

    //
    // Recurse into child ThreadGroups?
    //
    if (recurse)
    {
	for (int i = 0; i < groups_snapshot.length(); ++i)
	{
	    n = groups_snapshot[i] -> enumerate(list, len, n, 1);
	}
    }

    return n;
}

//
// Copy the set of ThreadGroups into the provided array.
//
int
JTCThreadGroup::enumerate(
    JTCThreadGroupHandle* list,
    int len,
    int n,
    bool recurse
) const
{
    JTCThreadGroupSet groups_snapshot;

    {
	JTCSyncT<JTCMonitor> sync(*this);

	if (m_destroyed)
	{
	    return 0;
	}
        
	int ng = m_nthreads;
	if (ng > len - n)
	{
	    ng = len - n;
	}
	if (ng > 0)
	{
	    for (int i = 0; i < ng; ++i)
	    {
		list[i+n] = m_groups[i];
	    }
	    n += ng;
	}
	create_group_snapshot(groups_snapshot);
    }

    //
    // Recurse into child ThreadGroups?
    //
    if (recurse)
    {
	for (int i = 0; i < groups_snapshot.length(); ++i)
	{
	    n = groups_snapshot[i] -> enumerate(list, len, n, 1);
	}
    }

    return n;
}

//
// Constructor helper.
//
void
JTCThreadGroup::init(
    const JTCThreadGroupHandle& parent,
    const char* name
)
{
    //
    // Initialize state variables.
    //
    m_ngroups = 0;
    m_groups = 0;
    m_groups_capacity = 0;
    m_nthreads = 0;
    m_threads = 0;
    m_threads_capacity = 0;
    m_destroyed = false;
    m_isdaemon = parent -> isDaemon();

    //
    // Copy name.
    //
    m_name = new char[strlen(name) + 1];
    strcpy(m_name, name);
    
    m_max_priority = parent -> getMaxPriority();

    //
    // Add myself to my parent.
    //
    m_parent = parent;
    parent -> add(this);
}

// ----------------------------------------------------------------------
// JTCThreadGroup private constructor
// ----------------------------------------------------------------------

//
// This is the constructor that creates the main thread group.
//
JTCThreadGroup::JTCThreadGroup()
    : m_parent(0),
      m_destroyed(false),
      m_nthreads(0),
      m_threads(0),
      m_threads_capacity(0),
      m_ngroups(0),
      m_groups(0),
      m_groups_capacity(0),
      m_isdaemon(false)
{
    //
    // Default name of the system ThreadGroup is system, otherwise
    // the name is adopted.
    //
    const char* default_name = "system_root";
    m_name = new char[strlen(default_name) + 1];
    strcpy(m_name, default_name);
    
    m_max_priority = JTCThread::JTC_MAX_PRIORITY;
}

// ----------------------------------------------------------------------
// JTCThreadGroup constructors and destructor
// ----------------------------------------------------------------------

JTCThreadGroup::JTCThreadGroup(
    const char* name
)
{
    init(JTCThread::currentThread() -> getThreadGroup().get(), name);
}

JTCThreadGroup::JTCThreadGroup(
    const JTCThreadGroupHandle& parent,
    const char* name
)
{
    init(parent, name);
}

JTCThreadGroup::~JTCThreadGroup()
{
    try
    {
	//
	// A root thread group is not destroyed until the owning
	// thread disappears.
	//
	if (!m_destroyed)
        {
	    destroy();
        }
    }
    catch(const JTCIllegalThreadStateException&)
    {
#ifndef HAVE_JTC_NO_IOSTREAM
	cerr << "Unexpected JTCIllegalThreadStateException in ~JTCThreadGroup"
	     << endl;
#endif // !HAVE_JTC_NO_IOSTREAM
    }
    delete[] m_name;
}

// ----------------------------------------------------------------------
// JTCThreadGroup public member implementation
// ----------------------------------------------------------------------

//
// Return name of the this ThreadGroup.
//
const char*
JTCThreadGroup::getName() const
{
    //
    // m_name is immutable -- no locking necessary
    //
    return m_name;
}

//
// Return the parent of this ThreadGroup.
//
JTCThreadGroupHandle
JTCThreadGroup::getParent() const
{
    return JTCThreadGroupHandle(m_parent);
}

//
// Is this a daemon ThreadGroup?
//
bool
JTCThreadGroup::isDaemon() const
{
    JTCSyncT<JTCMonitor> sync(*this);
    return m_isdaemon;
}

//
// Set the daemon ThreadGroup flag.
//

void
JTCThreadGroup::setDaemon(
    bool daemon
)
{
    JTCSyncT<JTCMonitor> sync(*this);
    m_isdaemon = daemon;
}

//
// Called on an uncaught exception.
//
void
JTCThreadGroup::uncaughtException(
    JTCThreadHandle t,
    const JTCException& e
)
{
    if (m_parent)
    {
	m_parent -> uncaughtException(t, e);
    }
    else
    {
#if !defined HAVE_JTC_NO_IOSTREAM && !defined NAV2_CLIENT_DEBUG
       cerr << e << endl;
#endif // !HAVE_JTC_NO_IOSTREAM
    }
}

//
// Called on an uncaught exception (... style)
//
void
JTCThreadGroup::uncaughtException(
    JTCThreadHandle t
)
{
    if (m_parent)
    {
	m_parent -> uncaughtException(t);
    }
    else
    {
#if !defined HAVE_JTC_NO_IOSTREAM && !defined NAV2_CLIENT_DEBUG
       cerr << "unknown exception" << endl;
#endif // !HAVE_JTC_NO_IOSTREAM
    }
}

//
// Return the maximum priority of this ThreadGroup.
//
int
JTCThreadGroup::getMaxPriority() const
{
    JTCSyncT<JTCMonitor> sync(*this);
    return m_max_priority;
}

//
// Has this ThreadGroup been destroyed?
//
bool
JTCThreadGroup::isDestroyed() const
{
    JTCSyncT<JTCMonitor> sync(*this);
    return m_destroyed;
}

//
// Destroy this ThreadGroup.
//
void
JTCThreadGroup::destroy()
{
    JTCThreadGroupSet groups_snapshot;

    {
	JTCSyncT<JTCMonitor> sync(*this);
	if (m_destroyed || m_nthreads > 0)
	{
	    throw JTCIllegalThreadStateException();
	}
	create_group_snapshot(groups_snapshot);

	m_destroyed = true;
	m_ngroups = 0;
	delete[] m_groups;
	m_groups = 0;
	m_groups_capacity = 0;
	m_nthreads = 0;
	delete[] m_threads;
	m_threads = 0;
	m_threads_capacity = 0;
    }

    for (int i = 0; i < groups_snapshot.length(); ++i)
    {
	groups_snapshot[i] -> destroy();
    }

    if (m_parent)
    {
	m_parent -> remove(this);
    }
}

//
// Set maximum priority value for this ThreadGroup, and our child
// ThreadGroups.
//
void
JTCThreadGroup::setMaxPriority(
    int pri
)
{
    JTCThreadGroupSet groups_snapshot;

    {
	JTCSyncT<JTCMonitor> sync(*this);
	if (pri < JTCThread::JTC_MIN_PRIORITY)
	{
	    m_max_priority = JTCThread::JTC_MIN_PRIORITY;
	}
	else if (pri < m_max_priority)
	{
	    m_max_priority = pri;
	}
	create_group_snapshot(groups_snapshot);
    }

    for (int i = 0; i < groups_snapshot.length(); ++i)
    {
	groups_snapshot[i] -> setMaxPriority(pri);
    }
}

//
// Is this ThreadGroup a parent of handle?
//
bool
JTCThreadGroup::parentOf(
    const JTCThreadGroupHandle& handle
)
{
    for (
	JTCThreadGroup* g = handle.get();
	g != 0;
	g = g -> m_parent.get()
    )
    {
	if (g == this)
	{
	    return true;
	}
    }
    return false;
}

//
// Determine the number of active threads in this ThreadGroup, and
// child ThreadGroups.
//
int
JTCThreadGroup::activeCount() const
{
    int result = 0;

    JTCThreadGroupSet groups_snapshot;

    {
	JTCSyncT<JTCMonitor> sync(*this);

	if (m_destroyed)
	{
	    return 0;
	}
	result = m_nthreads;

	create_group_snapshot(groups_snapshot);
    }

    for (int i = 0; i < groups_snapshot.length(); ++i)
    {
	result += groups_snapshot[i] -> activeCount();
    }

    return result;
}

//
// Determine the number of ThreadGroups in this group, and child
// ThreadGroups.
//
int
JTCThreadGroup::activeGroupCount() const
{
    JTCThreadGroupSet groups_snapshot;

    {
	JTCSyncT<JTCMonitor> sync(*this);

	if (m_destroyed)
	{
	    return 0;
	}
	create_group_snapshot(groups_snapshot);
    }

    int n = groups_snapshot.length();
    for (int i = 0; i < groups_snapshot.length(); ++i)
    {
	n += groups_snapshot[i] -> activeCount();
    }
    return n;
}

//
// Copy the set of threads in this ThreadGroup, and child ThreadGroups
// if recurse is true into array list.
//
int
JTCThreadGroup::enumerate(
    JTCThreadHandle* list,
    int len,
    bool recurse
) const
{
    return enumerate(list, len, 0, recurse);
}

//
// Copy the set child ThreadGroups in this ThreadGroup, and child ThreadGroups
// if recurse is true into array list.
//
int
JTCThreadGroup::enumerate(
    JTCThreadGroupHandle* list,
    int len,
    bool recurse
) const
{
    return enumerate(list, len, 0, recurse);
}

#ifndef HAVE_JTC_NO_IOSTREAM

//
// Display set of threads and ThreadGroups to cout.
//
void
JTCThreadGroup::list()
{
    list(cout, 0);
}

//
// Display the set of threads and ThreadGroups to os, with given indent
// level.
//
void
JTCThreadGroup::list(
    ostream& os,
    int indent
)
{
    JTCThreadGroupSet groups_snapshot;
    JTCThreadHandle* threads_snapshot;
    int nthreads_snapshot;

    {
	JTCSyncT<JTCMonitor> sync(*this);

	nthreads_snapshot = m_nthreads;

	threads_snapshot = new JTCThreadHandle[m_nthreads];
	for (int i = 0; i < m_nthreads; ++i)
	{
	    threads_snapshot[i] = m_threads[i];
	}

	create_group_snapshot(groups_snapshot);

	for (int j = 0; j < indent; ++j)
	{
	    os << " ";
	}
	os << *this << endl;
	indent += 4;
    }

    for (int i = 0; i < nthreads_snapshot; ++i)
    {
	for (int i_level = 0; i_level < indent; ++i_level)
	{
	    os << " ";
	}
	os << *threads_snapshot[i] << endl;
    }
    delete[] threads_snapshot;

    for (int j = 0; j < groups_snapshot.length(); ++j)
    {
	groups_snapshot[j] -> list(os, indent);
    }
}

//
// IOstream insertion operator for a ThreadGroup.
//
ostream&
operator<<(
    ostream& os,
    const JTCThreadGroup& g
)
{
    os << "ThreadGroup: "
       << "[name=" << g.getName() << ",maxpri=" << g.getMaxPriority() << "]";
    return os; 
}

#endif // !HAVE_JTC_NO_IOSTREAM
