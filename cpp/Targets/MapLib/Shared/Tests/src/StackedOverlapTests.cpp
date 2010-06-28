/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "TestFramework.h"
#include "StackedOverlapTests.h"
#include "MC2BoundingBox.h"
#include "RefCounted.h"
#include <vector>
#include <iostream>
#include "Utility.h"
#include "Layer.h"
#include "OverlayItemInternal.h"
#include "StackedSet.h"

void StackedOverlapTests::test1( TestReporter& reporter )
{
   // NEW_TEST_FUNCTION(reporter);
   // OverlayItemInternal* item1 = new OverlayItemInternal;
   // item1->setBoundingBox(
   //    MC2BoundingBox(MC2Coordinate(0, 0),
   //                   MC2Coordinate(100, 100)));

   // // Should not be overlapping
   // OverlayItemInternal* item2 = new OverlayItemInternal;
   // item2->setBoundingBox(
   //    MC2BoundingBox(MC2Coordinate(101, 0),
   //                   MC2Coordinate(200, 100)));

   // // Should overlap both previous, and should be stacked on item1.
   // OverlayItemInternal* item3 = new OverlayItemInternal;
   // item3->setBoundingBox(
   //    MC2BoundingBox(MC2Coordinate(50, 0),
   //                   MC2Coordinate(150, 100)));


   // // Should be stacked on item2.
   // OverlayItemInternal* item4 = new OverlayItemInternal;
   // item4->setBoundingBox(
   //    MC2BoundingBox(MC2Coordinate(150, 0),
   //                   MC2Coordinate(250, 100)));

   // // Should be stacked on item1.
   // OverlayItemInternal* item5 = new OverlayItemInternal;
   // item5->setBoundingBox(
   //    MC2BoundingBox(MC2Coordinate(-50, 0),
   //                   MC2Coordinate(50, 100)));



   // StackedSet holder;
   // holder.stack(item1);
   // holder.stack(item2);
   // holder.stack(item3);
   // holder.stack(item4);
   // holder.stack(item5);
   // std::vector<OverlayItemInternal*> stacks = holder.getStacks();
   
   // CHECK(stacks[0] == item1,
   //       reporter);
   // CHECK(stacks[0]->getStackTop() == item5,
   //       reporter);
   // CHECK(stacks[0]->getStackTop()->getStackTop() == item3,
   //       reporter);
   // CHECK(stacks[1] == item2,
   //       reporter);
   // CHECK(stacks[1]->getStackTop() == item4,
   //       reporter);

   // CHECK(item1->getStackSize() == 3,
   //       reporter);
   // CHECK(item2->getStackSize() == 2,
   //       reporter);

}
