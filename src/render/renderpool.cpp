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

#include "renderpool.h"


static void* rendpool_lvrthread(void* ptr)
{
	SRendPoolDat* me = (SRendPoolDat*)ptr;
	LVR* lvr = me->lvr;

	for (;;) {
		/* For allocated renderer, do next sub-frame */
		if (me->good) {
			lvr->Frame(); //meanwhile, main thread can copy previous sub-frame
			/* Optionally, do post-processing */
			if (me->dopproc)
				lvr->Postprocess();

			/* Lock renderer for swapping buffers */
			pthread_mutex_lock(&(me->mtx));

			lvr->SwapBuffers(); //select location for next sub-frame

			/* This renderer is ready to transfer its sub-frame */
			me->done = true;

			/* Unlock renderer */
			pthread_mutex_unlock(&(me->mtx));

		} else {
			/* Just set the done flag */
			pthread_mutex_lock(&(me->mtx));
			me->done = true;
			pthread_mutex_unlock(&(me->mtx));
		}

		/* Wait for transfer or quit event */
		while (me->done) {
			if (me->quit) pthread_exit(NULL);
			usleep(RENDERPOOLDESW);
		}
	}

	pthread_exit(NULL);
}

static void* rendpool_mainthread(void* ptr)
{
	int i;
	RenderPool* ths = (RenderPool*)ptr;

	/* Continuously check all renderers with some time gap until quit event */
	while (!ths->Quantum()) usleep(RENDERPOOLDESW);

	/* We're quitting, push quit event down to all renderers */
	for (i = 0; i < RENDERPOOLN; i++)
		ths->GetPoolDatN(i)->quit = true;

	pthread_exit(NULL);
}

RenderPool::RenderPool(DataPipe* data, bool started) :
		LVR(data)
{
	skies = new AtmoSky(data);
	quit = false;
	frames = 0;
	stopped = !started;

	/* Copy initialized PP to global setting and reset local */
	g_pproc = pproc;
	memset(&pproc,0,sizeof(pproc));

	/* Create frame mutex */
	pthread_mutex_init(&m_rend,NULL);

	/* Spawn all rendering threads */
	if (started) SpawnThreads();

	/* Spawn main thread */
	pthread_create(&t_rend,NULL,rendpool_mainthread,this);
}

RenderPool::~RenderPool()
{
	/* Quit event */
	quit = true;

	/* Wait for the end of the main thread */
	pthread_join(t_rend,NULL);

	/* Close all renderers */
	if (!stopped) KillThreads();

	/* Destroy frame mutex */
	pthread_mutex_destroy(&m_rend);

	/* Free other facilities */
	delete skies;
}

void RenderPool::SpawnThreads()
{
	for (int i = 0; i < RENDERPOOLN; i++) {
		/* Reset renderer data */
		memset(&pool[i],0,sizeof(SRendPoolDat));

		/* Create LVR instance */
		pool[i].lvr = new LVR(pipe);

		/* Create renderer mutex and start rendering thread */
		pthread_mutex_init(&(pool[i].mtx),NULL);
		pthread_create(&(pool[i].thr),NULL,rendpool_lvrthread,&(pool[i]));
	}
}

void RenderPool::KillThreads()
{
	for (int i = 0; i < RENDERPOOLN; i++) {
		/* Push quit event */
		pool[i].quit = true;

		/* Wait for ending and destroy mutex */
		pthread_join(pool[i].thr,NULL);
		pthread_mutex_destroy(&(pool[i].mtx));

		/* Now just destroy LVR instance */
		delete (pool[i].lvr);
		pool[i].lvr = NULL;
		pool[i].good = false;
	}
}

