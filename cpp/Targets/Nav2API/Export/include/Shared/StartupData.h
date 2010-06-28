/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_STARTUP_DATA_H
#define WFAPI_STARTUP_DATA_H

#include "WFAPIConfig.h"
#include "TextLanguage.h"
#include "VoiceLanguage.h"
#include "ImageDimension.h"
#include "ImageExtension.h"
#include "WFString.h"

// Forward declarations
class ConnectionManager;

namespace WFAPI {

/**
 * Class for setting startup data for Nav2.
 */
class StartupData
{
public:

   /**
    * Platform compability help:
    *
    * The iPhone typically stores its application resources in the application
    * bundle. Therefore it is recommended that the commonDataStoragePath and
    * resourceStoragePath points to a folder inside the application bundle
    * directory.
    *
    * However, this folder is not writeable at run-time and you will therefore
    * need to specify the imageStoragePath, categoryImageStoragePath and
    * parameterStoragePath to point elsewhere. Apple recommends that you use
    * the Documents folder of your application to store such files. Therefore,
    * we recommend that the imageStoragePath, categoryImageStoragePath,
    * parameterStoragePath are all set and pointing to directories within
    * the Documents folder of your application.
    * 
    * @param commonDataStoragePath The path to where common data is stored.
    *                              An application will need to place a valid
    *                              paramseed.txt file in this directory.
    *
    *                              Nav2 will write updated parameter files,
    *                              images and category images to this path.
    *                              If you want to place these files in a
    *                              different folder, or if this folder is not
    *                              writeable, you will need to specify
    *                              different paths for these files using the
    *                              imageStoragePath, categoryImageStoragePath
    *                              and parameterStoragePath arguments.
    *                        
    * @param resourceStoragePath The path to the base point of where sound 
    *                            clips and syntaxes are stored. Nav2 expects
    *                            the sound clips and syntaxes to be stored in
    *                            a subdirectory of this. The name of
    *                            this subdirectory depends upon the format of
    *                            the sound files, which is specific for the
    *                            platform you are using ("Vorbis" or "MP3" for
    *                            example).
    *
    *                            If you intend to use sound files, place the
    *                            supplied files which are valid for your
    *                            platform in this path. Nav2 will not write
    *                            any files to this folder, so it only needs to
    *                            be readable.
    *                            
    * @param textLanguage The language to be used for texts in the application.
    * 
    * @param voiceLanguage The language to be used for voices in the 
    *                      application.
    *                      
    * @param imageStoragePath The path to where Nav2 will write its images when
    *                         they are downloaded.
    *                         
    * @param categoryImageStoragePath The path to where Nav2 will write its
    *                                 category images when they are downloaded.
    *                                 
    * @param categoryTreeImageStoragePath The path to where Nav2 will write
    *                                     its category tree images when
    *                                     they are downloaded.
    *
    * @param parameterStoragePath The path to where Nav2 will write its
    *                             parameter files. This path is necessary for
    *                             some platforms (such as iPhone) where the
    *                             common data storage path is not writable at
    *                             runtime. If this is left empty, Nav2 will
    *                             write its parameter files to the 
    *                             commonDataStoragePath argument instead.
    *
    *                             @see commonDataStoragePath.
    *         
    */
   StartupData(const WFString& commonDataStoragePath,
               const WFString& resourceStoragePath,
               TextLanguage textLanguage,
               VoiceLanguage::VoiceLanguage voiceLanguage,
               const WFString& imageStoragePath = "",
               const WFString& categoryImageStoragePath = "",
               const WFString& categoryTreeImageStoragePath = "",
               const WFString& parameterStoragePath = "");

   
   /**
    * Legacy constructor with the deprecated dataStoragePath parameter.
    * You should use the constructor above instead.
    *
    * @deprecated since 2009-07-07.
    * 
    * @param commonDataStoragePath The path to where common data is stored.
    *                              An application will need to place a valid
    *                              paramseed.txt file in this directory.
    *
    *                              Nav2 will write images and category images
    *                              to this path. If you want to place these
    *                              files in a different folder, you will need
    *                              to specify different paths for these files
    *                              using the imageStoragePath and
    *                              categoryImageStoragePath arguments.
    *                              
    * @param dataStoragePath The path to where platform dependent data files are
    *                        stored. This path was previously used for
    *                        version files, but is now completely unused by Nav2.
    *                        @deprecated since 2009-07-07.
    *                        
    * @param resourceStoragePath The path to the base point of where sound 
    *                            clips and syntaxes are stored. Nav2 expects
    *                            the sound clips and syntaxes to be stored in
    *                            a subdirectory of this. The name of
    *                            this subdirectory depends upon the format of
    *                            the sound files, which is specific for the
    *                            platform you are using ("Vorbis" or "MP3" for
    *                            example).
    *
    *                            If you intend to use sound files, place the
    *                            supplied files which are valid for your
    *                            platform in this path. Nav2 will not write
    *                            any files to this folder, so it only needs to
    *                            be readable.
    *                            
    * @param textLanguage The language to be used for texts in the application.
    * 
    * @param voiceLanguage The language to be used for voices in the 
    *                      application.
    *                      
    * @param imageStoragePath The path to where Nav2 will write its images when
    *                         they are downloaded.
    *                         
    * @param categoryImageStoragePath The path to where Nav2 will write its
    *                                 category images when they are downloaded.
    *                                 
    * @param categoryTreeImageStoragePath The path to where Nav2 will write
    *                                 its category tree images when they
    *                                 are downloaded.
    *
    */
   StartupData(const WFString& commonDataStoragePath,
               const WFString& dataStoragePath,
               const WFString& resourceStoragePath,
               TextLanguage textLanguage,
               VoiceLanguage::VoiceLanguage voiceLanguage,
               const WFString& imageStoragePath = "",
               const WFString& categoryImageStoragePath = "",
               const WFString& categoryTreeImageStoragePath = "");
   
