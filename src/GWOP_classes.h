#include <string.h>
#include <math.h>
#include <time.h>
#include <windowsx.h>

#define WIND_NORTH 1
#define WIND_SOUTH 2
#define WIND_EAST 4
#define WIND_WEST 8

typedef enum 
{
    BR_WALL,
    BR_FLOOR,
    BR_DOOR,
    BR_WINDOW,
    BR_LAMP,
    BR_LIFE,
    BR_POINT,
    BR_AMMO,
    BR_KEY,
    BR_SWITCH,
    BR_SPIKES,
    BR_FAN,
    BR_MONSTER,
    BR_BOX,
    BR_TELEPORT,
    BR_LEVUP,
    BR_START,
    BR_BLANK
} Brick;

typedef enum
{
    GW_NORMAL,
    GW_AUTO_LOCK
} DoorType;

typedef enum
{ 
	D_OPEN,
	D_CLOSED,
	D_OPENING,
	D_CLOSING
} DoorState;

typedef enum 
{
    GW_MONSTER_1,
    GW_MONSTER_2,
    GW_MONSTER_3,
    GW_MONSTER_4,
    GW_MONSTER_5
} MonsterType;

typedef enum 
{
    GW_RANDOM=0,
    GW_LIFE=1,
    GW_AMMO=2,
    GW_EMPTY=3,
    GW_MONSTER=4
} BoxContent;

typedef enum 
{
    GW_WHITE=0,
    GW_YELLOW=1,
    GW_GREEN=2,
    GW_RED=3,
    GW_BLUE=4
} KeyType;

typedef enum 
{
    GW_WEST,
    GW_EAST,
    GW_NORTH,
    GW_SOUTH
} Direction;

typedef enum 
{
    GW_WEAST_EAST,
    GW_NORTH_SOUTH
} Orientation;

typedef enum
{ 
	S_HIDDEN,
	S_SHOWN,
	S_HIDING,
	S_SHOWING
} SpikeState;

typedef enum {
	M_LIVE,
	M_DEAD,
	M_DYING,
	M_ATTACK
} MonsterState;
class Monster;

class Element
{
public:
    Brick type;
    int x_poz;
    int y_poz;
    Element()
    {
    }
    Element(Brick t, int x, int y)
    {
        type = t;
        x_poz = x;
        y_poz = y;
    }
};

typedef struct {
	float north_wind;
	float south_wind;
	float east_wind;
	float west_wind;
}Wind;

typedef struct{
	int x;
	int y;
}Point;

class Window : public Element
{
public:
    Orientation Orient;
    Window(int x, int y)
    {
        type = BR_WINDOW;
        x_poz = x;
        y_poz = y;
        Orient = GW_NORTH_SOUTH;
    }
};

class Teleport : public Element
{
public:
    int x_des;
    int y_des;
    Teleport(int x, int y)
    {
        type = BR_TELEPORT;
        x_poz = x;
        y_poz = y;
        x_des = 0;
        y_des = 0;
    }
};

class Switch : public Element
{
public:
    bool isOn;
    int ID;
    Switch(int x, int y)
    {
        type = BR_SWITCH;
        x_poz = x;
        y_poz = y;
        isOn = false;
        ID = 0;
    }
};

class Spikes : public Element
{
public:
	SpikeState state;
    int HidePeriod;
    int UpPeriod;
    long int Delay;
	float r;
	Point spikes[40];
	float spikesize[40];
    Spikes(int x, int y)
    {
		srand(time(NULL));
		for (int i=0; i<40; i++) {
			spikes[i].x=rand()%1000;
			spikes[i].y=rand()%1000;
			spikesize[i]=rand()%500+250;
		}
		state = S_HIDDEN;
        type = BR_SPIKES;
        x_poz = x;
        y_poz = y;
        HidePeriod = 100;
        UpPeriod = 100;
        Delay = 0;
		r=0;
    }
};