bool RenderPool::Quantum()
{
	int i;
	uli l,shf,csh;
	SRendPoolDat* cur;
	LVR* lvr;
	SGUIPixel* vbf;
	const int* zbf;
	const vector3di* pbf;

	/* Dry-run: buffer(s) not ready */
	if ((!render) || (!zbuf) || (!pbuf)) return quit;

	/* Lock frame mutex */
	pthread_mutex_lock(&m_rend);

	/* Dry-run: pool stopped */
	if (stopped) {
		pthread_mutex_unlock(&m_rend);
		return quit;
	}

	/* Hold frame offset value */
#ifdef LVRDOUBLEBUFFERED
	shf = (activebuf)? rendsize:0;
#else
	shf = 0;
#endif

	/* Draw skies background */
	skies->RenderTo(render+shf,g_w,g_h);

	for (i = 0, cur = pool; i < RENDERPOOLN; i++, cur++) {
		if (!cur->good) continue; //discard not used renderers
		lvr = cur->lvr;

		/* Lock renderer buffer swapping */
		pthread_mutex_lock(&(cur->mtx));

		/* Prepare addresses */
		l = lvr->GetRenderLen();
		vbf = lvr->GetRender();
		zbf = lvr->GetZBuf();
		pbf = lvr->GetPBuf();

		/* Copy all data needed */
		csh = shf + cur->start;
		while (l--) {
			if (vbf[l].sym)
				render[csh+l] = vbf[l];
			zbuf[csh+l] = zbf[l];
			pbuf[csh+l] = pbf[l];
		}

		/* Release renderer */
		cur->done = false;
		pthread_mutex_unlock(&(cur->mtx));
	}

	/* Signal workers to start drawing next frame */
//	for (i = 0, cur = pool; i < RENDERPOOLN; i++, cur++)
//		cur->done = false;

	/* Do local postprocessing */
	Postprocess();

	/* Release frame buffer */
	pthread_mutex_unlock(&m_rend);

	/* Update skies */
	if (++frames >= RENDERPOOLSKYUP) {
		frames = 0;
		skies->Quantum();
	}

	return quit;
}

void RenderPool::Lock()
{
	int i;

	pthread_mutex_lock(&m_rend);

	for (i = 0; i < RENDERPOOLN; i++)
		pthread_mutex_lock(&(pool[i].mtx));
}

void RenderPool::Unlock()
{
	int i;

	pthread_mutex_unlock(&m_rend);

	for (i = 0; i < RENDERPOOLN; i++)
		pthread_mutex_unlock(&(pool[i].mtx));
}

void RenderPool::SetPos(vector3di p)
{
	int i;
	vector3d ps = p.ToReal();
	ipos = p;

	for (i = 0; i < RENDERPOOLN; i++)
		if (pool[i].lvr) pool[i].lvr->SetPosition(ps);
}

void RenderPool::SetRot(vector3di r)
{
	int i;
	float tmp;
	vector3d rt = r.ToReal();
	irot = r;

	for (i = 0; i < RENDERPOOLN; i++)
		if (pool[i].lvr) pool[i].lvr->SetEulerRotation(rt);

	//swap Y-Z
	tmp = rt.Y;
	rt.Y = rt.Z;
	rt.Z = tmp;

	//set skies rotation
	pthread_mutex_lock(&m_rend);
	skies->SetEulerAngles(rt);
	pthread_mutex_unlock(&m_rend);
}

SGUIPixel* RenderPool::GetRender()
{
	SGUIPixel* ptr;

	/* Lock frame mutex */
	pthread_mutex_lock(&m_rend);

	/* Return empty frame if pool is stopped */
	if (stopped) {
		pthread_mutex_unlock(&m_rend);
		return NULL;
	}

	/* We'll return the frame which just have been created,
	 * so select new frame location.
	 */
	SwapBuffers();

#ifdef LVRDOUBLEBUFFERED
	//return NOT active buffer data
	ptr = (render + ((activebuf)? 0:rendsize));
#else
	ptr = render;
#endif

	/* Release frame mutex */
	pthread_mutex_unlock(&m_rend);

	return ptr;
}

SRendPoolDat* RenderPool::GetPoolDatN(int n)
{
	if ((n < 0) || (n >= RENDERPOOLN)) n = 0; //always valid
	return (&(pool[n]));
}

