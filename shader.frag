
uniform sampler2D textures;
varying vec2 TexCoords;


void main(void)
{
    vec4 texColor=texture2D(textures, TexCoords);
    if(texColor.a < 0.1)
        discard;
    gl_FragColor = texColor;
}
