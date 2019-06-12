#ifndef __YOCTO__ENGINE_TEST__
#define __YOCTO__ENGINE_TEST__

#include "engine/engine.h"

#include "avatar.h"
#include "world.h"

class MEngineMinicraft : public YEngine {

	YFbo * FboPostProcess;

	GLuint ShaderCubeDebug;
	GLuint ShaderSun;
	GLuint ShaderCube;
	GLuint ShaderWorld;
	GLuint ShaderPostProcess;

	YVbo * VboMinecraft;
	YColor SunColor;
	YColor SkyColor;
	YVec3f SunPosition;
	YVec3f SunDirection;

	float _boost = 0;

	YVbo * VboCube;
	YVbo * VboSun;

	bool isSubjective;
	float zoomSpeed = 5.0f;

	MWorld * World;
	MAvatar * Avatar;

	float WaterHeight;
	#define NB_SLIDERS 10
	GUISlider * Sliders[NB_SLIDERS];

	//Texture
	YTexFile * tex;

public :
	//Gestion singleton
	static YEngine * getInstance()
	{
		if (Instance == NULL)
			Instance = new MEngineMinicraft();
		return Instance;
	}

	/*HANDLERS GENERAUX*/
	void loadShaders() {
		ShaderCubeDebug = Renderer->createProgram("shaders/cube_debug");
		ShaderSun = Renderer->createProgram("shaders/sun");
		ShaderCube = Renderer->createProgram("shaders/cube");
		ShaderWorld = Renderer->createProgram("shaders/world");
		ShaderPostProcess = Renderer->createProgram("shaders/postprocess");
	}

	void init() 
	{
		YLog::log(YLog::ENGINE_INFO,"Minicraft Started : initialisation");

		Renderer->setBackgroundColor(YColor(0.0f,0.0f,0.6f,1.0f));
		Renderer->Camera->setPosition(YVec3f(10, 10, 10));
		Renderer->Camera->setLookAt(YVec3f());

		#pragma region Creation VBO
		//Creation du VBO
		VboCube = new YVbo(3, 36, YVbo::PACK_BY_ELEMENT_TYPE);

		//Définition du contenu du VBO
		VboCube->setElementDescription(0, YVbo::Element(3)); //Sommet
		VboCube->setElementDescription(1, YVbo::Element(3)); //Normale
		VboCube->setElementDescription(2, YVbo::Element(2)); //UV
		VboCube->createVboCpu();

		fillVBOCube(VboCube, 1);

		//On envoie le contenu au GPU
		VboCube->createVboGpu();

		//On relache la mémoire CPU
		VboCube->deleteVboCpu();
		#pragma endregion

		#pragma region VBO Sun
		//Creation du VBO Sun
		VboSun = new YVbo(3, 36, YVbo::PACK_BY_ELEMENT_TYPE);

		//Définition du contenu du VBO
		VboSun->setElementDescription(0, YVbo::Element(3)); //Sommet
		VboSun->setElementDescription(1, YVbo::Element(3)); //Normale
		VboSun->setElementDescription(2, YVbo::Element(2)); //UV
		VboSun->createVboCpu();

		fillVBOCube(VboSun, 1);

		VboSun->createVboGpu();
		VboSun->deleteVboCpu();
		#pragma endregion

		#pragma region VBO Minecraft
		//Creation du VBO Sun
		VboMinecraft = new YVbo(3, 36, YVbo::PACK_BY_ELEMENT_TYPE);

		//Définition du contenu du VBO
		VboMinecraft->setElementDescription(0, YVbo::Element(3)); //Sommet
		VboMinecraft->setElementDescription(1, YVbo::Element(3)); //Normale
		VboMinecraft->setElementDescription(2, YVbo::Element(2)); //UV
		VboMinecraft->createVboCpu();

		fillVBOCube(VboMinecraft, 1);

		VboMinecraft->createVboGpu();
		VboMinecraft->deleteVboCpu();
		#pragma endregion

		World = new MWorld();
		World->init_world(10);

		Avatar = new MAvatar(Renderer->Camera, World);

		for (int i = 0; i < NB_SLIDERS; i++) {
			GUILabel * label = new GUILabel();
			Sliders[i] = new GUISlider();
			Sliders[i]->setPos(10, 20 * i + 50);
			addToScreenParam(Sliders[i]);
		}

		//Textures
		tex = YTexManager::getInstance()->loadTexture("textures/TexCustom_0.png");

		FboPostProcess = new YFbo();
	}

