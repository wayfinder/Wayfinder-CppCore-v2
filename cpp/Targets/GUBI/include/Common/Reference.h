#ifndef _REFERENCE_H_
#define _REFERENCE_H_

#include "IReference.h"

namespace gubi {

/**
 * Handles the reference count for an object.
 */
class Reference
{
public:
   /**
    * Default constructor.
    */
   Reference(
      void );

   /**
    * Destructor.
    */
   virtual ~Reference(
      void );

protected:
   /**
    * Adds a new reference to the object.
    *
    * @return Number of reference to the object.
    */
   unsigned int InternalAddReference(
      void ) const;

   /**
    * Releases a reference to the object.
    *
    * @return Number of reference to the object.
    */
   unsigned int InternalRelease(
      void ) const;

private:
   mutable unsigned int m_iReference;
};

}

#endif
