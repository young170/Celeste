#version 430 core

// structs
struct Transform
{
  vec2 pos;
  vec2 size;
  ivec2 atlasOffset;
  ivec2 spriteSize;
};

// input
layout(std430, binding = 0) buffer TransformSBO
{
  Transform transforms[];
};

uniform vec2 screenSize;

// output
layout (location = 0) out vec2 textureCoordsOut;

void main()
{
  Transform transform = transforms[gl_InstanceID];

  vec2 vertices[6] =
  {
    transform.pos,                                        // Top Left
    vec2(transform.pos + vec2(0.0, transform.size.y)),    // Bottom Left
    vec2(transform.pos + vec2(transform.size.x, 0.0)),    // Top Right
    vec2(transform.pos + vec2(transform.size.x, 0.0)),    // Top Right
    vec2(transform.pos + vec2(0.0, transform.size.y)),    // Bottom Left
    transform.pos + transform.size                        // Bottom Right
  };

  int left = transform.atlasOffset.x;
  int top = transform.atlasOffset.y;
  int right = transform.atlasOffset.x + transform.spriteSize.x;
  int bottom = transform.atlasOffset.y + transform.spriteSize.y;

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

  // normalize GL position
  {
    vec2 vertexPos = vertices[gl_VertexID];
    vertexPos.y = -vertexPos.y + screenSize.y; // y-coord in OpenGL is inverted
    vertexPos = 2.0 * (vertexPos / screenSize) - 1.0; // normalize coord
    gl_Position = vec4(vertexPos, 0.0, 1.0);
  }
  
  textureCoordsOut = textureCoords[gl_VertexID];
}