   /**
    * Copy constructor
    */
   StartupData(const StartupData& sd);
   
   /**
    * Destructor.
    */
   virtual ~StartupData();
   
   /**
    * Get the path to where platform independent data files are stored.
    *
    * @return The path to where platform independent data files are stored.
    */
   const WFString& getCommonDataStoragePath() const;
   
   /**
    * Get the path to where platform dependent data files are stored.
    *
    * @return The path to where platform dependet data files are stored.
    */
   const WFString& getDataStoragePath() const;

   /**
    * Get the path to the base point for where sound clips and syntaxes are
    * stored.
    *
    * @return The path to the base point for storage of sound clips and
    *         syntaxes.
    */
   const WFString& getResourceStoragePath() const;

   /**
    * Get the path to where images are stored.
    *
    * @return The path to where images are stored.
    */
   const WFString& getImageStoragePath() const;

   /**
    * Get the path to where all category images are stored.
    *
    * @return The path to where all category images are stored.
    */
   const WFString& getCategoryImageStoragePath() const;

   /**
    * Get the path to where all category tree images are stored.
    *
    * @return The path to where all category tree images are stored.
    */
   const WFString& getCategoryTreeImageStoragePath() const;
   
   /**
    * Get the path to where all parameter files are stored.
    *
    * @return The path to where all parameter files are stored.
    */
   const WFString& getParameterStoragePath() const;

   /** 
    * Get the selected extension for images.
    * 
    * @return The extension used for images.
    */
   ImageExtension getImageExtension() const;

   /** 
    * Get the selected dimension for images.
    * 
    * @return The dimension used for images.
    */
   ImageDimension getImageDimension() const;
   
   /**
    * Get which language are selected for texts in the application. 
    *
    * @return The selected language.
    */
   TextLanguage getTextLanguage() const;

   /**
    * To get which language are selected for voice instructions in the
    * application. 
    *
    * @return The selected language.
    */
   VoiceLanguage::VoiceLanguage getVoiceLanguage() const;