	void fillVBOCube(YVbo * vbo, float size = 5.0f)
	{
		int iVertice = 0;

		YVec3f a(size / 2.0f, -size / 2.0f, -size / 2.0f);
		YVec3f b(size / 2.0f, size / 2.0f, -size / 2.0f);
		YVec3f c(size / 2.0f, size / 2.0f, size / 2.0f);
		YVec3f d(size / 2.0f, -size / 2.0f, size / 2.0f);
		YVec3f e(-size / 2.0f, -size / 2.0f, -size / 2.0f);
		YVec3f f(-size / 2.0f, size / 2.0f, -size / 2.0f);
		YVec3f g(-size / 2.0f, size / 2.0f, size / 2.0f);
		YVec3f h(-size / 2.0f, -size / 2.0f, size / 2.0f);

		iVertice += addQuadToVbo(vbo, iVertice, a, b, c, d); //x+
		iVertice += addQuadToVbo(vbo, iVertice, f, e, h, g); //x-
		iVertice += addQuadToVbo(vbo, iVertice, b, f, g, c); //y+
		iVertice += addQuadToVbo(vbo, iVertice, e, a, d, h); //y-
		iVertice += addQuadToVbo(vbo, iVertice, c, g, h, d); //z+
		iVertice += addQuadToVbo(vbo, iVertice, e, f, b, a); //z-
	}

	int addQuadToVbo(YVbo * vbo, int iVertice, YVec3f & a, YVec3f & b, YVec3f & c, YVec3f & d)
	{
		YVec3f normal = (b - a).cross(d - a);
		normal.normalize();

		vbo->setElementValue(0, iVertice, a.X, a.Y, a.Z);
		vbo->setElementValue(1, iVertice, normal.X, normal.Y, normal.Z);
		vbo->setElementValue(2, iVertice, 0, 0);

		iVertice++;

		vbo->setElementValue(0, iVertice, b.X, b.Y, b.Z);
		vbo->setElementValue(1, iVertice, normal.X, normal.Y, normal.Z);
		vbo->setElementValue(2, iVertice, 1, 0);

		iVertice++;

		vbo->setElementValue(0, iVertice, c.X, c.Y, c.Z);
		vbo->setElementValue(1, iVertice, normal.X, normal.Y, normal.Z);
		vbo->setElementValue(2, iVertice, 1, 1);

		iVertice++;

		vbo->setElementValue(0, iVertice, a.X, a.Y, a.Z);
		vbo->setElementValue(1, iVertice, normal.X, normal.Y, normal.Z);
		vbo->setElementValue(2, iVertice, 0, 0);

		iVertice++;

		vbo->setElementValue(0, iVertice, c.X, c.Y, c.Z);
		vbo->setElementValue(1, iVertice, normal.X, normal.Y, normal.Z);
		vbo->setElementValue(2, iVertice, 1, 1);

		iVertice++;

		vbo->setElementValue(0, iVertice, d.X, d.Y, d.Z);
		vbo->setElementValue(1, iVertice, normal.X, normal.Y, normal.Z);
		vbo->setElementValue(2, iVertice, 0, 1);

		iVertice++;

		return 6;
	}

	void update(float elapsed) 
	{
		_boost += 0.05;
		updateLights(_boost);
		Avatar->update(elapsed);

		//Renderer->Camera->update(elapsed);
	}

