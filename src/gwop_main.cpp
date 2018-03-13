#include <windows.h>
#include <windowsx.h>
#include <stdarg.h>
#include <glut.h>
#include <glaux.h>
#include <math.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <time.h>
#include <string.h>
#include "SOIL.h"
#include "GWOP_classes.h"
#include "objects.h"

#define NUM_OF_LEVELS 5

const int num_of_textures = 50;

HDC			hDC=NULL;		// Private GDI Device Context
HGLRC		hRC=NULL;		// Permanent Rendering Context
HWND		hWnd=NULL;		// Holds Our Window Handle
HINSTANCE	hInstance;		// Holds The Instance Of The Application
GLuint	base;				// Base Display List For The Font Set

long int time_delta=0;
int t,t1,t2,t3,t4;
float r;
Player player;
Level level;
bool space_hit=false;
bool lmb_hit=false;
bool menu_on=true;
bool escape_hit=false;
bool the_end=false;
bool screen_shot=false;

int menu_state=0;
int menu_select=0;

bool dot_selected[9];

enum{
	MS_NEW=0,
	MS_LOAD=1,
	MS_SAVE=2,
	MS_EXIT=3
}MenuSelect;

enum {
	MN_MAIN=0,
	MN_LOAD=1,
	MN_SAVE=2
} MenuState;

float b_x,b_y,b_z;
float e_x,e_y,e_z;

GLfloat light_position[] = { 0, 1.0, 0, 1.0 };
GLfloat light_position1[] = { 0, 2, 0, 1.0 };
GLfloat light_spotdir[] = {0,-1,0};
GLfloat light_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };

float v[3][3];
float w[3];

void calcNormal(float v[3][3], float out[3])
{
	float v1[3],v2[3];
	static const int x = 0;
	static const int y = 1;
	static const int z = 2;

	// Calculate two vectors from the three points
	v1[x] = v[0][x] - v[1][x];
	v1[y] = v[0][y] - v[1][y];
	v1[z] = v[0][z] - v[1][z];

	v2[x] = v[1][x] - v[2][x];
	v2[y] = v[1][y] - v[2][y];
	v2[z] = v[1][z] - v[2][z];

	// Take the cross product of the two vectors to get
	// the normal vector which will be stored in out[]
	out[x] = -(v1[y]*v2[z] - v1[z]*v2[y]);
	out[y] = -(v1[z]*v2[x] - v1[x]*v2[z]);
	out[z] = -(v1[x]*v2[y] - v1[y]*v2[x]);

	// Normalize the vector (shorten length to one)
	//ReduceToUnit(out);
}

enum {
	TX_WALL=0,
	TX_FLOOR=1,
	TX_CEILING=2,
	TX_BOX=3,
	TX_WINDOW=4,
	TX_GLASS=5,
	TX_MONSTER_1=6,
	TX_METAL=7,
	TX_TELEPORT=8,
	TX_FAN=9,
	TX_MESH=10,
	TX_EXIT=11,
	TX_RED=12,
	TX_GOLD=13,
	TX_YELLOW=14,
	TX_MONSTER_2=15,
	TX_MONSTER_3=16,
	TX_MONSTER_4=17,
	TX_MONSTER_5=18,
	TX_WHITE=19,
	TX_GREEN=20,
	TX_BLUE=21,
	TX_SWITCH_ON=22,
	TX_SWITCH_OFF=23,
	TX_DOOR=24,
	TX_TARGET=25,
	TX_PANEL=26,
	TX_MONSTER_2A=27,
	TX_MONSTER_3A=28,
	TX_MONSTER_4A=29,
	TX_MONSTER_5A=30,
	TX_MONSTER_1A=31,
	TX_MAIN_MENU=32,
	TX_MENU_PANEL=33,
	TX_DOT=34,
	TX_DOT_GLOW=35,
	TX_EMPTY=36,
	TX_END=37
} TexturesID;

enum{
	TXT_SPACE=0,
	TXT_SWITCH=1,
	TXT_RED=2,
	TXT_GREEN=3,
	TXT_BLUE=4,
	TXT_YELLOW=5,
	TXT_WHITE=6,
	TXT_GAME_OVER=7
}TextsID;

GLuint	Faces[20];
GLuint	Texture[num_of_textures];
GLuint  Numbers[10];
GLuint	Keys[5];
GLuint	Texts[8];
GLuint	Marks[4];
GLuint	Thumbs[9];

void glEnable2D()
{
	int vPort[4];

   glGetIntegerv(GL_VIEWPORT, vPort);

   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadIdentity();

   glOrtho(0, vPort[2], 0, vPort[3], -1, 1);
   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
   glLoadIdentity();
}
void glDisable2D()
{
   glMatrixMode(GL_PROJECTION);
   glPopMatrix();   
   glMatrixMode(GL_MODELVIEW);
   glPopMatrix();	
}
void DrawTxt(int txt){
	glDisable(GL_LIGHTING);
	glDisable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glBlendFunc( GL_ONE,GL_ONE_MINUS_SRC_COLOR );
	glEnable2D();
	glBindTexture(GL_TEXTURE_2D,Texts[txt]);
	glBegin(GL_QUADS);
		glTexCoord2f(0,1);
		glVertex2i(GetSystemMetrics(SM_CXSCREEN)/2-201,181);
		glTexCoord2f(0,0);
		glVertex2i(GetSystemMetrics(SM_CXSCREEN)/2-201,200);
		glTexCoord2f(1,0);
		glVertex2i(GetSystemMetrics(SM_CXSCREEN)/2-201+402,200);
		glTexCoord2f(1,1);
		glVertex2i(GetSystemMetrics(SM_CXSCREEN)/2-201+402,181);
	glEnd();
	glDisable2D();
	glDisable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_LIGHTING);
}
void DrawNumber(int val, int x, int y){
	glDisable(GL_LIGHTING);
	glEnable2D();
	int num_of_digits=0;
	int digits[100];
	while (val>0) {
		digits[num_of_digits]=val%10;
		num_of_digits++;
		val=val/10;
	}
	for (int i=num_of_digits; i>0; i--){
		glBindTexture(GL_TEXTURE_2D,Numbers[digits[i-1]]);
		glBegin(GL_QUADS);
			glTexCoord2f(0,1);
			glVertex2i(x+(num_of_digits-i)*7,GetSystemMetrics(SM_CYSCREEN)-(y+8));
			glTexCoord2f(0,0);
			glVertex2i(x+(num_of_digits-i)*7,GetSystemMetrics(SM_CYSCREEN)-y);
			glTexCoord2f(1,0);
			glVertex2i(x+(num_of_digits-i)*7+5,GetSystemMetrics(SM_CYSCREEN)-y);
			glTexCoord2f(1,1);
			glVertex2i(x+(num_of_digits-i)*7+5,GetSystemMetrics(SM_CYSCREEN)-(y+8));
		glEnd();
	}
	glDisable2D();
	glEnable(GL_LIGHTING);
}

void DrawPanel(){
	DrawNumber(player.points,133,31);
	DrawNumber(player.ammo,133,60);

	glDisable(GL_LIGHTING);
	glEnable2D();

	int k=0;
	for (int i=0; i<5; i++){
		if(player.keys[i]) {
			glBindTexture(GL_TEXTURE_2D,Keys[i]);
			glBegin(GL_QUADS);
				glTexCoord2f(0,1);
				glVertex2i(20+83+k*18,GetSystemMetrics(SM_CYSCREEN)-(20+63+7));
				glTexCoord2f(0,0);
				glVertex2i(20+83+k*18,GetSystemMetrics(SM_CYSCREEN)-(20+63));
				glTexCoord2f(1,0);
				glVertex2i(20+83+14+k*18,GetSystemMetrics(SM_CYSCREEN)-(20+63));
				glTexCoord2f(1,1);
				glVertex2i(20+83+14+k*18,GetSystemMetrics(SM_CYSCREEN)-(20+63+7));
			glEnd();
			k++;
		}
	}
	int face = 19-player.health/5;
	glBindTexture(GL_TEXTURE_2D,Faces[face]);
	glBegin(GL_QUADS);
		glTexCoord2f(0,1);
		glVertex2i(27,GetSystemMetrics(SM_CYSCREEN)-(27+70));
		glTexCoord2f(0,0);
		glVertex2i(27,GetSystemMetrics(SM_CYSCREEN)-27);
		glTexCoord2f(1,0);
		glVertex2i(27+70,GetSystemMetrics(SM_CYSCREEN)-27);
		glTexCoord2f(1,1);
		glVertex2i(27+70,GetSystemMetrics(SM_CYSCREEN)-(27+70));
	glEnd();

	glBindTexture(GL_TEXTURE_2D,Texture[TX_PANEL]);
	glBegin(GL_QUADS);
		glTexCoord2f(0,1);
		glVertex2i(20,GetSystemMetrics(SM_CYSCREEN)-(20+85));
		glTexCoord2f(0,0);
		glVertex2i(20,GetSystemMetrics(SM_CYSCREEN)-20);
		glTexCoord2f(1,0);
		glVertex2i(20+182,GetSystemMetrics(SM_CYSCREEN)-20);
		glTexCoord2f(1,1);
		glVertex2i(20+182,GetSystemMetrics(SM_CYSCREEN)-(20+85));
	glEnd();
	
	glDisable2D();
	glEnable(GL_LIGHTING);
}

void DrawDoor(int i, int j, float gap){
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(i*2,0,j*2);
	if (((Door*)level.Building[j][i])->Orient==GW_WEAST_EAST){
		glRotatef(90,0,1,0);
		glTranslatef(-2,0,0);
	}
	glTranslatef(-gap,0,0);
	glScalef(2,2,1);
	glFrontFace(GL_CW);
	glBindTexture(GL_TEXTURE_2D,Texture[TX_DOOR]);
	glBegin(GL_POLYGON);
		glNormal3f(0,0,1);
		glTexCoord2f(0,0.5); 
		glVertex3f(0,0.5,1.25);
		glTexCoord2f(0,1);
		glVertex3f(0,0,1.25);
		glTexCoord2f(0.56,1);
		glVertex3f(0.56,0,1.25);
		glTexCoord2f(0.56,1-0.075);
		glVertex3f(0.56,0.075,1.25);
		glTexCoord2f(0.435,1-0.17);
		glVertex3f(0.435,0.17,1.25);
		glTexCoord2f(0.435,1-0.33);
		glVertex3f(0.435,0.33,1.25);
		glTexCoord2f(0.56,1-0.424);
		glVertex3f(0.56,0.425,1.25);
		glTexCoord2f(0.56,1-0.577);
		glVertex3f(0.56,0.577,1.25);
		glTexCoord2f(0.435,1-0.675);
		glVertex3f(0.435,0.675,1.25);
		glTexCoord2f(0.435,1-0.832);
		glVertex3f(0.435,0.832,1.25);
		glTexCoord2f(0.56,1-0.927);
		glVertex3f(0.56,0.927,1.25);
		glTexCoord2f(0.56,0);
		glVertex3f(0.56,1,1.25);
		glTexCoord2f(0,0);
		glVertex3f(0,1,1.25);
  glEnd();
  glBegin(GL_POLYGON);
		glNormal3f(0,0,-1);
		glTexCoord2f(0,0.5); 
		glVertex3f(0,0.5,0.75);
		glTexCoord2f(0,0);
		glVertex3f(0,1,0.75);
		glTexCoord2f(0.56,0);
		glVertex3f(0.56,1,0.75);
		glTexCoord2f(0.56,1-0.93);
		glVertex3f(0.56,0.93,0.75);
		glTexCoord2f(0.435,1-0.832);
		glVertex3f(0.435,0.832,0.75);
		glTexCoord2f(0.435,1-0.675);
		glVertex3f(0.435,0.675,0.75);
		glTexCoord2f(0.56,1-0.577);
		glVertex3f(0.56,0.577,0.75);
		glTexCoord2f(0.56,1-0.424);
		glVertex3f(0.56,0.425,0.75);
		glTexCoord2f(0.435,1-0.33);
		glVertex3f(0.435,0.33,0.75);
		glTexCoord2f(0.435,1-0.17);
		glVertex3f(0.435,0.17,0.75);
		glTexCoord2f(0.56,1-0.075);
		glVertex3f(0.56,0.075,0.75);
		glTexCoord2f(0.56,1);
		glVertex3f(0.56,0,0.75);
		glTexCoord2f(0,1);
		glVertex3f(0,0,0.75);
  glEnd();
  glFrontFace(GL_CCW);

  glLoadIdentity();

	glTranslatef(i*2,0,j*2);
	if (((Door*)level.Building[j][i])->Orient==GW_WEAST_EAST){
		glRotatef(90,0,1,0);
		glTranslatef(-2,0,0);
	}
	glTranslatef(+gap,0,0);
	glScalef(2,2,1);
	glBindTexture(GL_TEXTURE_2D,Texture[TX_DOOR]);
	glBegin(GL_POLYGON);
		glNormal3f(0,0,1);
		glTexCoord2f(1,0.5); 
		glVertex3f(1,0.5,1.25);
		glTexCoord2f(1,1);
		glVertex3f(1,0,1.25);
		glTexCoord2f(0.56,1);
		glVertex3f(0.56,0,1.25);
		glTexCoord2f(0.56,1-0.075);
		glVertex3f(0.56,0.075,1.25);
		glTexCoord2f(0.435,1-0.17);
		glVertex3f(0.435,0.17,1.25);
		glTexCoord2f(0.435,1-0.33);
		glVertex3f(0.435,0.33,1.25);
		glTexCoord2f(0.56,1-0.424);
		glVertex3f(0.56,0.425,1.25);
		glTexCoord2f(0.56,1-0.577);
		glVertex3f(0.56,0.577,1.25);
		glTexCoord2f(0.435,1-0.675);
		glVertex3f(0.435,0.675,1.25);
		glTexCoord2f(0.435,1-0.832);
		glVertex3f(0.435,0.832,1.25);
		glTexCoord2f(0.56,1-0.927);
		glVertex3f(0.56,0.927,1.25);
		glTexCoord2f(0.56,0);
		glVertex3f(0.56,1,1.25);
		glTexCoord2f(1,0);
		glVertex3f(1,1,1.25);
  glEnd();
  glFrontFace(GL_CW);
  glBegin(GL_POLYGON);
		glNormal3f(0,0,-1);
		glTexCoord2f(1,0.5); 
		glVertex3f(1,0.5,0.75);
		glTexCoord2f(1,1);
		glVertex3f(1,0,0.75);
		glTexCoord2f(0.56,1);
		glVertex3f(0.56,0,0.75);
		glTexCoord2f(0.56,1-0.075);
		glVertex3f(0.56,0.075,0.75);
		glTexCoord2f(0.435,1-0.17);
		glVertex3f(0.435,0.17,0.75);
		glTexCoord2f(0.435,1-0.33);
		glVertex3f(0.435,0.33,0.75);
		glTexCoord2f(0.56,1-0.424);
		glVertex3f(0.56,0.425,0.75);
		glTexCoord2f(0.56,1-0.577);
		glVertex3f(0.56,0.577,0.75);
		glTexCoord2f(0.435,1-0.675);
		glVertex3f(0.435,0.675,0.75);
		glTexCoord2f(0.435,1-0.832);
		glVertex3f(0.435,0.832,0.75);
		glTexCoord2f(0.56,1-0.927);
		glVertex3f(0.56,0.927,0.75);
		glTexCoord2f(0.56,0);
		glVertex3f(0.56,1,0.75);
		glTexCoord2f(1,0);
		glVertex3f(1,1,0.75);
  glEnd();
  glLoadIdentity();
  glFrontFace(GL_CCW);
  glMatrixMode(GL_PROJECTION);
}

