
#include "cGame.h"
#include "cLog.h"
#include "cKeyboard.h"
#include <list>
#include <time.h>
#include "cKeyboard.h";
#include <list>

cGame::cGame() 
{
	int newx, newy;

	srand(time(NULL));
	
	EnemArray.numEnemies = 3;
	for (int i=0;i<EnemArray.numEnemies;i++)
	{
		newx = rand()%(SCENE_WIDTH-2)+1;
		newy = rand()%(SCENE_HEIGHT-3)+1;
		EnemArray.enemies[i].SetEnemy(i,newx,newy,100);
	}

	Critters.numPlayers = 0;
}
cGame::~cGame(){}

bool cGame::Init(HWND hWnd,HINSTANCE hInst,bool exclusive)
{
	bool res;
	cLog *Log = cLog::Instance();

	res = Graphics.Init(hWnd);
	if(!res)
	{
		Log->Msg("Error initializing Graphics!");
		return false;
	}

	res = Input.Init(hInst,hWnd,exclusive,USE_MOUSE|USE_KEYBOARD);
	if(!res)
	{
		Log->Msg("Error initializing Input!");
		return false;
	}
	Input.SetMousePosition(SCREEN_RES_X >> 1,SCREEN_RES_Y >> 1);

	Graphics.LoadData();

	Scene.LoadMap("map.txt");

	return true;
}

void cGame::Finalize()
{
	Graphics.UnLoadData();
	Graphics.Finalize();
	Input.UnacquireAll();
	Input.Finalize();
}

bool cGame::Loop()
{
	bool res;

	//Input
	res = LoopInput();
	if(!res) return false;
	
	//Process
	res = LoopProcess();
	if(!res) return false;

	//Output
	res = LoopOutput();
	if(!res) return false;
	
	return true;
}

bool cGame::LoopInput()
{
	bool res;
	cLog *Log = cLog::Instance();

	res = Input.Read();
	if(!res)
	{
		Log->Msg("Error reading Input!");
		return false;
	}

	res = Controller.Read();
	return true;
}

bool cGame::LoopProcess()
{
	cMouse *Mouse;
	cKeyboard *Keyboard;

	Mouse = Input.GetMouse();
	Keyboard = Input.GetKeyboard();

	switch(state)
	{
		case STATE_MAIN:
						/*if(Mouse->ButtonDown(LEFT))
						{
							//Play button
							if(Mouse->In(334,236,420,278))
							{
								Critters.numPlayers = 3;
								Critters.Players[0].controller = false;
								Critters.Players[0].numController = 0;
								Critters.Players[1].controller = true;
								Critters.Players[1].numController = 0;
								Critters.Players[2].controller = true;
								Critters.Players[2].numController = 1;
								state = STATE_GAME;
							}
							//Exit button
							else if(Mouse->In(426,236,512,278))
							{
								return false;
							}
						}*/
						if(CheckNewPlayerUpdate(Keyboard))
						{
							state = STATE_GAME;
						}

						break;

		case STATE_GAME:
						ProcessOrder();
						for (int i=0;i<EnemArray.numEnemies;i++)
						{
							EnemArray.enemies[i].Move();
						}
						CheckNewPlayerUpdate(Keyboard);
						//Critter.Move();
						break;
	}
	return true;
}

bool cGame::LoopOutput()
{
	bool res;
	res = Render();
	return res;
}

bool cGame::Render()
{
	bool res;
	res = Graphics.Render(state, Input.GetMouse(), &Scene, &Critters, &EnemArray, &Bullet);
	return res;
}

bool cGame::CheckNewPlayerUpdate(cKeyboard *Keyboard)
{
	static bool keyboardUsed = false;

	for(int i=0;i<4;i++)
	{
		if(Controller.buttonDown(i, XINPUT_GAMEPAD_START) && !Controller.used[i])
		{
			Critters.numPlayers++;
			Critters.Players[Critters.numPlayers-1].controller = true;
			Critters.Players[Critters.numPlayers-1].numController = i;
			Controller.used[i] = true;
			return true;
		}
	}

	if(Keyboard->KeyDown(DIK_RETURN) && !keyboardUsed)
	{
		Critters.numPlayers++;
		Critters.Players[Critters.numPlayers-1].controller = false;
		Critters.Players[Critters.numPlayers-1].numController = 0;
		keyboardUsed = true;
		return true;
	}

	return false;
}

