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

#include "ALSAWrapper.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof *(a))

using namespace std;

#ifdef DEBUG_ALSA_CALLBACK_MESS
int k = 0;
#endif


ALSAWrapper::ALSAWrapper(bool callback, unsigned int SR, unsigned int NCh, short BPS)
{
	err = buf_size = bufferSize = 0;
	asyncCallback = callback;
	pSampleRate = SR;
	numChannels = NCh;
	bytePerSample = BPS;
	restart = 1;

#ifdef ALSA_WRAPPER_MESS
	cout << "Type of ALSA wrapper [" << (callback ? " callback ]":" non-callback ]") << endl;
#endif

	pcm_handle = NULL;
	hw_params = NULL;
	ahandler = NULL;

	if (asyncCallback) {
		data.inst = this;
		data.buf = NULL;
		data.cnt = 0;
		data.len = 0;
		data.buf_size = 0;
		data.chnum = NCh;
		data.bytePerS = BPS;
	}
	wavbuf = NULL;
	capt_cnt = 0;

	chandles.reserve(2);

	pthread_mutex_init(&bufmtx,NULL);
}

ALSAWrapper::~ALSAWrapper()
{
	CloseDevice();

	for (int i = 0; i < (int)chandles.size(); ++i)
		CloseDevice(SND_CAPTURE, i);
	chandles.clear();

	pthread_mutex_destroy(&bufmtx);
}

static void async_callback(snd_async_handler_t *ahandler)
{
	snd_pcm_t *handle = snd_async_handler_get_pcm(ahandler);
	struct SAsync_private_data *d = (SAsync_private_data *)snd_async_handler_get_callback_private(ahandler);
	char *buf = d->buf;
	int cnt = d->cnt;
	long len = d->len;
	int bsize = d->buf_size;
	unsigned int chnum = d->chnum;
	short bytePerS = d->bytePerS;
	snd_pcm_sframes_t avail;
	int err = 0;

#ifdef DEBUG_ALSA_CALLBACK_MESS
	fprintf(stderr, "Call Callback %d\n", k++);
#endif

	avail = snd_pcm_avail_update(handle);

#ifdef DEBUG_ALSA_CALLBACK_MESS
	cout << "Avail: " << avail << endl;
#endif

	d->inst->Lock();

	if ((!buf) || (cnt >= len)) goto async_callback_end;
	else {
		if (avail < bsize) bsize = avail;

		if (cnt + bsize >= d->len) {
#ifdef DEBUG_ALSA_CALLBACK_MESS
			cout << "Buffer overrun by " << (cnt + bsize - d->len) << endl;
#endif
			bsize -= cnt + bsize - d->len;

		}
		err = snd_pcm_writei(handle, (buf+cnt), bsize);

		if (err < 0) {
#ifdef ALSA_ERRORS
			cerr << "[DEBUG] Async Callback(): error write (" << snd_strerror(err) << ")" << endl;
#endif
			goto async_callback_end;

		} else if (err == -EPIPE) {
			snd_pcm_prepare(handle);

			goto async_callback_end;
		}

		d->cnt += (chnum*bytePerS*bsize);
	}

async_callback_end:
	d->inst->Unlock();
}


void ALSAWrapper::AvailableFormats(snd_pcm_t *device, snd_pcm_hw_params_t *hw_params)
{
	int i;

	printf("[ ALSAWrapper ] Available formats: ");
	int size = sizeof(formats) / sizeof *(formats);
	for (i = 0; i < size; ++i) {
		if (!snd_pcm_hw_params_test_format(device, hw_params, formats[i]))
			printf("%s ", snd_pcm_format_name(formats[i]));
	}
	printf("\n");
}

