+ In {
	*gr { arg bus = 0, numChannels = 1;
		^this.ar(bus, numChannels)
	}
}

+ Out {
	*gr { arg bus, channelsArray;
		^this.ar(bus, channelsArray)
	}
}

+ ReplaceOut {
	*gr { arg bus, channelsArray;
		^this.ar(bus, channelsArray)
	}
}