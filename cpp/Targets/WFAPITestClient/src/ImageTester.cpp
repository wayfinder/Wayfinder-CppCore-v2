/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "ImageTester.h"
#include "ImageInterface.h"
#include "WFString.h"
#include <iostream>

using namespace std;
using namespace WFAPI;

ImageTester::ImageTester(ImageInterface& imageInterface, const WFString& path)
        : m_imageInterface(imageInterface),m_path(path)
{
   m_imageInterface.addImageListener(this);
   addCommandOptions();
}

ImageTester::~ImageTester()
{

}

bool 
ImageTester::handleInput(const char* input, bool& goBack)
{

   StringMap::const_iterator it = m_commandOptions.find(input);

   if (it == m_commandOptions.end()) {
      return false;
   }
   switch((*it).second.m_commandType) {
   case help:  
      printHelp();
      break;
   case cmdDownloadImageToBuffer:
      downloadTestImageToBuffer();
      break;
   case cmdDownloadImageToFile:
      downloadTestImageToFile();
      break;
   case back:
      goBack = true;
      break;
   default:
      cout << "Command not handled here, please try again or select "
           << "B to go back." << endl;
      return false;
      break;
   }

   // If we get here, the key has been consumed.
   return true;
}   

void 
ImageTester::addCommandOptions()
{
   m_commandOptions.
      insert(make_pair("h",
                       CommandHolder("Displays this help menu.", help)));
   m_commandOptions.
      insert(make_pair("d", 
                       CommandHolder("Download image to buffer as data", 
                                     cmdDownloadImageToBuffer)));
   m_commandOptions.
      insert(make_pair("f", 
                       CommandHolder("download image to file", 
                                     cmdDownloadImageToFile)));
   m_commandOptions.
      insert(make_pair("b", 
                       CommandHolder("Back", 
                                     back)));
}

void
ImageTester::downloadTestImageToBuffer()
{
   m_imageInterface.setImagePath(m_path);
   m_imageInterface.setImageExtension(".png");
   m_imageInterface.getImage("policeStation", true);
}

void
ImageTester::downloadTestImageToFile()
{
   m_imageInterface.setImagePath(m_path);
   m_imageInterface.setImageExtension(".png");
   m_imageInterface.getImage("policeStation", false);
}

void
ImageTester::error(const AsynchronousStatus& status)
{
   printErrorStatus(&status, "ImageTester");
}

void
ImageTester::imageReply(RequestID requestID, const WFString& imageName)
{
   cout << "Image Downloaded" << endl;
   
}

void
ImageTester::imageReply(RequestID requestID, const WFString& imageName,
                        const ImageReplyData& imageReplyData)
{
   if (writeToFile(imageName, imageReplyData) == true) {
      cout << "The image was written to file." << endl;
   } else {
      cout << "Error when trying to write the buffer to file!" << endl;
   }
}

bool
ImageTester::writeToFile(WFString imageName,
                         const ImageReplyData& imageReplyData)
{
   // Check the filename, not the ultimate check but at least we check
   // that there is a name.
   if (imageName == "") {
      // No file exists
      return false;
   }
   // Create the file, if file already exists the content is erased.
   FILE* pSaveFile = fopen(imageName.c_str(), "wb");
   if (pSaveFile == NULL) {
      return false;
   }
   // Get the buffer for writing
   if (imageReplyData.getImageBuffer() != NULL) {
      // Write the buffer to the file
      fwrite(imageReplyData.getImageBuffer(),
             imageReplyData.getImageBufferSize(),
             1, pSaveFile);
      fclose(pSaveFile);
      return true;
   }
   // No buffer, this should never happen
   fclose(pSaveFile);
   return false;
}

