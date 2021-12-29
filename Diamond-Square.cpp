//algumas referencias
//https://en.wikipedia.org/wiki/Diamond-square_algorithm
//https://medium.com/@nickobrien/diamond-square-algorithm-explanation-and-c-implementation-5efa891e486f
//https://danielbeard.wordpress.com/2010/08/07/terrain-generation-and-smoothing/
//https://forum.processing.org/two/discussion/3419/diamond-square-fractal-terrain
//https://www.openprocessing.org/sketch/46391
//https://imasters.com.br/desenvolvimento/licao-tdd-geracao-de-terreno/
//

#include <iostream>
#include <gl/GL.h>
#include <GL/glut.h>

using namespace std;
const int tamanho = 513; // 2^n + 1 , por exemplo: 513  (2^9 +1 )
int range = 200; //para a funcao valorRandom
double zoom=0.5,rotate_y=0,rotate_x=0;

int mapa[tamanho][tamanho];

//valor random entre min e max
int valorRandom(int min = 0, int max = 50)
{
	return min + (rand() % (max - min + 1));
}

//cantos
void init()
{
	mapa[0][0] = valorRandom();
	mapa[0][tamanho - 1] = valorRandom();
	mapa[tamanho - 1][0] = valorRandom();
	mapa[tamanho - 1][tamanho - 1] = valorRandom();
}

// Diamond
void diamond(int comprimentoLado)
{
	int metade = comprimentoLado / 2;

	for (int y = 0; y < tamanho / (comprimentoLado-1); y++)
	{
		for (int x = 0; x < tamanho / (comprimentoLado-1); x++)
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


	if (x != tamanho - 1)
	{
		i++;
		soma += mapa[y][x + metade];
	}
	if (x != 0)
	{
		i++;
		soma += mapa[y][x - metade];
	}
	if (y != tamanho - 1)
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

	for (int y = 0; y < tamanho / (comprimentoLado - 1); y++)
	{
		for (int x = 0; x < tamanho / (comprimentoLado - 1); x++)
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
	int comprimentoLado = tamanho/2;

	diamond(tamanho);
	square(tamanho);

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
	for (int i = 0; i < tamanho; i++)
	{
		for (int j = 0; j < tamanho; j++)
		{
		    if(mapa[i][j] < 0)
                mapa[i][j] = 0;
            if(mapa[i][j] > 200)
                mapa[i][j] = 200;
		}
	}
}

void desenhar()
{

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glRotatef( -rotate_x, 1.0, 0.0, 0.0 );
    glRotatef( -rotate_y, 0.0, 1.0, 0.0 );

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



    for(int y=0; y<tamanho-1; y++) {
        for(int x=0; x<tamanho-1; x++) {
            float xPos = ( x + 100 );
            float yPos = ( y+ 100 );
            float xPos2 = xPos + 1;
            float yPos2 = yPos;
            float xPos3 = xPos + 1;
            float yPos3 = yPos +1;
            float xPos4 = xPos;
            float yPos4 = yPos + 1;
            float zPos = mapa[x][y]*2; float zPos2 = mapa[x+1][y]*2;
            float zPos3 = mapa[x+1][y+1]*2;
            float zPos4 = mapa[x][y+1]*2;


            glBegin(GL_QUADS);
           // cores diferentes a alturas diferentes, para ajudar na distincao das alturas
            if(zPos4 > 160|| zPos3 > 160|| zPos2 > 160 || zPos   > 160)
                 glColor3f(0.0f,1.0f,0.0f);
            else if(zPos4 > 100|| zPos3 > 100|| zPos2 > 100 || zPos   > 100)
                 glColor3f(0.7f,0.3f,0.2f);
            else
                glColor3f(0.0f,0.0f,1.0f);
            glVertex3f(xPos,yPos,zPos);  glVertex3f(xPos2,yPos2,zPos2);  glVertex3f(xPos3,yPos3,zPos3);  glVertex3f(xPos4,yPos4,zPos4);
            glEnd();
        }
    }

    glFlush();
    glutSwapBuffers();
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
	 if (vertical == 0)
        vertical = 1;

    GLfloat aspect = (GLfloat)horizontal / (GLfloat)vertical;

    glViewport(150, 50, horizontal, vertical);

    glOrtho(0, 1000, 0, 1000, -1000, 1000);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, aspect, 4.1f, 100.0f);
}

void inputRotacao( int key, int x, int y )
{

  if (key == GLUT_KEY_RIGHT)
    rotate_y += 5;

  else if (key == GLUT_KEY_LEFT)
    rotate_y -= 5;

  else if (key == GLUT_KEY_UP)
    rotate_x -= 5;

  else if (key == GLUT_KEY_DOWN)
    rotate_x += 5;

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
	glutCreateWindow("Algoritmo: Diamond-Square");
	glutDisplayFunc(desenhar);
	glutReshapeFunc(ChangeSize);
	glutSpecialFunc(inputRotacao);
	init();
	initGL();
	glutMainLoop();
}


