/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef SHARED_IMAGE_FILE_UTIL_H
#define SHARED_IMAGE_FILE_UTIL_H

#include "arch.h"
#include <vector>
#include "CSImageDimension.h"

class SharedImageFileUtil {
public:

   typedef std::vector<const char*> CharVec_t;

   /** 
    * Checks if a image file is downloaded or not. If not
    * the image name is added to the neededImages. The neededImages
    * vector can then be used to request downloads later.
    *
    * @param neededImages Vector containing image names that should be
    *                     downloaded, if file doesnt exist on disk and
    *                     not already in the vector it will be pushed to
    *                     this vector.
    * @param imageName The name of the image to search for.
    * @param imagePath The path to download the file to.
    * @param imageDimension The dimension of the image.
    */
   static void checkAndAddImageName(
      CharVec_t& neededImages,
      const char* imageName,
      const char* imagePath,
      const char* imageExt,
      const CombinedSearch::CSImageDimension imageDimension);
   
   /** 
    * Checks if a image file is downloaded or not. If not
    * the image name is added to the neededImages. The neededImages
    * vector can then be used to request downloads later.
    *
    * @param neededImages Vector containing image names that should be
    *                     downloaded, if file doesnt exist on disk and
    *                     not already in the vector it will be pushed to
    *                     this vector.
    * @param imageName The name of the image to search for.
    * @param imagePath The path to download the file to.
    */
   static void checkAndAddImageName(CharVec_t& neededImages,
                                    const char* imageName,
                                    const char* imagePath,
                                    const char* imageExt);

   /** 
    * Checks if the image (imageName) was in the list (requestedImages). If
    * so we erase it and checks the size of the list. If the list is empty
    * we return true, all images has been downloaded (there are no more
    * in the list at least). If the list isnt empty we return false.
    * 
    * @param requestedImages The vector of image names, contains a list
    *                        of names that should be downloaded.
    * @param imageName The name to look for in the list.
    * 
    * @return True if the list is empty, false if not.
    */
   static bool checkAllImagesReceived(CharVec_t& requestedImages,
                                      const char* imageName);

};

#endif // SHARED_IMAGE_FILE_UTIL_H
