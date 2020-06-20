#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;

out vec2 v_TexCoord;

uniform mat4 u_MVP = mat4(1);
uniform mat4 u_Transform = mat4(1);

void main() {
	
	gl_Position = u_MVP * u_Transform * vec4(position, 1.0);
	v_TexCoord = texCoord + vec2(position.x - 0.5, position.y - 0.5);
}