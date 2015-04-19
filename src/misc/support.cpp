/*
 * support.cpp
 *
 *  Created on: Apr 19, 2015
 *      Author: matrixsmaster
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "support.h"


void printsettings(SGameSettings* s)
{
	//TODO
}

void errout(char const* fmt, ...)
{
	va_list vl;
	va_start(vl,fmt);
	vfprintf(stderr,fmt,vl);
	va_end(vl);
}

bool argparser(int argc, char* argv[])
{
	//TODO
	return true;
}
