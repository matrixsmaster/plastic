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

#ifndef ADDSYNTH_H_
#define ADDSYNTH_H_

#include <string>
#include <vector>
#include <pthread.h>

#define AS_DEFRATE 44100
#define AS_DEFGAIN 100
#define AS_ENVNARGS 4
#define AS_DEBUG 1

enum EASWaveform {
	AS_SINE = 1,
	AS_TRIAN,
	AS_TRIBK,
	AS_SQR,
	AS_NOISE,
	AS_ADSR,
};

class AddSynth {
private:
	float gain,rate;
	std::vector<float> buf,env;
	pthread_mutex_t buflock;
	int ilock;

	void osc(std::vector<float>* out, float cr_gain, EASWaveform frm, float freq, float dur);
	void adsr(float tm_att, float tm_dec, float am_sus, float tm_rel);
	float inline duration() { return ((float)(buf.size()) / rate); }

public:
	AddSynth(float ng = AS_DEFGAIN, float nr = AS_DEFRATE);
	virtual ~AddSynth();

	void Gain(float ng)		{ gain = ng; }
	float Gain()			{ return gain; }
	void Rate(float nr)		{ rate = nr; }
	float Rate()			{ return rate; }

	void Lock();
	void Unlock();

	std::vector<float> GetBuf();
	float GetBufTime();
	std::string ToString();
	void Clear();

	void Generate(EASWaveform frm, float freq, float dur);
	void Envelope(EASWaveform frm, float args[AS_ENVNARGS]);
};

#endif /* ADDSYNTH_H_ */
