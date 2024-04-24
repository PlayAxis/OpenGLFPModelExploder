#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoords;

out VS_OUT {
	vec2 texCoords;
	mat4 vProjection;
	mat4 vModel;
	mat4 vView;
} vs_out;



uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
	vs_out.texCoords = aTexCoords;
	vs_out.vProjection = projection;
	vs_out.vModel = model;
	vs_out.vView = view;
	gl_Position = model * vec4(aPos, 1.0);
}