void ALSAWrapper::TestRate(snd_pcm_t *device, snd_pcm_hw_params_t *hw_params)
{
	unsigned int minrate, maxrate;
	int size = sizeof(rates) / sizeof *(rates);
	int err = 0;

	if ((err = snd_pcm_hw_params_get_rate_min(hw_params, &minrate, 0)) < 0) {
		fprintf (stderr, "[ DEBUG ] Cannot get min sample rate: %s\n",
				snd_strerror(err));
	}
	if ((err = snd_pcm_hw_params_get_rate_max(hw_params, &maxrate, 0)) < 0) {
		fprintf (stderr, "[ DEBUG ] Cannot get max sample rate: %s\n",
				snd_strerror(err));
	}
	printf("[ ALSAWrapper ] The range of sampling rates: [ %d : %d ]\n", minrate, maxrate);

	printf("[ ALSAWrapper ] Sample rates: ");
	for (int i = 0; i < size; ++i) {
		if (!snd_pcm_hw_params_test_rate(device, hw_params, rates[i], 0)) {
			printf("%u ", rates[i]);
		}
	}
	printf("\n");
}

bool ALSAWrapper::CreateCHandleUnit(const char *nd)
{
	char *name = (char *)nd;
	SDeviceSettings ds;

	for (size_t i = 0; i < chandles.size(); ++i) {
		if (chandles.at(i).name == name) {
			cout << "[ ALSAWrapper ] Device \"" << name << "\" is already registered" << endl;
			return false;
		}
	}
	ds.name = name;
	ds.handle = NULL;

	chandles.push_back(ds);
	capt_cnt++;

	return true;
}

int ALSAWrapper::FindCDeviceByName(char *name)
{
	for (size_t i = 0; i < chandles.size(); ++i)
		if (chandles.at(i).name == name)
			return i;

	return -1;
}

bool ALSAWrapper::Initialize(EPCMMode m, const char *nd)
{
	snd_pcm_t *tmp = NULL;
	unsigned int rate;
	char *name = (char *)nd;
	int i;

	if (*name == '\0') {
		cerr << "[ ALSAWrapper ] Name is empty." << endl;
		return false;
	}
	switch (m) {
	case SND_PLAYBACK:
		rate = pSampleRate;
		break;
	case SND_CAPTURE:
		rate = 16000; //FIXME!!!!
		break;
	default: break;
	}

	OpenPCM(&tmp, name, m);
	if (tmp == NULL || err < 0) return false;

	SetHwParams(tmp, rate);
	if (err < 0) {
		snd_pcm_close(tmp);
		return false;
	}

	switch (m) {
	case SND_PLAYBACK:
		pcm_handle = tmp;
		if (asyncCallback)
			InitializeCallback();
		break;
	case SND_CAPTURE:
		/* Appending object to chandles vector*/
		if (!CreateCHandleUnit(name)) return false;

		i = FindCDeviceByName(name);
		if (i < 0) {
			cerr << "[ ALSAWrapper ] Cannot find " << name << " in handles map." << endl;
			return false;
		}
		chandles.at(i).handle = tmp;

		break;
	default: break;
	}
	return true;
}

void ALSAWrapper::OpenPCM(snd_pcm_t **handle, const char *name, EPCMMode m)
{
	/* Open the PCM device in playback/capture mode */
	if ((err = snd_pcm_open(handle, name, (m ? SND_PCM_STREAM_CAPTURE : SND_PCM_STREAM_PLAYBACK), 0)) < 0) {
#ifdef ALSA_ERRORS
		cerr << "[ DEBUG ] SetHwParams(): cannot open audio device \"" << name << "\" (" << snd_strerror(err) << ")" << endl;
#endif
		return;
	}
	cout << "[ ALSAWrapper ] Device \"" << name << "\" open." << endl;
	cout << "[ ALSAWrapper ] Mode " << (m ? "SND_PCM_CAPTURE" : "SND_PCM_PLAYBACK") << endl;
}

