// sea.frag
#version 330 core
in vec4 ViewSpace;
in vec3 Normal;
in vec4 LightSpaceFragPos;
in vec3 FragPos;

layout (location = 0) out vec4 colorTexture;
// layout (location = 1) out vec4 velocityTexture;

uniform vec3 lightPos;
uniform sampler2D shadowMap;
uniform float ambientLightIntensity;

float shadowCalculation(vec4 lightSpaceFragPos) {
  vec3 projCoords = lightSpaceFragPos.xyz / lightSpaceFragPos.w;
  projCoords = projCoords * 0.5 + 0.5;
  float closestDepth = texture(shadowMap, projCoords.xy).r;
  float currentDepth = projCoords.z;
  if (currentDepth > 1.0)
    return 0;
  float shadow = 0.0;
  float bias = max(0.05 * (1.0 - dot(Normal, normalize(-lightPos))), 0.001);
  vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
  int sampleSize = 1;
  for(int x = -sampleSize; x <= sampleSize; ++x) {
    for(int y = -sampleSize; y <= sampleSize; ++y) {
        float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
        shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
    }
  }
  return shadow / ((sampleSize * 2 + 1) * (sampleSize * 2 + 1));
}

void main() {
  // Đổi màu nước → màu đất
  vec3 groundColor = vec3(0.4, 0.3, 0.2);  // Màu nâu đất
  vec3 grassColor = vec3(0.3, 0.5, 0.2);   // Màu xanh cỏ
  vec3 terrainColor = mix(groundColor, grassColor, 0.6);  // Pha trộn
  
  vec3 fogColor = vec3(0.69, 0.88, 0.9); // Màu bầu trời xanh (powder blue)  // directional light
  vec3 lightColor = vec3(1.0, 1.0, 1.0);
  vec3 lightDir = normalize(FragPos - lightPos);

  // ambient
  float ambientStrength = 0.15 * ambientLightIntensity;
  vec3 ambient = ambientStrength * lightColor;

  // diffuse
  float diff = max(dot(Normal, lightDir), 0.0);
  vec3 diffuse = diff * lightColor;

  // shadow
  float visibility = 1.0;
  float shadow = visibility * shadowCalculation(LightSpaceFragPos);
  vec3 fragColor = (ambient + (1 - shadow) * diffuse) * terrainColor;

  // fog
  float dist = abs(ViewSpace.z);
  float near = 100.0;
  float far = 300.0;
  float fogFactor = (far - dist)/(far - near);
  fogFactor = clamp(fogFactor, 0.0, 1.0);

  vec3 finalColor = (1.0 - fogFactor) * fogColor + fogFactor * fragColor;
  colorTexture = vec4(finalColor, min(0.8, fogFactor));

  // velocity
  // velocityTexture = vec4(1, 1, 1, 1);
}
