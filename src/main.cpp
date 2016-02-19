/* Copyright 2006 Florian Paul Schmidt */

#include "ofMain.h"
#include "ofApp.h"


#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include <signal.h>

#include "options.h"
#include "plugin_pool.h"
#include "texture_pool.h"

#ifdef HAVE_FTGL
#include "string_pool.h"
#endif

#include "scgraph.h"

bool signalled_again = false;

void signal_handler (int signal)
{
    if (signalled_again)
    {
        std::cout << "[SignalHandler]: Signalled again - performing unclean exit" << std::endl;
        exit (EXIT_SUCCESS);
    }
    
    Options *options = Options::get_instance ();
    
    if (signal == SIGINT || signal == SIGTERM)
    {
        if (options->_verbose)
            std::cout << "[SignalHandler]: Caught SIGINT or SIGTERM, exiting.." << std::endl;
        
        signalled_again = true;
        // TODO QApplication::instance()->exit ();
    }
}

int main (int argc, char *argv[])
{
    //  std::cout << "ScGraph "<< scgraph_VERSION_MAJOR << "." <<
    //  scgraph_VERSION_MINOR << "- (C) 2006, 2007, 2008, 2011"<<
    //  std::endl;
    std::cout << "ScGraph 0.20 - (C) 2006-2016"<< std::endl;
    std::cout << "[Run scgraph -h for help]" << std::endl;
    
    /* create QApplication object before option object so it can
     modify the argument list */
    //QApplication qapp (argc, argv);
    /*QIcon icon;
    icon.addFile(":icons/scgraph-cube-128");
    icon.addFile(":icons/scgraph-cube-48");
    icon.addFile(":icons/scgraph-cube-32");
    icon.addFile(":icons/scgraph-cube-16");
    qapp.setWindowIcon(icon);
    
    
    QGLFormat f;
    f.setDoubleBuffer(true);
    QGLFormat::setDefaultFormat(f);
     */
    
    /* pretty much everything else needs access to this. Especially stuff in
     ScGraph:: */
    Options *options = Options::get_instance (argc, argv);
    
    TexturePool::get_instance ();
    
    PluginPool::get_instance ();
    
#ifdef HAVE_FTGL
    StringPool::get_instance ();
#endif
    
    ScGraph *scgraph = ScGraph::get_instance (argc, argv);
    
    signal (SIGINT, signal_handler);
    signal (SIGTERM, signal_handler);
    
    /* setup a signal handler for clean shutdown */
    if (options->_verbose)
        std::cout << "[Main]: Up and running..." << std::endl;
    
    scgraph->start ();
    //qapp.exec ();
    
    //ofSetupOpenGL(100,100,OF_WINDOW);			// <-------- setup the GL context
    
    // this kicks off the running of my app
    // can be OF_WINDOW or OF_FULLSCREEN
    // pass in width and height too:
    ofGLFWWindowSettings settings;
    settings.width = 60;
    settings.height = 60;
    settings.setPosition(ofVec2f(300,0));
    settings.resizable = true;
    settings.title = "scgraph status";
    shared_ptr<ofAppBaseWindow> mainWindow = ofCreateWindow(settings);

    shared_ptr<ofApp> mainApp(new ofApp);
    ofRunApp(mainWindow, mainApp);
    ofRunMainLoop();
    
    if (options->_verbose)
        std::cout << "[Main]: Performing shutdown" << std::endl;
    
    scgraph->stop ();
    
    delete scgraph;
    
    if (options->_verbose >= 2)
        std::cout << "[Main]: ScGraph gone.." << std::endl;
    
    int verbose = options->_verbose;
    delete options;
    
    if (verbose >= 2)
        std::cout << "[Main]: Options gone.." << std::endl;
    
    
    std::cout << "Bye." << std::endl;

    
    /* whoohoo */
    exit (EXIT_SUCCESS);
}

