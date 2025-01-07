#version 430 core

void main()
{
  vec2 vertices[6] = 
  {
    // top left
    // bot left
    // top right
    // top right
    // bot left
    // bot right
    vec2(-0.5, 0.5),
    vec2(-0.5, -0.5),
    vec2(0.5, 0.5),
    vec2(0.5, 0.5),
    vec2(-0.5, -0.5),
    vec2(0.5, -0.5)
  };

  gl_Position = vec4(vertices[gl_VertexID], 1.0, 1.0);
}