/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef FIXED_H
#define FIXED_H

#include "config.h"
#include <iomanip>
#include <math.h>

class fixedp {

private:
   const static int BP= 16;  // how many low bits are right of Binary Point
   const static int BP2= BP*2;  // how many low bits are right of Binary Point
   const static int BPhalf= BP/2;  // how many low bits are right of Binary Point

   static double STEP() { return 1.0 / (1<<BP); }  // smallest step we can represent

	// for private construction via guts
public:
   int	fx; // the guts
   
	enum FixedRaw { RAW };
	fixedp(FixedRaw, int _fx) : fx(_fx) {}
   
	fixedp() : fx(0) {}
	fixedp(const fixedp& a) : fx( a.fx ) {}
	fixedp(float a) : fx( int(a / (float)STEP()) ) {}
	fixedp(double a) : fx( int(a / (double)STEP()) ) {}
	fixedp(int a) : fx( a << BP ) {}
   fixedp(unsigned int a) : fx( a << BP ) {}
	fixedp(long a) : fx( a << BP ) {}
	fixedp& operator =(const fixedp& a) { fx= a.fx; return *this; }
	fixedp& operator =(float a) { fx= fixedp(a).fx; return *this; }
	fixedp& operator =(double a) { fx= fixedp(a).fx; return *this; }
	fixedp& operator =(int a) { fx= fixedp(a).fx; return *this; }
	fixedp& operator =(long a) { fx= fixedp(a).fx; return *this; }

	operator float() const { return fx * (float)STEP(); }
	operator double() const { return fx * (double)STEP(); }
	operator int() const { return fx>>BP; }
	operator long() const { return fx>>BP; }

	fixedp operator +() const { return fixedp(RAW,fx); }
	fixedp operator -() const { return fixedp(RAW,-fx); }

	fixedp operator +(const fixedp& a) const { return fixedp(RAW, fx + a.fx); }
	fixedp operator -(const fixedp& a) const { return fixedp(RAW, fx - a.fx); }
#if 1
	// more acurate, using long long
	fixedp operator *(const fixedp& a) const {
      return fixedp(RAW,  (int)( ((long long)fx * (long long)a.fx ) >> BP));
   }
   
#else
	// faster, but with only half as many bits right of binary point
	fixedp operator *(const fixedp& a) const {
      return fixedp(RAW, (fx>>BPhalf) * (a.fx>>BPhalf) );
   }
#endif
	fixedp operator /(const fixedp& a) const {
      return fixedp(RAW, int( (((long long)fx << BP2) /
                              (long long)(a.fx)) >> BP) );
   }

	fixedp operator +(float a) const { return fixedp(RAW, fx + fixedp(a).fx); }
	fixedp operator -(float a) const { return fixedp(RAW, fx - fixedp(a).fx); }
	fixedp operator *(float a) const { return fixedp(RAW, (fx>>BPhalf) * (fixedp(a).fx>>BPhalf) ); }
	fixedp operator /(float a) const { return fixedp(RAW, int( (((long long)fx << BP2) / (long long)(fixedp(a).fx)) >> BP) ); }

	fixedp operator +(double a) const { return fixedp(RAW, fx + fixedp(a).fx); }
	fixedp operator -(double a) const { return fixedp(RAW, fx - fixedp(a).fx); }
	fixedp operator *(double a) const { return fixedp(RAW, (fx>>BPhalf) * (fixedp(a).fx>>BPhalf) ); }
	fixedp operator /(double a) const { return fixedp(RAW, int( (((long long)fx << BP2) / (long long)(fixedp(a).fx)) >> BP) ); }

	fixedp& operator +=(fixedp a) { return *this = *this + a; return *this; }
	fixedp& operator -=(fixedp a) { return *this = *this - a; return *this; }
	fixedp& operator *=(fixedp a) { return *this = *this * a; return *this; }
	fixedp& operator /=(fixedp a) { return *this = *this / a; return *this; }

	fixedp& operator +=(int a) { return *this = *this + (fixedp)a; return *this; }
	fixedp& operator -=(int a) { return *this = *this - (fixedp)a; return *this; }
	fixedp& operator *=(int a) { return *this = *this * (fixedp)a; return *this; }
	fixedp& operator /=(int a) { return *this = *this / (fixedp)a; return *this; }

	fixedp& operator +=(long a) { return *this = *this + (fixedp)a; return *this; }
	fixedp& operator -=(long a) { return *this = *this - (fixedp)a; return *this; }
	fixedp& operator *=(long a) { return *this = *this * (fixedp)a; return *this; }
	fixedp& operator /=(long a) { return *this = *this / (fixedp)a; return *this; }

	fixedp& operator +=(float a) { return *this = *this + a; return *this; }
	fixedp& operator -=(float a) { return *this = *this - a; return *this; }
	fixedp& operator *=(float a) { return *this = *this * a; return *this; }
	fixedp& operator /=(float a) { return *this = *this / a; return *this; }

	fixedp& operator +=(double a) { return *this = *this + a; return *this; }
	fixedp& operator -=(double a) { return *this = *this - a; return *this; }
	fixedp& operator *=(double a) { return *this = *this * a; return *this; }
	fixedp& operator /=(double a) { return *this = *this / a; return *this; }

