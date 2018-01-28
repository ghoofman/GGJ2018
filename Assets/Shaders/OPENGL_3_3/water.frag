#version 330 core

in vec2 vUV;
in vec3 FragPos;
in vec3 vNormal;

uniform sampler2D uColorTexture;
uniform vec3 uViewPos;

out vec4 FragColor;

void main() {


	float behindZ = 0;
	if(FragPos.z < -20) {
		behindZ = min(1.0, -(FragPos.z + 20) / 30.0);
	}

	vec4 c = texture(uColorTexture, vUV * vec2(40.0, 40.0));

	
    // Diffuse
    vec3 normal = normalize(vNormal);
	vec3 lightColor = vec3(1,1,1);
	vec3 uLightPos = vec3(-10,45,10);
    vec3 lightDir = normalize(uLightPos - FragPos);
    float diffAmnt = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diffAmnt * lightColor;
	
		
    // Specular
	float spec = (1.0 - normal.y);

	FragColor = vec4((diffuse + spec) * c.rgb, 0.35) * (1.0 - behindZ);
	//FragColor = vec4(1,0,0,1);
}
