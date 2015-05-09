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

#ifndef PLASTIC_H_
#define PLASTIC_H_

#include "actortypes.h"
#include "misconsts.h"


#ifndef BUILDNO
#define BUILDNO 1
#endif

#define VERMAJOR 0
#define VERMINOR 0

#define PRODNAME "Plastic Inquisitor"

#define HELLOSTR  "%s ver.%d.%d build %d\n\
Copyright (C) 2015, The Plastic Team\n\
%s comes with ABSOLUTELY NO WARRANTY.\n\
This is free software, and you are welcome to redistribute it\n\
under conditions of GNU GPL v2\n\n"


#define GAMEARGTYPES 2
#define GAMEARGHELPLEN 160

struct SGameSettings {
	char root[MAXPATHLEN];	//VFS root dir
	bool use_shell;			//launch interactive shell
	uli world_r;			//world radius
	ulli rammax;			//amount of memory that positively allowed to be used
	long wg_seed;			//seed for world map generation (zero for random seed)
	SPAStats PCData;		//player character settings
};

#define DEFAULT_SETTINGS { 						\
	"./data",									\
	false, 										\
	64,											\
	(4ULL*1024*1024*1024),						\
	0,											\
	{ "Mary", true, {PCLS_INQUISITOR, true} }	\
}

enum EGameArgType {
	GAT_NOTHING,
	GAT_ROOTDIR,
	GAT_USESHELL
};

struct SGameArg {
	EGameArgType typ;
	char sw;					//switch character
	char desc[GAMEARGHELPLEN];	//description string
};

static const SGameArg argp_table[GAMEARGTYPES] = {
	{ GAT_ROOTDIR,	'r', "Set the root directory of game files." },
	{ GAT_USESHELL,	's', "Show interactive shell after startup to "\
						 "adjust game settings and/or generate "\
						 "player character." },
};

#endif /* PLASTIC_H_ */
