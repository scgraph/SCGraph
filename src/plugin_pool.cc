#include "plugin_pool.h"

#include "sc_unit_wrapper.h"

#include "options.h"

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 600
#endif

#include <stdlib.h>
#include <stdarg.h>

#include <sstream>
#include <iostream>
#include <cmath>
#include <cstring>
#include <stdexcept>

#include <sys/types.h>
#include <dirent.h>

#include <assert.h>
#include <dlfcn.h>

#include "in_out.h"
#include "control.h"

#include <SC_Wire.h>
#include <SC_RGen.h>

#include <SC_InterfaceTable.h>
#include <SC_Node.h>
#include <SC_World.h>
#include <SC_Graph.h>

#ifdef SC36
#include <SC_Group.h>
#endif


#include "sc__unit.h"

bool fDefineUnit (const char *inUnitClassName, size_t inAllocSize, UnitCtorFunc inCtor, UnitDtorFunc inDtor, uint32 inFlags);

bool fDefineBufGen (const char *inName, BufGenFunc inFunc);

bool fDefinePlugInCmd (const char *inCmdName, PlugInCmdFunc inFunc, void* inUserData);


// call printf for debugging. should not use in finished code.
int fPrint(const char *fmt, ...);

// get a seed for a random number generator
int32 fRanSeed();

// define a command for a unit generator  /u_cmd
bool fDefineUnitCmd(const char *inUnitClassName, const char *inCmdName, UnitCmdFunc inFunc);

// clear all of the unit's outputs.
void fClearUnitOutputs(Unit *inUnit, int inNumSamples);

// non real time memory allocation
void* fNRTAlloc(size_t inSize);
void* fNRTRealloc(void *inPtr, size_t inSize);
void  fNRTFree(void *inPtr);

// real time memory allocation
void* fRTAlloc(World *inWorld, size_t inSize);
void* fRTRealloc(World *inWorld, void *inPtr, size_t inSize);
void  fRTFree(World *inWorld, void *inPtr);

// call to set a Node to run or not.
void fNodeRun(struct Node* node, int run);

// call to stop a Graph after the next buffer.
void fNodeEnd(struct Node* graph);

// send a trigger from a Node to clients
void fSendTrigger(struct Node* inNode, int triggerID, float value);

// sending messages between real time and non real time levels.
bool fSendMsgFromRT(World *inWorld, struct FifoMsg& inMsg);
bool fSendMsgToRT(World *inWorld, struct FifoMsg& inMsg);

// libsndfile support
int fSndFileFormatInfoFromStrings(SF_INFO *info,
		const char *headerFormatString, const char *sampleFormatString);

// get nodes by id
struct Node* fGetNode(World *inWorld, int inID);
struct Graph* fGetGraph(World *inWorld, int inID);

void fNRTLock(World *inWorld);
void fNRTUnlock(World *inWorld);

void fGroup_DeleteAll(struct Group* group);
void fDoneAction(int doneAction, struct Unit *unit);

PluginPool *PluginPool::_instance = 0;

PluginPool *PluginPool::get_instance ()
{
	if (!_instance)
	{
		_instance = new PluginPool;
		_instance->init();
	}

	return _instance;
}

