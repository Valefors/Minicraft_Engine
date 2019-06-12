#version 400

in vec2 uv;

uniform sampler2D TexColor;
uniform sampler2D TexDepth;
uniform float screen_width;
uniform float screen_height;
uniform vec2 near_far;

out vec4 color_out;

float LinearizeDepth(float z)
{
	float n = near_far.x; // camera z near
  	float f = near_far.y; // camera z far
  	return (2.0 * n) / (f + n - z * (f - n));
}

void main (void)
{
	float xstep = 1.0/screen_width; // Taille d'un pixel
	float ystep = 1.0/screen_height;
	float ratio = screen_width / screen_height;

	vec4 color = texture2D( TexColor , uv );
	float depth = texture2D( TexDepth , uv ).r;	
	
	//Permet de scaler la profondeur
	depth = LinearizeDepth(depth);

//	// DOF
//	// Flou
	int tailleFlou = 5;
	vec3 colorFlou = vec3(0,0,0);
	float sommePoids = 0;
	for(int x=-tailleFlou; x<=tailleFlou; x++)
		for (int y=-tailleFlou; y<=tailleFlou; y++)
		{
			vec2 offset = vec2(xstep*x, ystep*y);
			float poids = length(offset);
			colorFlou += texture2D(TexColor, uv + offset).rgb * poids;
			sommePoids += poids;
		}
	colorFlou /= sommePoids;
	//color.rgb = colorFlou.rgb; // Pour appliquer du flou à tout l'image
//	// Profondeur
	float depthCenter = texture2D(TexDepth, vec2(0.5,0.5)).r;
	depthCenter = LinearizeDepth(depthCenter);
	float deltaDepth = clamp(abs(depthCenter-depth)*2,0,1);
	color.rgb = mix(color.rgb, colorFlou.rgb, deltaDepth);

//	// Outline https://gamedev.stackexchange.com/questions/159585/sobel-edge-detection-on-depth-texture
	int tailleSobel = 1;
	vec3 colorSobel = 9 * color.rgb;
	for(int x=-tailleSobel; x<=tailleSobel; x++)
		for (int y=-tailleSobel; y<=tailleSobel; y++)
		{		
			vec2 offset = vec2(xstep*x, ystep*y);
			colorSobel -= texture2D(TexColor, uv + offset).rgb;
		}
	color.rgb += abs(colorSobel);

//	// Vignette
//	float distanceToCenter = min(1.5, 2 * length(uv - vec2(0.5,0.5)));
//	float vignette = clamp(pow(clamp(distanceToCenter - 0.5f,0,1),2),0,1); // Clamp between 0 & 1
//	//color = vec4(vignette, vignette, vignette, 1);
//	color *= (1 - vignette);

	//color = vec4(depth,0,0,1); // On peut utiliser ça pour fog, plus simple
	// Outline : vérifier que depth est pas trop différente sinon faire outline	
	//color = 1 - color;

    //Gamma correction
    color.r = pow(color.r,1.0/2.2);
    color.g = pow(color.g,1.0/2.2);
    color.b = pow(color.b,1.0/2.2);

	color_out = vec4(color.rgb,1.0);
}