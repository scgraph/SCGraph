#include "osc_handler.h"
#include "scgraph.h"

#include "shader_pool.h"
#include "texture_pool.h"
#include "string_pool.h"

#include <iostream>
#include <stdexcept>
#include <sstream>

#include <osc/OscReceivedElements.h>
#include <osc/OscOutboundPacketStream.h>

#include <algorithm>

#include <QtCore/QMetaType>
#include <QtCore/QReadLocker>
#include <QtCore/QWriteLocker>

OscHandler::OscHandler () :
	_notifications (false),
	_mutex (QMutex::Recursive)
{
	Options *options = Options::get_instance ();

	// pthread_mutex_init (&_mutex, 0);

	// queued connection is nessecary to get the msg across thread boundaries
	QObject::connect (this, SIGNAL (message_received(OscMessage*)), this, SLOT (handle_message(OscMessage*)), Qt::QueuedConnection);

	try
	{
		_socket = new UdpListeningReceiveSocket (IpEndpointName (IpEndpointName::ANY_ADDRESS, options->_port), this);
	}
	catch (std::runtime_error e)
	{
		std::cout << "[OscHandler]: Error: Couldn't create socket. Reason: " << e.what () << std::endl;
		std::exit (EXIT_FAILURE);
	}

	// start ();
}

OscHandler::~OscHandler ()
{
	stop();
	delete _socket;
}

void OscHandler::run ()
{
	Options *options = Options::get_instance ();

	bool done = false;

	while (!done)
	{
		// TODO: Fix this madness
		if (options->_verbose >= 2)
			std::cout << "[OscHandler]: Running!" << std::endl;

		try 
		{
			std::cout << "." << std::endl;
	
			_socket->Run ();
			std::cout << "......" << std::endl;
			done = true;
		}
		catch (std::runtime_error e)
		{
			std::cout << "[OscHandler]: Something went wrong: " << e.what () << std::endl;
			std::exit (EXIT_FAILURE);
		}
		catch (osc::MalformedMessageException e)
		{
			std::cout << "[OscHandler]: Error: Malformed message: " << e.what () << std::endl;
		}
		catch (osc::MalformedBundleException e)
		{
			std::cout << "[OscHandler]: Error: Malformed message: " << e.what () << std::endl;
		}
	}
	// std::cout << "fini" << std::endl;
}

void OscHandler::stop ()
{
	_mutex.lock ();

	_socket->AsynchronousBreak ();
	// std::cout << "break" << std::endl;

	_mutex.unlock ();
	// std::cout << "unlock" << std::endl;

	QThread::wait ();
	// std::cout << "wait" << std::endl;

}

void OscHandler::ProcessMessage (const osc::ReceivedMessage& message, const IpEndpointName& name)
{
	// std::cout << "message!" << std::endl;
	OscMessage *msg = new OscMessage (message, name);

	// std::cout << "path: " << message.AddressPattern () << std::endl;
	// we use queued connection to get the message in the main qt thread context
	//_condition_mutex.lock ();
	//_handling_done = false;

	emit (message_received(msg));
}

void OscHandler::send_notifications (std::string path, int id)
{
	if (_notifications.size () == 0)
		return;

	ScGraph *scgraph = ScGraph::get_instance ();

	Tree::Node<NodePtr> *node = scgraph->_node_tree.find_node_by_id (id);

	if (!node)
		throw ("[OscHandler]: send_notifications(): Warning: ID doesn't exist");

	int is_group = 0;
	if (dynamic_cast<GGroup*> (node->_payload.get()))
		is_group = 1;

	osc::OutboundPacketStream p (_message_buffer, 1536);

	try
	{
		p << osc::BeginMessage (path.c_str())	<< id << node->_parent->_payload->_id << -1 << -1 << is_group;
		if (is_group)
			p << -1 << -1; 

		p << osc::EndMessage;

		for (std::vector<IpEndpointName>::iterator it = _notifications.begin (); it != _notifications.end (); ++it)
		{
			_socket->SendTo ((*it), p.Data (), p.Size ());
		}
	}
	catch (osc::Exception &e)
	{
		std::cout << "[OscHandler]: Error: " << e.what () << std::endl;
	}

}


