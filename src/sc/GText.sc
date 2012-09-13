GText : UGen {
	*gr { arg fontSize = 12, stringID=0;
		^this.multiNew(
			'audio', 
			fontSize,
			stringID);
	}
}