void DrawSwitch(int i, int j){
	if(((Switch*)level.Building[i][j])->isOn ){
		glBindTexture(GL_TEXTURE_2D,Texture[TX_SWITCH_ON]);
	} else {
		glBindTexture(GL_TEXTURE_2D,Texture[TX_SWITCH_OFF]);
	}
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glNormal3f(0,0,1);
	glTranslatef(j*2+1,1,i*2+1);
	glRotatef(-player.dir_around,0,1,0);
	glBegin(GL_QUADS);
	  glTexCoord2f(1,0); 					  
	  glVertex3f(-1, -1,0);
	  glTexCoord2f(1,1);
	  glVertex3f(-1, 1,0);
	  glTexCoord2f(0,1);
	  glVertex3f(1, 1,0);						  
	  glTexCoord2f(0,0); 
	  glVertex3f(1, -1,0);
	glEnd();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
}
void DrawChest(int x, int y){
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_CULL_FACE);
	glTranslatef(1+x*2,0.75,1+y*2);
	glScalef(0.75,0.75,0.75);
	glBindTexture(GL_TEXTURE_2D,Texture[TX_BOX]);
	glBegin(GL_QUADS);
	  glNormal3f(0,0,1);
	  				  
	  glTexCoord2f(0,0);
	  glVertex3f(-1, 1, 1);
	  glTexCoord2f(1,0); 					  
	  glVertex3f(-1, -1, 1);
	  glTexCoord2f(1,1);
	  glVertex3f(1, -1, 1);
	  glTexCoord2f(0,1);
	  glVertex3f(1, 1, 1);	

	  glNormal3f(-1,0,0);
	  
	  glTexCoord2f(1,0); 					  
	  glVertex3f(-1, -1, -1);
	  glTexCoord2f(1,1);
	  glVertex3f(-1, 1, -1);
	  glTexCoord2f(0,1);
	  glVertex3f(-1, 1, 1);						  
	  glTexCoord2f(0,0); 
	  glVertex3f(-1, -1, 1);	

	  glNormal3f(0,0,-1);
	  						  
	  glTexCoord2f(0,0);
	  glVertex3f(1, 1, -1);
	  glTexCoord2f(1,0); 					  
	  glVertex3f(1, -1, -1);
	  glTexCoord2f(1,1);
	  glVertex3f(-1, -1, -1);
	  glTexCoord2f(0,1);
	  glVertex3f(-1, 1, -1);

	  glNormal3f(1,0,0);
	  					  
	  glTexCoord2f(0,0);
	  glVertex3f(1, 1, 1);
	  glTexCoord2f(1,0); 					  
	  glVertex3f(1, -1, 1);
	  glTexCoord2f(1,1);
	  glVertex3f(1, -1, -1);
	  glTexCoord2f(0,1);
	  glVertex3f(1, 1, -1);	

	  glNormal3f(0,1,0);
	  
	  glTexCoord2f(1,0); 					  
	  glVertex3f(-1, 1, 1);
	  glTexCoord2f(1,1);
	  glVertex3f(-1, 1, -1);
	  glTexCoord2f(0,1);
	  glVertex3f(1, 1, -1);						  
	  glTexCoord2f(0,0);
	  glVertex3f(1, 1, 1);	
	glEnd();
	glEnable(GL_CULL_FACE);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
}
void DrawKey(int x, int y){
	switch(((Key*)level.Building[y][x])->Key_Type){
		case GW_WHITE: glBindTexture(GL_TEXTURE_2D,Texture[TX_WHITE]); break;
		case GW_RED: glBindTexture(GL_TEXTURE_2D,Texture[TX_RED]); break;
		case GW_GREEN: glBindTexture(GL_TEXTURE_2D,Texture[TX_GREEN]); break;
		case GW_BLUE: glBindTexture(GL_TEXTURE_2D,Texture[TX_BLUE]); break;
		case GW_YELLOW: glBindTexture(GL_TEXTURE_2D,Texture[TX_YELLOW]); break;
	}
	glFrontFace(GL_CW);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(90,0,0,1);
	glTranslatef(0.5,-x*2-1,y*2+1);
	glRotatef(r,1,0,0);
	glScalef(0.25,0.25,0.25);
	glBegin(GL_TRIANGLES);
		for (int i=0; i<108; i+=9){
			v[0][0]=key1_obj[i]; v[0][1]=key1_obj[i+1]; v[0][2]=key1_obj[i+2];
			v[1][0]=key1_obj[i+3]; v[1][1]=key1_obj[i+4]; v[1][2]=key1_obj[i+5];
			v[2][0]=key1_obj[i+6]; v[2][1]=key1_obj[i+7]; v[2][2]=key1_obj[i+8];
			calcNormal(v,w);
			glNormal3f(-w[0],-w[1],-w[2]);
			glTexCoord2f(1,0);  
			glVertex3f(key1_obj[i],key1_obj[i+1],key1_obj[i+2]);
			glTexCoord2f(1,1);  
			glVertex3f(key1_obj[i+3],key1_obj[i+4],key1_obj[i+5]);
			glTexCoord2f(0,1);  
			glVertex3f(key1_obj[i+6],key1_obj[i+7],key1_obj[i+8]);
		}
		for (int i=0; i<108; i+=9){
			v[0][0]=key2_obj[i]; v[0][1]=key2_obj[i+1]; v[0][2]=key2_obj[i+2];
			v[1][0]=key2_obj[i+3]; v[1][1]=key2_obj[i+4]; v[1][2]=key2_obj[i+5];
			v[2][0]=key2_obj[i+6]; v[2][1]=key2_obj[i+7]; v[2][2]=key2_obj[i+8];
			calcNormal(v,w);
			glNormal3f(-w[0],-w[1],-w[2]);
			glTexCoord2f(1,0);  
			glVertex3f(key2_obj[i],key2_obj[i+1],key2_obj[i+2]);
			glTexCoord2f(1,1);  
			glVertex3f(key2_obj[i+3],key2_obj[i+4],key2_obj[i+5]);
			glTexCoord2f(0,1);  
			glVertex3f(key2_obj[i+6],key2_obj[i+7],key2_obj[i+8]);
		}
		for (int i=0; i<720; i+=9){
			v[0][0]=key3_obj[i]; v[0][1]=key3_obj[i+1]; v[0][2]=key3_obj[i+2];
			v[1][0]=key3_obj[i+3]; v[1][1]=key3_obj[i+4]; v[1][2]=key3_obj[i+5];
			v[2][0]=key3_obj[i+6]; v[2][1]=key3_obj[i+7]; v[2][2]=key3_obj[i+8];
			calcNormal(v,w);
			glNormal3f(-w[0],-w[1],-w[2]);
			glTexCoord2f(1,0);  
			glVertex3f(key3_obj[i],key3_obj[i+1],key3_obj[i+2]);
			glTexCoord2f(1,1);  
			glVertex3f(key3_obj[i+3],key3_obj[i+4],key3_obj[i+5]);
			glTexCoord2f(0,1);  
			glVertex3f(key3_obj[i+6],key3_obj[i+7],key3_obj[i+8]);
		}
		for (int i=0; i<108; i+=9){
			v[0][0]=key4_obj[i]; v[0][1]=key4_obj[i+1]; v[0][2]=key4_obj[i+2];
			v[1][0]=key4_obj[i+3]; v[1][1]=key4_obj[i+4]; v[1][2]=key4_obj[i+5];
			v[2][0]=key4_obj[i+6]; v[2][1]=key4_obj[i+7]; v[2][2]=key4_obj[i+8];
			calcNormal(v,w);
			glNormal3f(-w[0],-w[1],-w[2]);
			glTexCoord2f(1,0);  
			glVertex3f(key4_obj[i],key4_obj[i+1],key4_obj[i+2]);
			glTexCoord2f(1,1);  
			glVertex3f(key4_obj[i+3],key4_obj[i+4],key4_obj[i+5]);
			glTexCoord2f(0,1);  
			glVertex3f(key4_obj[i+6],key4_obj[i+7],key4_obj[i+8]);
		}
	glEnd();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glFrontFace(GL_CCW);
}
void DrawMonsters(){
	for (int i=0; i<level.num_of_monsters; i++){
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(level.Monsters[i]->world_poz_y,1-level.Monsters[i]->dead/91.0,level.Monsters[i]->world_poz_x);
		glRotatef(-player.dir_around,0,1,0);
		glRotatef(-level.Monsters[i]->dead,1,0,0);
		if (level.Monsters[i]->MState==M_ATTACK) {
			switch(level.Monsters[i]->MType){
				case GW_MONSTER_1: glBindTexture(GL_TEXTURE_2D,Texture[TX_MONSTER_1A]); break;
				case GW_MONSTER_2: glBindTexture(GL_TEXTURE_2D,Texture[TX_MONSTER_2A]); break;
				case GW_MONSTER_3: glBindTexture(GL_TEXTURE_2D,Texture[TX_MONSTER_3A]); break;
				case GW_MONSTER_4: glBindTexture(GL_TEXTURE_2D,Texture[TX_MONSTER_4A]); break;
				case GW_MONSTER_5: glBindTexture(GL_TEXTURE_2D,Texture[TX_MONSTER_5A]); break;
			}
		} else {
			switch(level.Monsters[i]->MType){
				case GW_MONSTER_1: glBindTexture(GL_TEXTURE_2D,Texture[TX_MONSTER_1]); break;
				case GW_MONSTER_2: glBindTexture(GL_TEXTURE_2D,Texture[TX_MONSTER_2]); break;
				case GW_MONSTER_3: glBindTexture(GL_TEXTURE_2D,Texture[TX_MONSTER_3]); break;
				case GW_MONSTER_4: glBindTexture(GL_TEXTURE_2D,Texture[TX_MONSTER_4]); break;
				case GW_MONSTER_5: glBindTexture(GL_TEXTURE_2D,Texture[TX_MONSTER_5]); break;
			}
		}
		glBegin(GL_QUADS);
		  glNormal3f(0,0,1);
		  glTexCoord2f(1,0); 					  
		  glVertex3f(-1, -1,0);
		  glTexCoord2f(1,1);
		  glVertex3f(-1, 1,0);
		  glTexCoord2f(0,1);
		  glVertex3f(1, 1,0);						  
		  glTexCoord2f(0,0); 
		  glVertex3f(1, -1,0);
		glEnd();
		glLoadIdentity();
		glMatrixMode(GL_PROJECTION);
	}
}
void DrawFan(int x, int y){
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glTranslatef(1+x*2,1,1+y*2);
	switch(((Fan*)level.Building[y][x])->Dir){
		case GW_NORTH:
			glRotatef(180,0,1,0);
			break;
		case GW_SOUTH:
			glRotatef(0,0,1,0);
			break;
		case GW_EAST:
			glRotatef(90,0,1,0);
			break;
		case GW_WEST:
			glRotatef(270,0,1,0);
			break;
	}
	glBindTexture(GL_TEXTURE_2D,Texture[TX_METAL]);
	glBegin(GL_QUADS);
	  		
	  v[0][0]=-1; v[0][1]=-1; v[0][2]=-1;
	  v[1][0]=-1; v[1][1]=1; v[1][2]=-1;
	  v[2][0]=-1; v[2][1]=1; v[2][2]=1;
	  calcNormal(v,w);
	  glNormal3fv(w);
	  
	  glTexCoord2f(1,0); 					  
	  glVertex3f(-1, -1, -1);
	  glTexCoord2f(1,1);
	  glVertex3f(-1, 1, -1);
	  glTexCoord2f(0,1);
	  glVertex3f(-1, 1, 1);						  
	  glTexCoord2f(0,0); 
	  glVertex3f(-1, -1, 1);
      
	  v[0][0]=1; v[0][1]=-1; v[0][2]=1;
	  v[1][0]=1; v[1][1]=1; v[1][2]=1;
	  v[2][0]=1; v[2][1]=1; v[2][2]=-1;
	  calcNormal(v,w);
	  glNormal3fv(w);

	  glTexCoord2f(1,0); 
	  glVertex3f(1, -1, 1);
	  glTexCoord2f(1,1);
	  glVertex3f(1, 1, 1);
	  glTexCoord2f(0,1);
	  glVertex3f(1, 1, -1);
	  glTexCoord2f(0,0); 
	  glVertex3f(1, -1, -1);
      
	  v[0][0]=1; v[0][1]=-1; v[0][2]=-1;
	  v[1][0]=1; v[1][1]=1; v[1][2]=-1;
	  v[2][0]=-1; v[2][1]=1; v[2][2]=-1;
	  calcNormal(v,w);
	  glNormal3fv(w);

	  glTexCoord2f(0,0);
	  glVertex3f(1, -1, -1);
	  glTexCoord2f(0,1);
	  glVertex3f(1, 1, -1);
	  glTexCoord2f(1,1);
	  glVertex3f(-1, 1, -1);
	  glTexCoord2f(1,0);  						 
	  glVertex3f(-1, -1, -1);
	  
	  //--------------------------------------------
	  	  		
	  v[0][0]=-1; v[0][1]=-1; v[0][2]=-1;
	  v[1][0]=-1; v[1][1]=1; v[1][2]=-1;
	  v[2][0]=-1; v[2][1]=1; v[2][2]=1;
	  calcNormal(v,w);
	  glNormal3f(-w[0],-w[1],-w[2]);
	  				  
	  glTexCoord2f(0,0); 
	  glVertex3f(-0.9, -1, 1);
	  glTexCoord2f(0,1);
	  glVertex3f(-0.9, 1, 1);
	  glTexCoord2f(1,1);
	  glVertex3f(-0.9, 1, -1);
	  glTexCoord2f(1,0); 					  
	  glVertex3f(-0.9, -1, -1);
      
	  v[0][0]=1; v[0][1]=-1; v[0][2]=1;
	  v[1][0]=1; v[1][1]=1; v[1][2]=1;
	  v[2][0]=1; v[2][1]=1; v[2][2]=-1;
	  calcNormal(v,w);
	  glNormal3f(-w[0],-w[1],-w[2]);

	  glTexCoord2f(0,0); 
	  glVertex3f(0.9, -1, -1);
	  glTexCoord2f(0,1);
	  glVertex3f(0.9, 1, -1);
	  glTexCoord2f(1,1);
	  glVertex3f(0.9, 1, 1);
	  glTexCoord2f(1,0); 
	  glVertex3f(0.9, -1, 1);
      
	  v[0][0]=1; v[0][1]=-1; v[0][2]=-1;
	  v[1][0]=1; v[1][1]=1; v[1][2]=-1;
	  v[2][0]=-1; v[2][1]=1; v[2][2]=-1;
	  calcNormal(v,w);
	  glNormal3f(-w[0],-w[1],-w[2]);

	  glTexCoord2f(1,0);  						 
	  glVertex3f(-1, -1, 0.5);
	  glTexCoord2f(1,1);
	  glVertex3f(-1, 1, 0.5);
	  glTexCoord2f(0,1);
	  glVertex3f(1, 1, 0.5);
	  glTexCoord2f(0,0);
	  glVertex3f(1, -1, 0.5);
	  
	  //-------------------------------------

	  glNormal3f(0,1,0);

	  glTexCoord2f(1,0);  						 
	  glVertex3f(-1, -0.9, -1);
	  glTexCoord2f(1,1);
	  glVertex3f(1, -0.9, -1);
	  glTexCoord2f(0,1);
	  glVertex3f(1, -0.9, 1);
	  glTexCoord2f(0,0);
	  glVertex3f(-1, -0.9, 1);

	  glNormal3f(0,-1,0);

	  glTexCoord2f(0,0);
	  glVertex3f(-1, 0.9, 1);
	  glTexCoord2f(0,1);
	  glVertex3f(1, 0.9, 1);
	  glTexCoord2f(1,1);
	  glVertex3f(1, 0.9, -1);
	  glTexCoord2f(1,0);  						 
	  glVertex3f(-1, 0.9, -1);
	glEnd();

	glBindTexture(GL_TEXTURE_2D,Texture[TX_MESH]);
	glBegin(GL_QUADS);
	  v[0][0]=-1; v[0][1]=-1; v[0][2]=1;
	  v[1][0]=-1; v[1][1]=1; v[1][2]=1;
	  v[2][0]=1; v[2][1]=1; v[2][2]=1;
	  calcNormal(v,w);
	  glNormal3fv(w);

	  glTexCoord2f(1,0);  
	  glVertex3f(-1, -1, 1);
	  glTexCoord2f(1,1);
	  glVertex3f(-1, 1, 1);
	  glTexCoord2f(0,1);
	  glVertex3f(1, 1, 1);
	  glTexCoord2f(0,0);  
	  glVertex3f(1, -1, 1);
	glEnd();
	
	if(((Fan*)level.Building[y][x])->isOn){
		((Fan*)level.Building[y][x])->r+=((Fan*)level.Building[y][x])->Strength*2;
	}

	glRotatef(((Fan*)level.Building[y][x])->r,0,0,1);
	glBindTexture(GL_TEXTURE_2D,Texture[TX_FAN]);
	glBegin(GL_QUADS);
	  glNormal3f(0,0,1);
	  glTexCoord2f(1,0);  
	  glVertex3f(-1, -1, 0.75);
	  glTexCoord2f(1,1);
	  glVertex3f(-1, 1, 0.75);
	  glTexCoord2f(0,1);
	  glVertex3f(1, 1, 0.75);
	  glTexCoord2f(0,0);  
	  glVertex3f(1, -1, 0.75);
	glEnd();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
}
void DrawLamp(int x, int y){
	glFrontFace(GL_CW);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(x*2+1,1.9,y*2+1);
	glScalef(0.2,0.2,0.2);

	glBindTexture(GL_TEXTURE_2D,Texture[TX_GLASS]);

	glBegin(GL_TRIANGLES);
		for (int i=0; i<288; i+=9){
			v[0][0]=lamp_obj[i]; v[0][1]=lamp_obj[i+1]; v[0][2]=lamp_obj[i+2];
			v[1][0]=lamp_obj[i+3]; v[1][1]=lamp_obj[i+4]; v[1][2]=lamp_obj[i+5];
			v[2][0]=lamp_obj[i+6]; v[2][1]=lamp_obj[i+7]; v[2][2]=lamp_obj[i+8];
			calcNormal(v,w);
			glNormal3f(-w[0],-w[1],-w[2]);
			glTexCoord2f(1,0);  
			glVertex3f(lamp_obj[i],lamp_obj[i+1],lamp_obj[i+2]);
			glTexCoord2f(1,1);  
			glVertex3f(lamp_obj[i+3],lamp_obj[i+4],lamp_obj[i+5]);
			glTexCoord2f(0,1);  
			glVertex3f(lamp_obj[i+6],lamp_obj[i+7],lamp_obj[i+8]);
		}
	glEnd();
	if(level.lamp[level.closest_lamp].x==y&&level.lamp[level.closest_lamp].y==x)glDisable(GL_LIGHTING);
	//glDisable(GL_LIGHTING);
	glBegin(GL_TRIANGLES);
		for (int i=0; i<288; i+=9){
			v[0][0]=lamp_obj[i]; v[0][1]=lamp_obj[i+1]; v[0][2]=lamp_obj[i+2];
			v[1][0]=lamp_obj[i+3]; v[1][1]=lamp_obj[i+4]; v[1][2]=lamp_obj[i+5];
			v[2][0]=lamp_obj[i+6]; v[2][1]=lamp_obj[i+7]; v[2][2]=lamp_obj[i+8];
			calcNormal(v,w);
			glNormal3fv(w);
			glTexCoord2f(1,0);  
			glVertex3f(lamp_obj[i+6],lamp_obj[i+7],lamp_obj[i+8]);
			glTexCoord2f(1,1);  
			glVertex3f(lamp_obj[i+3],lamp_obj[i+4],lamp_obj[i+5]);
			glTexCoord2f(0,1);  
			glVertex3f(lamp_obj[i],lamp_obj[i+1],lamp_obj[i+2]);
		}
		glColor3f(1,1,1);
	glEnd();
	glTranslatef(0,0.5,0);
	glScalef(0.8,0.8,0.8);
	glutSolidSphere(1,10,10);
	glEnable(GL_LIGHTING);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glFrontFace(GL_CCW);
}
void DrawLife(int x, int y){
	glBindTexture(GL_TEXTURE_2D,Texture[TX_RED]);
	glFrontFace(GL_CW);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(180,0,0,1);
	glTranslatef(-x*2-1,-1,y*2+1);

	
	glRotatef(r,0,1,0);
	
	glScalef(0.5,0.5,0.5);
	glBegin(GL_TRIANGLES);
		for (int i=0; i<5364; i+=9){
			v[0][0]=life_obj[i]; v[0][1]=life_obj[i+1]; v[0][2]=life_obj[i+2];
			v[1][0]=life_obj[i+3]; v[1][1]=life_obj[i+4]; v[1][2]=life_obj[i+5];
			v[2][0]=life_obj[i+6]; v[2][1]=life_obj[i+7]; v[2][2]=life_obj[i+8];
			calcNormal(v,w);
			glNormal3f(-w[0],-w[1],-w[2]);
			glTexCoord2f(1,0);  
			glVertex3f(life_obj[i],life_obj[i+1],life_obj[i+2]);
			glTexCoord2f(1,1);  
			glVertex3f(life_obj[i+3],life_obj[i+4],life_obj[i+5]);
			glTexCoord2f(0,1);  
			glVertex3f(life_obj[i+6],life_obj[i+7],life_obj[i+8]);
		}
	glEnd();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glFrontFace(GL_CCW);
}
void DrawPoint(int x, int y){
	glBindTexture(GL_TEXTURE_2D,Texture[TX_YELLOW]);
	glFrontFace(GL_CW);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(180,0,0,1);
	glTranslatef(-x*2-1,-0.5,y*2+1);
	
	glRotatef(r,0,1,0);
	
	glScalef(0.2,0.2,0.2);
	glBegin(GL_TRIANGLES);
		for (int i=0; i<540; i+=9){
			v[0][0]=point_obj[i]; v[0][1]=point_obj[i+1]; v[0][2]=point_obj[i+2];
			v[1][0]=point_obj[i+3]; v[1][1]=point_obj[i+4]; v[1][2]=point_obj[i+5];
			v[2][0]=point_obj[i+6]; v[2][1]=point_obj[i+7]; v[2][2]=point_obj[i+8];
			calcNormal(v,w);
			glNormal3f(-w[0],-w[1],-w[2]);
			glTexCoord2f(1,0);  
			glVertex3f(point_obj[i],point_obj[i+1],point_obj[i+2]);
			glTexCoord2f(1,1);  
			glVertex3f(point_obj[i+3],point_obj[i+4],point_obj[i+5]);
			glTexCoord2f(0,1);  
			glVertex3f(point_obj[i+6],point_obj[i+7],point_obj[i+8]);
		}
	glEnd();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glFrontFace(GL_CCW);
}
void DrawAmmo(int x, int y){
	glBindTexture(GL_TEXTURE_2D,Texture[TX_GOLD]);
	glFrontFace(GL_CW);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(180,0,0,1);
	glTranslatef(-x*2-1,-0.5,y*2+1);

	
	glRotatef(r,0,1,0);
	
	glScalef(0.25,0.25,0.25);
	glBegin(GL_TRIANGLES);
		for (int i=0; i<1404; i+=9){
			v[0][0]=ammo_obj[i]; v[0][1]=ammo_obj[i+1]; v[0][2]=ammo_obj[i+2];
			v[1][0]=ammo_obj[i+3]; v[1][1]=ammo_obj[i+4]; v[1][2]=ammo_obj[i+5];
			v[2][0]=ammo_obj[i+6]; v[2][1]=ammo_obj[i+7]; v[2][2]=ammo_obj[i+8];
			calcNormal(v,w);
			glNormal3f(-w[0],-w[1],-w[2]);
			glTexCoord2f(1,0);  
			glVertex3f(ammo_obj[i],ammo_obj[i+1],ammo_obj[i+2]);
			glTexCoord2f(1,1);  
			glVertex3f(ammo_obj[i+3],ammo_obj[i+4],ammo_obj[i+5]);
			glTexCoord2f(0,1);  
			glVertex3f(ammo_obj[i+6],ammo_obj[i+7],ammo_obj[i+8]);
		}
	glEnd();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glFrontFace(GL_CCW);
}
void DrawSpikes(int x, int y){
	glBindTexture(GL_TEXTURE_2D,Texture[TX_METAL]);
	glFrontFace(GL_CW);
	glMatrixMode(GL_MODELVIEW);
	for (int j=0; j<40; j++) {
		glLoadIdentity();
		glTranslatef(x*2+((Spikes*)level.Building[y][x])->spikes[j].x/500.0,((Spikes*)level.Building[y][x])->r,y*2+((Spikes*)level.Building[y][x])->spikes[j].y/500.0);
		glRotatef(-90,1,0,0);
		glScalef(0.25,0.25,0.25);
		glBegin(GL_TRIANGLES);
			for (int i=0; i<36; i+=9){
				v[0][0]=spike_obj[i]; v[0][1]=spike_obj[i+1]; v[0][2]=spike_obj[i+2]*((Spikes*)level.Building[y][x])->spikesize[j]/500.0;
				v[1][0]=spike_obj[i+3]; v[1][1]=spike_obj[i+4]; v[1][2]=spike_obj[i+5]*((Spikes*)level.Building[y][x])->spikesize[j]/500.0;
				v[2][0]=spike_obj[i+6]; v[2][1]=spike_obj[i+7]; v[2][2]=spike_obj[i+8]*((Spikes*)level.Building[y][x])->spikesize[j]/500.0;
				calcNormal(v,w);
				glNormal3f(-w[0],-w[1],-w[2]);
				glTexCoord2f(1,0);  
				glVertex3f(spike_obj[i],spike_obj[i+1],spike_obj[i+2]*((Spikes*)level.Building[y][x])->spikesize[j]/500.0);
				glTexCoord2f(1,1);  
				glVertex3f(spike_obj[i+3],spike_obj[i+4],spike_obj[i+5]*((Spikes*)level.Building[y][x])->spikesize[j]/500.0);
				glTexCoord2f(0,1);  
				glVertex3f(spike_obj[i+6],spike_obj[i+7],spike_obj[i+8]*((Spikes*)level.Building[y][x])->spikesize[j]/500.0);
			}
		glEnd();
	}
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glFrontFace(GL_CCW);
}
void DrawBox(float x1, float y1, float z1, float x2, float y2, float z2, GLuint Texture){
	glBindTexture(GL_TEXTURE_2D,Texture);
	glBegin(GL_QUADS);
		
		v[0][0]=x1; v[0][1]=y1; v[0][2]=z2;
	    v[1][0]=x2; v[1][1]=y1; v[1][2]=z2;
	    v[2][0]=x2; v[2][1]=y1; v[2][2]=z1;
	    calcNormal(v,w);
	    glNormal3fv(w);
		
		glTexCoord2f(0,0);
		glVertex3f(x1, y1, z2);
		glTexCoord2f(1,0);
		glVertex3f(x2, y1, z2);
		glTexCoord2f(1,1);
		glVertex3f(x2, y1, z1);
		glTexCoord2f(0,1);
		glVertex3f(x1, y1, z1);
		
		v[0][0]=x1; v[0][1]=y1; v[0][2]=z1;
	    v[1][0]=x2; v[1][1]=y1; v[1][2]=z1;
	    v[2][0]=x2; v[2][1]=y2; v[2][2]=z1;
	    calcNormal(v,w);
	    glNormal3fv(w);

		glTexCoord2f(0,1);
		glVertex3f(x1, y1, z1);
		glTexCoord2f(1,1);
		glVertex3f(x2, y1, z1);
		glTexCoord2f(1,0);
		glVertex3f(x2, y2, z1);
		glTexCoord2f(0,0);
		glVertex3f(x1, y2, z1);
		
		v[0][0]=x1; v[0][1]=y1; v[0][2]=z1;
	    v[1][0]=x1; v[1][1]=y2; v[1][2]=z1;
	    v[2][0]=x1; v[2][1]=y2; v[2][2]=z2;
	    calcNormal(v,w);
	    glNormal3fv(w);

		glTexCoord2f(0,1);
		glVertex3f(x1, y1, z1);
		glTexCoord2f(1,1);
		glVertex3f(x1, y2, z1);
		glTexCoord2f(1,0);
		glVertex3f(x1, y2, z2);
		glTexCoord2f(0,0);
		glVertex3f(x1, y1, z2);

		v[0][0]=x2; v[0][1]=y1; v[0][2]=z2;
	    v[1][0]=x1; v[1][1]=y1; v[1][2]=z2;
	    v[2][0]=x1; v[2][1]=y2; v[2][2]=z2;
	    calcNormal(v,w);
	    glNormal3fv(w);
		
		glTexCoord2f(0,0);
		glVertex3f(x2, y1, z2);
		glTexCoord2f(1,0);
		glVertex3f(x1, y1, z2);
		glTexCoord2f(1,1);
		glVertex3f(x1, y2, z2);
		glTexCoord2f(0,1);
		glVertex3f(x2, y2, z2);

		v[0][0]=x2; v[0][1]=y2; v[0][2]=z1;
	    v[1][0]=x2; v[1][1]=y1; v[1][2]=z1;
	    v[2][0]=x2; v[2][1]=y1; v[2][2]=z2;
	    calcNormal(v,w);
	    glNormal3fv(w);
		
		glTexCoord2f(0,0);
		glVertex3f(x2, y2, z1);
		glTexCoord2f(1,0);
		glVertex3f(x2, y1, z1);
		glTexCoord2f(1,1);
		glVertex3f(x2, y1, z2);
		glTexCoord2f(0,1);
		glVertex3f(x2, y2, z2);

		v[0][0]=x1; v[0][1]=y2; v[0][2]=z2;
	    v[1][0]=x1; v[1][1]=y2; v[1][2]=z1;
	    v[2][0]=x2; v[2][1]=y2; v[2][2]=z1;
	    calcNormal(v,w);
	    glNormal3fv(w);
		
		glTexCoord2f(0,0);
		glVertex3f(x1, y2, z2);
		glTexCoord2f(1,0);
		glVertex3f(x1, y2, z1);
		glTexCoord2f(1,1);
		glVertex3f(x2, y2, z1);
		glTexCoord2f(0,1);
		glVertex3f(x2, y2, z2);
	glEnd();
}
void DrawWindow(int i, int j, Orientation ort){
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if (ort==GW_NORTH_SOUTH) {
		glTranslatef(-1,0,-1);
		glRotatef(90,0,1,0);
		glTranslatef(1,0,1);
		glTranslatef(-i*2-4,0,j*2);
	} else {
		glTranslatef(j*2,0,i*2);
	}

	
	glBindTexture(GL_TEXTURE_2D,Texture[TX_WALL]);
	glBegin(GL_QUADS);
	  //solid side
	  v[0][0]=0; v[0][1]=0; v[0][2]=0;
      v[1][0]=0; v[1][1]=2.0; v[1][2]=0;
      v[2][0]=0; v[2][1]=2.0; v[2][2]=2;
      calcNormal(v,w);
      glNormal3fv(w);

	  glTexCoord2f(1,0); 					  
	  glVertex3f(0, 0.0, 0);
	  glTexCoord2f(1,1);
	  glVertex3f(0, 2.0, 0);
	  glTexCoord2f(0,1);
	  glVertex3f(0, 2.0, 2);						  
	  glTexCoord2f(0,0); 
	  glVertex3f(0, 0.0, 2);
	  //solid side
	  glNormal3f(-w[0],-w[1],-w[2]);
	  glTexCoord2f(1,0); 
	  glVertex3f(2, 0.0, 2);
	  glTexCoord2f(1,1);
	  glVertex3f(2, 2.0, 2);
	  glTexCoord2f(0,1);
	  glVertex3f(2, 2.0, 0);
	  glTexCoord2f(0,0); 
	  glVertex3f(2, 0.0, 0);

	  //window wside

	  v[0][0]=0; v[0][1]=0; v[0][2]=2;
      v[1][0]=0; v[1][1]=2.0; v[1][2]=2;
      v[2][0]=0.4; v[2][1]=2.0; v[2][2]=2;
      calcNormal(v,w);
      glNormal3fv(w);

	  glTexCoord2f(1,0);  
	  glVertex3f(0, 0.0, 2);
	  glTexCoord2f(1,1);
	  glVertex3f(0, 2.0, 2);
	  glTexCoord2f(0.8,1);
	  glVertex3f(0.4, 2.0, 2);
	  glTexCoord2f(0.8,0);  
	  glVertex3f(0.4, 0.0, 2);
	 
	  glTexCoord2f(0.2,0);  
	  glVertex3f(1.6, 0.0, 2);
	  glTexCoord2f(0.2,1);
	  glVertex3f(1.6, 2.0, 2);
	  glTexCoord2f(0,1);
	  glVertex3f(2, 2.0, 2);
	  glTexCoord2f(0,0);  
	  glVertex3f(2, 0.0, 2);

	  glTexCoord2f(0.8,0); 
	  glVertex3f(0.4, 0.0, 2);
	  glTexCoord2f(0.8,0.1);
	  glVertex3f(0.4, 0.2, 2);
	  glTexCoord2f(0.2,0.1);
	  glVertex3f(1.6, 0.2, 2);
	  glTexCoord2f(0.2,0); 
	  glVertex3f(1.6, 0.0, 2);

	  glTexCoord2f(0.8,0.7); 
	  glVertex3f(0.4, 1.4, 2);
	  glTexCoord2f(0.8,1);
	  glVertex3f(0.4, 2, 2);
	  glTexCoord2f(0.2,1);
	  glVertex3f(1.6, 2, 2);
	  glTexCoord2f(0.2,0.7); 
	  glVertex3f(1.6, 1.4, 2);
	  //windo wside
	  glNormal3f(-w[0],-w[1],-w[2]);
	  
	  
	  glTexCoord2f(0.8,0);  
	  glVertex3f(0.4, 0.0, 0);
	  glTexCoord2f(0.8,1);
	  glVertex3f(0.4, 2.0, 0);
	  glTexCoord2f(1,1);
	  glVertex3f(0, 2.0, 0);
	  glTexCoord2f(1,0);  
	  glVertex3f(0, 0.0, 0);
	 
	  
	  
	  
	  glTexCoord2f(0,0);  
	  glVertex3f(2, 0.0, 0);
	  glTexCoord2f(0,1);
	  glVertex3f(2, 2.0, 0);
	  glTexCoord2f(0.2,1);
	  glVertex3f(1.6, 2.0, 0);
	  glTexCoord2f(0.2,0);  
	  glVertex3f(1.6, 0.0, 0);	  
	  
	  glTexCoord2f(0.2,0); 
	  glVertex3f(1.6, 0.0, 0);
	  glTexCoord2f(0.2,0.1);
	  glVertex3f(1.6, 0.2, 0);
	  glTexCoord2f(0.8,0.1);
	  glVertex3f(0.4, 0.2, 0);
	  glTexCoord2f(0.8,0); 
	  glVertex3f(0.4, 0.0, 0);
	  
	  glTexCoord2f(0.2,0.7); 
	  glVertex3f(1.6, 1.4, 0);
	  glTexCoord2f(0.2,1);
	  glVertex3f(1.6, 2, 0);
	  glTexCoord2f(0.8,1);
	  glVertex3f(0.4, 2, 0);
	  glTexCoord2f(0.8,0.7); 
	  glVertex3f(0.4, 1.4, 0);
	  //inner side
	  
	  v[0][0]=1.6; v[0][1]=0.2; v[0][2]=0;
	  v[1][0]=1.6; v[1][1]=0.2; v[1][2]=2;
	  v[2][0]=0.4; v[2][1]=0.2; v[2][2]=2;
	  calcNormal(v,w);
	  glNormal3fv(w);  
	  
	  glTexCoord2f(0,0.6);
	  glVertex3f(1.6, 0.2, 0);
	  glTexCoord2f(1,0.6);
	  glVertex3f(1.6, 0.2, 2);
	  glTexCoord2f(1,0);
	  glVertex3f(0.4, 0.2, 2);
	  glTexCoord2f(0,0);
	  glVertex3f(0.4, 0.2, 0);
	  
	  v[0][0]=0.4; v[0][1]=0.2; v[0][2]=0;
	  v[1][0]=0.4; v[1][1]=0.2; v[1][2]=2;
	  v[2][0]=0.4; v[2][1]=1.4; v[2][2]=2;
	  calcNormal(v,w);
	  glNormal3fv(w);

	  glTexCoord2f(0,0);
	  glVertex3f(0.4, 0.2, 0);
	  glTexCoord2f(1,0);
	  glVertex3f(0.4, 0.2, 2);
	  glTexCoord2f(1,0.6);
	  glVertex3f(0.4, 1.4, 2);
	  glTexCoord2f(0,0.6);
	  glVertex3f(0.4, 1.4, 0);
	  
	  v[0][0]=1.6; v[0][1]=1.4; v[0][2]=0;
	  v[1][0]=1.6; v[1][1]=1.4; v[1][2]=2;
	  v[2][0]=1.6; v[2][1]=0.2; v[2][2]=2;
	  calcNormal(v,w);
	  glNormal3fv(w);	  
	  
	  glTexCoord2f(0,0.6);
	  glVertex3f(1.6, 1.4, 0);
	  glTexCoord2f(1,0.6);
	  glVertex3f(1.6, 1.4, 2);
	  glTexCoord2f(1,0);
	  glVertex3f(1.6, 0.2, 2);
	  glTexCoord2f(0,0);
	  glVertex3f(1.6, 0.2, 0);
	  
	  v[0][0]=0.4; v[0][1]=1.4; v[0][2]=0;
	  v[1][0]=0.4; v[1][1]=1.4; v[1][2]=2;
	  v[2][0]=1.6; v[2][1]=1.4; v[2][2]=2;
	  calcNormal(v,w);
	  glNormal3fv(w);

	  glTexCoord2f(0,0);
	  glVertex3f(0.4, 1.4, 0);
	  glTexCoord2f(1,0);
	  glVertex3f(0.4, 1.4, 2);
	  glTexCoord2f(1,0.6);
	  glVertex3f(1.6, 1.4, 2);
	  glTexCoord2f(0,0.6);
	  glVertex3f(1.6, 1.4, 0);
      	
  glEnd();
  DrawBox(0.5,1.5,0.9,0.3,0.1,1.1,Texture[TX_WINDOW]);
  DrawBox(1.7,1.5,0.9,1.5,0.1,1.1,Texture[TX_WINDOW]);
  DrawBox(1.5,1.5,0.9,0.5,1.3,1.1,Texture[TX_WINDOW]);
  DrawBox(1.5,0.3,0.9,0.5,0.1,1.1,Texture[TX_WINDOW]);
  glLoadIdentity();
}