	void renderObjects() 
	{
		glUseProgram(0);

		FboPostProcess->setAsOutFBO(true);

		#pragma region Axes
		//Rendu des axes
		glDisable(GL_LIGHTING);
		glBegin(GL_LINES);
		//AXE ROUGE
		glColor3d(1, 0, 0);
		glVertex3d(0, 0, 0);
		glVertex3d(10000, 0, 0);
		//AXE VERT
		glColor3d(0, 1, 0);
		glVertex3d(0, 0, 0);
		glVertex3d(0, 10000, 0);
		//AXE BLEU
		glColor3d(0, 0, 1);
		glVertex3d(0, 0, 0);
		glVertex3d(0, 0, 10000);

		glEnd();
		#pragma endregion

		//glRotatef(this->DeltaTimeCumul / 10.0f * 360, -1, 1, 0);
		glDisable(GL_CULL_FACE);

		#pragma region Cube
		//Exemple d'utilisation d'un shader
		glPushMatrix();
		glUseProgram(ShaderCubeDebug); //Demande au GPU de charger ces shaders
		Renderer->updateMatricesFromOgl(); //Calcule toute les matrices à partir des deux matrices OGL
		Renderer->sendMatricesToShader(ShaderCubeDebug); //Envoie les matrices au shader
		VboCube->render(); //Demande le rendu du VBO
		glPopMatrix();
		#pragma endregion

		#pragma region Avatar
		glUseProgram(ShaderCube);
		GLuint var = glGetUniformLocation(ShaderCube, "cube_color");
		glUniform4f(var, 1, 0, 0, 1);
		Avatar->render(VboCube);
		#pragma endregion

		#pragma region Soleil
		glPushMatrix();
		glUseProgram(ShaderSun);
		var = glGetUniformLocation(ShaderSun, "sun_color");
		glUniform3f(var, SunColor.R, SunColor.V, SunColor.B);
		glTranslatef(SunPosition.X, SunPosition.Y, SunPosition.Z);
		glScalef(20, 20, 20);
		Renderer->updateMatricesFromOgl();
		Renderer->sendMatricesToShader(ShaderSun);
		VboSun->render();
		glPopMatrix();
		#pragma endregion

		#pragma region  World

		glPushMatrix();
		//glUseProgram(ShaderCube);
		//World->render_world_basic(ShaderCube, VboMinecraft);

		glUseProgram(ShaderWorld);
		// Send all necessary data to shader
		var = glGetUniformLocation(ShaderWorld, "world_size");
		glUniform1f(var, MWorld::MAT_SIZE_CUBES);


		var = glGetUniformLocation(ShaderWorld, "lightDir");
		glUniform3f(var, SunPosition.X, SunPosition.Y, SunPosition.Z);

		var = glGetUniformLocation(ShaderWorld, "sunPos");
		glUniform3f(var, SunPosition.X, SunPosition.Y, SunPosition.Z);

		var = glGetUniformLocation(ShaderWorld, "skyColor");
		glUniform3f(var, SkyColor.R, SkyColor.V, SkyColor.B);

		var = glGetUniformLocation(ShaderWorld, "camPos");
		glUniform3f(var, YRenderer::getInstance()->Camera->Position.X, YRenderer::getInstance()->Camera->Position.Y, YRenderer::getInstance()->Camera->Position.Z);

		var = glGetUniformLocation(ShaderWorld, "elapsed");
		glUniform1f(var, this->DeltaTimeCumul);

		// Update with sliders
		for (int i = 0; i < NB_SLIDERS; i++)
		{
			string varName = "slider_" + toString(i);
			var = glGetUniformLocation(ShaderWorld, varName.c_str());
			glUniform1f(var, Sliders[i]->Value);
		}

		tex->setAsShaderInput(ShaderWorld, GL_TEXTURE4, "colTex");
		World->render_world_vbo(false, true);

		glPopMatrix();
		#pragma endregion

		#pragma region Post-Process
		FboPostProcess->setAsOutFBO(false);
		

		glUseProgram(ShaderPostProcess);
		FboPostProcess->setColorAsShaderInput(0, GL_TEXTURE0, "TextColor");
		FboPostProcess->setDepthAsShaderInput(GL_TEXTURE1, "TextDepth");
		

		Renderer->sendNearFarToShader(ShaderPostProcess);
		Renderer->sendScreenSizeToShader(ShaderPostProcess);
		Renderer->sendMatricesToShader(ShaderPostProcess);
		Renderer->drawFullScreenQuad();

		#pragma endregion
	}

