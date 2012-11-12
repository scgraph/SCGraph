GServer : Server {
	classvar <>graphics;
	classvar <>program;
	classvar <>default;

	init { arg argName, argAddr, argOptions, argClientID;
		argOptions = argOptions ? GServerOptions.new;
		if (argAddr.isNil, { argAddr = NetAddr("127.0.0.1", 37291) });		
		super.init(argName, argAddr, argOptions, argClientID);
	}

	*initClass {
		Platform.switch(\windows, {
			program = "scgraph.exe";
		}, {
			program = "scgraph";
		});
		default = GServer.new(\graphics, NetAddr("127.0.0.1", 37291));
	}

	// same as Server.sc, but don't try to create the default group
	initTree {
		nodeAllocator = NodeIDAllocator(clientID, options.initialNodeID);
		//this.sendMsg("/g_new", 1, 0, 0);
		tree.value(this);
		ServerTree.run(this);
	}

	// same as in Server.sc, but without SHM
	boot { arg startAliveThread=true, recover=false;
		var resp;
		if (serverRunning, { "server already running".inform; ^this });
		if (serverBooting, { "server already booting".inform; ^this });

		serverBooting = true;
		if(startAliveThread, { this.startAliveThread });
		if(recover) { this.newNodeAllocators } { this.newAllocators };
		bootNotifyFirst = true;
		this.doWhenBooted({
			serverBooting = false;
			if (sendQuit.isNil) {
				sendQuit = not(this.inProcess) and: {this.isLocal};
			};
			
			/*
			if (this.inProcess) {
				serverInterface = ServerShmInterface(thisProcess.pid);
			} {
				if (isLocal) {
					serverInterface = ServerShmInterface(addr.port);
				}
			};
			*/
			this.initTree;
			(volume.volume != 0.0).if({
				volume.play;
				});
		});
		if (remoteControlled.not, {
			"You will have to manually boot remote server.".inform;
		},{
			this.bootServerApp;
		});
	}

	verbosity {
		^this.options.verbosity
	}

	verbosity_ { arg verbosityLevel = 0;
		this.options.verbosity_(verbosityLevel);
		this.sendMsg("/verbose", verbosityLevel)
	}

	controlRate {
		^this.options.controlRate
	}

	controlRate_ { arg controlRate = 100;
		this.options.controlRate_(controlRate);
		this.sendMsg("/controlRate", controlRate)
	}

	graphicsRate {
		^this.options.graphicsRate
	}

	graphicsRate_ { arg graphicsRate = 0;
		this.options.graphicsRate_(graphicsRate);
		this.sendMsg("/graphicsRate", graphicsRate)
	}

	bootServerApp {
		pid = (options.pathsAsExports ++ 
			program ++ options.asOptionsString(addr.port)).unixCmd;
		("booting " ++ addr.port.asString).inform;
	}

	loadTexture { |path|
		this.sendMsg("/loadTexture", path)
	}
}

GServerOptions : ServerOptions
{
	var <>controlRate = 100;
	var <>graphicsRate = 50;
	var <>realtimePriority = 0;

	var <>synthDefPath;
	var <>texturePath;
	var <>shaderPath;
	var <>pluginPath;
	var <>scPluginPath;
	var <>recordingPath;

	var <>persistentSynth;

	*new {
		^super.new.init()
	}

	init {
		// set the right defaults for a GServer
		numAudioBusChannels = 10;
		numPrivateAudioBusChannels = 9;
		numOutputBusChannels = 1;
		numControlBusChannels = 1024;
		numInputBusChannels = 0;
		
		protocol = \udp;
	}

	protocol_ {
		warn("SCGraph only supports udp.")
	}

	pathsAsExports {
		var o = "";

		[
			synthDefPath, "SCGRAPH_SYNTHDEF_PATH",
			texturePath, "SCGRAPH_TEXTURE_PATH",
			shaderPath, "SCGRAPH_SHADER_PATH",
			pluginPath, "SCGRAPH_PLUGIN_PATH",
			scPluginPath, "SCGRAPH_SC_PLUGIN_PATH",
			recordingPath, "SCGRAPH_RECORDING_PATH"].pairsDo{
				|path,envVar|
				if(path.notNil, {
					if(File.exists(path), {
						o = o ++ " " ++ envVar ++ "=\"" ++ path ++ "\"";
					}, {
						warn("Path doesn't exist: " ++ path);
					});
				});
			};

		if(o == "", {
			^o 
		},{
			^("export " ++ o ++ ";")
		});
	}
	
	asOptionsString { | port = 37291 |
		var o;
		// just use UDP anyways
		o = " -u " ++ port;

		if (numControlBusChannels != 4096, {
			o = o ++ " -c " ++ numControlBusChannels;
		});
		if (this.numGraphicsBusChannels != 10, {
			o = o ++ " -g " ++ this.numGraphicsBusChannels;
		});
		if (verbosity != 0, {
			o = o ++ " -v " ++ verbosity.clip(0,5);
		});
		if (controlRate.notNil, {
			o = o ++ " -f " ++ controlRate;
		});
		if (graphicsRate.notNil, {
			o = o ++ " -r " ++ graphicsRate;
		});
		if (realtimePriority != 0, {
			o = o ++ " -p " ++ realtimePriority;
		});
		if (persistentSynth.notNil, {
			if(File.exists(persistentSynth), {
				o = o ++ " -s " ++ persistentSynth;
			}, {
				warn("File doesn't exist: " ++ persistentSynth);
			});
		});

		^o
	}

	numGraphicsBusChannels {
		^numPrivateAudioBusChannels + numInputBusChannels + numOutputBusChannels
	}

	numAudioBusChannels {
		^this.numGraphicsBusChannels()
	}

}
