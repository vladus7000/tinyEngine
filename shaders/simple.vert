#version 400

in vec3 normal;
in vec3 position;
in vec2 texCoord;
in vec3 tangent;
in vec3 bitangent;

out vec2 v_texCoord;
out vec3 v_normal;
out vec3 v_tangent;
out vec3 v_bitangent;
out vec3 v;

uniform mat4 projMatrix;
uniform mat4 worldMatrix;
uniform mat4 cameraMatrix;
uniform mat4 normalMatrix;

void main()
{
	v_texCoord = texCoord;

	v_normal = vec3(normalMatrix * vec4(normal, 0.0));
	v_tangent = vec3(normalMatrix * vec4(tangent, 0.0));
	v_bitangent = vec3(normalMatrix * vec4(bitangent, 0.0));

	v = normalize(vec3(0, 0, 0) - vec3( worldMatrix * vec4(position, 1))) ;

	gl_Position = projMatrix * cameraMatrix * worldMatrix * vec4(position, 1);
}