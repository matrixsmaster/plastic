/**
 *  Plastic Inquisitor
 *  Copyright (C) 2015 The Plastic Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef SPRITE_H_
#define SPRITE_H_

#include "visual.h"

class VSprite {
private:
	int g_w, g_h, allocram;
	SGUIPixel* img;

public:
	VSprite();
	virtual ~VSprite();

	bool Assign(SGUIPixel* pict, int w, int h);
	bool LoadFromFile(const char* fn);

	SGUIPixel* GetImage()				{ return img; }
	int GetWidth()						{ return g_w; }
	int GetHeight()						{ return g_h; }
	int GetAllocatedRAM()				{ return allocram; }
};

#endif /* SPRITE_H_ */
