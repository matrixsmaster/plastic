/**
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

#include <inttypes.h>
#include <time.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <sys/time.h>
#include "AddSynth.h"
#include "ALSAWrapper.h"

using namespace std;

#if 0

//#define CYCLES 300
#define FREQ 440
#define AMP 100
//#define QUANTUM 0.1

int gen(string* d, float f, float tm, int tp)
{
	int j,cnt = 0;
	float t,n,dt;
	int16_t s;

	dt = 1.f / 44100.f;

	for (t = 0; t < tm;) {
		n = floor(44100.f / f) - 1;

		for (j = 0; (j < n) && (t < tm); j++,cnt++,t+=dt) {
			switch (tp) {
			case 1:
				s = floor((float)AMP * sin(2.f * M_PI * ((float)j / n)));
				break;

			case 2:
				s = floor((float)AMP * (float)j / n);
				break;

			case 3:
				s = (j > (n/2))? 0:AMP;
				break;

			case 10:
				s = (float)random() / (float)RAND_MAX * AMP;
				break;
			}

			(*d) += (char)(s >> 8);
			(*d) += (char)(s >> 0);
		}
	}

	return cnt;
}

void slide(string* d, float beg, float end, float tm)
{
//	float ct,ot,cf,nf;
	float ct,cf,p;
	float df = (end - beg) / tm;

//	for (cf = nf = beg, ct = ot = 0; ct < tm; ct += QUANTUM) {
//		if (floor(fabs(cf-nf)) > 1) {
//			gen(d,cf,ot);
//			cf = nf;
//			ot = 0;
//		} else {
//			nf += df * QUANTUM;
//			ot += QUANTUM;
//		}
//	}

	for (cf = beg, ct = 0; ct < tm; ct += p, cf += df * p) {
		p = 1.f / cf;
		gen(d,cf,p,1);
	}
}
//#endif

void adsr(string* d, string* o, float tm_att, float tm_dec, float am_sus, float tm_rel)
{
	int16_t s;
	string::iterator i = d->begin();
	float ct = 0;
	float da = 1.f/(tm_att*44100.f);
	float dd = (1.f-am_sus)/(tm_dec*44100.f);
	float ts = (float)(d->size()>>1) / 44100.f - (tm_att+tm_dec+tm_rel);
	float dr = am_sus/(tm_rel*44100.f);
	float ca;
	float A,k; //debug

	cout << "da = " << da << endl;
	cout << "dd = " << dd << endl;
	cout << "dr = " << dr << endl;
	cout << "len = " << (float)((float)(d->size()>>1) / 44100.f) << endl;
	cout << "tm_sus = " << ts << endl;

	o->clear();

	ca = 0;
	for (; i != d->end(); ct += (1.f/44100.f)) {
		s = (int16_t)(*(i++)) << 8;
		s |= *(i++);

		if (ct < tm_att) {
			k = ca;
			ca += da;
			A = 1;
		} else if (ct < (tm_att+tm_dec)) {
			k = ca;
			ca -= dd;
			A = 2;
		} else if (ct < (tm_att+tm_dec+ts)) {
			k = am_sus;
			ca = am_sus;
			A = 3;
		} else {
			k = ca;
			ca -= dr;
			A = 4;
		}

//		cout << "A = " << A << "  ct = " << ct << "  k = " << k << "  s = " << s << endl;
		s *= k;

		(*o) += (char)(s >> 8);
		(*o) += (char)(s >> 0);
	}
}

#endif

int main(int argc, char* argv[])
{
	int i;
	ALSAWrapper wrap(false,44100,1);
	string sbf;
	timeval beg,end;
//	float f = FREQ;
	ofstream outp("/tmp/test_s16_44100.raw");
	AddSynth sth;

	cout << "Alive!" << endl;

	srandom(time(NULL));
	if (!wrap.Initialize()) return 1;

	cout << "Initialized!" << endl;

//	for (i = 0; i < CYCLES; i++) {
//		gen(&sbf,f,1);
//	}

//	gen(&sbf,440,1,1);
//	gen(&sbf,550,100,1);

//	slide(&sbf,440,880,1.5);
//	slide(&sbf,880,440,1.5);

	for (i = 1; i < 5; i++) {
		sbf.clear();
		cout << "Waveform " << i << endl;

//		gen(&sbf,880,3,i);
		sth.Clear();
		sth.Generate((EASWaveform)i,880,3);
//		string tmp = sth.ToString();
//		adsr(&tmp,&sbf,1e-3,1.2,0.3,1.2);

		float p[4];
		p[0] = 3;
		sth.Envelope(AS_SINE,p);

		p[0] = 0.6;
		p[1] = 1.2;
		p[2] = 0.3;
		p[3] = 1.2;
		sth.Envelope(AS_ADSR,p);

		sbf = sth.ToString();

		//	gen(&sbf,100,0.14,10);
		//	string tmp = sbf;
		//	adsr(&tmp,&sbf,1e-3,0.11,0.2,1e-2);

		gettimeofday(&beg,NULL);
		wrap.AppendData(0,sbf);
		gettimeofday(&end,NULL);

		end.tv_sec -= beg.tv_sec;
		end.tv_usec -= beg.tv_usec;
		cout << "Time = " << end.tv_sec << ":" << end.tv_usec << endl;

		outp << sbf;
	}

	cout << "Freeing resources..." << endl;

	wrap.CloseDevice();

	cout << "End of program." << endl;

	return 0;
}
