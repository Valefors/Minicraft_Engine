#ifndef __WORLD_H__
#define __WORLD_H__

#include "external/gl/glew.h"
#include "external/gl/freeglut.h" 
#include "engine/utils/types_3d.h"
#include "cube.h"
#include "chunk.h"
#include "engine/noise/perlin.h"

class MWorld
{
public :
	typedef uint8 MAxis;
	static const int AXIS_X = 0b00000001;
	static const int AXIS_Y = 0b00000010;
	static const int AXIS_Z = 0b00000100;

	#ifdef _DEBUG
	static const int MAT_SIZE = 4; //en nombre de chunks
	#else
	static const int MAT_SIZE = 3; //en nombre de chunks
	#endif // DEBUG

	static const int MAT_HEIGHT = 1; //en nombre de chunks
	static const int MAT_SIZE_CUBES = (MAT_SIZE * MChunk::CHUNK_SIZE);
	static const int MAT_HEIGHT_CUBES = (MAT_HEIGHT * MChunk::CHUNK_SIZE);
	static const int MAT_SIZE_METERS = (MAT_SIZE * MChunk::CHUNK_SIZE * MCube::CUBE_SIZE);
	static const int MAT_HEIGHT_METERS = (MAT_HEIGHT * MChunk::CHUNK_SIZE  * MCube::CUBE_SIZE);

	MChunk * Chunks[MAT_SIZE][MAT_SIZE][MAT_HEIGHT];
	int heightMap[MAT_SIZE_CUBES][MAT_SIZE_CUBES];
	YPerlin * perlinNoise;
	PerlinNoise perlin;

	int waterLevel = 35;
	int level1 = 40;
	int level2 = 45;
	int level3 = 55;
	int level4 = 60;
	int level5 = 64;

	MWorld()
	{
		//On cr�e les chunks
		for(int x=0;x<MAT_SIZE;x++)
			for(int y=0;y<MAT_SIZE;y++)
				for(int z=0;z<MAT_HEIGHT;z++)
					Chunks[x][y][z] = new MChunk(x,y,z);

		for(int x=0;x<MAT_SIZE;x++)
			for(int y=0;y<MAT_SIZE;y++)
				for(int z=0;z<MAT_HEIGHT;z++)
				{
					MChunk * cxPrev = NULL;
					if(x > 0)
						cxPrev = Chunks[x-1][y][z];
					MChunk * cxNext = NULL;
					if(x < MAT_SIZE-1)
						cxNext = Chunks[x+1][y][z];

					MChunk * cyPrev = NULL;
					if(y > 0)
						cyPrev = Chunks[x][y-1][z];
					MChunk * cyNext = NULL;
					if(y < MAT_SIZE-1)
						cyNext = Chunks[x][y+1][z];

					MChunk * czPrev = NULL;
					if(z > 0)
						czPrev = Chunks[x][y][z-1];
					MChunk * czNext = NULL;
					if(z < MAT_HEIGHT-1)
						czNext = Chunks[x][y][z+1];

					Chunks[x][y][z]->setVoisins(cxPrev,cxNext,cyPrev,cyNext,czPrev,czNext);
				}

					
	}

	inline MCube * getCube(int x, int y, int z)
	{	
		if(x < 0)x = 0;
		if(y < 0)y = 0;
		if(z < 0)z = 0;
		if(x >= MAT_SIZE * MChunk::CHUNK_SIZE) x = (MAT_SIZE * MChunk::CHUNK_SIZE)-1;
		if(y >= MAT_SIZE * MChunk::CHUNK_SIZE) y = (MAT_SIZE * MChunk::CHUNK_SIZE)-1;
		if(z >= MAT_HEIGHT * MChunk::CHUNK_SIZE) z = (MAT_HEIGHT * MChunk::CHUNK_SIZE)-1;

		return &(Chunks[x / MChunk::CHUNK_SIZE][y / MChunk::CHUNK_SIZE][z / MChunk::CHUNK_SIZE]->_Cubes[x % MChunk::CHUNK_SIZE][y % MChunk::CHUNK_SIZE][z % MChunk::CHUNK_SIZE]);
	}

