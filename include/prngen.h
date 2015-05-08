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

/* Pseudo-random numbers generator */

/* Ported from dynworld (formerly MSMRLCG) */

#ifndef PRNGEN_H_
#define PRNGEN_H_

#define PRNG_A 16807
#define PRNG_C 0
#define PRNG_M 2147483647
#define PRNG_X 0x5EECE66D

class PRNGen
{
private:
	long m_seed;

public:
	PRNGen(bool time_seed);
	virtual ~PRNGen()				{}

	void SetSeed(long new_seed);
	long GetSeed()					{ return m_seed; }
	long TimeSeed();

	int NextNumber();
	int RangedNumber(int Max);
	float FloatNum();
};

#endif /* PRNGEN_H_ */