void cGame::ProcessOrder()
{
	//cMouse *Mouse;
	cKeyboard *Keyboard;
	int mx,my,msx,msy,p,cx,cy,x,y;
	int s=5; //marge for directional pointers
	int xo,xf,yo,yf;
	//int b4pointer;
	bool KeyPushed = false;
	static int delayBullet[4] = {0,0,0,0};

	//Player position
	int px,py;

	//Mouse = Input.GetMouse();
	Keyboard = Input.GetKeyboard();
	//b4pointer = Mouse->GetPointer();
	//Mouse->GetPosition(&mx,&my);

	if(Scene.isMoving)
	{
		Scene.MoveMap(Scene.moveDir);
	}
	else
	{
		for(int i=0;i<Critters.numPlayers;i++)
		{
			delayBullet[i]++;

			if(Critters.Players[i].controller)
			{
				float xcont = Controller.getLeftStickX(Critters.Players[i].numController);
				float ycont = Controller.getLeftStickY(Critters.Players[i].numController);
				if(xcont!= 0 || ycont!=0) Critters.Players[i].MoveController(xcont,-ycont,&Scene);
				else {Critters.Players[i].MoveKey(STOP,&Scene);}

				if(Controller.buttonDown(Critters.Players[i].numController, XINPUT_GAMEPAD_X) && delayBullet[i] > 15)
				{
					int posx, posy, angulo;
					Critters.Players[i].Fire(&posx,&posy,&angulo);
					Bullet.NewBullet(0,posx, posy, angulo, false, 0, 10);
					delayBullet[i] = 0;

					Critters.Players[i].life--;
				}
			}
			else
			{
				if(Critters.Players[i].numController == 0)
				{
					if	   (Keyboard->KeyDown(DIK_W)&&Keyboard->KeyDown(DIK_D)) Critters.Players[i].MoveKey(NE,&Scene);
					else if(Keyboard->KeyDown(DIK_W)&&Keyboard->KeyDown(DIK_A)) Critters.Players[i].MoveKey(NO,&Scene);
					else if(Keyboard->KeyDown(DIK_W)&&Keyboard->KeyDown(DIK_D)) Critters.Players[i].MoveKey(NE,&Scene);
					else if(Keyboard->KeyDown(DIK_S)&&Keyboard->KeyDown(DIK_A)) Critters.Players[i].MoveKey(SO,&Scene);
					else if(Keyboard->KeyDown(DIK_S)&&Keyboard->KeyDown(DIK_D)) Critters.Players[i].MoveKey(SE,&Scene);
					else if(Keyboard->KeyDown(DIK_W)) Critters.Players[i].MoveKey(N,&Scene);
					else if(Keyboard->KeyDown(DIK_S)) Critters.Players[i].MoveKey(S,&Scene);
					else if(Keyboard->KeyDown(DIK_D)) Critters.Players[i].MoveKey(E,&Scene);
					else if(Keyboard->KeyDown(DIK_A)) Critters.Players[i].MoveKey(O,&Scene);
					else {Critters.Players[i].MoveKey(STOP,&Scene);}
			
					if(Keyboard->KeyDown(DIK_RCONTROL) && delayBullet[i] > 15)
					{
						int posx, posy, angulo;
						Critters.Players[i].Fire(&posx,&posy,&angulo);
						Bullet.NewBullet(0,posx, posy, angulo, false, 0, 10);
						delayBullet[i] = 0;

						Critters.Players[i].life--;
					}
				}
			}

			if(Scene.isDoorOpen)
			{
				switch(Scene.doorMap[int((Critters.Players[0].x+16)/32)][int((Critters.Players[0].y+16)/32)])
				{
					case Norte: Scene.MoveMap(Norte); break;
					case Sur: Scene.MoveMap(Sur); break;
					case Oeste: Scene.MoveMap(Oeste); break;
					case Este: Scene.MoveMap(Este); break;
				}
			}
		}

		if(Keyboard->KeyDown(DIK_LCONTROL))
			Scene.OpenNextDoor();

		bulletsCollision();
	
		Bullet.UpdateBullets(&Scene);

		/**
		* Update the doors animation
		**/
		Scene.UpdateDoors();

		/**
		* Enemy moving to the player
		**/
		Critters.Players[0].GetCell(&px,&py);
		for (int i=0;i<EnemArray.numEnemies;i++)
		{
			EnemArray.enemies[i].GoToPlayer(Scene.Pathmap,px,py);
		}
	}
}

void cGame::bulletsCollision(){
	/*
	Modificar� la lista de bullets i de los enemigos y Critter a�adiendo un flag "colision" por si ha colisionado con ellos
	*/
	int headx, heady;

	//Por cada bullet
	std::list<BulletStruct>::iterator illista;
	illista = Bullet.listaBullets.begin();
	while( illista != Bullet.listaBullets.end() )
	{
		headx = illista->x + Bullet.cosdeg(illista->angulo) * illista->speed;
		heady = illista->y + Bullet.sindeg(illista->angulo) * illista->speed;
		
		//	Si no se ha salido de limites y no esta explotando ya
		if(Scene.isWalkable(int(headx/32), int(heady/32)) && !illista->destroying)
		{
			//Por cada enemigo
			for (int i=0;i<EnemArray.numEnemies;i++)
			{
				//Comprovar colision
				//Si colisionConEnemigo
				if((headx > EnemArray.enemies[i].x && headx < EnemArray.enemies[i].x+32)
					&&
					(heady > EnemArray.enemies[i].y && heady < EnemArray.enemies[i].y+32)
					&&
					!EnemArray.enemies[i].dying)
				{
					EnemArray.enemies[i].Impactar(illista->poder);
					illista->destroying = true;
				}

			}
		}
		else 
		{
			illista->destroying = true;
		}
		illista++;
	}
}