	bool getSunDirFromDayTime(YVec3f & sunDir, float mnLever, float mnCoucher, float boostTime)
	{
		bool nuit = false;

		SYSTEMTIME t;
		GetLocalTime(&t);

		//On borne le tweak time à une journée (cyclique)
		while (boostTime > 24 * 60)
			boostTime -= 24 * 60;

		//Temps écoulé depuis le début de la journée
		float fTime = (float)(t.wHour * 60 + t.wMinute);
		fTime += boostTime;
		while (fTime > 24 * 60)
			fTime -= 24 * 60;

		//Si c'est la nuit
		if (fTime < mnLever || fTime > mnCoucher)
		{
			nuit = true;
			if (fTime < mnLever)
				fTime += 24 * 60;
			fTime -= mnCoucher;
			fTime /= (mnLever + 24 * 60 - mnCoucher);
			fTime *= (float)M_PI;
		}
		else
		{
			//c'est le jour
			nuit = false;
			fTime -= mnLever;
			fTime /= (mnCoucher - mnLever);
			fTime *= (float)M_PI;
		}

		//Direction du soleil en fonction de l'heure
		sunDir.X = cos(fTime);
		sunDir.Y = 0.2f;
		sunDir.Z = sin(fTime);
		sunDir.normalize();

		return nuit;
	}

	void updateLights(float boostTime = 0)
	{
		//On recup la direciton du soleil
		bool nuit = getSunDirFromDayTime(SunDirection, 6.0f * 60.0f, 19.0f * 60.0f, boostTime);
		SunPosition = SunDirection * 300.0f;
		//SunPosition = Renderer->Camera->Position + SunDirection * 150.0f;
		//SunPosition = (Renderer->Camera->Position + SunDirection) /1.25;
		//Pendant la journée
		if (!nuit)
		{
			//On definit la couleur
			SunColor = YColor(1.0f, 1.0f, 0.8f, 1.0f);
			SkyColor = YColor(0.0f, 181.f / 255.f, 221.f / 255.f, 1.0f);
			YColor downColor(0.9f, 0.5f, 0.1f, 1);

			SunColor = SunColor.interpolate(downColor, (abs(SunDirection.X)));
			SkyColor = SkyColor.interpolate(downColor, (abs(SunDirection.X)));
		}
		else
		{
			//La nuit : lune blanche et ciel noir
			SunColor = YColor(1, 1, 1, 1);
			SkyColor = YColor(0, 0, 0, 1);
		}

		Renderer->setBackgroundColor(SkyColor);
	}

	void resize(int width, int height) {
		FboPostProcess->resize(width, height);
	}

	/*INPUTS*/

	void keyPressed(int key, bool special, bool down, int p1, int p2) 
	{	
		if ((key == 103 || key == 107) && down) _boost += 60;
		if (key == 'Z' || key == 122) 
		{
			if (Renderer->Camera->ControllerType == Renderer->Camera->OBSERVER_CONTROLLER)
			{ 
				Renderer->Camera->move(Renderer->Camera->Direction);
			}
			
			else Avatar->avance = down;
		}

		if ((key == 81 || key == 113)) 
		{
			if (Renderer->Camera->ControllerType == Renderer->Camera->OBSERVER_CONTROLLER)
			{
				Renderer->Camera->move(-Renderer->Camera->RightVec);
			}
			
			else Avatar->gauche = down;
		}

		if ((key == 83 || key == 115)) 
		{
			if (Renderer->Camera->ControllerType == Renderer->Camera->OBSERVER_CONTROLLER)
			{
				Renderer->Camera->move(-Renderer->Camera->Direction);
			}

			else Avatar->recule = down;
		}

		if ((key == 68 || key == 100)) 
		{
			if (Renderer->Camera->ControllerType == Renderer->Camera->OBSERVER_CONTROLLER)
			{
				Renderer->Camera->move(Renderer->Camera->RightVec);
			}

			else Avatar->droite = down;
		}

		if (key == 32) {
			if (Renderer->Camera->ControllerType != Renderer->Camera->OBSERVER_CONTROLLER)
				Avatar->Jump = down;
		}

		if ((key == 65 || key == 97) && down)
		{
			Renderer->Camera->UpdateController();
		}
	}