PluginPool::PluginPool ()
{
	_sc_interface_table = new InterfaceTable;

	_sc_interface_table->fDefineUnit                   = fDefineUnit;
	_sc_interface_table->fDefineBufGen                 = fDefineBufGen;
	_sc_interface_table->fDefinePlugInCmd              = fDefinePlugInCmd;
	_sc_interface_table->fPrint                        = fPrint;
	_sc_interface_table->fRanSeed                      = fRanSeed;
	_sc_interface_table->fDefineUnitCmd                = fDefineUnitCmd;
	_sc_interface_table->fClearUnitOutputs             = fClearUnitOutputs;
	_sc_interface_table->fNRTAlloc                     = fNRTAlloc;
	_sc_interface_table->fNRTRealloc                   = fNRTRealloc;
	_sc_interface_table->fNRTFree                      = fNRTFree;
	_sc_interface_table->fRTAlloc                      = fRTAlloc;
	_sc_interface_table->fRTRealloc                    = fRTRealloc;
	_sc_interface_table->fRTFree                       = fRTFree;
	_sc_interface_table->fNodeRun                      = fNodeRun;
	_sc_interface_table->fNodeEnd                      = fNodeEnd;
	_sc_interface_table->fSendTrigger                  = fSendTrigger;
	_sc_interface_table->fSendMsgFromRT                = fSendMsgFromRT;
	_sc_interface_table->fSendMsgToRT                  = fSendMsgToRT;
	_sc_interface_table->fSndFileFormatInfoFromStrings = fSndFileFormatInfoFromStrings;
	_sc_interface_table->fGetNode                      = fGetNode;
	_sc_interface_table->fGetGraph                     = fGetGraph;
	_sc_interface_table->fNRTLock                      = fNRTLock;
	_sc_interface_table->fNRTUnlock                    = fNRTUnlock;
	_sc_interface_table->fGroup_DeleteAll              = fGroup_DeleteAll;
	_sc_interface_table->fDoneAction                   = fDoneAction;

	_sc_interface_table->mSineWavetable                = new float[2*128+1];
	_sc_interface_table->mSine                         = new float[128+1];
	_sc_interface_table->mCosecant                     = new float[128+1];
	_sc_interface_table->mSineSize                     = 128;

	_sc_world = new World;

	_sc_world->mRGen = new RGen;
	_sc_world->mRGen->init (573);

	_sc_graph = new Graph;

	_sc_graph->mRGen = new RGen;
	_sc_graph->mRGen->init (263);

	for (int i = 0; i < 128+1; ++i)
	{
		// std::cout << sin(2.0*M_PI*i/128.0) << std::endl;
		_sc_interface_table->mSine[i] = sin(2.0*M_PI*i/128.0);
		_sc_interface_table->mCosecant[i] = 1/(sin(2.0*M_PI*i/128.0));
	}

	float *in = _sc_interface_table->mSine;
	float *out = _sc_interface_table->mSineWavetable - 1;

	float val1, val2;
	for (int i = 0; i < 128; ++i)
	{
		// std::cout << sin(2.0*M_PI*i/128.0) << std::endl;
		val1 = in[i];
		val2 = in[i+1];
		*++out = 2.f * val1 - val2;
		*++out = val2 - val1;
	}
	val1 = in[128-1];
	val2 = in[0];
	*++out = 2.f * val1 - val2;
	// *++out = val2 - val1;
}

void PluginPool::init ()
{
	Options *options = Options::get_instance ();

	// traverse plugin paths and add plugins to pool
	tokenize_path (options->_sc_plugin_path, &PluginPool::add_sc_plugin_dir);
	tokenize_path (options->_plugin_path, &PluginPool::add_plugin_dir);
}


PluginPool::~PluginPool ()
{
#if 0
	for (size_t i = 0; i < _sc_plugins.size (); ++i)
		delete _sc_plugins [i];

	for (size_t i = 0; i < _plugins.size (); ++i)
		delete _plugins [i];
#endif
}

void PluginPool::tokenize_path (const std::string &path, void (PluginPool::*function)(const std::string &directory))
{
	std::stringstream path_stream (path);

	// FIXME: dirty hack
	char buffer [PATH_MAX];
	
	while (path_stream.getline (buffer, PATH_MAX, ':'))
	{
		if (std::string (buffer) == "")
			continue;

		// std::cout << "directory: " << buffer << std::endl;
		(*this.*function) (buffer);
	}
}

void PluginPool::traverse_directory (const std::string &directory, void (PluginPool::*function)(const std::string &filename))
{
	DIR *dir = opendir (directory.c_str ());
	if (!dir)
		throw ("[PluginPool]: Couldn't read directory");

	struct dirent *dirent;
	while ((dirent = readdir (dir)))
	{
		if ((std::string (dirent->d_name) == std::string (".")) || (std::string (dirent->d_name) == std::string ("..")))
			continue;
 
		if (dirent->d_type == DT_DIR)
			continue;

		else
		{
			std::string filename = directory + "/" + std::string (dirent->d_name);

			(*this.*function) (filename);
		}
	}

	closedir (dir);
}

