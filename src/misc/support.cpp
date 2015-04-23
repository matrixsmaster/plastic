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

#include "support.h"
#include "plastic.h"


void printsettings(SGameSettings* s)
{
	printf("\n%s settings:\n",PRODNAME);
	printf("Root dir:\t\t%s\n",s->root);
	printf("Use startup shell:\t%s\n",BOOLSTR(s->use_shell));
	printf("\n");
}

void errout(char const* fmt, ...)
{
	va_list vl;
	va_start(vl,fmt);
	vfprintf(stderr,fmt,vl);
	va_end(vl);
}

bool argparser(int argc, char* argv[], SGameSettings* sets)
{
	int i,j,an, fsm = 0;
	EGameArgType curt;

	for (i = 1; i < argc; i++) {
		switch (fsm) {
		case 0: /* default */
			//detect switch
			curt = GAT_NOTHING;
			an = 0;
			if ((argv[i][0] != '-') || (strlen(argv[i]) != 2)) {
				errout("Malformed parameter switch at position %d.\n",i);
				return false;
			}
			//search for known switch character
			for (j = 0; j < GAMEARGTYPES; j++)
				if (argv[i][1] == argp_table[j].sw) {
					curt = argp_table[j].typ;
					an = argp_table[j].anum;
					break;
				}
			if (curt == GAT_NOTHING) {
				errout("Unknown switch '%c' at position %d.\n",argv[i][1],i);
				return false;
			} else fsm++;
			break;

		case 1: /* switch detected, reading first argument */
			switch (curt) {
			case GAT_ROOTDIR:
				strncpy(sets->root,argv[i],MAXPATHLEN);
				fsm = 0;
				break;
			case GAT_USESHELL:
				sets->use_shell = (atoi(argv[i]) == 1);
				fsm = 0;
				break;
			default:
				fsm = 0;
			}
			break;

		/* more than one sub-argument cases will go here */

		default: /* just in case */
			fsm = 0;
			break;
		}
	}

	return true;
}
