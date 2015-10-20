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

#ifndef ALSAWRAPPER_H_
#define ALSAWRAPPER_H_

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <pthread.h>
#include <inttypes.h>
#include <alsa/asoundlib.h>

#define PCM_DEVICE 			"default"
#define NUM_CHANNELS		1
#define BYTE_PER_SAMPLE 	2
#define SAMPLE_RATE 		8000
#define SIZE_INIT_BUFFER	10
#define ALSA_RESET_TIMEOUT	100000

//#define DEBUG_ALSA
//#define DEBUG_ALSA_CALLBACK_MESS
#define ALSA_ERRORS
//#define ALSA_WRAPPER_MESS

static const snd_pcm_format_t formats[] = {
	SND_PCM_FORMAT_S8,
	SND_PCM_FORMAT_U8,
	SND_PCM_FORMAT_S16_LE,
	SND_PCM_FORMAT_S16_BE,
	SND_PCM_FORMAT_U16_LE,
	SND_PCM_FORMAT_U16_BE,
	SND_PCM_FORMAT_S24_LE,
	SND_PCM_FORMAT_S24_BE,
	SND_PCM_FORMAT_U24_LE,
	SND_PCM_FORMAT_U24_BE,
	SND_PCM_FORMAT_S32_LE,
	SND_PCM_FORMAT_S32_BE,
	SND_PCM_FORMAT_U32_LE,
	SND_PCM_FORMAT_U32_BE,
	SND_PCM_FORMAT_FLOAT_LE,
	SND_PCM_FORMAT_FLOAT_BE,
	SND_PCM_FORMAT_FLOAT64_LE,
	SND_PCM_FORMAT_FLOAT64_BE,
	SND_PCM_FORMAT_IEC958_SUBFRAME_LE,
	SND_PCM_FORMAT_IEC958_SUBFRAME_BE,
	SND_PCM_FORMAT_MU_LAW,
	SND_PCM_FORMAT_A_LAW,
	SND_PCM_FORMAT_IMA_ADPCM,
	SND_PCM_FORMAT_MPEG,
	SND_PCM_FORMAT_GSM,
	SND_PCM_FORMAT_SPECIAL,
	SND_PCM_FORMAT_S24_3LE,
	SND_PCM_FORMAT_S24_3BE,
	SND_PCM_FORMAT_U24_3LE,
	SND_PCM_FORMAT_U24_3BE,
	SND_PCM_FORMAT_S20_3LE,
	SND_PCM_FORMAT_S20_3BE,
	SND_PCM_FORMAT_U20_3LE,
	SND_PCM_FORMAT_U20_3BE,
	SND_PCM_FORMAT_S18_3LE,
	SND_PCM_FORMAT_S18_3BE,
	SND_PCM_FORMAT_U18_3LE,
	SND_PCM_FORMAT_U18_3BE,
};

static const unsigned int rates[] = {
	5512,
	8000,
	11025,
	16000,
	22050,
	32000,
	44100,
	48000,
	64000,
	88200,
	96000,
	176400,
	192000,
};

enum EPCMMode {
	SND_PLAYBACK = 0,
	SND_CAPTURE
};

class ALSAWrapper; //Forward declaration
struct SAsync_private_data {
	ALSAWrapper* inst;
	char *buf;					//Buffer with wav data
	long int cnt;				//Position in buffer
	long int len;				//Buffer length
	int buf_size;				//Hardware buffer size
	unsigned int chnum;			//Channels number
	short bytePerS;				//Bytes per sample
};

struct SDeviceSettings {
	char *name;
	snd_pcm_t *handle;
};

class ALSAWrapper
{
private:
	int err;					//Error code
	int buf_size;				//Period size in frames
	char *wavbuf;				//Buffer for wav file
	bool asyncCallback;			//Use or not async playback

	unsigned int pSampleRate;	//Sample rate
	unsigned int numChannels;	//Channels number
	short bytePerSample;		//Bytes per sample
	int restart;

	pthread_mutex_t bufmtx;

	std::vector<SDeviceSettings>chandles;	//vector of capture handles
	unsigned short capt_cnt;

	snd_pcm_t *pcm_handle;
	snd_pcm_hw_params_t *hw_params;
	snd_pcm_uframes_t bufferSize;
	snd_async_handler_t *ahandler;
	SAsync_private_data data;

	/* Open pcm device */
	void OpenPCM(snd_pcm_t **handle, const char *name, EPCMMode m);
	/* Set hardware parameters */
	void SetHwParams(snd_pcm_t *handle, unsigned int srate);
	/* Add capture handle to vector */
	bool CreateCHandleUnit(const char *name);
	/* Find device in handles vector */
	int FindCDeviceByName(char *name);
	/* Prepare device */
	void PrepareDevice(snd_pcm_t *handle);
	/* Initialize async callback */
	void InitializeCallback();
	/* Write initial chunk to ALSA */
	void WriteToALSA();
	/* Start synchronous playback */
	void SyncPlay(size_t size);
	/* Start asynchrous playback */
	void AsyncPlay();
	/* Start async playback */
	void StartAsync();
	/* Stop async playback */
	void StopAsync();

	int ReadData(snd_pcm_t *handle, int16_t *rdbuf);

	void AvailableFormats(snd_pcm_t *device, snd_pcm_hw_params_t *hw_params);
	void TestRate(snd_pcm_t *device, snd_pcm_hw_params_t *hw_params);

public:
	ALSAWrapper(bool callback = false, unsigned int SR = SAMPLE_RATE, unsigned int NCh = NUM_CHANNELS, short BPS = BYTE_PER_SAMPLE);
	~ALSAWrapper();

	//TODO reinitialize ALSA with new parameters
	bool Initialize(EPCMMode m = SND_PLAYBACK, const char *name = (char *)"default");
	/* Close device handles */
	void CloseDevice(EPCMMode m = SND_PLAYBACK, unsigned int n = 0);
	void CloseCDevice(unsigned int n);
	/* Append wav data to buffer */
	void AppendData(int ch, std::string data);

	int ReceiveData(int16_t *rdbuf, bool right);
	/* Reset async playback */
	void ResetAsync();
	/* Return error code */
	int GetLastResult()				{ return err; }
	/* Return Sample rate */
	unsigned int GetPSampleRate()	{ return pSampleRate; }
	/* Return async or not playback */
	bool IsAsyncCallback()			{ return asyncCallback; }
	void Lock()						{ pthread_mutex_lock(&bufmtx); }
	void Unlock()					{ pthread_mutex_unlock(&bufmtx); }

};

#endif /* ALSAWRAPPER_H_ */
