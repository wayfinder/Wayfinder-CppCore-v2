/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "config.h"
#include "MC2Logging.h"
#include "IntervalInterpolator.h"
#include <algorithm>

IntervalInterpolator::IntervalInterpolator()
{
   setIntervals(0.0f, 0.0f); 
}

IntervalInterpolator::IntervalInterpolator(float normAccLength,
                                           float normDecLength)
{
   setIntervals(normAccLength, normDecLength);
}

void IntervalInterpolator::setIntervals(float normAccLength, float normDecLength)
{
   normAccLength = std::max(0.0f, normAccLength);
   normDecLength = std::max(0.0f, normDecLength);

   if(normAccLength + normDecLength > 1.0f) {
      normAccLength = 0.0f;
      normDecLength = 0.0f;
   }
   
   m_accStart = 0.0f;
   m_linearStart = normAccLength;
   m_decStart = 1.0f - normDecLength;
   m_accLength = normAccLength;
   m_linearLength = 1.0f - normAccLength - normDecLength;
   m_decLength = normDecLength;
   
   m_height =
      1.0f / (m_accLength * 0.5f + m_linearLength + m_decLength * 0.5f);

   m_t1 = m_accLength * m_height * 0.5f;
   m_t2 = m_decLength * m_height * 0.5f;
   m_r1 = m_linearLength * m_height;

   if(m_accLength == 0.0f) {
      m_accTan = 0.0f;
   } else {
      m_accTan = (m_height / m_accLength);
   }

   if(m_decLength == 0.0f) {
      m_decTan = 0.0f; 
   } else {
      m_decTan = (m_height / m_decLength);      
   }
}

float IntervalInterpolator::getValue(float normTime) const
{
   if(normTime < 0.0f) {
      mc2dbg << "[IntIntp]: "
             << "Error, invalid value to interpolator: " << normTime << endl;
      return 0.0f;
   } else if(normTime > 1.0f ) {
      mc2dbg << "[IntIntp]: "
             << "Error, invalid value to interpolator: " << normTime << endl;
      return m_t1 + m_t2 + m_r1;
   }
   
   if(normTime < m_linearStart) {      
      // Within the acceleration phase
      float a = normTime * m_decTan;

      return a * normTime * 0.5f;
   } else if(normTime <= m_decStart) {
      // Within the linear phase
      float a = (normTime - m_accLength) * m_height;
      
      return m_t1 + a;
   } else { 
      // Within the deceleration phase
      float a = m_t2 - (1.0f - normTime) * m_decTan * (1.0f - normTime) * 0.5f;

      return m_t1 + m_r1 + a;
   }
}

float IntervalInterpolator::getMaxVelocity() const
{
   return m_height; 
}
