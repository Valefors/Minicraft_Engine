#version 400

// Variables en entrée
uniform float elapsed;
uniform mat4 m; // Données par les sendmatricestoshader
uniform mat4 v;
uniform mat4 p;
uniform mat4 nmat;
uniform float world_size; // A fournir avec uniform variables dans engine_minicraft
uniform vec3 camPos;

//Variables données par le VBO
layout(location=0) in vec3 vs_position_in;
layout(location=1) in vec3 vs_normal_in;
layout(location=2) in vec2 vs_uv_in;
layout(location=3) in float vs_type_in;

//Variables en sortie
out vec3 normal;
out vec4 color;
out vec2 uv;
flat out int type; // out : ce qu'on sort du vertex et ce que fragment va récupérer + flat pour pas d'interpolation !
out vec3 worldPos; // On le récupère dans fragment shader
out float pDist;
flat out float specLevel;

#define CUBE_EAU 3.0

const vec4 CubeColors[5] = vec4[5](
	vec4(0,1,0,1),
	vec4(0.2,0.1,0,1),
	vec4(0.2,0.1,0,1),
	vec4(0.0,0.0,1.0,0.7),
	vec4(0.2,0.1,0,1)
);

const float SpecLevels[5] = float[5](
	0.1f,
	0.1f,
	0.1f,
	1.0f,
	0.1f
);

//////// NOISE WATER SHADER ////////////////

float noiseWater(vec3 surfPos, float time, float world_size) 
{
	float dist = length(vec2(world_size/2, world_size/2)-surfPos.xy)/(world_size/2);
	float offset = - 1.5f;
	offset += 1.5f * sin(surfPos.x/5.0F + time)*pow(dist,3);
	offset += 1.5f * sin(dist*5+time)*pow(dist, 3);

	return sin(surfPos.x/5 + time);
}

void main()
{
	vec4 vecIn = vec4(vs_position_in,1.0);
	vec4 vecInWorld = m * vecIn;
	worldPos = vecInWorld.xyz;

	if (vs_type_in == CUBE_EAU)
	{
		vecInWorld.z += noiseWater(vecInWorld.xyz,elapsed,world_size);
	}

	vec4 vecInView = v * vecInWorld;


	//////// DISTORTION SHADER ////////////////

	//vecInView.y -= pow(length(vecInView.xyz)/50, 3.5f); //Enable distortion

	//////// ROUND WORLD SHADER ////////////////

	vec4 worldSpacePos = m * vecIn;
	worldSpacePos.xyz -= camPos.xyz; 
	worldSpacePos = vec4( 0.0, 0.0, ((worldSpacePos.x * worldSpacePos.x) + (worldSpacePos.y * worldSpacePos.y))* - 0.05, 0.0);

	vecIn +=  worldSpacePos; 

	/////// Final Render ///////////////////////

	normal = (nmat * vec4(vs_normal_in,1.0)).xyz;
	uv = vs_uv_in;

	gl_Position = p * vecInView; 
	//gl_Position = p * v * m * vecIn; //Enable Round World

	//////// COULEUR DES CUBES ////////////////

	//Couleur par défaut violet
	color = vec4(1.0,1.0,0.0,1.0);

	specLevel = SpecLevels[int(vs_type_in)];
	type = int(vs_type_in);

	//Couleur fonction du type
	color = CubeColors[int(vs_type_in)];
}