	void updateCube(int x, int y, int z)
	{	
		if(x < 0)x = 0;
		if(y < 0)y = 0;
		if(z < 0)z = 0;
		if(x >= MAT_SIZE * MChunk::CHUNK_SIZE)x = (MAT_SIZE * MChunk::CHUNK_SIZE)-1;
		if(y >= MAT_SIZE * MChunk::CHUNK_SIZE)y = (MAT_SIZE * MChunk::CHUNK_SIZE)-1;
		if (z >= MAT_HEIGHT * MChunk::CHUNK_SIZE)z = (MAT_HEIGHT * MChunk::CHUNK_SIZE) - 1; {
			Chunks[x / MChunk::CHUNK_SIZE][y / MChunk::CHUNK_SIZE][z / MChunk::CHUNK_SIZE]->disableHiddenCubes();
			Chunks[x / MChunk::CHUNK_SIZE][y / MChunk::CHUNK_SIZE][z / MChunk::CHUNK_SIZE]->toVbos();
		}
		
	}

	void deleteCube(int x, int y, int z)
	{
		MCube * cube = getCube(x,y,z);
		cube->setType(MCube::CUBE_AIR);
		cube->setDraw(false);
		cube = getCube(x-1,y,z);
		updateCube(x,y,z);	
	}
			
	void init_world(int seed)
	{
		YLog::log(YLog::USER_INFO,(toString("Creation du monde seed ")+toString(seed)).c_str());
		perlinNoise = new YPerlin();
		srand(seed);
		
		
		perlinNoise->setFreq(0.9f);
		perlinNoise->updateVecs();

		//Reset du monde
		for(int x=0;x<MAT_SIZE;x++)
			for(int y=0;y<MAT_SIZE;y++)
				for(int z=0;z<MAT_HEIGHT;z++)
					Chunks[x][y][z]->reset();

		//G�n�rer ici le monde en modifiant les cubes
		//Utiliser getCubes() 
		for (int x = 0; x < MAT_SIZE_CUBES; x++)
		{
			for (int y = 0; y < MAT_SIZE_CUBES; y++)
			{
				double nx = x / (MAT_SIZE_CUBES / 3.0f), ny = y / (MAT_SIZE_CUBES / 3.0f);

				//HeightMap Generation
				float e = 1 * perlin.noise(1 * nx, 1 * ny, 1) + 0.5 * perlin.noise(2 * nx, 2 * ny, 1) + 0.25 * perlin.noise(4 * nx, 4 * ny, 1);
				e = pow(e, 3.0f);
				e = round(e * 10) / 10;
				heightMap[x][y] = int(max((min(e, 1) * MAT_HEIGHT_CUBES), 0)); //Heightmap Values are between 0 and MAT_HEIGHT_CUBES
			}
		}

		for (int x = 0; x < MAT_SIZE_CUBES; x++) {
			for (int y = 0; y < MAT_SIZE_CUBES; y++) {
				for (int z = 0; z < MAT_HEIGHT_CUBES; z++) {

					MCube* cube = getCube(x, y, z);
					MCube::MCubeType cubeType = MCube::CUBE_AIR;

					//REGIONS

					if (z <= heightMap[x][y])
					{
						if(heightMap[x][y] <= level5) cubeType = MCube::CUBE_PIERRE;
						if(heightMap[x][y] < level4) cubeType = MCube::CUBE_DALLES_01;
						if(heightMap[x][y] < level3) cubeType = MCube::CUBE_HERBE;
						if (heightMap[x][y] < level2) cubeType = MCube::CUBE_LAINE_06;
						if (heightMap[x][y] < level1) cubeType = MCube::CUBE_TERRE;
					}
					if (z < waterLevel) cubeType = MCube::CUBE_EAU;

					cube->setType(cubeType);
				}
			}
		}

		for(int x=0;x<MAT_SIZE;x++)
			for(int y=0;y<MAT_SIZE;y++)
				for(int z=0;z<MAT_HEIGHT;z++)
					Chunks[x][y][z]->disableHiddenCubes();

		add_world_to_vbo();
	}

	void add_world_to_vbo(void)
	{
		for (int x = 0; x<MAT_SIZE; x++)
			for (int y = 0; y<MAT_SIZE; y++)
				for (int z = 0; z<MAT_HEIGHT; z++)
				{
					Chunks[x][y][z]->toVbos();
				}
	}
	
