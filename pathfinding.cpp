//
#include <iostream>
#include <gl/GL.h>
#include <GL/glut.h>
#include <vector>
#include <fstream>
#include <iterator>
#include <conio.h> //usado para getch();
#include <GL/freeglut.h> // para funcao: glutMouseWheelFunc(); se remover esta linha, remover tambem glutMouseWheelFunc(MouseWheel);
#include <math.h>

using namespace std;

class Celula;
vector< vector<Celula> > grelha;
int  colunas, linhas; // grelha
vector<Celula> listaAberta;
vector <Celula> listaFechada;
vector<Celula> caminho;
int inicioI,inicioJ; //posicao  i,j da celula inicial
int fimI,fimJ; // posicao  i,j da celula final
int escolha=1; //escolher posicao da celulaInicial(escolha == 1) ou final ( escolha ==2)
int janela_altura = 750,janela_largura = 750;
double zoom=1.0;
int posX=0,posY=0;

int btn;
int textoUmaVez = 1;
double baseX, baseY;
double baseWidth, baseHeight;
double centerX = 0, centerY = 0;
double width = 0, height = 0;
double subirDescerTranslacao = 0, esquerdaDireitaTranslacao = 0;


void mouse( int button, int state, int mx, int my ){}


void getMouseCoords( int mx, int my, double& x, double& y )
{
 // flip mouse y axis so up is +y
    my = glutGet( GLUT_WINDOW_HEIGHT ) - my;

    x = ( mx / (double)glutGet( GLUT_WINDOW_WIDTH ) ) ;
    y = ( my / (double)glutGet( GLUT_WINDOW_HEIGHT ) ) ;
}



void motion( int mx, int my )
{




    getMouseCoords( mx, my, baseX, baseY );




    int auxI,auxJ;
    auxI = (my * colunas) / janela_largura;
    auxJ = (mx * linhas) / janela_altura;

        if(auxI < 0)
            auxI = 0;
        if(auxI > colunas-1)
            auxI = colunas -1;
        if(auxJ < 0)
            auxJ = 0;
        if(auxJ > linhas-1)
            auxJ = linhas-1;

    if(escolha == 1){// celula inicial
        inicioI = auxI;
        inicioJ = auxJ;

    }
    else if (escolha == 2){
        fimI= auxI;
        fimJ = auxJ;
    }

//    }
}




//so desenha se existir a celula inicial, final e solucao
bool solucao = false,contemCelulaInicial=false,contemCelulaFinal=false;

class Celula{
    public:
        int corR=0,corG=0,corB=0;
        int i=-1,j=-1;
        int f=0,g=0,h=0;
        Celula * anterior;
        vector<Celula *> vizinhos;
        bool parede = false;
        char tipo; // se e' caminho ou obstaculo

    Celula(){};

    void criarCelula(int _i, int _j, char _tipo ){
        i = _i;
        j = _j;
        tipo = _tipo;

        switch(tipo){ //cores dependem do tipo
            case '.': {  corR = 1.0f;corG = 1.0f; corB = 1.0f;} break; //caminho
            case 'G': {  corR = 1.0f;corG = 1.0f; corB = 1.0f;} break; //caminho
            case '@': { corR = 0.0f; corG = 1.0f; corB = 0.0f; parede = true;} break; //obstaculo
            case 'O': { corR = 0.0f; corG = 1.0f; corB = 0.0f; parede = true;} break; //obstaculo
            case 'T': { corR = 0.0f; corG = 1.0f; corB = 0.0f;parede = true;} break; //obstaculo
            case 'S': {  corR = 1.0f;corG = 1.0f; corB = 1.0f;} break; //caminho
            case 'W': {  corR = 1.0f;corG = 1.0f; corB = 1.0f;} break; //caminhod
            default: { corR = 0.0f; corG = 1.0f; corB = 0.0f;parede = true;}
        }
    }

    void adicionarVizinhos(){
        if(i < colunas-1) vizinhos.push_back( &grelha[i+1][j]);
		if(i > 0) vizinhos.push_back(&grelha[i-1][j]);
		if(j < linhas -1) vizinhos.push_back(&grelha[i][j+1]);
		if(j > 0) vizinhos.push_back(&grelha[i][j-1]);


		//diagonais
		if(i > 0 && j > 0){
		     if( !grelha[i-1][j].parede || !grelha[i][j-1].parede) // evitar que passe entre paredes
                vizinhos.push_back(&grelha[i-1][j-1]);
		}
		if(i < colunas-1 && j > 0){
		     if( !grelha[i+1][j].parede || !grelha[i][j-1].parede)
                vizinhos.push_back(&grelha[i+1][j-1]);
        }
		if(i > 0 && j < colunas-1){
		    if( !grelha[i-1][j].parede || !grelha[i][j+1].parede)
                vizinhos.push_back(&grelha[i-1][j+1]);
		}
		if(i < colunas-1 && j < linhas-1){
		    if( !grelha[i+1][j].parede || !grelha[i][j+1].parede)
                vizinhos.push_back(&grelha[i+1][j+1]);
		}
    }

};

