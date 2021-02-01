#version 130
attribute highp vec3 pos;
attribute highp vec3 norm;
attribute highp vec2 uv;

varying highp vec2 var_uv;
varying highp vec3 light_color;
varying highp vec2 var_pos;

uniform highp vec3 eye;
uniform highp mat4 proj;
uniform highp mat4 local;
uniform bool use_light;

uniform lowp float time;

void calc_light(void) {
	vec3 l = normalize(vec3(32, 32, 20) - pos);
  vec3 view = normalize(eye - pos);
  
	vec3 ambient = vec3(0.5, 0.5, 0.5);
	vec3 diffuse = 1.2 * vec3(1.0, 1.0, 1.0) * max(0.0, dot(norm, l));
	vec3 specular = vec3(0.0, 0.0, 0.0);
	if(dot(norm, l) > 0.0)
	specular = vec3(1.0, 1.0, 1.0) * 1.2 * pow(max(0.0, dot(reflect(-l, norm), view)), 60);
    
	if(use_light)
		light_color = diffuse + ambient + specular;
	else
		light_color = vec3(1.0, 1.0, 1.0);
}

void main() {
	var_uv = uv;
	calc_light();
	gl_Position = proj * local *  vec4(pos, 1.0);
	
	var_pos = pos.xy;
}
