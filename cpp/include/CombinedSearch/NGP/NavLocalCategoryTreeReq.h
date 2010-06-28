/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef NAVLOCALCATEGORYTREEREQ_H
#define NAVLOCALCATEGORYTREEREQ_H

#include "NGP.h"
#include "NavPacket.h"
//#include "NavRequestPacket.h"
#include "NavServerComEnums.h"
#include "Common.h"
#include <string>


/**
 *  NAV_LOCAL_CATEGORY_TREE_REQ 

      Request to get the category tree for the specified location. This request 
      contains a Category tree crc parameter, a Language of client, see Section \ref{LanguageClient}, 
      parameter, a Category tree version parameter, and a position parameter which the
      server use to get a specific category tree for the client's area. See 
      description of parameter 4308 in Section \ref{Category position}.
    
 */
class NavLocalCategoryTreeReq : public isab::NavRequestPacket {
public:
   
   /**
    * Buffer constructor.
    * 
    * @param buff The buffer containing the data for the
    *             packet, assuming STX and length is stripped.
    * @param buffLen The length of the buffer in bytes.
    */
   NavLocalCategoryTreeReq(const byte* buff, uint32 buffLen)
      : isab::NavRequestPacket(buff, buffLen) 
   {
      uint32 index = 0;
      if(m_params.getParam(6) != NULL) {
         m_clientLanguage =
            isab::NavServerComEnums::languageCode(m_params.getParam(6)->getUint16());
      }
      if(m_params.getParam(6501) != NULL) {
         m_categoryTreeVersion = m_params.getParam(6501)->getUint16();
      }
      m_posForCategories = Coordinate(m_params.getParam(4308), index);
      if(m_params.getParam(6500) != NULL) {
         m_categoryTreeCrc = m_params.getParam(6500)->getString();
      }
      index = 0; // To avoid unused variable warning
   }
   
   virtual ~NavLocalCategoryTreeReq() {}
   
   /**
    * NavLocalCategoryTreeReq object constructor.
    * 
    * @param clientLanguage Language of the client. Should always be...
    * @param categoryTreeVersion The version format to use in the        ...
    * @param posForCategories ...
    * @param categoryTreeCrc The crc from a previous NAV\_LOCAL\_CATE...
    */
   NavLocalCategoryTreeReq(
      const isab::NavServerComEnums::languageCode& argClientLanguage,
      const uint16& argCategoryTreeVersion,
      const Coordinate& argPosForCategories,
      const std::string& argCategoryTreeCrc) : 
      NavRequestPacket(NGP_PROTOCOL_VERSION,
                       isab::navRequestType::NAV_LOCAL_CATEGORY_TREE_REQ, 0, 1),
      m_clientLanguage(argClientLanguage),
      m_categoryTreeVersion(argCategoryTreeVersion),
      m_posForCategories(argPosForCategories),
      m_categoryTreeCrc(argCategoryTreeCrc)
   {}
   
   /**
    *  NavLocalCategoryTreeReq default constructor
    *  
    *  @param clientLanguage Language of the client. Should always be..
    *  @param categoryTreeVersion The version format to use in the        ..
    *  @param posForCategories ..
    *  @param categoryTreeCrc The crc from a previous NAV\_LOCAL\_CATE..
    */
   NavLocalCategoryTreeReq() : 
      NavRequestPacket(NGP_PROTOCOL_VERSION,
                       isab::navRequestType::NAV_LOCAL_CATEGORY_TREE_REQ, 0, 1),
      m_clientLanguage(),
      m_categoryTreeVersion(),
      m_posForCategories(),
      m_categoryTreeCrc()
   {}
   
   /**
    * Compare equals.
    */
   bool operator == (const NavLocalCategoryTreeReq& rhs) const {
      return
         NavRequestPacket::operator == (rhs) &&
         m_clientLanguage == rhs.m_clientLanguage &&
         m_categoryTreeVersion == rhs.m_categoryTreeVersion &&
         m_posForCategories == rhs.m_posForCategories &&
         m_categoryTreeCrc == rhs.m_categoryTreeCrc;
   }
   
   /**
    *  NavLocalCategoryTreeReq serialization function
    *  @param param The param that will be filled with data.
    */ 
   virtual void serialize(isab::NParamBlock& block) const {
      using namespace isab;
      NParam& tmpClientLanguage = block.addParam(NParam(6));
      tmpClientLanguage.addUint16(m_clientLanguage);
      NParam& tmpCategoryTreeVersion = block.addParam(NParam(6501));
      tmpCategoryTreeVersion.addUint16(m_categoryTreeVersion);
      NParam& tmpPosForCategories = block.addParam(NParam(4308));
      m_posForCategories.serialize(tmpPosForCategories);
      NParam& tmpCategoryTreeCrc = block.addParam(NParam(6500));
      tmpCategoryTreeCrc.addString(m_categoryTreeCrc.c_str());
   }
   
   // Object getters
   /// @return Language of client
   const isab::NavServerComEnums::languageCode& getClientLanguage() const { return m_clientLanguage; }
   
   /// @return CategoryTreeVersion
   const uint16& getCategoryTreeVersion() const { return m_categoryTreeVersion; }
   
   /// @return Pos for categories
   const Coordinate& getPosForCategories() const { return m_posForCategories; }
   
   /// @return CategoryTreeCrc
   const std::string& getCategoryTreeCrc() const { return m_categoryTreeCrc; }
   
private:
   /// Language of the client. Should always be sent as it    affects almost every request. See Section \ref{languageCode} for languages.
   isab::NavServerComEnums::languageCode m_clientLanguage;
   /// The version format to use in the                          NAV\_LOCAL\_CATEGORY\_REPLY.
   uint16 m_categoryTreeVersion;
   /// 
   Coordinate m_posForCategories;
   /// The crc from a previous NAV\_LOCAL\_CATEGORY\_REPLY. Send                          empty attribute if no crc is available.
   std::string m_categoryTreeCrc;
};

#endif // NAVLOCALCATEGORYTREEREQ_H
