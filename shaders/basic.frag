#version 410 core

in vec3 fPosition;    
in vec3 fNormal;
in vec2 fTexCoords;
in vec4 fPosLightSpace;

out vec4 fColor;


uniform mat4 model;
uniform mat4 view;
uniform mat3 normalMatrix;

// directional light
uniform vec3 lightDir;      
uniform vec3 lightColor;
uniform float dirLightIntensity;

// point light (NEW)
uniform vec3 pointLightPos;      
uniform vec3 pointLightColor;
uniform vec3 pointLightAtt;      

// textures
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;

uniform int isSky;

uniform int isFlat; 
uniform sampler2D shadowMap;
uniform mat4 lightSpaceTrMatrix;

uniform float time; 

vec3 ambient;
float ambientStrength = 0.05f;  // kept as you had it
float specularStrength = 1.2f;  // kept as you had it

vec3 diffuseDir;
vec3 specularDir;

vec3 diffusePoint;
vec3 specularPoint;

// RAIN 
float hash(vec2 co) {
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

float computeRain() {
    vec2 uv = gl_FragCoord.xy;
    
    uv.x /= 2.0;  
    uv.y /= 50.0; 
    
    float rainSpeed = 20.0; 
    uv.y += time * rainSpeed;
    
    vec2 cell = floor(uv);
    vec2 local = fract(uv);
    
    float randomVal = hash(cell);
    
    if (randomVal > 0.98) { 
        float drop = 1.0 - local.y; 
        return drop * 0.3; 
    }
    
    return 0.0;
}

void computeDirLight(vec3 normalEye, vec3 viewDir)
{

    vec3 lightDirN = normalize(vec3(view * vec4(lightDir, 0.0f)));

    vec3 L = lightColor * dirLightIntensity;
    ambient    = ambientStrength * L;
    diffuseDir = max(dot(normalEye, lightDirN), 0.0f) * L;

    vec3 reflectDir = reflect(-lightDirN, normalEye);
    float specCoeff = pow(max(dot(viewDir, reflectDir), 0.0f), 32.0f);
    specularDir = specularStrength * specCoeff * L;
}

float computeFog()
{
    // --- CONFIGURARE ---
    float waterLevel = 5.0f; 
    
    float fogThickness = 2.0f; 
    
    // --- CALCUL ---
    float fogBottom = waterLevel - 2.0f; 
    float fogTop = waterLevel + fogThickness; 

    // 1. Height Fog
    float heightFactor = smoothstep(fogBottom, fogTop, fPosition.y);

    // 2. Distance Fog
    float dist = length((view * vec4(fPosition, 1.0f)).xyz);
    float distanceFog = exp(-pow(dist * 0.005f, 2)); 

    // 3. Combinare
    float finalFog = mix(distanceFog, 1.0f, heightFactor);

    return clamp(finalFog, 0.0f, 1.0f);
}

void computePointLight(vec3 posEye, vec3 normalEye, vec3 viewDir)
{
    vec3 pointPosEye = vec3(view * vec4(pointLightPos, 1.0f));

    vec3 L = pointPosEye - posEye;
    float dist = length(L);
    vec3 Ldir = normalize(L);

    // attenuation
    float att = 1.0f / (pointLightAtt.x + pointLightAtt.y * dist + pointLightAtt.z * dist * dist);

    // diffuse
    diffusePoint = max(dot(normalEye, Ldir), 0.0f) * pointLightColor * att;

    // specular
    vec3 reflectDir = reflect(-Ldir, normalEye);
    float specCoeff = pow(max(dot(viewDir, reflectDir), 0.0f), 32.0f);
    specularPoint = specularStrength * specCoeff * pointLightColor * att;
}

float computeShadow()
{
    // Perform perspective divide
    vec3 projCoords = fPosLightSpace.xyz / fPosLightSpace.w;

    // Transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    // Get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    // If outside the light frustum, no shadow
    if (projCoords.z > 1.0)
        return 0.0;

    vec3 normal = normalize(normalMatrix * fNormal);
    vec3 lightDirN = normalize(lightDir);
    float bias = max(0.05 * (1.0 - dot(normal, lightDirN)), 0.005);

    //PCF  
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0; 

    return shadow;
}

void main()
{
    float shadow = computeShadow();
    
    vec4 sceneColor; 

    if (isSky == 1) {
        sceneColor = texture(diffuseTexture, fTexCoords);
    } else {
        vec3 posEye = vec3(view * vec4(fPosition, 1.0f));
        vec3 viewDir = normalize(-posEye); 
        
        vec3 normalEye;

        if (isFlat == 1) {
            vec3 xTangent = dFdx(posEye);
            vec3 yTangent = dFdy(posEye);
            normalEye = normalize(cross(xTangent, yTangent));
        } else {
            normalEye = normalize(normalMatrix * fNormal);
        }
    
        computeDirLight(normalEye, viewDir);
        computePointLight(posEye, normalEye, viewDir);

        vec3 kd = texture(diffuseTexture, fTexCoords).rgb;
        vec3 ks = texture(specularTexture, fTexCoords).rgb;

        vec3 color = ambient * kd                                 
                   + (1.0 - shadow) * diffuseDir * kd             
                   + (1.0 - shadow) * specularDir * ks            
                   + diffusePoint * kd                            
                   + specularPoint * ks;                          
        
        float fogFactor = computeFog();
        vec4 fogColor = vec4(0.5f, 0.7f, 1.0f, 1.0f); 
        
        sceneColor = mix(fogColor, vec4(color, 1.0f), fogFactor);
    }
    
    float rainIntensity = computeRain();
    vec4 rainColor = vec4(1.0, 1.0, 1.0, 1.0); // White rain streaks
    
    fColor = mix(sceneColor, rainColor, rainIntensity);
}