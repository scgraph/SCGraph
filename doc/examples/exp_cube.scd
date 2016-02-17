g = GServer.default;
g.boot;

(
SynthDef(\ExpCube,
	{
		var cube1, cube2, env;

		env = Line.kr(1,0,4,doneAction: 2);

		cube1 = GTranslate.gr 
		(
			GStretch.gr(
				GCube.gr,
				[
					env,
					env,
					env
				]
			),
			[
				(1+(1*In.kr(0))) * (20 * SinOsc.kr(freq: 0.5.rand, phase: (pi/2).rand)),
				(1+(5*In.kr(1))) * (20 * SinOsc.kr(freq: 0.5.rand, phase: (pi/2).rand)),
				(1+(10*In.kr(2))) * (20 * SinOsc.kr(freq: 0.5.rand, phase: (pi/2).rand))
			]
		);

		Out.gr(
			0,
			cube1;
		);
	}).send(g)
)


(
SynthDef(\SimpleView,
	{
		arg in = 0;

		var light;

		light = GLight.gr 
		(
			spot_direction: [-10*SinOsc.kr(1),-5,-10*SinOsc.kr(1,phase: 1.7)], 
			//specular_color: Color.rand.asArray, 
			diffuse_color: Color.rand.asArray, 
			spot_cutoff: 180,
			position: [10*SinOsc.kr(1),5,10*SinOsc.kr(1, phase: 1.7),1]
		)
		+
		GLight.gr 
		(
			spot_direction: [10*SinOsc.kr(1),5,10*SinOsc.kr(1,phase: 1.7)], 
			spot_cutoff: 180,
			//specular_color: Color.rand.asArray, 
			diffuse_color: Color.rand.asArray, 
			position: [-10*SinOsc.kr(1),-5,-10*SinOsc.kr(1,phase: 1.7),1]
		)
		;

		GGLRenderer.gr
		(
			light + In.gr(in),
			fov: 90,
			near_plane: 0.1,
			far_plane: 50,
			lighting: 1,
			culling: 1,
			eye: [9 * SinOsc.kr(freq: 0.13), 10 * SinOsc.kr(freq: 0.11), 13 * SinOsc.kr(freq: 0.09, phase: pi/2)],
			fog: 1,
			fog_density: 1.0,
			fog_start: 10.0,
			fog_end: 50.0,
			fog_color: [0.0, 0.0, 0.0, 1.0],
			clear_color: [0.0, 0.0, 0.0, 0.05],
			clear_mode: 0
		);
	}
).send(g);
)
v = Synth.new(\SimpleView, target: g, addAction: \addToTail, args: [\in, 0]);


(
e = Task({
	loop {
		10.do({
			Synth.new(\ExpCube, target: g, addAction: \addToHead);
			0.02.wait;
		});
		0.2.wait;
	}
})
)

e.start
e.stop
