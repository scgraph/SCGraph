GStretch : GraphicsUGen {
	*gr { arg in, stretch_vector = [1, 1, 1];
		^this.multiNew('audio', in, stretch_vector[0], stretch_vector[1], stretch_vector[2]);
	}

	checkInputs {
		^this.checkAllInputs(1);
	}
}

