#version 300 es

out highp vec4 color;

in highp vec4 vColor;
in highp vec2 vTexCoords;
in bool vIsTexSet;

void main()
{    
    color = texture(texture1, TexCoords);
    //color = vColor;
}
