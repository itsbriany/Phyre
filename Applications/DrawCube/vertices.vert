#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (set = 0, binding = 0) uniform UBO {
    mat4 model;
    mat4 view;
    mat4 projection;
} ubo;

layout (location = 0) in vec4 pos;
layout (location = 1) in vec4 inColor;
layout (location = 0) out vec4 outColor;

out gl_PerVertex { 
    vec4 gl_Position;
};

void main() {
   outColor = inColor;
   gl_Position = ubo.projection * ubo.view * ubo.model * pos;
}