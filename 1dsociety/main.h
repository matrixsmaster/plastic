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

#ifndef MAIN_H_
#define MAIN_H_

#define WLENGTH 2000
#define SIZEPRC 0.5f
#define BVOLUME 0.3f
#define BMAXSIZ 40
#define BMINSIZ 2
#define MAXWIDTH 2048

#define MAXMISCVALUE 4000
#define MAXBPVALUE 4000
#define MINBODYVALUE 500
#define MAXBODYVALUE 7000
#define MAXINVOBJS 100

#define BUILDDECAYRATE 100

#define NLINES 3

extern SCell g_line[WLENGTH];
extern SBuilding g_estate[WLENGTH];
extern SActor g_actors[WLENGTH];
extern int g_actnum;
extern PRNGen* g_rng;
extern PropertyMap g_pmap;
extern std::ostringstream g_log;

#endif /* MAIN_H_ */
