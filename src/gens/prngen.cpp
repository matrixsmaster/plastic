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

#include <time.h>
#include "prngen.h"


PRNGen::PRNGen(bool time_seed)
{
	if (time_seed) TimeSeed();
}

void PRNGen::SetSeed(long new_seed)
{
	m_seed = new_seed ^ PRNG_X;
}

long PRNGen::TimeSeed()
{
	time_t t;
	time(&t);
	long tt = (long)t;
	SetSeed(tt);
	return tt;
}

int PRNGen::NextNumber()
{
	m_seed = ( PRNG_A * m_seed + PRNG_C ) % PRNG_M;
	return m_seed;
}

int PRNGen::RangedNumber(int Max)
{
	float a;
	int p = 1;
	int tmp = Max;
	if (Max < 1) return 0;
	do {
		tmp /= 10;
		p *= 10;
	} while (tmp >= 1.0);
	do {
		a = FloatNum();
		a *= p;
		tmp = static_cast<int> (a);
	} while (tmp >= Max);
	return tmp;
}

float PRNGen::FloatNum()
{
	int v = NextNumber();
	v &= 0x7FFFFFF;
	return float(v) / float(0x8000000);
}
