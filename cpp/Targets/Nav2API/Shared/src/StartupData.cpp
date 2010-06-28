/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#define CORE_LOGGING_MODULE_NAME "StartupData"
// #define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE

#include "CoreDebugPrint.h"
#include "StartupData.h"

namespace WFAPI {

StartupData::StartupData(const WFString& commonDataStoragePath,
                         const WFString& resourceStoragePath,
                         TextLanguage textLanguage,
                         VoiceLanguage::VoiceLanguage voiceLanguage,
                         const WFString& imageStoragePath,
                         const WFString& categoryImageStoragePath,
                         const WFString& categoryTreeImageStoragePath,
                         const WFString& parameterStoragePath )
   : m_commonDataStoragePath(commonDataStoragePath),
     m_dataStoragePath(""),
     m_resourceStoragePath(resourceStoragePath),
     m_imageStoragePath(imageStoragePath),
     m_categoryImageStoragePath(categoryImageStoragePath),
     m_categoryTreeImageStoragePath(categoryTreeImageStoragePath),
     m_parameterStoragePath(parameterStoragePath),
     m_textLanguage(textLanguage),
     m_voiceLanguage(voiceLanguage),
     m_imageDimension(DIMENSION_0X0),
     m_connMgr(NULL)
{
   if(m_imageStoragePath == "") {
      m_imageStoragePath = commonDataStoragePath;
   }
   
   if(m_categoryImageStoragePath == "") {
      m_categoryImageStoragePath = commonDataStoragePath;
   }

   if(m_categoryTreeImageStoragePath == "") {
      m_categoryTreeImageStoragePath = commonDataStoragePath;
   }
   
   if(m_parameterStoragePath == "") {
      m_parameterStoragePath = commonDataStoragePath;
   }

#if defined NAV2_CLIENT_SERIES60_V3 || defined NAV2_CLIENT_SERIES60_V5
   m_imageExtension = MIF;
#elif defined _WIN32_WCE
   m_imageExtension = SVG;
#else
   m_imageExtension = PNG;
#endif
   
}

StartupData::StartupData(const WFString& commonDataStoragePath,
                         const WFString& dataStoragePath,
                         const WFString& resourceStoragePath,
                         TextLanguage textLanguage,
                         VoiceLanguage::VoiceLanguage voiceLanguage,
                         const WFString& imageStoragePath,
                         const WFString& categoryImageStoragePath,
                         const WFString& categoryTreeImageStoragePath)
   : m_commonDataStoragePath(commonDataStoragePath),
     m_dataStoragePath(dataStoragePath),
     m_resourceStoragePath(resourceStoragePath),
     m_imageStoragePath(imageStoragePath),
     m_categoryImageStoragePath(categoryImageStoragePath),
     m_categoryTreeImageStoragePath(categoryTreeImageStoragePath),
     m_textLanguage(textLanguage),
     m_voiceLanguage(voiceLanguage),
     m_imageDimension(DIMENSION_0X0),
     m_connMgr(NULL)
{
   if(m_imageStoragePath == "") {
      m_imageStoragePath = commonDataStoragePath;
   }
   
   if(m_categoryImageStoragePath == "") {
      m_categoryImageStoragePath = commonDataStoragePath;
   }

   if(m_categoryTreeImageStoragePath == "") {
      m_categoryTreeImageStoragePath = commonDataStoragePath;
   }
   
   m_parameterStoragePath = commonDataStoragePath;
#if defined NAV2_CLIENT_SERIES60_V3 || defined NAV2_CLIENT_SERIES60_V5
   m_imageExtension = MIF;
#elif defined _WIN32_WCE
   m_imageExtension = SVG;
#else
   m_imageExtension = PNG;
#endif
}

StartupData::StartupData(const StartupData& sd)
   : m_commonDataStoragePath(sd.m_commonDataStoragePath),
     m_dataStoragePath(sd.m_dataStoragePath),
     m_resourceStoragePath(sd.m_resourceStoragePath),
     m_imageStoragePath(sd.m_imageStoragePath),
     m_categoryImageStoragePath(sd.m_categoryImageStoragePath),
     m_categoryTreeImageStoragePath(sd.m_categoryTreeImageStoragePath),
     m_parameterStoragePath(sd.m_parameterStoragePath),
     m_textLanguage(sd.m_textLanguage),
     m_voiceLanguage(sd.m_voiceLanguage),
     m_imageExtension(sd.m_imageExtension),
     m_imageDimension(sd.m_imageDimension),
     m_connMgr(NULL)
{
}

StartupData::~StartupData()
{
}

const WFString&
StartupData::getCommonDataStoragePath() const
{
   return m_commonDataStoragePath;
}


const WFString&
StartupData::getDataStoragePath() const
{
   return m_dataStoragePath;
}

const WFString&
StartupData::getResourceStoragePath() const
{
   return m_resourceStoragePath;
}

const WFString&
StartupData::getImageStoragePath() const
{
   return m_imageStoragePath;
}

const WFString&
StartupData::getCategoryImageStoragePath() const
{
   return m_categoryImageStoragePath;
}

const WFString&
StartupData::getCategoryTreeImageStoragePath() const
{
   return m_categoryTreeImageStoragePath;
}

const WFString&
StartupData::getParameterStoragePath() const
{
   return m_parameterStoragePath;
}

ImageExtension
StartupData::getImageExtension() const
{
   return m_imageExtension;
}

ImageDimension
StartupData::getImageDimension() const
{
   return m_imageDimension;
}

TextLanguage
StartupData::getTextLanguage() const
{
   return m_textLanguage;
}

VoiceLanguage::VoiceLanguage
StartupData::getVoiceLanguage() const
{
   return m_voiceLanguage;
}

void
StartupData::setCommonDataStoragePath(const WFString& newVal)
{
   m_commonDataStoragePath = newVal;
}

void
StartupData::setDataStoragePath(const WFString& newVal)
{
   m_dataStoragePath = newVal;
}

void
StartupData::setResourceStoragePath(const WFString& newVal)
{
   m_resourceStoragePath = newVal;
}

void
StartupData::setImageStoragePath(const WFString& newVal)
{
   m_imageStoragePath = newVal;
}

void
StartupData::setCategoryImageStoragePath(const WFString& newVal)
{
   m_categoryImageStoragePath = newVal;
}

void
StartupData::setCategoryTreeImageStoragePath(const WFString& newVal)
{
   m_categoryTreeImageStoragePath = newVal;
}

void
StartupData::setTextLanguage(TextLanguage newVal)
{
   m_textLanguage = newVal;
}

void
StartupData::setVoiceLanguage(VoiceLanguage::VoiceLanguage newVal)
{
   m_voiceLanguage = newVal;
}

void
StartupData::setParameterStoragePath(const WFString& newVal)
{
   m_parameterStoragePath = newVal; 
}

void WFAPI::StartupData::setImageDimension(ImageDimension imageDimension)
{
   m_imageDimension = imageDimension;
}

void WFAPI::StartupData::setImageExtension(ImageExtension imageExtension)
{
   m_imageExtension = imageExtension;
}

ConnectionManager* WFAPI::StartupData::getConnectionManager() const
{
   return m_connMgr;
}

void
StartupData::setConnectionManager(ConnectionManager* connMgr)
{
   m_connMgr = connMgr;
}

} // End namespace WFAPI




