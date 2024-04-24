#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT {
	vec2 texCoords;
} gs_in[];

out vec2 TexCoords;

uniform float time;

uniform int exploding;
uniform int shattering;
uniform float minY;

vec4 explode(vec4 position, vec3 normal) {
	float magnitude = 5.0;
	vec3 direction = normal * (time / 2.0);
	return position + vec4(direction, 0.0);
}

vec4 shatter(vec4 position, vec3 normal) {
	float magnitude = -1.0;
	vec3 direction = ((normal/2) + vec3(0.0, magnitude, 0.0)) * (time / 2.0);
	vec4 finalPosition = position + vec4(direction, 0.0);
	return finalPosition;
}

vec3 GetNormal(){
	vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
	vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
	return normalize(cross(a,b));
}

void main(){

	if(exploding == 1){
		vec3 normal = GetNormal();
		gl_Position = explode(gl_in[0].gl_Position, normal);
		TexCoords = gs_in[0].texCoords;
		EmitVertex();
		gl_Position = explode(gl_in[1].gl_Position, normal);
		TexCoords = gs_in[1].texCoords;
		EmitVertex();
		gl_Position = explode(gl_in[2].gl_Position, normal);
		TexCoords = gs_in[2].texCoords;
		EmitVertex();
		EndPrimitive();
	}
	else if(shattering == 1){
		vec3 normal = GetNormal();
		gl_Position = shatter(gl_in[0].gl_Position, normal);
		TexCoords = gs_in[0].texCoords;
		EmitVertex();
		gl_Position = shatter(gl_in[1].gl_Position, normal);
		TexCoords = gs_in[1].texCoords;
		EmitVertex();
		gl_Position = shatter(gl_in[2].gl_Position, normal);
		TexCoords = gs_in[2].texCoords;
		EmitVertex();
	}
	else{
		gl_Position = gl_in[0].gl_Position;
		TexCoords = gs_in[0].texCoords;
		EmitVertex();
		gl_Position = gl_in[1].gl_Position;
		TexCoords = gs_in[1].texCoords;
		EmitVertex();
		gl_Position = gl_in[2].gl_Position;
		TexCoords = gs_in[2].texCoords;
		EmitVertex();
	}
}