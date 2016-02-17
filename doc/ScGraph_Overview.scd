// ============= General ===========

// ScGraph

// ============= Startup ===========

// As ScGraph implements parts of the SuperCollider OSC interface
// we can just use a "remote" Server to control it. the executable
// scgraph takes different commandline options than scsynth and
// thus we cannot use the Server class to spawn it.  Simply
// run "scgraph" in some other window before evaluating the following
// lines:

n = NetAddr("localhost", 37291)
Server.default = h = Server.new("scgraph", n)


/// ============ UGens ============

// renderer (many argumenst, see source):
GGLRenderer.gr( in, ... );

// Example of a renderer that reads from Bus 0. You can "kill" the
// synth by closing the window. This renderer has central perspective
// turned off (useful for 2D stuff):
{GGLRenderer.gr(in: In.gr(0), perspective: 0)}.play

// A renderer that reads from graphics bus 0 and features support for
// texturing, alpha blending and lighting
(
{GGLRenderer.gr(
	in: In.gr(0), 
	texturing: 1,
	transparency: 1,
	lighting: 1
)}.play
)

// A simple example of a Synth that writes to a bus. Evaluate this after the above, 
// so the geometry producer is added to the head of the group and it gets to 
// create its output before the renderer tries to read it :)

(
SynthDef(\foo, 	{
	Out.gr(
		0, 
		GColor.gr(
			GQuad.gr,
			[0.5 + (0.5 * SinOsc.kr(1.1, phase: 0.3)), 0.5 + (0.5 * SinOsc.kr(1.0)), 0, 0]
		)
	)
}).play
)


/// =========== Alpha Blending / Transparency ============

// To use alpha blending you need to do three things
// - Color your primitives with an alpha component between
//   0 and 1 (either by using a material when using lighting
//   or by using GColor)
// - Enable transparency in the GGLRenderer that renders the 
//   primitive
// - Put a GBlending object onto the bus before the primitive(s)
//   for which alpha blending shall be performed.

// A "meta" graphics object to control the state of the blending
// functionality in OpenGL (src and dst are documented in source).
//
// The blending functions are syn=mbols that closely relate to 
// OpenGL blending functions. So take a look at the Red Book or
// other OpenGL documentation..
GBlending.gr( on, srcFun, dstFun );

// Here is an example that might shed some light on the issue. Keep 
// mouse pointer in the center of the screen to zoom in :)
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
		eye: eye, 
		lighting: 1,
		transparency: 1,

		in:
		GTranslate.gr
		(
			displacement_vector:
			[
				Lag.kr(WhiteNoise.kr,1),
				Lag.kr(WhiteNoise.kr,1),
				Lag.kr(WhiteNoise.kr,1)
			],

			in:
			GLighting.gr(1) 
			+
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
		)
	);
}).play
)

// ======== Spatial Transformations ===========

// A translation in 3D space by the respective vector components
GTranslate.gr( in, displacement_vector );

// A rotation around an axis (determined by a 3 component vector)
// by an angle. The angle is in degrees.
GRotate.gr( in, axis, angle );

// A simple scaling command that stretches the input geometry objects
// in the three spatial directions respectively
GStretch.gr( in, stretch_vector );

// A general linear transform. The components are specified in Row-Major
// order [not 100% sure. Must recheck code ;)]
GTransform.gr( in, components )

// ========= properties ==========

// Color: (color is a four element vector)
GColor.gr( in, color );

// Culling: (mode: 0 = off, 1 = front, 2 = back, 3 = front_and_back)
GCulling.gr( mode );

// Material (all quadruples, except in and shinyness)
GMaterial.gr( in, shinyness, ambient, diffuse, specular, emissive )

// ============= Basic Shapes ==============

// Most of these basic shapes are drawn in pure white when rendered directly.
// To give color to the shape, wrap it in a GColor UGen or a GMaterial (when
// using lighting)

// rectangle
GRectangle.gr( width, height )

// cube
GCube.gr( size )

// A line from point vector1 to point vector2. The strength indicates
// the line thickness (GL_LINE)
GLine.gr( strength, vector1, vector2 )


