#version 400

in vec2 uv;

uniform sampler2D TexColor;
uniform sampler2D TexDepth;
uniform float screen_width;
uniform float screen_height;
uniform vec2 near_far;
uniform float elapsed;

out vec4 color_out;

float LinearizeDepth(float z)
{
	float n = near_far.x;
  	float f = near_far.y; 
  	return (2.0 * n) / (f + n - z * (f - n));
}

float rand(vec2 st)
{
    return fract(sin(dot(st.xy, vec2(12.9898,78.233)))* 43758.5453123);
}

void main (void)
{
	float xstep = 1.0/screen_width;
	float ystep = 1.0/screen_height;
	float ratio = screen_width / screen_height;

	vec4 color = texture2D( TexColor , uv );
	float depth = texture2D( TexDepth , uv ).r;	
	
	depth = LinearizeDepth(depth);

	//// BLUR //////////////////////////

	int sizeBlur = 5;
	vec3 colorBlur = vec3(0,0,0);
	float totalWeight = 0;
	for(int x=-sizeBlur; x<=sizeBlur; x++)
		for (int y=-sizeBlur; y<=sizeBlur; y++)
		{
			vec2 offset = vec2(xstep*x, ystep*y);
			float weight = length(offset);
			colorBlur += texture2D(TexColor, uv + offset).rgb * weight;
			totalWeight += weight;
		}
	colorBlur /= totalWeight;
	//color.rgb = colorBlur.rgb; //Enable Blur


	//// OUTLINE //////////////////////////

	int sizeSobel = 1;
	vec3 colorSobel = 9 * color.rgb;
	for(int x=-sizeSobel; x<=sizeSobel; x++)
		for (int y=-sizeSobel; y<=sizeSobel; y++)
		{		
			vec2 offset = vec2(xstep*x, ystep*y);
			colorSobel -= texture2D(TexColor, uv + offset).rgb;
		}
	//color.rgb += abs(colorSobel); //Enable Outline


    //Gamma correction
    color.r = pow(color.r,1.0/2.2);
    color.g = pow(color.g,1.0/2.2);
    color.b = pow(color.b,1.0/2.2);

	//VHS Effect
	float red = texture2D( TexColor , vec2(uv.x + 0.005, uv.y + 0.005) ).r;
    float green = texture2D( TexColor , vec2(uv.x + 0.005, uv.y - 0.005) ).g;
    float blue = texture2D( TexColor , vec2(uv.x - 0.005, uv.y + 0.005) ).b;

	red = pow(red,1.0/2.2);
    green = pow(green,1.0/2.2);
    blue = pow(blue,1.0/2.2);

	vec3 grain = vec3(rand(uv * elapsed * 0.01));
    grain *= 0.1;

    //color_out = vec4(red, green, blue,1.0) + vec4(grain, 0.0);
	color_out = vec4(color.rgb,1.0f);
}