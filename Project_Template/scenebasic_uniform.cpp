#include "scenebasic_uniform.h"
#include <sstream>
#include <iostream>

using std::cerr;
using std::endl;

#include <glm/gtc/matrix_transform.hpp>
#include <time.h>
#include "helper/particleutils.h"
using glm::vec3;
using glm::mat4;

//constructor for Torus Teapot and Plane also loads pig mesh
SceneBasic_Uniform::SceneBasic_Uniform() :
    //paricles
    particleangle(0.0f), drawBuf(1), Particletime(0),deltaT(0), nParticles(4000),particleLifetime(6.0f), emitterPos(1,0,0),emitterDir(-1,2,0),
    //time
    timevertex(0),
    //plane constructor    
    plane(13.0f, 10.0f, 200, 2),
    /// teapot
    teapot(14, glm::mat4(1.0f)),
    //torus
    torus(1.75f * 0.75f, 0.75f * 0.75f, 50, 50)//, 
    //skybox
    //angle(0.0f), tPrev(0.0f), rotSpeed(glm::pi<float>() / 8.0f), sky(100.0f)
{
    mesh = ObjMesh::load("media/pig_triangulated.obj", true);
}



void SceneBasic_Uniform::initScene()
{
    compile();
    glClearColor(0.3f, 0.3f, 0.3f, 0.1f);


    //blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);

    model = mat4(1.0f);
    
    glActiveTexture(GL_TEXTURE5);

    GLuint Fire = Texture::loadTexture("media/texture/fire.png");

    glActiveTexture(GL_TEXTURE1);
    ParticleUtils::createRandomTex1D(nParticles * 3);

    initBuffers();

    AnimProg.use();
    AnimProg.setUniform("RandomTex", 1);
    AnimProg.setUniform("ParticleTex", 5);
    AnimProg.setUniform("ParticleLifetime", particleLifetime);
    AnimProg.setUniform("Accel", vec3(0.0f, -0.5f, 0.0f));
    AnimProg.setUniform("ParticleSize", 0.05f);
    AnimProg.setUniform("Emitter", emitterPos);
    AnimProg.setUniform("EmitterBasis", ParticleUtils::makeArbitraryBasis(emitterDir));


    float tPrev = 0;

    float angleVertex = glm::half_pi<float>();
    //point sprite setup
    numSprites = 50;
    locations = new float[numSprites * 3];
    srand((unsigned int)time(0));

    for (int i = 0; i < numSprites; i++) {
        vec3 p(((float)rand() / RAND_MAX * 2.0f) - 1.0f,
            ((float)rand() / RAND_MAX * 5.0f) - 1.0f, 
            ((float)rand() / RAND_MAX * 2.0f) - 1.0f);
        locations[i * 3] = p.x;
        locations[i * 3 + 1] = p.y;
        locations[i * 3 + 2] = p.z;
    }
    GLuint handle;
    glGenBuffers(1, &handle);
    
    glBindBuffer(GL_ARRAY_BUFFER, handle);
    glBufferData(GL_ARRAY_BUFFER, numSprites * 3 * sizeof(float), locations, GL_STATIC_DRAW);

    delete[] locations;

    glGenVertexArrays(1, &sprites);
    glBindVertexArray(sprites);

    glBindBuffer(GL_ARRAY_BUFFER, handle);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte*)NULL + (0)));
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    //loading textures

    const char* texName = "media/texture/HiMarius.png";
    Texture::loadTexture(texName);
    PointSpriteProg.use();
    PointSpriteProg.setUniform("SpriteTex", 0);
    PointSpriteProg.setUniform("Size2", 0.150f);
    PointSpriteProg.use();
    //Fog setup

    prog.use();
    prog.setUniform("Fog.MaxDist", 5.0f);
    prog.setUniform("Fog.MinDist", 0.0f);
    prog.setUniform("Fog.Color", vec3(0.2f, 0.2f, 0.2f));
    prog.use();
    //initialise the model matrix
    model = mat4(1.0f);

    //initialise the view using glm::Lookat to set where we are looking at
    view = glm::lookAt(vec3(2.0f, 4.5f, 7.0f), vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

    projection = mat4(1.0f);

    angle = glm::radians(90.0f);


    //make Light Initilization

    float x, z;
    //Loop for multiple lights slightly changes position
    for (int i = 0; i < 3; i++)
    {
        std::stringstream name;
        name << "Lights[" << i << "].Position";
        x = 2.0f * cosf((glm::two_pi<float>() / 3) * i);
        z = 2.0f * sinf((glm::two_pi<float>() / 3) * i);
        prog.setUniform(name.str().c_str(), view * glm::vec4(x, 1.2f, z + 1.0f, 1.0f));
    }


    //setting .L is Diffuse and Specular combined
  /*  prog.setUniform("Lights[0].L", vec3(0.0f, 2.0f, 0.0f));
    prog.setUniform("Lights[1].L", vec3(0.0f, 0.0f, 2.0f));
    prog.setUniform("Lights[2].L", vec3(2.0f, 0.0f, 0.0f));*/

    //Lights Ambient set
  /*  prog.setUniform("Lights[0].La", 0.0f, 0.5f, 0.0f);
    prog.setUniform("Lights[1].La", 0.0f, 0.0f, 0.01f);
    prog.setUniform("Lights[2].La", 0.01f, 0.0f, 0.0f);*/


    //make sure you use the correct name, check your vertex shader

    //Spotlight Uniforms. Ambient and specular are combine into L LA is abient. Exponent and cutoff for thecoplight cone
    prog.setUniform("Spot[0].L", vec3(0.9f, 0.9f, 0.0f));
    prog.setUniform("Spot[0].La", vec3(0.1f, 0.1f, 0.1f));
    prog.setUniform("Spot[0].Exponent", 100.0f);
    prog.setUniform("Spot[0].Cutoff", glm::radians(15.0f));

    prog.setUniform("Spot[1].L", vec3(0.9f, 0.9f, 0.0f));
    prog.setUniform("Spot[1].La", vec3(0.1f));
    prog.setUniform("Spot[1].Exponent", 100.0f);
    prog.setUniform("Spot[1].Cutoff", glm::radians(15.0f));

    prog.setUniform("Spot[2].L", vec3(0.9f, 0.9f, 0.90f));
    prog.setUniform("Spot[2].La", vec3(0.1f, 0.1f, 0.1f));
    prog.setUniform("Spot[2].Exponent", 100.0f);
    prog.setUniform("Spot[2].Cutoff", glm::radians(15.0f));

 
    //Example of setting texture's being set more set later
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Brick);


    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, Moss);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);

}