void DrawGlass(int i, int j, Orientation ort) {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if (ort==GW_NORTH_SOUTH) {
		glTranslatef(-1,0,-1);
		glRotatef(90,0,1,0);
		glTranslatef(1,0,1);
		glTranslatef(-i*2-4,0,j*2);
	} else {
		glTranslatef(j*2,0,i*2);
	}
	  glBindTexture(GL_TEXTURE_2D,Texture[TX_GLASS]);
	  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	  glEnable(GL_BLEND);
	  glBegin(GL_QUADS);
		  
		  v[0][0]=0.4; v[0][1]=0.2; v[0][2]=1.05;
	      v[1][0]=0,4; v[1][1]=1.4; v[1][2]=1.05;
	      v[2][0]=1.6; v[2][1]=1.4; v[2][2]=1.05;
	      calcNormal(v,w);
	      glNormal3fv(w);

		  glTexCoord2f(1,0);  
		  glVertex3f(0.4, 0.2, 1.05);
		  glTexCoord2f(1,1);
		  glVertex3f(0.4, 1.4, 1.05);
		  glTexCoord2f(0,1);
		  glVertex3f(1.6, 1.4, 1.05);
		  glTexCoord2f(0,0);  
		  glVertex3f(1.6, 0.2, 1.05);
			
		  glNormal3f(-w[0],-w[1],-w[2]);
		  glTexCoord2f(0,0);  
		  glVertex3f(1.6, 0.2, 0.95);
		  glTexCoord2f(0,1);
		  glVertex3f(1.6, 1.4, 0.95);
		  glTexCoord2f(1,1);
		  glVertex3f(0.4, 1.4, 0.95);
		  glTexCoord2f(1,0);  
		  glVertex3f(0.4, 0.2, 0.95);
	  glEnd();
	  glDisable(GL_BLEND);
      glLoadIdentity();
}
void DrawCeiling(int i, int j) {
	glBindTexture(GL_TEXTURE_2D,Texture[TX_CEILING]);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glBegin(GL_QUADS);
	  v[0][0]=j*2; v[0][1]=0.0; v[0][2]=(i+1)*2;
	  v[1][0]=(j+1)*2; v[1][1]=0.0; v[1][2]=(i+1)*2;
	  v[2][0]=(j+1)*2; v[2][1]=0.0; v[2][2]=i*2;
	  calcNormal(v,w);
	  glNormal3fv(w);
	  glTexCoord2f(0,0);
	  glVertex3f(j*2, 2.0, (i+1)*2);
	  glTexCoord2f(1,0);
	  glVertex3f((j+1)*2, 2.0, (i+1)*2);
	  glTexCoord2f(1,1);
	  glVertex3f((j+1)*2, 2.0, i*2);
	  glTexCoord2f(0,1);
	  glVertex3f(j*2, 2.0, i*2);
	glEnd();
	//glDisable(GL_BLEND);
}
void DrawFloor(int i, int j) {
	glBindTexture(GL_TEXTURE_2D,Texture[TX_FLOOR]);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glBegin(GL_QUADS);
	  v[0][0]=j*2; v[0][1]=0.0; v[0][2]=i*2;
	  v[1][0]=(j+1)*2; v[1][1]=0.0; v[1][2]=i*2;
	  v[2][0]=(j+1)*2; v[2][1]=0.0; v[2][2]=(i+1)*2;
	  calcNormal(v,w);
	  glNormal3fv(w);

	  glTexCoord2f(0,1);
	  glVertex3f(j*2, 0.0, i*2);
	  glTexCoord2f(1,1);
	  glVertex3f((j+1)*2, 0.0, i*2);
	  glTexCoord2f(1,0);
	  glVertex3f((j+1)*2, 0.0, (i+1)*2);
	  glTexCoord2f(0,0);
	  glVertex3f(j*2, 0.0, (i+1)*2);
	glEnd();
	//glDisable(GL_BLEND);
}
void DrawWall(int i, int j) {
	glBindTexture(GL_TEXTURE_2D,Texture[TX_WALL]);
	glBegin(GL_QUADS);
	  		
	  v[0][0]=j*2; v[0][1]=0.0; v[0][2]=i*2;
	  v[1][0]=j*2; v[1][1]=2.0; v[1][2]=i*2;
	  v[2][0]=j*2; v[2][1]=2.0; v[2][2]=(i+1)*2;
	  calcNormal(v,w);
	  glNormal3fv(w);
	  
	  glTexCoord2f(1,0); 					  
	  glVertex3f(j*2, 0.0, i*2);
	  glTexCoord2f(1,1);
	  glVertex3f(j*2, 2.0, i*2);
	  glTexCoord2f(0,1);
	  glVertex3f(j*2, 2.0, (i+1)*2);						  
	  glTexCoord2f(0,0); 
	  glVertex3f(j*2, 0.0, (i+1)*2);
	  
	  v[0][0]=j*2; v[0][1]=0.0; v[0][2]=(i+1)*2;
	  v[1][0]=j*2; v[1][1]=2.0; v[1][2]=(i+1)*2;
	  v[2][0]=(j+1)*2; v[2][1]=2.0; v[2][2]=(i+1)*2;
	  calcNormal(v,w);
	  glNormal3fv(w);

	  glTexCoord2f(1,0);  
	  glVertex3f(j*2, 0.0, (i+1)*2);
	  glTexCoord2f(1,1);
	  glVertex3f(j*2, 2.0, (i+1)*2);
	  glTexCoord2f(0,1);
	  glVertex3f((j+1)*2, 2.0, (i+1)*2);
	  glTexCoord2f(0,0);  
	  glVertex3f((j+1)*2, 0.0, (i+1)*2);
      
	  v[0][0]=(j+1)*2; v[0][1]=0.0; v[0][2]=(i+1)*2;
	  v[1][0]=(j+1)*2; v[1][1]=2.0; v[1][2]=(i+1)*2;
	  v[2][0]=(j+1)*2; v[2][1]=2.0; v[2][2]=i*2;
	  calcNormal(v,w);
	  glNormal3fv(w);

	  glTexCoord2f(1,0); 
	  glVertex3f((j+1)*2, 0.0, (i+1)*2);
	  glTexCoord2f(1,1);
	  glVertex3f((j+1)*2, 2.0, (i+1)*2);
	  glTexCoord2f(0,1);
	  glVertex3f((j+1)*2, 2.0, i*2);
	  glTexCoord2f(0,0); 
	  glVertex3f((j+1)*2, 0.0, i*2);
      
	  v[0][0]=(j+1)*2; v[0][1]=0.0; v[0][2]=i*2;
	  v[1][0]=(j+1)*2; v[1][1]=2.0; v[1][2]=i*2;
	  v[2][0]=j*2; v[2][1]=2.0; v[2][2]=i*2;
	  calcNormal(v,w);
	  glNormal3fv(w);

	  glTexCoord2f(0,0);
	  glVertex3f((j+1)*2, 0.0, i*2);
	  glTexCoord2f(0,1);
	  glVertex3f((j+1)*2, 2.0, i*2);
	  glTexCoord2f(1,1);
	  glVertex3f(j*2, 2.0, i*2);
	  glTexCoord2f(1,0);  						 
	  glVertex3f(j*2, 0.0, i*2);
	glEnd();
}