//usados na funcao AEstrela
Celula atual;
Celula *vizinho;



int heuristica(int aI, int aJ){ //devolve distancia entre dois pontos
    return sqrt((fimI-aI)*(fimI-aI)+(fimJ-aJ)*(fimJ-aJ));
}

bool listaFechadaInclui( int i, int j){//verificar se lista fechada contem elemento com  posicao i,j
    for(int p = 0; p < listaFechada.size(); p++)
        if(i == listaFechada.at(p).i && j == listaFechada.at(p).j)
            return true;

    return false;
}

bool listaAbertaInclui( int i, int j){ //verificar se lista aberta contem elemento com  posicao i,j
    for(int p = 0; p < listaAberta.size(); p++)
        if(i == listaAberta.at(p).i && j == listaAberta.at(p).j)
            return true;

    return false;
}

void removerAtualListaAberta(){ //remove o valor atual da listaAberta
    int j=0;
    vector <Celula>::iterator it3;
    for (it3 = listaAberta.begin(); it3 != listaAberta.end(); ++it3){
        if ( it3->i == atual.i &&  it3->j == atual.j){
            listaAberta.erase(listaAberta.begin()+j);
            break;
        }
        j++;
    }
}

void criarCaminho(){ //criar path
    Celula aux;
    aux = grelha[fimI][fimJ];

    while(true){
        if((grelha[aux.i][aux.j].anterior->i == inicioI) && ( grelha[aux.i][aux.j].anterior->j  == inicioJ))
            break;
        aux.criarCelula(grelha[aux.i][aux.j].anterior->i,grelha[aux.i][aux.j].anterior->j,'.') ;
        caminho.push_back(aux);
    }
}

void desenharCelulaInicial(){
    glColor3f(1.0f,0.0f,0.0f);
    glBegin(GL_QUADS);
    glVertex3f(grelha[inicioI][inicioJ].i,grelha[inicioI][inicioJ].j,0);
    glVertex3f(grelha[inicioI][inicioJ].i+1,grelha[inicioI][inicioJ].j,0);
    glVertex3f(grelha[inicioI][inicioJ].i+1,grelha[inicioI][inicioJ].j+1,0);
    glVertex3f(grelha[inicioI][inicioJ].i,grelha[inicioI][inicioJ].j+1,0);
    glEnd();
}
void desenharCelulaFinal(){
     glColor3f(0,0,1.0);
    glBegin(GL_QUADS);
    glVertex3f(grelha[fimI][fimJ].i,grelha[fimI][fimJ].j,0);
    glVertex3f(grelha[fimI][fimJ].i+1,grelha[fimI][fimJ].j,0);
    glVertex3f(grelha[fimI][fimJ].i+1,grelha[fimI][fimJ].j+1,0);
    glVertex3f(grelha[fimI][fimJ].i,grelha[fimI][fimJ].j+1,0);
    glEnd();

}


void desenharCaminho(){
    for(int i = 0; i < caminho.size(); i++){
        float  xPos =  caminho.at(i).i;
        float yPos =  caminho.at(i).j;
        float xPos2 = xPos + 1;
        float yPos2 = yPos;
        float xPos3 = xPos + 1;
        float yPos3 = yPos +1;
        float xPos4 = xPos;
        float yPos4 = yPos + 1;
        float  zPos = 0;
        glColor3f(0.2,0.2,0.2);
        glBegin(GL_QUADS);
        glVertex3f(xPos,yPos,zPos);
        glVertex3f(xPos2,yPos2,zPos);
        glVertex3f(xPos3,yPos3,zPos);
        glVertex3f(xPos4,yPos4,zPos);
        glEnd();
    }
    desenharCelulaInicial();
}

void desenharGrelha(){
    //desenhar grelha e cor de cada celula
    for(int i=0; i<grelha.size(); i++){
        for(int j = 0; j < grelha[i].size(); j++){
            float xPos = grelha[i][j].i;
            float yPos = grelha[i][j].j;
            float xPos2 = xPos + 1;
            float yPos2 = yPos;
            float xPos3 = xPos + 1;
            float yPos3 = yPos +1;
            float xPos4 = xPos;
            float yPos4 = yPos + 1;
            float zPos = 0;

            glColor3f(grelha[i][j].corR,grelha[i][j].corG,grelha[i][j].corB);
            glBegin(GL_QUADS);
            glVertex3f(xPos,yPos,zPos);
            glVertex3f(xPos2,yPos2,zPos);
            glVertex3f(xPos3,yPos3,zPos);
            glVertex3f(xPos4,yPos4,zPos);
            glEnd();
        }
    }
}


