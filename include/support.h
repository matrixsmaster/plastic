/*
 * support.h
 *
 *  Created on: Apr 19, 2015
 *      Author: matrixsmaster
 */

#ifndef SUPPORT_H_
#define SUPPORT_H_

#include <stdarg.h>
#include "plastic.h"

/// Prints out SGameSettings data.
void printsettings(SGameSettings* s);

/// Prints a simple formatted message to stderr.
void errout(char const* fmt, ...);

/// Argument parser. Supposed to be used for analyze startup environment.
bool argparser(int argc, char* argv[]);


#endif /* SUPPORT_H_ */
