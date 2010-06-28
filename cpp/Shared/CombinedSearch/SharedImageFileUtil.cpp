/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "SharedImageFileUtil.h"
#include <cstring>
#include <stdio.h>

void
SharedImageFileUtil::checkAndAddImageName(CharVec_t& neededImages,
                                          const char* imageName,
                                          const char* imagePath,
                                          const char* imageExt)
{
   using namespace std;
   if (imageName && strcmp(imageName, "") != 0 ) {
      char* completeName = new char[strlen(imagePath) + strlen(imageName) + 
                                    strlen(imageExt) + 1];
      strcpy(completeName, imagePath);
      strcat(completeName, imageName);
      strcat(completeName, imageExt);
      FILE* imageFile = fopen(completeName, "r");
      if (imageFile == NULL) {
         // File do not exist on disk, check to see if it already exists
         // in the vector.
         
         // neededImages takes ownership over completeName
         bool addImgForDownload = neededImages.empty();
         for (CharVec_t::const_iterator it = neededImages.begin();
              it != neededImages.end();
              ++it) {
            if ( strstr((*it), completeName) != NULL ) {
               // Found the matching string, dont add it again
               addImgForDownload = false;
               break;
            } else {
               addImgForDownload = true;
            }
         }
         if (addImgForDownload) {
            neededImages.push_back( completeName );
         } else {
            delete[] completeName;
         }
      } else {
         fclose(imageFile);
         delete[] completeName;
      }
   }   
}

void
SharedImageFileUtil::checkAndAddImageName(
   CharVec_t& neededImages,
   const char* imageName,
   const char* imagePath,
   const char* imageExt,
   const CombinedSearch::CSImageDimension imageDimension)
{
   int tmp = imageDimension.width;
   int digitCountWidht = 0;
   while (tmp > 0) {
      tmp = tmp / 10;
      digitCountWidht++;
   }

   tmp = imageDimension.height;
   int digitCountHeight = 0;
   while (tmp > 0) {
      tmp = tmp / 10;
      digitCountHeight++;
   }
   char* imageNameDimension  = new char[strlen(imageName) + 1 + digitCountWidht
                                        + 1 + digitCountHeight + 1];
   strcpy(imageNameDimension, imageName);
   strcat(imageNameDimension, "_");
   sprintf(imageNameDimension, "%s%d", imageNameDimension, imageDimension.width);
   strcat(imageNameDimension, "x");
   sprintf(imageNameDimension, "%s%d", imageNameDimension, imageDimension.height);

   checkAndAddImageName(neededImages,
                        imageNameDimension,
                        imagePath,
                        imageExt);
   delete[] imageNameDimension;
}

bool
SharedImageFileUtil::checkAllImagesReceived(CharVec_t& requestedImages,
                                            const char* imageName)
{
   for (CharVec_t::iterator it = requestedImages.begin();
        it != requestedImages.end();
        ++it) {
      if (strstr((*it), imageName) != NULL) {
         // Found the matching string, erase since we got it from the server
         delete[] (*it);
         requestedImages.erase(it);
         if (requestedImages.size() == 0){
            return true;
         }
         // We found the image, so no need to search any further.
         return false;
      } 
   }
   return true;
}