void ALSAWrapper::CloseDevice(EPCMMode m, unsigned int n)
{
	//Close the audio interface
	switch (m) {
	case SND_PLAYBACK:
		/* Close playback handle */
		if (pcm_handle != NULL) {
			err = snd_pcm_close(pcm_handle);
#ifdef ALSA_ERRORS
			if (err < 0) cerr << "[ALSAWrapper] Cannot close pcm handle (" << snd_strerror(err) << ")" << endl;
			pcm_handle = NULL;
#endif
		}
		break;
	case SND_CAPTURE:
		/* Close capture handle */
		if (chandles.empty() || (chandles.size() < n+1) ) {
			cerr << "[ ALSAWrapper ] There is no such element (" << n << "). Number of available elements " << chandles.size() << endl;
			return;
		}
		cout << "[ ALSAWrapper ] Closing capture device...";

		if (chandles.at(n).handle == NULL) return;

		//Close pcm
		err = snd_pcm_close(chandles.at(n).handle);

#ifdef ALSA_ERRORS
		if (err < 0) cerr << "\n[ALSAWrapper] Cannot close pcm handle (" << chandles.at(n).name << ") (" << snd_strerror(err) << ")" << endl;
#endif

		//Remove unit from map
		chandles.at(n).handle = NULL;
		chandles.at(n).name = NULL;
		if (err < 0) cout << "FALSE." << endl;
		else cout << "OK." << endl;

		break;
	default: break;
	}
}

void ALSAWrapper::CloseCDevice(unsigned int n)
{
	if (chandles.empty() || (chandles.size() < n+1) ) {
		cerr << "[ ALSAWrapper ] There is no such element (" << n << "). Number of available elements " << chandles.size() << endl;
		return;
	}
	cout << "[ ALSAWrapper ] Closing capture device (" << chandles.at(n).name << ")...";
	if (chandles.at(n).handle == NULL) return;
	err = snd_pcm_close(chandles.at(n).handle);
	if (err < 0) cerr << "\n[ALSAWrapper] Cannot close pcm handle (" << chandles.at(n).name << ") (" << snd_strerror(err) << ")" << endl;
	else cout << "OK." << endl;

	chandles.at(n).handle = NULL;
	chandles.at(n).name = NULL;
	if (--capt_cnt == 0) chandles.clear();
}

void ALSAWrapper::StartAsync()
{
	if (asyncCallback)
		InitializeCallback();
}

void ALSAWrapper::StopAsync()
{
	if (asyncCallback) {
		err = snd_pcm_drop(pcm_handle);
#ifdef ALSA_ERRORS
		if (err < 0) cerr << "[ALSAWrapper] Cannot drop handle (" << snd_strerror(err) << ")" << endl;
#endif

		err = snd_async_del_handler(ahandler);
#ifdef ALSA_ERRORS
		if (err < 0) cerr << "[ALSAWrapper] Cannot delete async handle (" << snd_strerror(err) << ")" << endl;
#endif
		ahandler = NULL;
	}
}

void ALSAWrapper::ResetAsync()
{
	StopAsync();
	usleep(ALSA_RESET_TIMEOUT);
	StartAsync();
	usleep(ALSA_RESET_TIMEOUT);
}

