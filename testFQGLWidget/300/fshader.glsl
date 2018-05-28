#version 300 es

// This works on desktop with Qt, because highp and the like get inserted.
// But, for now, we're keeping them separate.

out highp vec4 color;

in highp vec4 vColor;
in highp vec2 vTexCoords;
in bool vIsTexSet;

void main()
{    
    //color = texture(texture1, TexCoords);
    color = vColor;
}
