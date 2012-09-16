GText : UGen {
	*gr { arg fontSize = 1, stringID=0;
		^this.multiNew(
			'audio', 
			fontSize,
			stringID);
	}
}

