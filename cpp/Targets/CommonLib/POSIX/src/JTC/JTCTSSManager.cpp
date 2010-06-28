// **********************************************************************
//
// Copyright (c) 2001
// IONA Technologies, Inc.
// Waltham, MA, USA
//
// All Rights Reserved
//
// **********************************************************************

#include "JTC/JTCTSS.h"

#include "JTC/JTCTSSManager.h"

#include <assert.h>

//
// Static members
//
JTCTSSManager* JTCTSSManager::m_instance = 0;

// ----------------------------------------------------------------------
// JTCTSSManager constructor/destructor
// ----------------------------------------------------------------------

JTCTSSManager::JTCTSSManager()
    : m_head(0)
{
    assert(m_instance == 0);
    m_instance = this;
}

JTCTSSManager::~JTCTSSManager()
{
    assert(m_instance != 0);
    m_instance = 0;
}

// ----------------------------------------------------------------------
// JTCTSSManager public member implementation
// ----------------------------------------------------------------------

JTCTSSManager*
JTCTSSManager::instance()
{
    return m_instance;
}

//
// Associate a key and a cleanup function
//
void
JTCTSSManager::allocate(
    JTCThreadKey key,
    void(*release)(void*)
)
{
    m_mutex.lock();

    KeyCleanup* node = new KeyCleanup;
    node -> key = key;
    node -> release = release;
    node -> next = m_head;
    m_head = node;

    m_mutex.unlock();
}

//
// Remove the key from our internal table
//
void
JTCTSSManager::release(
    JTCThreadKey key
)
{
    m_mutex.lock();

    KeyCleanup** node = &m_head;
    while (*node != 0)
    {
        if ((*node) -> key == key)
            break;
        node = &(*node) -> next;
    }
    if (*node != 0)
    {
        KeyCleanup* tmp = *node;
        *node = (*node) -> next;
        delete tmp;
    }

    m_mutex.unlock();
}

//
// Run the cleanup functions
//
void
JTCTSSManager::cleanup()
{
    m_mutex.lock();

    KeyCleanup* node = m_head;
    while (node != 0)
    {
        void* data = JTCTSS::get(node -> key);
        (*node -> release)(data);
        JTCTSS::set(node -> key, 0);
        node = node -> next;
    }

    m_mutex.unlock();
}