	//Boites de collisions plus petites que deux cubes
	MAxis getMinCol(YVec3f pos, YVec3f dir, float width, float height, float & valueColMin, bool oneShot)
	{
		int x = (int)(pos.X / MCube::CUBE_SIZE);
		int y = (int)(pos.Y / MCube::CUBE_SIZE);
		int z = (int)(pos.Z / MCube::CUBE_SIZE);

		int xNext = (int)((pos.X + width / 2.0f) / MCube::CUBE_SIZE);
		int yNext = (int)((pos.Y + width / 2.0f) / MCube::CUBE_SIZE);
		int zNext = (int)((pos.Z + height / 2.0f) / MCube::CUBE_SIZE);

		int xPrev = (int)((pos.X - width / 2.0f) / MCube::CUBE_SIZE);
		int yPrev = (int)((pos.Y - width / 2.0f) / MCube::CUBE_SIZE);
		int zPrev = (int)((pos.Z - height / 2.0f) / MCube::CUBE_SIZE);

		if (x < 0)	x = 0;
		if (y < 0)	y = 0;
		if (z < 0)	z = 0;

		if (xPrev < 0)	xPrev = 0;
		if (yPrev < 0)	yPrev = 0;
		if (zPrev < 0)	zPrev = 0;

		if (xNext < 0)	xNext = 0;
		if (yNext < 0)	yNext = 0;
		if (zNext < 0)	zNext = 0;

		if (x >= MAT_SIZE_CUBES)	x = MAT_SIZE_CUBES - 1;
		if (y >= MAT_SIZE_CUBES)	y = MAT_SIZE_CUBES - 1;
		if (z >= MAT_HEIGHT_CUBES)	z = MAT_HEIGHT_CUBES - 1;

		if (xPrev >= MAT_SIZE_CUBES)	xPrev = MAT_SIZE_CUBES - 1;
		if (yPrev >= MAT_SIZE_CUBES)	yPrev = MAT_SIZE_CUBES - 1;
		if (zPrev >= MAT_HEIGHT_CUBES)	zPrev = MAT_HEIGHT_CUBES - 1;

		if (xNext >= MAT_SIZE_CUBES)	xNext = MAT_SIZE_CUBES - 1;
		if (yNext >= MAT_SIZE_CUBES)	yNext = MAT_SIZE_CUBES - 1;
		if (zNext >= MAT_HEIGHT_CUBES)	zNext = MAT_HEIGHT_CUBES - 1;

		//On fait chaque axe
		MAxis axis = 0x00;
		valueColMin = oneShot ? 0.5f : 10000.0f;
		float seuil = 0.0000001f;
		float prodScalMin = 1.0f;
		if (dir.getSqrSize() > 1)
			dir.normalize();

		//On verif tout les 4 angles de gauche
		if (getCube(xPrev, yPrev, zPrev)->isSolid() ||
			getCube(xPrev, yPrev, zNext)->isSolid() ||
			getCube(xPrev, yNext, zPrev)->isSolid() ||
			getCube(xPrev, yNext, zNext)->isSolid())
		{
			//On verif que resoudre cette collision est utile
			if (!(getCube(xPrev + 1, yPrev, zPrev)->isSolid() ||
				getCube(xPrev + 1, yPrev, zNext)->isSolid() ||
				getCube(xPrev + 1, yNext, zPrev)->isSolid() ||
				getCube(xPrev + 1, yNext, zNext)->isSolid()) || !oneShot)
			{
				float depassement = ((xPrev + 1) * MCube::CUBE_SIZE) - (pos.X - width / 2.0f);
				float prodScal = abs(dir.X);
				if (abs(depassement) > seuil)
					if (abs(depassement) < abs(valueColMin))
					{
						prodScalMin = prodScal;
						valueColMin = depassement;
						axis = AXIS_X;
					}
			}
		}

		//float depassementx2 = (xNext * NYCube::CUBE_SIZE) - (pos.X + width / 2.0f);

		//On verif tout les 4 angles de droite
		if (getCube(xNext, yPrev, zPrev)->isSolid() ||
			getCube(xNext, yPrev, zNext)->isSolid() ||
			getCube(xNext, yNext, zPrev)->isSolid() ||
			getCube(xNext, yNext, zNext)->isSolid())
		{
			//On verif que resoudre cette collision est utile
			if (!(getCube(xNext - 1, yPrev, zPrev)->isSolid() ||
				getCube(xNext - 1, yPrev, zNext)->isSolid() ||
				getCube(xNext - 1, yNext, zPrev)->isSolid() ||
				getCube(xNext - 1, yNext, zNext)->isSolid()) || !oneShot)
			{
				float depassement = (xNext * MCube::CUBE_SIZE) - (pos.X + width / 2.0f);
				float prodScal = abs(dir.X);
				if (abs(depassement) > seuil)
					if (abs(depassement) < abs(valueColMin))
					{
						prodScalMin = prodScal;
						valueColMin = depassement;
						axis = AXIS_X;
					}
			}
		}

		//float depassementy1 = (yNext * NYCube::CUBE_SIZE) - (pos.Y + width / 2.0f);

		//On verif tout les 4 angles de devant
		if (getCube(xPrev, yNext, zPrev)->isSolid() ||
			getCube(xPrev, yNext, zNext)->isSolid() ||
			getCube(xNext, yNext, zPrev)->isSolid() ||
			getCube(xNext, yNext, zNext)->isSolid())
		{
			//On verif que resoudre cette collision est utile
			if (!(getCube(xPrev, yNext - 1, zPrev)->isSolid() ||
				getCube(xPrev, yNext - 1, zNext)->isSolid() ||
				getCube(xNext, yNext - 1, zPrev)->isSolid() ||
				getCube(xNext, yNext - 1, zNext)->isSolid()) || !oneShot)
			{
				float depassement = (yNext * MCube::CUBE_SIZE) - (pos.Y + width / 2.0f);
				float prodScal = abs(dir.Y);
				if (abs(depassement) > seuil)
					if (abs(depassement) < abs(valueColMin))
					{
						prodScalMin = prodScal;
						valueColMin = depassement;
						axis = AXIS_Y;
					}
			}
		}

		//float depassementy2 = ((yPrev + 1) * NYCube::CUBE_SIZE) - (pos.Y - width / 2.0f);

		//On verif tout les 4 angles de derriere
		if (getCube(xPrev, yPrev, zPrev)->isSolid() ||
			getCube(xPrev, yPrev, zNext)->isSolid() ||
			getCube(xNext, yPrev, zPrev)->isSolid() ||
			getCube(xNext, yPrev, zNext)->isSolid())
		{
			//On verif que resoudre cette collision est utile
			if (!(getCube(xPrev, yPrev + 1, zPrev)->isSolid() ||
				getCube(xPrev, yPrev + 1, zNext)->isSolid() ||
				getCube(xNext, yPrev + 1, zPrev)->isSolid() ||
				getCube(xNext, yPrev + 1, zNext)->isSolid()) || !oneShot)
			{
				float depassement = ((yPrev + 1) * MCube::CUBE_SIZE) - (pos.Y - width / 2.0f);
				float prodScal = abs(dir.Y);
				if (abs(depassement) > seuil)
					if (abs(depassement) < abs(valueColMin))
					{
						prodScalMin = prodScal;
						valueColMin = depassement;
						axis = AXIS_Y;
					}
			}
		}

		//On verif tout les 4 angles du haut
		if (getCube(xPrev, yPrev, zNext)->isSolid() ||
			getCube(xPrev, yNext, zNext)->isSolid() ||
			getCube(xNext, yPrev, zNext)->isSolid() ||
			getCube(xNext, yNext, zNext)->isSolid())
		{
			//On verif que resoudre cette collision est utile
			if (!(getCube(xPrev, yPrev, zNext - 1)->isSolid() ||
				getCube(xPrev, yNext, zNext - 1)->isSolid() ||
				getCube(xNext, yPrev, zNext - 1)->isSolid() ||
				getCube(xNext, yNext, zNext - 1)->isSolid()) || !oneShot)
			{
				float depassement = (zNext * MCube::CUBE_SIZE) - (pos.Z + height / 2.0f);
				float prodScal = abs(dir.Z);
				if (abs(depassement) > seuil)
					if (abs(depassement) < abs(valueColMin))
					{
						prodScalMin = prodScal;
						valueColMin = depassement;
						axis = AXIS_Z;
					}
			}
		}

		//On verif tout les 4 angles du bas
		if (getCube(xPrev, yPrev, zPrev)->isSolid() ||
			getCube(xPrev, yNext, zPrev)->isSolid() ||
			getCube(xNext, yPrev, zPrev)->isSolid() ||
			getCube(xNext, yNext, zPrev)->isSolid())
		{
			//On verif que resoudre cette collision est utile
			if (!(getCube(xPrev, yPrev, zPrev + 1)->isSolid() ||
				getCube(xPrev, yNext, zPrev + 1)->isSolid() ||
				getCube(xNext, yPrev, zPrev + 1)->isSolid() ||
				getCube(xNext, yNext, zPrev + 1)->isSolid()) || !oneShot)
			{
				float depassement = ((zPrev + 1) * MCube::CUBE_SIZE) - (pos.Z - height / 2.0f);
				float prodScal = abs(dir.Z);
				if (abs(depassement) > seuil)
					if (abs(depassement) < abs(valueColMin))
					{
						prodScalMin = prodScal;
						valueColMin = depassement;
						axis = AXIS_Z;
					}
			}
		}

		return axis;
	}
		
