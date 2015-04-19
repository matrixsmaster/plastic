/*
 * visual.h
 *
 *  Created on: Apr 7, 2015
 *      Author: matrixsmaster
 */

#ifndef VISUAL_H_
#define VISUAL_H_

#include "CurseGUI.h"

#define COLPAIRS 7

static const struct SGUIWCol gui_coltable[COLPAIRS] = {
		{COLOR_WHITE,COLOR_BLACK}, //the first one is default
		{COLOR_WHITE,COLOR_BLUE},
		{COLOR_YELLOW,COLOR_GREEN},
		{COLOR_WHITE,COLOR_YELLOW},
		{COLOR_BLACK,COLOR_WHITE},
		{COLOR_WHITE,COLOR_RED},
		{COLOR_WHITE,COLOR_MAGENTA}
};

#endif /* VISUAL_H_ */
