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
#include <ctype.h>
#include "support.h"
#include "vecmath.h"
#include "wrldgen.h"


void errout(char const* fmt, ...)
{
	va_list vl;
	va_start(vl,fmt);
	vfprintf(stderr,fmt,vl);
	va_end(vl);
}

void printsettings(SGameSettings* s)
{
	printf("\n%s settings:\n",			PRODNAME);
	printf("Root dir:\t\t%s\n",			s->root);
	printf("Use startup shell:\t%s\n",	BOOLSTR(s->use_shell));
	printf("World radius:\t\t%lu\n",	s->world_r);
	printf("World seed value:\t%ld\n",	s->wg_seed);
	printf("Max RAM amount:\t\t%llu\n",	s->rammax);

	//don't print player information
	printf("\n");
}

static void ishell_options(SGameSettings* s)
{
	int i,in;
	EGameArgType det;
	char sfmt[16];
	WorldGen* wgen;
	ulli sz;
	float cov,tb,pb;
	vector3di dim;

opts_begin:
	puts("\nGame options:");
	for (i = 0; i < GAMEARGTYPES; i++)
		if (argp_table[i].edit) {
			printf("%c) %s\n",argp_table[i].sw,argp_table[i].desc);
		}
	puts("*) Exit to main menu");

	while (!isprint(in = getchar())) ; //trash non-printable chars (NL,LF,EOF etc)
	det = GAT_NOTHING;
	for (i = 0; i < GAMEARGTYPES; i++)
		if (argp_table[i].sw == in) {
			det = argp_table[i].typ;
			break;
		}

	switch (det) {
	default: return;

	case GAT_ROOTDIR:
		printf("Current root dir = '%s'\n",s->root);
		printf("New root dir> ");
		snprintf(sfmt,sizeof(sfmt),"%%%ds\\n",MAXPATHLEN);
		scanf(sfmt,s->root);
		break;

	case GAT_WORLDRAD:
		printf("Current radius = %lu\n",s->world_r);
		printf("New radius> ");
		scanf("%lu",&(s->world_r));

		//generate world to get area values
		wgen = new WorldGen(s->world_r,NULL,0);
		dim = wgen->GetVolume();
		printf("Calculating values...\n");
		printf("World volume dimensions = [%d, %d, %d]\n",dim.X,dim.Y,dim.Z);
		printf("World surface area = %llu chunk^2\n",wgen->GetPlaneArea());

		//get RAM weight of world map
		wgen->NewMap(1);
		cov = wgen->GetAllocatedRAM() / 1024.f / 1024.f / 1024.f;
		printf("World map RAM cost = %llu bytes (%.3f GiB)\n",wgen->GetAllocatedRAM(),cov);
		//discard world
		delete wgen;

		//calculate estimate HDD consumption
		cov = (float)(sizeof(VChunk)) * (float)dim.X * (float)dim.Y * (float)dim.Z;
		cov = cov / 1024.f / 1024.f / 1024.f;
		tb = cov / 1024.f;
		pb = tb / 1024.f;
		printf("Estimated full-world HDD consumption: %.3f GiB or %.3f TiB or %.3f PiB\n",cov,tb,pb);
		printf("Please note this is worst case scenario!\n");
		break;

	case GAT_RAMMAX:
		printf("Current RAM threshold = %llu\n",s->rammax);
		sz = sizeof(VChunk) * HOLDCHUNKS;
		cov = (float)sz / 1024.f / 1024.f / 1024.f;
		printf("In current game configuration chunk buffer only will be of size %llu bytes",sz);
		printf(" (%.3f GiB).\nPlease note that fact.\n",cov);
		printf("New RAM threshold> ");
		scanf("%llu",&(s->rammax));
		if (s->rammax < 100) {
			s->rammax *= 1024 * 1024 * 1024;
			printf("Did you mean this value is in GiB? Converted to %llu bytes.\n",s->rammax);
		}
		break;

	case GAT_SEED:
		printf("Current seed = %ld\n",s->wg_seed);
		printf("New seed value (long int, %lu bytes)> ",sizeof(s->wg_seed));
		scanf("%ld",&(s->wg_seed));

		//generate test world
		puts("Generating test world...");
		wgen = new WorldGen(s->world_r,NULL,0);
		wgen->NewMap(s->wg_seed);
		printf("Cities generated: %d\n",wgen->GetNumCities());
		printf("Factories generated: %d\n",wgen->GetNumFactories());

		//print covering values
		cov = (float)wgen->GetNumCellsOf(WGCC_SPECBUILD) + (float)wgen->GetNumCellsOf(WGCC_CONCRETEB);
		cov = cov / wgen->GetPlaneArea() * 100.f;
		printf("Industrial-related cells: %.2f%%\n",cov);
		cov = (float)wgen->GetNumCellsOf(WGCC_HUGEBUILD) / wgen->GetPlaneArea() * 100.f;
		printf("Downtown cells: %.2f%%\n",cov);
		cov = (float)wgen->GetNumCellsOf(WGCC_MIDDLBLDS) / wgen->GetPlaneArea() * 100.f;
		printf("City end cells: %.2f%%\n",cov);
		cov = (float)wgen->GetNumCellsOf(WGCC_SMALLBLDS) / wgen->GetPlaneArea() * 100.f;
		printf("Suburb cells: %.2f%%\n",cov);
		cov = (float)wgen->GetNumCellsOf(WGCC_HUGEBUILD) + (float)wgen->GetNumCellsOf(WGCC_MIDDLBLDS);
		cov += (float)wgen->GetNumCellsOf(WGCC_SMALLBLDS);
		cov = cov / wgen->GetPlaneArea() * 100.f;
		printf("Total city cells: %.2f%%\n",cov);
		cov = (float)wgen->GetNumCellsOf(WGCC_TREEGRASS) / wgen->GetPlaneArea() * 100.f;
		printf("Park cells: %.2f%%\n",cov);
		cov = (float)wgen->GetNumCellsOf(WGCC_WASTELAND) / wgen->GetPlaneArea() * 100.f;
		printf("Pure wasteland cells: %.2f%%\n",cov);

		//discard testing world
		delete wgen;
		break;
	}

	goto opts_begin;
}

