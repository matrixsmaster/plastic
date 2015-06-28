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

/* Part of DataPipe class. Game data management facilities. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "datapipe.h"
#include "support.h"
#include "vecmisc.h"

#ifdef DPDEBUG
#include "debug.h"
#endif


SSavedGameHeader* DataPipe::LoadGameHeader()
{
	FILE* f;
	char tmp[MAXPATHLEN];
	memset(&svhead,0,sizeof(svhead));

	/* Open the file */
	snprintf(tmp,MAXPATHLEN,GAMEHDRFNPAT,root);
	f = fopen(tmp,"rb");
	if (!f) return NULL;

	/* Load up data */
	if (fread(&svhead,sizeof(svhead),1,f) != 1) {
		memset(&svhead,0,sizeof(svhead)); //reset broken data
		fclose(f);
		return NULL;
	}

	/* Close the file */
	fclose(f);

	/* Restore RNG seed */
	rngen->SetSeed(svhead.gseed);

	return &svhead;
}

bool DataPipe::SaveGameHeader(SSavedGameHeader* hdr)
{
	FILE* f;
	char tmp[MAXPATHLEN];

	if (!hdr) return false;

	/* Open the file */
	snprintf(tmp,MAXPATHLEN,GAMEHDRFNPAT,root);
	f = fopen(tmp,"wb");
	if (!f) return false;

	/* Save current seed */
	hdr->gseed = rngen->GetSeed();

	/* Write down data */
	fwrite(hdr,sizeof(SSavedGameHeader),1,f);

	/* And close the file */
	fclose(f);

	return true;
}

bool DataPipe::LoadStaticWorld()
{
	char tmp[MAXPATHLEN];

	if (!wgen) return false;

	/* Load wgen map */
	snprintf(tmp,MAXPATHLEN,WORLDMAPFNPAT,root);
	if (!wgen->LoadMap(tmp)) return false;

	/* Map known chunks */
	ScanFiles();

	return true;
}

bool DataPipe::SaveStaticWorld()
{
	char tmp[MAXPATHLEN];

	/* Save wgen map */
	snprintf(tmp,MAXPATHLEN,WORLDMAPFNPAT,root);
	wgen->SaveMap(tmp);

	//TODO

	return true;
}

bool DataPipe::DeserializeThem(GDVec* arr, const char* name)
{
	FILE* f;
	char tmp[MAXPATHLEN];

	if ((!arr) || (!name) || (arr->empty())) return false;

	snprintf(tmp,MAXPATHLEN,PACKAGEFNPAT,root,name);
	f = fopen(tmp,"rb");
	if (!f) {
		errout("Unable to open file '%s' for reading\n",tmp);
		return false;
	}

#ifdef DPDEBUG
	dbg_print("File %s opened for deserialization",tmp);
#endif

	//TODO

	fclose(f);
	return false;
}

bool DataPipe::SerializeThem(GDVec* arr, const char* name)
{
	FILE* f;
	char tmp[MAXPATHLEN];
	GDVec::iterator ia;

	if ((!arr) || (!name) || (arr->empty())) return false;

	snprintf(tmp,MAXPATHLEN,PACKAGEFNPAT,root,name);
	f = fopen(tmp,"wb");
	if (!f) {
		errout("Unable to open file '%s' for writing\n",tmp);
		return false;
	}

#ifdef DPDEBUG
	dbg_print("File %s opened for serialization",tmp);
#endif

	//TODO
	for (ia = arr->begin(); ia != arr->end(); ++ia)
		if (!(((*ia))->SerializeToFile(f))) {
			dbg_print("Unable to ser. obj %p",(*ia));
			fclose(f);
			return false;
		}

	fclose(f);
	return true;
}
