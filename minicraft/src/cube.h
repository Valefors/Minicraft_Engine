#pragma once

#include <stdio.h>

class MCube
{
public :
	enum MCubeType
	{
		CUBE_HERBE = 0, 
		CUBE_TERRE, //1
		CUBE_BOIS,  //2
		CUBE_EAU,   //3
		CUBE_PIERRE,//4
		CUBE_VERRE, //5
		CUBE_PLANCHE_01, //6
		CUBE_PLANCHE_02, //7
		CUBE_PLANCHE_03, //8
		CUBE_PLANCHE_04, //9
		CUBE_PLANCHE_05, //10
		CUBE_PLANCHE_06, //11
		CUBE_BRIQUES, //12
		CUBE_DALLES_01, //13
		CUBE_DALLES_02, //14
		CUBE_DALLES_03, //15
		CUBE_DALLES_04, //16
		CUBE_SABLE_01,  //17
		CUBE_SABLE_02,  //18
		CUBE_LAINE_01,  //19
		CUBE_LAINE_02,  //20
		CUBE_LAINE_03,  //21
		CUBE_LAINE_04,  //22
		CUBE_LAINE_05,  //23
		CUBE_LAINE_06,  //24
		CUBE_LAINE_07,  //25
		CUBE_LAINE_08,  //26
		CUBE_LAINE_09,  //27
		CUBE_LAINE_10,  //28
		CUBE_LAINE_11,  //29
		CUBE_LAINE_12,  //30
		CUBE_LAINE_13,  //31
		CUBE_LAINE_14,  //32
		CUBE_LAINE_15,  //33
		CUBE_LAINE_16,  //34

		CUBE_CUSTOM_IMAGE, //35
		CUBE_LIVRE,     //36

		CUBE_TRONC,     //37
		CUBE_BRANCHES,  //38
		CUBE_AIR,       //39

		CUBE_STAIRS,    //40
		CUBE_NB_TYPES
	}; //Limité à 128 types

	static const int CUBE_DRAW_BIT = 0x80;

	private :
		uint8 _Code; //premier bit si on doit le draw ou pas, le reste un des 127 types
		
	public :
		YVec3f _NavDir;
		static const int CUBE_SIZE = 2;
		
		MCube()
		{
			setDraw(false);
			setType(CUBE_AIR); 
		}

		void setType(MCubeType type){
			bool draw = getDraw();
			_Code = (uint8)type;
			setDraw(draw);
		}

		MCubeType getType(void){
			return (MCubeType)(_Code & ~CUBE_DRAW_BIT);
		}

		bool getDraw(){
			return _Code & CUBE_DRAW_BIT ? true : false;
		}

		void setDraw(bool draw){
			if (draw)
				_Code |= CUBE_DRAW_BIT;
			else
				_Code &= ~CUBE_DRAW_BIT;
		}

		bool isSolid(void)
		{
			MCubeType type = getType();
			return (type != CUBE_AIR && type != CUBE_EAU);
		}

		bool isPickable(void)
		{
			MCubeType type = getType();
			return (type != CUBE_AIR);
		} 

		bool isOpaque(void)
		{
			MCubeType type = getType();
			return  (type != CUBE_AIR && type != CUBE_EAU && type != CUBE_VERRE && type != CUBE_BRANCHES);
		}

		bool isTransparent(void)
		{
			MCubeType type = getType();
			return  (type == CUBE_AIR || type == CUBE_EAU || type == CUBE_VERRE);
		}

		bool isCutoff(void)
		{
			MCubeType type = getType();
			return  (type == CUBE_BRANCHES);
		}

		bool isGround(void)
		{
			MCubeType type = getType();
			return (type == CUBE_HERBE || type == CUBE_TERRE || type == CUBE_EAU || type == CUBE_BOIS || type == CUBE_PIERRE);
		}

		bool isNouricier(void)
		{
			MCubeType type = getType();
			return (type == CUBE_HERBE || type == CUBE_TERRE);
		}

		void saveToFile(FILE * fs)
		{
			fputc(_Code,fs);
		}

		void loadFromFile(FILE * fe)
		{
			_Code = fgetc(fe);
		}

		uint8 getRawCode()
		{
			return _Code;
		}

		void setRawCode(uint8 code){
			_Code = code;
		}

