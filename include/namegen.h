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

#ifndef NAMEGEN_H_
#define NAMEGEN_H_

#include <string>
#include "datapipe.h"

#define NMGFEMALEDICT "names_f"
#define NMGMALEDICT "names_m"
#define NMGVOWELS "vowels"
#define NMGCONSON "consonants"
#define NMGMAXSYLL 3
#define NMGPSYLNUMCH 0.4
#define NMGPSYLSTCON 0.25

class NameGen {
private:
	DataPipe* pipe;
	PRNGen* rng;

public:
	NameGen(DataPipe* pipeptr);
	virtual ~NameGen()				{}

	std::string Syllable();

	std::string GetHumanName(bool female);
};

#endif /* NAMEGEN_H_ */