void SceneBasic_Uniform::initBuffers() 
{
    glGenBuffers(2, posBuf);
    glGenBuffers(2, velBuf);
    glGenBuffers(2, age);

    //make space for buffers
    int size = nParticles * 3 * sizeof(GLfloat);
    glBindBuffer(GL_ARRAY_BUFFER, posBuf[0]);
    glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, posBuf[1]);
    glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, velBuf[0]);
    glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, velBuf[1]);
    glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, age[0]);
    glBufferData(GL_ARRAY_BUFFER, nParticles * sizeof(float), 0, GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, age[1]);
    glBufferData(GL_ARRAY_BUFFER, nParticles * sizeof(float), 0, GL_DYNAMIC_COPY);

    std::vector<GLfloat> tempData(nParticles);
    float rate = particleLifetime / nParticles;
    for (int i = 0; i < nParticles; i++)
    {
        tempData[i] = rate * (i - nParticles);
    }
    glBindBuffer(GL_ARRAY_BUFFER, age[0]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, nParticles * sizeof(float), tempData.data());

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //vertex arrays
    glGenVertexArrays(2, particleArray);

    //particle array 0
    glBindVertexArray(particleArray[0]);
    glBindBuffer(GL_ARRAY_BUFFER, posBuf[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

   
    glBindBuffer(GL_ARRAY_BUFFER, velBuf[0]);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, age[0]);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    //particle array 1
    glBindVertexArray(particleArray[1]);
    glBindBuffer(GL_ARRAY_BUFFER, posBuf[1]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);


    glBindBuffer(GL_ARRAY_BUFFER, velBuf[1]);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, age[1]);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    //feedback
    glGenTransformFeedbacks(2, feedback);

    //feedback zero
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[0]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, posBuf[0]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, velBuf[0]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, age[0]);

    //feedback one
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[1]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, posBuf[1]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, velBuf[1]);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, age[1]);

    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
}

