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

#ifndef KEYBINDER_H_
#define KEYBINDER_H_

#include <string>
#include <map>
#include "datapipe.h"


class KeyBinder {
private:
	DataPipe* pipe;
	std::map<int,int> keymap;

public:
	KeyBinder(DataPipe* pptr)		{ pipe = pptr; }
	virtual ~KeyBinder()			{ keymap.clear(); }

	///Returns a code associated with key.
	int RegKeyByName(const char* name);

	///Returns code of registered key (if found).
	int DecodeKey(int key);
};

#endif /* KEYBINDER_H_ */
