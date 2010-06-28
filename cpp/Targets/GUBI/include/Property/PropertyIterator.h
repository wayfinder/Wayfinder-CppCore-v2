#ifndef _PROPERTY_ITERATOR_H_
#define _PROPERTY_ITERATOR_H_

namespace gubi {
    
// Prototypes
class IProperty;
class IPropertyBag;

/**
 * Wrapper for the IPropertyBag for easier iterations.
 */
class PropertyIterator
{
public:
   /**
    * Default constructor.
    */
   PropertyIterator(
      void );

   /**
    * Initialize constructor from a IPropertyBag instance.
    */
   PropertyIterator(
      gubi::IPropertyBag* pPropertyBag );

   /**
    * Destructor.
    */
   ~PropertyIterator(
      void );

   /**
    * 
    */
   PropertyIterator& operator=(
      gubi::IPropertyBag* pPropertyBag );

   /**
    * 
    */
   gubi::IProperty* operator*(
      void );

   /**
    * Go to the next property in line.
    */
   PropertyIterator& operator++(
      void );

   /**
    * Check if the iterator is valid.
    */
   bool IsValid(
      void );

private:
   gubi::IPropertyBag* m_pPropertyBag;
   gubi::IProperty*    m_pProperty;
   bool                m_bEndOfBag;
};

}

#endif // _PROPERTY_ITERATOR_H_