void SceneBasic_Uniform::compile()
{

    try {
        prog.compileShader("shader/basic_uniform.vert");
        prog.compileShader("shader/basic_uniform.frag");
        prog.link();
        prog.use();
        PointSpriteProg.compileShader("shader/PointSpriteVert.vert");
        PointSpriteProg.compileShader("shader/PointSpriteFrag.frag");
        PointSpriteProg.compileShader("shader/Geometry_Shader.geom");
        PointSpriteProg.link();
        PointSpriteProg.use();
        AnimProg.compileShader("shader/VertexAnimation.vert");
        AnimProg.compileShader("shader/VertexFrag.frag");

        GLuint progHandle = prog.getHandle();
        const char* outputNames[] = { "Position", "Velocity", "Age" };
        glTransformFeedbackVaryings(progHandle, 3, outputNames, GL_SEPARATE_ATTRIBS);

        AnimProg.link();
        AnimProg.use();
    }

    catch (GLSLProgramException& e) {
        cerr << e.what() << endl;
        exit(EXIT_FAILURE);
    }
}

void SceneBasic_Uniform::update(float t)
{
    //Delta time to track time and using delta time to change Angle
    timevertex = t;
    
    deltaT = t - Particletime;
    Particletime = t;

    particleangle = std::fmod(particleangle + 0.01f, glm::two_pi<float>());
}