int OscHandler::command_name_to_int (const std::string& command_name)
{
	if (command_name == std::string("/notify"))
		return cmd_notify;
	if (command_name == std::string("/status"))
		return cmd_status;
	if (command_name == std::string("/quit"))
		return cmd_quit;
	if (command_name == std::string("/cmd"))
		return cmd_cmd;
	if (command_name == std::string("/d_recv"))
		return cmd_d_recv;
	if (command_name == std::string("/d_load"))
		return cmd_d_load;
	if (command_name == std::string("/d_loadDir"))
		return cmd_d_loadDir;
	if (command_name == std::string("/d_freeAll"))
		return cmd_d_freeAll;
	if (command_name == std::string("/s_new"))
		return cmd_s_new;
	if (command_name == std::string("/n_trace"))
		return cmd_n_trace;
	if (command_name == std::string("/n_free"))
		return cmd_n_free;
	if (command_name == std::string("/n_run"))
		return cmd_n_run;
	if (command_name == std::string("/n_cmd"))
		return cmd_n_cmd;
	if (command_name == std::string("/n_map"))
		return cmd_n_map;
	if (command_name == std::string("/n_set"))
		return cmd_n_set;
	if (command_name == std::string("/n_setn"))
		return cmd_n_setn;
	if (command_name == std::string("/n_fill"))
		return cmd_n_fill;
	if (command_name == std::string("/n_before"))
		return cmd_n_before;
	if (command_name == std::string("/n_after"))
		return cmd_n_after;
	if (command_name == std::string("/u_cmd"))
		return cmd_u_cmd;
	if (command_name == std::string("/g_new"))
		return cmd_g_new;
	if (command_name == std::string("/g_head"))
		return cmd_g_head;
	if (command_name == std::string("/g_tail"))
		return cmd_g_tail;
	if (command_name == std::string("/g_freeAll"))
		return cmd_g_freeAll;
	if (command_name == std::string("/c_set"))
		return cmd_c_set;
	if (command_name == std::string("/c_setn"))
		return cmd_c_setn;
	if (command_name == std::string("/c_fill"))
		return cmd_c_fill;
	if (command_name == std::string("/b_alloc"))
		return cmd_b_alloc;
	if (command_name == std::string("/b_allocRead"))
		return cmd_b_allocRead;
	if (command_name == std::string("/b_read"))
		return cmd_b_read;
	if (command_name == std::string("/b_write"))
		return cmd_b_write;
	if (command_name == std::string("/b_free"))
		return cmd_b_free;
	if (command_name == std::string("/b_close"))
		return cmd_b_close;
	if (command_name == std::string("/b_zero"))
		return cmd_b_zero;
	if (command_name == std::string("/b_set"))
		return cmd_b_set;
	if (command_name == std::string("/b_setn"))
		return cmd_b_setn;
	if (command_name == std::string("/b_fill"))
		return cmd_b_fill;
	if (command_name == std::string("/b_gen"))
		return cmd_b_gen;
	if (command_name == std::string("/dumpOSC"))
		return cmd_dumpOSC;
	if (command_name == std::string("/c_get"))
		return cmd_c_get;
	if (command_name == std::string("/c_getn"))
		return cmd_c_getn;
	if (command_name == std::string("/b_get"))
		return cmd_b_get;
	if (command_name == std::string("/b_getn"))
		return cmd_b_getn;
	if (command_name == std::string("/s_get"))
		return cmd_s_get;
	if (command_name == std::string("/s_getn"))
		return cmd_s_getn;
	if (command_name == std::string("/n_query"))
		return cmd_n_query;
	if (command_name == std::string("/b_query"))
		return cmd_b_query;
	if (command_name == std::string("/n_mapn"))
		return cmd_n_mapn;
	if (command_name == std::string("/s_noid"))
		return cmd_s_noid;
	if (command_name == std::string("/g_deepFree"))
		return cmd_g_deepFree;
	if (command_name == std::string("/clearSched"))
		return cmd_clearSched;
	if (command_name == std::string("/sync"))
		return cmd_sync;
	if (command_name == std::string("/d_free"))
		return cmd_d_free;
	if (command_name == std::string("/foo"))
		return cmd_foo;
	if (command_name == std::string("/verbose"))
		return cmd_verbose;
	if (command_name == std::string("/graphicsRate"))
		return cmd_graphicsRate;
	if (command_name == std::string("/controlRate"))
		return cmd_controlRate;
	if (command_name == std::string("/loadTexture"))
		return cmd_loadTexture;
	if (command_name == std::string("/loadTextureDir"))
		return cmd_loadTextureDir;
	if (command_name == std::string("/loadShaderProgram"))
		return cmd_loadShaderProgram;
	if (command_name == std::string("/clearShaderPrograms"))
		return cmd_clearShaderPrograms;
	if (command_name == std::string("/addString"))
		return cmd_addString;
	if (command_name == std::string("/changeString"))
		return cmd_changeString;
	if (command_name == std::string("/setFont"))
		return cmd_setFont;

	return cmd_none;
}


