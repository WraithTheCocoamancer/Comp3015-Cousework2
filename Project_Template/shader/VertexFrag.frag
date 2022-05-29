#version 460

layout (location = 0) out vec4 FragColor;

in float Transp;
in vec2 TexCoord;

uniform sampler2D ParticleTex;

void main()
{
    FragColor = texture(ParticleTex, TexCoord);
    // mix with black as it gets older, to simulate a bit of smoke
    FragColor = vec4(mix(vec3(0,0,0), FragColor.xyz, Transp), FragColor.a);
    FragColor.a *= Transp;
}