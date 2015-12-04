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
#include "main.h"
#include "social.h"

SCell g_line[WLENGTH];
SBuilding g_estate[WLENGTH];
SActor g_actors[WLENGTH];
int g_actnum;
PRNGen* g_rng;
PropertyMap g_pmap;
std::ostringstream g_log;
static WINDOW *scrn;
static int g_x,g_s,g_y;
static unsigned g_qn;

using namespace std;

/* ********************************************************************************** */

static void create_misc(SDynObj* p)
{
	if (!p) return;
	p->type = (g_rng->FloatNum() < 0.1)? DO_BODYPRT:DO_MISC;
	p->cond = g_rng->RangedNumber(100);
	p->bvalue = g_rng->RangedNumber((p->type == DO_MISC)? MAXMISCVALUE:MAXBPVALUE);
}

static void print_build(SBuilding* b)
{
	if (!b) return;
	g_log << "Building " << b->ID << ": " << ((b->is_plant)? "Plant.":"Generic.");
	g_log << " " << b->len << " \"floors\". Cond.: " << b->cond << endl;
}

static void print_obj(SDynObj* o)
{
	//TODO
}

static void print_actor(SActor* a)
{
	if (!a) return;
	g_log << "Actor " << a->ID << ": " << paclass_tab[a->cls].s << " ";
	g_log << ((a->female)? "female":"male") << endl;
	g_log << "HP:\t" << a->base.HP << "\t/\t" << a->curr.HP << endl;
	g_log << "Qual:\t" << a->base.Qual << "\t/\t" << a->curr.Qual << endl;
	g_log << "CC:\t" << a->base.CC << "\t/\t" << a->curr.CC << endl;
	g_log << "Spd:\t" << a->base.Spd << "\t/\t" << a->curr.Spd << endl;
	g_log << "Chr:\t" << a->base.Chr << "\t/\t" << a->curr.Chr << endl;
	g_log << "AP:\t" << a->base.AP << "\t/\t" << a->curr.AP << endl;
	g_log << "DT:\t" << a->base.DT << "\t/\t" << a->curr.DT << endl;
	g_log << "Eng:\t" << a->base.MR.Eng << "\t/\t" << a->curr.MR.Eng << endl;
	g_log << "Spch:\t" << a->base.MR.Spch << "\t/\t" << a->curr.MR.Spch << endl;
	g_log << "Brv:\t" << a->base.MR.Brv << "\t/\t" << a->curr.MR.Brv << endl;
	g_log << "Trd:\t" << a->base.MR.Trd << "\t/\t" << a->curr.MR.Trd << endl;
	g_log << "Sor:\t" << a->base.MR.Sor << "\t/\t" << a->curr.MR.Sor << endl;
	g_log << "Mot:\t";
	for (int i = 0; i < NUMMOTIVES; i++)
		g_log << a->curr.MR.Mtv[i] << ", ";
	g_log << endl;
}

static void init_inv(vector<SDynObj>* p)
{
	SDynObj o;
	int i,tot = g_rng->RangedNumber(MAXINVOBJS) + 1;

	if (!p) return;
	for (i = 0; i < tot; i++) {
		create_misc(&o);
		p->push_back(o);
	}
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
			g_actors[n].ID = n;
			g_actors[n].cls = (EPAClass)i;
			g_actors[n].base = paclass_tab[i].atr;
			g_actors[n].curr = g_actors[n].base;
			g_actors[n].female = (g_rng->FloatNum() < 0.6);
			if (g_actors[n].female) f++; //count females
			//init inventory
			g_actors[n].inv = new vector<SDynObj>;
			init_inv(g_actors[n].inv);
			//reset motives
			memset(g_actors[n].curr.MR.Mtv,0,sizeof(g_actors[n].curr.MR.Mtv));

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
		//pick a size and a place
		i = g_rng->RangedNumber(BMAXSIZ);
		if (i < BMINSIZ) i = BMINSIZ;
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
}
/*
static void drop_object(SDynObj o, int pnt)
{
	int i;

	if ((pnt < 0) || (pnt >= WLENGTH)) {
		g_log << "Dropping object into void (given " << pnt << " coord)" << endl;
		return;
	}

	for (i = 0; i < WLENGTH/2; i++) {
		if ((pnt + i < WLENGTH) && (g_line[pnt+i].obj.type == DO_INVALID)) {
			g_log << "Dropping object @ " << pnt << " to " << pnt+i << endl;
			g_line[pnt+i].obj = o;
			return;
		}
		if ((pnt - i >= 0) && (g_line[pnt-i].obj.type == DO_INVALID)) {
			g_log << "Dropping object @ " << pnt << " to " << pnt-i << endl;
			g_line[pnt-i].obj = o;
			return;
		}
	}

	g_log << "Unable to drop object from " << pnt << " (no space available)" << endl;
}

static void death(int a)
{
	int i;
	SDynObj c;

	for (i = 0; i < WLENGTH; i++)
		if (g_line[i].npc == &(g_actors))

	g_log << "Dead: " << g_actors[a].ID << endl;

	c.type = DO_BODY;
	c.cond = g_rng->RangedNumber(10);
	c.bvalue = g_actors[a].curr.Qual * MAXBODYVALUE / 100;
	if (c.bvalue < MINBODYVALUE) c.bvalue = MINBODYVALUE;

	drop_object(c,)
}

static void fight(int a, int b)
{
	//TODO
}
*/

