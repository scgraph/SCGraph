GFreqAmp : GraphicsUGen {
	*kr { arg port = 0, freq = 440; // default is nyquist/2
		^this.multiNew('control', port, freq);
	}

	checkInputs {
		^this.checkAllInputs(0)
	}
}

