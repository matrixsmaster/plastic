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
	memset(&svhead,0,sizeof(svhead));

	//TODO

	return &svhead;
}

bool DataPipe::SaveGameHeader(SSavedGameHeader* hdr)
{
	if (!wgen) return false;

	//TODO
	hdr->gseed = rngen->GetSeed();

	return true;
}

bool DataPipe::LoadStaticWorld()
{
	char tmp[MAXPATHLEN];

	if (!wgen) return false;

	/* Load wgen map */
	snprintf(tmp,MAXPATHLEN,"%s/usr/worldmap",root);
	if (!wgen->LoadMap(tmp)) return false;

	/* Map known chunks */
	ScanFiles();

	return true;
}

bool DataPipe::SaveStaticWorld()
{
	char tmp[MAXPATHLEN];

	/* Save wgen map */
	snprintf(tmp,MAXPATHLEN,"%s/usr/worldmap",root);
	wgen->SaveMap(tmp);

	//TODO

	return true;
}
