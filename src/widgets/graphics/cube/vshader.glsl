#version 410

uniform mat4 mvp_matrix;

in vec4 vertex;
in vec2 texcoord;

out vec2 v_texcoord;

//! [0]
void main()
{
    // Calculate vertex position in screen space
    gl_Position = mvp_matrix * vertex;

    // Pass texture coordinate to fragment shader
    // Value will be automatically interpolated to fragments inside polygon faces
    v_texcoord = texcoord;
}
//! [0]
