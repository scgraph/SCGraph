GColor : GraphicsUGen {
	*gr { arg in, color = [1, 1, 1, 1];
		^this.multiNew('audio', in, color[0], color[1], color[2], color[3]);
	}

	checkInputs {
		^this.checkAllInputs(1)
	}
}

+ Color {
	gr {|in|
		^GColor.gr(in, this.asArray)
	}

	asInt {
		var fl = 0;
		this.asArray255.asInteger.do{ |i,ind| fl = fl + (i << (8 * ind)); };
		^fl;
	}
}