static void dump_info()
{
	if (g_line[g_s].bld) print_build(g_line[g_s].bld);
	if (g_line[g_s].obj.type > DO_INVALID)
		print_obj(&(g_line[g_s].obj));
	if (g_line[g_s].npc) print_actor(g_line[g_s].npc);
}

static void quantum()
{
	int i;

	g_qn++;

	//buildings decay
	if (g_qn % BUILDDECAYRATE == 0) {
		for (i = 0; i < WLENGTH; i++)
			if (g_estate[i].cond) g_estate[i].cond--;
	}
}

static void update()
{
	int i,j,k,w,h;
	char buf[NLINES][MAXWIDTH+1];
	string buf2; //'cause I'm lazy bastard

	wclear(scrn);
	box(scrn,0,0);
	wrefresh(scrn);
	getmaxyx(scrn,h,w);
	w -= 2; //border
	h -= 2;
	if (w > MAXWIDTH) w = MAXWIDTH;

	//current co-ordinate: top left corner
	snprintf(buf[0],MAXWIDTH,"%d",g_x);
	mvwaddstr(scrn,0,0,buf[0]);

	//current quantum number: bottom right corner
	snprintf(buf[0],MAXWIDTH,"%u",g_qn);
	mvwaddstr(scrn,h+1,w-strlen(buf[0]+2),buf[0]);

	//cells: right at the bottom line
	memset(buf,0,sizeof(buf));
	for (i = g_x, j = 0; j < w; i++,j++) {
		for (k = 0; k < NLINES; k++)
			buf[k][j] = ' ';

		if ((i < 0) || (i >= WLENGTH)) continue;

		if (g_line[i].bld) {
			buf[0][j] = '#';
			//TODO
		} else
			buf[0][j] = '_';

		switch (g_line[i].obj.type) {
		case DO_REACTOR:
			buf[1][j] = 'R';
			break;

		case DO_CRADLE:
			buf[1][j] = '@';
			break;

		case DO_MISC:
			buf[1][j] = '$';
			break;

		case DO_BODY:
			buf[1][j] = '%';
			break;

		case DO_BODYPRT:
			buf[1][j] = '`';
			break;

		default:
			break;
		}

		if (g_line[i].npc) {
			buf[2][j] = (g_line[i].npc->female)? 'F':'M';
			//TODO
		}
	}

	//print out cells data
	for (k = 0; k < NLINES; k++)
		mvwaddnstr(scrn,h-k,1,buf[k],w);

	//print log
	k = h - NLINES;
	for (j = 0, i = g_log.str().length()-1; (i >= 0) && (k > 0); i--) {
		if (g_log.str().at(i) == '\n') {
			if (j++ < g_y) {
				buf2.clear();
				continue;
			}
			mvwaddnstr(scrn,k--,1,buf2.c_str(),w);
			buf2.clear();
		} else
			/* If you see this, please apply holy water to your eyes
			 * IMMEDIATELY, to unsee this. Otherwise you'll get an eye
			 * cancer! You've been warned! */
			buf2.insert(buf2.begin(),1,g_log.str().at(i));
	}

	//place cursor
	i = w / 2;
	g_s = i + g_x - 1;
	if (g_s < 0) g_s = 0;
	if (g_s >= WLENGTH) g_s = WLENGTH-1;
	move(h+1,i);
}

int main(int argc, char* argv[])
{
	int i,key;
	float f;

	//try to get ncurses screen beforehand
	scrn = initscr();
	if (!scrn) abort();
	g_log << endl; //starting line

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
	g_x = 0;
	g_s = 0;
	g_y = 1;
	g_qn = 0;
	for (key = 0; key != ERR;) {
		i = halfdelay(1);
		if (i != ERR) key = getch();
		if (key == ERR) key = 0;

		switch (key) {
		case 'q': key = ERR; break;
		case KEY_LEFT: g_x--; break;
		case KEY_RIGHT: g_x++; break;
		case KEY_DOWN: g_y = (g_y > 1)? g_y-1:g_y; break;
		case KEY_UP: g_y++; break;
		case KEY_HOME: g_x = 0; break;
		case KEY_END: g_x = WLENGTH-1; break;
		case 'i': dump_info(); break;
		}

		quantum();
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
