#ifndef SCGRAPH_OSC_HANDLER_CC
#define SCGRAPH_OSC_HANDLER_CC

//TODO: 
// - fix oscpack/scgraph to not alloc mem.on incoming osc message
// - implement wildcard support for d_load

#include <pthread.h>
#include <osc/OscPacketListener.h>
#include <ip/UdpSocket.h>

#include <vector>

// FIXME: remove
#include <iostream>

#include "ofEvent.h"
#include "ofThread.h"

/* if you modify this, do the same to the command_name_to_int () method */
enum {
	cmd_none = 0,

	cmd_notify = 1,
	cmd_status = 2,
	cmd_quit = 3,
	cmd_cmd = 4,

	cmd_d_recv = 5,
	cmd_d_load = 6,
	cmd_d_loadDir = 7,
	cmd_d_freeAll = 8,

	cmd_s_new = 9,
	
	cmd_n_trace = 10,
	cmd_n_free = 11,
	cmd_n_run = 12,
	cmd_n_cmd = 13,
	cmd_n_map = 14,
	cmd_n_set = 15,
	cmd_n_setn = 16,
	cmd_n_fill = 17,
	cmd_n_before = 18,
	cmd_n_after = 19,

	cmd_u_cmd = 20,

	cmd_g_new = 21,
	cmd_g_head = 22,
	cmd_g_tail = 23,
	cmd_g_freeAll = 24,
	
	cmd_c_set = 25,
	cmd_c_setn = 26,
	cmd_c_fill = 27,

	cmd_b_alloc = 28,
	cmd_b_allocRead = 29,
	cmd_b_read = 30,
	cmd_b_write = 31,
	cmd_b_free = 32,
	cmd_b_close = 33,
	cmd_b_zero = 34,
	cmd_b_set = 35,
	cmd_b_setn = 36,
	cmd_b_fill = 37,
	cmd_b_gen = 38,
	
	cmd_dumpOSC = 39,

	cmd_c_get = 40,
	cmd_c_getn = 41,
	cmd_b_get = 42,
	cmd_b_getn = 43,
	cmd_s_get = 44,
	cmd_s_getn = 45,
	
	cmd_n_query = 46,
	cmd_b_query = 47,

	cmd_n_mapn = 48,
	cmd_s_noid = 49,

	cmd_g_deepFree = 50,
	cmd_clearSched = 51,

	cmd_sync = 52,

	cmd_d_free = 53,

	cmd_foo = 54,
	cmd_verbose = 55,
	cmd_graphicsRate = 56,
	cmd_controlRate = 57,
	cmd_loadShaderProgram = 58,
	cmd_clearShaderPrograms = 59,

	cmd_addString = 60,
	cmd_changeString = 61,
	cmd_setFont = 62,

	NUMBER_OF_COMMANDS = 61
};

class OscMessage //: public QObject
{
	//Q_OBJECT
	public:
		OscMessage (const osc::ReceivedMessage& msg,  const osc::IpEndpointName &endpoint_name) :
			_msg (msg),
			_endpoint_name (endpoint_name) 
		{

		};

		osc::ReceivedMessage _msg;
		osc::IpEndpointName       _endpoint_name;
};	

// FIXME: bad style
#define SCGRAPH_OSC_MESSAGE_BUFFER_SIZE 15360

class OscHandler : public osc::OscPacketListener, public ofThread
{
	//Q_OBJECT

	osc::UdpListeningReceiveSocket   *_socket;
	char                         _message_buffer [SCGRAPH_OSC_MESSAGE_BUFFER_SIZE];

	/** a list of clients wishing to receive notifications by the server */
	std::vector<osc::IpEndpointName>  _notifications;
	
	// TODO ofThread                       _condition_mutex;
    /*
	QWaitCondition               _condition;
	bool                         _handling_done;
	*/

	private:
		void ProcessMessage (const osc::ReceivedMessage& message, const osc::IpEndpointName& name);
		// void ProcessBundle (const osc::ReceivedBundle& b, const IpEndpointName& remoteEndpoint);

		int command_name_to_int (const std::string& command_name);

		/* used to send notifications to clients */
		void send_notifications (std::string path, int id);

		void send_done (std::string command, osc::IpEndpointName endpoint);
		/* inherited from QThread */
		void run ();
    

	public:
		OscHandler ();
		~OscHandler ();

		void stop ();
        void threadedFunction();

	/* slots to pass messages from the receiving thread to the QMain thread */
	//public slots:
		void handle_message (OscMessage *message);
		void handle_message_locked (OscMessage *message);
	//signals:
		ofEvent<OscMessage> message_received;
};

#endif