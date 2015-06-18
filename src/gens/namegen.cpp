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

#include "namegen.h"
#include "misconsts.h"

using namespace std;

NameGen::NameGen(DataPipe* pipeptr)
{
	pipe = pipeptr;
	rng = pipe->GetRNG();
}

string NameGen::Syllable()
{
	int i,j,n;
	bool c;
	string r;
	DPDict* vow,* con,* cur;

	vow = pipe->GetDictionary(NMGVOWELS);
	con = pipe->GetDictionary(NMGCONSON);

	n = (rng->FloatNum() < NMGPSYLNUMCH)? 3:2;
	c = (rng->FloatNum() < NMGPSYLSTCON);
	for (i = 0; i < n; i++) {
		cur = (c)? con:vow;
		j = rng->RangedNumber((int)cur->size());
		r += cur->at(j);
		c ^= true;
	}

	return r;
}

string NameGen::GetHumanName(bool female)
{
	DPDict* dct;
	string r;
	int n;
	unsigned i;

	//Load first names dictionary
	dct = (female)? pipe->GetDictionary(NMGFEMALEDICT):pipe->GetDictionary(NMGMALEDICT);
	if (!dct) return r;

	//Select a random first name
	r = dct->at(rng->RangedNumber((int)dct->size()));

	//Generate true random last name
	r += ' '; //a space between first and last names
	n = rng->RangedNumber(NMGMAXSYLL) + 1; //number of syllables
	i = r.size();
	while (n--) r += Syllable();
	r[i] = toupper(r[i]); //make first character upper-cased

	return r;
}
