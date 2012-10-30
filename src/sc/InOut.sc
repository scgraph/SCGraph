+ In {
	*gr { arg bus = 0, numChannels = 1;
		^this.ar(bus, numChannels)
	}
}

+ InFeedback {
	*gr { arg bus = 0, numChannels = 1;
		^this.ar(bus, numChannels)
	}
}


+ XFade2 {
	*gr { arg inA, inB = 0.0, pan = 0.0, level = 1.0;
		^this.ar(inA, inB = 0.0, pan = 0.0, level = 1.0)
	}
}


+ Out {
	*gr { arg bus, channelsArray;
		^this.ar(bus, channelsArray)
	}
}

+ XOut {
	*gr { arg bus, xfade, channelsArray;
		^this.ar(bus, xfade, channelsArray)
	}
}

+ ReplaceOut {
	*gr { arg bus, channelsArray;
		^this.ar(bus, channelsArray)
	}
}