void SceneBasic_Uniform::render()
{
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   
  
    prog.use();
   
    //static spotlight
   /* glm:: vec4 lightPosSpot = glm::vec4(0.0f, 10.0f, 0.0f, 1.0f);
     prog.setUniform("Spot.Position", vec3(view * lightPosSpot));*/

     //glm::vec4 lightPosSpot1 = glm::vec4(20.0f * cos(angle), 10.0f, 1.0f * sin(angle), 1.0f);
     //prog.setUniform("Spot[0].Position", vec3(view * lightPosSpot1));

     //Angle changes slowly thanks to Delta time making spotlight slowly turn
    glm::vec4 lightPosSpot1 = glm::vec4(-3.0f, 9.0f, -40.0f, 1.0f);
    prog.setUniform("Spot[0].Position", vec3(view * lightPosSpot1));
    glm::mat3 normalMatrix = glm::mat3(vec3(view[0]), vec3(view[1]), vec3(view[2]));
    prog.setUniform("Spot[0].Direction", vec3(0.0f, -1.0f, 0.0f));
    glm::vec4 lightPosSpot2 = glm::vec4(-3.0f, 9.0f, -25.0f, 1.0f);
    prog.setUniform("Spot[1].Position", vec3(view * lightPosSpot2));
    prog.setUniform("Spot[1].Direction", vec3(0.0f, -1.0f, 0.0f));
    glm::vec4 lightPosSpot3 = glm::vec4(20.0f * cos(particleangle), 5.0f, 1.0f * sin(particleangle), 1.0f);
    prog.setUniform("Spot[2].Position", vec3(view * lightPosSpot3));
    prog.setUniform("Spot[2].Direction", normalMatrix * vec3(-lightPosSpot3));


   
    //setMatrices(AnimProg);
    ////sky
    //vec3 cameraPos = vec3(0.0f , 0.0f, 3.0f );
    //view = glm::lookAt(cameraPos, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f,
    //    0.0f));
        //// Draw sky
   // prog.use();

    glClearColor(0.2f, 0.2f, 0.2f, 0.2f);

    //Teapot start
    //material diffuse
    prog.setUniform("Material.Kd", 0.2f, 0.55f, 0.9f);
    //material specular
    prog.setUniform("Material.Ks", 0.95f, 0.95f, 0.95f);
    //material ambient
    prog.setUniform("Material.Ka", 0.2f * 0.3f, 0.55f * 0.3f, 0.9f * 0.3f);
    prog.setUniform("Material.Shininess", 100.0f);

    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, 0.0f, -15.0f));
    model = glm::rotate(model, glm::radians(45.0f), vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
    setMatrices(prog);
    teapot.render();

    //Teapot end


    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, Dough);

    glActiveTexture(GL_TEXTURE1);

    glBindTexture(GL_TEXTURE_2D, Sugar);
    //torus start
    prog.setUniform("Material.Kd", 0.9f, 0.0f, 0.0f);
    prog.setUniform("Material.Ks", 0.95f, 0.95f, 0.95f);
    prog.setUniform("Material.Ka", 0.9f * 0.3f, 0.8f * 0.3f, 0.8f * 0.3f);
    prog.setUniform("Material.Shininess", 100.0f);
    model = mat4(1.0f);
    model = glm::translate(model, vec3(-1.0f, 0.75f, 3.0f));
    model = glm::rotate(model, glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
    setMatrices(prog);
    torus.render();
    //torus end

    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, PigPink);
    glActiveTexture(GL_TEXTURE1);

    glBindTexture(GL_TEXTURE_2D, PigPink);
    //+===================
    //setting material uniforms plane and pig
    prog.setUniform("Material.Kd", 0.8f, 0.8f, 0.0f);
    prog.setUniform("Material.Ks", 0.9f, 0.9f, 0.0f);
    prog.setUniform("Material.Ka", 0.8f, 0.8f, 0.0f);
    prog.setUniform("Material.Shininess", 60.0f);
    model = mat4(1.0f);
    model = glm::rotate(model, glm::radians(-20.0f), vec3(0.0f, 1.0f, 0.0f)); //Y rotate pig
    //model = glm::rotate(model, glm::radians(10.0f), vec3(1.0f, 0.0f, 0.0f)); // x rotate pig
    model = glm::translate(model, vec3(-7.0f, 1.0f, -8.0f));
    model = glm::scale(model, vec3(2.5f, 2.5f, 2.5f));
    setMatrices(prog);
    mesh->render();

    
    //spotlight position
    

    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, Cement);
    glActiveTexture(GL_TEXTURE1);

    glBindTexture(GL_TEXTURE_2D, Cement);
    
    prog.setUniform("Material.Kd", 0.99f, 0.99f, 0.99f);
    prog.setUniform("Material.Ks", 0.95f, 0.95f, 0.0f);
    prog.setUniform("Material.Ka", 0.9f * 0.3f, 0.9f * 0.3f, 0.8f * 0.3f);
    prog.setUniform("Material.Shininess", 20.0f);
    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, -0.10f, 0.0f));
    model = glm::scale(model, vec3(2.5f, 0.0f, 10.0f));
    //model = glm::rotate(model, glm::radians(10.0f), vec3(1.0f, 0.0f, 0.0f)); //y rotate pig
    setMatrices(prog);
    plane.render();
    //we set matrices 
    prog.use();
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTex);

   
    model = mat4(1.0f);
    setMatrices(prog);
    //sky.render();


    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, Brick);
    glActiveTexture(GL_TEXTURE1);

    glBindTexture(GL_TEXTURE_2D, Moss);

    PointSpriteProg.use();
    model = mat4(1.0f);

    setMatrices(PointSpriteProg);
    PointSpriteProg.setUniform("SpriteTex", 0);
    glBindVertexArray(sprites);
    glDrawArrays(GL_POINTS, 0, numSprites);
    glFinish();
    PointSpriteProg.use();

    AnimProg.use();
    AnimProg.setUniform("Time", Particletime);
    AnimProg.setUniform("DeltaT", deltaT);


    //update pass
    AnimProg.setUniform("Pass", 1);

    glEnable(GL_RASTERIZER_DISCARD);
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[drawBuf]);
    glBeginTransformFeedback(GL_POINTS);

    glBindVertexArray(particleArray[1 - drawBuf]);
    glVertexAttribDivisor(0, 0);
    glVertexAttribDivisor(1, 0);
    glVertexAttribDivisor(2, 0);
    glDrawArrays(GL_POINTS, 0, nParticles);
    glBindVertexArray(0);

    glEndTransformFeedback();
    glDisable(GL_RASTERIZER_DISCARD);

    // render pass
    AnimProg.setUniform("Pass", 2);
    view = glm::lookAt(vec3(4.0f * cos(angle), 1.5f, 4.0f * sin(particleangle)), vec3(0.0f, 1.5f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    setMatrices(AnimProg);

    glDepthMask(GL_FALSE);
    glBindVertexArray(particleArray[drawBuf]);
    glVertexAttribDivisor(0, 1);
    glVertexAttribDivisor(1, 1);
    glVertexAttribDivisor(2, 1);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, nParticles);
    glBindVertexArray(0);
    glDepthMask(GL_TRUE);

    drawBuf = 1 - drawBuf;

}

void SceneBasic_Uniform::setMatrices(GLSLProgram& p)
{
    mat4 mv = view * model; //we create a model view matrix

    p.setUniform("ModelViewMatrix", mv); //set the uniform for the model view matrix

    p.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2]))); //we set the uniform for normal matrix

    p.setUniform("MVP", projection * mv); //we set the model view matrix by multiplying the mv with the projection matrix

    p.setUniform("ProjectionMatrix", projection);

    p.setUniform("MV", mv);
    p.setUniform("Proj", projection);
    
}

void SceneBasic_Uniform::resize(int w, int h)
{
    glViewport(0, 0, w, h);
    width = w;
    height = h;
    projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 100.0f);
}