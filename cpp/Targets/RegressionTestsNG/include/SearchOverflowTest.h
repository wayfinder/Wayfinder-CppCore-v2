/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef _SEARCHOVERFLOWTEST_H_
#define _SEARCHOVERFLOWTEST_H_

#include "SearchListener.h"
#include "SearchInterface.h"
#include "RegressionTest.h"

class Nav2Session;

class SearchOverflowTest : public WFAPI::SearchListener,
                           public RegressionTest {
public:
   enum state_t { UNINITIALIZED,
                  SEARCHING_FOR,
                  ERROR,
                  REQUESTING_UNTIL_END,
                  REQUESTING_BACK_ONE,
                  REQUESTING_FORWARD_END,
                  DONE };
   
   SearchOverflowTest(Nav2Session& session, const char* name);

   virtual ~SearchOverflowTest();
   
   virtual void startTest();
   
   bool finished();

   virtual void searchReply(WFAPI::RequestID requestID,
                            const WFAPI::SearchHeadingArray& searchHeadings,
                            bool final);
   
   virtual void nextSearchReply(WFAPI::RequestID requestID,
                                const WFAPI::SearchItemArray& searchItemArray,
                                WFAPI::wf_uint32 heading);

   virtual void error(const WFAPI::AsynchronousStatus& status);

   const char* getError();
private:
   unsigned int m_reqSize;
   
   std::vector<WFAPI::SearchItemArray> m_results;
   
   WFAPI::SearchInterface& m_searchInterface;
   
   state_t m_state;
   
   unsigned int m_curIndex;
   
   unsigned int m_totalHits;
   
   WFAPI::wf_uint32 m_prevDist;
   
   const char* m_errorString;
};

#endif /* _SEARCHOVERFLOWTEST_H_ */
