#version 420 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 8) out;

in EXPORT
{
	vec3 norm;
	vec3 colors;
	vec3 viewPosition;
	vec3 fragPos;
} vs_export[];

out EXPORT
{
	vec3 norm;
	vec3 colors;
	vec3 viewPosition;
	vec3 fragPos;
} gs_export;

out vec2 uv_frag;
out	float height_frag;

in 	vec2 uv[];
in	float height[];

in int type[];

uniform float higher, lower;

void exportToFrag(int index){
	gs_export.norm = vs_export[index].norm;
	gs_export.colors = vs_export[index].colors;
	gs_export.viewPosition = vs_export[index].viewPosition;
	gs_export.fragPos = vs_export[index].fragPos;
	uv_frag = uv[index];
	height_frag = height[index];
}

void build_grass(int index) {
	vec4 pos = gl_in[index].gl_Position;
	float h = height[index];

	if (h < lower || h > higher) return;


	gl_Position = pos + vec4(-0.2, -0.2, 0.0, 0.0);
	exportToFrag(index);
	gs_export.colors = vec3(0.0, 0.8, 0.1);
	EmitVertex();

	gl_Position = pos + vec4( 0.2, -0.2, 0.0, 0.0);
	exportToFrag(index);
	gs_export.colors = vec3(0.0, 0.8, 0.1);
	EmitVertex();

	gl_Position = pos + vec4(-0.2, 5.0, 0.0, 0.0);
	exportToFrag(index);
	gs_export.colors = vec3(0.0, 0.8, 0.1);
	EmitVertex();

	gl_Position = pos + vec4( 0.2, 5.0, 0.0, 0.0);
	exportToFrag(index);
	gs_export.colors = vec3(0.0, 0.8, 0.1);
	EmitVertex();

	gl_Position = pos + vec4( 0.0, 6.0, 0.0, 0.0);
	exportToFrag(index);
	gs_export.colors = vec3(0.0, 0.8, 0.1);
	EmitVertex();

	EndPrimitive();
}

void build_floor() {
	gl_Position = gl_in[0].gl_Position;
	gl_ClipDistance = gl_in[0].gl_ClipDistance;
	gs_export.norm = vs_export[0].norm;
	gs_export.colors = vs_export[0].colors;
	gs_export.viewPosition = vs_export[0].viewPosition;
	gs_export.fragPos = vs_export[0].fragPos;
	uv_frag = uv[0];
	height_frag = height[0];

	EmitVertex();

	gl_Position = gl_in[1].gl_Position;
	gl_ClipDistance = gl_in[1].gl_ClipDistance;
	gs_export.norm = vs_export[1].norm;
	gs_export.colors = vs_export[1].colors;
	gs_export.viewPosition = vs_export[1].viewPosition;
	gs_export.fragPos = vs_export[1].fragPos;
	uv_frag = uv[1];
	height_frag = height[1];
	EmitVertex();

	gl_Position = gl_in[2].gl_Position;
	gl_ClipDistance = gl_in[2].gl_ClipDistance;
	gs_export.norm = vs_export[2].norm;
	gs_export.colors = vs_export[2].colors;
	gs_export.viewPosition = vs_export[2].viewPosition;
	gs_export.fragPos = vs_export[2].fragPos;
	uv_frag = uv[2];
	height_frag = height[2];
	EmitVertex();

	EndPrimitive();
}

void main() {
	build_floor();

	if(type[0] == 1){
		build_grass(0);
		build_grass(1);
		build_grass(2);
	}
}