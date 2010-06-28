
#ifndef _IREFERENCE_H_
#define _IREFERENCE_H_

// GUBI namespace
namespace gubi {

/**
 * Basic reference counted interface.
 */
class IReference
{
public:
   /**
    * Adds a new reference to the object.
    *
    * @return Number of reference to the object.
    */
   virtual unsigned int AddReference(
      void ) const = 0;

   /**
    * Releases a reference to the object.
    *
    * @return Number of reference to the object.
    */
   virtual unsigned int Release(
      void ) const = 0;
};

}

#endif // _IREFERENCE_H_