void PluginPool::add_sc_plugin_dir (const std::string &directory)
{
	Options *options = Options::get_instance ();

	if (options->_verbose >= 2)
		std::cout << "[PluginPool]: Adding SuperCollider plugin directory: " << directory << std::endl;

	try
	{
		traverse_directory (directory, &PluginPool::add_sc_plugin);
	}
	catch (const char *e)
	{
		std::cout << "[PluginPool]: Failed to add directory: " << directory << "  Reason: " << e << std::endl;
	}
}



void PluginPool::add_plugin_dir (const std::string &directory)
{
	Options *options = Options::get_instance ();

	if (options->_verbose >= 2)
		std::cout << "[PluginPool]: Adding ScGraph plugin directory: " << directory << std::endl;

	// traverse_directory (directory, &PluginPool::add_plugin);
	try
	{
		traverse_directory (directory, &PluginPool::add_plugin);
	}
	catch (const char *e)
	{
		std::cout << "[PluginPool]: Failed to add directory: " << directory << "  Reason: " << e << std::endl;
	}
}

void PluginPool::add_sc_plugin (const std::string &filename)
{
	Options *options = Options::get_instance ();

	if (options->_verbose >= 2)
		std::cout << "  [PluginPool]: Adding SuperCollider plugin: " << filename << std::endl;

	try 
	{
		// we can never close the files due to how SuperCollider plugins work
		void *dl;
		dl = dlopen (filename.c_str (), RTLD_NOW);
		if (!dl)
		{
			throw (std::string("[ScPlugin]: Error: dlopen failed! \n    Reason: ") + std::string (dlerror()));
		}
	
		// FIXME: this hack is due to -pedantic not working fine with dlsym
		void (*load) (InterfaceTable *ft);
		void *ptr = dlsym (dl, "load");
		assert (sizeof (load) == sizeof (ptr));
		memcpy (&load, &ptr, sizeof (ptr));
	
		if (!load)
		{
			dlclose (dl);
			throw ("[ScPlugin]: Error: Couldn't find symbol \"load\". This doesn't look like a SuperCollider plugin");
		}
		
		load (_sc_interface_table);

	}
	catch (std::string e)
	{
		std::cout << "[PluginPool]: Error loading SuperCollider plugin: " << filename << "." << std::endl << "  Reason: " << e << std::endl;
	}
}

void PluginPool::add_plugin (const std::string &filename)
{
	Options *options = Options::get_instance ();

	if (options->_verbose >= 2)
		std::cout << "  [PluginPool]: Adding ScGraph plugin: " << filename << std::endl;

	try 
	{
		boost::shared_ptr<GPlugin> plugin (new GPlugin (filename));
		_plugins.push_back (plugin);
	}
	catch (std::string e)
	{
		std::cout << "[PluginPool]: Error loading Scgraph plugin: " << filename << "." << std::endl << "  Reason: " << e << std::endl;
	}
}

unsigned int PluginPool::get_number_of_plugins ()
{
	return _plugins.size ();
}

boost::shared_ptr<GPlugin> PluginPool::get_plugin (unsigned int index)
{
	return _plugins[index];
}

