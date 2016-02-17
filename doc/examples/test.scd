g = GServer.default;
g.boot;

g.quit

g.freeAll
g.quitAll;
g.status;

a = g.addr

a.sendMsg("/status");
a.sendMsg("/foo");
a.sendMsg("/notify", 1);
a.sendMsg(9,"Ugha", 1, 0, 0);
a.sendMsg("/quit");
a.sendMsg("/g_freeAll");
a.sendMsg("/g_freeAll", 1);
n.sendMsg("/g_new", -1, 0, 0);
a.sendMsg("/s_new", "SimpleCube", -1, 0, 0);
a.sendMsg("/s_new", "SimpleSin", -1, 0, 0);
a.sendMsg("/c_set", 0, 0.1);
5.do {a.sendMsg("/s_new", "SimpleView", -1, 1, 0);}
a.sendMsg("/d_free", "Ugh");
a.sendMsg("/d_freeAll");

g.sendMsg("/verbose",0);
g.sendMsg("/verbose",1);
g.sendMsg("/verbose",2);
g.sendMsg("/verbose",3);
g.sendMsg("/verbose",4);
g.sendMsg("/verbose",5);

g.sendMsg("/graphicsRate",25);
g.sendMsg("/controlRate", 100);

g.sendMsgSync;
g.sendMsg(3);


(
SynthDef(\WobblingCube,	{
	Out.gr(0, 
		GTranslate.gr(GCube.gr(size: SinOsc.kr(freq: 0.5)), [-3, -2, 3]));
}).send(g);
)

// Tell the server to produce one instance of this Definition
s = Synth.new(\WobblingCube, target: g, addAction: \addToHead);

s.free


(
SynthDef(\SimpleView, {
	GGLRenderer.gr(In.gr(0));
}).send(g);
)

v = Synth.new(\SimpleView, target: g, addAction: \addToTail);

(
SynthDef(\WobblingCube, {
	var source;
	source = GCube.gr(size: 1 + SinOsc.kr(freq: 0.5));
	GGLRenderer.gr(source);
}).play(g);
)


// test graphics busses
x = { Out.gr(9999, GCube.gr) }.play(g)
