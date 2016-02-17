g = GServer.default;
g.boot;

(
SynthDef (\CoordinateSystem, {
	Out.ar(
		0, 
		GColor.gr(GLine.gr(1.0, [0,0,0], [1,0,0]), Color.red.asArray) 
		+ GColor.gr(GLine.gr(1.0, [0,0,0], [0,1,0]), Color.green.asArray) 
		+ GColor.gr(GLine.gr(1.0, [0,0,0], [0,0,1]), Color.blue.asArray)
	);
}).send(g)
)

e = Synth.new (\CoordinateSystem, target: g, addAction: \addToHead);
e.free

(
SynthDef (\Grain, {
	arg x,y,z, r=1.0, g=1.0, b=1.0, wx=0.0, wy=0.0, wz=0.0, size = 0.01;

	var grain;

	grain = GColor.gr(
		GTranslate.gr(
			GStretch.gr(
				GCube.gr (), 
				[
					size + (size * wx * SinOsc.kr(5)),
					size + (size * wy * SinOsc.kr(5)),
					size + (size * wz * SinOsc.kr(5))
				]
			), 
			[x,y,z]
		), 
		[r,g,b,1]
	);
	Out.gr(0, grain);
}).send(g)
)

(
r = Synth.new(
	\Grain, 
	args: 
	[
		\x,-0.225,
		\y, 0.235,
		\z, 0.2
		\wx, 0.1,
		\wy, 0.1,
		\wz, 0.3,
	],
	target:v, 
	addAction: \addBefore)
)
r.free

(
SynthDef(\VerySimpleView,
{
	GGLRenderer.gr
	(
		In.gr(0), 

		clear_mode: 0, 
		clear_color: [0, 0, 0, 0.05], 
		texturing: 1, 
		perspective: 1
);
}).send(g)
)

(
v = Synth.new(\VerySimpleView, target: g, addAction: \addToTail);
)
v.free

(
f = TabFileReader.readInterpret 
(
	"/home/tapas/source/my_projects/scgraph/thesis/fulltext_latex/iris_data2.txt"
);
)

f[0][0]

(
l = Task.new (
{
	f.do (
	{
		arg x;
		Synth.new 
		(
			\Grain, 
			args: 
			[
				\x, x[0] / 8, 
				\y, x[1] / 5, 
				\z, x[2] / 7,
				\r, x[5] / 30,
				\g, 0.1 + (x[6] / 15),
				\b, 0.1 + (x[8] / 15),
				\wx, x[3] / 3,
				\wy, x[3] / 3,
				\wz, x[3] / 3,
			], 
			target: v, addAction: \addBefore
		);
	});
})
)

l.start
l.stop