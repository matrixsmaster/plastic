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

/* This unit defines a Render Pool - special case of multithread LVR */


#ifndef RENDERPOOL_H_
#define RENDERPOOL_H_

#include <pthread.h>
#include "datapipe.h"
#include "LVR.h"
#include "sky.h"
#include "vecmath.h"
#include "visual.h"


///Size of render pool (max number of threads).
#define RENDERPOOLN 3

///Desync time for render threads (us).
#define RENDERPOOLDESW 100

/* Pool runtime data structure */
struct SRendPoolDat {
	pthread_t thr;
	pthread_mutex_t mtx;
	LVR* lvr;
	bool done;
	bool quit;
	int start;
	bool good;
};

/* RenderPool: multi-thread LVR */
class RenderPool : public LVR {
private:
	SRendPoolDat pool[RENDERPOOLN];
	AtmoSky* skies;
	vector3di ipos,irot;
	bool quit;
	pthread_t t_rend;
	pthread_mutex_t m_rend;
	ulli fps;

	void SpawnThreads();
	void KillThreads();

public:
	RenderPool(DataPipe* pipe);
	virtual ~RenderPool();

	//FIXME: comment it
	bool Quantum();

	void Lock();
	void Unlock();
	bool IsClosing()				{ return quit; }

	void SetPos(vector3di p);
	void SetRot(vector3di r);

	SGUIPixel* GetRender();
	SRendPoolDat* GetPoolDatN(int n);

	bool Resize(int w, int h);
	void SetMask(char* m, int w, int h);

	void SetScale(const double s);
	void SetFOV(const vector3d f);
	void SetFarDist(const int d);
	void SetFogStart(const int d);
	void SetFogColor(const vector3di nfc);

	void Frame()					{}

	ulli GetFPS()					{ return fps; }
};

#endif /* RENDERPOOL_H_ */
