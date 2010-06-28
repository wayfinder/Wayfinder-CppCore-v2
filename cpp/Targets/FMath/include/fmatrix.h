#ifndef _FMATRIX_H_
#define _FMATRIX_H_

#include "fcommon.h"
#include <algorithm>

namespace fmath {

class fmatrix16
{
public:
   fmatrix16(
      void )
   { }

   fmatrix16(
      const fmath::fixed* matrix )
   {
      m_matrix[0][0] = matrix[0];
      m_matrix[1][0] = matrix[1];
      m_matrix[2][0] = matrix[2];
      m_matrix[3][0] = matrix[3];

      m_matrix[0][1] = matrix[4];
      m_matrix[1][1] = matrix[5];
      m_matrix[2][1] = matrix[6];
      m_matrix[3][1] = matrix[7];

      m_matrix[0][2] = matrix[8];
      m_matrix[1][2] = matrix[9];
      m_matrix[2][2] = matrix[10];
      m_matrix[3][2] = matrix[11];

      m_matrix[0][3] = matrix[12];
      m_matrix[1][3] = matrix[13];
      m_matrix[2][3] = matrix[14];
      m_matrix[3][3] = matrix[15];
   }

   fmath::fixed* operator[](
      int index )
   {
      return &m_matrix[index][0];
   }

   operator fmath::fixed*(
      void )
   {
      return &m_matrix[0][0];
   }

   operator const fmath::fixed*(
      void ) const
   {
      return &m_matrix[0][0];
   }

   fmath::fmatrix16 operator+(
      const fmath::fmatrix16& matrix )
   {
      fmath::fmatrix16 mat(*this);

      return (mat += matrix);
   }

   fmath::fmatrix16 operator-(
      const fmath::fmatrix16& matrix )
   {
      fmath::fmatrix16 mat(*this);

      return (mat -= matrix);
   }

   fmath::fmatrix16 operator*(
      const fmath::fmatrix16& matrix )
   {
      fmath::fmatrix16 mat(*this);

      return (mat *= matrix);
   }

   fmath::fmatrix16& operator+=(
      const fmath::fmatrix16& matrix )
   {
      for (int r = 0; r < 4; ++r)
      {
         for (int c = 0; c < 4; ++c)
         {
            m_matrix[r][c] += matrix.m_matrix[r][c];
         }
      }

      return *this;
   }

   fmath::fmatrix16& operator-=(
      const fmath::fmatrix16& matrix )
   {
      for (int r = 0; r < 4; ++r)
      {
         for (int c = 0; c < 4; ++c)
         {
            m_matrix[r][c] -= matrix.m_matrix[r][c];
         }
      }

      return *this;
   }

   fmath::fmatrix16& operator*=(
      const fmath::fmatrix16& matrix )
   {
      for (int r = 0; r < 4; ++r)
      {
         for (int c = 0; c < 4; ++c)
         {
            m_matrix[r][c] =
               fmath::multiply(m_matrix[r][0], matrix.m_matrix[0][c]) +
               fmath::multiply(m_matrix[r][1], matrix.m_matrix[1][c]) + 
               fmath::multiply(m_matrix[r][2], matrix.m_matrix[2][c]) +
               fmath::multiply(m_matrix[r][3], matrix.m_matrix[3][c]);
         }
      }

      return *this;
   }

private:
   fmath::fixed m_matrix[4][4];
};

inline void fmatrix16identity(
   fmath::fmatrix16& matrix )
{
   matrix[0][0] = fmath::static_int2fix<1>::value;
   matrix[1][0] = fmath::static_int2fix<0>::value;
   matrix[2][0] = fmath::static_int2fix<0>::value;
   matrix[3][0] = fmath::static_int2fix<0>::value;

   matrix[0][1] = fmath::static_int2fix<0>::value;
   matrix[1][1] = fmath::static_int2fix<1>::value;
   matrix[2][1] = fmath::static_int2fix<0>::value;
   matrix[3][1] = fmath::static_int2fix<0>::value;

   matrix[0][2] = fmath::static_int2fix<0>::value;
   matrix[1][2] = fmath::static_int2fix<0>::value;
   matrix[2][2] = fmath::static_int2fix<1>::value;
   matrix[3][2] = fmath::static_int2fix<0>::value;

   matrix[0][3] = fmath::static_int2fix<0>::value;
   matrix[1][3] = fmath::static_int2fix<0>::value;
   matrix[2][3] = fmath::static_int2fix<0>::value;
   matrix[3][3] = fmath::static_int2fix<1>::value;
}

inline void fmatrix16transpose(
   fmath::fmatrix16& matrix )
{
   for (int r = 0; r < 4; ++r)
   {
      for (int c = 0; c < 4; ++c)
      {
         std::swap(matrix[r][c], matrix[c][r]);
      }
   }
}

}

#endif // _FMATRIX_H_
