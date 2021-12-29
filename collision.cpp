//controlar personagem com teclas W,A,S,D
//rodar camera com setas

#include <iostream>
#include <gl/GL.h>
#include <GL/glut.h>
#include <math.h>
#include <GL/freeglut.h> // para funcao: glutMouseWheelFunc(); se remover esta linha, remover tambem glutMouseWheelFunc(MouseWheel);

using namespace std;
const int  tamanhoLado = 17; // 2^n + 1 , por exemplo: 513  (2^9 +1 )
float ladoTerrain = tamanhoLado;
int range = 200; //para a funcao valorRandom
double zoom=27.5,rotate_y=0,rotate_x=-50;
int terrenoPosXInicial = 290,terrenoPosYInicial = 290;


float mapa[tamanhoLado][tamanhoLado];

float posXJogador = 300, posYJogador = 300,posZJogador = 250;
float ladoPersonagem = 0.5f,alturaPersonagem=1.0f;
//valor random entre min e max

//Normal triangulo
float normalX ;
float normalY ;
float normalZ  ;
float aX,aY,aZ;
float bX,bY,bZ;
float pontoZ; // altura Z camada fina
float plinhaX,plinhaY,plinhaZ,qlinhaX,qlinhaY,qlinhaZ;
bool apagarLock = false;
float apagar1X ,apagar1Y,apagar1Z;
float apagar2X,apagar2Y,apagar2Z;
float apagar3X,apagar3Y,apagar3Z;
//inicio: algoritmo diamond-square
int valorRandom(int min = 0, int max = 5)
{
	return min + (rand() % (max - min + 1));
}

//cantos
void init()
{
	mapa[0][0] = valorRandom();
	mapa[0][tamanhoLado - 1] = valorRandom();
	mapa[tamanhoLado - 1][0] = valorRandom();
	mapa[tamanhoLado - 1][tamanhoLado - 1] = valorRandom();
}

// Diamond
void diamond(int comprimentoLado)
{
	int metade = comprimentoLado / 2;

	for (int y = 0; y < tamanhoLado / (comprimentoLado-1); y++)
	{
		for (int x = 0; x < tamanhoLado / (comprimentoLado-1); x++)
		{
			int media = (mapa[ x*(comprimentoLado - 1) ][ y*(comprimentoLado - 1) ] +
						mapa[ x*(comprimentoLado - 1) ][ (y+1) * (comprimentoLado - 1) ] +
						mapa[ (x + 1) * (comprimentoLado - 1) ][y*(comprimentoLado - 1) ] +
						mapa[ (x + 1) * (comprimentoLado - 1) ][ (y + 1) * (comprimentoLado - 1 )])
						/ 4.0f;

			mapa[ x*(comprimentoLado-1) + metade][y*(comprimentoLado-1) + metade] = media + valorRandom(-range, range);
		}
	}
}


//calcular valor media : usado no square p/ ignorar pontos fora
void media(int x, int y, int comprimentoLado)
{
	int i = 0,soma = 0;

	int metade = comprimentoLado / 2;


	if (x != tamanhoLado - 1)
	{
		i++;
		soma += mapa[y][x + metade];
	}
	if (x != 0)
	{
		i++;
		soma += mapa[y][x - metade];
	}
	if (y != tamanhoLado - 1)
	{
		i++;
		soma += mapa[y + metade][x];
	}

	if (y != 0)
	{
		i++;
		soma += mapa[y - metade][x];
	}

	mapa[y][x] = (soma / i) + valorRandom(-range, range);
}

// Square
void square(int comprimentoLado)
{
	int metadeCompLado = comprimentoLado / 2;

	for (int y = 0; y < tamanhoLado / (comprimentoLado - 1); y++)
	{
		for (int x = 0; x < tamanhoLado / (comprimentoLado - 1); x++)
		{
			media(x*(comprimentoLado - 1) + metadeCompLado, y*(comprimentoLado - 1), comprimentoLado);// Cimo
			media((x + 1)*(comprimentoLado - 1), y*(comprimentoLado - 1) + metadeCompLado, comprimentoLado); //direita
			media(x*(comprimentoLado - 1) + metadeCompLado, (y+1)*(comprimentoLado - 1), comprimentoLado);// baixo
			media(x*(comprimentoLado - 1), y*(comprimentoLado - 1) + metadeCompLado, comprimentoLado); // esquerda
		}
	}
}

