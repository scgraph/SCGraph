#include "osc_handler.h"
#include "scgraph.h"

#include "shader_pool.h"
#include "texture_pool.h"
#include "string_pool.h"

#include <iostream>
#include <stdexcept>
#include <sstream>


#include <algorithm>


#include <osc/OscReceivedElements.h>
#include <osc/OscOutboundPacketStream.h>


/*
 #include <QtCore/QMetaType>
 #include <QtCore/QReadLocker>
 #include <QtCore/QWriteLocker>
 */

OscHandler::OscHandler () :
_notifications (false)
{
    Options *options = Options::get_instance ();
    
    // queued connection is nessecary to get the msg across thread boundaries
    // TODO QObject::connect (this, SIGNAL (message_received(OscMessage*)), this, SLOT (handle_message(OscMessage*)), Qt::QueuedConnection);

#ifndef OFXOSC
    try
    {
        _socket = new osc::UdpListeningReceiveSocket (osc::IpEndpointName (osc::IpEndpointName::ANY_ADDRESS, options->_port), this);
    }
    catch (std::runtime_error e)
    {
        std::cout << "[OscHandler]: Error: Couldn't create socket. Reason: " << e.what () << std::endl;
        std::exit (EXIT_FAILURE);
    }
#endif
    
    // start ();

#ifdef OFXOSC
    _sender = new ofxOscSender();
    _sender->setup("localhost", options->_port);
#endif
}

OscHandler::~OscHandler ()
{
    stop();
    delete _socket;
}

void OscHandler::threadedFunction()
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
        catch (osc::MalformedMessageException e)
        {
            std::cout << "[OscHandler]: Error: Malformed message: " << e.what () << std::endl;
        }
        catch (osc::MalformedBundleException e)
        {
            std::cout << "[OscHandler]: Error: Malformed bundle: " << e.what () << std::endl;
        }
        catch (osc::MalformedPacketException e)
        {
            std::cout << "[OscHandler]: Error: Malformed packet: " << e.what () << std::endl;
        }
        catch (std::exception e)
        {
            std::cout << "[OscHandler]: Something went wrong: " << e.what () << std::endl;
            std::exit (EXIT_FAILURE);
        }
    }
    // std::cout << "fini" << std::endl;
}


void OscHandler::stop ()
{
    lock();
    
    _socket->AsynchronousBreak ();
    // std::cout << "break" << std::endl;
    
    unlock();
    // std::cout << "unlock" << std::endl;
    
    // TODO QThread::wait ();
    // std::cout << "wait" << std::endl;
}



void OscHandler::ProcessMessage (const osc::ReceivedMessage& message, const osc::IpEndpointName& name)
{
    //std::cout << "message!" << std::endl;
    OscMessage *msg = new OscMessage (message, name);
    
    //std::cout << "path: " << message.AddressPattern () << std::endl;
   
    handle_message(msg);
    
    // we used queued connection to get the message in the main qt thread context
    // not necessary in ofx it seems
    //_channel.send(std::move(msg));
}


void OscHandler::send_notifications (std::string path, int id)
{
    if (_notifications.empty())
        return;
    
    ScGraph *scgraph = ScGraph::get_instance ();
    
    Tree::Node<NodePtr> *node = scgraph->_node_tree.find_node_by_id (id);
    
    if (!node)
        throw (std::runtime_error("[OscHandler]: send_notifications(): Warning: ID doesn't exist"));
    
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
        
        for (std::vector<osc::IpEndpointName>::iterator it = _notifications.begin (); it != _notifications.end (); ++it)
        {
            _socket->SendTo ((*it), p.Data (), p.Size ());
        }
    }
    catch (osc::Exception &e)
    {
        std::cout << "[OscHandler]: Error: " << e.what () << std::endl;
    }
    
}


