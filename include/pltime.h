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

enum EPlDayOfWeek {
	DOW_ALPHA = 0,
	DOW_BETA,
	DOW_GAMMA,
	DOW_DELTA
};

struct PlasticTime {
	int hr;				//Hours
	int mn;				//Minutes
	int sc;				//Seconds
	int fr;				//Frames
	int day;			//Day
	EPlDayOfWeek dow;	//Day of week
	int month;			//Month
	int year;			//Year
};

#endif /* PLTIME_H_ */
