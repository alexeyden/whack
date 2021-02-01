#version 130
varying highp vec2 var_uv;
varying highp vec3 light_color;

uniform sampler2D tex_sampler;
uniform lowp float time;

varying vec2 var_pos;
uniform bool grid;
uniform vec4 color;
uniform bool highlight;
uniform highp vec2 sel_0;
uniform highp vec2 sel_1;

void main() {
	vec4 tex = color;
	bool is_sel = false;
	
	if (var_pos.x+0.05 >= sel_0.x && var_pos.y+0.05 >= sel_0.y && var_pos.x-0.05 <= sel_1.x+1 && var_pos.y-0.05 <= sel_1.y+1 || highlight)
		is_sel = true;
	
	if(color.a == 0.0)
		tex = texture2D(tex_sampler, var_uv);
	
	if(is_sel) {
		if(tex.r == 1.0 && tex.g == 0.0 && tex.b == 1.0)
			discard;
		else {
			float k = abs(sin(time)) + 4.0;
			gl_FragColor = vec4(k, k, k, 1.0) * tex;
		}
	}
	else if(grid &&
		(fract(var_pos.x) >= 0 && fract(var_pos.x) <= 0.02 || fract(var_pos.y) >= 0 && fract(var_pos.y) <= 0.02 || fract(var_pos.x) >= 0.98 || fract(var_pos.y) >= 0.98))
		gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
	else {
		if(tex.r == 1.0 && tex.g == 0.0 && tex.b == 1.0)
			discard;
		else
			gl_FragColor = vec4(light_color,1.0) * tex;
	}
}
