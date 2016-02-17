GraphicsUGen : UGen {
	
	isValidGraphicsUGenInput { ^true }

	checkValidInputs {
		super.checkValidInputs;
		inputs.do({arg in,i;
			var argName;
			if(((in.rate == 'audio') 
				and: in.class.superclasses.indexOf(GraphicsUGen).isNil
				and: in.class.superclasses.indexOf(AbstractIn).isNil
				and: in.isKindOf(BinaryOpUGen).not
				and: in.isKindOf(Sum3).not
				and: in.isKindOf(Sum4).not
				and: in.isKindOf(MulAdd).not
				and: in.isKindOf(OutputProxy).not)
				or: (in.rate == 'demand'),{ 
					argName = this.argNameForInputAt(i) ? i;
					^"arg: '" ++ argName ++ "' has bad input:" + in;
				})
		});
		^nil
	}

	checkNInputs { arg n;
		if (rate == 'audio') {
			n.do {| i |
				if (inputs.at(i).rate != 'audio') {
						^("input " ++ i ++ " is not audio rate: " 
							+ inputs.at(i) + inputs.at(0).rate);
					};
			};
		};
		^this.checkValidInputs
	}

	checkAllInputs { arg numAudioIns = 0;
		inputs[numAudioIns..].do({arg in,i;
			var argName;
			if((in.rate == 'audio')
				or: (in.rate == 'demand'), { 
					argName = this.argNameForInputAt(i) ? i;
					^"arg: '" ++ argName ++ "' has bad input:" + in;
				})
		});
		^this.checkNInputs(numAudioIns)
	}
}