	bool operator ==(const fixedp& a) const { return fx == a.fx; }
	bool operator !=(const fixedp& a) const { return fx != a.fx; }
	bool operator <=(const fixedp& a) const { return fx <= a.fx; }
	bool operator >=(const fixedp& a) const { return fx >= a.fx; }
	bool operator  <(const fixedp& a) const { return fx  < a.fx; }
	bool operator  >(const fixedp& a) const { return fx  > a.fx; }

	bool operator ==(float a) const { return fx == fixedp(a).fx; }
	bool operator !=(float a) const { return fx != fixedp(a).fx; }
	bool operator <=(float a) const { return fx <= fixedp(a).fx; }
	bool operator >=(float a) const { return fx >= fixedp(a).fx; }
	bool operator  <(float a) const { return fx  < fixedp(a).fx; }
	bool operator  >(float a) const { return fx  > fixedp(a).fx; }

	bool operator ==(double a) const { return fx == fixedp(a).fx; }
	bool operator !=(double a) const { return fx != fixedp(a).fx; }
	bool operator <=(double a) const { return fx <= fixedp(a).fx; }
	bool operator >=(double a) const { return fx >= fixedp(a).fx; }
	bool operator  <(double a) const { return fx  < fixedp(a).fx; }
	bool operator  >(double a) const { return fx  > fixedp(a).fx; }
};

inline fixedp operator +(float a, const fixedp& b) { return fixedp(a)+b; }
inline fixedp operator -(float a, const fixedp& b) { return fixedp(a)-b; }
inline fixedp operator *(float a, const fixedp& b) { return fixedp(a)*b; }
inline fixedp operator /(float a, const fixedp& b) { return fixedp(a)/b; }

inline bool operator ==(float a, const fixedp& b) { return fixedp(a) == b; }
inline bool operator !=(float a, const fixedp& b) { return fixedp(a) != b; }
inline bool operator <=(float a, const fixedp& b) { return fixedp(a) <= b; }
inline bool operator >=(float a, const fixedp& b) { return fixedp(a) >= b; }
inline bool operator  <(float a, const fixedp& b) { return fixedp(a)  < b; }
inline bool operator  >(float a, const fixedp& b) { return fixedp(a)  > b; }



inline fixedp operator +(double a, const fixedp& b) { return fixedp(a)+b; }
inline fixedp operator -(double a, const fixedp& b) { return fixedp(a)-b; }
inline fixedp operator *(double a, const fixedp& b) { return fixedp(a)*b; }
inline fixedp operator /(double a, const fixedp& b) { return fixedp(a)/b; }

inline bool operator ==(double a, const fixedp& b) { return fixedp(a) == b; }
inline bool operator !=(double a, const fixedp& b) { return fixedp(a) != b; }
inline bool operator <=(double a, const fixedp& b) { return fixedp(a) <= b; }
inline bool operator >=(double a, const fixedp& b) { return fixedp(a) >= b; }
inline bool operator  <(double a, const fixedp& b) { return fixedp(a)  < b; }
inline bool operator  >(double a, const fixedp& b) { return fixedp(a)  > b; }


inline int& operator +=(int& a, const fixedp& b) { a = (fixedp)a + b; return a; }
inline int& operator -=(int& a, const fixedp& b) { a = (fixedp)a - b; return a; }
inline int& operator *=(int& a, const fixedp& b) { a = (fixedp)a * b; return a; }
inline int& operator /=(int& a, const fixedp& b) { a = (fixedp)a / b; return a; }

inline long& operator +=(long& a, const fixedp& b) { a = (fixedp)a + b; return a; }
inline long& operator -=(long& a, const fixedp& b) { a = (fixedp)a - b; return a; }
inline long& operator *=(long& a, const fixedp& b) { a = (fixedp)a * b; return a; }
inline long& operator /=(long& a, const fixedp& b) { a = (fixedp)a / b; return a; }

inline float& operator +=(float& a, const fixedp& b) { a = a + b; return a; }
inline float& operator -=(float& a, const fixedp& b) { a = a - b; return a; }
inline float& operator *=(float& a, const fixedp& b) { a = a * b; return a; }
inline float& operator /=(float& a, const fixedp& b) { a = a / b; return a; }

inline double& operator +=(double& a, const fixedp& b) { a = a + b; return a; }
inline double& operator -=(double& a, const fixedp& b) { a = a - b; return a; }
inline double& operator *=(double& a, const fixedp& b) { a = a * b; return a; }
inline double& operator /=(double& a, const fixedp& b) { a = a / b; return a; }

/**
 *
 *    fixedp helper functions reside in this namespace.
 *    
 *    These functions operate on standard integer numbers which are
 *    interpreted to be of 16.16 format. They are used internally by
 *    the fixedp class.
 *    
 */
namespace FixedPointHelpers {
   const static double FXFACTOR = ( 1.0 / 65536.0 );
   const static double FXINV = 65536.0;
   const static unsigned int FIXED_SHIFT = 16;
}

/* Useful constants for fixed point numbers */
namespace fx {
   const fixedp FX_ONE =     1;
   const fixedp FX_ZERO =    0;
   const fixedp DEG_360 = 360;
   const fixedp DEG_180 = 180;
   const fixedp DEG_90 =  90;
}

/* Negates the value */

fixedp operator-(const fixedp a);

ostream& operator << (ostream& oStream, const fixedp& val);

#endif
