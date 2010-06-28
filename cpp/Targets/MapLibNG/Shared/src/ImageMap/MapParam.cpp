/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "ImageMap/MapParam.h"


MapParam::MapParam(int zoomIndex,
                   BoundingBox worldBox,
                   const MC2SimpleString& string,
                   int x,
                   int y)
   : m_x(x),
     m_y(y),
     m_zoomIndex(zoomIndex),
     m_worldBox(worldBox),
     m_string(string)
{
   
}

int MapParam::getZoomIndex() const
{
   return m_zoomIndex;
}

BoundingBox MapParam::getWorldBox() const
{
   return m_worldBox;
}

const MC2SimpleString& MapParam::getString() const
{
   return m_string;
}

bool MapParam::operator<(const char* str) const
{
   return strcmp(m_string.c_str(), str) < 0; 
}

bool MapParam::operator==(const char* str) const
{
   return strcmp(m_string.c_str(), str) == 0; 
}

bool MapParam::operator<(const MapParam& rhs) const
{
#if 0
   return *this < rhs.m_string.c_str();
#else
   if(m_x < rhs.m_x) {
      return true;
   } else if(m_x > rhs.m_x) {
      return false;
   }

   if(m_y < rhs.m_y) {
      return true;
   } else if(m_y < rhs.m_y) {
      return false;
   }

   return m_zoomIndex < rhs.m_zoomIndex;
#endif
}

bool MapParam::operator==(const MapParam& rhs) const
{
   return m_x == rhs.m_x && m_y == rhs.m_y && m_zoomIndex == rhs.m_zoomIndex;
}

MapParam::MapParam() :
   m_x(0),
   m_y(0),
   m_zoomIndex(0),
   m_worldBox(),
   m_string()
{
    
}

bool MapParam::isValid() const
{
   return m_string != "";
}
