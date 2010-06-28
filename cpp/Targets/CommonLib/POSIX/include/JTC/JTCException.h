// **********************************************************************
//
// Copyright (c) 2001
// IONA Technologies, Inc.
// Waltham, MA, USA
//
// All Rights Reserved
//
// **********************************************************************

#ifndef JTC_EXCEPTION_H
#define JTC_EXCEPTION_H

#include "JTC/JTCTypes.h"

//
// Exception hierarchy for JTC.
//
// JTCThreadDeath
//
// JTCException
//   |
//   +-- JTCInterruptedException
//   |
//   +-- JTCIllegalThreadStateException
//   |
//   +-- JTCIllegalArgumentException
//   |
//   +-- JTCSystemCallException
//   |
//   +-- JTCOutOfMemoryError
//   |
//   +-- JTCIllegalMonitorStateException
//   |
//   +-- JTCUnknownThreadException
//   |
//   +-- JTCInitializeError
//

#include <string.h>

class JTCThreadDeath
{
};

class JTCException
{
protected:

    char* note_;
    int error_;

public:

    JTCException(const char* note = "", long error = 0)
	: error_(error)
    {
	note_ = new char[strlen(note) + 1];
	strcpy(note_, note);
    }

    JTCException(const JTCException& e)
	: error_(e.error_)
    {
	note_ = new char[strlen(e.note_) +1];
	strcpy(note_, e.note_);
    }

    JTCException& operator=(const JTCException& e)
    {
	if (this != &e)
	{
	    error_ = e.error_;
	    note_ = new char[strlen(e.note_) +1];
	    strcpy(note_, e.note_);
	}
	return *this;
    }

    virtual ~JTCException()
    {
	delete[] note_;
    }

    long getError() const
    {
	return error_;
    }

    virtual const char* getType() const
    {
	return "JTCException";
    }

    const char* getMessage() const
    {
	return note_;
    }
};

#ifndef HAVE_JTC_NO_IOSTREAM

inline JTC_STD(ostream)&
operator<<(JTC_STD(ostream)& os, const JTCException& e)
{
    os << e.getType() << ": " << e.getMessage();
    return os;
}

#endif // !HAVE_JTC_NO_IOSTREAM


class JTCInterruptedException : public JTCException
{
public:

    JTCInterruptedException() {}
    JTCInterruptedException(const char* note)
	: JTCException(note) { }

    virtual const char* getType() const
    {
	return "JTCInterruptedException";
    }
};

class JTCIllegalThreadStateException : public JTCException
{
public:

    JTCIllegalThreadStateException() {}
    JTCIllegalThreadStateException(const char* note)
	: JTCException(note) { }

    virtual const char* getType() const
    {
	return "JTCIllegalThreadStateException";
    }
};

class JTCIllegalMonitorStateException : public JTCException
{
public:

    JTCIllegalMonitorStateException() { }
    JTCIllegalMonitorStateException(const char* note)
	: JTCException(note) { }

    virtual const char* getType() const
    {
	return "JTCIllegalMonitorStateException";
    }
};

class JTCIllegalArgumentException : public JTCException
{
public:

    JTCIllegalArgumentException() { }
    JTCIllegalArgumentException(const char* note)
	: JTCException(note) { }

    virtual const char* getType() const
    {
	return "JTCIllegalArgumentException";
    }
};

class JTCSystemCallException : public JTCException
{
public:

    JTCSystemCallException(const char* note, long error)
	: JTCException(note, error) { }

    virtual const char* getType() const
    {
	return "JTCSystemCallException";
    }
};

class JTCOutOfMemoryError : public JTCException
{
public:

    JTCOutOfMemoryError() { }
    JTCOutOfMemoryError(const char* note)
	: JTCException(note) { }

    virtual const char* getType() const
    {
	return "JTCOutOfMemoryError";
    }
};

class JTCUnknownThreadException : public JTCException
{
public:

    JTCUnknownThreadException() { }
    JTCUnknownThreadException(const char* note)
	: JTCException(note) { }

    virtual const char* getType() const
    {
	return "JTCUnknownThreadException";
    }
};

class JTCInitializeError : public JTCException
{
public:

    JTCInitializeError(const char* note)
	: JTCException(note) { }

    virtual const char* getType() const
    {
	return "JTCInitializeError";
    }
};

#endif
