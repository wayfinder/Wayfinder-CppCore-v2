/*
 *    $Id: NotCopyable.h,v 1.1 2006-06-29 12:30:48 hkin Exp $
 *
 *    Copyright, Wayfinder Systems AB, 2006
 */

#ifndef NOTCOPYABLE_H
#define NOTCOPYABLE_H

/// not copyable
class NotCopyable {
protected:
   NotCopyable() { }
private:
   NotCopyable( const NotCopyable& );
   NotCopyable& operator = (const NotCopyable& );
};

#endif