boost::shared_ptr<GUnit> PluginPool::create_unit (const std::string &name, int special_index, GSynth *synth)
{
	Options *options = Options::get_instance ();

	if (options->_verbose >= 3)
		std::cout << "[PluginPool]: Creating unit (name: \"" << name << "\")" << std::endl;

	if ((name == "Control") || (name == "LagControl") || (name == "AudioControl"))
	{
		boost::shared_ptr<GUnit> unit (new Control (special_index));

		return unit;	
	}

	if (name == "TrigControl")
	{
		boost::shared_ptr<GUnit> unit (new TrigControl (special_index));

		return unit;	
	}

	if (name == "InFeedback")
	{
		boost::shared_ptr<GUnit> unit (new InFeedback ());

		return unit;
		}

	if (name == "In")
	{
		boost::shared_ptr<GUnit> unit (new In ());

		return unit;
		}

	if (name == "XFade2")
	{
		boost::shared_ptr<GUnit> unit (new XFade2 ());

		return unit;
		}

	if (name == "XOut")
	{
		boost::shared_ptr<GUnit> unit (new XOut ());

		return unit;
	}

	if (name == "Out")
	{
		boost::shared_ptr<GUnit> unit (new Out ());

		return unit;
	}

	if (name == "ReplaceOut")
	{
		boost::shared_ptr<GUnit> unit (new ReplaceOut ());

		return unit;
	}


	/* first we check for ScGraph Units */
	// TODO: if a unit with matching name exists, but
	// it doesn't have a control rate function, then search for
	// supercollider plugin
	for (size_t i = 0; i < _plugins.size (); ++i)
	{
		for (size_t j = 0; j < _plugins[i]->get_num_of_units (); ++j)
		{
			if (_plugins[i]->get_unit_name (j) == name)
				{
					boost::shared_ptr<GUnit> unit(_plugins[i]->create_unit (j, special_index));
					return unit;
				}
		}
	}

	/* then we look for SuperCollider Plugins */
	for (size_t i = 0; i < _sc_units.size (); ++i)
	{
		if (_sc_units[i]->_name == name)
		{
			/* found one. so create a Wrapper for it */
			boost::shared_ptr<GUnit> sc_unit (new ScUnitWrapper (special_index,_sc_units[i]));
			return sc_unit;
		}
	}
	throw (std::runtime_error (("[PluginPool]: Error: Could not find unit: ") + name));
}

void PluginPool::add_sc_unit (boost::shared_ptr<ScUnit> unit)
{
	_sc_units.push_back(unit);
}


void PluginPool::destroy_sc_unit(Unit *u) {
	if (!u) return;

	for (unsigned int i = 0; i < u->mNumInputs; ++i) {
		delete [] u->mInBuf[i];
		delete u->mInput[i];
	}
	for (unsigned int i = 0; i < u->mNumOutputs; ++i) {
		delete [] u->mOutBuf[i];
		delete u->mOutput[i];
	}
	delete u->mRate;
	delete [] u->mInBuf;
	delete [] u->mInput;
	delete [] u->mOutBuf;
	delete [] u->mOutput;
	free(u);
}