// points (vectors is an array of multiple vectors of size 3). The size 
// indicates the size of the point (GL_POINTS) 
GPoints.gr( size, vectors );


// A polygon (vectors is an array of multiple vectors of size 3) with a single
// normal for the face (see Lighting below)
GPolygon.gr( vectors, normal )

// A quadrilateral with a single normal for the face (see Lighting below)
GQuad.gr( vector1, vector2, vector3, vector4, normal )

// A textured quad. See Texturing below. A single normal for the face (see Lighting below)
GTexQuad.gr( vector1, vector2, vector3, vector4, tex_coord1, tex_coord2, tex_coord3, tex_coord4, tex_index, normal )
// triangle
GTriangle.gr( vector1, vector2, vector3, normal );



// ============ Lighting ==========

// light (many arguments... see source!)
GLight.gr( index, on, position, ... )

// turns lighting on...
GLighting.gr( on );

// ============= misc ============

// freq-amp : freq is relative to nyquist??
GFreqAmp.kr( port, freq )


/// =========== Extensions of the OSC protocol ===========

/// =========== Textures ================

// Upon startup ScGraph tries to traverse a directory recursively (by default 
// this is the directory ~/.scgraph/textures in the user's home directory). 
// All textures found in this directory are loaded into scgraph and are 
// available for use e.g. with the GTexQuad unit. The GTexQuad UGen has 4 
// additional parameters as opposed to the "normal" GQuad unit. These 
// additional parameters are a texture index and an array of texture 
// coordinates (2-dim arrays themself).

// To use texturing at all it needs to be enabled in the corresponding 
// GGLRenderer, too.



/// =========== Shaders ================

// ScGraoh allows to load GLSL programs onto the graphics card. To load a bunch 
// of shaders, pack them into a OSC Message of the format:

// ["/loadShaderProgram", index, uniforms, source1, isVertex1, source2, isVertex2, ... ]

// Here index is a number > 0 which is lateron used to refer to this program
// i.e. via the GShaderProgram unit.

// where source1 is a string representing the  the first shader source, 
// isVertex1 is indicating whether it's a vertex shader (1) or a fragment 
// shader (0). 

// The same applies to source2 and isVertex2.  All shaders in such a message 
// will be compiled and linked together into a single shader program which is 
// then pushed to the graphics card. Use the GShaderProgram UGen to activate a 
// shader program.

// uniforms is a string that holds uniform variable names.
// The uniforms have to point to uniform variable names in the global name-
// space of the shaders. Then lateron it can be controlled with a 
// GShaderUniform GUnit (see Example below)


// Setup a GGLRenderer that shows something (and use below sendMsg commands to 
// send a simple shader program
{GGLRenderer.gr(GShaderProgram.gr(0,1) + GShaderUniform.gr(0, [SinOsc.kr(1) * 0.5 + 0.5]) + GTranslate.gr(GCube.gr, [SinOsc.kr(1.21212121),SinOsc.kr(1,phase:1),0]))}.play

// You can thus switch to different shader programs between different graphics objects
// which should allow for some funky effects

// This example sends a single fragment shader that colors all fragments with a 
// single color. The r component is determined by the value of the uniform x
(
n.sendMsg
(
"/loadShaderProgram", 
0,
"x",
"
uniform float x;
        void main()
        {
                gl_FragColor = vec4(x,0.9,0.1,1.0);
        }
", 
0);
)


// This example sends two shaders. The first one is a fragment
// shader from the previous example and the second one is a vertex shader
// that simply scales all objects
(
n.sendMsg
(
"/loadShaderProgram", 
0,
"",
"
        void main()
        {
                gl_FragColor = vec4(0.1,0.9,0.1,1.0);
        }
", 
0,
"
void main(void)
{
   vec4 a = gl_Vertex;
   a.x = a.x * 0.5;
   a.y = a.y * 0.5;
   a.z = a.z * 0.3;
   gl_Position = gl_ModelViewProjectionMatrix * a;
}
",
1);
)

// Use this message to clear all shader programs
(
n.sendMsg(
	"/clearShaderPrograms"
);
)