	void render_world_basic(GLuint shader, YVbo * vboCube) 
	{
		for (int x = 0; x < MAT_SIZE_CUBES; x++) {
			for (int y = 0; y < MAT_SIZE_CUBES; y++) {
				for (int z = 0; z < MAT_HEIGHT_CUBES; z++) {

					MCube *cube = getCube(x, y, z);

					if (cube->getType() == MCube::CUBE_TERRE) {

						cube->setDraw(true);

						glPushMatrix();
						glUseProgram(shader);
						glTranslatef(x * MCube::CUBE_SIZE / 2.0f, y * MCube::CUBE_SIZE / 2.0f, z * MCube::CUBE_SIZE / 2.0f);
						YRenderer::getInstance()->updateMatricesFromOgl();
						YRenderer::getInstance()->sendMatricesToShader(shader);
						GLuint var = glGetUniformLocation(shader, "cube_color");
						glUniform4f(var, 40.0f / 255.0f, 25.0f / 255.0f, 0.0f, 1.0f);
						
						vboCube->render();
						glPopMatrix();
					}
				}
			}
		}

	}

	void render_world_vbo(bool debug,bool doTransparent)
	{
		glDisable(GL_BLEND);
		//Dessiner les chunks opaques

		for (int x = 0; x < MAT_SIZE; x++)
			for (int y = 0; y < MAT_SIZE; y++)
				for (int z = 0; z < MAT_HEIGHT; z++)
				{	
					glPushMatrix();
					glTranslatef(x * MChunk::CHUNK_SIZE * MCube::CUBE_SIZE, y * MChunk::CHUNK_SIZE * MCube::CUBE_SIZE, z * MChunk::CHUNK_SIZE * MCube::CUBE_SIZE);
					
					YRenderer::getInstance()->updateMatricesFromOgl();
					YRenderer::getInstance()->sendMatricesToShader(YRenderer::CURRENT_SHADER);
					Chunks[x][y][z]->render(false);
					glPopMatrix();
				}
				
		glEnable(GL_BLEND);
		//Dessiner les chunks transparents
		for (int x = 0; x < MAT_SIZE; x++)
			for (int y = 0; y < MAT_SIZE; y++)
				for (int z = 0; z < MAT_HEIGHT; z++)
				{
					glPushMatrix();
					glTranslatef(x * MChunk::CHUNK_SIZE * MCube::CUBE_SIZE, y * MChunk::CHUNK_SIZE * MCube::CUBE_SIZE, z * MChunk::CHUNK_SIZE * MCube::CUBE_SIZE);

					YRenderer::getInstance()->updateMatricesFromOgl();
					YRenderer::getInstance()->sendMatricesToShader(YRenderer::CURRENT_SHADER);
					Chunks[x][y][z]->render(true);
					glPopMatrix();
				}
	}