Unit *PluginPool::create_sc_unit (const std::string &name, int special_index, int parent_index, int num_inputs, int num_outputs, int calc_rate, double rate, int buf_length)
{
	void *mem = 0;

	for (size_t i = 0; i < _sc_units.size (); ++i)
	{
		if (_sc_units[i]->_name == name)
		{
			// std::cout << "memalign: " << 8 << " " << _sc_units[i]->_size << " " << sizeof (void*) << std::endl;
			//if ((posix_memalign (&mem, 16, _sc_units[i]->_size)))
			//if (!memalign(&mem, 8, _sc_units[i]->_size))
			if (!(mem = malloc(_sc_units[i]->_size)))
			{
				std::cout << "[PluginPool]: Error: posix_memalign() failed. Exiting" << std::endl;
				exit (EXIT_FAILURE);
			}

			Unit *unit = static_cast<Unit*>(mem);

			unit->mCalcRate = calc_rate;
			unit->mNumInputs = num_inputs;
			unit->mNumOutputs = num_outputs;
			unit->mSpecialIndex = special_index;
			unit->mParentIndex = parent_index;

			unit->mRate = new Rate;
			unit->mRate->mSampleRate = rate;
			unit->mRate->mBufRate = (double)rate/(double)buf_length;
			unit->mRate->mBufDuration = (double)buf_length/(double)rate;
			unit->mRate->mSampleDur = 1.0/(double)rate;
			//unit->mRate->mSampleDur = unit->mRate->mBufDuration;
			unit->mRate->mRadiansPerSample = 2.0 * M_PI/(double)rate;
			unit->mRate->mBufLength = buf_length;
			unit->mRate->mSlopeFactor = 1;

			unit->mCalcFunc = 0;

			unit->mInBuf = new float*[num_inputs];
			unit->mInput = new struct Wire*[num_inputs];

			for (int j = 0; j < num_inputs; ++j)
			{
				unit->mInBuf[j] = new float[buf_length];
				unit->mInput[j] = new Wire;
				unit->mInput[j]->mFromUnit = unit;
				unit->mInput[j]->mCalcRate = calc_rate;
			}


			unit->mOutBuf = new float*[num_outputs];
			unit->mOutput = new struct Wire*[num_outputs];

			for (int j = 0; j < num_outputs; ++j)
			{
				unit->mOutBuf[j] = new float[buf_length];
				unit->mOutput[j] = new Wire;
				unit->mOutput[j]->mFromUnit = unit;
				unit->mOutput[j]->mCalcRate = calc_rate;
			}

			unit->mBufLength = buf_length;

			unit->mWorld = _sc_world;
			unit->mParent = _sc_graph;

			//_sc_units[i]->_ctor(unit);
			break;
		}
	}

	return reinterpret_cast<Unit*>(mem);
}


void PluginPool::set_current_synth (GSynth *synth)
{
	_current_synth = synth;
}

GSynth *PluginPool::get_current_synth ()
{
	return _current_synth;
}



bool fDefineUnit (const char *inUnitClassName, size_t inAllocSize, UnitCtorFunc inCtor, UnitDtorFunc inDtor, uint32 inFlags)
{
	// std::cout << "[Warning]: fDefineUnit not supported" << std::endl;

	boost::shared_ptr<ScUnit> unit (new ScUnit);

	unit->_name = inUnitClassName;
	unit->_ctor = inCtor;
	unit->_dtor = inDtor;
	unit->_size = inAllocSize;

	PluginPool *pool = PluginPool::get_instance();

	pool->add_sc_unit (unit);

	Options *options = Options::get_instance ();

	if (options->_verbose >= 3)
		std::cout << "   [define_unit]: name: " << inUnitClassName << std::endl;

	return true;
}

bool fDefineBufGen (const char *inName, BufGenFunc inFunc)
{
	std::cout << "[Warning]: fDefineBufGen not supported" << std::endl;

	Options *options = Options::get_instance ();

	if (options->_verbose >= 3)
		std::cout << "[define_buf_gen]: name: " << inName << std::endl;

	return true;
}

bool fDefinePlugInCmd (const char *inCmdName, PlugInCmdFunc inFunc, void* inUserData)
{
	std::cout << "[Warning]: fDefinePlugInCmd not supported" << std::endl;

	Options *options = Options::get_instance ();

	if (options->_verbose >= 3)
		std::cout << "[define_plugin_command]: name: " << inCmdName << std::endl;

	return true;
}

extern char *itoa(int, char *, int);

// call printf for debugging. should not use in finished code.
int fPrint(const char *fmt, ...)
{
	va_list vargs;
	va_start(vargs, fmt);
	return vprintf(fmt, vargs);
}
/*
int fPrint(const char *fmt, ...)
{
	//std::cout << "[Warning]: fPrint() not supported" << std::endl;

	QString output = new QString(fmt);

	va_list ap;
    int j;
    va_start(ap, count); //Requires the last fixed parameter (to get the address)
    for(j=0; j<count; j++) {
        va_arg(ap); //Requires the type to cast to.
								 //Increments ap to the next argument.
	}
    va_end(ap);
	return 0;
}
*/
// get a seed for a random number generator
int32 fRanSeed()
{
	std::cout << "[Warning]: fRanSeed() not supported. Returning 0.." << std::endl;
	return 0;
}

