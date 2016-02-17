g = GServer.default;
g.boot;

(
SynthDef (\Testdust,
{
	var factor, cube;

	factor = Array.fill(3,{Decay.kr(Dust.kr(10))});

	cube = GColor.gr(GCube.gr, factor ++ 1);

	GGLRenderer.gr (GTranslate.gr(GStretch.gr(cube, 1+factor), [SinOsc.kr(freq: 0.1),0,0]), clear_color: [0,0,0.1,0.1], clear_mode: 1);
	// GGLRenderer.ar (GCube.ar);
}).play(g)
)
