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

/* Some not module-specific or widely used constants / defines / macros goes here */

#ifndef MISCONSTS_H_
#define MISCONSTS_H_

#define MAXPATHLEN 512

#define DEFFARPLANE 64

/*
 * I don't want to use <inttypes.h> because it causes all kinds of problems
 * on different platforms. Or even with different versions of glibc inclusion files.
 */
typedef unsigned long long int ulli; //for short
typedef unsigned long int uli;


#endif /* MISCONSTS_H_ */