void ALSAWrapper::SetHwParams(snd_pcm_t *handle, unsigned int srate)
{
	snd_pcm_hw_params_t *params;

	/* Allocate parameters object and fill it with default values */
	if ((err = snd_pcm_hw_params_malloc(&params)) < 0) {
#ifdef ALSA_ERRORS
		cerr << "[ DEBUG ] SetHwParams(): cannot allocate hardware parameter structure (" << snd_strerror(err) << ")" << endl;
#endif
		return;
	}

	if ((err = snd_pcm_hw_params_any(handle, params)) < 0) {
#ifdef ALSA_ERRORS
		cerr << "[ DEBUG ] SetHwParams(): cannot initialize hardware parameter structure (" << snd_strerror(err) << ")" << endl;
#endif
		return;
	}

	/* Set parameters */
	if ((err = snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
#ifdef ALSA_ERRORS
		cerr << "[ DEBUG ] SetHwParams(): cannot set acces type (" << snd_strerror(err) << ")" << endl;
#endif
		return;
	}

	AvailableFormats(handle, params);
	TestRate(handle, params);

	if ((err = snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE)) < 0) {
#ifdef ALSA_ERRORS
		cerr << "[ DEBUG ] SetHwParams(): cannot set sample format (" << snd_strerror(err) << ")" << endl;
#endif
		return;
	}

	if ((err = snd_pcm_hw_params_set_rate_near (handle, params, &srate, 0)) < 0) {
#ifdef ALSA_ERRORS
		cerr << "[ DEBUG ] SetHwParams(): cannot set sample rate (" << snd_strerror(err) << ")" << endl;
#endif
		return;
	}

	if ((err = snd_pcm_hw_params_set_channels(handle, params, numChannels)) < 0) {
#ifdef ALSA_ERRORS
		cerr << "[ DEBUG ] SetHwParams(): cannot set channel count (" << snd_strerror(err) << ")" << endl;
#endif
		return;
	}

	/* write the parameters to device */
	if ((err = snd_pcm_hw_params (handle, params)) < 0) {
#ifdef ALSA_ERRORS
		cerr << "[ DEBUG ] SetHwParams(): cannot set parameters (" << snd_strerror(err) << ")" << endl;
#endif
		return;
	}

	// Free memory for the hardware parameter struct
	snd_pcm_hw_params_free (params);

	/* Prepare the device */
	PrepareDevice(handle);

	if ((err = snd_pcm_hw_params_get_period_size(params, &bufferSize, NULL))< 0) {
#ifdef ALSA_ERRORS
		cerr << "[ DEBUG ] SetHwParams(): Unable to get period size for playback: (" << snd_strerror(err) << ")" << endl;
#endif
		return;
	}
	buf_size = bufferSize * numChannels * bytePerSample;
	if (asyncCallback) data.buf_size = buf_size;

	unsigned int tmp2 = 0;
	if ((err = snd_pcm_hw_params_get_rate(params, &tmp2, 0)) < 0)
		cerr <<
		fprintf (stderr, "[ DEBUG ] SetHwParams(): Cannot get sample rate: %s\n",
				snd_strerror(err));
	printf("[ ALSAWrapper ] Current sample rate: %d\n", tmp2);

}

void ALSAWrapper::PrepareDevice(snd_pcm_t *handle)
{
	if ((err = snd_pcm_prepare (handle)) < 0) {
#ifdef ALSA_ERRORS
		cerr << "[ DEBUG ] SetHwParams(): cannot prepare audio interface for use (" << snd_strerror(err) << ")" << endl;
#endif
		return;
	}
#ifdef DEBUG_ALSA
	cout << "[ DEBUG ]: Audio device has been prepared for use." << endl;
#endif
}

void ALSAWrapper::InitializeCallback()
{
	/* Initialize callback */
	if ((err = snd_async_add_pcm_handler(&ahandler, pcm_handle, async_callback, &data)) < 0) {
#ifdef ALSA_ERRORS
		cerr << "[ DEBUG ] InitializeCallback(): Unable to register async handler (" << snd_strerror (err) << ")" << endl;
#endif
		return;
	}

	/* Start (Enabling) the Callback */
#ifdef DEBUG_ALSA
	cout << "[ALSAWrapper]: Snd_pcm start...";
#endif
	if (snd_pcm_state(pcm_handle) == SND_PCM_STATE_PREPARED) {
		if ((err = snd_pcm_start(pcm_handle)) < 0) {
#ifdef ALSA_ERRORS
			cerr << "[ DEBUG ] InitializeCallback(): Start error: " << snd_strerror(err) << endl;
#endif
			return;
		}
	}
#ifdef DEBUG_ALSA
	cout << "[ OK ]" << endl;
#endif

	WriteToALSA();
}

void ALSAWrapper::WriteToALSA()
{
	if (!pcm_handle) return;

#ifdef DEBUG_ALSA
	cout << "[DEBUG] [ALSAWrapper]: Write initial chunk...";
#endif

	char *tmp = (char *)malloc(buf_size*sizeof(char));
	if (!tmp) {
		cerr << "Cannot alocate memory for initial buffer" << endl;
		return;
	}
	memset(tmp, 0, buf_size);

	/* Write initial chunk */
	for (int i = 0; i < 1; ++i) {
		if ((err = snd_pcm_writei (pcm_handle, tmp, buf_size)) == -EPIPE) { //SIZE_INIT_BUFFER
#ifdef ALSA_ERRORS
			cerr << "something going wrong... (" << snd_strerror(err) << ") ";
#endif
			snd_pcm_prepare(pcm_handle);
		} else if (err < 0) {
#ifdef ALSA_ERRORS
			cerr << "write to audio interface failed (" << snd_strerror (err) << ")" << endl;
#endif
			return;
		}
	}
	free(tmp);

#ifdef DEBUG_ALSA
	cout << "[ OK ]" << endl;
#endif
}