// define a command for a unit generator  /u_cmd
bool fDefineUnitCmd(const char *inUnitClassName, const char *inCmdName, UnitCmdFunc inFunc)
{
	std::cout << "[Warning]: fDefineUnitCmd() not supported" << std::endl;
	return false;
}

// clear all of the unit's outputs.
void fClearUnitOutputs(Unit *inUnit, int inNumSamples)
{
	std::cout << "[Warning]: fClearUnitOutputs not supported" << std::endl;
}

// non real time memory allocation
void* fNRTAlloc(size_t inSize)
{
	// std::cout << "[Warning]: fNRTAlloc not supported" << std::endl;
	char *mem = new char[inSize];

	return (void *)mem;
}

void* fNRTRealloc(void *inPtr, size_t inSize)
{
	// std::cout << "[Warning]: fNRTRealloc not supported" << std::endl;
	void *mem = realloc (inPtr, inSize);

	return mem;
}

void  fNRTFree(void *inPtr)
{
	// std::cout << "[Warning]: fNRTFree not supported" << std::endl;
	free (inPtr);
}

// real time memory allocation
void* fRTAlloc(World *inWorld, size_t inSize)
{
	// std::cout << "[Warning]: fRTAlloc not supported" << std::endl;
	void *mem = malloc (inSize);
	return mem;
}

void* fRTRealloc(World *inWorld, void *inPtr, size_t inSize)
{
	void *mem = realloc (inPtr, inSize);

	return mem;

	//std::cout << "[Warning]:fRTRealloc  not supported" << std::endl;
}

void  fRTFree(World *inWorld, void *inPtr)
{
	std::cout << "[Warning]: fRTFree not supported" << std::endl;
	free (inPtr);
}


// call to set a Node to run or not.
void fNodeRun(struct Node* node, int run)
{
	std::cout << "[Warning]: fNodeRun not supported" << std::endl;

}

// call to stop a Graph after the next buffer.
void fNodeEnd(struct Node* graph)
{
	std::cout << "[Warning]: fNodeEnd not supported" << std::endl;

}

// send a trigger from a Node to clients
void fSendTrigger(struct Node* inNode, int triggerID, float value)
{
	std::cout << "[Warning]: fSendTrigger not supported" << std::endl;

}

// sending messages between real time and non real time levels.
bool fSendMsgFromRT(World *inWorld, struct FifoMsg& inMsg)
{
	std::cout << "[Warning]: fSendMsgFromRT not supported" << std::endl;
	return false;
}

bool fSendMsgToRT(World *inWorld, struct FifoMsg& inMsg)
{
	std::cout << "[Warning]: fSendMsgToRT not supported" << std::endl;
	return false;
}

// libsndfile support
int fSndFileFormatInfoFromStrings(SF_INFO *info,
		const char *headerFormatString, const char *sampleFormatString)
{
	std::cout << "[Warning]: fSndFileFormatInfoFromStrings not supported" << std::endl;
	return 0;
}

// get nodes by id
struct Node* fGetNode(World *inWorld, int inID)
{
	std::cout << "[Warning]: fGetNode not supported" << std::endl;
	return 0;
}

struct Graph* fGetGraph(World *inWorld, int inID)
{
	std::cout << "[Warning]: fGetGraph not supported" << std::endl;
	return 0;
}


void fNRTLock(World *inWorld)
{
	std::cout << "[Warning]: fNRTLock not supported" << std::endl;

}

void fNRTUnlock(World *inWorld)
{
	std::cout << "[Warning]: fNRTUnlock not supported" << std::endl;

}

void fGroup_DeleteAll(struct Group* group)
{
	std::cout << "[Warning]: fGroup_DeleteAll not supported" << std::endl;
}


void fDoneAction(int doneAction, struct Unit *unit)
{
	PluginPool *plugin_pool = PluginPool::get_instance ();

	if (doneAction != 0)
		plugin_pool->get_current_synth()->set_done_action (doneAction);

	// std::cout << "[Warning]: fDoneAction not supported" << std::endl;
}


