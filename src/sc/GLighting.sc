GLighting : GraphicsUGen
{
	*gr
	{
		arg on = 1;
		^this.multiNew ('audio', on);
	}

	checkInputs {
		^this.checkAllInputs(0);
	}
}