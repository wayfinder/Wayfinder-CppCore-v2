/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#undef WIN32_NO_OLE

#define CORE_LOGGING_MODULE_NAME "NAV2_SESSION"


#if defined NAV2_CLIENT_I386
#include <LinuxNav2API.h>
#elif defined NAV2_CLIENT_MAEMO
#include <MaemoNav2API.h>
#elif defined (_WIN32_WCE)
#include <windows.h>
#include <aygshell.h>
#include <shlobj.h>
#include <WindowsMobileNav2API.h>
#elif defined (IPHONE)
#include <IPhonePathUtil.h>
#include <IPhoneNav2API.h>
#elif defined __SYMBIAN__
# include <eikappui.h>
# include <eikapp.h>
# include <f32file.h>
# include <coemain.h>
# include <eikapp.h>
# include <e32const.h>
# include <WFTextUtil.h>
# include <S60Nav2API.h>
# include <e32base.h>
#elif defined (IPHONE)
#include <IPhoneNav2API.h>
#endif

#include <string.h>
#include "Nav2Session.h"
#include "StartupData.h"
#include "Environment.h"
#include "CoreDebugPrint.h"
#include "Nav2API.h"
#include "MapLibAPI.h"
#include "RegressionTestErrorPrinting.h"
#include <string.h>

using namespace WFAPI;

Nav2API* Nav2Session::getNav2()
{
   return m_nav2;
}

Nav2Session::Nav2Session()
{   
   m_paramSeedPath = NULL;
   m_paramFilesPath = NULL;
   m_resourcePath = NULL;
   m_imagesPath = NULL;
   m_catImagesPath = NULL;
   m_catTreeImagesPath = NULL;
   m_nav2 = NULL;
   
   initPaths();
   createNav2();
   m_mapLib = NULL;
}

Nav2Session::~Nav2Session()
{
   delete[] m_paramSeedPath;
   delete[] m_paramFilesPath;
   delete[] m_resourcePath;
   delete[] m_imagesPath;
   delete[] m_catImagesPath;
   delete[] m_catTreeImagesPath;

   deleteMapLib();
   deleteNav2();
}

void Nav2Session::deleteMapLib()
{
   delete m_mapLib;
   m_mapLib = NULL;
}

void Nav2Session::deleteNav2()
{
   if(m_nav2)  {
      m_nav2->setStatusListener(NULL);
   }
   
   delete m_nav2;
   m_nav2 = NULL;
}

