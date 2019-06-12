#pragma once

class YCamera
{
public:

	typedef enum {
		PROJECTION_ORTHO,
		PROJECTION_PERSPECTIVE
	} PROJTYPE;

	typedef enum
	{
		FIRST_PERSON_CONTROLLER,
		THIRD_PERSON_CONTROLLER,
		OBSERVER_CONTROLLER
	} CONTROLLER_TYPE;

	YVec3<float> Position; ///< Position de la camera
	YVec3<float> LookAt; ///< Point regarde par la camera
	YVec3<float> Direction; ///< Direction de la camera
	YVec3<float> UpVec; ///< Vecteur up de la camera
	YVec3<float> RightVec; ///< Si on se place dans la camera, indique la droite	
	YVec3<float> UpRef; ///< Ce qu'on consid�re comme le "haut" dans notre monde (et pas le up de la cam)
	int Speed;

	PROJTYPE ProjType; ///< Type de projection
	CONTROLLER_TYPE ControllerType; ///< Type de camera
	float FovY; ///< fov angle sur y in degrees
	float Ratio; ///< with / height
	float Left; ///< Left YPlane
	float Right;
	float Bottom;
	float Top;
	float Near;
	float Far;
			
	YCamera()
	{
		Position = YVec3<float>(0, -1, 0);
		LookAt = YVec3<float>(0, 0, 0);
		UpRef = YVec3<float>(0, 0, 1);
		UpVec = UpRef;
		FovY = 45.0f;
		Ratio = 800.0f / 600.0f;
		Near = 0.1f;
		Far = 800.0f;
		Speed = 0.05;
		ControllerType = CONTROLLER_TYPE::OBSERVER_CONTROLLER;
		setProjectionPerspective(FovY, Ratio, Near, Far);
		updateVecs();
	}

	void setProjectionPerspective(float fovy, float ratio, float nearPlane, float farPlane) {
		ProjType = PROJECTION_PERSPECTIVE;
		FovY = fovy;
		Ratio = ratio;
		Near = nearPlane;
		Far = farPlane;
	}

	void setProjectionOrtho(float left, float right, float bottom, float top, float nearPlane, float farPlane) {
		ProjType = PROJECTION_ORTHO;
		Left = left;
		Right = right;
		Bottom = bottom;
		Top = top;
		Near = nearPlane;
		Far = farPlane;
	}

	void UpdateController()
	{
		switch (ControllerType)
		{
		case CONTROLLER_TYPE::FIRST_PERSON_CONTROLLER:
			ControllerType = CONTROLLER_TYPE::THIRD_PERSON_CONTROLLER;
			break;

		case CONTROLLER_TYPE::THIRD_PERSON_CONTROLLER:
			ControllerType = CONTROLLER_TYPE::OBSERVER_CONTROLLER;
			break;

		case CONTROLLER_TYPE::OBSERVER_CONTROLLER:
			ControllerType = CONTROLLER_TYPE::FIRST_PERSON_CONTROLLER;
			break;

		default:
			break;
		}
	}

	/**
	* Mise a jour de la camera
	*/
	virtual void update(float elapsed)
	{
		look();
	}

	/**
	* Definition du point regarde
	*/
	void setLookAt(YVec3<float> lookat)
	{
		LookAt = lookat;
		updateVecs();
	}

	/**
	* Definition de la position de la camera
	*/
	void setPosition(YVec3<float> pos)
	{
		Position = pos;
		updateVecs();
	}

	/**
	* Definition du haut de notre monde
	*/
	void setUpRef(YVec3<float> upRef)
	{
		UpRef = upRef;
		updateVecs();
	}

	/**
	* Deplacement de la camera d'un delta donn�
	*/
	void move(YVec3<float> delta)
	{
		LookAt += delta;
		Position += delta;
		updateVecs();
	}

	/**
	* Deplacement de la camera � un point donn�
	*/
	void moveTo(const YVec3<float> & target)
	{
		this->move(target - Position);
	}

	/**
	* On recalcule les vecteurs utiles au d�placement de la camera (Direction, RightVec, UpVec)
	* on part du principe que sont connus :
	* - la position de la camera
	* - le point regarde par la camera
	* - la vecteur up de notre monde
	*/
	void updateVecs(void)
	{
		//Direction = YVec3f(LookAt.X - Position.X, LookAt.Y - Position.Y, LookAt.Z - Position.Z);
		Direction = LookAt - Position;
		Direction.normalize();

		RightVec = Direction.cross(UpRef).normalize();
		UpVec = RightVec.cross(Direction);
		UpVec.normalize();
	}

	/**
	* Rotation droite gauche en subjectif
	*/
	void rotate(float angle)
	{
		LookAt -= Position;
		LookAt.rotate(UpRef, angle);
		LookAt += Position;
		updateVecs();
	}

	/**
	* Rotation haut bas en subjectif
	*/
	void rotateUp(float angle)
	{
		YVec3f previousPos = Position;

		LookAt -= Position;
		LookAt.rotate(RightVec, angle);
		LookAt += Position;

		YVec3f normPos = Position;
		normPos.normalize();
		float newAngle = normPos.dot(UpRef);
		if (newAngle > 0.99 || newAngle < -0.99)
			Position = previousPos;

		updateVecs();
	}

	/**
	* Rotation droite gauche en troisi�me personne
	*/
	void rotateAround(float angle)
	{
		Position -= LookAt;
		Position.rotate(UpRef, angle);
		Position += LookAt;
		updateVecs();
	}

	/**
	* Rotation haut bas en troisi�me personne
	*/
	void rotateUpAround(float angle)
	{
		Position -= LookAt;

		//On ne monte pas trop haut pour ne pas passer de l'autre cot�
		YVec3f previousPos = Position;
		Position.rotate(RightVec, angle);
		YVec3f normPos = Position;
		normPos.normalize();
		float newAngle = normPos.dot(UpRef);
		if (newAngle > 0.99 || newAngle < -0.99)
			Position = previousPos;

		Position += LookAt;
		updateVecs();
	}

	/**
	* Calcul du bon rep�re de d�part pour la matrice monde
	*/
	void look(void)
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		if (ProjType == PROJECTION_PERSPECTIVE) {
			gluPerspective(FovY, Ratio, Near, Far);
		}
		if (ProjType == PROJECTION_ORTHO) {
			glOrtho(Left, Right, Bottom, Top, Near, Far);
		}
		
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(Position.X, Position.Y, Position.Z, LookAt.X, LookAt.Y, LookAt.Z, UpVec.X, UpVec.Y, UpVec.Z);
		
	}
};
