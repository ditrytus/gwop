#include <stdio.h>
#include <string.h>
#include <sstream>
#include "GWOP_classes.h"

using namespace std;

extern Level level;

bool Level::Load(int num)
{
	level.number=num;
	loaded=false;
	active_text=-1;
    width = 50;
    height = 50;
    strcpy(title,"BUBA");
	lamp_count=0;
	closest_lamp=0;
	num_of_monsters=0;
	num_of_doors=0;
	num_of_swtches=0;
	num_of_fans=0;
    for (int i = 0; i < 200; i++)
    {
        for (int j=0; j<200; j++) 
        {
            Building[i][j] = NULL;
			Obstacle[i][j] = false;
        }
    }
	loaded=true;
	std::stringstream cnv;
	char* cstr;
	std::string str;
	std::stringstream sb;
	sb<<"levels/lev"<<num<<".glv";
	FILE* plik = fopen(sb.str().c_str(),"r");
	fscanf(plik,"%s\n",level.title);
	fscanf(plik,"%d\n",&level.width);
	fscanf(plik,"%d\n",&level.height);
	int u=7;
    for (int k = 0; k < level.width*level.height; k++)
    {
        int i = k%level.height;
        int j = k/level.width;
        char  Type[256];
		fscanf(plik,"%s\n",Type);
		if (strcmp(Type,"FLOOR")==0) {
			level.Building[i][j] = new Element(BR_FLOOR, i, j);
		} else
		if (strcmp(Type,"WALL")==0) {
			level.Building[i][j] = new Element(BR_WALL, i, j);
			level.Obstacle[i][j] = true;
		} else 
		if (strcmp(Type,"WINDOW")==0) {
			level.Building[i][j] = (Element*) new Window(i,j);
			level.Obstacle[i][j] = true;
			char  Type2[256];
			fscanf(plik,"%s\n",Type2);
			if (strcmp(Type2,"NORTH_SOUTH"))
				((Window*)level.Building[i][j])->Orient = GW_NORTH_SOUTH;
			else if(strcmp(Type2,"WEAST_EAST"))
				((Window*)level.Building[i][j])->Orient = GW_WEAST_EAST;
		} else 
		if(strcmp(Type,"LAMP")==0){
			level.Building[i][j] = (Element*) new Element(BR_LAMP,i,j);
			level.lamp[level.lamp_count].x=i;
			level.lamp[level.lamp_count].y=j;
			lamp_count++;
		} else 
		if(strcmp(Type,"FAN")==0) {
			level.Fans[level.num_of_fans] = new Fan(i,j);
			level.Building[i][j] = (Element*) level.Fans[level.num_of_fans];
			level.Obstacle[i][j] = true;
			level.num_of_fans++;
			char boolean[256];
			fscanf(plik,"%s\n",boolean);
			if(strcmp(boolean,"True")==0)
				((Fan*)level.Building[i][j])->isOn=true;
			else
				((Fan*)level.Building[i][j])->isOn=false;
			char  Type2[256];
			fscanf(plik,"%s\n",Type2);
			if (strcmp(Type2,"NORTH")==0)
				((Fan*)level.Building[i][j])->Dir = GW_NORTH;
			else if(strcmp(Type2,"SOUTH")==0)
				((Fan*)level.Building[i][j])->Dir = GW_SOUTH;
			else if(strcmp(Type2,"EAST")==0)
				((Fan*)level.Building[i][j])->Dir = GW_EAST;
			else if(strcmp(Type2,"WEST")==0)
				((Fan*)level.Building[i][j])->Dir = GW_WEST;
			fscanf(plik,"%d\n",&((Fan*)level.Building[i][j])->Strength);
			fscanf(plik,"%d\n",&((Fan*)level.Building[i][j])->SwitchID_0);
			fscanf(plik,"%d\n",&((Fan*)level.Building[i][j])->SwitchID_1);
			fscanf(plik,"%d\n",&((Fan*)level.Building[i][j])->SwitchID_2);
			fscanf(plik,"%d\n",&((Fan*)level.Building[i][j])->SwitchID_3);
			fscanf(plik,"%d\n",&((Fan*)level.Building[i][j])->SwitchID_4);
		}  else
		if(strcmp(Type,"START")==0){
			level.Building[i][j] = new Element(BR_FLOOR, i, j);
			level.begin.x=i;
			level.begin.y=j;
		} else
		if(strcmp(Type,"LEVUP")==0){ 
			level.Building[i][j] = new Element(BR_LEVUP, i, j);
		} else
		if (strcmp(Type,"LIFE")==0) {
			level.Building[i][j] = new Element(BR_LIFE, i, j);
		} else 
		if (strcmp(Type,"AMMO")==0) {
			level.Building[i][j] = new Element(BR_AMMO, i, j);
		} else 
		if (strcmp(Type,"POINT")==0) {
			level.Building[i][j] = new Element(BR_POINT, i, j);
		} else 
		if (strcmp(Type,"SPIKES")==0) {
			level.Building[i][j] = (Element*) new Spikes( i, j);
			fscanf(plik,"%d\n",&((Spikes*)level.Building[i][j])->Delay);
            fscanf(plik,"%d\n",&((Spikes*)level.Building[i][j])->HidePeriod);
            fscanf(plik,"%d\n",&((Spikes*)level.Building[i][j])->UpPeriod);
		} else 
		if (strcmp(Type,"MONSTER")==0) {
			level.Building[i][j] = new Element(BR_FLOOR,i,j);
			level.Monsters[level.num_of_monsters] = new Monster(i,j);
			char  Type2[256];
			fscanf(plik,"%s\n",Type2);
			if (strcmp(Type2,"MONSTER_1")==0)
				level.Monsters[level.num_of_monsters]->MType = GW_MONSTER_1;
			else if(strcmp(Type2,"MONSTER_2")==0)
				level.Monsters[level.num_of_monsters]->MType = GW_MONSTER_2;
			else if(strcmp(Type2,"MONSTER_3")==0)
				level.Monsters[level.num_of_monsters]->MType = GW_MONSTER_3;
			else if(strcmp(Type2,"MONSTER_4")==0)
				level.Monsters[level.num_of_monsters]->MType = GW_MONSTER_4;
			else if(strcmp(Type2,"MONSTER_5")==0)
				level.Monsters[level.num_of_monsters]->MType = GW_MONSTER_5;
			level.Monsters[level.num_of_monsters]->health+=level.Monsters[level.num_of_monsters]->MType*10;
			level.num_of_monsters++;
		} else 
		if (strcmp(Type,"TELEPORT")==0) {
			level.Building[i][j] = (Element*) new Teleport( i, j);
			fscanf(plik,"%d\n",&((Teleport*)level.Building[i][j])->x_des);
			fscanf(plik,"%d\n",&((Teleport*)level.Building[i][j])->y_des);
		} else 
		if (strcmp(Type,"KEY")==0) {
			level.Building[i][j] = (Element*) new Key( i, j);
			char  Type2[256];
			fscanf(plik,"%s\n",Type2);
			if (strcmp(Type2,"GREEN")==0)
				((Key*)level.Building[i][j])->Key_Type = GW_GREEN;
			else if(strcmp(Type2,"BLUE")==0)
				((Key*)level.Building[i][j])->Key_Type = GW_BLUE;
			else if(strcmp(Type2,"RED")==0)
				((Key*)level.Building[i][j])->Key_Type = GW_RED;
			else if(strcmp(Type2,"WHITE")==0)
				((Key*)level.Building[i][j])->Key_Type = GW_WHITE;
			else if(strcmp(Type2,"YELLOW")==0)
				((Key*)level.Building[i][j])->Key_Type = GW_YELLOW;
		} else 
		if (strcmp(Type,"BOX")==0) {
			level.Building[i][j] = (Element*) new Box( i, j);
			level.Obstacle[i][j] = true;
			char  Type2[256];
			fscanf(plik,"%s\n",Type2);
			if (strcmp(Type2,"RANDOM")==0)
				((Box*)level.Building[i][j])->Content = GW_RANDOM;
			else if(strcmp(Type2,"LIFE")==0)
				((Box*)level.Building[i][j])->Content = GW_LIFE;
			else if(strcmp(Type2,"AMMO")==0)
				((Box*)level.Building[i][j])->Content = GW_AMMO;
			else if(strcmp(Type2,"EMPTY")==0)
				((Box*)level.Building[i][j])->Content = GW_EMPTY;
			else if(strcmp(Type2,"MONSTER")==0)
				((Box*)level.Building[i][j])->Content = GW_MONSTER;
		} else 
		if (strcmp(Type,"SWITCH")==0) {
			level.Switches[level.num_of_swtches] = new Switch( i, j);
			level.Building[i][j] = (Element*) level.Switches[level.num_of_swtches];
			level.num_of_swtches++;
			fscanf(plik,"%d\n",&((Switch*)level.Building[i][j])->ID);
			char boolean[256];
			fscanf(plik,"%s\n",boolean);
			if(strcmp(boolean,"True")==0)
				((Switch*)level.Building[i][j])->isOn=true;
			else
				((Switch*)level.Building[i][j])->isOn=false;
		} else
		if(strcmp(Type,"DOOR")==0) {
			level.Doors[level.num_of_doors] = new Door(i,j);
			level.Building[i][j] = (Element*) level.Doors[level.num_of_doors];
			level.num_of_doors++;
			level.Obstacle[i][j] = true;
			char  Type2[256];
			fscanf(plik,"%s\n",Type2);
			if (strcmp(Type2,"NORMAL")==0)
				((Door*)level.Building[i][j])->Door_Type = GW_NORMAL;
			else if(strcmp(Type2,"AUTO_LOCK")==0)
				((Door*)level.Building[i][j])->Door_Type = GW_AUTO_LOCK;
			char boolean[256];
			fscanf(plik,"%s\n",boolean);
			if(strcmp(boolean,"True")==0)
				((Door*)level.Building[i][j])->hasLock=true;
			else
				((Door*)level.Building[i][j])->hasLock=false;
			fscanf(plik,"%s\n",boolean);
			if(strcmp(boolean,"True")==0)
				((Door*)level.Building[i][j])->isOpen=true;
			else
				((Door*)level.Building[i][j])->isOpen=false;
			fscanf(plik,"%s\n",Type2);
			if (strcmp(Type2,"GREEN")==0)
				((Door*)level.Building[i][j])->LockType= GW_GREEN;
			else if(strcmp(Type2,"WHITE")==0)
				((Door*)level.Building[i][j])->LockType = GW_WHITE;
			else if (strcmp(Type2,"RED")==0)
				((Door*)level.Building[i][j])->LockType = GW_RED;
			else if(strcmp(Type2,"BLUE")==0)
				((Door*)level.Building[i][j])->LockType = GW_BLUE;
			else if(strcmp(Type2,"YELLOW")==0)
				((Door*)level.Building[i][j])->LockType = GW_YELLOW;
			fscanf(plik,"%s\n",Type2);
			if (strcmp(Type2,"NORTH_SOUTH"))
				((Door*)level.Building[i][j])->Orient = GW_NORTH_SOUTH;
			else if(strcmp(Type2,"WEAST_EAST"))
				((Door*)level.Building[i][j])->Orient = GW_WEAST_EAST;
			fscanf(plik,"%d\n",&((Door*)level.Building[i][j])->SwitchID_0);
			fscanf(plik,"%d\n",&((Door*)level.Building[i][j])->SwitchID_1);
			fscanf(plik,"%d\n",&((Door*)level.Building[i][j])->SwitchID_2);
			fscanf(plik,"%d\n",&((Door*)level.Building[i][j])->SwitchID_3);
			fscanf(plik,"%d\n",&((Door*)level.Building[i][j])->SwitchID_4);

		} else {
			level.Building[i][j] = new Element(BR_BLANK, i, j);
		}
    }
	fclose(plik);
	return true;
}
bool Level::BuildWindMap(){
	for (int i = 0; i < 200; i++)
		for (int j=0; j<200; j++){
			level.WindMap[i][j].south_wind=0;
			level.WindMap[i][j].north_wind=0;
			level.WindMap[i][j].east_wind=0;
			level.WindMap[i][j].west_wind=0;
		}
	for (int i=0; i<level.height; i++)
		for (int j=0; j<level.width; j++) {
			if(level.Building[i][j]!=NULL) {
				if(level.Building[i][j]->type==BR_FAN){
					if(((Fan*)level.Building[i][j])->isOn){
						int x=j;
						int y=i;
						for (int k=0; k<((Fan*)level.Building[i][j])->Strength; k++) {
							switch(((Fan*)level.Building[i][j])->Dir){
								case GW_NORTH:
									y--;
									level.WindMap[x][y].north_wind=((Fan*)level.Building[i][j])->Strength-k;
									break;
								case GW_SOUTH:
									y++;
									level.WindMap[x][y].south_wind=((Fan*)level.Building[i][j])->Strength-k;
									break;
								case GW_EAST:
									x++;
									level.WindMap[x][y].east_wind=((Fan*)level.Building[i][j])->Strength-k;
									break;
								case GW_WEST:
									x--;
									level.WindMap[x][y].west_wind=((Fan*)level.Building[i][j])->Strength-k;
									break;
							}
							if (level.Obstacle[y][x]) k=((Fan*)level.Building[i][j])->Strength;
						}
					}
				}
			}
		}
		return true;
}