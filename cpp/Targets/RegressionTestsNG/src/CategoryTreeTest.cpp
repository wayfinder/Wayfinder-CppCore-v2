/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#define CORE_LOGGING_MODULE_NAME "CategoryTreeTest"
#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE

#include "CoreDebugPrint.h"

#include "CategoryTreeTest.h"

#include "WGS84Coordinate.h"
#include "Nav2Session.h"
#include "Nav2API.h"
#include "CategoryTreeArray.h"
#include <string>

CategoryTreeTest::CategoryTreeTest(Nav2Session& session, const char* name)
   : RegressionTest(name, 30000),
     m_searchInterface(session.getNav2()->getSearchInterface())
{
}

CategoryTreeTest::~CategoryTreeTest()
{
   m_searchInterface.removeSearchListener(this);
}

void CategoryTreeTest::startTest()
{
   m_searchInterface.addSearchListener(this);
   
   // Request category tree from server.
   m_searchInterface.requestCategoryTree(WFAPI::WGS84Coordinate(55.7, 13.2));
}

void CategoryTreeTest::error(const WFAPI::AsynchronousStatus& status)
{
   setTestFailed("Failed to request category tree.");
}

void CategoryTreeTest::categoryTreeReceived(WFAPI::RequestID requestID,
                                            bool treeUpdated)
{
   coreprintln("Category updated: %d", treeUpdated);
   WFAPI::CategoryTreeArray rootCategoryList;
   m_searchInterface.getRootCategories(rootCategoryList);
   if (rootCategoryList.empty()) {
      // Could not get any categories from server.
      setTestFailed("Root category list empty.");
      return;
   }

   // Test integrity of tree.
   testCategoryTree(rootCategoryList, 0, 0);

   // If testCategoryTree did not crash or segfault or anything else bad,
   // then the test passed.
   setTestPassed();
}

void
CategoryTreeTest::testCategoryTree(WFAPI::CategoryTreeArray categoryList,
                                   int index, int subLevel)
{
   if (categoryList.empty() || (index >= int(categoryList.size()))) {
      return;
   }
   
   std::string levelStr;
   for (int i = 0; i <= subLevel; ++i) {
      levelStr += ">";
   }
   
   int catId = categoryList[index].getCategoryID();
   // Print the categoryId and the category name and image name 
   // for all categories.
   coreprintln("%s CategoryId: %d",
               levelStr.c_str(), catId);
   coreprintln("%s CategoryName: %s",
               levelStr.c_str(), categoryList[index].getName().c_str());
   coreprintln("%s CategoryImage: %s",
               levelStr.c_str(), categoryList[index].getImageName().c_str());
   
   // Get subcategories if there are any.
   WFAPI::CategoryTreeArray subCategoryList;
   m_searchInterface.getSubCategoriesOf(catId, subCategoryList);
   
   // Recursive calls to get all categories from the category tree.
   if (!subCategoryList.empty()) {
      testCategoryTree(subCategoryList, 0, subLevel + 1);
   } 
   testCategoryTree(categoryList, index + 1, subLevel);
}
