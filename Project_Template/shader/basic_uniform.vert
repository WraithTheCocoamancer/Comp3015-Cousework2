#version 460


//in variables, this are in model coordinates
layout (location = 0) in vec3 VertexPosition; 
layout (location = 1) in vec3 VertexNormal; 
layout (location = 2) in vec2 VertexTexCoord;
layout (location = 3) in vec4 VertexTangent;

//out vector needed for the fragment shader
 //out vec4 Position;
 out vec3 Position; //For non wibblewobble
 out vec3 Normal;
 out vec2 TexCoord;

 //wave paramiters for animation and wibble wobble animation time float
 uniform float Time;

 uniform float Freq = 2.5;
 uniform float velocity = 2.5;
 uniform float Amp = 0.6;
//uniforms for matrices required in the shader
uniform mat4 ModelViewMatrix;   //model view matrix
uniform mat3 NormalMatrix;		//normal matrix
uniform mat4 ProjectionMatrix;
uniform mat4 MVP;				//model view projection matrix
 
void main() 
{ 
  //transfrom normal from model coordinates to view coordinates
  vec4 pos = vec4(VertexPosition,1.0);

  float u = Freq * pos.x - velocity * Time;
  pos.y = Amp * sin(u);

  vec3 n = vec3(0.0);
  n.xy = normalize(vec2(cos(u), 1.0));


 Normal = normalize( NormalMatrix * VertexNormal);  // for non wibblewobble
 //Position = ModelViewMatrix * pos;
 //Normal = NormalMatrix * n;
  //transform vertex position from model coordinates to view coordinates
 Position = (ModelViewMatrix * vec4(VertexPosition,1.0)).xyz; // for non wibblewobble

 TexCoord = VertexTexCoord;

  //turns any vertex position into model view projection in preparations to 
  //graphics pipeline processes before fragment shader (clipping)
  gl_Position = MVP * vec4(VertexPosition,1.0); 
  //gl_Position = MVP * pos;
} 