static void ishell_player(SGameSettings* s)
{
	//TODO
}

bool interactive_shell(SGameSettings* s)
{
	int in;

	puts("Welcome to the Plastic Inquisitor startup shell.");

shell_begin:
	puts("\nMain menu:");
	puts("1) Game options");
	puts("2) Player character generation");
	puts("9) Review current settings");
	puts("0) Close shell and run the game");
	puts("Q) Quit game");

	while (!isprint(in = getchar())) ; //trash non-printable chars (NL,LF,EOF etc)
	switch (in) {
	case '0':
		return true;
	case '1':
		ishell_options(s);
		break;
	case '2':
		ishell_player(s);
		break;
	case '9':
		printsettings(s);
		break;
	case 'Q':
		return false;
	}
	goto shell_begin;
	return false; //to make compiler happy
}

bool argparser(int argc, char* argv[], SGameSettings* sets)
{
	int i,j, fsm = 0;
	EGameArgType curt = GAT_NOTHING;

	for (i = 1; i < argc; i++) {
		switch (fsm) {
		case 0: /* default */
			//detect switch
			curt = GAT_NOTHING;
			if ((argv[i][0] != '-') || (strlen(argv[i]) != 2)) {
				errout("Malformed parameter switch at position %d.\n",i);
				return false;
			}
			//search for known switch character
			for (j = 0; j < GAMEARGTYPES; j++)
				if (argv[i][1] == argp_table[j].sw) {
					curt = argp_table[j].typ;
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

			case GAT_WORLDRAD:
				sets->world_r = atol(argv[i]);
				fsm = 0;
				break;

			case GAT_RAMMAX:
				sets->rammax = atoll(argv[i]);
				fsm = 0;
				break;

			case GAT_SEED:
				sets->wg_seed = atol(argv[i]);
				fsm = 0;
				break;

			case GAT_PLAYER:
				strncpy(sets->PCData.name,argv[i],MAXACTNAMELEN);
				fsm++;
				break;

			default:
				fsm = 0;
			}
			break;

		/* more than one sub-argument cases will go here */
		case 2:
			switch (curt) {
			case GAT_PLAYER:
				//TODO
				fsm = 0;
				break;

			default:
				fsm = 0;
			}
			break;

		default: /* just in case */
			fsm = 0;
			break;
		}
	}

	return true;
}

void arghelp(char* pname)
{
	int i;
	SGameSettings dflt = DEFAULT_SETTINGS;

	printf("\nUsage: %s [OPTIONS]\nAvailable options are:\n",pname);

	for (i = 0; i < GAMEARGTYPES; i++)
		printf("\t-%c <arg>: %s\n",argp_table[i].sw,argp_table[i].desc);

	printf("\nDefault setting are listed below.\n");
	printsettings(&dflt);
}

vector3d tripletovecf(const SCTriple s)
{
	return vector3d(s.r,s.g,s.b);
}

SCTriple vecftotriple(const vector3d s)
{
	SCTriple r;
	r.r = s.X;
	r.g = s.Y;
	r.b = s.Z;
	return r;
}
