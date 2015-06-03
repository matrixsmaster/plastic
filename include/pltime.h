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

#ifndef PLTIME_H_
#define PLTIME_H_

#include "misconsts.h"

///Conversion value (in this case, from ns to ms).
#define PLTIMEUS 1000000

///Conversion from sec to ms. Just to get rid of tons of zeros in code.
#define PLTIMEMS 1000

///Minimal clock resolution acceptable (ns).
///Set to 1 ms.
#define PLTIMEMINRES PLTIMEUS

///Time scale between game time and real time.
///Game sec = Real sec * scale
#define PLTIMESCALE 30

///Length of game day (in hours).
#define PLTIMEDAYLEN 10

///Number of days in a game week.
#define PLTIMENUMDAYS 4

///Number of weeks in a game month.
#define PLTIMENUMWEEKS 4

///Number of game months in a game year.
#define PLTIMENUMMONTHS 8

///Initial year (begin of an epoch).
#define PLTIMEINITYEAR 102

///Maximum length of the name of day.
#define PLTIMEDAYNAMECHARS 7

///Days of game week.
enum EPlDayOfWeek {
	DOW_ALPHA = 0,
	DOW_BETA,
	DOW_GAMMA,
	DOW_DELTA
};

///Structure for EPlDayOfWeek <-> string conversion.
struct SPlTimeDayNaming {
	EPlDayOfWeek d;
	char s[PLTIMEDAYNAMECHARS];
};

///EPlDayOfWeek <-> string conversion table.
static const SPlTimeDayNaming day_to_string[PLTIMENUMDAYS] = {
		{ DOW_ALPHA,	"Appa" },
		{ DOW_BETA,		"Bala" },
		{ DOW_GAMMA,	"Goty" },
		{ DOW_DELTA,	"Dana" },
};

///Main game time data structure.
struct PlasticTime {
	int hr;				//Hours
	int mn;				//Minutes
	int sc;				//Seconds
	volatile int rms;	//Real-world milliseconds
	volatile uli fr;	//Frames
	int day;			//Day
	EPlDayOfWeek dow;	//Day of week
	int month;			//Month
	int year;			//Year
};

#endif /* PLTIME_H_ */
