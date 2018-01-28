#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aUV;

uniform mat4 uWorld;
uniform mat4 uView;
uniform mat4 uProj;

uniform float uTime;
uniform sampler2D uNoiseTexture;

out vec2 vUV;
out vec3 FragPos;
out vec3 vNormal;

void main() {
		

	vUV = vec2(aUV.x, 1.0 - aUV.y);
	
	vec4 worldPos = uWorld * vec4(aPosition,1);

	float amnt = sin(uTime + worldPos.x * worldPos.z) * 0.1;	
	float amnt2 = sin(uTime * 0.5 + worldPos.x * worldPos.z) * 0.1;
	vec4 c = texture(uNoiseTexture, vUV + vec2(amnt2));
	float totalOffset = amnt + (c.r - 0.5) * 0.5;
	worldPos.y += totalOffset * 0.5;

	gl_Position = (uProj * uView) * worldPos;
	
	
    FragPos = vec3(uWorld * vec4(aPosition, 1.0));
	
    vNormal = vec3(0, 1, totalOffset * 2.0);
}