class Monster : public Element
{
public:
    MonsterType MType;
	MonsterState MState;
	float world_poz_x;
	float world_poz_y;
	int old_x_poz;
	int old_y_poz;
	int delay;
	int attack_delay;
	int step;
	int health;
	float dead;
    Monster(int x, int y)
    {
		old_x_poz=0;
		old_y_poz=0;
		delay=0;
		attack_delay=0;
		health=20;
		world_poz_x=x*2+1;
		world_poz_y=y*2+1;
		step=0;
        type = BR_MONSTER;
        x_poz = x;
        y_poz = y;
        MType = GW_MONSTER_1;
		MState = M_LIVE;
		dead=0;
    }
};

class LevUp : public Element
{
public:
    Orientation Orient;
    LevUp(int x, int y)
    {
        type = BR_LEVUP;
        x_poz = x;
        y_poz = y;
        Orient = GW_NORTH_SOUTH;
    }
};

class Key: public Element
{
public:
    KeyType Key_Type;
    Key(int x, int y)
    {
        type = BR_KEY;
        x_poz = x;
        y_poz = y;
        Key_Type = GW_RED;
    }
};

class Fan: public Element
{
public:
    Direction Dir;
	double r;
    bool isOn;
    int Strength;
    int SwitchID_0;
    int SwitchID_1;
    int SwitchID_2;
    int SwitchID_3;
    int SwitchID_4;
    Fan(int x, int y) 
    {
        type = BR_FAN;
        x_poz = x;
        y_poz = y;
        Dir = GW_EAST;
        isOn = true;
        Strength = 100;
        SwitchID_0 = 0;
        SwitchID_1 = 0;
        SwitchID_2 = 0;
        SwitchID_3 = 0;
        SwitchID_4 = 0;
		r=0;
    }
};

class Door: public Element
{
public:
	DoorState Door_State;
    DoorType Door_Type;
    Orientation Orient;
    bool isOpen;
    bool hasLock;
    KeyType LockType;
    int SwitchID_0;
    int SwitchID_1;
    int SwitchID_2;
    int SwitchID_3;
    int SwitchID_4;
	float gap;
    Door(int x, int y) 
    {
		gap=0;
		Door_State = D_CLOSED;
        type = BR_DOOR;
        x_poz = x;
        y_poz = y;
        Door_Type = GW_NORMAL;
        Orient = GW_NORTH_SOUTH;
        isOpen = true;
        hasLock = false;
        LockType = GW_RED;
        SwitchID_0 = 0;
        SwitchID_1 = 0;
        SwitchID_2 = 0;
        SwitchID_3 = 0;
        SwitchID_4 = 0;
    }
};

class Box : public Element
{
public:
    BoxContent Content;
    Box(int x, int y)
    {
        type = BR_BOX;
        x_poz = x;
        y_poz = y;
        Content = GW_EMPTY;
    }
};

class Player {
public:
	float health;
	int ammo;
	int points;
	bool keys[5];
	bool immobileized;
	float world_poz_x;
	float world_poz_z;
	float new_world_poz_x;
	float new_world_poz_z;
	float dir_around;
	float dir_updown;
	float step;
	float death;
	int level_poz_x;
	int level_poz_y;
	int new_level_poz_x;
	int new_level_poz_y;
	Player(){
		for (int i=0; i<5; i++) keys[i]=false;
		immobileized=false;
		death=0;
		health=99;
		ammo=50;
		points=0;
		step=0;
		world_poz_x=-1;
		world_poz_z=-1;
		dir_around=0;
		dir_updown=0;
	}
};

class Level
{
public:
	int number;
	bool loaded;
	int active_text;
    Element* Building[200][200];
	Monster* Monsters[200];
	int num_of_monsters;
	int num_of_doors;
	int num_of_swtches;
	int num_of_fans;
	int Obstacle[200][200];
	Wind WindMap[200][200];
	Point lamp[100];
	Door* Doors[200];
	Switch* Switches[200];
	Fan* Fans[200];
	int lamp_count;
	int closest_lamp;
	Point begin;
    int width;
    int height;
	char title[256];
    Level()
    {
		number=0;
		loaded=false;
		active_text=-1;
        width = 50;
        height = 50;
        strcpy(title,"New Level");
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
    }
	bool Load(int);
	bool BuildWindMap(void);
};