// this slot is called in the context of the qapplication object due to the queued
// connection
void OscHandler::handle_message_locked (OscMessage *msg)
{
	// std::cout << "handle message lcoked" << std::endl;

	// std::cout << "Message received. Path: " << msg->_msg.AddressPattern () << std::endl;
	// std::cout << "Message received. Path: " << std::endl;

	osc::ReceivedMessage *message = &(msg->_msg);

	Options *options = Options::get_instance ();
	ScGraph *scgraph = ScGraph::get_instance ();

	int command_num;
	if (message->AddressPatternIsUInt32 ())
	{
		command_num = message->AddressPatternAsUInt32 ();
		if (options->_verbose >= 4)
		{
			std::cout << "[OscHandler]: Message received. Command num: " << message->AddressPatternAsUInt32 () << ", # of args: " << message->ArgumentCount () << std::endl; // ", Types: " << message->TypeTags () << std::endl;
		}
	}
	else
	{
		command_num = command_name_to_int (message->AddressPattern ());
		if (options->_verbose >= 4)
		{
			std::cout << "[OscHandler]: Message received. Path: " << message->AddressPattern () << ", # of args: " << message->ArgumentCount () << std::endl; // ", Types: " << message->TypeTags () << std::endl;
		}
	}

	switch (command_num)
	{
		case cmd_quit:
		{
			QReadLocker locker (&scgraph->_read_write_lock);
			osc::OutboundPacketStream p (_message_buffer, SCGRAPH_OSC_MESSAGE_BUFFER_SIZE);

			try
			{
				p << osc::BeginMessage ("/done") 
				  << "/quit"
				  << osc::EndMessage;
	
				_socket->SendTo (msg->_endpoint_name, p.Data (), p.Size ());
			}
			catch (osc::Exception &e)
			{
				std::cout << "[OscHandler]: Error: " << e.what () << std::endl;
			}
			QApplication::instance()->exit ();
			// we are done anyways
			//pthread_mutex_unlock (&_mutex);
			std::cout << "[OscHandler]: cmd_quit()" << std::endl;
			// return;
		}
		break;

		case cmd_setFont:
			{
				osc::ReceivedMessage::const_iterator arg = message->ArgumentsBegin();
	
				const char *str = 0;
				try
					{
						str = (*(arg++)).AsString ();

						std::string tmp (str);
						if (options->_verbose >= 2)
							std::cout << "[OscHandler]: /setFont " << tmp << std::endl;

						QReadLocker locker (&scgraph->_read_write_lock);
						StringPool::get_instance()->set_font(tmp);
						//send_notifications ("/n_go", synth->_id);
	
					}
				catch (const char* error)
					{
						if (str)
							std::cout << "[OscHandler]: Warning: Set font failed: (font: \"" << str << "\"). Reason: " << error << std::endl;
					}
				catch (osc::Exception &e)
					{
						std::cout << "[OscHandler]: Error while parsing message: /setFont: " << e.what () << ". TypeTags: " << message->TypeTags() << std::endl;
					}
				//scgraph->unlock ();
			}
			
		break;

		case cmd_changeString:
			{
				osc::ReceivedMessage::const_iterator arg = message->ArgumentsBegin();
	
				const char *str = 0;
				osc::int32 index;
				try
					{

						index = (arg++)->AsInt32();
						str = (*(arg++)).AsString ();

						std::string tmp (str);
						if (options->_verbose >= 2)
							std::cout << "[OscHandler]: /changeString " << tmp << std::endl;

						QReadLocker locker (&scgraph->_read_write_lock);
						StringPool::get_instance()->change_string(tmp, index);
						//send_notifications ("/n_go", synth->_id);
	
					}
				catch (const char* error)
					{
						if (str)
							std::cout << "[OscHandler]: Warning: String changing failed: (string: \"" << str << "\"). Reason: " << error << std::endl;
					}
				catch (osc::Exception &e)
					{
						std::cout << "[OscHandler]: Error while parsing message: /changeString: " << e.what () << ". TypeTags: " << message->TypeTags() << std::endl;
					}
				//scgraph->unlock ();
			}
			
		break;

		case cmd_addString:
			{
				osc::ReceivedMessage::const_iterator arg = message->ArgumentsBegin();
	
				const char *str = 0;
				try
					{
						str = (*(arg++)).AsString ();

						std::string tmp (str);
						if (options->_verbose >= 2)
							std::cout << "[OscHandler]: /addString " << tmp << std::endl;

						QReadLocker locker (&scgraph->_read_write_lock);
						StringPool::get_instance()->add_string(tmp, -1);
						//send_notifications ("/n_go", synth->_id);
	
					}
				catch (const char* error)
					{
						if (str)
							std::cout << "[OscHandler]: Warning: String adding failed: (string: \"" << str << "\"). Reason: " << error << std::endl;
					}
				catch (osc::Exception &e)
					{
						std::cout << "[OscHandler]: Error while parsing message: /addString: " << e.what () << ". TypeTags: " << message->TypeTags() << std::endl;
					}
				//scgraph->unlock ();
			}
			
		break;

		case cmd_clearShaderPrograms:
		{
			ShaderPool::get_instance()->clear_shader_programs();
		}
		break;

		case cmd_loadShaderProgram:
		{

			osc::ReceivedMessage::const_iterator arg = message->ArgumentsBegin();

			std::cout << "[OscHandler]: cmd_loadShaderProgram()" << std::endl;

			// false = FragmentShader
	
			try
			{
				QWriteLocker locker (&scgraph->_read_write_lock);

				boost::shared_ptr<ShaderPool::ShaderProgram> p(new ShaderPool::ShaderProgram);

				int index = (arg++)->AsInt32();

				std::stringstream attributes_stream;
				attributes_stream.str((arg++)->AsString());
				std::string attribute;

				while((attribute = "", (attributes_stream >> attribute)), attribute != "")
				{
					p->_uniforms.push_back(attribute);
					std::cout << "[OscHandler]: add attribute: \"" << attribute << "\"" << std::endl;
				}
#if 0
				attributes_stream >> attribute;

				while(attributes_stream.good() || attributes_stream.eof()) {
					p->_attributes.push_back(attribute);
					if (attributes_stream.eof()) break;
					std::cout << "[OscHandler]: add attribute: \"" << attribute << "\"" << std::endl;
					attributes_stream >> attribute;
				}
#endif				

				while(arg != message->ArgumentsEnd())
				{
					std::string program = (arg++)->AsString();
					bool isVertexShader = (arg++)->AsInt32();


					std::pair<std::string, int> shader;

					if (isVertexShader)
						shader.second = ShaderPool::ShaderProgram::VERTEX;
					else
						shader.second = ShaderPool::ShaderProgram::FRAGMENT;

					shader.first = program;
					p->_shaders.push_back(shader);
					std::cout << "[OscHandler]: Program: " << std::endl << program << std::endl;
				}

				ShaderPool::get_instance()->add_shader_program(index, p);

			}
			catch (osc::Exception &e)
			{
				std::cout << "[OscHandler]: Error while parsing message: /loadShaderProgram | /dumpOSC: " << e.what () << std::endl;
			}
		}
		break;

		case cmd_graphicsRate:
		{
			osc::ReceivedMessageArgumentStream args = message->ArgumentStream();
	
			osc::int32 graphics_rate;
	
			try
			{
				args >> graphics_rate;
				std::cout << "[OscHandler]: Setting graphics rate to: " << graphics_rate << std::endl;

				QWriteLocker locker (&scgraph->_read_write_lock);

				scgraph->_graphic_loop.set_rate (graphics_rate);
			}
			catch (osc::Exception &e)
			{
				std::cout << "[OscHandler]: Error while parsing message: /graphicsRate | /dumpOSC: " << e.what () << std::endl;
			}
		}
		break;

		case cmd_controlRate:
		{
			osc::ReceivedMessageArgumentStream args = message->ArgumentStream();
	
			osc::int32 control_rate;
	
			try
			{
				args >> control_rate;
				std::cout << "[OscHandler]: Setting control rate to: " << control_rate << std::endl;

				QWriteLocker locker (&scgraph->_read_write_lock);

				scgraph->_control_loop.set_rate (control_rate);
			}
			catch (osc::Exception &e)
			{
				std::cout << "[OscHandler]: Error while parsing message: /verbose | /dumpOSC: " << e.what () << std::endl;
			}
		}
		break;

		case cmd_dumpOSC:
		case cmd_verbose:
		{
			osc::ReceivedMessageArgumentStream args = message->ArgumentStream();
	
			osc::int32 verbose_level;
	
			try
			{
				args >> verbose_level;
				std::cout << "[OscHandler]: Setting verbose level to: " << verbose_level << std::endl;

				QWriteLocker locker (&scgraph->_read_write_lock);

				options->_verbose = verbose_level;
			}
			catch (osc::Exception &e)
			{
				std::cout << "[OscHandler]: Error while parsing message: /verbose | /dumpOSC: " << e.what () << std::endl;
			}
		}
		break;

		case cmd_d_free:
		{
			osc::ReceivedMessage::const_iterator arg = message->ArgumentsBegin();

			while (arg != message->ArgumentsEnd ())
			{
				try
				{
					std::string name = (arg++)->AsString();

					QWriteLocker locker (&scgraph->_read_write_lock);

					scgraph->_synthdef_pool.d_free (name);
				}
				catch (osc::Exception &e)
				{
					std::cout << "[OscHandler]: Error while parsing message: /d_free: " << e.what () << std::endl;
				}
				catch (const char *e)
				{
					std::cout << "[OscHandler]: Error while parsing message /d_free (): " << e << std::endl;
				}
			}
		}
		break;

		case cmd_d_freeAll:
		{
			QWriteLocker locker (&scgraph->_read_write_lock);

			scgraph->_synthdef_pool.d_freeAll ();
		}
		break;

		case cmd_sync:
		{
			QReadLocker locker (&scgraph->_read_write_lock);

			osc::ReceivedMessageArgumentStream args = message->ArgumentStream();
	
			osc::int32 command_id = 0;
	
			try
			{
				args >> command_id;
			}
			catch (osc::Exception &e)
			{
				std::cout << "[OscHandler]: Error while parsing message: /sync: " << e.what () << std::endl;
			}
	
			osc::OutboundPacketStream p (_message_buffer, SCGRAPH_OSC_MESSAGE_BUFFER_SIZE);
	
			p << osc::BeginBundleImmediate << osc::BeginMessage ("/synced") << command_id
			<< osc::EndMessage << osc::EndBundle;
	
			try
			{
				_socket->SendTo (msg->_endpoint_name, p.Data (), p.Size ());
			}
			catch (osc::Exception &e)
			{
				std::cout << "[OscHandler]: Error while parsing message: /sync: " << e.what () << std::endl;
			}
		}
		break;


		case cmd_notify:
		{
			QWriteLocker locker (&scgraph->_read_write_lock);

			if (message->ArgumentCount () != 1)
			{
				if (options->_verbose)
					std::cout << "[OscHandler]: Warning: /notify received without argument..." << std::endl;
				
				break;
			}
	
			osc::int32 on_off;
	
			osc::ReceivedMessageArgumentStream args = message->ArgumentStream();
	
			args >> on_off;
	
			if (std::find (_notifications.begin (), _notifications.end (), msg->_endpoint_name) != _notifications.end ())
			{
				if (!on_off)
				{
					_notifications.erase(std::remove (_notifications.begin (), _notifications.end (), msg->_endpoint_name), _notifications.end());
				}
			}
			else
			{
				if (on_off)
					_notifications.push_back (msg->_endpoint_name);
			}
	
			// _socket->Connect (msg->_endpoint_name);
			osc::OutboundPacketStream p (_message_buffer, SCGRAPH_OSC_MESSAGE_BUFFER_SIZE);
	
			p << osc::BeginBundleImmediate 
			  << osc::BeginMessage ("/done")
			<< osc::EndMessage << osc::EndBundle;
	
			_socket->SendTo (msg->_endpoint_name, p.Data (), p.Size ());
	
			if (options->_verbose >= 2)
			{
				std::cout << "[OscHandler]: Notification list changed [" << _notifications.size () << " item(s)]:" << std::endl;
				for (unsigned int i = 0; i < _notifications.size (); ++i)
				{
					_notifications[i].AddressAndPortAsString (_message_buffer);
					std::cout << i << ": " << _message_buffer << std::endl;
				}
			}
		}
		break;

		case cmd_status:
		{
			QReadLocker locker (&scgraph->_read_write_lock);

			osc::OutboundPacketStream p (_message_buffer, SCGRAPH_OSC_MESSAGE_BUFFER_SIZE);

			try
			{
				p << osc::BeginMessage ("/status.reply")	<< 1 << 2 << 2 << 2 << 10 << (float)0.1 << (float)0.1 << (double)48000.0 << (double)48000.0	<< osc::EndMessage;
	
				_socket->SendTo (msg->_endpoint_name, p.Data (), p.Size ());
			}
			catch (osc::Exception &e)
			{
				std::cout << "[OscHandler]: Error: " << e.what () << std::endl;
			}
		}
		break;

		case cmd_g_freeAll:
		{

			osc::ReceivedMessage::const_iterator arg = message->ArgumentsBegin();

			try
			{
				while (arg != message->ArgumentsEnd ())
				{
					try
					{
						QWriteLocker locker (&scgraph->_read_write_lock);
						scgraph->_node_tree.g_freeAll ((*(arg++)).AsInt32 ());
					}
					catch (const char *e)
					{
						std::cout << "[OscHandler]: g_freeAll(): " << e << std::endl;
					}
				}
			}
			catch (osc::Exception &e)
			{
				std::cout << "[OscHandler]: Error: " << e.what () << std::endl;
			}
		}
		break;

		case cmd_g_new:
		{
			osc::ReceivedMessage::const_iterator arg = message->ArgumentsBegin();
	
			try 
			{
				int count = message->ArgumentCount ();
				while (count > 0)
				{
					int id = -1;
					int add_action = 0;
					int target_id = SCGRAPH_DEFAULT_GROUP_ID;
	
					if (count-- > 0)
						id = (int)(arg++)->AsInt32 ();
			
					if (count-- > 0)
						add_action = (int)(arg++)->AsInt32 ();
			
					if (count-- > 0)
						target_id = (int)(arg++)->AsInt32 ();

					if (options->_verbose >= 2)
						std::cout << "[OscHandler]: /g_new " << id << " " << add_action << " " << target_id << std::endl;

					QWriteLocker locker (&scgraph->_read_write_lock);

					id = scgraph->_node_tree.g_new (id, add_action, target_id);

					if (id != -1)
						send_notifications ("/n_go", id);
				}
			}
			catch (const char *e)
			{
				std::cout << "[OscHandler]: Error while calling g_new (): " << e << std::endl;
			}
			catch (osc::Exception &e)
			{
				std::cout << "[OscHandler]: Error while parsing message: /g_new: " << e.what () << std::endl;
			}
		}
		break;

		case cmd_foo:
		{
	
		}
		break;


		case cmd_d_recv:
		{
			osc::ReceivedMessage::const_iterator arg = message->ArgumentsBegin();
	
			const void *synthdefblob;
			unsigned long synthdef_size;
	
			const void *completion_message;
			unsigned long completion_message_size;
	
			try 
			{
				(*arg++).AsBlob (synthdefblob, synthdef_size);
				// (*arg++).AsBlob (completion_message, completion_message_size);
	
				// std::cout << "size: " << synthdef_size << std::endl;
				ScGraph *scgraph = ScGraph::get_instance ();
	
				GSynthDefFile synthdef_file ((unsigned char*)synthdefblob, synthdef_size);
	
				for (std::vector <boost::shared_ptr<GSynthDef> >::iterator it = synthdef_file._synthdefs.begin (); it != synthdef_file._synthdefs.end (); ++it)
				{
					QWriteLocker locker (&scgraph->_read_write_lock);
					scgraph->_synthdef_pool.add_synthdef ((*it));
				}

				//FIXME: what's up with the optional osc message?
				if (arg != message->ArgumentsEnd () && ((*arg).TypeTag () == 'b'))
				{
					// std::cout << (*arg).TypeTag () << std::endl;
					(*arg++).AsBlob (completion_message, completion_message_size);

					// std::cout << "ugha?" << std::endl;
					osc::ReceivedPacket tmp_pack ((const char*)completion_message, completion_message_size);
					osc::ReceivedMessage tmp_rec_msg (tmp_pack);

					OscMessage *tmp_msg = new OscMessage (tmp_rec_msg, msg->_endpoint_name);
					handle_message_locked (tmp_msg);
					// ProcessPacket ((const char*)completion_message, completion_message_size, msg->_endpoint_name);

				}
			}
			catch (osc::Exception &e)
			{
				std::cout << "[OscHandler]: Error while parsing message: /d_recv: " << e.what () << std::endl;
			}
		}
		break;

		case cmd_d_load:
		{
			std::cout << "[OscHandler]: Warning: /d_load is not implemented yet." << std::endl;

			// QWriteLocker locker (&scgraph->_read_write_lock);

			// osc::ReceivedMessageArgumentStream args = message->ArgumentStream();
			// const char *filename;
	
			// args >> filename >> osc::EndMessage;
			// //scgraph->_scgraph->d_load (filename);
		}
		break;

		case cmd_c_set:
		{
			osc::ReceivedMessage::const_iterator arg = message->ArgumentsBegin();

			QWriteLocker locker (&scgraph->_read_write_lock);


			// FIXME: c_set really needs to take a vector and do the locking itself
			while (arg != message->ArgumentsEnd ())
			{
				try 
				{
					int bus = (int)(arg++)->AsInt32 ();
					float value = (arg++)->AsFloat ();

					scgraph->c_set (bus, value);
				}
				catch (osc::Exception &e)
				{
					std::cout << "[OscHandler]: Error while parsing message: /c_set: " << e.what () << std::endl;
				}
			}
		}
		break;

		case cmd_s_new:
		{
			osc::ReceivedMessage::const_iterator arg = message->ArgumentsBegin();
	
			const char *name = 0;
			osc::int32 id;
			osc::int32 add_action;
			osc::int32 add_target_id;
	
			try
			{
				name = (*(arg++)).AsString ();
				id = (*(arg++)).AsInt32 ();
				add_action = (*(arg++)).AsInt32 ();
				add_target_id = (*(arg++)).AsInt32 ();
				// >> id >> add_action >> add_target_id >;
	
				std::string tmp (name);
				if (options->_verbose >= 2)
					std::cout << "[OscHandler]: /s_new " << tmp << " " << id << " " << add_action << " " << add_target_id << std::endl;

				QWriteLocker locker (&scgraph->_read_write_lock);
				boost::shared_ptr<GSynth> synth = scgraph->_node_tree.s_new (tmp, (int)id, (int)add_action, (int)add_target_id);

				send_notifications ("/n_go", synth->_id);
	
				char *control_name = 0;
				osc::int32 control_index = 0;
				float control_value;
	
				while (arg != message->ArgumentsEnd ())
				{
					//TODO: Notify sc-dev about wrong types
					//std::cout << (*arg).TypeTag () << std::endl;
					if ((*arg).TypeTag () == 's')
					{
						control_name = (char*)(*(arg++)).AsString ();

						//std::cout << control_name << std::endl;

						//std::cout << (*arg).TypeTag () << std::endl;

						if ((*arg).TypeTag () == 'f')
							control_value = (*(arg++)).AsFloat ();
						else
							control_value = (*(arg++)).AsInt32 ();

						synth->c_set (control_name, control_value);
					}
					else 
					{
						control_index = (*(arg++)).AsInt32 ();

						// on /s_new, an empty
						// args list is
						// sent as nil value.
						// Ignore it.
						if(arg != message->ArgumentsEnd ())
						  {

						    // std::cout << control_index << std::endl;

						    if ((*arg).TypeTag () == 'f')
						      control_value = (*(arg++)).AsFloat ();
						    else
						      control_value = (*(arg++)).AsInt32 ();

						    synth->c_set (control_index, control_value);
						  }
					}
				}
				synth->process_c (0);
			}
			catch (const char* error)
			{
				if (name)
					std::cout << "[OscHandler]: Warning: Synth creation failed (SynthDef-name: \"" << name << "\"). Reason: " << error << std::endl;
			}
			catch (osc::Exception &e)
			{
				std::cout << "[OscHandler]: Error while parsing message: /s_new: " << e.what () << ". TypeTags: " << message->TypeTags() << std::endl;
			}
			//scgraph->unlock ();
		}
		break;

		case cmd_n_set:
		{
			osc::ReceivedMessage::const_iterator arg = message->ArgumentsBegin();

			int node_id;

			node_id = (*(arg++)).AsInt32 ();

			while (arg != message->ArgumentsEnd ())
			{
				//TODO: Notify sc-dev about wrong types
				//std::cout << (*arg).TypeTag () << std::endl;
				if ((*arg).TypeTag () == 's')
				{
					std::string control_name = (char*)(*(arg++)).AsString ();
					float control_value;

					//std::cout << control_name << std::endl;

					//std::cout << (*arg).TypeTag () << std::endl;

					if ((*arg).TypeTag () == 'f')
						control_value = (*(arg++)).AsFloat ();
					else
						control_value = (*(arg++)).AsInt32 ();

					QWriteLocker locker (&scgraph->_read_write_lock);

					scgraph->_node_tree.n_set (node_id, control_name, control_value);
				}
				else if((*arg).TypeTag () == 'i')
				{
					int control_index = (*(arg++)).AsInt32 ();
					float control_value;

					// std::cout << control_index << std::endl;

					if ((*arg).TypeTag () == 'f')
						control_value = (*(arg++)).AsFloat ();
					else
						control_value = (*(arg++)).AsInt32 ();

					scgraph->_node_tree.n_set (node_id, control_index, control_value);
				}
				else
				  {
				    // ignore array starts and ends
				    // for now
				    // FIX: handle arrays of values etc.
				    arg++;
				  }
			}
		}
		break;

		case cmd_n_setn:
		{
			osc::ReceivedMessage::const_iterator arg = message->ArgumentsBegin();

			int node_id;

			try
			{
				node_id = (*(arg++)).AsInt32 ();

				int control_index;
				//TODO: Notify sc-dev about wrong types
				//std::cout << (*arg).TypeTag () << std::endl;
				if ((*arg).TypeTag () == 's')
				{
					std::string control_name = (char*)(*(arg++)).AsString ();
					control_index = scgraph->_node_tree.get_index(node_id, control_name);
				} 
				else
				{
					control_index = (int)(*(arg++)).AsInt32 ();
				}

				// read number of values from stream	
				int num_vals = (*(arg++)).AsInt32 ();
				//std::cout << control_name << std::endl;
	
				//std::cout << (*arg).TypeTag () << std::endl;
				for (int i = 0;arg != message->ArgumentsEnd () &&  i < num_vals; ++i)
				{
					float control_value;
	
					if ((*arg).TypeTag () == 'f')
						control_value = (*(arg++)).AsFloat ();
					else
						control_value = (*(arg++)).AsInt32 ();
				// synth->c_set (control_name, control_value);
					scgraph->_node_tree.n_set (node_id, control_index++, control_value);
				}
			}
			catch (osc::Exception &e)
			{
				std::cout << "[OscHandler]: Error while parsing message: /n_setn: " << e.what () << ". TypeTags: " << message->TypeTags() << std::endl;
			}
			catch(std::runtime_error &e)
			{
				std::cout << "[OscHandler]: Runtime Error. Reason: " << e.what() << std::endl;
			}

		}
		break;

		case cmd_n_free:
		{
			osc::ReceivedMessage::const_iterator arg = message->ArgumentsBegin();

			try
			{
				while (arg != message->ArgumentsEnd ())
				{
					try
					{
						QWriteLocker locker (&scgraph->_read_write_lock);

						scgraph->_node_tree.n_free ((*(arg++)).AsInt32 ());
					}
					catch (const char *e)
					{
						std::cout << "[OscHandler]: n_free(): " << e << std::endl;
					}
				}
			}
			catch (osc::Exception &e)
			{
				std::cout << "[OscHandler]: Error: " << e.what () << std::endl;
			}
		}

		break;

		case cmd_loadTexture:
                {
                    osc::ReceivedMessage::const_iterator arg = message->ArgumentsBegin();
	
                    const char *path = 0;
                    try
			{
                            path = (*(arg++)).AsString ();

                            std::string tmp (path);
                            if (options->_verbose >= 2)
                                std::cout << "[OscHandler]: /loadTexture " << tmp << std::endl;

                            QReadLocker locker (&scgraph->_read_write_lock);
                            TexturePool::get_instance()->add_image(tmp, -1);

                            //send_notifications ("/n_go", synth->_id);
	
			}
                    catch (const char* error)
			{
                            if (path)
                                std::cout << "[OscHandler]: Warning: Texture loading failed: (path: \"" << path << "\"). Reason: " << error << std::endl;
			}
                    catch (osc::Exception &e)
			{
                            std::cout << "[OscHandler]: Error while parsing message: /loadTexture: " << e.what () << ". TypeTags: " << message->TypeTags() << std::endl;
			}
                    //scgraph->unlock ();
                }
		break;

		case cmd_loadTextureDir:

		break;

		default:
		{
			if (options->_verbose)
			{
				if (message->AddressPatternIsUInt32 ())
					std::cout << "[OscHandler]: Warning: Unknown OSC message-> Command num: " << command_num << std::endl;
				else
					std::cout << "[OscHandler]: Warning: Unknown OSC message-> Path: " << message->AddressPattern () << std::endl;
			}
		}
		break;
	}

	// std::cout << "handle message lcoked end" << std::endl;
}

void OscHandler::handle_message (OscMessage *message)
{
	//osc::ReceivedMessage msg(*(message->_msg));
	// std::cout << "handle message" << std::endl;
	//_condition_mutex.lock ();

	_mutex.lock ();

	handle_message_locked (message);

	//_handling_done = true;

	_mutex.unlock ();

	//_condition_mutex.unlock ();
	//_condition.wakeAll ();

	// std::cout << "handle message end" << std::endl;
	delete message;
}

#if 0
void OscHandler::ProcessBundle (const osc::ReceivedBundle& b, const IpEndpointName& remoteEndpoint)
{

}
#endif


