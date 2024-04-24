#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT {
	vec2 texCoords;
	mat4 vProjection;
	mat4 vModel;
	mat4 vView;
} gs_in[];

out vec2 TexCoords;

uniform float time;

uniform int exploding;
uniform int shattering;
uniform float minY;

vec4 explode(vec4 position, vec3 normal) {
	vec3 direction = normal * (time / 2.0) * -1;
	return position + vec4(direction, 0.0);
}

vec4 shatter(vec4 position, vec3 normal) {
	float magnitude = -1.0;
	vec3 direction = ((normal) + vec3(0.0, magnitude, 0.0)) * (time / 2.0);
	vec4 finalPosition = position + vec4(direction, 0.0);

	return finalPosition;
}

vec3 GetNormal(){
	vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
	vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
	return normalize(cross(a,b));
}

void main(){
	vec3 normal = GetNormal();
	if(exploding == 1){
		gl_Position = gs_in[0].vProjection * gs_in[0].vView * explode(gl_in[0].gl_Position, normal);
		TexCoords = gs_in[0].texCoords;
		EmitVertex();
		gl_Position = gs_in[1].vProjection * gs_in[1].vView * explode(gl_in[1].gl_Position, normal);
		TexCoords = gs_in[1].texCoords;
		EmitVertex();
		gl_Position = gs_in[2].vProjection * gs_in[1].vView * explode(gl_in[2].gl_Position, normal);
		TexCoords = gs_in[2].texCoords;
		EmitVertex();
		EndPrimitive();
	}
	else if(shattering == 1){
		gl_Position = gs_in[0].vProjection * gs_in[0].vView * shatter(gl_in[0].gl_Position, normal);
		TexCoords = gs_in[0].texCoords;
		EmitVertex();
		gl_Position = gs_in[1].vProjection * gs_in[1].vView * shatter(gl_in[1].gl_Position, normal);
		TexCoords = gs_in[1].texCoords;
		EmitVertex();
		gl_Position = gs_in[2].vProjection * gs_in[2].vView * shatter(gl_in[2].gl_Position, normal);
		TexCoords = gs_in[2].texCoords;
		EmitVertex();
		EndPrimitive();
	}
	else{
		gl_Position = gs_in[0].vProjection * gs_in[0].vView * gl_in[0].gl_Position;
		TexCoords = gs_in[0].texCoords;
		EmitVertex();
		gl_Position = gs_in[1].vProjection * gs_in[1].vView * gl_in[1].gl_Position;
		TexCoords = gs_in[1].texCoords;
		EmitVertex();
		gl_Position = gs_in[2].vProjection * gs_in[2].vView * gl_in[2].gl_Position;
		TexCoords = gs_in[2].texCoords;
		EmitVertex();
	}
}