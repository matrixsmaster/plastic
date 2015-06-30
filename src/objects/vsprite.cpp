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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vsprite.h"


VSprite::VSprite()
{
	img = NULL;
	g_w = g_h = 0;
	allocram = 0;
}

VSprite::~VSprite()
{
	if (img) free(img);
}

bool VSprite::Assign(SGUIPixel* pict, int w, int h)
{
	if ((w < 1) || (h < 1)) return false;

	allocram = w * h * sizeof(SGUIPixel);
	img = (SGUIPixel*)realloc(img,allocram);
	if (!img) return false;

	g_w = w;
	g_h = h;
	memcpy(img,pict,allocram);

	return true;
}

bool VSprite::LoadFromFile(const char* fn)
{
	FILE* fi;
	int i,j,k,l,r,ps,tr;
	char transp;
	SGUIPixel curp;
	char* line;

	//open a file
	fi = fopen(fn,"r");
	if (!fi) return false;

	//get dimensions
	r = fscanf(fi,"%d %d %d %d\n",&g_w,&g_h,&ps,&tr);
	if ((r != 4) || (g_w <= 0) || (g_h <= 0) || (ps <= 0)) {
		fclose(fi);
		return false;
	}

	//prepare some values
	transp = ((tr < 0) || (tr > 255))? 0:(char)tr;
	allocram = g_w * g_h * sizeof(SGUIPixel);

	//allocate memory
	img = (SGUIPixel*)malloc(allocram);
	line = (char*)malloc(g_w+2);
	if ((!img) || (!line)) {
		fclose(fi);
		if (line) free(line); //img will be freed at destruction
		return false;
	}

	//begin loading frames
	for (i = 0; ((i < ps) && (!feof(fi))); i++) {
		//read frame color data
		r = fscanf(fi,"%hd %hd %hd %hd %hd %hd",&(curp.fg.r),&(curp.fg.g),&(curp.fg.b),
				&(curp.bg.r),&(curp.bg.g),&(curp.bg.b));
		if (r != 6) {
			if (!r) fgetc(fi); //advance by one char
			i--;
			continue;
		}

		//read frame "pixels"
		for (j = 0; j < g_h; j++) {
			if ((!fgets(line,g_w+2,fi)) || (g_w > (int)strlen(line))) {
				//skip possibly empty lines
				j--;
				continue;
			}

			l = j * g_w;
			//apply pixels to the final image
			for (k = 0; k < g_w; k++,l++) {
				if (line[k] == transp) continue;
				curp.sym = line[k];
				img[l] = curp;
			}
		}
	}

	//free memory and IO
	free(line);
	fclose(fi);
	return true;
}