   /**
    * To set the path to where to store platform independet files.
    *
    * @param newVal Where to store platform independent files.
    */
   void setCommonDataStoragePath(const WFString& newVal);
   
   /**
    * To set the path to where to store platform dependent files.
    *
    * @param newVal Where to store platform dependent files.
    */
   void setDataStoragePath(const WFString& newVal);

   /**
    * To set the path of the base point of where sound clips and syntaxes are
    * stored.
    *
    * @param newVal Where to to set the base point for storage of sound clips
    * and syntaxes.
    */
   void setResourceStoragePath(const WFString& newVal);

   /**
    * To set the path to where images are stored.
    *
    * @param newVal The path to where to store images.
    */
   void setImageStoragePath(const WFString& newVal);

   /**
    * To set the path to where category images are stored
    *
    * @param newVal Where category images are set.
    */
   void setCategoryImageStoragePath(const WFString& newVal);

   /**
    * To set the path to where category tree images are stored
    *
    * @param newVal Where category tree images are set.
    */
   void setCategoryTreeImageStoragePath(const WFString& newVal);
   
   /**
    * To set the path to where parameter files are stored.
    *
    * @param newVal Where parameter files are stored.
    */ 
   void setParameterStoragePath(const WFString& newVal);

   /** 
    * Set the dimension of the images to download. If not set the default value
    * will be used, meaning the images will be fetched with the dimension set
    * for that particular image on the server.
    * 
    * @param imageDimension The dimension of the images that will be downloaded.
    */
   void setImageDimension(ImageDimension imageDimension);

   /** 
    * Set the extension of the images to download, i.e set if they should be
    * PNG or SVG images.
    * 
    * @param imageExtension The extension of the images to download.
    */
   void setImageExtension(ImageExtension imageExtension);
   
   /**
    * To set which language to be used for texts in the application.
    *
    * @param newVal Which language to be used for texts in the application.
    */
   void setTextLanguage(TextLanguage newVal);

   /**
    * To set which language to be used for voices in the application.
    *
    * @param newVal Which language to be used for voices in the application.
    */
   void setVoiceLanguage(VoiceLanguage::VoiceLanguage newVal);

   // ------------ Internal functions for the api.----------------
    
   /**
    * Internal function to get the current connection manager.
    */
   ConnectionManager* getConnectionManager() const;
   
   /**
    * Internal function used to set the current connection manager.
    */
   void setConnectionManager(ConnectionManager* connMgr);

   
private:
   
   /**
    * Where to store platform independent files like parameter files and
    * downloaded images. These files are read and written during application 
    * use.
    */
   WFString m_commonDataStoragePath;

   /**
    * Where to store platform dependent data files. These files are read and
    * written during application use.
    */
   WFString m_dataStoragePath;

   /**
    * The base point for the sound clips and syntaxes. First there is a
    * subdirectory for the sound encoding, like Vorbis. Then in that directory
    * a directory for each language, like EN, SV. This directory is only read,
    * not modified.
    */
   WFString m_resourceStoragePath;

   /**
    * The path to where all images are stored. These files are read and written
    * during application use.
    */
   WFString m_imageStoragePath;

   /**
    * The path to where all category images are stored. These files are written
    * and read during application use.
    */
   WFString m_categoryImageStoragePath;

   /**
    * The path to where all category tree images are stored. These files
    * are written and read during application use.
    */
   WFString m_categoryTreeImageStoragePath;

   /**
    * The path to where the parameter files are written. 
    */ 
   WFString m_parameterStoragePath;

   /// The selected language for texts in the application.
   TextLanguage m_textLanguage;

   /// The selected language for voices in the application.
   VoiceLanguage::VoiceLanguage m_voiceLanguage;

   /// The selected extension for images.
   ImageExtension m_imageExtension;
   
   /// The selected dimensions for images.
   ImageDimension m_imageDimension;

   /// The connection manager to use internally.
   ConnectionManager* m_connMgr;
};

} // End namespace WFAPI

#endif // End WFAPI_STARTUP_DATA_H
