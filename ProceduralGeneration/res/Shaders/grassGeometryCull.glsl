#version 420 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in EXPORT
{
	vec3 norm;
	vec3 viewPosition;
	vec3 fragPos;
} vs_export[];

out EXPORT
{
	vec3 norm;
	vec3 viewPosition;
	vec3 fragPos;
} gs_export;

out vec2 uv_frag;
out	float height_frag;

in 	vec2 uv[];
in	float height[];
in int cull[];

void exportToFrag(int index){
	gs_export.norm = vs_export[index].norm;
	gs_export.viewPosition = vs_export[index].viewPosition;
	gs_export.fragPos = vs_export[index].fragPos;
	uv_frag = uv[index];
	height_frag = height[index];
}

void build_floor() {
	gl_Position = gl_in[0].gl_Position;
	gl_ClipDistance = gl_in[0].gl_ClipDistance;
	gs_export.viewPosition = vs_export[0].viewPosition;
	gs_export.fragPos = vs_export[0].fragPos;
	uv_frag = uv[0];
	height_frag = height[0];

	EmitVertex();

	gl_Position = gl_in[1].gl_Position;
	gl_ClipDistance = gl_in[1].gl_ClipDistance;
	gs_export.viewPosition = vs_export[1].viewPosition;
	gs_export.fragPos = vs_export[1].fragPos;
	uv_frag = uv[1];
	height_frag = height[1];
	EmitVertex();

	gl_Position = gl_in[2].gl_Position;
	gl_ClipDistance = gl_in[2].gl_ClipDistance;
	gs_export.viewPosition = vs_export[2].viewPosition;
	gs_export.fragPos = vs_export[2].fragPos;
	uv_frag = uv[2];
	height_frag = height[2];
	EmitVertex();

	EndPrimitive();
}

void main() {
	if (cull[0] == 1) {
		build_floor();
	}
}