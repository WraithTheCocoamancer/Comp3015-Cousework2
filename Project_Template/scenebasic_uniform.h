#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"

#include <glad/glad.h>
#include "helper/glslprogram.h"

#include "helper/plane.h"
#include "helper/objmesh.h"
#include <glm/glm.hpp>

#include "helper/texture.h"
#include "helper/torus.h"
#include "helper/teapot.h"
#include "helper/skybox.h"
#include "helper/random.h"


class SceneBasic_Uniform : public Scene
{
private:
    GLSLProgram prog;
    GLSLProgram PointSpriteProg;
    GLSLProgram AnimProg;

    //particles
    Random randparticle;

    glm::vec3 emitterPos, emitterDir;

    GLuint posBuf[2], velBuf[2], age[2];
    GLuint particleArray[2];
    GLuint feedback[2];
    GLuint drawBuf;
   
    int nParticles;
    float particleLifetime;
    float particleangle;
    float Particletime, deltaT;


    Torus torus;
    Teapot teapot;
    Plane plane; //plane surface

    float angleVertex;
    float timevertex;

    SkyBox sky;
    GLuint sprites;

    int numSprites;
    float* locations;


    //brick texture file into channel 0
    GLuint Brick = Texture::loadTexture("media/texture/brick1.jpg");

    //GLuint Fire = Texture::loadTexture("../Project_Template/media/texture/fire.png");

    //moss texture file for channel 1
    GLuint Moss = Texture::loadTexture("media/texture/moss.png");

    GLuint Wood = Texture::loadTexture("media/texture/hardwood2_diffuse.jpg");

    GLuint Cement = Texture::loadTexture("media/texture/cement.jpg");

    GLuint Dough = Texture::loadTexture("media/texture/Dough.jpg");

    GLuint Sugar = Texture::loadTexture("media/texture/Sugar.png");

    GLuint PigPink = Texture::loadTexture("media/texture/PigPink.jpg");
    std::unique_ptr<ObjMesh> mesh; //pig mesh

    GLuint cubeTex =
        Texture::loadHdrCubeMap("media/texture/cube/pisa-hdr/pisa");

    float angle, tPrev, rotSpeed;

    void initBuffers();
    void setMatrices(GLSLProgram&);

    void compile();

public:
    SceneBasic_Uniform();

    void initScene();
    void update(float t);
    void render();
    void resize(int, int);
};

#endif // SCENEBASIC_UNIFORM_H
