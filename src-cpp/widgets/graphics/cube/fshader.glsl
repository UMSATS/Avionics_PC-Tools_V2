#version 410

uniform sampler2D a_texture;

in vec2 v_texcoord;

out vec4 color;

//! [0]
void main()
{
    // Set fragment color from texture
    color = texture(a_texture, v_texcoord);
}
//! [0]