	/**
	* Attention ce code n'est pas optimal, il est compr�hensible. Il existe de nombreuses
	* versions optimis�es de ce calcul.
	*/
	inline bool intersecDroitePlan(const YVec3f & debSegment, const  YVec3f & finSegment,
		const YVec3f & p1Plan, const YVec3f & p2Plan, const YVec3f & p3Plan,
		YVec3f & inter)
	{
		
		return true;
	}

	/**
	* Attention ce code n'est pas optimal, il est compr�hensible. Il existe de nombreuses
	* versions optimis�es de ce calcul. Il faut donner les points dans l'ordre (CW ou CCW)
	*/
	inline bool intersecDroiteCubeFace(const YVec3f & debSegment, const YVec3f & finSegment,
		const YVec3f & p1, const YVec3f & p2, const YVec3f & p3, const  YVec3f & p4,
		YVec3f & inter)
	{
		
		return false;
	}

	bool getRayCollision(const YVec3f & debSegment, const YVec3f & finSegment,
		YVec3f & inter,
		int &xCube, int&yCube, int&zCube)
	{
		
		return false;
	}

	/**
	* De meme cette fonction peut �tre grandement opitimis�e, on a priviligi� la clart�
	*/
	bool getRayCollisionWithCube(const YVec3f & debSegment, const YVec3f & finSegment,
		int x, int y, int z,
		YVec3f & inter)
	{

		return true;
	}
};



#endif