void OscHandler::send_notifications_of (std::string path, int id)
{
    if (_notifications_of.empty())
        return;
    
    ScGraph *scgraph = ScGraph::get_instance ();
    
    Tree::Node<NodePtr> *node = scgraph->_node_tree.find_node_by_id (id);
    
    if (!node)
        throw (std::runtime_error("[OscHandler]: send_notifications(): Warning: ID doesn't exist"));
    
    int is_group = 0;
    if (dynamic_cast<GGroup*> (node->_payload.get()))
        is_group = 1;
    
    ofxOscMessage msg;
    msg.setAddress(path);
    msg.addInt32Arg(id);
    msg.addInt32Arg(node->_parent->_payload->_id);
    msg.addInt32Arg(-1);
    msg.addInt32Arg(-1);
    msg.addInt32Arg(is_group);
    
    if (is_group) {
        msg.addInt32Arg(-1);
        msg.addInt32Arg(-1);
    }

        for (std::vector<std::pair<string, int>>::iterator it = _notifications_of.begin (); it != _notifications_of.end (); ++it)
        {
            //msg.setRemoteEndpoint((*it).first, (*it).second);
            _sender->setup((*it).first, (*it).second);
            _sender->sendMessage(msg);
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

// this slot is called in the context of the ofapp object due to the queued
// connection
void OscHandler::handle_message_locked (OscMessage *msg)
{
    //std::cout << "handle message locked" << std::endl;
    
    // std::cout << "Message received. Path: " << msg->_msg.AddressPattern () << std::endl;
    
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
            send_done("/quit", msg->_endpoint_name);
            
            ofExit();
            // we are done anyways
            std::cout << "[OscHandler]: cmd_quit()" << std::endl;
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
                
                //QWriteLocker locker (&scgraph->_read_write_lock);
                StringPool::get_instance()->set_font(tmp);
                
            }
           
            catch (osc::Exception &e)
            {
                std::cout << "[OscHandler]: Error while parsing message: /setFont: " << e.what () << ". TypeTags: " << message->TypeTags() << std::endl;
            }
            catch (const std::exception& ex)
            {
                if (str)
                    std::cout << "[OscHandler]: Warning: Set font failed: (font: \"" << str << "\"). Reason: " << ex.what() << std::endl;
            }
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
                
                //QWriteLocker locker (&scgraph->_read_write_lock);
                StringPool::get_instance()->change_string(tmp, index);
                
            }
            
            catch (osc::Exception &e)
            {
                std::cout << "[OscHandler]: Error while parsing message: /changeString: " << e.what () << ". TypeTags: " << message->TypeTags() << std::endl;
            }
            catch (const std::exception& ex)
            {
                if (str)
                    std::cout << "[OscHandler]: Warning: String changing failed: (string: \"" << str << "\"). Reason: " << ex.what() << std::endl;
            }
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
                
                //QReadLocker locker (&scgraph->_read_write_lock);
                
                StringPool::get_instance()->add_string(tmp, -1);
                
            }
            catch (osc::Exception &e)
            {
                std::cout << "[OscHandler]: Error while parsing message: /addString: " << e.what () << ". TypeTags: " << message->TypeTags() << std::endl;
            }
            catch (const std::exception& ex)
            {
                if (str)
                    std::cout << "[OscHandler]: Warning: String adding failed: (string: \"" << str << "\"). Reason: " << ex.what() << std::endl;
            }
            
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
            
            try
            {
                // TODO QWriteLocker locker (&scgraph->_read_write_lock);
                
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
                
                // TODO QWriteLocker locker (&scgraph->_read_write_lock);
                
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
                
                // TODO QWriteLocker locker (&scgraph->_read_write_lock);
                
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
                
                // TODO QWriteLocker locker (&scgraph->_read_write_lock);
                
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
            std::cout << "[OscHandler]: Warning: /d_free is currently deactivated." << std::endl;
            /*osc::ReceivedMessage::const_iterator arg = message->ArgumentsBegin();
            try {
                while (arg != message->ArgumentsEnd ())
                {
                    try
                    {
                        std::string name = (arg++)->AsString();
                        
                        scgraph->_synthdef_pool.d_free (name);
                        
                    }
                    catch (const std::runtime_error& ex)
                    {
                        std::cout << "[OscHandler]: Error while parsing message /d_free (): " << ex.what() << std::endl;
                    }
                }
            }
            catch (osc::Exception &e)
            {
                std::cout << "[OscHandler]: Error while parsing message: /d_free: " << e.what () << std::endl;
            }*/
        }
            break;
            
        case cmd_d_freeAll:
        {
            // TODO QWriteLocker locker (&scgraph->_read_write_lock);
            
            scgraph->_synthdef_pool.d_freeAll ();
        }
            break;
            
        case cmd_clearSched:
        {
            std::cout << "[OscHandler]: Warning: /clearSched is not implemented yet." << std::endl;
            // TODO: implement /clearSched
        }
            break;
            
        case cmd_sync:
        {
            // TODO QReadLocker locker (&scgraph->_read_write_lock);
            
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
            // TODO QWriteLocker locker (&scgraph->_read_write_lock);
            
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
            
            send_done("/notify", msg->_endpoint_name);
            
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
            // TODO QReadLocker locker (&scgraph->_read_write_lock);
            
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
                        // TODO QWriteLocker locker (&scgraph->_read_write_lock);
                        scgraph->_node_tree.g_freeAll ((*(arg++)).AsInt32 ());
                    }
                    catch (const std::runtime_error& ex)
                    {
                        std::cout << "[OscHandler]: g_freeAll(): " << ex.what() << std::endl;
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
                    
                    // TODO QWriteLocker locker (&scgraph->_read_write_lock);
                    
                    id = scgraph->_node_tree.g_new (id, add_action, target_id);
                    
                    if (id != -1)
                        send_notifications ("/n_go", id);
                }
            }
            catch (const std::runtime_error& ex)
            {
                std::cout << "[OscHandler]: Error while calling g_new (): " << ex.what() << std::endl;
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
            osc::osc_bundle_element_size_t synthdef_size;
            
            const void *completion_message;
            osc::osc_bundle_element_size_t completion_message_size;
            
            try
            {
                (*arg++).AsBlob (synthdefblob, synthdef_size);
                // (*arg++).AsBlob (completion_message, completion_message_size);
                
                // std::cout << "size: " << synthdef_size << std::endl;
                ScGraph *scgraph = ScGraph::get_instance ();
                
                GSynthDefFile synthdef_file ((unsigned char*)synthdefblob, synthdef_size);
                
                for (std::vector <boost::shared_ptr<GSynthDef> >::iterator it = synthdef_file._synthdefs.begin (); it != synthdef_file._synthdefs.end (); ++it)
                {
                    // TODO QWriteLocker locker (&scgraph->_read_write_lock);
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
            // TODO: implement /d_load
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
            
            // TODO QWriteLocker locker (&scgraph->_read_write_lock);
            
            
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
                
                // TODO QWriteLocker locker (&scgraph->_read_write_lock);
                boost::shared_ptr<GSynth> synth = scgraph->_node_tree.s_new (tmp, (int)id, (int)add_action, (int)add_target_id);
                
                send_notifications ("/n_go", synth->_id);
                
                char *control_name = 0;
                osc::int32 control_index = 0;
                float control_value;
                
                while (arg != message->ArgumentsEnd ())
                {
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
            catch (osc::Exception &e)
            {
                std::cout << "[OscHandler]: Error while parsing message: /s_new: " << e.what () << ". TypeTags: " << message->TypeTags() << std::endl;
            }
            catch (const std::exception& ex) {
                if (name)
                    std::cout << "[OscHandler]: Warning: Synth creation failed (SynthDef-name: \"" << name << "\"). Reason: " << ex.what() << std::endl;
            }
        }
            break;
            
        case cmd_n_set:
        {
            osc::ReceivedMessage::const_iterator arg = message->ArgumentsBegin();
            
            int node_id;
            
            node_id = (*(arg++)).AsInt32 ();
            
            while (arg != message->ArgumentsEnd ())
            {
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
                    
                    // TODO QWriteLocker locker (&scgraph->_read_write_lock);
                    
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
                    ++arg;
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
            catch(std::exception &e)
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
                        // TODO QWriteLocker locker (&scgraph->_read_write_lock);
                        //std::cout << (*(arg++)).AsInt32() << std::endl;
                        scgraph->_node_tree.n_free ((*(arg++)).AsInt32 ());
                    }
                    catch (const std::runtime_error& ex) {
                        std::cout << "[OscHandler]: n_free(): " << ex.what() << std::endl;
                    }
                }
            }
            catch (osc::Exception &e)
            {
                std::cout << "[OscHandler]: Error: " << e.what () << std::endl;
            }
        }
            
            break;
            
        case cmd_b_read:
        {
            osc::ReceivedMessage::const_iterator arg = message->ArgumentsBegin();
            
            
            osc::int32 bufnum, starting_frame_in_file, num_frames, starting_frame_in_buffer, leave_file_open_p;
            
            const char *path = 0;
            
            const void *completion_message;
            osc::osc_bundle_element_size_t completion_message_size;
            
            try
            {
                bufnum = (*(arg++)).AsInt32 ();
                path = (*(arg++)).AsString ();
                starting_frame_in_file = (*(arg++)).AsInt32 ();
                num_frames = (*(arg++)).AsInt32 ();
                starting_frame_in_buffer = (*(arg++)).AsInt32 ();
                leave_file_open_p = (*(arg++)).AsInt32 ();
                
                std::string tmp (path);
                if (options->_verbose >= 2)
                    std::cout << "[OscHandler]: /b_read " << tmp << std::endl;
                
                TexturePool * tp = TexturePool::get_instance();
                
                // TODO QReadLocker locker (&scgraph->_read_write_lock);
                if(bufnum < tp->get_number_of_textures())
                    tp->change_image(tmp, bufnum);
                else
                    tp->add_image(tmp, bufnum);
                
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
                
                
                send_done("/b_read", msg->_endpoint_name);
            }
            
            catch (osc::Exception &e)
            {
                std::cout << "[OscHandler]: Error while parsing message: /b_read: " << e.what () << ". TypeTags: " << message->TypeTags() << std::endl;
            }
            
            catch (const std::exception* error)
            {
                if (path)
                    std::cout << "[OscHandler]: Warning: Texture loading failed: (path: \"" << path << "\"). Reason: " << error->what() << std::endl;
            }
        }
            break;
            
        case cmd_b_allocRead:
        {
            osc::ReceivedMessage::const_iterator arg = message->ArgumentsBegin();
            
            osc::int32 bufnum, starting_frame_in_file, num_frames;
            
            const char *path = 0;
            
            const void *completion_message;
            osc::osc_bundle_element_size_t completion_message_size;
            
            try
            {
                bufnum = (*(arg++)).AsInt32 ();
                path = (*(arg++)).AsString ();
                starting_frame_in_file = (*(arg++)).AsInt32 ();
                num_frames = (*(arg++)).AsInt32 ();
                std::string tmp (path);
                
                if (options->_verbose >= 2)
                    std::cout << "[OscHandler]: /b_allocRead " << tmp << std::endl;
                
                // TODO QReadLocker locker (&scgraph->_read_write_lock);
                TexturePool::get_instance()->add_image(tmp, bufnum);
                
                if (arg != message->ArgumentsEnd () && ((*arg).TypeTag () == 'b'))
                {
                    (*arg++).AsBlob (completion_message,
                                     completion_message_size);
                    
                    osc::ReceivedPacket tmp_pack ((const char*)
                                                  completion_message,
                                                  completion_message_size);
                    osc::ReceivedMessage tmp_rec_msg (tmp_pack);
                    
                    OscMessage *tmp_msg = new OscMessage
                    (tmp_rec_msg,
                     msg->_endpoint_name);
                    handle_message_locked (tmp_msg);
                }
                
                send_done("/b_allocRead", msg->_endpoint_name);
            }
            catch (osc::Exception &e)
            {
                std::cout << "[OscHandler]: Error while parsing message: /b_allocRead: " << e.what () << ". TypeTags: " << message->TypeTags() << std::endl;
            }
            catch (std::exception* error)
            {
                if (path)
                    std::cout << "[OscHandler]: Warning: Texture loading failed: (path: \"" << path << "\"). Reason: " << error->what() << std::endl;
            }
        }
            break;
            
        case cmd_b_query:
        {
            osc::ReceivedMessage::const_iterator arg = message->ArgumentsBegin();
            osc::int32 bufnum;
            try	{
                bufnum = (*(arg++)).AsInt32 ();
                
                TexturePool *tp = TexturePool::get_instance();
                boost::optional<boost::shared_ptr<AbstractTexture> > t = tp->get_texture(bufnum);
                
                if(t) {
                    osc::OutboundPacketStream p (_message_buffer, SCGRAPH_OSC_MESSAGE_BUFFER_SIZE);
                    
                    try
                    {
                        p << osc::BeginMessage ("/b_info")
                        << bufnum
                        << (*t)->get_width() // num frames
                        << (*t)->get_height() // num channels
                        << 0 // sample rate
                        << osc::EndMessage;
                        
                        _socket->SendTo (msg->_endpoint_name, p.Data (), p.Size ());
                    }
                    catch (osc::Exception &e)
                    {
                        std::cout << "[OscHandler]: Error: " << e.what () << std::endl;
                    }
                    
                    
                }
                else {
                    std::cout << "[OscHandler]: /b_query: texture id " << bufnum << " doesn't exist!" << std::endl;
                }
                
            }
            catch (osc::Exception &e)
            {
                std::cout << "[OscHandler]: Error while parsing message: /b_query: " << e.what () << ". TypeTags: " << message->TypeTags() << std::endl;
            }
        }
            break;
            
            
        case cmd_b_zero:
        {
            osc::ReceivedMessage::const_iterator arg = message->ArgumentsBegin();
            
            osc::int32 bufnum;
            
            const void *completion_message;
            osc::osc_bundle_element_size_t completion_message_size;
            
            try
            {
                bufnum = (*(arg++)).AsInt32 ();
                
                if (options->_verbose >= 2)
                    std::cout << "[OscHandler]: /b_zero " << bufnum << std::endl;
                
                boost::optional<boost::shared_ptr<AbstractTexture> > texture =
                TexturePool::get_instance()->get_texture(bufnum);
                
                if(texture) {
                    (*texture)->zero();
                    TexturePool::get_instance()->update_texture(bufnum);
                }
                
                if (arg != message->ArgumentsEnd () && ((*arg).TypeTag () == 'b'))
                {
                    (*arg++).AsBlob (completion_message,
                                     completion_message_size);
                    
                    osc::ReceivedPacket tmp_pack ((const char*)
                                                  completion_message,
                                                  completion_message_size);
                    osc::ReceivedMessage tmp_rec_msg (tmp_pack);
                    
                    OscMessage *tmp_msg = new OscMessage
                    (tmp_rec_msg,
                     msg->_endpoint_name);
                    handle_message_locked (tmp_msg);
                }
                
                send_done("/b_zero", msg->_endpoint_name);
            }
            catch (osc::Exception &e)
            {
                std::cout << "[OscHandler]: Error while parsing message: /b_zero: " << e.what () << ". TypeTags: " << message->TypeTags() << std::endl;
            }
        }
            break;
            
        case cmd_b_set:
        {
            osc::ReceivedMessage::const_iterator arg = message->ArgumentsBegin();
            
            int bufnum;
            
            int32_t color;
            osc::int32 pindex;
            
            try {
                bufnum = (*(arg++)).AsInt32 ();
                
                boost::optional<boost::shared_ptr<AbstractTexture> > texture = TexturePool::get_instance()->get_texture(bufnum);
                
                if(texture) {
                    while (arg != message->ArgumentsEnd ())
                    {
                        pindex = (*(arg++)).AsInt32();
                        if((*arg).TypeTag () == 'i')
                            color = (*(arg++)).AsInt32();
                        else
                            color = (int32_t) (*(arg++)).AsFloat();
                        
                        (*texture)->set_pixel(pindex, color);
                    }
                    TexturePool::get_instance()->update_texture(bufnum);
                }
            }
            catch (osc::Exception &e)
            {
                std::cout << "[OscHandler]: Error while parsing message: /b_set: " << e.what () << ". TypeTags: " << message->TypeTags() << std::endl;
            }
        }
            break;
            
        case cmd_b_setn:
        {
            osc::ReceivedMessage::const_iterator arg = message->ArgumentsBegin();
            
            int bufnum;
            
            osc::int32 starting_index, num_samples;
            
            int32_t color;
            
            try {
                bufnum = (*(arg++)).AsInt32 ();
                
                boost::optional<boost::shared_ptr<AbstractTexture> > texture = TexturePool::get_instance()->get_texture(bufnum);
                
                if(texture) {
                    while (arg != message->ArgumentsEnd ())
                    {
                        starting_index = (*(arg++)).AsInt32();
                        num_samples = (*(arg++)).AsInt32();
                        
                        if((*arg).TypeTag () == 'i')
                            color = (*(arg++)).AsInt32();
                        else
                            color = (int32_t) (*(arg++)).AsFloat();
                        
                        
                        for(int i = 0; i < num_samples; i++) {
                            (*texture)->set_pixel(starting_index + i,
                                                  color);
                        }
                    }
                    TexturePool::get_instance()->update_texture(bufnum);
                }
            }
            catch (osc::Exception &e)
            {
                std::cout << "[OscHandler]: Error while parsing message: /b_setn: " << e.what () << ". TypeTags: " << message->TypeTags() << std::endl;
            }
        }
            break;
            
            
        case cmd_b_fill:
        {
            osc::ReceivedMessage::const_iterator arg = message->ArgumentsBegin();
            
            int bufnum;
            
            osc::int32 starting_index, num_samples;
            int32_t color;
            
            try {
                bufnum = (*(arg++)).AsInt32 ();
                
                boost::optional<boost::shared_ptr<AbstractTexture> > texture = TexturePool::get_instance()->get_texture(bufnum);
                
                if(texture) {
                    
                    while (arg != message->ArgumentsEnd ())
                    {
                        starting_index = (*(arg++)).AsInt32();
                        num_samples = (*(arg++)).AsInt32();
                        
                        if((*arg).TypeTag () == 'i')
                            color = (*(arg++)).AsInt32();
                        else
                            color = (int32_t) (*(arg++)).AsFloat();
                        
                        (*texture)->fill(starting_index,
                                         num_samples,
                                         color);
                    }
                    TexturePool::get_instance()->update_texture(bufnum);
                }
            }
            catch (osc::Exception &e)
            {
                std::cout << "[OscHandler]: Error while parsing message: /b_fill: " << e.what () << ". TypeTags: " << message->TypeTags() << std::endl;
            }
        }
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
    
    // std::cout << "handle message locked end" << std::endl;
}