void RenderPool::Stop()
{
	if (stopped) return;

	/* Stop all the threads */
	pthread_mutex_lock(&m_rend);
	KillThreads();

	/* Set flag and move on */
	stopped = true;
	pthread_mutex_unlock(&m_rend);
}

void RenderPool::Start()
{
	if (!stopped) return;

	/* Lock frame mutex and spawn rendering workers */
	pthread_mutex_lock(&m_rend);
	SpawnThreads();

	/* Set flag and release frame */
	stopped = false;
	pthread_mutex_unlock(&m_rend);

	/* Resize everything */
	Resize(g_w,g_h);
}

bool RenderPool::Resize(int w, int h)
{
	int i,n,s;
	vector2di mid;

#if RENDERPOOLN < 3
#error "RENDERPOOL is too small"
#endif

	/* Prepare height of each sub-frame, and a central mid-point */
	n = h / (RENDERPOOLN - 1);
	mid.X = w / 2;
	mid.Y = h / 2;
	g_w = w;
	g_h = h;
	rendsize = w * h;

	/* Lock frame mutex */
	pthread_mutex_lock(&m_rend);

	/* Early return in case of stopped pool */
	if (stopped) {
		pthread_mutex_unlock(&m_rend);
		return true;
	}

	/* Destroy all renderers (just to stop them completely) */
	KillThreads();

	/* Reallocate main buffers and start renderers */
	ReallocBuffers();
	SpawnThreads();

	for (i = 0, s = 0; i < RENDERPOOLN; i++, s+=n) {
		pthread_mutex_lock(&(pool[i].mtx));
		pool[i].start = s * g_w; //sub-frame offset
		if (s+n >= h) n = h - s; //sub-frame height check
		pool[i].good = pool[i].lvr->Resize(w,n); //resize sub-frame
		pool[i].lvr->SetMid(mid); //set sub-frame mid-point
		mid.Y -= n; //and move mid-point up bu sub-frame height
		pool[i].done = false; //sub-frame is dirty now and ready to process
		pthread_mutex_unlock(&(pool[i].mtx));
	}

	/* Reset post-processing completely */
	SetPostprocess(g_pproc);

	/* Release frame mutex */
	pthread_mutex_unlock(&m_rend);

	return true;
}

void RenderPool::SetMask(char* m, int w, int h)
{
	int i;
	char* msk = m;

	Lock();
	for (i = 0; i < RENDERPOOLN; i++)
		if (pool[i].good) {//set the mask for active renderers only
			msk = m + pool[i].start;
			pool[i].lvr->SetMask((m)? msk:NULL,w,pool[i].lvr->GetHeight());
		}
	Unlock();
}

void RenderPool::SetScale(const double s)
{
	scale = vector3d(s);
	for (int i = 0; i < RENDERPOOLN; i++)
		if (pool[i].lvr) pool[i].lvr->SetScale(s);
}

void RenderPool::SetFOV(const vector3d f)
{
	fov = f;
	far = (int)round(f.Z);
	for (int i = 0; i < RENDERPOOLN; i++)
		if (pool[i].lvr) pool[i].lvr->SetFOV(f);
}

void RenderPool::SetPostprocess(const SLVRPostProcess p)
{
	//global PP enable flag
	bool ppc = (	(p.fog_dist) ||
					(p.noise) );

	g_pproc = p;
	memset(&pproc,0,sizeof(pproc));

	//disable textured rendering in global PP
	if (p.txd_fplane) g_pproc.txd_fplane = 0;

	//set PP for all workers
	for (int i = 0; i < RENDERPOOLN; i++) {
		if (pool[i].lvr)
			pool[i].lvr->SetPostprocess(g_pproc);
		pool[i].dopproc = ppc;
	}

	//set renderpool's own PP (local)
	if (p.txd_fplane) {
		pproc.txd_fplane = p.txd_fplane;
		pproc.txd_nplane = p.txd_nplane;
		pproc.txd_minw = p.txd_minw;
		pproc.txd_minh = p.txd_minh;
	}

	//restore all the values to make sure we'll return
	//the right structure in GetPostprocess()
	g_pproc = p;
}