int ALSAWrapper::ReceiveData(int16_t *rdbuf, bool right)
{
	int k;
	if (chandles.empty()) return 0;

	k = (right ? ((capt_cnt > 1) ? 1 : 0 ) : 0);

	return ReadData(chandles.at(k).handle, rdbuf);
}

int ALSAWrapper::ReadData(snd_pcm_t *handle, int16_t *rdbuf)
{
	int frame_size, frames, inframes;

	if (restart) {
		/* Drop any output we might got and stop */
		snd_pcm_drop(handle);

		restart = 0;

		/* Prepare for use */
		snd_pcm_prepare(handle);
	}

	frame_size = 1 * (16 / 8);
	frames = buf_size / frame_size;

	memset (rdbuf, 0, frames);

	while ((inframes = snd_pcm_readi(handle, rdbuf, frames)) < 0) {
		if (inframes == -EAGAIN)
			continue;

		fprintf(stderr, "Input buffer overrun\n");
		restart = 1;
		snd_pcm_prepare(handle);
	}
	if (inframes != frames)
		fprintf(stderr, "Short read from capture device: %d, expecting %d\n", inframes, frames);
	return inframes;
}


void ALSAWrapper::AppendData(int ch, string dat)
{
	if (!pcm_handle) return;

	if (asyncCallback) { // && !first) {
		snd_pcm_drop(pcm_handle);
	}

	Lock();

#ifdef DEBUG_ALSA
	cout << "[ Append Data ]: Reallocating.... ";
#endif

	wavbuf = (char*)realloc(wavbuf,dat.size());

#ifdef DEBUG_ALSA
	cout << "done!" << endl;
#endif

	if (!wavbuf) {
#ifdef DEBUG_ALSA
		cerr << "[ Append Data ]: Error allocate memory. Return." << endl;
#endif
		Unlock();
		return;
	}

	/* Copy wav data to buffer */
	copy(dat.begin(), dat.end(), wavbuf);

	if (asyncCallback) {
		data.buf = wavbuf;
		data.cnt = 0;
		data.len = dat.size();
		Unlock();
		AsyncPlay();
	} else {
		ResetAsync();
		SyncPlay(dat.size());
		if (wavbuf) {
			free(wavbuf);
			wavbuf = NULL;
		}
		Unlock();
	}
}

void ALSAWrapper::AsyncPlay()
{
	if (!pcm_handle) return;

	if (!wavbuf) {
#ifdef DEBUG_ALSA
		cerr << "[ALSAWrapper] No buffer for async callback!" << endl;
#endif
		return;
	}
	snd_pcm_drop(pcm_handle);
	//prepare the device
	err = snd_pcm_prepare(pcm_handle);
#ifdef ALSA_ERRORS
	if (err < 0) cerr << "Prepare error:" <<  snd_strerror(err) << endl;
#endif
}

void ALSAWrapper::SyncPlay(size_t size)
{
	if (err < 0) {
#ifdef DEBUG_ALSA
		cerr << "[ALSAWrapper] Ooops, ASLA is not started..." << endl;
#endif
		return;
	}
	int number_chunks = size / buf_size;
	if (!wavbuf) {
#ifdef DEBUG_ALSA
		cerr << "[ALSAWrapper] (Sync play) No buffer with wav data" << endl;
#endif
		return;
	}
	for (int i = 0; i < number_chunks; ++i) {
		if ((err = snd_pcm_writei (pcm_handle, (wavbuf+(i*buf_size)), bufferSize)) == -EPIPE) {
			snd_pcm_prepare(pcm_handle);
		} else if (err < 0) {
#ifdef ALSA_ERRORS
			cerr << "[ALSAWrapper] (Sync play) write to audio interface failed (" << snd_strerror (err) << ")" << endl;;
#endif
			return;
		}
//		printf("Write data to device...[%d/%d]\033[1A\n", i, number_chunks);
//		fflush(stdout);
	}
}
