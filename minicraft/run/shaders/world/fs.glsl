#version 400

//Variables en entree
in vec3 normal;
in vec4 color;
in vec2 uv;
flat in int type;
flat in float specLevel;

uniform vec3 lightDir;
uniform vec3 sunColor;
uniform vec3 skyColor;
uniform vec3 camPos;
uniform float world_size;

in vec3 worldPos;
in float pDist;

uniform float elapsed;

uniform sampler2D colTex;

// Sliders
uniform float slider_0; // Ambient
uniform float slider_1; // Fog
uniform float slider_2; // Up Fog
uniform float slider_3; // Move Fog
uniform float slider_4;

out vec4 color_out;

#define CUBE_HERBE 0.0
#define CUBE_TERRE 1.0
#define CUBE_BOIS 2.0
#define CUBE_EAU 3.0
#define CUBE_PIERRE 4.0
#define CUBE_SABLE_01 17.0

//Globales
//const float ambientLevel = slider_0;

//float rand(float n){return fract(sin(n) * 43758.5453123);} // Fonction random

float noiseWater(vec3 surfPos, float time, float world_size)
{
	float dist = length(vec2(world_size/2, world_size/2)-surfPos.xy)/(world_size/2);
	float offset = - 1.5f;
	offset += 1.5f * sin(surfPos.x/5.0F + time)*pow(dist,3);
	offset += 1.5f * sin(dist*5+time)*pow(dist, 3);

	float offset2 = 0.5f * sin(surfPos.y*3+time*6)*pow(dist, 3);
	offset2 += 0.3f * sin(length(surfPos.xy)*5+time*4)*pow(dist, 3);
	offset2 *= sin(surfPos.x/5.0f + time);

	return max(offset, offset2);
}

void main()
{
	// Texture
	vec4 colorTex = texture2D(colTex,vec2((uv.x+float(type))/32.f,uv.y/2)).rgba; // Mapping de la texture

	// Traitement pour de l'eau, calcul nouvelle normale
	vec3 normalFs = normal;
	vec4 albedo = color;
	if (type == CUBE_EAU)
	{
		vec3 A = worldPos;
		vec3 B = worldPos + vec3(0.2,0,0);
		vec3 C = worldPos ;//+ vec3(0,0.2,0);

		A.z += noiseWater(A, elapsed, world_size);
		B.z += noiseWater(B, elapsed, world_size);
		C.z += noiseWater(C, elapsed, world_size);

		normalFs = normalize(cross(normalize(B-A), normalize(C-A)));
	}
	else
	{
		albedo = colorTex;
	}

	// Diffuse
	float diffuse = dot(normalize(lightDir), normalFs);
	diffuse = clamp(diffuse, 0.001f, 0.4f);
	vec3 colorShaded = diffuse * albedo.xyz;

	// Speculaire
	vec3 halfVector = normalize(normalize(lightDir) + normalize(camPos - worldPos));
	float spec = max(0, dot(normalFs, halfVector));
	spec = specLevel * pow(spec, 50);
//	float bruit = rand(uv*3); // Random, donne aspect granuleux au sable (bruit)
//	if (bruit > 0.5)
	if (type == CUBE_SABLE_01)
	{
		if (albedo.r > 0.76)
			spec *= 2;
		else
			spec /= 2;
	}
	//colorShaded += spec * sunColor;

	// Ambient
	float ambientLevel = slider_0;
	colorShaded += ambientLevel * pow(1-diffuse, 10) * skyColor;

	// Fog
	float brDist = pow(min(1, pDist/50) * slider_1, 2);
	float brUp = 1 - pow (min(1,(worldPos.z / 65)) * slider_2, 2);
	float brMove = (1+sin(worldPos.x / 10 + elapsed))/2 * (1+sin(worldPos.y / 15 + elapsed/5))/2 * (1+sin(worldPos.z / 15 + elapsed/5))/2;
	float br = min(brDist, brUp); // Distance ou hauteur
	br *= max(1-slider_3, brMove);
	//colorShaded = mix(colorShaded, skyColor, br);
	colorShaded = mix(colorShaded, skyColor, pow(min(1, (pDist / 100)) * slider_1, 5)); // Equivalent à lerp, pow pour brouillard pas linéaire, min pour pas dépasser 1 sinon comportements étranges

	//color_out = vec4(sqrt(colorShaded), color.a);
	color_out = vec4(sqrt(colorShaded), albedo.a); // Pour Texture
}