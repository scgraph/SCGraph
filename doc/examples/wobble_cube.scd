g = GServer.default;
g.boot;

(
SynthDef (\TestBlend,
{
	var cube1, cube2, cube3, eye, stretch1, stretch2, stretch3;

	stretch1 = 1 + Lag.kr(2*Decay.kr(Dust.kr(1)), 0.5);
	stretch2 = 3 + Lag.kr(2*Decay.kr(Dust.kr(2)), 0.5);
	stretch3 = 5 + Lag.kr(2*Decay.kr(Dust.kr(3)), 0.5);

	cube1 = GStretch.gr
	(
		GMaterial.gr
		(
			GCube.gr,
			shinyness: 0.5,
			diffuse: [0.8, 0.3, 0.1, 0.031],
			ambient: [0.1, 0.8, 0.2, 0.005]
		),
		[stretch1, stretch1, stretch1]
	);

	cube2 = GStretch.gr 
	(
		GMaterial.gr
		(
			GCube.gr,
			shinyness: 0.5,
			diffuse: [0.1, 0.8, 0.5, 0.06],
			ambient: [0.3, 0.4, 0.1, 0.018]
		),
		[stretch2, stretch2, stretch2]
	);

	cube3 = GStretch.gr 
	(
		GMaterial.gr
		(
			GCube.gr,
			shinyness: 0.5,
			diffuse: [0.2, 0.3, 0.9, 0.12],
			ambient: [0.7, 0.2, 0.6, 0.11]
		),
		[stretch3, stretch3, stretch3]
	);

	eye = 10 * [MouseX.kr(-10,10)+SinOsc.kr(freq: 0.111), MouseY.kr(-10,10)+SinOsc.kr(freq: 0.132), SinOsc.kr(freq: 0.134)];

	GGLRenderer.gr 
	(
		GTranslate.gr
		(
			GLight.gr 
			(
				index: 0, on: 1, 
				diffuse_color: [0.3, 0.5, 0.4, 0.5], 
				specular_color: [1,1,1,0.5],
				ambient_color: [1, 1, 1, 0.1],
				position:[30, 15, 10, 1]
			) 
			+ GLight.gr 
			(
				index: 1, on: 1, 
				diffuse_color: [0.3, 0.1, 0.7, 0.5], 
				specular_color: [1,1,1,0.5],
					position:[-30, -15, -10, 1]
			) 	
			+ GBlending.gr(on: 1)
			+ cube1 
			+ cube2 
			+ cube3
			+ GBlending.gr(on:0), 
			[
				Lag.kr(WhiteNoise.kr,1),
				Lag.kr(WhiteNoise.kr,1),
				Lag.kr(WhiteNoise.kr,1)
			]
		),
		eye: eye, 
		lighting: 1,
		clear_mode: 1,
		transparency: 1,
		clear_color: [0, 0, 0, 0.04]
	);
}).play(g)
)
