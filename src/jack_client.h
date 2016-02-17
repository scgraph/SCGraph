#ifndef SCGRAPH_JACK_CLIENT_HH
#define SCGRAPH_JACK_CLIENT_HH

#ifdef HAVE_JACK

#include <jack/jack.h>
#include <vector>
#include <string>
#include <sstream>

#include <assert.h>

#include <fftw3.h>

#include <QtCore/QMutex>

struct JackClient
{
	QMutex                      _mutex;

	jack_client_t              *_client;
	std::vector <jack_port_t*>  _ports;

	std::string                 _jack_name;

	int                         _buffer_size;
	double                      _sample_rate;

	fftwf_plan                  _fft_plan;

	float*                      _window;

	float*                      _fft_in_buffer;

	fftwf_complex*              _fft_out_buffer;

	std::vector <fftwf_complex*> _per_port_output_buffers;

	JackClient (int num_of_ports, const std::string &jack_name);
	~JackClient ();

	void init_window (float *buffer, int size);

	void do_windowing (float *buffer, int size);

	float get_frequency (int port, float freq);

	int process (jack_nframes_t frames);
};

#endif

#endif