void DrawExit(int i, int j) {
	glBindTexture(GL_TEXTURE_2D,Texture[TX_EXIT]);
	glBegin(GL_QUADS);
	  		
	  v[0][0]=j*2; v[0][1]=0.0; v[0][2]=i*2;
	  v[1][0]=j*2; v[1][1]=2.0; v[1][2]=i*2;
	  v[2][0]=j*2; v[2][1]=2.0; v[2][2]=(i+1)*2;
	  calcNormal(v,w);
	  glNormal3fv(w);
	  
	  glTexCoord2f(1,0); 					  
	  glVertex3f(j*2, 0.0, i*2);
	  glTexCoord2f(1,1);
	  glVertex3f(j*2, 2.0, i*2);
	  glTexCoord2f(0,1);
	  glVertex3f(j*2, 2.0, (i+1)*2);						  
	  glTexCoord2f(0,0); 
	  glVertex3f(j*2, 0.0, (i+1)*2);
	  
	  v[0][0]=j*2; v[0][1]=0.0; v[0][2]=(i+1)*2;
	  v[1][0]=j*2; v[1][1]=2.0; v[1][2]=(i+1)*2;
	  v[2][0]=(j+1)*2; v[2][1]=2.0; v[2][2]=(i+1)*2;
	  calcNormal(v,w);
	  glNormal3fv(w);

	  glTexCoord2f(1,0);  
	  glVertex3f(j*2, 0.0, (i+1)*2);
	  glTexCoord2f(1,1);
	  glVertex3f(j*2, 2.0, (i+1)*2);
	  glTexCoord2f(0,1);
	  glVertex3f((j+1)*2, 2.0, (i+1)*2);
	  glTexCoord2f(0,0);  
	  glVertex3f((j+1)*2, 0.0, (i+1)*2);
      
	  v[0][0]=(j+1)*2; v[0][1]=0.0; v[0][2]=(i+1)*2;
	  v[1][0]=(j+1)*2; v[1][1]=2.0; v[1][2]=(i+1)*2;
	  v[2][0]=(j+1)*2; v[2][1]=2.0; v[2][2]=i*2;
	  calcNormal(v,w);
	  glNormal3fv(w);

	  glTexCoord2f(1,0); 
	  glVertex3f((j+1)*2, 0.0, (i+1)*2);
	  glTexCoord2f(1,1);
	  glVertex3f((j+1)*2, 2.0, (i+1)*2);
	  glTexCoord2f(0,1);
	  glVertex3f((j+1)*2, 2.0, i*2);
	  glTexCoord2f(0,0); 
	  glVertex3f((j+1)*2, 0.0, i*2);
      
	  v[0][0]=(j+1)*2; v[0][1]=0.0; v[0][2]=i*2;
	  v[1][0]=(j+1)*2; v[1][1]=2.0; v[1][2]=i*2;
	  v[2][0]=j*2; v[2][1]=2.0; v[2][2]=i*2;
	  calcNormal(v,w);
	  glNormal3fv(w);

	  glTexCoord2f(0,0);
	  glVertex3f((j+1)*2, 0.0, i*2);
	  glTexCoord2f(0,1);
	  glVertex3f((j+1)*2, 2.0, i*2);
	  glTexCoord2f(1,1);
	  glVertex3f(j*2, 2.0, i*2);
	  glTexCoord2f(1,0);  						 
	  glVertex3f(j*2, 0.0, i*2);
	glEnd();
}


