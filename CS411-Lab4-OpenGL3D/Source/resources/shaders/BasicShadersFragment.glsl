#version 330 core
layout (location = 0) out vec4 color;
in vec2 v_TexCoord;

uniform vec4 u_Color = vec4(1.0, 1.0, 1.0, 1.0);
uniform sampler2D u_Texture;

void main() {
	color = texture(u_Texture, v_TexCoord) * u_Color;
}
