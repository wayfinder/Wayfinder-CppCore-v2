#ifndef _FVEC_H_
#define _FVEC_H_

#include "fmath.h"

namespace fmath {

class fvec3
{
public:
   fvec3(
      void )
   { }

   fvec3(
      fixed x,
      fixed y,
      fixed z )
   {
      m_vec[0] = x;
      m_vec[1] = y;
      m_vec[2] = z;
   }

   fvec3(
      const fixed* vec )
   {
      m_vec[0] = vec[0];
      m_vec[1] = vec[1];
      m_vec[2] = vec[2];
   }

   fvec3(
      const fmath::fvec3& vec )
   {
      m_vec[0] = vec.m_vec[0];
      m_vec[1] = vec.m_vec[1];
      m_vec[2] = vec.m_vec[2];
   }

   operator const fixed*(
      void ) const
   {
      return &m_vec[0];
   }

   fmath::fixed& operator[](
      int index )
   {
      return m_vec[index];
   }

   const fmath::fixed& operator[](
      int index ) const
   {
      return m_vec[index];
   }

   bool operator==(
      const fmath::fvec3& vec ) const
   {
      return (m_vec[0] == vec.m_vec[0]) &&
             (m_vec[1] == vec.m_vec[1]) &&
             (m_vec[2] == vec.m_vec[2]);
   }

   bool operator!=(
      const fmath::fvec3& vec ) const
   {
      return !(m_vec[0] == vec.m_vec[0]) &&
              (m_vec[1] == vec.m_vec[1]) &&
              (m_vec[2] == vec.m_vec[2]);
   }

   fmath::fvec3& operator=(
      const fmath::fvec3& vec )
   {
      m_vec[0] = vec.m_vec[0];
      m_vec[1] = vec.m_vec[1];
      m_vec[2] = vec.m_vec[2];

      return *this;
   }

   fmath::fvec3 operator+(
      const fmath::fvec3& vec ) const
   {
      return fmath::fvec3(m_vec[0] + vec.m_vec[0],
                          m_vec[1] + vec.m_vec[1],
                          m_vec[2] + vec.m_vec[2]);
   }

   fmath::fvec3 operator-(
      const fmath::fvec3& vec ) const
   {
      return fmath::fvec3(m_vec[0] - vec.m_vec[0],
                          m_vec[1] - vec.m_vec[1],
                          m_vec[2] - vec.m_vec[2]);
   }

   fmath::fvec3 operator*(
      const fmath::fixed& scalar ) const
   {
      return fmath::fvec3(fmath::multiply(m_vec[0], scalar),
                          fmath::multiply(m_vec[1], scalar),
                          fmath::multiply(m_vec[2], scalar));
   }

   fmath::fvec3& operator+=(
      const fmath::fvec3& vec )
   {
      m_vec[0] += vec.m_vec[0];
      m_vec[1] += vec.m_vec[1];
      m_vec[2] += vec.m_vec[2];

      return *this;
   }

   fmath::fvec3& operator-=(
      const fmath::fvec3& vec )
   {
      m_vec[0] -= vec.m_vec[0];
      m_vec[1] -= vec.m_vec[1];
      m_vec[2] -= vec.m_vec[2];

      return *this;
   }

   fmath::fvec3& operator*=(
      const fmath::fixed& scalar )
   {
      m_vec[0] = fmath::multiply(m_vec[0], scalar);
      m_vec[1] = fmath::multiply(m_vec[1], scalar);
      m_vec[2] = fmath::multiply(m_vec[2], scalar);

      return *this;
   }

protected:
   fmath::fixed m_vec[3];
};

inline fmath::fixed fvec3dot(
   const fmath::fvec3& a,
   const fmath::fvec3& b )
{
   return (fmath::multiply(a[0], b[0]) + 
           fmath::multiply(a[1], b[1]) +
           fmath::multiply(a[2], b[2]));
}

inline fmath::fvec3 fvec3cross(
   const fmath::fvec3& a,
   const fmath::fvec3& b )
{
   return fmath::fvec3(fmath::multiply(a[1], b[2]) -
                       fmath::multiply(b[1], a[2]),
                       fmath::multiply(a[2], b[0]) -
                       fmath::multiply(b[2], a[0]),
                       fmath::multiply(a[0], b[1]) -
                       fmath::multiply(b[0], a[1]));
}

}

#endif
