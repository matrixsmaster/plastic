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

#include <iostream>
#include <math.h>
#include <stdlib.h>
#include "AddSynth.h"

using namespace std;

AddSynth::AddSynth(float ng, float nr)
{
	rate = nr;
	gain = ng;
	pthread_mutex_init(&buflock,NULL);
	ilock = 0;
}

AddSynth::~AddSynth()
{
	pthread_mutex_destroy(&buflock);
}

void AddSynth::Lock()
{
//	if (ilock) ilock++;
//	else
		pthread_mutex_lock(&buflock);
}

void AddSynth::Unlock()
{
//	if (ilock > 0) ilock--;
//	else
		pthread_mutex_unlock(&buflock);
}

vector<float> AddSynth::GetBuf()
{
	vector<float> tmp;
	Lock();
	tmp = buf;
	Unlock();
	return tmp;
}

float AddSynth::GetBufTime()
{
	float l;

	Lock();
	l = duration();
	Unlock();

	return l;
}

string AddSynth::ToString()
{
	int16_t s;
	string str;
	vector<float>::iterator i;

	Lock();
	for (i = buf.begin(); i != buf.end(); ++i) {
		s = (int16_t)floor(*i);

		str += (char)(s >> 8); //LE
		str += (char)(s >> 0);
	}
	Unlock();

	return str;
}

void AddSynth::Clear()
{
	Lock();
	buf.clear();
	env.clear();
	Unlock();
}

void AddSynth::osc(vector<float>* out, float cr_gain, EASWaveform frm, float freq, float dur)
{
	int j;
	float s,t,n,dt = 1.f / rate;

	for (t = 0; t < dur;) {
		n = floor(rate / freq) - 1;

		for (j = 0; (j < n) && (t < dur); j++,t+=dt) {
			switch (frm) {
			case AS_SINE:
				s = cr_gain * sin(2.f * M_PI * ((float)j / n));
				break;

			case AS_TRIAN:
				s = cr_gain * (float)j / n;
				break;

			case AS_TRIBK:
				s = cr_gain * (n - (float)j) / n;
				break;

			case AS_SQR:
				s = (j > (n/2))? 0 : cr_gain;
				break;

			case AS_NOISE:
				s = (float)random() / (float)RAND_MAX * cr_gain;
				break;

			default:
				//just to make compiler happy
				break;
			}

			out->push_back(s);
		}
	}
}

void AddSynth::Generate(EASWaveform frm, float freq, float dur)
{
	Lock();
	osc(&buf,gain,frm,freq,dur);
	Unlock();
}

void AddSynth::adsr(float tm_att, float tm_dec, float am_sus, float tm_rel)
{
	vector<float>::iterator i = buf.begin();
	float ca = 0,ct = 0;
	float da = 1.f/(tm_att*rate);
	float dd = (1.f-am_sus)/(tm_dec*rate);
	float ts = (float)(buf.size()) / rate - (tm_att+tm_dec+tm_rel);
	float dr = am_sus/(tm_rel*rate);

#if AS_DEBUG > 1
	cout << "ADSR da = " << da << endl;
	cout << "ADSR dd = " << dd << endl;
	cout << "ADSR dr = " << dr << endl;
	cout << "ADSR len = " << (float)((float)(buf.size()) / rate) << endl;
	cout << "ADSR tm_sus = " << ts << endl;
#endif

	for (; i != buf.end(); ct += (1.f/rate), ++i) {
		if (ct < tm_att) {
			env.push_back(ca);
			ca += da;
		} else if (ct < (tm_att+tm_dec)) {
			env.push_back(ca);
			ca -= dd;
		} else if (ct < (tm_att+tm_dec+ts)) {
			env.push_back(am_sus);
			ca = am_sus;
		} else {
			env.push_back(ca);
			ca -= dr;
		}
	}
}

void AddSynth::Envelope(EASWaveform frm, float args[AS_ENVNARGS])
{
	vector<float>::iterator i,j;

	Lock();
//	ilock = 1;

	env.clear();

	switch (frm) {
	case AS_ADSR:
		adsr(args[0],args[1],args[2],args[3]);
		break;

	case AS_SINE:
	case AS_TRIAN:
	case AS_TRIBK:
	case AS_SQR:
	case AS_NOISE:
		osc(&env,1.f,frm,args[0],duration());
		break;

	default:
		//do nothing
		return;
	}

	for (i = buf.begin(), j = env.begin(); (i != buf.end()) && (j != env.end()); ++i,++j)
		*i *= *j;

//	ilock = 0;
	Unlock();
}