void A_Estrela(){
    while(listaAberta.size() > 0 ) { //no inicio esta la o inicial, entao pelo menos tem que existir algum, se nao acaba e nao tem solucao
		//--1. encontrar o que tem menor valor f, esse sera o atual
		int vencedor = 0;



        for(int i = 0; i < listaAberta.size();i++){
			if(grelha[listaAberta.at(i).i][listaAberta.at(i).j].f <grelha[listaAberta.at(vencedor).i][listaAberta.at(vencedor).j].f)
				vencedor = i; // vence o que tem menor i
        }

		//--
		//2. apos encontrar o atual verificar se e' o fim. Se for termina, se nao, remover da listaAtual e colocar na listaFechada
		atual.criarCelula(listaAberta[vencedor].i,listaAberta[vencedor].j,listaAberta[vencedor].tipo); // atual = contem menor f
        atual.vizinhos = grelha[listaAberta[vencedor].i][listaAberta[vencedor].j].vizinhos;
		if(atual.i == fimI && atual.j == fimJ){//se o atual for fim criar caminho
            solucao = true;
			break;

        }



        removerAtualListaAberta();
        listaFechada.push_back(atual);

		//3. ver cada vizinhos do atual. Adicionar g a cada vizinho g = g+1
		// Se ja esta na lista fechada passar
		// se esta na listaAberta, ver se agora tem valor menor do que no ciclo anterior
		// se nao esta na listaAberta nem listaFechada, adicionar aos vizinhos do atual e adicioanar-lhe g=atual.g+1
		// depois de tudo calcular f=g+h de cada vizinho

		for(int i = 0; i <  atual.vizinhos.size();i++){//pegar em cada vizinho
			vizinho = atual.vizinhos[ i ];
			if( !listaFechadaInclui(vizinho->i,vizinho->j) && !vizinho->parede ){ //Desde que nao esteja na lista fechada
				//Pode estar na listaAberta como tambem ainda nao estar. Mas se estiver avaliar de novo pois pode ter agora um g menor que antes
				int tempG = atual.g +1; //todos os vizinhos tem g+1
				if(  listaAbertaInclui(vizinho->i,vizinho->j) ){
					if (tempG < vizinho->g){  // caso ja tenha sido avaliado, ver se tem menor g que antes
						grelha[vizinho->i][vizinho->j].g = vizinho->g;
					}
				}
				else{ // se nao esta na listaAberta nem na listaFechada, dar-lhe um g e colocar na listaAberta

					grelha[vizinho->i][vizinho->j].g = vizinho->g;
					listaAberta.push_back(grelha[vizinho->i][vizinho->j]);
				}

				//calcular f=g+h
				 grelha[vizinho->i][vizinho->j].h = heuristica(grelha[vizinho->i][vizinho->j].i,grelha[vizinho->i][vizinho->j].j);
				 grelha[vizinho->i][vizinho->j].f =  grelha[vizinho->i][vizinho->j].g +  grelha[vizinho->i][vizinho->j].h;
				//--
                grelha[vizinho->i][vizinho->j].anterior = &grelha[atual.i][atual.j];
                grelha[vizinho->i][vizinho->j].f = vizinho->f;
                grelha[vizinho->i][vizinho->j].g= vizinho->g;
                grelha[vizinho->i][vizinho->j].h= vizinho->h;
			}
		}

    }
}


int lerFicheiro( string nomeFicheiro ){
//ficheiro comeca com as seguintes linhas:
//type octile (ignorar primeira)
//height i  (guardar o valor i na variavel colunas)
//width k (guardar o valor k na variavel linhas)
//map (ignorar esta linha)
    ifstream ficheiro (nomeFicheiro.c_str());

    if (ficheiro.is_open()){

        char _tipo;
        string ignorar;
        cout << "ficheiro aberto!\n\nInstrucoes:\n";
        cout << "Cursor: Escolher posicao inicial/final" << "\n";
        cout << "Enter/Barra: confirmar posicao inicial/final" << "\n";
        cout << "tecla R: reiniciar " << "\n";


        ficheiro >> ignorar; ficheiro >> ignorar; //ignorar primeira linha
        ficheiro >> ignorar; //ignorar palavra height
        ficheiro >> colunas; // (guardar o valor  na variavel colunas)
        ficheiro >>ignorar; //ignorar palavra width
        ficheiro >> linhas; // (guardar o valor  na variavel linhas)
        ficheiro >> ignorar; //ignorar linha 'map'

        grelha.resize(colunas,vector<Celula>(linhas));


        //criar grelha com celulas
        for(int i = 0; i < colunas; i++){
            for(int j= 0; j < linhas; j++){
                ficheiro >> _tipo;
                grelha[i][j].criarCelula(i,j,_tipo);



                }
            }


            //adicionar vizinhos
            for(int i = 0; i < colunas; i++)
                for(int j= 0; j < linhas; j++)
                    grelha[i][j].adicionarVizinhos();

            return 0;
        }

        else{
            return -1;
        }
}

