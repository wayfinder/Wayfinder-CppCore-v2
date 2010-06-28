/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef NAVLOCALCATEGORYTREEREPLY_H
#define NAVLOCALCATEGORYTREEREPLY_H

#include "NGP.h"
#include "NavPacket.h"
//#include "NavReplyPacket.h"
//#include "Common.h"
#include <vector>
#include <string>


/**
 *  NAV_LOCAL_CATEGORY_TREE_REPLY 

     Reply with the Category tree for the specified location. The reply has an 
     optional Categories crc parameter that is set if the request succeedes.
     The category tree data is sent in the category table, lookup table and string 
     table parameters.
    
 */
class NavLocalCategoryTreeReply : public isab::NavReplyPacket {
public:
   
   /**
    * Buffer constructor.
    * 
    * @param buff The buffer containing the data for the
    *             packet, assuming STX and length is stripped.
    * @param buffLen The length of the buffer in bytes.
    */
   NavLocalCategoryTreeReply(const byte* buff, uint32 buffLen)
    : NavReplyPacket(buff, buffLen) 
   {
      pal::wf_uint32 index = 0;
      
      { /* Array block */ 
         const isab::NParam* param = m_params.getParam(6503);
         
         pal::wf_uint32 arrIndex = 0;
         
         m_lookupTable.reserve(param->getLength() / sizeof(uint8));
         
         while(arrIndex < param->getLength()) {
            uint8 tmp = param->getUint8(arrIndex);
            m_lookupTable.push_back(tmp);
         }
      }
      
      
      { /* Array block */ 
         const isab::NParam* param = m_params.getParam(6502);
         
         pal::wf_uint32 arrIndex = 0;
         
         m_categoryTable.reserve(param->getLength() / sizeof(uint8));
         
         while(arrIndex < param->getLength()) {
            uint8 tmp = param->getUint8(arrIndex);
            m_categoryTable.push_back(tmp);
         }
      }
      
      if(m_params.getParam(6500) != NULL) {
         m_categoryTreeCrc = m_params.getParam(6500)->getString();
      }
      // m_crcOK = boolean(m_params.getParam(6505), index);
      m_crcOK = m_params.getParam(6505)->getBool();
      
      { /* Array block */ 
         const isab::NParam* param = m_params.getParam(6504);
         
         pal::wf_uint32 arrIndex = 0;
         
         m_stringTable.reserve(param->getLength() / sizeof(uint8));
         
         while(arrIndex < param->getLength()) {
            uint8 tmp = param->getUint8(arrIndex);
            m_stringTable.push_back(tmp);
         }
      }
      
      index = 0; // To avoid unused variable warning
   }
   
   virtual ~NavLocalCategoryTreeReply() {}
   
   /**
    * NavLocalCategoryTreeReply object constructor.
    * 
    * @param lookupTable Used to get the byte offset of a categor...
    * @param categoryTable Contains information about categories (s...
    * @param categoryTreeCrc The crc from a previous NAV\_LOCAL\_CATE...
    * @param crcOK Indicates that the crc sent in the reque...
    * @param stringTable Table containing category names and icon...
    */
   NavLocalCategoryTreeReply(
      const std::vector< pal::wf_uint8 >& argLookupTable,
      const std::vector< pal::wf_uint8 >& argCategoryTable,
      const std::string& argCategoryTreeCrc,
      const bool& argCrcOK,
      const std::vector< pal::wf_uint8 >& argStringTable) : 
      isab::NavReplyPacket(NGP_PROTOCOL_VERSION,
                           isab::navRequestType::NAV_LOCAL_CATEGORY_TREE_REPLY,
                           0, 1),
      m_lookupTable(argLookupTable),
      m_categoryTable(argCategoryTable),
      m_categoryTreeCrc(argCategoryTreeCrc),
      m_crcOK(argCrcOK),
      m_stringTable(argStringTable)
   {}
   