void diamondSquare()
{
	int comprimentoLado = tamanhoLado/2;

	diamond(tamanhoLado);
	square(tamanhoLado);

	range = range / 2;

	while (comprimentoLado >= 2)
	{
		diamond(comprimentoLado + 1);
		square(comprimentoLado + 1);
		range = range / 2;
		comprimentoLado = comprimentoLado / 2;
	}
}

//evitar valores menores que 0 e maiores que 250
void melhorarMapa()
{
	for (int i = 0; i < tamanhoLado; i++)
	{
		for (int j = 0; j < tamanhoLado; j++)
		{
		    if(mapa[i][j] < 0)
                mapa[i][j] = 0;
            if(mapa[i][j] > 200)
                mapa[i][j] = 200;
		}
	}
}

// -- fim diamond square


// inicio colisao

void normalTriangulo(float pt1x, float pt1y,float pt1z, float pt2x, float pt2y,float pt2z, float pt3x, float pt3y,float pt3z){
        aX = pt3x-pt1x; aY=pt3y-pt1y; aZ=pt3z-pt1z;
        bX = pt2x-pt1x; bY=pt2y-pt1y; bZ=pt2z-pt1z;

        normalX =aY*bZ - aZ * bY;
        normalY =aZ* bX -aX *bZ;
        normalZ = aX * bY - aY * bX;


       float mag = (float(sqrt( (normalX *normalX) +(normalY*normalY) + (normalZ*normalZ) )));
        normalX = normalX / mag;
        normalY = normalY / mag;
        normalZ = normalZ / mag;


}

void intersecaoTriangulo(){
    plinhaX=posXJogador,plinhaY=posYJogador,plinhaZ=posZJogador+300;
    qlinhaX=posXJogador,qlinhaY=posYJogador,qlinhaZ=posZJogador-300;



    float pAx= plinhaX-aX,pAy = plinhaY-aY ,pAz = 1-aZ ;

    float qAx= qlinhaX-aX,qAy = qlinhaX-aY,qAz = -1-aZ ;


    //produto escalar
    float pEscalarP_N = (pAx * normalX) + (pAy * normalY) + (pAz * normalZ);
    float pEscalarQ_N = (qAx * normalX) + (qAy * normalY) + (qAz * normalZ);

    pontoZ = ( ((-1*pEscalarP_N) -(1*pEscalarQ_N)) / (pEscalarP_N-pEscalarQ_N)   );

}

