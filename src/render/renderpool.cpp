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
//debug:
#include "debug.h"
#include "support.h"


static void* rendpool_lvrthread(void* ptr)
{
	SRendPoolDat* me = reinterpret_cast<SRendPoolDat*> (ptr);
	LVR* lvr = me->lvr;

	for (;;) {
		if (me->good) {
			lvr->Frame();

			pthread_mutex_lock(&(me->mtx));

//			memset(lvr->GetRender(),0,lvr->GetRenderLen()*sizeof(SGUIPixel));

			lvr->SwapBuffers();

			pthread_mutex_unlock(&(me->mtx));
		}

		me->done = true;

		while (me->done) {
			if (me->quit) return NULL;
			usleep(RENDERPOOLDESW);
		}
	}

	return NULL;
}

static void* rendpool_mainthread(void* ptr)
{
	int i;
	RenderPool* ths = reinterpret_cast<RenderPool*> (ptr);

	while (!ths->Quantum()) usleep(RENDERPOOLDESW);

	for (i = 0; i < RENDERPOOLN; i++)
		ths->GetPoolDatN(i)->quit = true;

	return NULL;
}

RenderPool::RenderPool(DataPipe* pipe) :
		LVR(pipe)
{
	int i;

	skies = new AtmoSky(pipe);
	quit = false;
	fps = 0;

	pthread_mutex_init(&m_rend,NULL);

	for (i = 0; i < RENDERPOOLN; i++) {
		memset(&pool[i],0,sizeof(SRendPoolDat));

		pool[i].lvr = new LVR(pipe);

		pthread_mutex_init(&(pool[i].mtx),NULL);
		pthread_create(&(pool[i].thr),NULL,rendpool_lvrthread,&(pool[i]));
	}

	pthread_create(&t_rend,NULL,rendpool_mainthread,this);
}

RenderPool::~RenderPool()
{
	int i;

	quit = true;

	pthread_join(t_rend,NULL);

	for (i = 0; i < RENDERPOOLN; i++) {
		pool[i].quit = true;

		pthread_join(pool[i].thr,NULL);
		pthread_mutex_destroy(&(pool[i].mtx));

		delete (pool[i].lvr);
	}

	pthread_mutex_destroy(&m_rend);

	delete skies;
}

bool RenderPool::Quantum()
{
	int i;
	uli l,shf;
	SRendPoolDat* cur;
	LVR* lvr;
	SGUIPixel* tmp;

	if ((!render) || (!zbuf) || (!pbuf)) return quit;

	pthread_mutex_lock(&m_rend);

#ifdef LVRDOUBLEBUFFERED
	shf = (activebuf)? rendsize:0;
#else
	shf = 0;
#endif

	skies->RenderTo(render+shf,g_w,g_h);

//	pipeptr->Lock();

	for (i = 0; i < RENDERPOOLN; i++) {
		cur = pool + i;
//		if ((!cur->done) || (!cur->good)) continue;
		if (!cur->good) continue;
//		while (!cur->done) ;
		lvr = cur->lvr;

		pthread_mutex_lock(&(cur->mtx));

//		lvr->SwapBuffers();

		l = lvr->GetRenderLen();
		tmp = lvr->GetRender();

		while (l--) {
			if (tmp[l].sym)
				render[shf+cur->start+l] = tmp[l];
		}
		cur->done = false;
		pthread_mutex_unlock(&(cur->mtx));
//		memcpy(render+shf+cur->start,lvr->GetRender(),l * sizeof(SGUIPixel));
	}

//	for (i = 0; i < RENDERPOOLN; i++) {
//		cur = pool + i;
//		if (!cur->good) continue;
//		while (!cur->done) ;
//		cur->done = false;
//	}

//	pipeptr->Unlock();

	pthread_mutex_unlock(&m_rend);

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

	for (i = 0; i < RENDERPOOLN; i++)
		pool[i].lvr->SetPosition(ps);
}

void RenderPool::SetRot(vector3di r)
{
	int i;
	float tmp;
	vector3d rt = r.ToReal();

	for (i = 0; i < RENDERPOOLN; i++)
		pool[i].lvr->SetEulerRotation(rt);

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

	pthread_mutex_lock(&m_rend);
//	Lock();
	SwapBuffers();
#ifdef LVRDOUBLEBUFFERED
	//return NOT active buffer data
	ptr = (render + ((activebuf)? 0:rendsize));
#else
	ptr = render;
#endif

//	Unlock();
	pthread_mutex_unlock(&m_rend);

	return ptr;
}

SRendPoolDat* RenderPool::GetPoolDatN(int n)
{
	if ((n < 0) || (n >= RENDERPOOLN)) n = 0; //always valid
	return (&(pool[n]));
}

bool RenderPool::Resize(int w, int h)
{
	int i,n,s;
	vector2di mid;

#if RENDERPOOLN < 3
#error "RENDERPOOL is too small"
#endif

	n = h / (RENDERPOOLN - 1);
	mid.X = w / 2;
	mid.Y = h / 2;

	dbg_logstr("Awaiting resize...");

	Lock();

	g_w = w;
	g_h = h;
	rendsize = w * h;

	dbg_print("Resized to %d x %d = %u",w,h,rendsize);

	ReallocBuffers();

	for (i = 0, s = 0; i < RENDERPOOLN; i++, s+=n) {
		pool[i].start = s * g_w;
		if (s+n >= h) n = h - s;
		pool[i].good = pool[i].lvr->Resize(w,n);
//		if (!pool[i].good) {
//			errout("Resize %d %d\n",w,n);
//			abort();
//		}
		pool[i].lvr->SetMid(mid);
		mid.Y -= n;
		pool[i].done = false;
	}

	Unlock();

	return true;
}

void RenderPool::SetMask(char* m, int w, int h)
{
	Lock();
	Unlock();
}

void RenderPool::SetScale(const double s)
{
	//TODO
}

void RenderPool::SetFOV(const vector3d f)
{
	//TODO
}

void RenderPool::SetFarDist(const int d)
{
	//TODO
}

void RenderPool::SetFogStart(const int d)
{
	//TODO
}

void RenderPool::SetFogColor(const vector3di nfc)
{
	//TODO
}
