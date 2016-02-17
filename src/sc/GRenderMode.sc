GRenderMode : GraphicsUGen
{
	*gr
	{
		arg in, mode = 0, // 0: normal, 1: wireframe
		strength = 1.0; 
		^this.multiNew ('audio', in, mode, strength);
	}

	checkInputs {
		^this.checkAllInputs(1);
	}
}