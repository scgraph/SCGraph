GRenderMode : UGen
{
	*gr
	{
		arg in, mode = 0, // 0: normal, 1: wireframe
		strength = 1.0; 
		^this.multiNew ('audio', in, mode, strength);
	}
}