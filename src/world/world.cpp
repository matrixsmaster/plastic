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

#include "world.h"
#include "support.h"
#include "debug.h"


PlasticWorld::PlasticWorld(SGameSettings* settings)
{
	float alloc_gb;

	/* Init internal variables */
	result = 0;
	sets = settings;
	data = NULL;
	lvr = NULL;
	gui = NULL;
	PC = new Player();

	/* Create and set up DataPipe */
	data = new DataPipe(sets->root);
	if (data->GetStatus() == DPIPE_ERROR) {
		errout("Unable to initialize data pipe. Possibly invalid root directory.\n");
		result = 1;
		return;
	}
	alloc_gb = (float)(data->GetAllocatedRAM()) / 1024.f / 1024.f / 1024.f;
	printf("Size of voxel = %lu bytes\n",sizeof(voxel));
	printf("Allocated data pipe memory: %llu bytes (%.3f GiB)\n",data->GetAllocatedRAM(),alloc_gb);

	/* Create LVR */
	lvr = new LVR(data);

	//FIXME:
	scale = 1.0;
	fov = vector2di(DEFFOVX,DEFFOVY);
	data->SetGP(vector3dulli(0));
	lvr->SetPosition(vector3d(128));
	PC->SetPos(vector3di(128));
}

PlasticWorld::~PlasticWorld()
{
	if (PC) delete PC;
	if (lvr) delete lvr;
	if (data) delete data;
}

void PlasticWorld::Quantum()
{
	//TODO: world update (!)
}

void PlasticWorld::ConnectGUI(CurseGUI* guiptr)
{
	gui = guiptr;
	ConnectGUI();
}

void PlasticWorld::ConnectGUI()
{
	if ((!gui) || (!lvr)) {
		result = 1;
		return;
	}

	//resize LVR frame
	if (!lvr->Resize(gui->GetWidth(),gui->GetHeight())) {
		errout("Unable to resize LVR frame!\n");
		result = 2;
		return;
	}

	//Connect lvr output to CurseGUI main background
	gui->SetBackgroundData(lvr->GetRender(),lvr->GetRenderLen());

	//OK
	result = 0;
}

void PlasticWorld::ProcessEvents(const CGUIEvent* e)
{
	//DEBUG:
	vector3di r = PC->GetRot();
	vector3di p = PC->GetPos();

	result = 0;
	switch (e->t) {
	case GUIEV_KEYPRESS:
		switch (e->k) {
		case KEY_UP: r.X += 1; break;
		case KEY_DOWN: r.X -= 1; break;
		case KEY_LEFT: r.Z += 1; break;
		case KEY_RIGHT: r.Z -= 1; break;
		case 'w': p.Y += 1; break;
		case 's': p.Y -= 1; break;
		case 'a': p.X -= 1; break;
		case 'd': p.X += 1; break;
		case '-': p.Z -= 1; break;
		case '=': p.Z += 1; break;
		case '[': scale -= 0.01; break;
		case ']': scale += 0.01; break;
		case ',': fov.X--; break;
		case '.': fov.X++; break;
		case 'n': fov.Y--; break;
		case 'm': fov.Y++; break;
		}
		lvr->SetEulerRotation(r.ToReal());
		lvr->SetPosition(p.ToReal());
		lvr->SetScale(scale);
		lvr->SetFOV(fov);
		PC->SetPos(p);
		PC->SetRot(r);
		break;

		case GUIEV_RESIZE:
			/* Size of terminal has changed */
			ConnectGUI();
			break;

		default:
			errout("Warning: unknown event type pumped. Possibly memory corruption.\n");
			result = 1;
	}
}
