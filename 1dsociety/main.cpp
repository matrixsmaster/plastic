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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <iostream>
#include <sstream>
#include <ncurses.h>
#include "prngen.h"
#include "cell.h"
#include "prop.h"

#define WLENGTH 2000
#define SIZEPRC 0.5f
#define BVOLUME 0.3f
#define BMAXSIZ 40

SCell g_line[WLENGTH];
SBuilding g_estate[WLENGTH];
SActor g_actors[WLENGTH];
int g_actnum;
PRNGen* g_rng;
PropertyMap g_pmap;
static WINDOW *scrn;
static int g_x;

using namespace std;

ostringstream g_log;

static void init_inv(vector<SDynObj>* p)
{
	//TODO
}

static void init_pop()
{
	int i,n,l,j,f,x = 0;
	int cap = ceil((float)WLENGTH * SIZEPRC);
	g_log << "Maximum capacity = " << cap << endl;

	for (i = 0, n = 0; i < NUMCLASSES; i++) {
		l = g_rng->RangedNumber(floor((float)cap * paclass_tab[i].pop));
		g_log << "Class " << paclass_tab[i].s << " pop. " << l;

		for (j = 0, f = 0; j < l; j++,n++) {
			//make actor
			g_actors[n].ID = n + 100;
			g_actors[n].cls = (EPAClass)i;
			g_actors[n].base = paclass_tab[i].atr;
			g_actors[n].curr = g_actors[n].base;
			g_actors[n].female = (g_rng->FloatNum() < 0.6);
			if (g_actors[n].female) f++;
			g_actors[n].inv = new vector<SDynObj>;
			init_inv(g_actors[n].inv);

			//place actor
			while (g_line[x].npc) x = g_rng->RangedNumber(WLENGTH);
			g_line[x].npc = &(g_actors[n]);
		}
		g_log << ", " << f << " women" << endl;
	}
	g_log << "Total: " << n << " out of " << cap << endl;
	g_actnum = n;
}

static void init_build()
{
	int i,j,x = 0,m,n = 0,k = 0;
	unsigned ow;
	SProperty pd;

	m = ceil((float)WLENGTH * BVOLUME);

	//build
	while (n < m) {
		//pick a place
		i = g_rng->RangedNumber(BMAXSIZ)+1;
		while (g_line[x].bld) x = g_rng->RangedNumber(WLENGTH);
		for (j = 0; j < i; j++)
			if ((j+x >= WLENGTH) || (g_line[j+x].bld)) {
				j = -1;
				break;
			}
		if (j < 0) continue;

		//fill info
		g_estate[k].ID = k + 1000;
		g_estate[k].len = i;
		g_estate[k].start = x;
		g_estate[k].cond = 100;
		g_estate[k].is_plant = false;
		memset(&(g_estate[k].info),0,sizeof(g_estate[k].info));

		//select the owner
		ow = g_rng->RangedNumber(g_actnum);
		while (g_actors[ow].cls != PCLS_NOBLE)
			ow = g_rng->RangedNumber(g_actnum);
		ow = g_actors[ow].ID;

		//assign to it
		pd.t = PRP_TERRITORY; //owns a land too
		pd.d.ter.terr.len = i;
		pd.d.ter.terr.start = x;
		pd.d.ter.val = i * 10; //TODO
		g_pmap.insert(make_pair(ow,pd));
		pd.t = PRP_BUILDING; //and now the building itself
		pd.d.bld = &(g_estate[k]);
		g_pmap.insert(make_pair(ow,pd));

		//print info
		g_log << "Building: " << g_estate[k].ID << " @ " << g_estate[k].start;
		g_log << " : " << g_estate[k].len << " assigned to " << ow << endl;

		//build
		for (j = 0; j < i; j++) {
			g_line[x+j].bld = &(g_estate[k]);
			//don't forget about crucial objects
			if (j <= (i/2)) {
				g_line[x+j].obj.type = (j)? DO_CRADLE:DO_REACTOR;
				g_line[x+j].obj.cond = 100;
				g_line[x+j].obj.bvalue = 1000;
			}
		}

		//move to next
		n += i;
		k++;
	}

	//assign plants data to first buildings
	for (i = 0; i < PRODLINEL; i++) {
		g_estate[i].is_plant = true;
		g_estate[i].info = prod_line[i];
		g_log << "Building: " << g_estate[i].ID << " converted to a plant" << endl;
	}
}

static void init_objs()
{
	int i,j;

	for (i = 0; i < WLENGTH; i++) {
		//
	}
}

static void update()
{
	box(scrn,0,0);
	wrefresh(scrn);
}

int main(int argc, char* argv[])
{
	int i,key;
	float f;

	//try to get ncurses screen beforehand
	scrn = initscr();
	if (!scrn) abort();

	i = (argc > 1)? atoi(argv[1]):0;
	g_rng = new PRNGen(true);
	if (i) g_rng->SetSeed(i);
	g_log << "Seed = " << g_rng->GetSeed() << endl;

	//check table
	for (i = 0, f = 0; paclass_tab[i].c != PCLS_NONE; i++)
		f += paclass_tab[i].pop;
	if ((f - 1e-5 > 1.f) || (f + 1e-5 < 1.f)) {
		cerr << "Incorrect population koeffs: " << f << endl;
		abort();
	}

	//init world
	memset(g_line,0,sizeof(SCell)*WLENGTH);
	memset(g_estate,0,sizeof(SBuilding)*WLENGTH);
	memset(g_actors,0,sizeof(SActor)*WLENGTH);

	//fill world
	init_pop();
	init_build();
	init_objs();

	//init ui
	noecho();
	cbreak();
	keypad(scrn,TRUE);

	//main loop
	for (g_x = 0, key = 0; key != ERR;) {
		i = halfdelay(1);
		if (i != ERR) key = getch();
		if (key == ERR) key = 0;

		switch (key) {
		case 'q': key = ERR; break;
		case KEY_LEFT: g_x--; break;
		case KEY_RIGHT: g_x++; break;
		}

		update();
	}

	//destroy
	delwin(scrn);
	endwin();
	refresh();
	for (i = 0; i < WLENGTH; i++)
		if (g_actors[i].inv) delete (g_actors[i].inv);
	delete g_rng;
	cout << g_log.str() << endl;
	return 0;
}
