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
#define RENDERPOOLN 4

///Desync time for render threads (us).
#define RENDERPOOLDESW 150

///Update time of Sky object (frames).
#define RENDERPOOLSKYUP 270


/* Pool runtime data structure */
struct SRendPoolDat {
	pthread_t thr;
	pthread_mutex_t mtx;
	LVR* lvr;
	volatile bool done;
	volatile bool quit;
	int start;
	volatile bool good;
	volatile bool dopproc;
};

/* RenderPool: multi-thread LVR */
class RenderPool : public LVR {
private:
	SRendPoolDat pool[RENDERPOOLN];	//Render pool data
	AtmoSky* skies;					//Skies rendering instance
	vector3di ipos,irot;			//Position and rotation of camera (integer)
	bool quit;						//Quit event flag
	pthread_t t_rend;				//Main thread
	pthread_mutex_t m_rend;			//Main thread frame mutex
	unsigned frames;				//Frame counter

	void SpawnThreads();			//Rendering threads spawner
	void KillThreads();				//Rendering threads killer

public:
	RenderPool(DataPipe* pipe);
	virtual ~RenderPool();

	///Internal facility. Do Not Use in external code!
	///Made public to be accessible from thread.
	bool Quantum();

	///Lock all RenderPool.
	void Lock();
	///Release RenderPool.
	void Unlock();
	///Return state of quit event flag.
	bool IsClosing()				{ return quit; }

	///Set camera position (integer).
	void SetPos(vector3di p);
	///Set camera rotation (integer, degrees).
	void SetRot(vector3di r);

	///Returns recently finished frame and moves on to make next one.
	SGUIPixel* GetRender();

	///Returns render pool internal data. Do Not Use in external code!
	///Made public to be accessible from thread.
	SRendPoolDat* GetPoolDatN(int n);

	///Returns current skies instance.
	AtmoSky* GetSkies()				{ return skies; }

	///LVR-compatible frame resizing.
	bool Resize(int w, int h);
	///LVR-compatible frame masking.
	void SetMask(char* m, int w, int h);

	///Set camera frustrum scaling.
	void SetScale(const double s);
	///Set camera Field of View.
	void SetFOV(const vector3d f);
	///Set far clipping plane distance.
	void SetFarDist(const int d);
	///Set new postprocessing settings.
	void SetPostprocess(const SLVRPostProcess p);

	void Frame()					{}
	void Postprocess()				{}
};

#endif /* RENDERPOOL_H_ */
