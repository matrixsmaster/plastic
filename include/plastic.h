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


#define GAMEARGTYPES 7
#define GAMEARGHELPLEN 360

struct SGameSettings {
	char root[MAXPATHLEN];	//VFS root dir
	bool use_shell;			//launch interactive shell
	bool new_game;			//discard old data
	uli world_r;			//world radius
	ulli rammax;			//amount of memory that positively allowed to be used
	long wg_seed;			//seed for world map generation (zero for random seed)
	long u_seed;			//seed of the universe (zero for random seed)
	SPAAttrib PCData;		//player character settings
};

#define DEFAULT_SETTINGS { 							\
	"./data",										\
	false, 											\
	true,											\
	32,												\
	(4ULL*1024*1024*1024),							\
	110864270,										\
	110864270,										\
	{ "Mary", true, PCLS_INQUISITOR, PBOD_PNEUMO, },\
}
//24141595

enum EGameArgType {
	GAT_NOTHING,
	GAT_ROOTDIR,
	GAT_USESHELL,
	GAT_NEWGAME,
	GAT_WORLDRAD,
	GAT_RAMMAX,
	GAT_SEED,
	GAT_PLAYER
};

struct SGameArg {
	EGameArgType typ;
	char sw;					//switch character
	bool edit;					//is this setting is editable in interactive shell
	char desc[GAMEARGHELPLEN];	//description string
};

static const SGameArg argp_table[GAMEARGTYPES] = {
	{ GAT_ROOTDIR,	'r', true,	"Set the root directory of game files." },
	{ GAT_USESHELL,	's', false,	"Show interactive shell after startup to "\
								"adjust game settings and/or generate "\
								"player character." },
	{ GAT_NEWGAME,	'N', true,	"Start new game (discard old world)." },
	{ GAT_WORLDRAD,	'R', true,	"Set the radius of the world hypertorus." },
	{ GAT_RAMMAX,	'm', true,	"Set the memory threshold for the main "\
								"data holding facilities. This amount is "\
								"only the rough approximation. "\
								"Real memory consumption will be higher, "\
								"but not too much." },
	{ GAT_SEED,		'S', true,	"Set the seed value used to generate the world. "\
								"Should be zero to generate random seed." },
	{ GAT_PLAYER,	'P', false,	"String of data used to create player "\
								"character. See details in documentation." },
};

#endif /* PLASTIC_H_ */