void Nav2Session::initPaths()
{
#ifdef _WIN32_WCE
   m_paramSeedPath = new char[MAX_PATH];
   m_paramFilesPath = new char[MAX_PATH];
   m_resourcePath = new char[MAX_PATH];
   m_imagesPath = new char[MAX_PATH];
   m_catImagesPath = new char[MAX_PATH];
   m_catTreeImagesPath = new char[MAX_PATH];
   
   wchar_t wideProgramFilesPath[MAX_PATH];
   char programFilesPath[MAX_PATH];
   
   memset(m_paramSeedPath, 0, MAX_PATH);
   memset(m_paramFilesPath, 0, MAX_PATH);
   memset(m_resourcePath, 0, MAX_PATH);
   memset(m_imagesPath, 0, MAX_PATH);
   memset(m_catImagesPath, 0, MAX_PATH);
   memset(m_catTreeImagesPath, 0, MAX_PATH);
   
   memset(wideProgramFilesPath, 0, MAX_PATH * 2);
   memset(programFilesPath, 0, MAX_PATH);
   
   if(!SUCCEEDED(SHGetSpecialFolderPath(NULL,
                                        wideProgramFilesPath,
                                        CSIDL_PROGRAM_FILES,
                                        true))) 
   {
      //Uh-oh
   }

   DWORD uniLen = WideCharToMultiByte(CP_OEMCP,
                                      0,
                                      wideProgramFilesPath,
                                      wcslen(wideProgramFilesPath),
                                      programFilesPath,
                                      MAX_PATH,
                                      NULL, NULL);
   
   sprintf(m_paramSeedPath, "%s\\%s", programFilesPath,
           "RegressionTester\\nav2data\\");
   sprintf(m_paramFilesPath, "%s\\%s", programFilesPath,
           "RegressionTester\\nav2data\\");
   sprintf(m_resourcePath, "%s\\%s", programFilesPath,
           "RegressionTester\\nav2data\\");
   sprintf(m_imagesPath, "%s\\%s", programFilesPath,
           "RegressionTester\\nav2data\\");
   sprintf(m_catImagesPath, "%s\\%s", programFilesPath,
           "RegressionTester\\nav2data\\csicons\\");
   sprintf(m_catTreeImagesPath, "%s\\%s", programFilesPath,
           "RegressionTester\\nav2data\\cticons\\");
#elif defined (IPHONE)
   m_paramSeedPath = new char[512];
   m_paramFilesPath = new char[512];
   m_resourcePath = new char[512];
   m_imagesPath = new char[512];
   m_catImagesPath = new char[512];
   m_catTreeImagesPath = new char[512];
   
   memset(m_paramSeedPath, 0, 512);
   memset(m_paramFilesPath, 0, 512);
   memset(m_resourcePath, 0, 512);
   memset(m_imagesPath, 0, 512);
   memset(m_catImagesPath, 0, 512);
   memset(m_catTreeImagesPath, 0, 512);
   
   std::string bundlePath = IPhonePathUtil::getBundlePath();
   std::string docPath = IPhonePathUtil::getDocumentsPath();

   sprintf(m_paramSeedPath, "%s/nav2data/", bundlePath.c_str());
   sprintf(m_paramFilesPath, "%s/", docPath.c_str());
   sprintf(m_resourcePath, "%s/", docPath.c_str());
   sprintf(m_imagesPath, "%s/", docPath.c_str());
   sprintf(m_catImagesPath, "%s/", docPath.c_str());
   sprintf(m_catTreeImagesPath, "%s/", docPath.c_str());
   
#elif defined __SYMBIAN32__

   m_paramSeedPath = new char[KMaxFileName];
   m_paramFilesPath = new char[KMaxFileName];
   m_resourcePath = new char[KMaxFileName];
   m_imagesPath = new char[KMaxFileName];
   m_catImagesPath = new char[KMaxFileName];
   m_catTreeImagesPath = new char[KMaxFileName];
   
   _LIT(KSysDataBasePath, "\\shared\\");
   _LIT(KPathTrailer, "\\");

   RProcess self;
   HBufC* applicationDllName = self.FileName().AllocL();
   RDebug::Print(_L("Application name = %S"), applicationDllName);
   // coreprintln("Application name = %s", *path);
   self.Close();
   
   class TParsePtrC pathParser(*applicationDllName);
   HBufC* applicationName = pathParser.Name().AllocL();
   HBufC* applicationNameBase = NULL;
      
   TInt sepPos = 0;
   sepPos = applicationName->LocateReverse('_');
   if (sepPos != KErrNotFound) {
      // Create a application name without the architecture type in it.
      applicationNameBase = HBufC::NewL(applicationName->Length());
      applicationNameBase->Des().Copy(applicationName->Left(sepPos));
   } else {
      applicationNameBase = applicationName->AllocL();
   }

   HBufC* dataPath = HBufC::NewL(pathParser.Drive().Length() +
                                 KSysDataBasePath().Length() +
                                 applicationName->Length() +
                                 KPathTrailer().Length());
      
   dataPath->Des().Copy(pathParser.Drive());
   dataPath->Des().Append(KSysDataBasePath);
   dataPath->Des().Append(*applicationNameBase);
   dataPath->Des().Append(KPathTrailer);

   //This is since when using the SDK the app drive is Z.
#if defined __WINS__ || defined __WINSCW__
   dataPath->Des()[0] = 'c';
#endif

   delete applicationDllName;
   delete applicationName;
   delete applicationNameBase;

   const char* programFilesPath = WFTextUtil::TDesCToUtf8L(*dataPath);

   sprintf(m_paramSeedPath, "%s%s", programFilesPath,
           "nav2data\\");
   sprintf(m_paramFilesPath, "%s%s", programFilesPath,
           "nav2data\\");
   sprintf(m_resourcePath, "%s%s", programFilesPath,
           "nav2data\\");
   sprintf(m_imagesPath, "%s%s", programFilesPath,
           "nav2data\\");
   sprintf(m_catImagesPath, "%s%s", programFilesPath,
           "nav2data\\csicons\\");
   sprintf(m_catTreeImagesPath, "%s%s", programFilesPath,
           "nav2data\\cticons\\");
   
   RFs fs;
   if (fs.Connect() == KErrNone) {
      HBufC* paramSeedPath = WFTextUtil::AllocLC(m_paramSeedPath);
      HBufC* paramFilesPath = WFTextUtil::AllocLC(m_paramFilesPath);
      HBufC* resourcePath = WFTextUtil::AllocLC(m_resourcePath);
      HBufC* imagesPath = WFTextUtil::AllocLC(m_imagesPath);
      HBufC* catImagesPath = WFTextUtil::AllocLC(m_catImagesPath);
      HBufC* catTreeImagesPath = WFTextUtil::AllocLC(m_catTreeImagesPath);
      // RDebug::Print(_L("m_paramSeedPath: %S"), paramSeedPath);
      // RDebug::Print(_L("m_paramFilesPath: %S"), paramFilePath);
      // RDebug::Print(_L("m_resourcePath: %S"), resourcePath);
      // RDebug::Print(_L("m_imagesPath: %S"), imagesPath);
      // RDebug::Print(_L("m_catImagesPath: %S"), catImagesPath);
      fs.MkDirAll(*paramSeedPath);
      fs.MkDirAll(*paramFilesPath);
      fs.MkDirAll(*resourcePath);
      fs.MkDirAll(*imagesPath);
      fs.MkDirAll(*catImagesPath);
      fs.MkDirAll(*catTreeImagesPath);
      CleanupStack::PopAndDestroy(catTreeImagesPath);
      CleanupStack::PopAndDestroy(catImagesPath);
      CleanupStack::PopAndDestroy(imagesPath);
      CleanupStack::PopAndDestroy(resourcePath);
      CleanupStack::PopAndDestroy(paramFilesPath);
      CleanupStack::PopAndDestroy(paramSeedPath);
      fs.Close();
   }

   delete programFilesPath;
#else
   m_paramSeedPath = new char[512];
   m_paramFilesPath = new char[512];
   m_resourcePath = new char[512];
   m_imagesPath = new char[512];
   m_catImagesPath = new char[512];
   m_catTreeImagesPath = new char[512];
   
   memset(m_paramSeedPath, 0, 512);
   memset(m_paramFilesPath, 0, 512);
   memset(m_resourcePath, 0, 512);
   memset(m_imagesPath, 0, 512);
   memset(m_catImagesPath, 0, 512);
   memset(m_catTreeImagesPath, 0, 512);
   
   sprintf(m_paramSeedPath, "%s", "./nav2data/");
   sprintf(m_paramFilesPath, "%s", "./nav2data/");
   sprintf(m_resourcePath, "%s", "./nav2data/");
   sprintf(m_imagesPath, "%s", "./nav2data/csicons/");
   sprintf(m_catImagesPath, "%s", "./nav2data/csicons/");
   sprintf(m_catTreeImagesPath, "%s", "./nav2data/cticons/");
#endif
}

