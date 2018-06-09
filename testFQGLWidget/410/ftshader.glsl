#version 410

out vec4 color;

in vec4 vColor;
in vec2 vTexCoords;

uniform sampler2D texture1;

void main()
{    
    //color = texture(texture1, vTexCoords) * vColor;
    color = texture(texture1, vTexCoords);
}