	void mouseWheel(int wheel, int dir, int x, int y, bool inUi)
	{
		if (Renderer->Camera->ControllerType == Renderer->Camera->OBSERVER_CONTROLLER)
		{
			Renderer->Camera->move(Renderer->Camera->Direction * zoomSpeed * dir);
		}

		else
		{
			Avatar->updateZoom(zoomSpeed * dir);
		}
	}

	void mouseClick(int button, int state, int x, int y, bool inUi)
	{

	}

	void mouseMove(int x, int y, bool pressed, bool inUi)
	{
		static int lastx = -1;
		static int lasty = -1;

		//if (Renderer->Camera->ControllerType != Renderer->Camera->OBSERVER_CONTROLLER) return;

		if (!pressed)
		{
			lastx = x;
			lasty = y;
			showMouse(true);
		}
		else
		{
			if (lastx == -1 && lasty == -1)
			{
				lastx = x;
				lasty = y;
			}

			int dx = x - lastx;
			int dy = y - lasty;

			if (dx == 0 && dy == 0)
				return;

			lastx = x;
			lasty = y;

			//if right click
			if (MouseBtnState & GUI_MRBUTTON)
			{
				showMouse(false);
				//If Left Control clicked
				if (GetKeyState(VK_LCONTROL) & 0x80)
				{
					Renderer->Camera->rotateAround((float)-dx / 300.0f);
					Renderer->Camera->rotateUpAround((float)-dy / 300.0f);
					glutWarpPointer(Renderer->ScreenWidth / 2, Renderer->ScreenHeight / 2);
					lastx = Renderer->ScreenWidth / 2;
					lasty = Renderer->ScreenHeight / 2;
				}
				//Subjective view
				else {
					showMouse(false);
					Renderer->Camera->rotate((float)-dx / 300.0f);
					Renderer->Camera->rotateUp((float)-dy / 300.0f);
					glutWarpPointer(Renderer->ScreenWidth / 2, Renderer->ScreenHeight / 2);
					lastx = Renderer->ScreenWidth / 2;
					lasty = Renderer->ScreenHeight / 2;
				}
			}
			//If molette click
			if (MouseBtnState & GUI_MMBUTTON)
			{
				showMouse(false);
				//If Left Control clicked
				if (GetKeyState(VK_LCONTROL) & 0x80)
				{
					YVec3f strafe = Renderer->Camera->RightVec;
					strafe.Z = 0;
					strafe.normalize();
					strafe *= (float)-dx / 2.0f;

					YVec3f avance = Renderer->Camera->Direction;
					avance.Z = 0;
					avance.normalize();
					avance *= (float)dy / 2.0f;

					Renderer->Camera->move(avance + strafe);
				}
				else {
					YVec3f strafe = Renderer->Camera->RightVec;
					strafe.Z = 0;
					strafe.normalize();
					strafe *= (float)-dx / 5.0f;

					Renderer->Camera->move(Renderer->Camera->UpRef * (float)dy / 5.0f);
					Renderer->Camera->move(strafe);
					glutWarpPointer(Renderer->ScreenWidth / 2, Renderer->ScreenHeight / 2);
					lastx = Renderer->ScreenWidth / 2;
					lasty = Renderer->ScreenHeight / 2;
				}
			}
		}
	}
	
};


#endif