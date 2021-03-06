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

/* Implementation file of Picture control class */

#include "CGUIControls.h"


CurseGUIPicture::CurseGUIPicture(CurseGUICtrlHolder* p, int x, int y, int w, int h) :
		CurseGUIControl(p,x,y)
{
	typ = GUICL_PICTURE;
	pict = NULL;
	autoalloc = false;
	g_w = w;
	g_h = h;
	length = w * h * sizeof(SGUIPixel);
}

CurseGUIPicture::~CurseGUIPicture()
{
	if (autoalloc && pict) free(pict);
}

bool CurseGUIPicture::SetAutoAlloc(bool a)
{
	SCTriple fill;
	fill.r = 0; fill.g = 0; fill.b = 0;

	//free memory if auto-allocated
	if (autoalloc && pict) {
		free(pict);
		pict = NULL;
	}
	//set new flag
	autoalloc = a;
	//allocate memory
	if (a) {
		pict = (SGUIPixel*)malloc(length);
		if (!pict) {
			autoalloc = false;
			return false;
		}
		ColorFill(fill);
	}
	//All operations are completed successfully
	return true;
}

void CurseGUIPicture::SetPicture(SGUIPixel* p)
{
	if (autoalloc) {
		if (!pict) return; //fail silently
		memcpy(pict,p,length);
	} else
		pict = p;

}

void CurseGUIPicture::ColorFill(SCTriple col)
{
	int i;

	if (!pict) return;

	for (i = 0; i < (g_w*g_h); i++) {
		pict[i].bg = col;
		pict[i].fg = col;
		pict[i].sym = ' ';
	}
}

void CurseGUIPicture::Update()
{
	int i,wh,pw,ph;
	SGUIPixel* rcv = wnd->GetBackgroundData();

	if ((!pict) || (!rcv)) return;

	wh = wnd->GetWidth();
	pw = ((g_w + g_x) > wh)? (wh-g_x):g_w;
	ph = ((g_h + g_y) > wnd->GetHeight())? (wnd->GetHeight()-g_y):g_h;

	for (i = 0; i < ph; i++)
		memcpy(rcv+((i+g_y)*wh+g_x),pict+(i*g_w),pw*sizeof(SGUIPixel));
}

