/*
MIT License
Copyright (c) 2022 PCSX-Redux authors
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "common/syscalls/syscalls.h"
#include "psyqo/application.hh"
#include "psyqo/font.hh"
#include "psyqo/gpu.hh"
#include "psyqo/scene.hh"

#include "fix16.hpp"
#include "inline_n.h"
#include "sincosmath.h"
#include "matrixstuff.h"
#include "fileio.h"


/* Cube vertices */
gtetri vertices[] = {
    //tri 1
    (gtetri){(gtevtx){-100, -100, -100},
     (gtevtx){100, -100, -100},
     (gtevtx){100,  100, -100}},
    //tri 2
    (gtetri){(gtevtx){ 100,  100, -100},
     (gtevtx){-100,  100, -100},
     (gtevtx){-100, -100, -100}},
    //tri 3
    (gtetri){
     (gtevtx){100, -100,  100},
     (gtevtx){100,  100,  100}},
    //tri 4
    (gtetri){(gtevtx){ 100,  100,  100},
     (gtevtx){ -100,  100,  100},
     (gtevtx){ -100, -100,  100}},
    //tri 5
    (gtetri){(gtevtx){-100,  100,  100},
     (gtevtx){-100,  100, -100},
     (gtevtx){-100, -100, -100}},
    //tri 6
    (gtetri){(gtevtx){-100, -100, -100},
     (gtevtx){-100, -100,  100},
     (gtevtx){-100,  100,  100}},
    //tri 7
    (gtetri){(gtevtx){100,  100,  100},
     (gtevtx){100,  100, -100},
     (gtevtx){100, -100, -100}},
    //tri 8
    (gtetri){(gtevtx){100, -100, -100},
     (gtevtx){100, -100,  100},
     (gtevtx){100,  100,  100}},
    //tri 9
    (gtetri){(gtevtx){-100, -100, -100},
     (gtevtx){ 100, -100, -100},
     (gtevtx){ 100, -100,  100}},
    //tri 100
    (gtetri){(gtevtx){ 100, -100,  100},
     (gtevtx){-100, -100,  100},
     (gtevtx){-100, -100, -100}},
    //tri 12
    (gtetri){(gtevtx){-100,  100, -100},
     (gtevtx){100,  100, -100},
     (gtevtx){100,  100,  100}},
    //tri 13
    (gtetri){(gtevtx){100,  100,  100},
     (gtevtx){-100,  100,  100},
     (gtevtx){-100,  100, -100}}
};

/* Cube face normals */
SVECTOR cube_norms[] = {
	{ 0, 0, -ONE, 0 },
	{ 0, 0, ONE, 0 },
	{ 0, -ONE, 0, 0 },
	{ 0, ONE, 0, 0 },
	{ -ONE, 0, 0, 0 },
	{ ONE, 0, 0, 0 }
};

class mesh {
    public :
    jkltri* trilist;
    jkltri* uvlist;
    psyqo::Color rgbium;
};

namespace {

    int32_t framsince = 0;
    int32_t framesinsec = 0;
    int32_t previousseccount =0;
    Fix16 orb(38.5);

// A PSYQo software needs to declare one `Application` object.
// This is the one we're going to do for our hello world.
class Hello final : public psyqo::Application {

    void prepare() override;
    void createScene() override;

  public:
    psyqo::Font<> m_font;
};

// And we need at least one scene to be created.
// This is the one we're going to do for our hello world.
class HelloScene final : public psyqo::Scene {
    void frame() override;

    // We'll have some simple animation going on, so we
    // need to keep track of our state here.
    uint8_t m_anim = 0;
    bool m_direction = true;
};

// We're instantiating the two objects above right now.
Hello hello;
HelloScene helloScene;

}  // namespace

void Hello::prepare() {
    psyqo::GPU::Configuration config;
    config.set(psyqo::GPU::Resolution::W320)
        .set(psyqo::GPU::VideoMode::NTSC)
        .set(psyqo::GPU::ColorMode::C15BITS)
        .set(psyqo::GPU::Interlace::PROGRESSIVE);
    gpu().initialize(config);

}

void Hello::createScene() {
    pushScene(&helloScene);
}

MATRIX tmtx;
jklvtx pos = {120, 180, 1000};
gtevtx rot = {0, 0, 0, 0};
 int initialized = 0;
void HelloScene::frame() {
    if(initialized == 0) {
            
        initialized = 1;
    }
    if (m_anim == 0) {
        m_direction = true;
    } else if (m_anim == 255) {
        m_direction = false;
    }
    psyqo::Color bg{{.r = 0, .g = 64, .b = 91}};
    bg.r = m_anim;

    hello.gpu().clear(bg);

    if (m_direction) {
        m_anim++;
    } else {
        m_anim--;
    }
        
    
    
    psyqo::Color c = {{.r = 255, .g = 255, .b = uint8_t(255 - m_anim)}};

    psyqo::Prim::Triangle tritankista;
    tritankista.setColor(psyqo::Color{222,22,22});

	rot.vx += 16;
	rot.vz += 16;

    RotMatrix((SVECTOR *)&rot, &tmtx);
    TransMatrix(&tmtx,(VECTOR *) &pos);

	gte_SetRotMatrix( &tmtx );
	gte_SetTransMatrix( &tmtx );

    for(int i = 0; i < 12; i++) {

    gte_ldv3( 
				&vertices[(i)].pta, 
				&vertices[(i)].ptb, 
				&vertices[(i)].ptc );

    gte_rtpt();

    gte_nclip();
    int p;
    gte_stopz(&p);

   // if(p < 0) continue;

    gte_avsz4();
	gte_stotz( &p );

 //   if( (p>>2) > 256 ) continue;

    gte_stsxy0( &tritankista.pointA );
	gte_stsxy1( &tritankista.pointB );
	gte_stsxy2( &tritankista.pointC );



    psyqo::Vertex test;

    test.packed = orb.value;
    gpu().sendPrimitive(tritankista);
    }
}

int main() { 
    InitGeom();
    return hello.run(); 
    }
