/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "WinCEBitmapPainter.h"

#include "MC2Point.h"

SimpleBitmap::SimpleBitmap ()
{
	m_width = 0;
	m_height = 0;
	m_data = NULL;
}

SimpleBitmap::~SimpleBitmap ()
{
	delete [] m_data;
}

bool SimpleBitmap::allocate (int width, int height)
{
	if (width < 0 || height < 0)
		return false;

	m_width = width;
	m_height = height;

	unsigned int size = m_width * m_height * 4;
	m_data = new unsigned char[size];

	if (!m_data)
		return false;

	return true;
}

int SimpleBitmap::getWidth () const
{
	return m_width;
}

int SimpleBitmap::getHeight () const
{
	return m_height;
}

unsigned char* SimpleBitmap::getData () const
{
	return m_data;
}

class WinCEBitmapEntry
{
public:
	WinCEBitmapEntry() : m_position( 0, 0 )
	{
		m_bitmap = 0;
		m_next = 0;
	}
	~WinCEBitmapEntry()
	{
		// If the ownership of SimpleBitmap is tranfered
		// to this class when adding the bitmap,
		// then here should be delete m_bitmap
	}

	SimpleBitmap* m_bitmap;
	MC2Point m_position;
	WinCEBitmapEntry* m_next;
};

WinCEBitmapPainter::WinCEBitmapPainter()
{
}

WinCEBitmapPainter::~WinCEBitmapPainter()
{
	WinCEBitmapEntry* current = m_firstBitmap;
	while (current)
	{
		WinCEBitmapEntry* next = m_firstBitmap->m_next;
		delete current;
		current = next;
	}
}

bool WinCEBitmapPainter::DrawBitmap( SimpleBitmap* bitmap, const MC2Point& pos,
									const MC2Point& topLeft, const MC2Point& bottomRight,
									int bufferWidth, void* buffer ) const
{
	if (!bitmap)
		return false;

	int bmWidth = bitmap->getWidth();
	int bmHeight = bitmap->getHeight();

	int scrLeft = topLeft.getX() > pos.getX() ? topLeft.getX() : pos.getX();
	int scrTop = topLeft.getY() > pos.getY() ? topLeft.getY() : pos.getY();
	int scrRight = bottomRight.getX() < pos.getX() + bmWidth ? bottomRight.getX() : pos.getX() + bmWidth;
	int scrBottom = bottomRight.getY() < pos.getY() + bmHeight ? bottomRight.getY() : pos.getY() + bmHeight;

	int picLeftClip = scrLeft - pos.getX();
	int picTopClip = scrTop - pos.getY();
	int picRightClip = (pos.getX() + bmWidth) - scrRight;
	int picBottomClip = (pos.getY() + bmHeight) - scrBottom;

	if (bmWidth < picLeftClip + picRightClip ||
		bmHeight < picTopClip + picBottomClip)
		return true;

	unsigned short* tgt = (unsigned short*)buffer;
	unsigned char* src = (unsigned char*)bitmap->getData();

	if (!tgt || !src)
		return false;

	// Assumes that the target buffer is 16-bit RGB565 and the bitmap is 32-bit RGBA8888

	tgt += scrTop * bufferWidth;
	src += picTopClip * bmWidth * 4;

	int y;
	for (y = scrTop; y < scrBottom; y++)
	{
		src += picLeftClip * 4;
		tgt += scrLeft;

		int x;
		for (x = scrLeft; x < scrRight; x++)
		{
			unsigned int t = src[3];
			if (t == 0)
			{
				tgt++;
				src += 4;
				continue;
			}

			unsigned int rb;
			unsigned int g;

			if (t == 255)
			{
				rb = (((unsigned int)src[0]) << 16) | ((unsigned int)src[2]);
				g = ((unsigned int)src[1]) << 8;
			}
			else
			{
				unsigned int irb = (((unsigned int)src[0]) << 16) | ((unsigned int)src[2]);
				unsigned int ig = ((unsigned int)src[1]) << 8;

				unsigned int brb = ((((unsigned int)*tgt) & 0xf800) << 8) | ((((unsigned int)*tgt) << 3) & 0xff);
				unsigned int bg = (((unsigned int)*tgt) & 0x7e0) << 5;

				rb = (((brb * (256 - t)) >> 8) & 0xff00ff) + (((irb * t) >> 8) & 0xff00ff);
				g = (((bg * (256 - t)) >> 8) & 0xff00) + (((ig * t) >> 8) & 0xff00);
			}

			*tgt = (unsigned short)((((rb & 0xf80000) >> 8) | ((g >> 5) & 0x7e0) | ((rb & 0xff) >> 3)));
			
			tgt++;
			src += 4;
		}

		src += picRightClip * 4;
		tgt += bufferWidth - scrRight;
	}

	return true;
}

void WinCEBitmapPainter::DrawBitmaps( const MC2Point& topLeft, const MC2Point& bottomRight,
									 int bufferWidth, void* buffer ) const
{
	WinCEBitmapEntry* current = m_firstBitmap;
	while (current)
	{
		DrawBitmap( current->m_bitmap, current->m_position, topLeft, bottomRight, bufferWidth, buffer );
		current = current->m_next;
	}
}

void WinCEBitmapPainter::addBitmap( SimpleBitmap* bitmap, const MC2Point& pos )
{
	WinCEBitmapEntry* entry = new WinCEBitmapEntry();
	if (entry)
	{
		entry->m_bitmap = bitmap;
		entry->m_position = pos;
		entry->m_next = m_firstBitmap;
		m_firstBitmap = entry;
	}
}

void WinCEBitmapPainter::removeBitmap( SimpleBitmap* bitmap )
{
	WinCEBitmapEntry* last = 0;
	WinCEBitmapEntry* current = m_firstBitmap;

	while (current && current->m_bitmap != bitmap)
	{
		last = current;
		current = current->m_next;
	}

	if (last)
		last->m_next = current->m_next;
	else
		m_firstBitmap = current->m_next;

	delete current;
}

void WinCEBitmapPainter::moveBitmap( SimpleBitmap* bitmap, const MC2Point& pos )
{
	WinCEBitmapEntry* current = m_firstBitmap;
	
	while (current && current->m_bitmap != bitmap)
		current = current->m_next;

	if (current)
		current->m_position = pos;
}