void Nav2Session::createNav2()
{
#if defined NAV2_CLIENT_I386
   m_nav2 = new LinuxNav2API;
#elif defined NAV2_CLIENT_MAEMO
   m_nav2 = new MaemoNav2API;
#elif defined(_WIN32_WCE)
   m_nav2 = new WindowsMobileNav2API;
#elif defined NAV2_CLIENT_SERIES60_V3 || defined NAV2_CLIENT_SERIES60_V5
   m_nav2 = new S60Nav2API;
   m_nav2->setHttpProto(true);
   m_nav2->setClientType("wf-r1v-vf");
#elif defined(IPHONE)
   m_nav2 = new IPhoneNav2API;
#endif
}

void Nav2Session::error(const AsynchronousStatus& s)
{
   dumpError(s);
}

void Nav2Session::startupComplete()
{
   coreprintln("Nav2Session::startupComplete - SHOULD NOT HAPPEN");
}

void Nav2Session::mapLibStartupComplete()
{
   coreprintln("Nav2Session::mapLibStartupComplete - SHOULD NOT HAPPEN");
}

void Nav2Session::stopComplete()
{
   coreprintln("Nav2Session::stopComplete - SHOULD NOT HAPPEN");
}

WFAPI::MapLibAPI* Nav2Session::getMapLib()
{
   return m_mapLib;
}

void Nav2Session::setMapLib(WFAPI::MapLibAPI* mapLib)
{
   m_mapLib = mapLib; 
}

void Nav2Session::redirectStatusNotification(WFAPI::Nav2StatusListener* newListener)
{
   m_nav2->removeStatusListener(this);
   m_nav2->addStatusListener(newListener);
}

void Nav2Session::resetStatusNotification(WFAPI::Nav2StatusListener* prevListener)
{
   m_nav2->removeStatusListener(prevListener);
   m_nav2->addStatusListener(this);
}

const char* Nav2Session::getParamSeedPath()
{
   return m_paramSeedPath;
}

const char* Nav2Session::getParameterStoragePath()
{
   return m_paramFilesPath;
}

const char* Nav2Session::getResourceStoragePath()
{
   return m_resourcePath;
}

const char* Nav2Session::getImageStoragePath()
{
   return m_imagesPath;
}

const char* Nav2Session::getCategoryImageStoragePath()
{
   return m_catImagesPath;
}

const char* Nav2Session::getCategoryTreeImageStoragePath()
{
   return m_catTreeImagesPath;
}
