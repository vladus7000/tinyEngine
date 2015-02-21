#version 400
in vec2 v_texCoord;
in vec3 v_normal;
in vec3 v_tangent;
in vec3 v_bitangent;
in vec3 v;
uniform sampler2D diffuseTexture;
uniform sampler2D normalTexture;

out vec3 out_color;

void main()
{
	vec3 v2 = normalize(v);
	vec3 normal = normalize(v_normal);
	vec3 tangent = normalize(v_tangent);
	vec3 bitangent = normalize(v_bitangent);

	vec3 diffuseColor = texture(diffuseTexture, v_texCoord).rgb;
	vec3 bumpNormal = texture(normalTexture, v_texCoord).rgb;

	bumpNormal = 2.0 * bumpNormal - vec3(1.0, 1.0, 1.0);

	mat3 TBN = transpose (mat3(tangent, bitangent, normal));

	vec3 resNormal = normalize(TBN * bumpNormal);

	float rim = pow (1.0 + 0.15 - max (dot(normal, v2), 0.0), 20);
	out_color = diffuseColor* dot(resNormal, vec3(0,-1, 1)) + rim * vec3(1.0, 0.0, 0.0) ;
}