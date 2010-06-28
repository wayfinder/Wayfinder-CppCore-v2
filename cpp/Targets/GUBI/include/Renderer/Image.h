
#ifndef IMAGE_H
#define IMAGE_H

#include "Renderer/IImage.h"
   
namespace gubi {

template <class T>
class Image : public IImage {
  public:
  Image(T *t, unsigned int width, unsigned int height)
      :m_type(t)
      ,m_width(width)
      ,m_height(height)
  {
    
  }

  virtual ~Image()
  {
    
  }

  unsigned int getWidth()
  {
    return m_width;
  }

  unsigned int getHeight()
  {
    return m_height;
  }

  T* getData()
  {
     return m_type;
  }

  /**
   * Queries the objects property bag.
   *
   * @return Returns true if the operation was successful.
   */
  virtual bool GetProperties(
      gubi::IPropertyBag **properties )
  {
     return false;
  }

  /**
   * Queries the objects property bag.
   *
   * @return Returns true if the operation was successful.
   */
  virtual bool GetProperties(
      const gubi::IPropertyBag** ppProperties ) const
  {
     return false;
  }

  /**
   * Adds a new reference to the object.
   *
   * @return Number of reference to the object.
   */
  virtual unsigned int AddReference(
      void ) const
  {
     // do nothing
  }

  /**
   * Releases a reference to the object.
   *
   * @return Number of reference to the object.
   */
  virtual unsigned int Release(
      void ) const
  {
     // do nothing
  }

protected:
  T *m_type;
  unsigned int m_width;
  unsigned int m_height;
};

}

#endif // IMAGE_H
