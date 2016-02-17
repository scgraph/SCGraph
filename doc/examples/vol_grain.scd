g = GServer.default;
g.boot;

(
SynthDef(\PointGrain, {
	arg x,y,z,alpha;
	var source, env;

	env = alpha * EnvGen.kr(Env.perc(releaseTime: 1.0, level: 0.4), doneAction: 2);

	source =  GColor.gr(GTranslate.gr(
		GTexQuad.gr(tex_index: 6), [x,y,z]), [1, 1, 1, env]);

	Out.gr(0, source);
}).send(g)
)

p = Synth.new(\PointGrain, args: [\x, 10.0.rand-5, \y, 10.0.rand-5, \z, 10.0.rand-5, \alpha, 1], target: g);

(
SynthDef(\VerySimpleView, {
	GGLRenderer.gr(
		GBlending.gr(on: 1) 
		+ In.gr(0) 
		+ GBlending.gr(on: 0), 
		texturing: 1, 
		transparency: 1, 
		clear_mode: 1, 
		clear_color: [0,0,0,0.03]);
}).send(g)
)

v = Synth.new(\VerySimpleView, addAction: \addToTail, target: g)

(
var x,y,z, alpha;
l = Task ({
	loop {
		10.do (
		{
			arg i;
			x = 10.0.rand-5.0;
			y = 10.0.rand-5.0;
		  	z = 10 - i;//10.0.rand-5;
		 	alpha = (x+5.0)/1600.0;
			Synth.new(\PointGrain, target: g,
				args: [\x, x, \y, y, \z, z, \alpha, alpha]);
			0.005.wait;
		});
	  	0.2.wait;
	}
})
)

l.start
l.stop

p = Synth.new(\PointGrain, target: g, args: [\x, 10.0.rand-5, \y, 10.0.rand-5, \z, 10.0.rand-5, \alpha, 1]);