void desenhar()
{


    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glRotatef(-90,0,0,1);
    glScalef(1.0f, 1.0f, 1.0f);

    if(solucao){
        glScalef(zoom, zoom, 1.0f);
        if(zoom != 1)
            glTranslatef( subirDescerTranslacao, esquerdaDireitaTranslacao, 0 );
        else{
             glTranslatef( 0, 0, 0 );
        }
    }

    desenharGrelha();

    desenharCelulaInicial();
    if(escolha > 1 )
        desenharCelulaFinal();


    if(escolha > 2){
        A_Estrela();

        if(!solucao && textoUmaVez){
            cout <<"\nNao existe caminho!"<<"\n";
            textoUmaVez = 0;
        }
        else if(solucao){
            criarCaminho();
            desenharCaminho();
            if(textoUmaVez){
                zoom=1.0;
                textoUmaVez = false;
            }
       }
    }

    glEnd();
    glFlush();
    glutSwapBuffers();
    glutPostRedisplay();
}

void MouseWheel(int wheel, int direction, int x, int y)
{
    wheel=0;

    if (direction== -1 &&  zoom >= 1)
        zoom -= 0.5;

    if(zoom < 1)
        zoom = 1;

    else if (direction==+1)
        zoom += 0.5;

    glutPostRedisplay();

}

void initGL()
{
   glClearColor(0.0f, 0.0f, 0.0f, 1.0f);// fundo preto
   glClearDepth(1.0f);
   glEnable(GL_DEPTH_TEST);
   glDepthFunc(GL_LEQUAL);
   glShadeModel(GL_SMOOTH);
   glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}

void ChangeSize(GLsizei horizontal, GLsizei vertical)
{
    GLfloat aspect = (GLfloat)horizontal / (GLfloat)vertical;
    glOrtho(0,colunas, 0, linhas, -colunas,linhas);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glutReshapeWindow( janela_largura, janela_altura);

}


void confirmarCelulaTeclado(unsigned char key,int x, int y){
    if(key == ' ' || key ==13){ //ALTERA PARA ENTER
        if(escolha == 1){
            if(!grelha[inicioI][inicioJ].parede){
                listaAberta.push_back(grelha[inicioI][inicioJ]); // quando clicar ENTER
                escolha = 2;
            }
            else{
                cout << "Celula inicial nao pode ser obstaculo\n";
            }
        }

        else if(escolha == 2){
            if(!grelha[fimI][fimJ].parede){  //celula nao pode ser obstaculo
                if(inicioI != fimI && fimI != fimJ){// celula inicial tem que ser diferente da final
                    escolha = 3;
                }
                else{
                    cout <<"\nCelula inicial tem que ser diferente da celula final\n";
                }
            }
            else{
                    cout << "Celula final nao pode ser parede\n";
            }
        }
      }

      if(key == 'R' || key == 'r'){
        escolha = 1;
        listaAberta.clear();
        listaFechada.clear();
        caminho.clear();
        inicioI = 0; fimI = 0;
        inicioJ = 0; fimJ = 0;
        textoUmaVez;
      }


}

void inputTranslacao( int key, int x, int y )
{

 if(zoom > 1){
      if (key == GLUT_KEY_RIGHT ){
        esquerdaDireitaTranslacao -= 0.01f;
        }
      else if (key == GLUT_KEY_LEFT)
        esquerdaDireitaTranslacao += 0.01f;

      else if (key == GLUT_KEY_UP)
        subirDescerTranslacao += 0.01f;

      else if (key == GLUT_KEY_DOWN)
        subirDescerTranslacao -=0.01f;

      glutPostRedisplay();
  }

}

int main(int argc, char * argv[])
{
    string nomeFicheiro;
    cout << "Introduza nome do ficheiro: ";
    cin >> nomeFicheiro;
    while( lerFicheiro(nomeFicheiro) == -1 ){//erro leitura
        cout << "Erro ficheiro\n";
        getch();
        return -1;
    }




    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
    glutInitWindowSize(janela_largura, janela_altura);
    glutInitWindowPosition (50, 50);
    glutCreateWindow("Pathfinding com A*");
    glutReshapeFunc(ChangeSize);

    glutDisplayFunc(desenhar);
    glutSpecialFunc(inputTranslacao);
    glutPassiveMotionFunc( motion );
    glutMouseFunc( mouse );
    glutMouseWheelFunc(MouseWheel); //funcao vem do freeglut
    glutKeyboardFunc(confirmarCelulaTeclado);
    initGL();
    glutMainLoop();

	return 0;
}