		static string getName(MCubeType type)
		{
			switch (type)
			{
			case CUBE_HERBE: return string("CUBE_HERBE"); break;
			case CUBE_TERRE: return string("CUBE_TERRE"); break;
			case CUBE_BOIS: return string("CUBE_BOIS"); break;
			case CUBE_PIERRE: return string("CUBE_PIERRE"); break;
			case CUBE_EAU: return string("CUBE_EAU"); break;
			case CUBE_VERRE: return string("CUBE_VERRE"); break;
			case CUBE_STAIRS: return string("CUBE_STAIRS"); break;				
			case CUBE_PLANCHE_01: return string("CUBE_PLANCHE_01"); break;
			case CUBE_PLANCHE_02: return string("CUBE_PLANCHE_02"); break;
			case CUBE_PLANCHE_03: return string("CUBE_PLANCHE_03"); break;
			case CUBE_PLANCHE_04: return string("CUBE_PLANCHE_04"); break;
			case CUBE_PLANCHE_05: return string("CUBE_PLANCHE_05"); break;
			case CUBE_PLANCHE_06: return string("CUBE_PLANCHE_06"); break;
			case CUBE_BRIQUES: return string("CUBE_BRIQUES"); break;
			case CUBE_DALLES_01: return string("CUBE_DALLES_01"); break;
			case CUBE_DALLES_02: return string("CUBE_DALLES_02"); break;
			case CUBE_DALLES_03: return string("CUBE_DALLES_03"); break;
			case CUBE_DALLES_04: return string("CUBE_DALLES_04"); break;
			case CUBE_SABLE_01: return string("CUBE_SABLE_01"); break;
			case CUBE_SABLE_02: return string("CUBE_SABLE_02"); break;
			case CUBE_LAINE_01: return string("CUBE_LAINE_01"); break;
			case CUBE_LAINE_02: return string("CUBE_LAINE_02"); break;
			case CUBE_LAINE_03: return string("CUBE_LAINE_03"); break;
			case CUBE_LAINE_04: return string("CUBE_LAINE_04"); break;
			case CUBE_LAINE_05: return string("CUBE_LAINE_05"); break;
			case CUBE_LAINE_06: return string("CUBE_LAINE_06"); break;
			case CUBE_LAINE_07: return string("CUBE_LAINE_07"); break;
			case CUBE_LAINE_08: return string("CUBE_LAINE_08"); break;
			case CUBE_LAINE_09: return string("CUBE_LAINE_09"); break;
			case CUBE_LAINE_10: return string("CUBE_LAINE_10"); break;
			case CUBE_LAINE_11: return string("CUBE_LAINE_11"); break;
			case CUBE_LAINE_12: return string("CUBE_LAINE_12"); break;
			case CUBE_LAINE_13: return string("CUBE_LAINE_13"); break;
			case CUBE_LAINE_14: return string("CUBE_LAINE_14"); break;
			case CUBE_LAINE_15: return string("CUBE_LAINE_15"); break;
			case CUBE_LAINE_16: return string("CUBE_LAINE_16"); break;
			case CUBE_CUSTOM_IMAGE: return string("CUBE_CUSTOM_IMAGE"); break;
			case CUBE_LIVRE: return string("CUBE_LIVRE"); break;
			case CUBE_TRONC: return string("CUBE_TRONC"); break;
			case CUBE_BRANCHES: return string("CUBE_BRANCHES"); break;
			case CUBE_AIR: return string("CUBE_AIR"); break;
			}

			return string("INCONNU");
		}

		static bool isManipulable(MCubeType type)
		{
			switch (type)
			{
			case CUBE_HERBE: 
			case CUBE_TERRE: 
			case CUBE_BOIS:
			case CUBE_PIERRE: 
			case CUBE_EAU: 
			case CUBE_VERRE:
			case CUBE_STAIRS:
			case CUBE_PLANCHE_01: 
			case CUBE_PLANCHE_02: 
			case CUBE_PLANCHE_03: 
			case CUBE_PLANCHE_04:
			case CUBE_PLANCHE_05: 
			case CUBE_PLANCHE_06: 
			case CUBE_BRIQUES: 
			case CUBE_DALLES_01: 
			case CUBE_DALLES_02: 
			case CUBE_DALLES_03: 
			case CUBE_DALLES_04: 
			case CUBE_SABLE_01:
			case CUBE_SABLE_02:
			case CUBE_LAINE_01:
			case CUBE_LAINE_02:
			case CUBE_LAINE_03:
			case CUBE_LAINE_04:
			case CUBE_LAINE_05:
			case CUBE_LAINE_06:
			case CUBE_LAINE_07:
			case CUBE_LAINE_08:
			case CUBE_LAINE_09:
			case CUBE_LAINE_10:
			case CUBE_LAINE_11:
			case CUBE_LAINE_12:
			case CUBE_LAINE_13:
			case CUBE_LAINE_14:
			case CUBE_LAINE_15:
			case CUBE_LAINE_16:
			case CUBE_CUSTOM_IMAGE:
			case CUBE_LIVRE: 
			case CUBE_TRONC: 
					return true;
			}

			return false;
		}
};