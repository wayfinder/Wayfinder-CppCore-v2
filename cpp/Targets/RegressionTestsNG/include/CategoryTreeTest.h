/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef CATEGORYTREETEST_H
#define CATEGORYTREETEST_H

#include "SearchListener.h"
#include "SearchInterface.h"
#include "RegressionTest.h"

#include <map>

class Nav2Session;

/**
 *
 */
class CategoryTreeTest : public WFAPI::SearchListener,
                         public RegressionTest
{
public:

   /**
    * Standard constructor.
    */
   CategoryTreeTest(Nav2Session& session, const char* name);

   /**
    * Destructor.
    */
   virtual ~CategoryTreeTest();
   
   /**
    * Starts the test.
    */
   virtual void startTest();

   /**
    * From SearchInterface, called when a search has triggered
    * an error in some way.
    *
    * @param status The status containing the error message.
    */
   virtual void error(const WFAPI::AsynchronousStatus& status);
   
   /**
    * A reply for a category tree request.
    *
    * @param requestID The identification of the category tree request
    *                  sent from the SearchInterface.
    * @param treeUpdated If this is true it means that we have received an
    *                    updated version of the category tree.
    *                    False if the category tree is up to date. 
   */
   virtual void categoryTreeReceived(WFAPI::RequestID requestID,
                                     bool treeUpdated);

private:
   /** 
    * Convenience function to test the category tree data structure.
    * 
    * @param categoryList The list of categories.
    * @param index  The start index for recursion.
    */
   virtual void testCategoryTree(WFAPI::CategoryTreeArray categoryList,
                                 int index, int subLevel);
   
private:
   /// The search interface
   WFAPI::SearchInterface& m_searchInterface;
};

#endif /* CATEGORYTREETEST_H */
