g = GServer.default;
g.boot;

(
SynthDef(\EEGHead,
{
	arg chan1, chan2, chan3, chan4;

	var head, channels;

	head = GColor.gr(GTexQuad.gr(tex_index: 1), [1,1,1,0.4]);
	Out.gr(0, head);
}).send(g)
)

e = Synth.new(\EEGHead, args: [], target: g, addAction: \addToHead);
e.free

(
SynthDef(\Grain,
{
	arg x,y,z,amp;

	var grain, env;

    env = Line.kr(1, 0, 2.5, doneAction: 2);

	grain = GColor.gr
	(
		GStretch.gr 
		(
			GTranslate.gr 
			(
				GTexQuad.gr(tex_index: 2), 
				[x,y,z]
			), 
			[0.5*(1-env), 0.5*(1-env), 1]
		), 
		[amp, 0, 1-amp, env]
	);
	Out.gr(0, grain);
}).send(g)
)

(
r = Synth.new 
(
	\Grain, 
	args: 
	[
		\amp, 0.4,
		\x,-0.225,
		\y, 0.235
	],
	target:e, 
	addAction: \addAfter
)
)

(
SynthDef(\VerySimpleView,
{
	GGLRenderer.gr
	(
		GBlending.gr
		(
			on:1
		) 
		+ 
		In.gr(0), 
		transparency: 1, 
		clear_mode: 0, 
		clear_color: [1, 1, 1, 0.05], 
		texturing: 1, 
		perspective: 0
);
}).send(g)
)

(
v = Synth.new(
	\VerySimpleView, 
	target: g, 
	addAction: \addToTail);
)
v.free

(
f = TabFileReader.readInterpret(
	"/home/tapas/source/my_stuff/scgraph/doc/fulltext_latex/eeg_data_A");
)

f[0][0]

(
l = Task.new(
{
	f.do(
	{
		arg x;
		Synth.new 
		(
			\Grain, 
			args: 
			[
				\amp, 2 * (0.35 + x[0]), 
				\x,0.125, 
				\y, -0.31, 
				\z, 0.1
			], 
			target: v, addAction: \addBefore
		);

		Synth.new 
		(
			\Grain, 
			args: 
			[
				\amp, 2 * (0.35 + x[1]), 
				\x,-0.225, 
				\y, -0.31, 
				\z, 0.1
			], 
			target: v, addAction: \addBefore
		);

		Synth.new 
		(
			\Grain, 
			args: 
			[
				\amp, 2 * (0.35 + x[2]), 
				\x,0.135, 
				\y, 0.245, 
				\z, 0.1
			], 
			target: v, addAction: \addBefore
		);

		Synth.new 
		(
			\Grain, 
			args: 
			[
				\amp, 2 * (0.35 + x[3]), 
				\x,-0.225, 
				\y, 0.235, 
				\z, 0.1
			], 
			target: v, addAction: \addBefore
		);

		0.2.wait;
	});
})
)

l.start
l.stop