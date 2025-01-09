#version 430 core

// input

// output
layout (location = 0) out vec2 textureCoordsOut;

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

  float left = 0.0;
  float top = 0.0;
  float right = 16.0;
  float bottom = 16.0;

  vec2 textureCoords[6] = 
  {
    vec2(left, top),
    vec2(left, bottom),
    vec2(right, top),
    vec2(right, top),
    vec2(left, bottom),
    vec2(right, bottom)
  };

  gl_Position = vec4(vertices[gl_VertexID], 1.0, 1.0);
  textureCoordsOut = textureCoords[gl_VertexID];
}