void colisao(){

    if(posXJogador < terrenoPosXInicial+tamanhoLado){ // camada grossa
        if(mapa[int(floor(posXJogador-terrenoPosXInicial))][int(floor(posYJogador-terrenoPosXInicial))] > posZJogador && posYJogador >= terrenoPosYInicial && posXJogador >= terrenoPosXInicial && posXJogador <= tamanhoLado+terrenoPosXInicial && posYJogador <= tamanhoLado+terrenoPosYInicial && posZJogador >= -20){
                posZJogador = mapa[int(floor(posXJogador-terrenoPosXInicial))][int(floor(posYJogador-terrenoPosXInicial))];

            //camada fina
                float xCoord = fmod(posXJogador-terrenoPosXInicial, 1) ;
            float yCoord = fmod(posYJogador-terrenoPosXInicial, 1) ;
                //cout <<"x: " << xCoord << " y: " << yCoord <<"\n";
               if( xCoord < yCoord){//trinagulo esquerdo
                    //cout << "ESQUERDA!\n";

                    int pt1X = int(floor(posXJogador));
                    int pt1Y =int(floor(posYJogador));
                    int pt1Z = mapa[pt1X-terrenoPosXInicial][pt1Y-terrenoPosYInicial];




                    int pt2X = int(floor(posXJogador));
                    int pt2Y = int(floor(posYJogador))+1;
                     int pt2Z = mapa[pt2X-terrenoPosXInicial][pt2Y-terrenoPosYInicial];




                    int pt3X = int(floor(posXJogador))+1;
                    int pt3Y = int(floor(posYJogador))+1;
                    int pt3Z = mapa[pt3X-terrenoPosXInicial][pt3Y-terrenoPosYInicial];


                    apagar1X = pt1X; apagar1Y = pt1Y; apagar1Z = pt1Z;
                    apagar2X = pt2X; apagar2Y = pt2Y; apagar2Z = pt2Z;
                    apagar3X = pt3X; apagar3Y = pt3Y; apagar3Z = pt3Z;



                    normalTriangulo(pt1X,pt1Y,pt1Z, pt2X,pt2Y,pt2Z, pt3X, pt3Y,pt3Z); //calcular normal do triangulo


               }

                else{// triangulo direito
              //  cout << "DIREITA!\n";

                    int pt1X = int(floor(posXJogador));
                    int pt1Y =int(floor(posYJogador));
                    int pt1Z =  mapa[pt1X-terrenoPosXInicial][pt1Y-terrenoPosYInicial];

                    int pt2X = int(floor(posXJogador))+1;
                    int pt2Y = int(floor(posYJogador));
                    int pt2Z =  mapa[pt2X-terrenoPosXInicial][pt2Y-terrenoPosYInicial];

                    int pt3X = int(floor(posXJogador))+1;
                    int pt3Y = int(floor(posYJogador))+1;
                    int pt3Z =  mapa[pt3X-terrenoPosXInicial][pt3Y-terrenoPosYInicial];



                    apagar1X = pt1X; apagar1Y = pt1Y; apagar1Z = pt1Z;
                    apagar2X = pt2X; apagar2Y = pt2Y; apagar2Z = pt2Z;
                    apagar3X = pt3X; apagar3Y = pt3Y; apagar3Z = pt3Z;

                    normalTriangulo(pt1X,pt1Y,pt1Z, pt2X,pt2Y,pt2Z, pt3X, pt3Y,pt3Z); //calcular normal do triangulo



                }

                intersecaoTriangulo(); //intersecao de linha com o triangulo
                //posZJogador = pontoZ;

        }
    }
}

