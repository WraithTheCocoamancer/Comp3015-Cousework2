#version 460

    //select 4.3 in scenerunner if in lab make shaders 430 at home 460
//in variable that receives the diffuse calculation from the vertex shader
in vec3 LightIntensity;
//in vec4 Position;//for wibble wobble
in vec3 Position; 
in vec3 Normal;
in vec2 TexCoord;


//out variable, this typical for all fragment shaders

layout (location = 0) out vec4 FragColour;
//vec3 phongModel(  vec3 postion, vec3 n) 
layout(binding=0) uniform sampler2D TextureOne;
layout(binding=1) uniform sampler2D TextureTwo;
//layout(binding=3) uniform samplerCube SkyBoxTex;
//uniform struct LightInfo 
//{
//  vec4 Position; // Light position in eye coords.
//  vec3 L;       // Diffuse light 
//  vec3 La; //Ambient intensity
//  
//} Lights[3];
//
uniform struct SpotLightInfo {
vec3 Position; // Position in cam coords
vec3 L; // Diffuse/spec intensity
vec3 La; // Amb intensity
vec3 Direction; // Direction of the spotlight in cam coords.
float Exponent; // Angular attenuation exponent
float Cutoff; // Cutoff angle (between 0 and pi/2)
} Spot[3];


uniform struct MaterialInfo 
{
  vec3 Kd;      // Diffuse reflectivity
  vec3 Ka; // Ambient reflectivity
  vec3 Ks; // Specular reflectivity
  float Shininess; //Specular shininess
} Material;

uniform struct FogInfo
{
float MaxDist; //max distance
float MinDist; //min distance
vec3 Color; //colour of the fog
} Fog;
//BlinnPhongSpotlight ===============================


vec3 blinnPhongSpot(int spotlight, vec3 position, vec3 n ) {

 vec3 spec = vec3(0.0);

 vec3 diffuse = vec3(0.0);

vec4 TextureOneColor = texture(TextureOne, TexCoord);

vec4 TextureTwoColor = texture(TextureTwo, TexCoord);

vec3 col = mix(TextureOneColor.rgb, TextureTwoColor.rgb, TextureTwoColor.a);

vec3 ambient = Spot[spotlight].La * col;

vec3 s = normalize(Spot[spotlight].Position - Position).xyz;

float cosAng = dot(-s,normalize(Spot[spotlight].Direction)); //(Cosine of the angle)

float angle = acos(cosAng); //(actual angle)

float spotScale = 0.0;

        spotScale = pow(cosAng, Spot[spotlight].Exponent);

        float sDotN = max( dot(s,n), 0.0 );

        //diffuse
         diffuse =  Material.Kd *col *  sDotN; // in case i need it later


if(angle < Spot[spotlight].Cutoff)
    {

        //calculate specular here.   

        if( sDotN > 0.0 )
            {

                vec3 v = normalize(- Position.xyz);
                vec3 h = normalize( v + s ); 

                spec = Material.Ks * pow( max( dot(h,n), 0.0), Material.Shininess );
            }
    }
    

//return ambient*0.3  + spotScale * (diffuse + spec);
return ambient* 0.3 + spotScale + Spot[spotlight].L * (diffuse + spec);
//return  spotScale + Spot[spotlight].L * (diffuse );
}

//=============================================================================




//Point lighting

//Making three point lights in here the int light takes it from either light 0 1 or 2  and uses that info when making point lights
//vec3 blinnPhongModelPointLighting( int light , vec3 postion, vec3 n)
//{
////ambient
//vec4 TextureOneColor = texture(TextureOne, TexCoord);
//vec4 TextureTwoColor = texture(TextureTwo, TexCoord);
//vec3 col = mix(TextureOneColor.rgb, TextureTwoColor.rgb, TextureTwoColor.a);
//vec3 ambient = Lights[light].La * col;
//vec3 s = normalize(Lights[light].Position - vec4(Position, 1.0f)).xyz;
//float sDotN = max( dot(s,n), 0.0 );
//
////diffuse
//vec3 diffuse =  Material.Kd * sDotN * col; // in case i need it later
//
////calculate specular here.   
// vec3 spec = vec3(0.0);
// if( sDotN > 0.0 )
//{
// vec3 v = normalize(- Position.xyz);
// vec3 h = normalize( v + s ); 
//
//spec = Material.Ks * pow( max( dot(h,n), 0.0), Material.Shininess );
// }
//return ambient + Lights[light].L * (diffuse + spec);
//}
//


//===============================================================================

//vec3 blinnPhongTexture(vec3 position, vec3 n)
//{
//vec3 texColor = texture(Tex1, TexCoord).rgb;
////ambient
//vec3 ambient = Lights[1].La * texColor;
//vec3 s = normalize(Lights[1].Position - vec4(Position, 1.0f)).xyz;
//float sDotN = max( dot(s,n), 0.0 );
//
////diffuse
//vec3 diffuse =  Material.Kd * sDotN; 
//
////calculate specular here.   
// vec3 spec = vec3(0.0);
// if( sDotN > 0.0 )
//{
// vec3 v = normalize(- Position.xyz);
// vec3 h = normalize( v + s ); 
//
//spec = Material.Ks * pow( max( dot(h,n), 0.0), Material.Shininess );
// }
//return ambient + Lights[1].L * (diffuse + spec);
//}
//


//==================================================================================
//Fog Sim


void main()
{
float dist = abs( Position.y ); //distance calculations
//fogFactor calculation based on the formula presented earlier
float fogFactor = (Fog.MaxDist - dist) / (Fog.MaxDist - Fog.MinDist);
fogFactor = clamp( fogFactor, 0.3, 1.0 ); //we clamp values

 vec3 Colour = vec3(0.0);
 // Colour += texture(SkyBoxTex, normalize(Position)).rgb;
 for( int i = 0; i < 3; i++ ){
 //Colour += phongModel( i,Position, normalize(Normal));
 Colour += blinnPhongSpot(i,Position, normalize(Normal));
   //Colour += blinnPhongModelPointLighting( i,Position, normalize(Normal));
    }
    
    vec3 FinalColor = mix(Fog.Color, Colour, fogFactor);

//    Colour += blinnPhongTexture(Position, normalize(Normal));
    FragColour = vec4(FinalColor,1);
   
    
}
