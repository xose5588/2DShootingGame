#include "cBullet.h"

#include "cScene.h"

cBullet::cBullet()
{
	delay = 0;
	listaBullets.clear();
}
cBullet::~cBullet()
{
	
}

void cBullet::NewBullet ( int id, float x, float y, float fAngulo, bool rebota, int numRebotes,int speed )
{
	BulletStruct bullet;
	bullet.id = id;
	bullet.x = x;
	bullet.y = y;
	bullet.angulo = fAngulo;
	bullet.rebota = rebota;
	bullet.numRebotes = numRebotes;
	bullet.speed = speed;
	bullet.destroying = false;
	bullet.animseq = 0;
	bullet.destseq = 0;
	bullet.poder = 50;
	listaBullets.insert( listaBullets.end(), bullet );

	delay = 0;
}

float cBullet::sindeg(float angle)
{
	float fAux = sin ( angle*3.14159265/180 );
	if( fAux < 0.001f && fAux > -0.001f ) return 0;
	return fAux;
}

float cBullet::cosdeg(float angle)
{
	float fAux = cos ( angle*3.14159265/180 );
	if( fAux < 0.001f && fAux > -0.001f )return 0;
	return fAux;
}

bool cBullet::isReadyToShoot()
{
	if(delay > 15) return true;

	return false;
}

void cBullet::UpdateBullets(cScene *Scene)
{
	delay++;
	bool deleted;

	//Mou tots els projectils
	std::list<BulletStruct>::iterator illista;
	illista = listaBullets.begin();
	while( illista != listaBullets.end() )
	{
		//Variable hyper util, perque sino peta tot
		deleted = false;

		//Colisi�n!!!
		if(illista->destroying && illista->destseq < 8)
		{
			//No hacemos nada
		}			
		else if (illista->destroying && illista->destseq >= 8)
		{
			//Explosi�n
			illista = listaBullets.erase(illista);
			deleted = true;
		}
		else
		{
			illista->x += cosdeg(illista->angulo) * illista->speed;
			illista->y -= sindeg(illista->angulo) * illista->speed;
		}

		if (!deleted)
			illista++;
	}
			/*Colisi�n con el personaje principal

			if ( pantalla.lsTank.actiu )
			{
				//Col�lisi� del projectil amb el tanc principal
				boundingBox.rotaCaixa(tadASE.boundings[5], pantalla.lsTank.angleInclinacio + 90.0f, 1);
				if(boundingBox.colisio(tadASE.boundings[7], x + 0.25, y - 0.25, tadASE.boundings[5], posXtank, posYtank) && (int)id != iPlayer )
				{
					proj.nouAnimation ( 0, x + 0.25, y - 0.25, 0.13, llistaAnimations );
					
					illista = llistaProjectils.erase(illista);
					sound.play(soundsid[0]);
					//Perd una vida
					pantalla.lsTank.vida--;
					printf("perd 1 vida\n");
					if( pantalla.lsTank.vida <= 0 )
					{
						proj.nouAnimation ( 2, posXtank, posYtank, 0.12, llistaAnimations );
						pantalla.lsTank.actiu = 0;
					}
					entra = true;
				}
				boundingBox.recuperaCaixa(tadASE.boundings[5], 1);
			}*/

			/*Col�lisi� amb els tancs enemics

			for ( k=0; k < pantalla.numTanksEnemics; k++ )
			{
				if(!pantalla.tancsEnemics[k].mort){
					float fAuxEnemicX = pantalla.tancsEnemics[k].posicioX+0.25+ (0.055 * funcions.cosdeg(pantalla.tancsEnemics[k].angleInclinacio));
					float fAuxEnemicY = pantalla.tancsEnemics[k].posicioY-0.25 + (0.055 * funcions.sindeg(pantalla.tancsEnemics[k].angleInclinacio));
					boundingBox.rotaCaixa(tadASE.boundings[5], pantalla.tancsEnemics[k].angleInclinacio + 90.0f, 1);
					if ( boundingBox.colisio( tadASE.boundings[7], x + 0.25, y - 0.25, tadASE.boundings[5], fAuxEnemicX, fAuxEnemicY ) && id != -2 ){
						pantalla.tancsEnemics[k].mort = true;
						pantalla.tancsEnemics[k].temps = iTimer;
						proj.nouAnimation ( 0, x + 0.25, y - 0.25, 0.13, llistaAnimations );
						proj.nouAnimation ( 2, fAuxEnemicX, fAuxEnemicY, 0.12, llistaAnimations );
						illista = llistaProjectils.erase(illista);
						sound.play(soundsid[0]);
						entra = true;
					}
					boundingBox.recuperaCaixa(tadASE.boundings[5], 1);
				}
			}*/

			//Actualitza posici� dels projectils
			
}

void cBullet::GetRect(RECT *rc,int destseq){
	SetRect(rc,destseq/2*50,32,destseq/2*50+50,78);
}
