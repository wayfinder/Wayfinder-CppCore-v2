/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "arch.h"
#include "RegressionTest.h"
#include "WFString.h"

using namespace WFAPI;

RegressionTest::RegressionTest(const char* name,
                               unsigned int timeOutMs) :
   m_name(name),
   m_timeOutMs(timeOutMs)
{
   m_testFinished = false;
   m_testPassed = false;
}

RegressionTest::~RegressionTest()
{
    
}

void RegressionTest::setTestPassed()
{
   m_testFinished = true;
   m_testPassed = true;
}

void RegressionTest::setTestFailed(const WFString& failReason)
{
   m_testFinished = true;
   m_testPassed = false;
   m_failReason = failReason;
   throw m_exception;
}

const bool& RegressionTest::isTestFinished()
{
   return m_testFinished;
}

bool RegressionTest::testPassed()
{
   return m_testPassed;
}

const WFAPI::WFString& RegressionTest::getFailedReason()
{
   return m_failReason; 
}

unsigned int RegressionTest::getTimeOutMs()
{
   return m_timeOutMs; 
}

void RegressionTest::setTimeOut(unsigned int timeOutMs)
{
   m_timeOutMs = timeOutMs; 
}

void RegressionTest::setArg(const std::string& arg)
{
   m_arg = arg;
   setArgImpl(arg);
}

const std::string& RegressionTest::getArg()
{
   return m_arg; 
}