void desenharPersonagem(){

 float plinhaX=posXJogador,plinhaY=posYJogador,plinhaZ=posZJogador+50;

    glColor3f(1.0f,0.0f,0.0f);
    //glLineWidth((GLfloat)5.0f);
    glBegin(GL_LINES);
    glVertex3f(plinhaX,plinhaY,plinhaZ);
    glVertex3f(qlinhaX,qlinhaY,qlinhaZ);
    glEnd();


  glColor3f(1.0,1.0,0);
     glBegin(GL_QUADS);
        //base
        glVertex3f(posXJogador-ladoPersonagem/2,posYJogador-ladoPersonagem/2,posZJogador);
        glVertex3f(posXJogador+ladoPersonagem/2, posYJogador-ladoPersonagem/2, posZJogador);
        glVertex3f(posXJogador+ladoPersonagem/2, posYJogador+ladoPersonagem/2, posZJogador);
        glVertex3f(posXJogador-ladoPersonagem/2, posYJogador+ladoPersonagem/2, posZJogador);

        //topo
        glVertex3f(posXJogador-ladoPersonagem/2,posYJogador-ladoPersonagem/2,posZJogador+50);
        glVertex3f(posXJogador+ladoPersonagem/2, posYJogador-ladoPersonagem/2, posZJogador+50);
        glVertex3f(posXJogador+ladoPersonagem/2, posYJogador+ladoPersonagem/2, posZJogador+50);
        glVertex3f(posXJogador-ladoPersonagem/2, posYJogador+ladoPersonagem/2, posZJogador+50);


        //frentre
        glVertex3f(posXJogador - ladoPersonagem/2,posYJogador + ladoPersonagem/2 ,posZJogador);
        glVertex3f(posXJogador + ladoPersonagem/2 , posYJogador + ladoPersonagem/2 , posZJogador );
          glVertex3f(posXJogador - ladoPersonagem/2,posYJogador + ladoPersonagem/2 ,posZJogador+50);
        glVertex3f(posXJogador + ladoPersonagem/2 , posYJogador + ladoPersonagem/2 , posZJogador+50);


        //tra
         glVertex3f(posXJogador - ladoPersonagem/2,posYJogador - ladoPersonagem/2 ,posZJogador);
        glVertex3f(posXJogador + ladoPersonagem/2 , posYJogador - ladoPersonagem/2 , posZJogador );
          glVertex3f(posXJogador - ladoPersonagem/2,posYJogador - ladoPersonagem/2 ,posZJogador+50);
        glVertex3f(posXJogador + ladoPersonagem/2 , posYJogador - ladoPersonagem/2 , posZJogador+50);

        glEnd();






    posZJogador+=-10; //gravidade
}
//fim colosiao
void desenharTerreno(){
for(int x=0; x<tamanhoLado-1; x++) {
        for(int y=0; y<tamanhoLado; y++) {
            float xPos = ( x + terrenoPosXInicial );
            float yPos = ( y+ terrenoPosYInicial );
            float xPos2 = xPos + 1;
            float yPos2 = yPos;
            float xPos3 = xPos + 1;
            float yPos3 = yPos +1;
            float xPos4 = xPos;
            float yPos4 = yPos + 1;
            float zPos = mapa[x][y]; float zPos2 = mapa[x+1][y];
            float zPos3 = mapa[x+1][y+1];
            float zPos4 = mapa[x][y+1];

            if(zPos < 0) zPos = 0;
            if(zPos2 < 0) zPos2 = 0;
             if(zPos3 < 0) zPos3 = 0;
              if(zPos4 < 0) zPos4 = 0;

                 glColor3f(1.0f,0.0f,1.0f);
            glBegin(GL_LINE_STRIP);

            //triangulo direita
            glVertex3f(xPos,yPos,zPos);  glVertex3f(xPos2,yPos2,zPos2);


            glVertex3f(xPos2,yPos2,zPos2);    glVertex3f(xPos3,yPos3,zPos3);

            glVertex3f(xPos3,yPos3,zPos3);    glVertex3f(xPos,yPos,zPos);


            //triangulo esquerda
            glVertex3f(xPos,yPos,zPos);  glVertex3f(xPos3,yPos3,zPos3);

            glVertex3f(xPos3,yPos3,zPos3);    glVertex3f(xPos4,yPos4,zPos4);

            glVertex3f(xPos4,yPos4,zPos4);    glVertex3f(xPos,yPos,zPos);
            glEnd();


        }
    }
    glColor3f(1.0f,0.0f,0.0f);
    glBegin(GL_TRIANGLES);
    glVertex3f(apagar1X,apagar1Y,apagar1Z);
    glVertex3f(apagar2X,apagar2Y,apagar2Z);
    glVertex3f(apagar3X,apagar3Y,apagar3Z);
    glEnd();

//DEBUG
//    glColor3f(0.0f,0.0f,1.0f);
//    glBegin(GL_LINES);
//    glVertex3f(posXJogador,posYJogador,posZJogador);
//    glVertex3f(posXJogador+normalX,apagar1Y+normalY,apagar1Z+normalZ);

    glEnd();



}

