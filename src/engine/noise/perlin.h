#pragma once

#include "engine/utils/types_3d.h"

class YPerlin
{
public:
	YVec3f * Gradients;
	int Width;
	int Size;
	float Freq;

	YPerlin()
	{
		Width = 41;
		Size = Width * Width*Width;
		Gradients = new YVec3f[Size];
		updateVecs();
		Freq = 1;
	}

	void updateVecs() {
		for (int i = 0; i < Size; i++)
		{
			Gradients[i].X = (float)randf();
			Gradients[i].Y = (float)randf();
			Gradients[i].Z = (float)randf();
		}
	}

	float lerp(float a, float b, float alpha) {
		float alphaSmooth = alpha * (3 * alpha - 2 * alpha*alpha);
		return (1 - alphaSmooth)*a + alphaSmooth * b;
	}
public:

	void setFreq(float freq)
	{
		Freq = freq;
	}

	virtual float sample(float xBase, float yBase, float zBase)
	{
		float x = xBase * Freq;
		float y = yBase * Freq;
		float z = zBase * Freq;

		while (x >= Width - 1)
			x -= Width - 1;
		while (y >= Width - 1)
			y -= Width - 1;
		while (z >= Width - 1)
			z -= Width - 1;

		int x1 = (int)floor(x);
		int x2 = (int)floor(x) + 1;
		int y1 = (int)floor(y);
		int y2 = (int)floor(y) + 1;
		int z1 = (int)floor(z);
		int z2 = (int)floor(z) + 1;
		float dx = x - x1;
		float dy = y - y1;
		float dz = z - z1;

		YVec3f pos(x, y, z);
		YVec3f sommets[8];
		//plan X2
		sommets[0] = YVec3f((float)x2, (float)y1, (float)z1);
		sommets[1] = YVec3f((float)x2, (float)y1, (float)z2);
		sommets[2] = YVec3f((float)x2, (float)y2, (float)z2);
		sommets[3] = YVec3f((float)x2, (float)y2, (float)z1);

		//plan X1
		sommets[4] = YVec3f((float)x1, (float)y1, (float)z1);
		sommets[5] = YVec3f((float)x1, (float)y1, (float)z2);
		sommets[6] = YVec3f((float)x1, (float)y2, (float)z2);
		sommets[7] = YVec3f((float)x1, (float)y2, (float)z1);

		float angles[8];
		for (int i = 0; i < 8; i++)
			angles[i] = (pos - sommets[i]).dot(Gradients[(int)(sommets[i].X*Width*Width + sommets[i].Y*Width + sommets[i].Z)]);

		//plan X2
		float ybas = lerp(angles[0], angles[3], dy);
		float yhaut = lerp(angles[1], angles[2], dy);
		float mid2 = lerp(ybas, yhaut, dz);

		//plan X1
		ybas = lerp(angles[4], angles[7], dy);
		yhaut = lerp(angles[5], angles[6], dy);
		float mid1 = lerp(ybas, yhaut, dz);

		float res = lerp(mid1, mid2, dx);

		res = (res + 1) / 2.0f;

		//Milieu
		return min(1, max(0, res));
	}
};

#include <vector>

// THIS CLASS IS A TRANSLATION TO C++11 FROM THE REFERENCE
// JAVA IMPLEMENTATION OF THE IMPROVED PERLIN FUNCTION (see http://mrl.nyu.edu/~perlin/noise/)
// THE ORIGINAL JAVA IMPLEMENTATION IS COPYRIGHT 2002 KEN PERLIN

// I ADDED AN EXTRA METHOD THAT GENERATES A NEW PERMUTATION VECTOR (THIS IS NOT PRESENT IN THE ORIGINAL IMPLEMENTATION)

class PerlinNoise
{
	// The permutation vector
	std::vector<int> p;

public:

	// Initialize with the reference values for the permutation vector
	PerlinNoise()
	{
		// Initialize the permutation vector with the reference values
		p = {
			151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,
			8,99,37,240,21,10,23,190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,
			35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,
			134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,
			55,46,245,40,244,102,143,54, 65,25,63,161,1,216,80,73,209,76,132,187,208, 89,
			18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,
			250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,
			189,28,42,223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167,
			43,172,9,129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,
			97,228,251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,
			107,49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
			138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180 };
		// Duplicate the permutation vector
		p.insert(p.end(), p.begin(), p.end());
	}

	// Get a noise value, for 2D images z can have any value
	double noise(double x, double y, double z)
	{
		// Find the unit cube that contains the point
		int X = (int)floor(x) & 255;
		int Y = (int)floor(y) & 255;
		int Z = (int)floor(z) & 255;

		// Find relative x, y,z of point in cube
		x -= floor(x);
		y -= floor(y);
		z -= floor(z);

		// Compute fade curves for each of x, y, z
		double u = fade(x);
		double v = fade(y);
		double w = fade(z);

		// Hash coordinates of the 8 cube corners
		int A = p[X] + Y;
		int AA = p[A] + Z;
		int AB = p[A + 1] + Z;
		int B = p[X + 1] + Y;
		int BA = p[B] + Z;
		int BB = p[B + 1] + Z;

		// Add blended results from 8 corners of cube
		double res = lerp(w, lerp(v, lerp(u, grad(p[AA], x, y, z), grad(p[BA], x - 1, y, z)), lerp(u, grad(p[AB], x, y - 1, z), grad(p[BB], x - 1, y - 1, z))), lerp(v, lerp(u, grad(p[AA + 1], x, y, z - 1), grad(p[BA + 1], x - 1, y, z - 1)), lerp(u, grad(p[AB + 1], x, y - 1, z - 1), grad(p[BB + 1], x - 1, y - 1, z - 1))));
		return (res + 1.0) / 2.0;
	}

private:
	double fade(double t)
	{
		return t * t* t* (t * (t * 6 - 15) + 10);
	}

	double lerp(double t, double a, double b)
	{
		return a + t * (b - a);
	}

	double grad(int hash, double x, double y, double z)
	{
		int h = hash & 15;
		// Convert lower 4 bits of hash into 12 gradient directions
		double u = h < 8 ? x : y,
			v = h < 4 ? y : h == 12 || h == 14 ? x : z;
		return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
	}
};