void DrawTeleport(int i, int j){
	glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glNormal3f(0,0,1);
		glTranslatef(j*2+1,0.75,i*2+1);
		glRotatef(-player.dir_around,0,1,0);
		glRotatef(r*8,0,0,1);
		glBindTexture(GL_TEXTURE_2D,Texture[TX_TELEPORT]);
		glBegin(GL_QUADS);
		  glTexCoord2f(1,0); 					  
		  glVertex3f(-1, -1,0);
		  glTexCoord2f(1,1);
		  glVertex3f(-1, 1,0);
		  glTexCoord2f(0,1);
		  glVertex3f(1, 1,0);						  
		  glTexCoord2f(0,0); 
		  glVertex3f(1, -1,0);
		glEnd();
		glLoadIdentity();
		glMatrixMode(GL_PROJECTION);
}
void DrawMenu(){
	glEnable2D();
	glDisable(GL_LIGHTING);
	glDisable(GL_CULL_FACE);
	glDisable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glBlendFunc( GL_ONE,GL_ONE_MINUS_SRC_COLOR );
	switch(menu_state){
		case MN_MAIN:
			switch(menu_select){
				case MS_NEW:
					glBindTexture(GL_TEXTURE_2D,Marks[0]);
					glBegin(GL_QUADS);
						glTexCoord2f(0,0);
						glVertex2i(GetSystemMetrics(SM_CXSCREEN)/2-126+28,GetSystemMetrics(SM_CYSCREEN)/2+196-109);
						glTexCoord2f(0,1);
						glVertex2i(GetSystemMetrics(SM_CXSCREEN)/2-126+28,GetSystemMetrics(SM_CYSCREEN)/2+196-109-58);
						glTexCoord2f(1,1);
						glVertex2i(GetSystemMetrics(SM_CXSCREEN)/2-126+28+200,GetSystemMetrics(SM_CYSCREEN)/2+196-109-58);
						glTexCoord2f(1,0);
						glVertex2i(GetSystemMetrics(SM_CXSCREEN)/2-126+28+200,GetSystemMetrics(SM_CYSCREEN)/2+196-109);
					glEnd();
					break;
				case MS_LOAD:
					glBindTexture(GL_TEXTURE_2D,Marks[1]);
					glBegin(GL_QUADS);
						glTexCoord2f(0,0);
						glVertex2i(GetSystemMetrics(SM_CXSCREEN)/2-126+72,GetSystemMetrics(SM_CYSCREEN)/2+196-170);
						glTexCoord2f(0,1);
						glVertex2i(GetSystemMetrics(SM_CXSCREEN)/2-126+72,GetSystemMetrics(SM_CYSCREEN)/2+196-170-66);
						glTexCoord2f(1,1);
						glVertex2i(GetSystemMetrics(SM_CXSCREEN)/2-126+72+107,GetSystemMetrics(SM_CYSCREEN)/2+196-170-66);
						glTexCoord2f(1,0);
						glVertex2i(GetSystemMetrics(SM_CXSCREEN)/2-126+72+107,GetSystemMetrics(SM_CYSCREEN)/2+196-170);
					glEnd();
					break;
				case MS_SAVE:
					glBindTexture(GL_TEXTURE_2D,Marks[2]);
					glBegin(GL_QUADS);
						glTexCoord2f(0,0);
						glVertex2i(GetSystemMetrics(SM_CXSCREEN)/2-126+81,GetSystemMetrics(SM_CYSCREEN)/2+196-243);
						glTexCoord2f(0,1);
						glVertex2i(GetSystemMetrics(SM_CXSCREEN)/2-126+81,GetSystemMetrics(SM_CYSCREEN)/2+196-243-53);
						glTexCoord2f(1,1);
						glVertex2i(GetSystemMetrics(SM_CXSCREEN)/2-126+81+90,GetSystemMetrics(SM_CYSCREEN)/2+196-243-53);
						glTexCoord2f(1,0);
						glVertex2i(GetSystemMetrics(SM_CXSCREEN)/2-126+81+90,GetSystemMetrics(SM_CYSCREEN)/2+196-243);
					glEnd();
					break;
				case MS_EXIT:
					glBindTexture(GL_TEXTURE_2D,Marks[3]);
					glBegin(GL_QUADS);
						glTexCoord2f(0,0);
						glVertex2i(GetSystemMetrics(SM_CXSCREEN)/2-126+80,GetSystemMetrics(SM_CYSCREEN)/2+196-313);
						glTexCoord2f(0,1);
						glVertex2i(GetSystemMetrics(SM_CXSCREEN)/2-126+80,GetSystemMetrics(SM_CYSCREEN)/2+196-313-56);
						glTexCoord2f(1,1);
						glVertex2i(GetSystemMetrics(SM_CXSCREEN)/2-126+80+87,GetSystemMetrics(SM_CYSCREEN)/2+196-313-56);
						glTexCoord2f(1,0);
						glVertex2i(GetSystemMetrics(SM_CXSCREEN)/2-126+80+87,GetSystemMetrics(SM_CYSCREEN)/2+196-313);
					glEnd();
					break;
			}
			glBindTexture(GL_TEXTURE_2D,Texture[TX_MAIN_MENU]);
			glBegin(GL_QUADS);
				glTexCoord2f(0,0);
				glVertex2i(GetSystemMetrics(SM_CXSCREEN)/2-126,GetSystemMetrics(SM_CYSCREEN)/2+196);
				glTexCoord2f(0,1);
				glVertex2i(GetSystemMetrics(SM_CXSCREEN)/2-126,GetSystemMetrics(SM_CYSCREEN)/2-196);
				glTexCoord2f(1,1);
				glVertex2i(GetSystemMetrics(SM_CXSCREEN)/2+127,GetSystemMetrics(SM_CYSCREEN)/2-196);
				glTexCoord2f(1,0);
				glVertex2i(GetSystemMetrics(SM_CXSCREEN)/2+127,GetSystemMetrics(SM_CYSCREEN)/2+196);
			glEnd();
			break;
		case MN_LOAD:
		case MN_SAVE:
			glBindTexture(GL_TEXTURE_2D,Thumbs[menu_select]);
			if (Thumbs[menu_select]==0) glBindTexture(GL_TEXTURE_2D,Texture[TX_EMPTY]);
			glBegin(GL_QUADS);
				glTexCoord2f(0,1);
				glVertex2i(GetSystemMetrics(SM_CXSCREEN)/2-126+25,GetSystemMetrics(SM_CYSCREEN)/2+196-28-150);
				glTexCoord2f(0,0);
				glVertex2i(GetSystemMetrics(SM_CXSCREEN)/2-126+25,GetSystemMetrics(SM_CYSCREEN)/2+196-28);
				glTexCoord2f(1,0);
				glVertex2i(GetSystemMetrics(SM_CXSCREEN)/2-126+25+200,GetSystemMetrics(SM_CYSCREEN)/2+196-28);
				glTexCoord2f(1,1);
				glVertex2i(GetSystemMetrics(SM_CXSCREEN)/2-126+25+200,GetSystemMetrics(SM_CYSCREEN)/2+196-28-150);
			glEnd();
			for (int i=0; i<9; i++){
				glBindTexture(GL_TEXTURE_2D,Texture[TX_DOT]);
				if(i==menu_select)glBindTexture(GL_TEXTURE_2D,Texture[TX_DOT_GLOW]);
				glBegin(GL_QUADS);
					glTexCoord2f(0,0);
					glVertex2i(GetSystemMetrics(SM_CXSCREEN)/2-126+45+66*(i%3),GetSystemMetrics(SM_CYSCREEN)/2+196-215-66*(i/3)+30);
					glTexCoord2f(0,1);
					glVertex2i(GetSystemMetrics(SM_CXSCREEN)/2-126+45+66*(i%3),GetSystemMetrics(SM_CYSCREEN)/2+196-215-66*(i/3));
					glTexCoord2f(1,1);
					glVertex2i(GetSystemMetrics(SM_CXSCREEN)/2-127+45+66*(i%3)+30,GetSystemMetrics(SM_CYSCREEN)/2+196-215-66*(i/3));
					glTexCoord2f(1,0);
					glVertex2i(GetSystemMetrics(SM_CXSCREEN)/2-127+45+66*(i%3)+30,GetSystemMetrics(SM_CYSCREEN)/2+196-215-66*(i/3)+30);
				glEnd();
			}
			glBindTexture(GL_TEXTURE_2D,Texture[TX_MENU_PANEL]);
			glBegin(GL_QUADS);
				glTexCoord2f(0,0);
				glVertex2i(GetSystemMetrics(SM_CXSCREEN)/2-126,GetSystemMetrics(SM_CYSCREEN)/2+196);
				glTexCoord2f(0,1);
				glVertex2i(GetSystemMetrics(SM_CXSCREEN)/2-126,GetSystemMetrics(SM_CYSCREEN)/2-196);
				glTexCoord2f(1,1);
				glVertex2i(GetSystemMetrics(SM_CXSCREEN)/2+127,GetSystemMetrics(SM_CYSCREEN)/2-196);
				glTexCoord2f(1,0);
				glVertex2i(GetSystemMetrics(SM_CXSCREEN)/2+127,GetSystemMetrics(SM_CYSCREEN)/2+196);
			glEnd();
			break;
	}
	glEnable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glDisable2D();
}
void menusim(){
	switch(menu_state){
		case MN_MAIN:
			if(escape_hit && level.loaded){
				menu_on=!menu_on;
			}
			if (space_hit){
				switch(menu_select){
					case MS_NEW:
						level.Load(0);
						level.BuildWindMap();
						player.health=99;
						player.ammo=50;
						player.death=0;
						player.immobileized=false;
						player.world_poz_x=-level.begin.y*2-1;
						player.world_poz_z=-level.begin.x*2-1;
						player.level_poz_x= abs(player.world_poz_x/2);
						player.level_poz_y= abs(player.world_poz_z/2);
						menu_on=!menu_on;
						break;
					case MS_LOAD:
						Thumbs[0] = SOIL_load_OGL_texture("save/ss0.bmp",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
						Thumbs[1] = SOIL_load_OGL_texture("save/ss1.bmp",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
						Thumbs[2] = SOIL_load_OGL_texture("save/ss2.bmp",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
						Thumbs[3] = SOIL_load_OGL_texture("save/ss3.bmp",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
						Thumbs[4] = SOIL_load_OGL_texture("save/ss4.bmp",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
						Thumbs[5] = SOIL_load_OGL_texture("save/ss5.bmp",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
						Thumbs[6] = SOIL_load_OGL_texture("save/ss6.bmp",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
						Thumbs[7] = SOIL_load_OGL_texture("save/ss7.bmp",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
						Thumbs[8] = SOIL_load_OGL_texture("save/ss8.bmp",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
						menu_state=MN_LOAD;
						menu_select=0;
						break;
					case MS_SAVE:
						Thumbs[0] = SOIL_load_OGL_texture("save/ss0.bmp",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
						Thumbs[1] = SOIL_load_OGL_texture("save/ss1.bmp",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
						Thumbs[2] = SOIL_load_OGL_texture("save/ss2.bmp",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
						Thumbs[3] = SOIL_load_OGL_texture("save/ss3.bmp",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
						Thumbs[4] = SOIL_load_OGL_texture("save/ss4.bmp",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
						Thumbs[5] = SOIL_load_OGL_texture("save/ss5.bmp",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
						Thumbs[6] = SOIL_load_OGL_texture("save/ss6.bmp",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
						Thumbs[7] = SOIL_load_OGL_texture("save/ss7.bmp",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
						Thumbs[8] = SOIL_load_OGL_texture("save/ss8.bmp",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
						menu_state=MN_SAVE;
						menu_select=0;
						break;
					case MS_EXIT:
						exit(0);
						break;
				}
			}
			break;
		case MN_LOAD:
			if(escape_hit){
				menu_state=MN_MAIN;
				menu_select=MS_LOAD;
			}
			if (space_hit){
				player.health=99;
				player.ammo=50;
				player.death=0;
				player.immobileized=false;
				std::stringstream filename;
				filename<<"save/gam"<<menu_select<<".sav";
				std::ifstream save(filename.str().c_str());
				save.read((char*)&level.number,sizeof(level.number));
				///*save.read((char*)&player.ammo,sizeof(player.ammo));
				//save.read((char*)&player.death,sizeof(player.death));
				//save.read((char*)&player.dir_around,sizeof(player.dir_around));
				//save.read((char*)&player.dir_updown,sizeof(player.dir_updown));
				//save.read((char*)&player.health,sizeof(player.health));
				//save.read((char*)&player.immobileized,sizeof(player.immobileized));
				//for (int i=0; i<5; i++) save.read((char*)&player.keys[i],sizeof(player.keys[i]));
				//save.read((char*)&player.level_poz_x,sizeof(player.level_poz_x));
				//save.read((char*)&player.level_poz_y,sizeof(player.level_poz_y));
				//save.read((char*)&player.new_level_poz_x,sizeof(player.new_level_poz_x));
				//save.read((char*)&player.new_level_poz_y,sizeof(player.new_level_poz_y));
				//save.read((char*)&player.new_world_poz_x,sizeof(player.new_world_poz_x));
				//save.read((char*)&player.new_world_poz_z,sizeof(player.new_world_poz_z));
				//save.read((char*)&player.points,sizeof(player.points));
				//save.read((char*)&player.step,sizeof(player.step));
				//save.read((char*)&player.world_poz_x,sizeof(player.world_poz_x));
				//save.read((char*)&player.world_poz_z,sizeof(player.world_poz_z));

				//save.read((char*)&level.active_text,sizeof(level.active_text));
				//save.read((char*)&level.begin.x,sizeof(level.begin.x));
				//save.read((char*)&level.begin.y,sizeof(level.begin.y));
				//save.read((char*)&level.closest_lamp,sizeof(level.closest_lamp));
				//save.read((char*)&level.height,sizeof(level.height));
				//save.read((char*)&level.width,sizeof(level.width));
				//save.read((char*)&level.lamp_count,sizeof(level.lamp_count));
				//level.lamp_count=0;
				//level.lamp[level.lamp_count].x=0;
				//save.read((char*)&level.num_of_doors,sizeof(level.num_of_doors));
				//level.num_of_doors=0;
				//save.read((char*)&level.num_of_fans,sizeof(level.num_of_fans));
				//level.num_of_fans=0;
				//level.num_of_monsters=0;
				//*save.read((char*)&level.num_of_monsters,sizeof(level.num_of_monsters));
				//for (int i=0; i<level.num_of_monsters; i++) {
				//	level.Monsters[i] = new Monster(0,0);
				//	save.read((char*)&level.Monsters[i]->MType,sizeof(level.Monsters[i]->MType));
				//	save.read((char*)&level.Monsters[i]->MType,sizeof(level.Monsters[i]->MType));
				//	save.read((char*)&level.Monsters[i]->world_poz_x,sizeof(level.Monsters[i]->world_poz_x));
				//	save.read((char*)&level.Monsters[i]->world_poz_y,sizeof(level.Monsters[i]->world_poz_y));
				//	save.read((char*)&level.Monsters[i]->old_x_poz,sizeof(level.Monsters[i]->old_x_poz));
				//	save.read((char*)&level.Monsters[i]->old_y_poz,sizeof(level.Monsters[i]->old_y_poz));
				//	save.read((char*)&level.Monsters[i]->delay,sizeof(level.Monsters[i]->delay));
				//	save.read((char*)&level.Monsters[i]->attack_delay,sizeof(level.Monsters[i]->attack_delay));
				//	save.read((char*)&level.Monsters[i]->step,sizeof(level.Monsters[i]->step));
				//	save.read((char*)&level.Monsters[i]->health,sizeof(level.Monsters[i]->health));
				//	save.read((char*)&level.Monsters[i]->dead,sizeof(level.Monsters[i]->dead));
				//	save.read((char*)&level.Monsters[i]->x_poz,sizeof(level.Monsters[i]->x_poz));
				//	save.read((char*)&level.Monsters[i]->y_poz,sizeof(level.Monsters[i]->y_poz));
				//}*/
				//save.read((char*)&level.num_of_swtches,sizeof(level.num_of_swtches));
				//level.num_of_swtches=0;
				//save.read((char*)&level.title,sizeof(level.title));
				//save.read((char*)&level.width,sizeof(level.width));
				//for (int i=0; i<200; i++){
				//	for (int j=0; j<200; j++){
				//		save.read((char*)&level.Obstacle[i][j],sizeof(level.Obstacle[i][j]));
				//	}
				//}
				//for (int i=0; i<200; i++){
				//	for (int j=0; j<200; j++){
				//		save.read((char*)&level.WindMap[i][j].east_wind,sizeof(level.WindMap[i][j].east_wind));
				//		save.read((char*)&level.WindMap[i][j].north_wind,sizeof(level.WindMap[i][j].north_wind));
				//		save.read((char*)&level.WindMap[i][j].south_wind,sizeof(level.WindMap[i][j].south_wind));
				//		save.read((char*)&level.WindMap[i][j].west_wind,sizeof(level.WindMap[i][j].west_wind));
				//	}
				//}
				//for (int i=0; i<level.height; i++){
				//	for (int j=0; j<level.width; j++){
				//		Brick probe;
				//		save.read((char*)&probe,sizeof(probe));
				//		switch(probe){
				//			case BR_BLANK:
				//				level.Building[i][j] = new Element(BR_BLANK, i, j);
				//				break;
				//			case BR_FLOOR:
				//				level.Building[i][j] = new Element(BR_FLOOR, i, j);
				//				break;
				//			case BR_WALL:
				//				level.Building[i][j] = new Element(BR_WALL, i, j);
				//				break;
				//			case BR_WINDOW:
				//				level.Building[i][j] = (Element*) new Window(i,j);
				//				save.read((char*)&((Window*)level.Building[i][j])->Orient,sizeof(((Window*)level.Building[i][j])->Orient));
				//				save.read((char*)&((Window*)level.Building[i][j])->x_poz,sizeof(((Window*)level.Building[i][j])->x_poz));
				//				save.read((char*)&((Window*)level.Building[i][j])->y_poz,sizeof(((Window*)level.Building[i][j])->y_poz));
				//				break;
				//			case BR_FAN:
				//				level.Building[i][j] = (Element*) new Fan(i,j);
				//				save.read((char*)&((Fan*)level.Building[i][j])->Dir,sizeof(((Fan*)level.Building[i][j])->Dir));
				//				save.read((char*)&((Fan*)level.Building[i][j])->isOn,sizeof(((Fan*)level.Building[i][j])->isOn));
				//				save.read((char*)&((Fan*)level.Building[i][j])->r,sizeof(((Fan*)level.Building[i][j])->r));
				//				save.read((char*)&((Fan*)level.Building[i][j])->Strength,sizeof(((Fan*)level.Building[i][j])->Strength));
				//				save.read((char*)&((Fan*)level.Building[i][j])->SwitchID_0,sizeof(((Fan*)level.Building[i][j])->SwitchID_0));
				//				save.read((char*)&((Fan*)level.Building[i][j])->SwitchID_1,sizeof(((Fan*)level.Building[i][j])->SwitchID_1));
				//				save.read((char*)&((Fan*)level.Building[i][j])->SwitchID_2,sizeof(((Fan*)level.Building[i][j])->SwitchID_2));
				//				save.read((char*)&((Fan*)level.Building[i][j])->SwitchID_3,sizeof(((Fan*)level.Building[i][j])->SwitchID_3));
				//				save.read((char*)&((Fan*)level.Building[i][j])->SwitchID_4,sizeof(((Fan*)level.Building[i][j])->SwitchID_4));
				//				save.read((char*)&((Fan*)level.Building[i][j])->x_poz,sizeof(((Fan*)level.Building[i][j])->x_poz));
				//				save.read((char*)&((Fan*)level.Building[i][j])->y_poz,sizeof(((Fan*)level.Building[i][j])->y_poz));
				//				break;
				//			case BR_LAMP:
				//				level.Building[i][j] = new Element(BR_LAMP, i, j);
				//				level.lamp[level.lamp_count].x=level.Building[i][j]->y_poz*2+1;
				//				level.lamp[level.lamp_count].y=level.Building[i][j]->x_poz*2+1;
				//				level.lamp_count++;
				//				break;
				//			case BR_LEVUP:
				//				level.Building[i][j] = new Element(BR_LEVUP, i, j);
				//				break;
				//			case BR_LIFE:
				//				level.Building[i][j] = new Element(BR_LIFE, i, j);
				//				break;
				//			case BR_AMMO:
				//				level.Building[i][j] = new Element(BR_AMMO, i, j);
				//				break;
				//			case BR_POINT:
				//				level.Building[i][j] = new Element(BR_POINT, i, j);
				//				break;
				//			case BR_SPIKES:
				//				level.Building[i][j] = (Element*) new Spikes(i,j);
				//				save.read((char*)&((Spikes*)level.Building[i][j])->Delay,sizeof(((Spikes*)level.Building[i][j])->Delay));
				//				save.read((char*)&((Spikes*)level.Building[i][j])->HidePeriod,sizeof(((Spikes*)level.Building[i][j])->HidePeriod));
				//				save.read((char*)&((Spikes*)level.Building[i][j])->r,sizeof(((Spikes*)level.Building[i][j])->r));
				//				save.read((char*)&((Spikes*)level.Building[i][j])->state,sizeof(((Spikes*)level.Building[i][j])->state));
				//				save.read((char*)&((Spikes*)level.Building[i][j])->UpPeriod,sizeof(((Spikes*)level.Building[i][j])->UpPeriod));
				//				save.read((char*)&((Spikes*)level.Building[i][j])->x_poz,sizeof(((Spikes*)level.Building[i][j])->x_poz));
				//				save.read((char*)&((Spikes*)level.Building[i][j])->y_poz,sizeof(((Spikes*)level.Building[i][j])->y_poz));
				//				for (int h=0; h<40; h++) {
				//					save.read((char*)&((Spikes*)level.Building[i][j])->spikes[h].x,sizeof(((Spikes*)level.Building[i][j])->spikes[h].x));
				//					save.read((char*)&((Spikes*)level.Building[i][j])->spikes[h].y,sizeof(((Spikes*)level.Building[i][j])->spikes[h].y));
				//					save.read((char*)&((Spikes*)level.Building[i][j])->spikesize[h],sizeof(((Spikes*)level.Building[i][j])->spikesize[h]));
				//				}
				//				break;
				//			case BR_TELEPORT:
				//				level.Building[i][j] = (Element*) new Teleport(i,j);
				//				save.read((char*)&((Teleport*)level.Building[i][j])->x_des,sizeof(((Teleport*)level.Building[i][j])->x_des));
				//				save.read((char*)&((Teleport*)level.Building[i][j])->x_poz,sizeof(((Teleport*)level.Building[i][j])->x_poz));
				//				save.read((char*)&((Teleport*)level.Building[i][j])->y_des,sizeof(((Teleport*)level.Building[i][j])->y_des));
				//				save.read((char*)&((Teleport*)level.Building[i][j])->y_poz,sizeof(((Teleport*)level.Building[i][j])->y_poz));
				//				break;
				//			case BR_KEY:
				//				level.Building[i][j] = (Element*) new Key(i,j);
				//				save.read((char*)&((Key*)level.Building[i][j])->Key_Type,sizeof(((Key*)level.Building[i][j])->Key_Type));
				//				save.read((char*)&((Key*)level.Building[i][j])->x_poz,sizeof(((Key*)level.Building[i][j])->x_poz));
				//				save.read((char*)&((Key*)level.Building[i][j])->y_poz,sizeof(((Key*)level.Building[i][j])->y_poz));
				//				break;
				//			case BR_BOX:
				//				level.Building[i][j] = (Element*) new Box(i,j);
				//				save.read((char*)&((Box*)level.Building[i][j])->Content,sizeof(((Box*)level.Building[i][j])->Content));
				//				save.read((char*)&((Box*)level.Building[i][j])->x_poz,sizeof(((Box*)level.Building[i][j])->x_poz));
				//				save.read((char*)&((Box*)level.Building[i][j])->y_poz,sizeof(((Box*)level.Building[i][j])->y_poz));
				//				break;
				//			case BR_SWITCH:
				//				level.Switches[level.num_of_swtches] = new Switch(i,j);
				//				level.Building[i][j] = (Element*) level.Switches[level.num_of_swtches];
				//				level.num_of_swtches++;
				//				save.read((char*)&((Switch*)level.Building[i][j])->ID,sizeof(((Switch*)level.Building[i][j])->ID));
				//				save.read((char*)&((Switch*)level.Building[i][j])->isOn,sizeof(((Switch*)level.Building[i][j])->isOn));
				//				save.read((char*)&((Switch*)level.Building[i][j])->x_poz,sizeof(((Switch*)level.Building[i][j])->x_poz));
				//				save.read((char*)&((Switch*)level.Building[i][j])->y_poz,sizeof(((Switch*)level.Building[i][j])->y_poz));
				//				break;
				//			case BR_DOOR:
				//				level.Doors[level.num_of_doors] = new Door(i,j);
				//				level.Building[i][j] = (Element*) level.Doors[level.num_of_doors];
				//				level.num_of_doors++;
				//				save.read((char*)&((Door*)level.Building[i][j])->Door_State,sizeof(((Door*)level.Building[i][j])->Door_State));
				//				save.read((char*)&((Door*)level.Building[i][j])->Door_Type,sizeof(((Door*)level.Building[i][j])->Door_Type));
				//				save.read((char*)&((Door*)level.Building[i][j])->gap,sizeof(((Door*)level.Building[i][j])->gap));
				//				save.read((char*)&((Door*)level.Building[i][j])->hasLock,sizeof(((Door*)level.Building[i][j])->hasLock));
				//				save.read((char*)&((Door*)level.Building[i][j])->isOpen,sizeof(((Door*)level.Building[i][j])->isOpen));
				//				save.read((char*)&((Door*)level.Building[i][j])->LockType,sizeof(((Door*)level.Building[i][j])->LockType));
				//				save.read((char*)&((Door*)level.Building[i][j])->Orient,sizeof(((Door*)level.Building[i][j])->Orient));
				//				save.read((char*)&((Door*)level.Building[i][j])->SwitchID_0,sizeof(((Door*)level.Building[i][j])->SwitchID_0));
				//				save.read((char*)&((Door*)level.Building[i][j])->SwitchID_1,sizeof(((Door*)level.Building[i][j])->SwitchID_1));
				//				save.read((char*)&((Door*)level.Building[i][j])->SwitchID_2,sizeof(((Door*)level.Building[i][j])->SwitchID_2));
				//				save.read((char*)&((Door*)level.Building[i][j])->SwitchID_3,sizeof(((Door*)level.Building[i][j])->SwitchID_3));
				//				save.read((char*)&((Door*)level.Building[i][j])->SwitchID_4,sizeof(((Door*)level.Building[i][j])->SwitchID_4));
				//				save.read((char*)&((Door*)level.Building[i][j])->x_poz,sizeof(((Door*)level.Building[i][j])->x_poz));
				//				save.read((char*)&((Door*)level.Building[i][j])->y_poz,sizeof(((Door*)level.Building[i][j])->y_poz));
				//				break;
				//		}
				//	}
				//}
				save.close();
				level.Load(level.number);
				level.BuildWindMap();
				player.world_poz_x=-level.begin.y*2-1;
				player.world_poz_z=-level.begin.x*2-1;
				player.level_poz_x= abs(player.world_poz_x/2);
				player.level_poz_y= abs(player.world_poz_z/2);
				menu_state=MN_MAIN;
				menu_on=false;
			}
			break;
		case MN_SAVE:
			if(escape_hit){
				menu_state=MN_MAIN;
				menu_select=MS_SAVE;
			}
			if (space_hit){
				std::stringstream filename;
				filename<<"save/gam"<<menu_select<<".sav";
				std::ofstream save(filename.str().c_str());
				save.write((char*)&level.number,sizeof(level.number));
				///*save.write((char*)&player.ammo,sizeof(player.ammo));
				//save.write((char*)&player.death,sizeof(player.death));
				//save.write((char*)&player.dir_around,sizeof(player.dir_around));
				//save.write((char*)&player.dir_updown,sizeof(player.dir_updown));
				//save.write((char*)&player.health,sizeof(player.health));
				//save.write((char*)&player.immobileized,sizeof(player.immobileized));
				//for (int i=0; i<5; i++) save.write((char*)&player.keys[i],sizeof(player.keys[i]));
				//save.write((char*)&player.level_poz_x,sizeof(player.level_poz_x));
				//save.write((char*)&player.level_poz_y,sizeof(player.level_poz_y));
				//save.write((char*)&player.new_level_poz_x,sizeof(player.new_level_poz_x));
				//save.write((char*)&player.new_level_poz_y,sizeof(player.new_level_poz_y));
				//save.write((char*)&player.new_world_poz_x,sizeof(player.new_world_poz_x));
				//save.write((char*)&player.new_world_poz_z,sizeof(player.new_world_poz_z));
				//save.write((char*)&player.points,sizeof(player.points));
				//save.write((char*)&player.step,sizeof(player.step));
				//save.write((char*)&player.world_poz_x,sizeof(player.world_poz_x));
				//save.write((char*)&player.world_poz_z,sizeof(player.world_poz_z));
				//
				//save.write((char*)&level.active_text,sizeof(level.active_text));
				//save.write((char*)&level.begin.x,sizeof(level.begin.x));
				//save.write((char*)&level.begin.y,sizeof(level.begin.y));
				//save.write((char*)&level.closest_lamp,sizeof(level.closest_lamp));
				//save.write((char*)&level.height,sizeof(level.height));
				//save.write((char*)&level.width,sizeof(level.width));
				//save.write((char*)&level.lamp_count,sizeof(level.lamp_count));
				//save.write((char*)&level.num_of_doors,sizeof(level.num_of_doors));
				//save.write((char*)&level.num_of_fans,sizeof(level.num_of_fans));
				//*save.write((char*)&level.num_of_monsters,sizeof(level.num_of_monsters));
				//for (int i=0; i<level.num_of_monsters; i++) {
				//	save.write((char*)&level.Monsters[i]->MType,sizeof(level.Monsters[i]->MType));
				//	save.write((char*)&level.Monsters[i]->MType,sizeof(level.Monsters[i]->MType));
				//	save.write((char*)&level.Monsters[i]->world_poz_x,sizeof(level.Monsters[i]->world_poz_x));
				//	save.write((char*)&level.Monsters[i]->world_poz_y,sizeof(level.Monsters[i]->world_poz_y));
				//	save.write((char*)&level.Monsters[i]->old_x_poz,sizeof(level.Monsters[i]->old_x_poz));
				//	save.write((char*)&level.Monsters[i]->old_y_poz,sizeof(level.Monsters[i]->old_y_poz));
				//	save.write((char*)&level.Monsters[i]->delay,sizeof(level.Monsters[i]->delay));
				//	save.write((char*)&level.Monsters[i]->attack_delay,sizeof(level.Monsters[i]->attack_delay));
				//	save.write((char*)&level.Monsters[i]->step,sizeof(level.Monsters[i]->step));
				//	save.write((char*)&level.Monsters[i]->health,sizeof(level.Monsters[i]->health));
				//	save.write((char*)&level.Monsters[i]->dead,sizeof(level.Monsters[i]->dead));
				//}*/
				//save.write((char*)&level.num_of_swtches,sizeof(level.num_of_swtches));
				//save.write((char*)&level.title,sizeof(level.title));
				//save.write((char*)&level.width,sizeof(level.width));
				//for (int i=0; i<200; i++){
				//	for (int j=0; j<200; j++){
				//		save.write((char*)&level.Obstacle[i][j],sizeof(level.Obstacle[i][j]));
				//	}
				//}
				//for (int i=0; i<200; i++){
				//	for (int j=0; j<200; j++){
				//		save.write((char*)&level.WindMap[i][j].east_wind,sizeof(level.WindMap[i][j].east_wind));
				//		save.write((char*)&level.WindMap[i][j].north_wind,sizeof(level.WindMap[i][j].north_wind));
				//		save.write((char*)&level.WindMap[i][j].south_wind,sizeof(level.WindMap[i][j].south_wind));
				//		save.write((char*)&level.WindMap[i][j].west_wind,sizeof(level.WindMap[i][j].west_wind));
				//	}
				//}
				//for (int i=0; i<level.height; i++){
				//	for (int j=0; j<level.width; j++){
				//		switch(level.Building[i][j]->type){
				//			case BR_BLANK:
				//				save.write((char*)&level.Building[i][j]->type,sizeof(level.Building[i][j]->type));
				//				break;
				//			case BR_FLOOR:
				//				save.write((char*)&level.Building[i][j]->type,sizeof(level.Building[i][j]->type));
				//				break;
				//			case BR_WALL:
				//				save.write((char*)&level.Building[i][j]->type,sizeof(level.Building[i][j]->type));
				//				break;
				//			case BR_WINDOW:
				//				save.write((char*)&level.Building[i][j]->type,sizeof(level.Building[i][j]->type));
				//				save.write((char*)&((Window*)level.Building[i][j])->Orient,sizeof(((Window*)level.Building[i][j])->Orient));
				//				save.write((char*)&((Window*)level.Building[i][j])->x_poz,sizeof(((Window*)level.Building[i][j])->x_poz));
				//				save.write((char*)&((Window*)level.Building[i][j])->y_poz,sizeof(((Window*)level.Building[i][j])->y_poz));
				//				break;
				//			case BR_FAN:
				//				save.write((char*)&level.Building[i][j]->type,sizeof(level.Building[i][j]->type));
				//				save.write((char*)&((Fan*)level.Building[i][j])->Dir,sizeof(((Fan*)level.Building[i][j])->Dir));
				//				save.write((char*)&((Fan*)level.Building[i][j])->isOn,sizeof(((Fan*)level.Building[i][j])->isOn));
				//				save.write((char*)&((Fan*)level.Building[i][j])->r,sizeof(((Fan*)level.Building[i][j])->r));
				//				save.write((char*)&((Fan*)level.Building[i][j])->Strength,sizeof(((Fan*)level.Building[i][j])->Strength));
				//				save.write((char*)&((Fan*)level.Building[i][j])->SwitchID_0,sizeof(((Fan*)level.Building[i][j])->SwitchID_0));
				//				save.write((char*)&((Fan*)level.Building[i][j])->SwitchID_1,sizeof(((Fan*)level.Building[i][j])->SwitchID_1));
				//				save.write((char*)&((Fan*)level.Building[i][j])->SwitchID_2,sizeof(((Fan*)level.Building[i][j])->SwitchID_2));
				//				save.write((char*)&((Fan*)level.Building[i][j])->SwitchID_3,sizeof(((Fan*)level.Building[i][j])->SwitchID_3));
				//				save.write((char*)&((Fan*)level.Building[i][j])->SwitchID_4,sizeof(((Fan*)level.Building[i][j])->SwitchID_4));
				//				save.write((char*)&((Fan*)level.Building[i][j])->x_poz,sizeof(((Fan*)level.Building[i][j])->x_poz));
				//				save.write((char*)&((Fan*)level.Building[i][j])->y_poz,sizeof(((Fan*)level.Building[i][j])->y_poz));
				//				break;
				//			case BR_LAMP:
				//				save.write((char*)&level.Building[i][j]->type,sizeof(level.Building[i][j]->type));
				//				break;
				//			case BR_LEVUP:
				//				save.write((char*)&level.Building[i][j]->type,sizeof(level.Building[i][j]->type));
				//				break;
				//			case BR_LIFE:
				//				save.write((char*)&level.Building[i][j]->type,sizeof(level.Building[i][j]->type));
				//				break;
				//			case BR_AMMO:
				//				save.write((char*)&level.Building[i][j]->type,sizeof(level.Building[i][j]->type));
				//				break;
				//			case BR_POINT:
				//				save.write((char*)&level.Building[i][j]->type,sizeof(level.Building[i][j]->type));
				//				break;
				//			case BR_SPIKES:
				//				save.write((char*)&level.Building[i][j]->type,sizeof(level.Building[i][j]->type));
				//				save.write((char*)&((Spikes*)level.Building[i][j])->Delay,sizeof(((Spikes*)level.Building[i][j])->Delay));
				//				save.write((char*)&((Spikes*)level.Building[i][j])->HidePeriod,sizeof(((Spikes*)level.Building[i][j])->HidePeriod));
				//				save.write((char*)&((Spikes*)level.Building[i][j])->r,sizeof(((Spikes*)level.Building[i][j])->r));
				//				save.write((char*)&((Spikes*)level.Building[i][j])->state,sizeof(((Spikes*)level.Building[i][j])->state));
				//				save.write((char*)&((Spikes*)level.Building[i][j])->UpPeriod,sizeof(((Spikes*)level.Building[i][j])->UpPeriod));
				//				save.write((char*)&((Spikes*)level.Building[i][j])->x_poz,sizeof(((Spikes*)level.Building[i][j])->x_poz));
				//				save.write((char*)&((Spikes*)level.Building[i][j])->y_poz,sizeof(((Spikes*)level.Building[i][j])->y_poz));
				//				for (int h=0; h<40; h++) {
				//					save.write((char*)&((Spikes*)level.Building[i][j])->spikes[h].x,sizeof(((Spikes*)level.Building[i][j])->spikes[h].x));
				//					save.write((char*)&((Spikes*)level.Building[i][j])->spikes[h].y,sizeof(((Spikes*)level.Building[i][j])->spikes[h].y));
				//					save.write((char*)&((Spikes*)level.Building[i][j])->spikesize[h],sizeof(((Spikes*)level.Building[i][j])->spikesize[h]));
				//				}
				//				break;
				//			case BR_TELEPORT:
				//				save.write((char*)&level.Building[i][j]->type,sizeof(level.Building[i][j]->type));
				//				save.write((char*)&((Teleport*)level.Building[i][j])->x_des,sizeof(((Teleport*)level.Building[i][j])->x_des));
				//				save.write((char*)&((Teleport*)level.Building[i][j])->x_poz,sizeof(((Teleport*)level.Building[i][j])->x_poz));
				//				save.write((char*)&((Teleport*)level.Building[i][j])->y_des,sizeof(((Teleport*)level.Building[i][j])->y_des));
				//				save.write((char*)&((Teleport*)level.Building[i][j])->y_poz,sizeof(((Teleport*)level.Building[i][j])->y_poz));
				//				break;
				//			case BR_KEY:
				//				save.write((char*)&level.Building[i][j]->type,sizeof(level.Building[i][j]->type));
				//				save.write((char*)&((Key*)level.Building[i][j])->Key_Type,sizeof(((Key*)level.Building[i][j])->Key_Type));
				//				save.write((char*)&((Key*)level.Building[i][j])->x_poz,sizeof(((Key*)level.Building[i][j])->x_poz));
				//				save.write((char*)&((Key*)level.Building[i][j])->y_poz,sizeof(((Key*)level.Building[i][j])->y_poz));
				//				break;
				//			case BR_BOX:
				//				save.write((char*)&level.Building[i][j]->type,sizeof(level.Building[i][j]->type));
				//				save.write((char*)&((Box*)level.Building[i][j])->Content,sizeof(((Box*)level.Building[i][j])->Content));
				//				save.write((char*)&((Box*)level.Building[i][j])->x_poz,sizeof(((Box*)level.Building[i][j])->x_poz));
				//				save.write((char*)&((Box*)level.Building[i][j])->y_poz,sizeof(((Box*)level.Building[i][j])->y_poz));
				//				break;
				//			case BR_SWITCH:
				//				save.write((char*)&level.Building[i][j]->type,sizeof(level.Building[i][j]->type));
				//				save.write((char*)&((Switch*)level.Building[i][j])->ID,sizeof(((Switch*)level.Building[i][j])->ID));
				//				save.write((char*)&((Switch*)level.Building[i][j])->isOn,sizeof(((Switch*)level.Building[i][j])->isOn));
				//				save.write((char*)&((Switch*)level.Building[i][j])->x_poz,sizeof(((Switch*)level.Building[i][j])->x_poz));
				//				save.write((char*)&((Switch*)level.Building[i][j])->y_poz,sizeof(((Switch*)level.Building[i][j])->y_poz));
				//				break;
				//			case BR_DOOR:
				//				save.write((char*)&level.Building[i][j]->type,sizeof(level.Building[i][j]->type));
				//				save.write((char*)&((Door*)level.Building[i][j])->Door_State,sizeof(((Door*)level.Building[i][j])->Door_State));
				//				save.write((char*)&((Door*)level.Building[i][j])->Door_Type,sizeof(((Door*)level.Building[i][j])->Door_Type));
				//				save.write((char*)&((Door*)level.Building[i][j])->gap,sizeof(((Door*)level.Building[i][j])->gap));
				//				save.write((char*)&((Door*)level.Building[i][j])->hasLock,sizeof(((Door*)level.Building[i][j])->hasLock));
				//				save.write((char*)&((Door*)level.Building[i][j])->isOpen,sizeof(((Door*)level.Building[i][j])->isOpen));
				//				save.write((char*)&((Door*)level.Building[i][j])->LockType,sizeof(((Door*)level.Building[i][j])->LockType));
				//				save.write((char*)&((Door*)level.Building[i][j])->Orient,sizeof(((Door*)level.Building[i][j])->Orient));
				//				save.write((char*)&((Door*)level.Building[i][j])->SwitchID_0,sizeof(((Door*)level.Building[i][j])->SwitchID_0));
				//				save.write((char*)&((Door*)level.Building[i][j])->SwitchID_1,sizeof(((Door*)level.Building[i][j])->SwitchID_1));
				//				save.write((char*)&((Door*)level.Building[i][j])->SwitchID_2,sizeof(((Door*)level.Building[i][j])->SwitchID_2));
				//				save.write((char*)&((Door*)level.Building[i][j])->SwitchID_3,sizeof(((Door*)level.Building[i][j])->SwitchID_3));
				//				save.write((char*)&((Door*)level.Building[i][j])->SwitchID_4,sizeof(((Door*)level.Building[i][j])->SwitchID_4));
				//				save.write((char*)&((Door*)level.Building[i][j])->x_poz,sizeof(((Door*)level.Building[i][j])->x_poz));
				//				save.write((char*)&((Door*)level.Building[i][j])->y_poz,sizeof(((Door*)level.Building[i][j])->y_poz));
				//				break;
				//		}
				//	}
				//}
				save.close();
				screen_shot=true;
				menu_state=MN_MAIN;
				menu_on=false;
			}
			break;
	}
}
void display() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glFrontFace(GL_CCW);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if(!the_end){
		if(!menu_on){
			glEnable(GL_LIGHTING);
			glEnable(GL_LIGHT0);
			if(level.lamp_count>0) glEnable(GL_LIGHT1);
			else glDisable(GL_LIGHT1);
			glEnable(GL_ALPHA_TEST);
			glAlphaFunc(GL_GREATER,0.5);
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			
			light_position[0]=-player.world_poz_x;
			light_position[2]=-player.world_poz_z;
			
			//glLightModelfv(GL_LIGHT_MODEL_AMBIENT,light_ambient);

			glLightfv(GL_LIGHT0, GL_POSITION, light_position);
			level.closest_lamp=0;
			for (int i=0; i<level.lamp_count; i++){
				if(pow((level.lamp[i].x*2+1)+player.world_poz_z,2)+pow((level.lamp[i].y*2+1)+player.world_poz_x,2)<pow((level.lamp[level.closest_lamp].x*2+1)+player.world_poz_z,2)+pow((level.lamp[level.closest_lamp].y*2+1)+player.world_poz_x,2)){
					level.closest_lamp=i;
				}
			}
			light_position1[0]=level.lamp[level.closest_lamp].y*2+1;
			light_position1[2]=level.lamp[level.closest_lamp].x*2+1;
			glLightfv(GL_LIGHT1, GL_POSITION, light_position1);

			gluPerspective(80, GetSystemMetrics(SM_CXSCREEN)/(float)GetSystemMetrics(SM_CYSCREEN) , 0.001, 1000.0);
			gluLookAt(0, 0, 0, 0, 0, -1, 0.0, 1.0, 0.0);
			glRotatef(player.dir_updown,1,0,0);
			glRotatef(player.dir_around,0,1,0);
			glTranslatef(player.world_poz_x, -0.5+sin(player.step)/32.0+player.death, player.world_poz_z);

			//DrawLamp(14,9);
			//DrawFan(15,8);
			//DrawLife(3,4);
			//DrawAmmo(5,7);
			//DrawPoint(5,8);
			//DrawSpikes(5,9);
			//DrawTeleport(4,4);
			//DrawKey(3,5);
			//DrawChest(5,5);
			//DrawDoor(1,2);
			//------------------------------
			
			//glVertexPointer(3,GL_FLOAT,0,obj);

		/*	
			light_spotdir[0]=-sin((player.dir_around*3.14)/180.0);
			light_spotdir[1]=0;
			light_spotdir[2]=cos((player.dir_around*3.14)/180.0);
		*/
			//glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, light_spotdir);

			//RYSOWANIE POD£OGI
			for (int i=0; i<level.height; i++)
				for (int j=0; j<level.width; j++) {
					if(level.Building[i][j]!=NULL) {
						switch(level.Building[i][j]->type){
							case BR_FLOOR:
								DrawCeiling(i,j);
								DrawFloor(i,j);
								break;
							case BR_WALL:
								DrawWall(i,j);
								break;
							case BR_WINDOW:
								DrawWindow(i,j,((Window*)level.Building[i][j])->Orient);
								break;
							case BR_FAN:
								DrawFan(j,i);
								break;
							case BR_LAMP:
								DrawCeiling(i,j);
								DrawFloor(i,j);
								DrawLamp(j,i);
								break;
							case BR_LEVUP:
								DrawExit(i,j);
								break;
							case BR_LIFE:
								DrawCeiling(i,j);
								DrawLife(j,i);
								DrawFloor(i,j);
								break;
							case BR_AMMO:
								DrawCeiling(i,j);
								DrawAmmo(j,i);
								DrawFloor(i,j);
								break;
							case BR_POINT:
								DrawCeiling(i,j);
								DrawPoint(j,i);
								DrawFloor(i,j);
								break;
							case BR_SPIKES:
								DrawCeiling(i,j);
								DrawSpikes(j,i);
								DrawFloor(i,j);
								break;
							case BR_TELEPORT:
								DrawCeiling(i,j);
								DrawTeleport(i,j);
								DrawFloor(i,j);
								break;
							case BR_KEY:
								DrawCeiling(i,j);
								DrawKey(j,i);
								DrawFloor(i,j);
								break;
							case BR_BOX:
								DrawCeiling(i,j);
								DrawChest(j,i);
								DrawFloor(i,j);
								break;
							case BR_SWITCH:
								DrawCeiling(i,j);
								DrawSwitch(i,j);
								DrawFloor(i,j);
								break;
							case BR_DOOR:
								DrawCeiling(i,j);
								DrawDoor(j,i,((Door*)level.Building[i][j])->gap);
								DrawFloor(i,j);
								break;
						}
					}
				}
			if (lmb_hit) {
				glBegin(GL_LINES);
					glVertex3f(-b_x+0.1,-b_y-0.1,-b_z);
					glVertex3f(-e_x,-e_y,-e_z);
				glEnd();
			}
			//RYSOWANIE POTWORÓW
			DrawMonsters();
			/*DrawNumber(time_delta,400,400);
			DrawNumber(t,500,400);*/
			//RYSOWANIE SZK£A
			for (int i=0; i<level.height; i++)
				for (int j=0; j<level.width; j++) {
					if(level.Building[i][j]!=NULL) {
						if (level.Building[i][j]->type==BR_WINDOW) {
							DrawGlass(i,j,((Window*)level.Building[i][j])->Orient);
						}
					}
				}
			glDisable(GL_LIGHTING);
			glEnable2D();
			glBindTexture(GL_TEXTURE_2D,Texture[TX_TARGET]);
			glBegin(GL_QUADS);
				glTexCoord2f(0,0);
				glVertex2i(GetSystemMetrics(SM_CXSCREEN)/2-16,GetSystemMetrics(SM_CYSCREEN)/2-16);
				glTexCoord2f(0,1);
				glVertex2i(GetSystemMetrics(SM_CXSCREEN)/2-16,GetSystemMetrics(SM_CYSCREEN)/2+16);
				glTexCoord2f(1,1);
				glVertex2i(GetSystemMetrics(SM_CXSCREEN)/2+16,GetSystemMetrics(SM_CYSCREEN)/2+16);
				glTexCoord2f(1,0);
				glVertex2i(GetSystemMetrics(SM_CXSCREEN)/2+16,GetSystemMetrics(SM_CYSCREEN)/2-16);
			glEnd();
			glDisable2D();
			glEnable(GL_LIGHTING);
			DrawPanel();
			if (level.active_text>=0) DrawTxt(level.active_text);
			level.active_text=-1;
			//DrawNumber(1987,100,100);
		} else {
			DrawMenu();
		}
	} else {
		glDisable(GL_LIGHTING);
		glEnable2D();
		glBindTexture(GL_TEXTURE_2D,Texture[TX_END]);
		glBegin(GL_QUADS);
			glTexCoord2f(0,0);
			glVertex2i(GetSystemMetrics(SM_CXSCREEN)/2-138,GetSystemMetrics(SM_CYSCREEN)/2-51);
			glTexCoord2f(0,1);
			glVertex2i(GetSystemMetrics(SM_CXSCREEN)/2-138,GetSystemMetrics(SM_CYSCREEN)/2+51);
			glTexCoord2f(1,1);
			glVertex2i(GetSystemMetrics(SM_CXSCREEN)/2+138,GetSystemMetrics(SM_CYSCREEN)/2+51);
			glTexCoord2f(1,0);
			glVertex2i(GetSystemMetrics(SM_CXSCREEN)/2+138,GetSystemMetrics(SM_CYSCREEN)/2-51);
		glEnd();
		glDisable2D();
		glEnable(GL_LIGHTING);
	}
	glFlush();
	if(screen_shot){
		std::stringstream filename;
		filename<<"save/ss"<<menu_select<<".bmp";
		SOIL_save_screenshot(filename.str().c_str(),SOIL_SAVE_TYPE_BMP,0,0,GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN));
		screen_shot=false;
	}
	glutSwapBuffers();
	r++;
}
void collision(){
	//player.new_world_poz_x+=0.01;
	//player.new_world_poz_z+=0.01;
	player.new_level_poz_x= abs(player.new_world_poz_x/2);
	player.new_level_poz_y= abs(player.new_world_poz_z/2);
	if (player.new_level_poz_x>=0 && player.new_level_poz_y>=0 && player.new_level_poz_x<200 && player.new_level_poz_y<200)
		if(level.Obstacle[player.new_level_poz_y][player.new_level_poz_x]) {
			if (player.new_level_poz_y!=player.level_poz_y && player.new_level_poz_x!=player.level_poz_x) {
				if(!level.Obstacle[player.level_poz_y][player.new_level_poz_x]) {
					player.new_world_poz_z = player.world_poz_z;
				} else
				if(!level.Obstacle[player.new_level_poz_y][player.level_poz_x]) {
					player.new_world_poz_x = player.world_poz_x;
				} else {
					player.new_world_poz_z = player.world_poz_z;
					player.new_world_poz_x = player.world_poz_x;
				}
			} else {
				if(player.new_level_poz_x!=player.level_poz_x ) {
					player.new_world_poz_x = player.world_poz_x;
				} 
				if(player.new_level_poz_y!=player.level_poz_y ) {
					player.new_world_poz_z = player.world_poz_z;
				} 
			}	
		}
	player.world_poz_z=player.new_world_poz_z;
	player.world_poz_x=player.new_world_poz_x;
	player.level_poz_x= abs(player.world_poz_x/2);
	player.level_poz_y= abs(player.world_poz_z/2);
}
void windmove(){
	
	if(level.WindMap[player.level_poz_x][player.level_poz_y].south_wind>0){
		player.new_world_poz_z = player.world_poz_z-level.WindMap[player.level_poz_x][player.level_poz_y].south_wind/80;
		collision();
	}
	if(level.WindMap[player.level_poz_x][player.level_poz_y].north_wind>0){
		player.new_world_poz_z = player.world_poz_z+level.WindMap[player.level_poz_x][player.level_poz_y].north_wind/80;
		collision();
	}
	if(level.WindMap[player.level_poz_x][player.level_poz_y].east_wind>0){
		player.new_world_poz_x = player.world_poz_x-level.WindMap[player.level_poz_x][player.level_poz_y].east_wind/80;
		collision();
	}
	if(level.WindMap[player.level_poz_x][player.level_poz_y].west_wind>0){
		player.new_world_poz_x = player.world_poz_x+level.WindMap[player.level_poz_x][player.level_poz_y].west_wind/80;
		collision();
	}

}
void spikesim(){
	for (int i=0; i<level.height; i++)
		for (int j=0; j<level.width; j++) {
			if(level.Building[i][j]!=NULL) {
				if (level.Building[i][j]->type==BR_SPIKES) {
					((Spikes*)level.Building[i][j])->Delay+=time_delta>20?time_delta:20;
					t1 = ((Spikes*)level.Building[i][j])->HidePeriod;
					t2 = ((Spikes*)level.Building[i][j])->HidePeriod+250;
					t3 = ((Spikes*)level.Building[i][j])->HidePeriod+250+((Spikes*)level.Building[i][j])->UpPeriod;
					t4 = ((Spikes*)level.Building[i][j])->HidePeriod+250+((Spikes*)level.Building[i][j])->UpPeriod+250;
					t=((Spikes*)level.Building[i][j])->Delay%t4;
					if(t>0&&t<t1){
						((Spikes*)level.Building[i][j])->state=S_HIDDEN;
						((Spikes*)level.Building[i][j])->r=-1.25;
					}
					if(t>t1&&t<t2){
						((Spikes*)level.Building[i][j])->state=S_SHOWING;
						((Spikes*)level.Building[i][j])->r=-1.25+1.25*((t-t1)/250.0);
					}
					if(t>t2&&t<t3){
						((Spikes*)level.Building[i][j])->state=S_SHOWN;
						((Spikes*)level.Building[i][j])->r=0;
					}
					if(t>t3&&t<t4){
						((Spikes*)level.Building[i][j])->state=S_HIDING;
						((Spikes*)level.Building[i][j])->r=-1.25*((t-t3)/250.0);
					}
				}
			}
		}
}
void monstersim(){
	for (int i=0; i<level.num_of_monsters; i++){
		if (level.Monsters[i]->MState!=M_DYING && level.Monsters[i]->MState!=M_DEAD) {
			if(level.Monsters[i]->delay==0) {
				if (level.Monsters[i]->step==0) {
					int x=level.Monsters[i]->old_x_poz=level.Monsters[i]->x_poz;
					int y=level.Monsters[i]->old_y_poz=level.Monsters[i]->y_poz;
					do {
						x = level.Monsters[i]->x_poz + rand()%3-1;
						y = level.Monsters[i]->y_poz + rand()%3-1;
					} while(level.Building[x][y]->type!=BR_FLOOR);
					level.Monsters[i]->x_poz=x;
					level.Monsters[i]->y_poz=y;
					level.Monsters[i]->step=50*level.Monsters[i]->MType+50;
				} else {
					level.Monsters[i]->world_poz_x=level.Monsters[i]->x_poz*2+1-((level.Monsters[i]->x_poz-level.Monsters[i]->old_x_poz)/(float)(50*level.Monsters[i]->MType+50))*level.Monsters[i]->step*2;
					level.Monsters[i]->world_poz_y=level.Monsters[i]->y_poz*2+1-((level.Monsters[i]->y_poz-level.Monsters[i]->old_y_poz)/(float)(50*level.Monsters[i]->MType+50))*level.Monsters[i]->step*2;
					level.Monsters[i]->step--;
					if (level.Monsters[i]->step==0) level.Monsters[i]->delay=0;//50*level.Monsters[i]->MType;
				}
			} else {
				level.Monsters[i]->delay--;
			}
			float d = sqrt(pow(level.Monsters[i]->world_poz_x-abs(player.world_poz_z),2)+pow(level.Monsters[i]->world_poz_y-abs(player.world_poz_x),2));
			if (d<level.Monsters[i]->MType*0.4+1.5){
				switch(level.Monsters[i]->MState){
					case M_LIVE:
						if (level.Monsters[i]->attack_delay==0) {
							level.Monsters[i]->attack_delay=(rand()%50)*level.Monsters[i]->MType+10;
							level.Monsters[i]->MState=M_ATTACK;
							player.immobileized=true;
						} else {
							level.Monsters[i]->attack_delay--;
						}
						break;
					case M_ATTACK:
						if (level.Monsters[i]->attack_delay==0) {
							level.Monsters[i]->attack_delay=(rand()%50)*level.Monsters[i]->MType+75;
							level.Monsters[i]->MState=M_LIVE;
							player.immobileized=false;
						} else {
							level.Monsters[i]->attack_delay--;
							player.health-=0.2;
						}
						break;
				}
			} else {
				if (level.Monsters[i]->MState==M_ATTACK) {
					if (level.Monsters[i]->attack_delay==0) {
							level.Monsters[i]->attack_delay=(rand()%50)*level.Monsters[i]->MType+75;
							level.Monsters[i]->MState=M_LIVE;
							player.immobileized=false;
						} else {
							level.Monsters[i]->attack_delay--;
							player.health-=0.2;
						}
				}
				//level.Monsters[i]->MState=M_LIVE;
			}
			if (level.Monsters[i]->health<0) {
				level.Monsters[i]->MState=M_DYING;
				player.immobileized=false;
			}
		} else {
			if(level.Monsters[i]->MState==M_DYING) {
				level.Monsters[i]->dead+=0.5;
				if (level.Monsters[i]->dead>=90) level.Monsters[i]->MState=M_DEAD;
			}
		}
	}
}
void doorsim(){
	for (int i=0; i<level.num_of_doors; i++) {
		bool token=true;
		if(level.Doors[i]->SwitchID_0!=0){
			level.Doors[i]->isOpen=false;
			bool token=false;
			for (int j=0; j<level.num_of_swtches; j++) {
				if(level.Switches[j]->ID==level.Doors[i]->SwitchID_0 && level.Switches[j]->isOn){
					level.Doors[i]->isOpen=true;
					token=true;
				}
			}
		}
		if(level.Doors[i]->SwitchID_1!=0 && token){
			level.Doors[i]->isOpen=false;
			token=false;
			for (int j=0; j<level.num_of_swtches; j++) {
				if(level.Switches[j]->ID==level.Doors[i]->SwitchID_1 && level.Switches[j]->isOn){
					level.Doors[i]->isOpen=true;
					token=true;
				}
			}
		}
		if(level.Doors[i]->SwitchID_2!=0 && token){
			level.Doors[i]->isOpen=false;
			token=false;
			for (int j=0; j<level.num_of_swtches; j++) {
				if(level.Switches[j]->ID==level.Doors[i]->SwitchID_2 && level.Switches[j]->isOn){
					level.Doors[i]->isOpen=true;
					token=true;
				}
			}
		}
		if(level.Doors[i]->SwitchID_3!=0 && token){
			level.Doors[i]->isOpen=false;
			token=false;
			for (int j=0; j<level.num_of_swtches; j++) {
				if(level.Switches[j]->ID==level.Doors[i]->SwitchID_3 && level.Switches[j]->isOn){
					level.Doors[i]->isOpen=true;
					token=true;
				}
			}
		}
		if(level.Doors[i]->SwitchID_4!=0 && token){
			level.Doors[i]->isOpen=false;
			token=false;
			for (int j=0; j<level.num_of_swtches; j++) {
				if(level.Switches[j]->ID==level.Doors[i]->SwitchID_4 && level.Switches[j]->isOn){
					level.Doors[i]->isOpen=true;
					token=true;
				}
			}
		}
		if (abs(level.Doors[i]->x_poz-player.level_poz_y)<2 && abs(level.Doors[i]->y_poz-player.level_poz_x)<2) {
			if(!level.Doors[i]->isOpen) {
				level.active_text=TXT_SWITCH;
			} else if(level.Doors[i]->hasLock) {
				switch(level.Doors[i]->LockType) {
					case GW_GREEN: level.active_text=TXT_GREEN; break;
					case GW_YELLOW: level.active_text=TXT_YELLOW; break;
					case GW_RED: level.active_text=TXT_RED; break;
					case GW_BLUE: level.active_text=TXT_BLUE; break;
					case GW_WHITE: level.active_text=TXT_WHITE; break;
				}
			}
		}
		if (abs(level.Doors[i]->x_poz-player.level_poz_y)<2 && abs(level.Doors[i]->y_poz-player.level_poz_x)<2 && space_hit && level.Doors[i]->hasLock) {
			if (player.keys[level.Doors[i]->LockType]) {
				player.keys[level.Doors[i]->LockType]=false;
				level.Doors[i]->hasLock=false;

			}
		}
		if (abs(level.Doors[i]->x_poz-player.level_poz_y)<2 && abs(level.Doors[i]->y_poz-player.level_poz_x)<2 && level.Doors[i]->isOpen && !level.Doors[i]->hasLock) {
			if(level.Doors[i]->Door_State!=D_OPEN) level.Doors[i]->Door_State = D_OPENING;
			level.Obstacle[level.Doors[i]->x_poz][level.Doors[i]->y_poz]=false;
		} else {
			if(level.Doors[i]->Door_State!=D_CLOSED) level.Doors[i]->Door_State = D_CLOSING;
			level.Obstacle[level.Doors[i]->x_poz][level.Doors[i]->y_poz]=true;
		}
		switch(level.Doors[i]->Door_State){
			case D_OPENING: if (level.Doors[i]->gap<1) level.Doors[i]->gap+=0.05; else level.Doors[i]->Door_State=D_OPEN; break;
			case D_CLOSING: if (level.Doors[i]->gap>0) level.Doors[i]->gap-=0.05;  else level.Doors[i]->Door_State=D_CLOSED; break;
			case D_OPEN: level.Doors[i]->gap=1; break;
			case D_CLOSED: level.Doors[i]->gap=0; break;
		}
	}
}
void fansim(){
	for (int i=0; i<level.num_of_fans; i++) {
		bool token=true;
		bool original_state = level.Fans[i]->isOn;
		if(level.Fans[i]->SwitchID_0!=0){
			level.Fans[i]->isOn=false;
			token=false;
			for (int j=0; j<level.num_of_swtches; j++) {
				if(level.Switches[j]->ID==level.Fans[i]->SwitchID_0 && level.Switches[j]->isOn){
					level.Fans[i]->isOn=true;
					token=true;
				}
			}
		}
		if(level.Fans[i]->SwitchID_1!=0 && token){
			level.Fans[i]->isOn=false;
			token=false;
			for (int j=0; j<level.num_of_swtches; j++) {
				if(level.Switches[j]->ID==level.Fans[i]->SwitchID_1 && level.Switches[j]->isOn){
					level.Fans[i]->isOn=true;
					token=true;
				}
			}
		}
		if(level.Fans[i]->SwitchID_2!=0 && token){
			level.Fans[i]->isOn=false;
			token=false;
			for (int j=0; j<level.num_of_swtches; j++) {
				if(level.Switches[j]->ID==level.Fans[i]->SwitchID_2 && level.Switches[j]->isOn){
					level.Fans[i]->isOn=true;
					token=true;
				}
			}
		}
		if(level.Fans[i]->SwitchID_3!=0 && token){  
			level.Fans[i]->isOn=false;
			token=false;
			for (int j=0; j<level.num_of_swtches; j++) {
				if(level.Switches[j]->ID==level.Fans[i]->SwitchID_3 && level.Switches[j]->isOn){
					level.Fans[i]->isOn=true;
					token=true;
				}
			}
		}
		if(level.Fans[i]->SwitchID_4!=0 && token){
			level.Fans[i]->isOn=false;
			token=false;
			for (int j=0; j<level.num_of_swtches; j++) {
				if(level.Switches[j]->ID==level.Fans[i]->SwitchID_4 && level.Switches[j]->isOn){
					level.Fans[i]->isOn=true;
					token=true;
				}
			}
		}
		if (original_state!=level.Fans[i]->isOn) level.BuildWindMap(); 
	}
}
void interact(){
	switch(level.Building[player.level_poz_y][player.level_poz_x]->type){
		case BR_LIFE:
			delete level.Building[player.level_poz_y][player.level_poz_x];
			level.Building[player.level_poz_y][player.level_poz_x] = new Element(BR_FLOOR,player.level_poz_y,player.level_poz_x);
			player.health+=15;
			if(player.health>=100) player.health=99;
			break;
		case BR_POINT:
			delete level.Building[player.level_poz_y][player.level_poz_x];
			level.Building[player.level_poz_y][player.level_poz_x] = new Element(BR_FLOOR,player.level_poz_y,player.level_poz_x);
			player.points+=1;
			if(player.points%10==0){
				player.ammo+=20;
				player.health=99;
			}
			break;
		case BR_AMMO:
			delete level.Building[player.level_poz_y][player.level_poz_x];
			level.Building[player.level_poz_y][player.level_poz_x] = new Element(BR_FLOOR,player.level_poz_y,player.level_poz_x);
			player.ammo+=10;
			break;
		case BR_SWITCH:
			level.active_text=TXT_SPACE;
			if(space_hit) {
				((Switch*)level.Building[player.level_poz_y][player.level_poz_x])->isOn=!((Switch*)level.Building[player.level_poz_y][player.level_poz_x])->isOn;
			}
			break;
		case BR_KEY:
			player.keys[((Key*)level.Building[player.level_poz_y][player.level_poz_x])->Key_Type]=true;
			delete level.Building[player.level_poz_y][player.level_poz_x];
			level.Building[player.level_poz_y][player.level_poz_x] = new Element(BR_FLOOR,player.level_poz_y,player.level_poz_x);
			break;
		case BR_TELEPORT: {
			int x=((Teleport*)level.Building[player.level_poz_y][player.level_poz_x])->x_des;
			int y=((Teleport*)level.Building[player.level_poz_y][player.level_poz_x])->y_des;
			player.level_poz_y = y;
			player.level_poz_x = x;
			player.world_poz_x=-(player.level_poz_x*2+1);
			player.world_poz_z=-(player.level_poz_y*2+1);
			}break;
		case BR_LEVUP:
			++level.number;
			if (level.number<NUM_OF_LEVELS){
				level.Load(level.number);
				level.BuildWindMap();
				player.health=99;
				player.ammo=50;
				player.world_poz_x=-level.begin.y*2-1;
				player.world_poz_z=-level.begin.x*2-1;
				player.level_poz_x= abs(player.world_poz_x/2);
				player.level_poz_y= abs(player.world_poz_z/2);
			} else {
				the_end=true;
			}
			menu_on=false;
			break;
	}
}
void shoot() {
	if (player.ammo>0) {
		player.ammo--;
		e_x = b_x = player.world_poz_x;
		e_y = b_y = -0.5+sin(player.step)/32.0;
		e_z = b_z = player.world_poz_z;
		int w_x,w_y;
		bool hit = false;
		while(!hit) {
			w_x = abs(e_x)/2;
			w_y= abs(e_z)/2;
			switch(level.Building[w_y][w_x]->type){
				case BR_WALL:
				case BR_FAN:
				case BR_TELEPORT:
				case BR_WINDOW:
				case BR_DOOR:
					hit=true;
					break;
				case BR_BOX:
					hit=true;
					level.Obstacle[w_y][w_x]=false;
					while (((Box*)level.Building[w_y][w_x])->Content==GW_RANDOM) {
						switch(rand()%4+1){
							case 1: ((Box*)level.Building[w_y][w_x])->Content=GW_LIFE; break;
							case 2: ((Box*)level.Building[w_y][w_x])->Content=GW_AMMO; break;
							case 3: ((Box*)level.Building[w_y][w_x])->Content=GW_EMPTY; break;
							case 4: ((Box*)level.Building[w_y][w_x])->Content=GW_MONSTER; break;
						}
					}
					Brick content;
					switch(((Box*)level.Building[w_y][w_x])->Content){
						case GW_LIFE:
							content = BR_LIFE;
							break;
						case GW_AMMO:
							content = BR_AMMO;
							break;
						case GW_EMPTY:
							content = BR_FLOOR;
							break;
						case GW_MONSTER:
							content = BR_MONSTER;
							break;
					};
					delete level.Building[w_y][w_x];
					switch(content){
						case BR_LIFE:
							level.Building[w_y][w_x] = new Element(BR_LIFE,w_y,w_x);
							break;
						case BR_AMMO:
							level.Building[w_y][w_x] = new Element(BR_AMMO,w_y,w_x);
							break;
						case BR_FLOOR:
							level.Building[w_y][w_x] = new Element(BR_FLOOR,w_y,w_x);
							break;
						case BR_MONSTER:
							level.Building[w_y][w_x] = new Element(BR_FLOOR,w_y,w_x);
							level.Monsters[level.num_of_monsters] = new Monster(w_y,w_x);
							level.num_of_monsters++;
							break;
					};
					break;
			}
			for (int i=0; i<level.num_of_monsters; i++) {
				float d =sqrt(pow(level.Monsters[i]->world_poz_y-abs(e_x),2)+pow(1-abs(e_y),2)+pow(level.Monsters[i]->world_poz_x-abs(e_z),2));
				if (d<1.0){
					hit=true;
					level.Monsters[i]->health-=10;
				}
			}
			e_x-=sin((player.dir_around*3.14)/180.0)/18.0;
			e_y+=sin((player.dir_updown*3.14)/180.0)/18.0;
			e_z+=cos((player.dir_around*3.14)/180.0)/18.0;
		}
	}
}
void playersim(){
	if (player.health<0) player.health=0;
	if (player.health==0) {
		level.active_text=TXT_GAME_OVER;
		player.immobileized=true;
		if (player.death<0.4) {
			player.death+=0.025;
		}
	}
	if (level.Building[player.level_poz_y][player.level_poz_x]->type==BR_SPIKES) {
		if (((Spikes*)level.Building[player.level_poz_y][player.level_poz_x])->state!=S_HIDDEN) {
			player.health=0;
		}
			
	}
}
void input() {
	if(!the_end) {
		if(!menu_on) {
			static POINT m_poz;
			GetCursorPos(&m_poz);
			player.dir_around += (m_poz.x-200)/5.0;
			player.dir_updown += (m_poz.y-200)/5.0;
			if (player.dir_around>360) player.dir_around=0;
			if (player.dir_around<0) player.dir_around=360;
			if (player.dir_updown<-80) player.dir_updown=-80;
			if (player.dir_updown>80) player.dir_updown=80;
			SetCursorPos(200,200);
			lmb_hit=false;
			static bool lmb_release=true;
			if (!player.immobileized){
				if(HIBYTE(GetKeyState(0x57))) { // W
					player.new_world_poz_x=player.world_poz_x-sin((player.dir_around*3.14)/180.0)/18.0;
					player.new_world_poz_z=player.world_poz_z+cos((player.dir_around*3.14)/180.0)/18.0;
					collision();
					player.step+=0.2;
				}
				if(HIBYTE(GetKeyState(0x41))) { // A
					player.new_world_poz_x=player.world_poz_x+cos((-player.dir_around*3.14)/180.0)/18.0;
					player.new_world_poz_z=player.world_poz_z-sin((-player.dir_around*3.14)/180.0)/18.0;
					collision();
					player.step+=0.2;
				}
				if(HIBYTE(GetKeyState(0x53))) { // S
					player.new_world_poz_x=player.world_poz_x+sin((player.dir_around*3.14)/180.0)/18.0;
					player.new_world_poz_z=player.world_poz_z-cos((player.dir_around*3.14)/180.0)/18.0;
					collision();
					player.step+=0.2;
				}
				if(HIBYTE(GetKeyState(0x44))) { // D
					player.new_world_poz_x=player.world_poz_x-cos((-player.dir_around*3.14)/180.0)/18.0;
					player.new_world_poz_z=player.world_poz_z+sin((-player.dir_around*3.14)/180.0)/18.0;
					collision();
					player.step+=0.2;
				}
				if(HIBYTE(GetKeyState(VK_LBUTTON)) && lmb_release) {
					lmb_hit=true;
					lmb_release=false;
					shoot();
				}
			}
			if(!HIBYTE(GetKeyState(VK_LBUTTON))){
				lmb_release=true;
			}
			if (player.step>314) player.step=0;
		} else {
			static int delay=0;
			if(HIBYTE(GetKeyState(VK_UP))) {
				if(menu_state==MN_MAIN)
				if (delay==0){
					delay=250;
					menu_select--;
					if(menu_select<0){
						menu_select=3;
					}
				}
				if(menu_state==MN_LOAD || menu_state==MN_SAVE)
				if (delay==0){
					delay=250;
					menu_select-=3;
					if(menu_select<0)menu_select+=9;
				}
			}
			if(HIBYTE(GetKeyState(VK_DOWN))) {
				if(menu_state==MN_MAIN)
				if(delay==0){
					delay=250;
					menu_select++;
					if(menu_select>3){
						menu_select=0;
					}
				}
				if(menu_state==MN_LOAD || menu_state==MN_SAVE)
				if (delay==0){
					delay=250;
					menu_select+=3;
					menu_select%=9;
				}
			}
			if(HIBYTE(GetKeyState(VK_LEFT))) {
				if(menu_state==MN_LOAD || menu_state==MN_SAVE)
				if (delay==0){
					delay=250;
					menu_select--;
					if(menu_select<0)menu_select+=9;
				}
			}
			if(HIBYTE(GetKeyState(VK_RIGHT))) {
				if(menu_state==MN_LOAD || menu_state==MN_SAVE)
				if (delay==0){
					delay=250;
					menu_select++;
					menu_select%=9;
				}
			}
			delay-=time_delta;
			if(delay<0) delay=0;
		}
	}
	static bool escape_release=true;
	escape_hit=false;
	if(HIBYTE(GetKeyState(VK_ESCAPE)) && escape_release) {
		escape_hit=true;
		escape_release=false;
	}
	if(!HIBYTE(GetKeyState(VK_ESCAPE))){
		escape_release=true;
	}
	space_hit=false;
	static bool space_release=true;
	if((HIBYTE(GetKeyState(VK_SPACE)) || HIBYTE(GetKeyState(VK_RETURN))) && space_release) {
		space_hit=true;
		space_release=false;
	}
	if(!HIBYTE(GetKeyState(VK_SPACE)) && !HIBYTE(GetKeyState(VK_RETURN))){
		space_release=true;
	}
}

void logic() {
	static clock_t begin=0;
	static clock_t end=0;
	input();
	if(!menu_on){
		fansim();
		windmove();
		spikesim();
		monstersim();
		doorsim();
		interact();
		playersim();
		if(escape_hit){
			menu_on=!menu_on;
			menu_select=MN_SAVE;
		}
	} else {
		menusim();
	}
	if (the_end && space_hit) {
		menu_on=true;
		menu_select=0;
		menu_state=MN_MAIN;
		the_end=false;
		level.loaded=false;
	}
	display();
	end = clock();
	time_delta=((long)(end-begin));
	begin = clock();
	if((float)time_delta<20) Sleep(20-time_delta);
}

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPRevInstance, LPSTR lpCmdLine, int nShowCmd) {
	glutInit(&nShowCmd, &lpCmdLine);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(800,600);
	hWnd = (HWND) glutCreateWindow("Scena testowa");
	hDC=GetDC(hWnd);
	hRC=wglCreateContext(hDC);
	glutFullScreen();
	glutDisplayFunc(display);
	glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);
	glEnable(GL_TEXTURE_2D);
	glutIdleFunc(logic);
	glClearColor(0,0,0,1);
	ShowCursor(false);
	Texture[TX_WALL] = SOIL_load_OGL_texture("textures/wall.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Texture[TX_CEILING] = SOIL_load_OGL_texture("textures/ceiling.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Texture[TX_BOX] = SOIL_load_OGL_texture("textures/box.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Texture[TX_WINDOW] = SOIL_load_OGL_texture("textures/window.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Texture[TX_GLASS] = SOIL_load_OGL_texture("textures/glass.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Texture[TX_FLOOR] = SOIL_load_OGL_texture("textures/floor.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Texture[TX_METAL] = SOIL_load_OGL_texture("textures/metal.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Texture[TX_MESH] = SOIL_load_OGL_texture("textures/mesh.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Texture[TX_FAN] = SOIL_load_OGL_texture("textures/fan2.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Texture[TX_EXIT] = SOIL_load_OGL_texture("textures/exit.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Texture[TX_RED] = SOIL_load_OGL_texture("textures/red.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Texture[TX_GOLD] = SOIL_load_OGL_texture("textures/gold.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Texture[TX_YELLOW] = SOIL_load_OGL_texture("textures/yellow.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Texture[TX_MONSTER_1] = SOIL_load_OGL_texture("textures/monster_1.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Texture[TX_MONSTER_2] = SOIL_load_OGL_texture("textures/monster_2.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Texture[TX_MONSTER_3] = SOIL_load_OGL_texture("textures/monster_3.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Texture[TX_MONSTER_4] = SOIL_load_OGL_texture("textures/monster_4.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Texture[TX_MONSTER_5] = SOIL_load_OGL_texture("textures/monster_5.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Texture[TX_TELEPORT] = SOIL_load_OGL_texture("textures/teleport.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Texture[TX_WHITE] = SOIL_load_OGL_texture("textures/white.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Texture[TX_GREEN] = SOIL_load_OGL_texture("textures/green.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Texture[TX_BLUE] = SOIL_load_OGL_texture("textures/blue.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Texture[TX_SWITCH_ON] = SOIL_load_OGL_texture("textures/switch_on.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Texture[TX_SWITCH_OFF] = SOIL_load_OGL_texture("textures/switch_off.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Texture[TX_DOOR] = SOIL_load_OGL_texture("textures/door.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Texture[TX_TARGET] = SOIL_load_OGL_texture("textures/target.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Texture[TX_PANEL] = SOIL_load_OGL_texture("textures/panel_rich.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Texture[TX_MONSTER_1A] = SOIL_load_OGL_texture("textures/monster_1a.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Texture[TX_MONSTER_2A] = SOIL_load_OGL_texture("textures/monster_2a.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Texture[TX_MONSTER_3A] = SOIL_load_OGL_texture("textures/monster_3a.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Texture[TX_MONSTER_4A] = SOIL_load_OGL_texture("textures/monster_4a.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Texture[TX_MONSTER_5A] = SOIL_load_OGL_texture("textures/monster_5a.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Texture[TX_END] = SOIL_load_OGL_texture("textures/end.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);

	Faces[0] =SOIL_load_OGL_texture("textures/face01a.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Faces[1] =SOIL_load_OGL_texture("textures/face02a.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Faces[2] =SOIL_load_OGL_texture("textures/face03a.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Faces[3] =SOIL_load_OGL_texture("textures/face04a.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Faces[4] =SOIL_load_OGL_texture("textures/face05a.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Faces[5] =SOIL_load_OGL_texture("textures/face06a.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Faces[6] =SOIL_load_OGL_texture("textures/face07a.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Faces[7] =SOIL_load_OGL_texture("textures/face08a.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Faces[8] =SOIL_load_OGL_texture("textures/face09a.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Faces[9] =SOIL_load_OGL_texture("textures/face10a.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Faces[10]= SOIL_load_OGL_texture("textures/face11a.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Faces[11]= SOIL_load_OGL_texture("textures/face12a.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Faces[12]= SOIL_load_OGL_texture("textures/face13a.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Faces[13]= SOIL_load_OGL_texture("textures/face14a.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Faces[14]= SOIL_load_OGL_texture("textures/face15a.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Faces[15]= SOIL_load_OGL_texture("textures/face16a.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Faces[16]= SOIL_load_OGL_texture("textures/face17a.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Faces[17]= SOIL_load_OGL_texture("textures/face18a.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Faces[18]= SOIL_load_OGL_texture("textures/face19a.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Faces[19]= SOIL_load_OGL_texture("textures/face20a.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	
	Numbers[0] =  SOIL_load_OGL_texture("textures/0.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Numbers[1] =  SOIL_load_OGL_texture("textures/1.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Numbers[2] =  SOIL_load_OGL_texture("textures/2.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Numbers[3] =  SOIL_load_OGL_texture("textures/3.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Numbers[4] =  SOIL_load_OGL_texture("textures/4.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Numbers[5] =  SOIL_load_OGL_texture("textures/5.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Numbers[6] =  SOIL_load_OGL_texture("textures/6.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Numbers[7] =  SOIL_load_OGL_texture("textures/7.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Numbers[8] =  SOIL_load_OGL_texture("textures/8.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Numbers[9] =  SOIL_load_OGL_texture("textures/9.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	
	Keys[0] =  SOIL_load_OGL_texture("textures/key_white.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Keys[1] =  SOIL_load_OGL_texture("textures/key_yellow.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Keys[2] =  SOIL_load_OGL_texture("textures/key_green.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Keys[3] =  SOIL_load_OGL_texture("textures/key_red.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Keys[4] =  SOIL_load_OGL_texture("textures/key_blue.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	
	Texts[TXT_SWITCH] =  SOIL_load_OGL_texture("textures/switch_txt.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Texts[TXT_SPACE] =  SOIL_load_OGL_texture("textures/space_txt.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Texts[TXT_RED] =  SOIL_load_OGL_texture("textures/red_txt.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Texts[TXT_GREEN] =  SOIL_load_OGL_texture("textures/green_txt.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Texts[TXT_YELLOW] =  SOIL_load_OGL_texture("textures/yellow_txt.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Texts[TXT_BLUE] =  SOIL_load_OGL_texture("textures/blue_txt.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Texts[TXT_WHITE] =  SOIL_load_OGL_texture("textures/white_txt.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Texts[TXT_GAME_OVER] =  SOIL_load_OGL_texture("textures/game_over.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	
	Texture[TX_MENU_PANEL] = SOIL_load_OGL_texture("textures/menu_panel.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Texture[TX_MAIN_MENU] = SOIL_load_OGL_texture("textures/main_menu.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Texture[TX_DOT] = SOIL_load_OGL_texture("textures/dot.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Texture[TX_DOT_GLOW] = SOIL_load_OGL_texture("textures/dot_glow.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Texture[TX_EMPTY] = SOIL_load_OGL_texture("textures/empty.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Marks[0] = SOIL_load_OGL_texture("textures/new_game_mark.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Marks[1] = SOIL_load_OGL_texture("textures/load_mark.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Marks[2] = SOIL_load_OGL_texture("textures/save_mark.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);
	Marks[3] = SOIL_load_OGL_texture("textures/exit_mark.png",4,0,SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA);

	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess[] = { 50.0 };
   	
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

	GLfloat light_spotcut[] = {45};

	GLfloat quadric[] = {0.04};

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, quadric);

	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, quadric);
	glLightfv(GL_LIGHT1, GL_SPOT_CUTOFF, light_spotcut);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, light_spotdir);
		
	glEnable(GL_NORMALIZE);

	glutMainLoop();

	return 0;
}