void luz(){
   //gluLookAt(1,1,1,1,1,1,1,1,1);
    //luz  ambiente
    GLfloat ambientColor[] = {0.2f,0.2f,0.2f,1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT,ambientColor);

    //posicao luz
    GLfloat lightColor0[] = {1,1,1,1.0f};
    GLfloat lightPos0[] = {400.0f,300.0f,300.0f,1.0f};
    glLightfv(GL_LIGHT0,GL_DIFFUSE,lightColor0);
    glLightfv(GL_LIGHT0,GL_POSITION,lightPos0);

    //luz direcional
    GLfloat lightColor1[] = {1,1,1.0f};
    GLfloat lightPos1[] = {400.0f,300.0f,300.0f,1.0f};
    glLightfv(GL_LIGHT1,GL_DIFFUSE,lightColor1);
    glLightfv(GL_LIGHT1,GL_POSITION,lightPos1);
}



void desenhar()
{

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glRotatef( -rotate_x, 1.0, 0.0, 0.0 );
    glRotatef( -rotate_y, 0.0, 1.0, 0.0 );
    glScalef(zoom, zoom, 1.0f);


    luz();
    colisao();



    desenharPersonagem();
    desenharTerreno();


    glEnd();

    glFlush();
    glutSwapBuffers();


 glutPostRedisplay();
}

void initGL() {

   glClearColor(0.0f, 0.0f, 0.0f, 1.0f);// fundo preto
   glClearDepth(1.0f);
   glEnable(GL_DEPTH_TEST);
   //luz
   glEnable(GL_COLOR_MATERIAL);
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glEnable(GL_LIGHT1);
   glEnable(GL_NORMALIZE);
   //fim
   glDepthFunc(GL_LEQUAL);

   glShadeModel(GL_SMOOTH);
   glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);


}

void ChangeSize(GLsizei horizontal, GLsizei vertical)
{


    GLfloat aspect = (GLfloat)horizontal / (GLfloat)vertical;
    glOrtho(0, 600, 0, 600, -600, 600);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glutReshapeWindow( 800, 600);

}

void inputRotacao( int key, int x, int y )
{


  if (key == GLUT_KEY_RIGHT ){
    rotate_y += 5;
    }
  else if (key == GLUT_KEY_LEFT)
    rotate_y -= 5;

  else if (key == GLUT_KEY_UP)
    rotate_x -= 5;

  else if (key == GLUT_KEY_DOWN)
    rotate_x += 5;

  glutPostRedisplay();

}

void moverObjeto(unsigned char key,int x, int y){
    if(key == 'a' || key == 'A'){
        posXJogador-=0.1;

    }

    else if(key == 'd' || key == 'D'){
        posXJogador+=0.1;

    }


    if(key == 'w' || key == 'W'){
        posYJogador+=0.1;
    }

    else if(key == 's' || key == 'S'){
        posYJogador-=0.1;
    }
//    else if(key =='j' || key =='J'){
//        terrenoPosXInicial -= 5;
//    }
//    else if(key =='l' || key =='L'){
//        terrenoPosXInicial += 5;
//    }
//    else if(key =='i' || key =='I'){
//        terrenoPosYInicial -= 5;
//    }
//    else if(key =='k' || key =='K'){
//        terrenoPosYInicial += 5;
//    }


    glutPostRedisplay();
}
void MouseWheel(int wheel, int direction, int x, int y)
{
    wheel=0;

    if (direction== -1 &&  zoom >= 1)
        zoom -= 0.5;

    if(zoom <= 0)
        zoom = 0.5;

    else if (direction==+1)
        zoom += 0.5;

    glutPostRedisplay();

}
int main(int argc, char * argv[])
{
	glutInit(&argc, argv);
	init();
	diamondSquare();

	melhorarMapa(); // evitar que valores sejam negativos ou superiores ao maximo

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowSize(800, 600);
    glutInitWindowPosition (150, 150);
	glutCreateWindow("colisao");

	glutReshapeFunc(ChangeSize);
	glutSpecialFunc(inputRotacao);
	glutMouseWheelFunc(MouseWheel); //funcao vem do freeglut
	glutKeyboardFunc(moverObjeto);
	glutDisplayFunc(desenhar);
	initGL();
	glutMainLoop();
}

