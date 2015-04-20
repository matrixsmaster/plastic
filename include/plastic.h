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

#ifndef BUILDNO
#define BUILDNO 1
#endif

#define VERMAJOR 0
#define VERMINOR 0

#define PRODNAME "Plastic Inquisitor"

#define HELLOSTR  "%s ver.%d.%d build %d\n \
Copyright (C) 2015, The Plastic Team\n \
%s comes with ABSOLUTELY NO WARRANTY.\n \
This is free software, and you are welcome to redistribute it \
under conditions of GNU GPL v2\n"


struct SGameSettings {
	//TODO
	bool use_shell;
};

#define DEFAULT_SETTINGS { \
	true \
}

#endif /* PLASTIC_H_ */