#ifdef OFXOSC
void OscHandler::handle_message_of (ofxOscMessage * message)
{
    //std::cout << "handle message of" << std::endl;
    
    //std::cout << "Message received. Path: " << message->getAddress() << std::endl;
    
    Options *options = Options::get_instance ();
    ScGraph *scgraph = ScGraph::get_instance ();
    
    scgraph->lock();
    
    int command_num;
    try {
        command_num = std::stoi(message->getAddress());
        if (options->_verbose >= 4)
            std::cout << "[OscHandler]: Message received. Command num: " << command_num << ", # of args: " << message->getNumArgs() << std::endl; // ", Types: " << message->TypeTags () << std::endl;
    }
    catch(std::exception &ex)
    {
        command_num = command_name_to_int (message->getAddress ());
        if (options->_verbose >= 4)
            std::cout << "[OscHandler]: Message received. Path: " << message->getAddress () << ", # of args: " << message->getNumArgs() << std::endl; // ", Types: " << message->TypeTags () << std::endl;
        
    }
    
    switch (command_num)
    {
        case cmd_quit:
            send_done("/quit", message->getRemoteIp(), message->getRemotePort());
            
            ofExit();
            // we are done anyways
            std::cout << "[OscHandler]: cmd_quit()" << std::endl;
            break;
            
        case cmd_setFont: {
            string path = message->getArgAsString(0);
            
            if (options->_verbose >= 2)
                std::cout << "[OscHandler]: /setFont " << path << std::endl;
            
            //QWriteLocker locker (&scgraph->_read_write_lock);
            StringPool::get_instance()->set_font(path);
            break; }
            
        case cmd_changeString: {
            int index = message->getArgAsInt(0);
            string newStr = message->getArgAsString(1);
            
            if (options->_verbose >= 2)
                std::cout << "[OscHandler]: /changeString " << newStr << std::endl;
            
            //QWriteLocker locker (&scgraph->_read_write_lock);
            StringPool::get_instance()->change_string(newStr, index);
            break; }
            
        case cmd_addString: {
            string newStr = message->getArgAsString(0);
            if (options->_verbose >= 2)
                std::cout << "[OscHandler]: /addString " << newStr << std::endl;
            
            //QReadLocker locker (&scgraph->_read_write_lock);
            
            StringPool::get_instance()->add_string(newStr, -1);
            break; }
            
        case cmd_clearShaderPrograms:
            ShaderPool::get_instance()->clear_shader_programs();
            break;
            
        case cmd_loadShaderProgram: {
            // TODO QWriteLocker locker (&scgraph->_read_write_lock);
            boost::shared_ptr<ShaderPool::ShaderProgram> p(new ShaderPool::ShaderProgram);
            int index = message->getArgAsInt(0);
            std::stringstream attributes_stream;
            attributes_stream.str(message->getArgAsString(0));
            std::string attribute;
            
            while((attribute = "", (attributes_stream >> attribute)), attribute != "")
            {
                p->_uniforms.push_back(attribute);
                std::cout << "[OscHandler]: add attribute: \"" << attribute << "\"" << std::endl;
            }
            
            for(int i = 1; i < message->getNumArgs(); i += 2) {
                std::string program = message->getArgAsString(i);
                bool isVertexShader = message->getArgAsBool(i+1);
                
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
            break; }
            
        case cmd_graphicsRate: {
            osc::int32 graphics_rate = message->getArgAsInt32(0);
            std::cout << "[OscHandler]: Setting graphics rate to: " << graphics_rate << std::endl;
            
            // TODO QWriteLocker locker (&scgraph->_read_write_lock);
            
            scgraph->_graphic_loop.set_rate (graphics_rate);
            break; }
            
        case cmd_controlRate: {
            osc::int32 control_rate = message->getArgAsInt32(0);
            std::cout << "[OscHandler]: Setting control rate to: " << control_rate << std::endl;
            
            // TODO QWriteLocker locker (&scgraph->_read_write_lock);
            
            scgraph->_control_loop.set_rate (control_rate);
            break; }
            
        case cmd_dumpOSC:
            break;
        case cmd_verbose: {
            osc::int32 verbose_level = message->getArgAsInt32(0);
            std::cout << "[OscHandler]: Setting verbose level to: " << verbose_level << std::endl;
            
            // TODO QWriteLocker locker (&scgraph->_read_write_lock);
            
            options->_verbose = verbose_level;
            break; }
            
        case cmd_d_free: {
            for (int i = 0; i < message->getNumArgs(); i++)
            {
                std::string name = message->getArgAsString(i);
                // TODO QWriteLocker locker (&scgraph->_read_write_lock);
                try {
                    scgraph->_synthdef_pool.d_free (name);
                }
                catch (std::runtime_error *e)
                {
                    std::cout << "[OscHandler]: d_free(): " << e->what() << std::endl;
                }
            }
            catch (std::runtime_error *e)
            {
                std::cout << "[OscHandler]: d_free(): " << e->what() << std::endl;
            }
            break; }
            
        case cmd_d_freeAll:
            // TODO QWriteLocker locker (&scgraph->_read_write_lock);
            scgraph->_synthdef_pool.d_freeAll ();
            break;
            
        case cmd_sync: {
            // TODO QReadLocker locker (&scgraph->_read_write_lock);
            osc::int32 command_id = message->getArgAsInt32(0);
            
            ofxOscMessage msg;
            //msg.setRemoteEndpoint(message->getRemoteIp(), message->getRemotePort());
            msg.addInt32Arg(command_id);
            msg.setAddress("/synced");
            _sender->setup(message->getRemoteIp(), message->getRemotePort());
            _sender->sendMessage(msg);
            break; }
            
        case cmd_notify: {
            // TODO QWriteLocker locker (&scgraph->_read_write_lock);
            if (message->getNumArgs() != 1)
            {
                if (options->_verbose)
                    std::cout << "[OscHandler]: Warning: /notify received without argument..." << std::endl;
                
                break;
            }
            
            osc::int32 on_off = message->getArgAsInt32(0);
            
            std::pair<string, int> endpoint = make_pair(message->getRemoteIp(), message->getRemotePort());
            if (std::find (_notifications_of.begin (), _notifications_of.end (), endpoint) != _notifications_of.end ())
            {
                if (!on_off)
                {
                    _notifications_of.erase(std::remove (_notifications_of.begin (), _notifications_of.end (), endpoint), _notifications_of.end());
                }
            }
            else
            {
                if (on_off)
                    _notifications_of.push_back (endpoint);
            }
            
            send_done("/notify", message->getRemoteIp(), message->getRemotePort());
            
            if (options->_verbose >= 2)
            {
                std::cout << "[OscHandler]: Notification list changed [" << _notifications.size () << " item(s)]:" << std::endl;
                for (unsigned int i = 0; i < _notifications_of.size (); ++i)
                {
                    std::cout << i << ": " << _notifications_of[i].first << " " << _notifications_of[i].second << std::endl;
                }
            }
            break; }
            
        case cmd_status: {
            // TODO QReadLocker locker (&scgraph->_read_write_lock);
            ofxOscMessage msg;
            msg.setAddress("/status.reply");
            msg.addIntArg(1);
            msg.addIntArg(2);
            msg.addIntArg(2);
            msg.addIntArg(2);
            msg.addIntArg(10);
            msg.addFloatArg(0.1);
            msg.addFloatArg(0.1);
            msg.addDoubleArg(48000.0);
            msg.addDoubleArg(48000.0);
            //msg.setRemoteEndpoint(message->getRemoteIp(), message->getRemotePort());
            
            _sender->setup(message->getRemoteIp(), message->getRemotePort());
            _sender->sendMessage(msg);
            break; }
            
        case cmd_g_freeAll:
            for(int i=0; i<message->getNumArgs(); i++) {
                try
                {
                    // TODO QWriteLocker locker (&scgraph->_read_write_lock);
                    scgraph->_node_tree.g_freeAll (message->getArgAsInt32(i));
                }
                catch (std::exception *e)
                {
                    std::cout << "[OscHandler]: g_freeAll(): " << e->what() << std::endl;
                }
            }
            break;
            
        case cmd_g_new: {
            int arg = 0;
            int numArgs = message->getNumArgs();
            while (arg < numArgs)
            {
                int id = -1;
                int add_action = 0;
                int target_id = SCGRAPH_DEFAULT_GROUP_ID;
                
                if (arg < numArgs)
                    id = message->getArgAsInt(arg++);
                
                if (arg < numArgs)
                    add_action = message->getArgAsInt32(arg++);
                
                if (arg < numArgs)
                    target_id = message->getArgAsInt32(arg++);
                
                if (options->_verbose >= 2)
                    std::cout << "[OscHandler]: /g_new " << id << " " << add_action << " " << target_id << std::endl;
                
                // TODO QWriteLocker locker (&scgraph->_read_write_lock);
                id = scgraph->_node_tree.g_new (id, add_action, target_id);
                
                if (id != -1)
                    send_notifications_of ("/n_go", id);
            }
            break; }
            
        case cmd_foo:
            
            break;
            
            
        case cmd_d_recv: {
            ofBuffer synthdefblob = message->getArgAsBlob(0);
            long synthdef_size = synthdefblob.size();
            
            ofBuffer completion_message = message->getArgAsBlob(1);
            long completion_message_size = completion_message.size();
            
            
            // std::cout << "size: " << synthdef_size << std::endl;
            ScGraph *scgraph = ScGraph::get_instance ();
            
            GSynthDefFile synthdef_file ((unsigned char*)synthdefblob.getData(), synthdef_size);
            
            for (std::vector <boost::shared_ptr<GSynthDef> >::iterator it = synthdef_file._synthdefs.begin (); it != synthdef_file._synthdefs.end (); ++it)
            {
                // TODO QWriteLocker locker (&scgraph->_read_write_lock);
                scgraph->_synthdef_pool.add_synthdef ((*it));
            }
            
            //FIXME: what's up with the optional osc message?
            if (message->getNumArgs() > 2 && message->getArgType(2) == ofxOscArgType::OFXOSC_TYPE_BLOB)
            {
                completion_message = message->getArgAsBlob(1);
                completion_message_size = completion_message.size();
                
                // std::cout << "ugha?" << std::endl;
                ofxOscMessage msg;
                msg.setRemoteEndpoint(message->getRemoteIp(), message->getRemotePort());
                msg.addBlobArg(completion_message);
                msg.setAddress(message->getAddress());
                
                handle_message_of (&msg);
            }
            break; }
            
        case cmd_d_load:
            std::cout << "[OscHandler]: Warning: /d_load is not implemented yet." << std::endl;
            // TODO: implement d_load
            
            // QWriteLocker locker (&scgraph->_read_write_lock);
            
            // osc::ReceivedMessageArgumentStream args = message->ArgumentStream();
            // const char *filename;
            
            // args >> filename >> osc::EndMessage;
            // //scgraph->_scgraph->d_load (filename);
            break;
            
        case cmd_c_set:
            // TODO QWriteLocker locker (&scgraph->_read_write_lock);
            
            // FIXME: c_set really needs to take a vector and do the locking itself
            
            for(size_t i = 0; i < message->getNumArgs(); i+=2) {
                int bus = message->getArgAsInt32(i);
                float value = message->getArgAsFloat(i+1);
                scgraph->c_set (bus, value);
            }
            break;
            
        case cmd_s_new: {
            string name = "";
            int id;
            int add_action;
            int add_target_id;
            
            try {
                name = message->getArgAsString(0);
                id = message->getArgAsInt32(1);
                add_action = message->getArgAsInt32(2);
                add_target_id = message->getArgAsInt32(3);
                
                if (options->_verbose >= 2)
                    std::cout << "[OscHandler]: /s_new " << name << " " << id << " " << add_action << " " << add_target_id << std::endl;
                
                // TODO QWriteLocker locker (&scgraph->_read_write_lock);
                boost::shared_ptr<GSynth> synth = scgraph->_node_tree.s_new (name, (int)id, (int)add_action, (int)add_target_id);
                
                send_notifications_of ("/n_go", synth->_id);
                
                string control_name;
                osc::int32 control_index = 0;
                float control_value;
                
                for(size_t i = 4; i < message->getNumArgs(); i+=2) {
                    if (message->getArgType(i) == ofxOscArgType::OFXOSC_TYPE_STRING) {
                        control_name = message->getArgAsString(i);
                        
                        //std::cout << control_name << std::endl;
                        //std::cout << (*arg).TypeTag () << std::endl;
                        
                        if (message->getArgType(i+1) == ofxOscArgType::OFXOSC_TYPE_FLOAT)
                            control_value = message->getArgAsFloat(i+1);
                        else
                            control_value = message->getArgAsInt32(i+1);
                        
                        synth->c_set (control_name.c_str(), control_value);
                    }
                    else {
                        control_index = message->getArgAsInt32 (i);
                        // on /s_new, an empty
                        // args list is
                        // sent as nil value.
                        // Ignore it.
                        if(i+1 < message->getNumArgs())
                        {
                            if (message->getArgType(i+1) == ofxOscArgType::OFXOSC_TYPE_FLOAT)
                                control_value = message->getArgAsFloat(i+1);
                            else
                                control_value = message->getArgAsInt32(i+1);
                            
                            // std::cout << control_index << std::endl;
                            synth->c_set (control_index, control_value);
                        }
                    }
                    
                }                synth->process_c (0);
            }
            catch (const std::exception& ex) {
                if (name != "")
                    std::cout << "[OscHandler]: Warning: Synth creation failed (SynthDef-name: \"" << name << "\"). Reason: " << ex.what() << std::endl;
            }
            break; }
            
        case cmd_n_set: {
            int node_id = message->getArgAsInt32(0);
            string control_name;
            float control_value;
            int control_index;
            for(size_t i = 1; i < message->getNumArgs(); i+=2) {
                if (message->getArgType(i) == ofxOscArgType::OFXOSC_TYPE_STRING) {
                    control_name = message->getArgAsString(i);
                    
                    //std::cout << control_name << std::endl;
                    //std::cout << (*arg).TypeTag () << std::endl;
                    
                    if (message->getArgType(i+1) == ofxOscArgType::OFXOSC_TYPE_FLOAT)
                        control_value = message->getArgAsFloat(i+1);
                    else
                        control_value = message->getArgAsInt32(i+1);
                    
                    scgraph->_node_tree.n_set (node_id, control_name, control_value);
                }
                else if(message->getArgType(i) == ofxOscArgType::OFXOSC_TYPE_INT32) {
                    control_index = message->getArgAsInt32 (i);
                    // on /s_new, an empty
                    // args list is
                    // sent as nil value.
                    // Ignore it.
                    if(i+1 < message->getNumArgs())
                    {
                        if (message->getArgType(i+1) == ofxOscArgType::OFXOSC_TYPE_FLOAT)
                            control_value = message->getArgAsFloat(i+1);
                        else
                            control_value = message->getArgAsInt32(i+1);
                        
                        // std::cout << control_index << std::endl;
                        scgraph->_node_tree.n_set (node_id, control_index, control_value);
                    }
                } else {
                    // ignore array starts and ends
                    // for now
                    // TODO: handle arrays of values etc.
                }
            }
            break; }
            
        case cmd_n_setn:
            try
        {
            int node_id = message->getArgAsInt32(0);
            
            int control_index;
            //TODO: Notify sc-dev about wrong types
            //std::cout << (*arg).TypeTag () << std::endl;
            if (message->getArgType(1) == ofxOscArgType::OFXOSC_TYPE_STRING)
            {
                std::string control_name = message->getArgAsString(1);
                control_index = scgraph->_node_tree.get_index(node_id, control_name);
            }
            else
            {
                control_index = message->getArgAsInt32(1);
            }
            
            // read number of values from stream
            int num_vals = message->getArgAsInt32(2);
            //std::cout << control_name << std::endl;
            
            //std::cout << (*arg).TypeTag () << std::endl;
            for (int i = 3; i < message->getNumArgs() &&  i < num_vals; ++i)
            {
                float control_value;
                
                if (message->getArgType(i) == ofxOscArgType::OFXOSC_TYPE_FLOAT)
                    control_value = message->getArgAsFloat(i);
                else
                    control_value = message->getArgAsInt32(i);
                
                // synth->c_set (control_name, control_value);
                scgraph->_node_tree.n_set (node_id, control_index++, control_value);
            }
        }
            catch(std::runtime_error &e)
        {
            std::cout << "[OscHandler]: Runtime Error. Reason: " << e.what() << std::endl;
        }
            break;
            
        case cmd_n_free:
            for (int i = 0; i < message->getNumArgs(); ++i)
            {
                try
                {
                    // TODO QWriteLocker locker (&scgraph->_read_write_lock);
                    //std::cout << (*(arg++)).AsInt32() << std::endl;
                    scgraph->_node_tree.n_free(message->getArgAsInt32(i));
                }
                catch (const std::exception& ex) {
                    std::cout << "[OscHandler]: n_free(): " << ex.what() << std::endl;
                }
            }
            break;
            
        case cmd_b_read: {
            string path;
            try
            {
                int bufnum = message->getArgAsInt32(0);
                path = message->getArgAsString(1);
                int starting_frame_in_file = message->getArgAsInt32(2);
                int num_frames = message->getArgAsInt32(3);
                int starting_frame_in_buffer = message->getArgAsInt32(4);
                int leave_file_open_p = message->getArgAsInt32(5);
                
                if (options->_verbose >= 2)
                    std::cout << "[OscHandler]: /b_read " << path << std::endl;
                
                TexturePool * tp = TexturePool::get_instance();
                
                // TODO QReadLocker locker (&scgraph->_read_write_lock);
                if(bufnum < tp->get_number_of_textures())
                    tp->change_image(path, bufnum);
                else
                    tp->add_image(path, bufnum);
                
                if (message->getNumArgs() > 5 && message->getArgType(6) == ofxOscArgType::OFXOSC_TYPE_BLOB)
                {
                    ofBuffer completion_message = message->getArgAsBlob(6);
                    long completion_message_size = completion_message.size();
                    
                    // std::cout << "ugha?" << std::endl;
                    ofxOscMessage msg;
                    msg.setRemoteEndpoint(message->getRemoteIp(), message->getRemotePort());
                    msg.addBlobArg(completion_message);
                    msg.setAddress(message->getAddress());
                    
                    handle_message_of (&msg);
                }
                send_done("/b_read", message->getRemoteIp(), message->getRemotePort());
            }
            catch (std::exception error)
            {
                std::cout << "[OscHandler]: Warning: Texture loading failed: (path: \"" << path << "\"). Reason: " << error.what() << std::endl;
            }
            break; }
            
        case cmd_b_allocRead:{
            string path;
            try
            {
                int bufnum = message->getArgAsInt32(0);
                path = message->getArgAsString(1);
                int starting_frame_in_file = message->getArgAsInt32(2);
                int num_frames = message->getArgAsInt32(3);
                
                if (options->_verbose >= 2)
                    std::cout << "[OscHandler]: /b_allocRead " << path << std::endl;
                
                TexturePool * tp = TexturePool::get_instance();
                
                // TODO QReadLocker locker (&scgraph->_read_write_lock);
                TexturePool::get_instance()->add_image(path, bufnum);
                
                if (message->getNumArgs() > 6 && message->getArgType(6) == ofxOscArgType::OFXOSC_TYPE_BLOB)
                {
                    ofBuffer completion_message = message->getArgAsBlob(6);
                    long completion_message_size = completion_message.size();
                    
                    // std::cout << "ugha?" << std::endl;
                    ofxOscMessage msg;
                    msg.setRemoteEndpoint(message->getRemoteIp(), message->getRemotePort());
                    msg.addBlobArg(completion_message);
                    msg.setAddress(message->getAddress());
                    
                    handle_message_of (&msg);
                }
                send_done("/b_allocRead", message->getRemoteIp(), message->getRemotePort());
            }
            catch (std::exception error)
            {
                std::cout << "[OscHandler]: Warning: Texture loading failed: (path: \"" << path << "\"). Reason: " << error.what() << std::endl;
            }
            break; }
            
        case cmd_b_query: {
            int bufnum = message->getArgAsInt32(0);
            TexturePool *tp = TexturePool::get_instance();
            boost::optional<boost::shared_ptr<AbstractTexture> > t = tp->get_texture(bufnum);
            
            if(t) {
                ofxOscMessage msg;
                
                try
                {
                    msg.setAddress("/b_info");
                    msg.addInt32Arg(bufnum);
                    msg.addInt32Arg((*t)->get_width()); // num frames
                    msg.addInt32Arg((*t)->get_height()); // num channels
                    msg.addInt32Arg(0);
                    
                    msg.setRemoteEndpoint(message->getRemoteIp(), message->getRemotePort());
                }
                catch (osc::Exception &e)
                {
                    std::cout << "[OscHandler]: Error: " << e.what () << std::endl;
                }
            }
            else {
                std::cout << "[OscHandler]: /b_query: texture id " << bufnum << " doesn't exist!" << std::endl;
            }
            break;}
            
        case cmd_b_zero: {
            int bufnum = message->getArgAsInt32(0);
            
            if (options->_verbose >= 2)
                std::cout << "[OscHandler]: /b_zero " << bufnum << std::endl;
            
            boost::optional<boost::shared_ptr<AbstractTexture> > texture =
            TexturePool::get_instance()->get_texture(bufnum);
            
            if(texture) {
                (*texture)->zero();
                TexturePool::get_instance()->update_texture(bufnum);
            }
            
            if (message->getNumArgs() > 1 && message->getArgType(1) == ofxOscArgType::OFXOSC_TYPE_BLOB)
            {
                ofBuffer completion_message = message->getArgAsBlob(1);
                long completion_message_size = completion_message.size();
                
                ofxOscMessage msg;
                msg.setRemoteEndpoint(message->getRemoteIp(), message->getRemotePort());
                msg.addBlobArg(completion_message);
                msg.setAddress(message->getAddress());
                
                handle_message_of (&msg);
            }
            
            send_done("/b_zero", message->getRemoteIp(), message->getRemotePort());
            break;}
            
        case cmd_b_set:{
            int32_t bufnum = message->getArgAsInt32(0);
            
            int32_t color;
            int32_t pindex;
            
            boost::optional<boost::shared_ptr<AbstractTexture> > texture = TexturePool::get_instance()->get_texture(bufnum);
            
            if(texture) {
                for(int32_t i = 1; i < message->getNumArgs(); i+=2) {
                    pindex = message->getArgAsInt32(i);
                    if(message->getArgType(i+1) == ofxOscArgType::OFXOSC_TYPE_INT32)
                        color = message->getArgAsInt32(i+1);
                    else
                        color = (int) message->getArgAsFloat(i+1);
                    
                    (*texture)->set_pixel(pindex, color);
                }
                TexturePool::get_instance()->update_texture(bufnum);
            }
            break;}
            
        case cmd_b_setn: {
            int32_t bufnum = message->getArgAsInt32(0);
            int32_t starting_index, num_samples;
            
            int32_t color;
            
            boost::optional<boost::shared_ptr<AbstractTexture> > texture = TexturePool::get_instance()->get_texture(bufnum);
            
            
            if(texture) {
                for(int32_t i = 1; i < message->getNumArgs(); i+=3) {
                    starting_index = message->getArgAsInt32(i);
                    num_samples = message->getArgAsInt32(i+1);
                    if(message->getArgType(i+1) == ofxOscArgType::OFXOSC_TYPE_INT32)
                        color = message->getArgAsInt32(i+2);
                    else
                        color = (int) message->getArgAsFloat(i+2);
                    
                    for(int j = 0; j < num_samples; j++) {
                        (*texture)->set_pixel(starting_index + j,
                                              color);
                    }
                }
                TexturePool::get_instance()->update_texture(bufnum);
            }
            break;}
        case cmd_b_fill: {
            int32_t bufnum = message->getArgAsInt32(0);
            int32_t starting_index, num_samples;
            
            int32_t color;
            
            
            boost::optional<boost::shared_ptr<AbstractTexture> > texture = TexturePool::get_instance()->get_texture(bufnum);
            
            if(texture) {
                for(int32_t i = 1; i < message->getNumArgs(); i+=3) {
                    starting_index = message->getArgAsInt32(i);
                    num_samples = message->getArgAsInt32(i+1);
                    if(message->getArgType(i+1) == ofxOscArgType::OFXOSC_TYPE_INT32)
                        color = message->getArgAsInt32(i+2);
                    else
                        color = (int) message->getArgAsFloat(i+2);
                    
                    (*texture)->fill(starting_index,
                                     num_samples,
                                     color);
                }
                TexturePool::get_instance()->update_texture(bufnum);
            }
            break;}
            
        default:
            if (options->_verbose)
            {
                std::cout << "[OscHandler]: Warning: Unknown OSC message-> Path: " << message->getAddress() << std::endl;
            }
            break;
    }
    scgraph->unlock();
    // std::cout << "handle message locked end" << std::endl;
}
#endif

void OscHandler::send_done (std::string command, osc::IpEndpointName endpoint)
{
    
    osc::OutboundPacketStream p (_message_buffer, SCGRAPH_OSC_MESSAGE_BUFFER_SIZE);
    
    try
    {
        p << osc::BeginMessage ("/done")
        << command.c_str()
        << osc::EndMessage;
        
        _socket->SendTo (endpoint, p.Data (), p.Size ());
    }
    catch (osc::Exception &e)
    {
        std::cout << "[OscHandler]: Error: " << e.what () << std::endl;
    }
}

void OscHandler::send_done (std::string command, string ip, int port)
{
    ofxOscMessage msg;
    std::cout << "sending done " << command << std::endl;
    //msg.setRemoteEndpoint(ip, port);
    msg.setAddress("/done");
    msg.addStringArg(command);
    _sender->setup(ip, port);
    _sender->sendMessage(msg);
}

void OscHandler::handle_message (OscMessage *message)
{
    lock();
    //std::cout << "handle message" << std::endl;
    
    handle_message_locked (message);
    
    //std::cout << "handle message done" << std::endl;
    
    unlock();
    delete message;
}



