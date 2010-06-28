/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "config.h"
#include "DebugDrawing.h"
#include "MC2Point.h"
#include <vector>
#include "MapPlotter.h"
#include "PixelBox.h"
#include "ExternalRenderer.h"


namespace DebugDrawing {

void pixelBox(const PixelBox& box, isab::MapPlotter* plotter)
{
   std::vector<MC2Point> points;

   points.push_back(box.getTopLeft());
   points.push_back(box.getTopRight());
   points.push_back(box.getBottomRight());
   points.push_back(box.getBottomLeft());
   points.push_back(box.getTopLeft());

   plotter->setPenColor(255, 0, 0);
   plotter->setLineWidth(3);
   
   plotter->drawPolyLine(points.begin(), points.end());
}

void rectangle(const isab::Rectangle& rect, isab::MapPlotter* plotter)
{
   std::vector<MC2Point> points;


   MC2Point topLeft(rect.getX(), rect.getY());
   MC2Point topRight = topLeft + MC2Point(rect.getWidth(), 0);
   MC2Point bottomLeft = topLeft + MC2Point(0, rect.getHeight());
   MC2Point bottomRight = topRight + MC2Point(0, rect.getHeight());

   points.push_back(topLeft);
   points.push_back(topRight);
   points.push_back(bottomRight);
   points.push_back(bottomLeft);
   points.push_back(topLeft);
   
   plotter->setPenColor(255, 0, 0);
   plotter->setLineWidth(3);
   
   plotter->drawPolyLine(points.begin(), points.end());
}


void point(const MC2Point& p, isab::MapPlotter* plotter) {
   std::vector<MC2Point> points;

   points.push_back(p + MC2Point(-1, -1));
   points.push_back(p + MC2Point( 1, -1));
   points.push_back(p + MC2Point( 1,  1));
   points.push_back(p + MC2Point(-1,  1));
   points.push_back(p + MC2Point(-1,  -1));

   plotter->setLineWidth(1);
   plotter->setPenColor(0, 255, 0);
   
   plotter->drawPolyLine(points.begin(), points.end());
}

void point(const MC2Point& p, ExternalRenderer* renderer)
{
   PixelBox pBox(p + MC2Point(-1, -1),
                 p + MC2Point(1, 1));

   renderer->drawPixelBox(pBox, 0.0f, 1.0f, 0.0f,
                          MC2Point(0, 0), 1.0f, 0.0f);
}

}