   /**
    *  NavLocalCategoryTreeReply default constructor
    *  
    *  @param lookupTable Used to get the byte offset of a categor..
    *  @param categoryTable Contains information about categories (s..
    *  @param categoryTreeCrc The crc from a previous NAV\_LOCAL\_CATE..
    *  @param crcOK Indicates that the crc sent in the reque..
    *  @param stringTable Table containing category names and icon..
    */
   NavLocalCategoryTreeReply() : 
      isab::NavReplyPacket(NGP_PROTOCOL_VERSION,
                           isab::navRequestType::NAV_LOCAL_CATEGORY_TREE_REPLY, 0, 1),
      m_lookupTable(),
      m_categoryTable(),
      m_categoryTreeCrc(),
      m_crcOK(),
      m_stringTable()
   {}
   
   /**
    * Compare equals.
    */
   bool operator == (const NavLocalCategoryTreeReply& rhs) const {
      return
         NavReplyPacket::operator == (rhs) &&
         m_lookupTable == rhs.m_lookupTable &&
         m_categoryTable == rhs.m_categoryTable &&
         m_categoryTreeCrc == rhs.m_categoryTreeCrc &&
         m_crcOK == rhs.m_crcOK &&
         m_stringTable == rhs.m_stringTable;
   }
   
   /**
    *  NavLocalCategoryTreeReply serialization function
    *  @param param The param that will be filled with data.
    */ 
   virtual void serialize(isab::NParamBlock& block) const {
      using namespace isab;
      NParam& tmpLookupTable = block.addParam(NParam(6503));
      
      for(unsigned int j = 0; j < m_lookupTable.size(); j++) {
         tmpLookupTable.addUint8(m_lookupTable[j]);
      }
      NParam& tmpCategoryTable = block.addParam(NParam(6502));
      
      for(unsigned int j = 0; j < m_categoryTable.size(); j++) {
         tmpCategoryTable.addUint8(m_categoryTable[j]);
      }
      NParam& tmpCategoryTreeCrc = block.addParam(NParam(6500));
      tmpCategoryTreeCrc.addString(m_categoryTreeCrc.c_str());
      NParam& tmpCrcOK = block.addParam(NParam(6505));
      tmpCrcOK.addBool(m_crcOK);
      NParam& tmpStringTable = block.addParam(NParam(6504));
      
      for(unsigned int j = 0; j < m_stringTable.size(); j++) {
         tmpStringTable.addUint8(m_stringTable[j]);
      }
   }
   
   // Object getters
   /// @return Lookup table
   const std::vector< pal::wf_uint8 >& getLookupTable() const { return m_lookupTable; }
   
   /// @return Category table
   const std::vector< pal::wf_uint8 >& getCategoryTable() const { return m_categoryTable; }
   
   /// @return CategoryTreeCrc
   const std::string& getCategoryTreeCrc() const { return m_categoryTreeCrc; }
   
   /// @return crc ok
   const bool& getCrcOK() const { return m_crcOK; }
   
   /// @return string table
   const std::vector< pal::wf_uint8 >& getStringTable() const { return m_stringTable; }
   
private:
   /// Used to get the byte offset of a category in the category table.
   std::vector< pal::wf_uint8 > m_lookupTable;
   
   /**
    * Contains information about categories (sub categories, offset
    * of name and icon in the string table)
    */
   std::vector< pal::wf_uint8 > m_categoryTable;
   
   /**
    * The crc from a previous NAV_LOCAL_CATEGORY_REPLY. Send
    * empty attribute if no crc is available.
    */
   std::string m_categoryTreeCrc;
   
   /**
    * Indicates that the crc sent in the request matched the
    * latest version. No new category tree is sent when this
    * parameter is true.
    */
   bool m_crcOK;
   
   /// Table containing category names and icon names.
   std::vector< pal::wf_uint8 > m_stringTable;
};

#endif // NAVLOCALCATEGORYTREEREPLY_H
