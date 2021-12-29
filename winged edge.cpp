#include <GL/glut.h>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <conio.h> //usado para getch();
#include <GL/freeglut.h> // para funcao: glutMouseWheelFunc(); se remover esta linha, remover tambem glutMouseWheelFunc(MouseWheel);


using namespace std;
class Aresta;
class Face;


class PONTO;
class MESH;


class Cor{
public:
    float corR,corG,corB;

    Cor(){}
};

class PONTO{
    public:

    float x,y,z;

    PONTO(float _x,float _y, float _z){
        x =_x;
        y = _y;
        z = _z;
    }

    PONTO(){
    }

    void setPonto(float _x,float _y,float _z){
        x = _x;
        y = _y;
        z = _z;
    }
};

class Vertice{
    public:

    Aresta * aresta;

    PONTO ponto;
    Vertice(float _x, float _y, float _z ){
       ponto.setPonto(_x,_y,_z);

    }

    Vertice(){
    }

    void setVertice(float _x, float _y, float _z){
        ponto.setPonto(_x,_y,_z);
    }


};

class Aresta{

public:
    Vertice origem,destino;
    Face *faceEsquerda, *faceDireita;
    Aresta *dirProx,  *dirAnt, *esqProx, *esqAnt;
    int marca; //-1 ->aresta inicial da face,0->arestas do meio da face, marca == 1 ->aresta final da face
    int tamanho; //saber quantos vertices existem na face onde esta esta aresta (usado para escrever no ficheiro)
    int posFace;
    int id; // (usado para escrever no ficheiro)

    Aresta(Vertice _origem,Vertice _destino){
        origem = _origem;
        destino = _destino;
    }

    Aresta(){
    }

    void setAresta(Vertice _origem,Vertice _destino){
        origem = _origem;
        destino = _destino;
    }

};


class Face{
    public:

    Aresta *aresta;
    int id;

    Face(){
    }

    void setAresta(Aresta *_aresta){
        aresta = _aresta;
    }
};

char title[] = "Mesh";

double zoom=0.5,rotate_y=0,rotate_x=0;

float x,y,z; // pontos do vertice que vao ser passados para o construtor Vertice(x,y,z)
int nVertices, nFaces, nArestas; // ler primeira linha
int faceNumVertices; //quantos vertices tem cada face
int i= 0,j = 0,idVertice; //posicao dentro do array VOV
int faceVerticeId = 0; // corresponde ao
MESH *mesh;
Aresta * ateste;
bool possivelDesenhar = false;

class MESH{
public:

    vector<Vertice > VOV; //vetor com vertices
    vector<Face> VOF; //vetor com faces
    vector<Aresta> VOA; //vetor com arestas
    vector<Cor> VCOR; //vetor com cor de cada face

    MESH( string nomeFicheiro ){ //criar geometria
        ifstream ficheiro (nomeFicheiro.c_str());

        if (ficheiro.is_open()){
            cout << "ficheiro aberto!\n";

            Vertice vertice;
            Cor auxCor;
            Aresta  *arestaAux = new Aresta();
            vector<int> FaceIndexVertice;
            Face face;
            int posInicioFace = 0;

            //ler a primeira linha para 3 variaveis
            ficheiro >> nVertices;
            ficheiro >> nFaces;
            ficheiro >> nArestas;

            // primeiro ciclo (coordenadas vertices)
            for(i = 0; i < nVertices; i++){
               ficheiro >> x; ficheiro >> y; ficheiro >> z;
               vertice.setVertice(x,y,z);
               VOV.push_back(vertice); //armazenar todos os vertices no vetor VOV
            }


            //segundo ciclo (faces)
            for(i = 0; i < nFaces; i++){
                ficheiro >> faceNumVertices; // le primeiro numero da linha ex: 4: 0 1 2 3 (ler 4)
                for(j = 0; j < faceNumVertices ; j++){
                    ficheiro >> idVertice; // primeira volta idVertice = 4, segunda volta idVertice = 0, 3volta = 1 4volta = 3
                    FaceIndexVertice.push_back(idVertice); //contem ids das faces 0 1 2 3
                }



                for(int k = 0; k < faceNumVertices; k++){ //para cada face ex 4: 0 1 2 3 4

                    if(k == 0){ //pegar no primeiro elemento 0  (4: 0 1 2 3 4)
                        arestaAux->setAresta(VOV.at(FaceIndexVertice.at(0)), VOV.at(FaceIndexVertice.at(1) ) ); //criar aresta 0->1
                        arestaAux->marca = -1; //inicial
                        arestaAux->tamanho = faceNumVertices;
                        arestaAux->posFace = i;

                        arestaAux->id =FaceIndexVertice.at(0);

                        //Dar a cada vertice uma aresta
                        VOV.at( FaceIndexVertice.at(0) ).aresta=  arestaAux;
                        VOV.at( FaceIndexVertice.at(1) ).aresta= arestaAux;
                    }
                    else if(k < faceNumVertices-1){//meio (criar aresta 1->2 , 2->3, 3->4
                        arestaAux->setAresta(VOV.at(FaceIndexVertice.at(k)), VOV.at(FaceIndexVertice.at(k+1) ) );
                        arestaAux->marca = 0;
                        arestaAux->posFace = i;

                        arestaAux->id =FaceIndexVertice.at(k);

                        //Dar a cada vertice uma aresta
                        VOV.at( FaceIndexVertice.at(k) ).aresta=  arestaAux;

                        VOV.at( FaceIndexVertice.at(k+1) ).aresta= arestaAux;

                    }
                    else{ // ultima aresta  (liga ultimo ao primeiro, criar aresta 4->0)
                        arestaAux->setAresta(VOV.at(FaceIndexVertice.at(k)), VOV.at(FaceIndexVertice.at(0) ) );
                        arestaAux->marca = 1; // final
                        arestaAux->posFace = i;
                        arestaAux->id =FaceIndexVertice.at(k);

                        //Dar a cada vertice uma aresta
                        VOV.at( FaceIndexVertice.at(k) ).aresta=  arestaAux;
                         //VOA.push_back(*arestaAux);

                    }

                    VOA.push_back(*arestaAux);
				}

                //cores de cada face
                ficheiro >>auxCor.corR;
                ficheiro >> auxCor.corG;
                ficheiro >> auxCor.corB;
                VCOR.push_back(auxCor);

                //limpar FaceIndex, para que na proxima volta (proxima face) consiga aceder as posicoes de cada face e nao de outras. exemplo:
                //4: 0 1 2 3 4
                //4: 5 6 7 8
                //VOV.at(FaceIndexVertice.at(0)) na segunda volta, depois de fazer .clear() conseguimos aceder ao 5
                //sem .clear() VOV.at(FaceIndexVertice.at(0)) iria aceder sempre ao valor 0

                FaceIndexVertice.clear();
            }



            //ligacoes  aresta ( esquerda)
            for(unsigned int i = 0; i< VOA.size(); i++){
                if(VOA.at(i).marca == -1){ //aresta INICIAL
                    posInicioFace = i;

                    VOA.at(i).esqProx = &VOA.at(i+1);
                    face.aresta =  &VOA.at(i) ;
                    face.id = VOA.at(i).posFace;
                    VOF.push_back(face);

                    VOA.at(i).faceEsquerda = &VOF.at(VOA.at(i).posFace);

                    //(*) esqAnt do primeiro definido na ultima aresta face (marca == 1)

                    }
                    else if(VOA.at(i).marca == 0){//MEIO

                        VOA.at(i).esqProx = &VOA.at(i+1);
                        VOA.at(i).esqAnt = &VOA.at(i-1);
                        VOA.at(i).faceEsquerda = &VOF.at(VOA.at(i).posFace);

                    }
                    else if (VOA.at(i).marca == 1){ //FINAL

                        VOA.at(i).esqProx = &VOA.at(posInicioFace);
                        VOA.at(i).esqAnt = &VOA.at(i-1);
                        VOA.at(i).faceEsquerda = &VOF.at(VOA.at(i).posFace);

                        //(*) esqAnt do primeiro definido na ultima aresta face (marca == 1)
                        VOA.at(posInicioFace).esqAnt = &VOA.at(i); //anterior do primeiro corresponde a este

                    }
          }

           //ligacoes aresta (direita)
           for(unsigned int i = 0; i< VOA.size(); i++){
                // ex aresta 0->1 :  porcurar aresta 1->0 e a face esquerda de 1->0 sera a face direita de 0->1   (para cada VOA (i), procura (k))
                     for(unsigned int k = 0 ; k < VOA.size();k++){
                             if( ((VOA.at(i).origem.ponto.x ==  VOA.at(k).destino.ponto.x) && (VOA.at(i).origem.ponto.y ==  VOA.at(k).destino.ponto.y) && (VOA.at(i).origem.ponto.z ==  VOA.at(k).destino.ponto.z)
                                   && ( (VOA.at(i).destino.ponto.x ==  VOA.at(k).origem.ponto.x) && (VOA.at(i).destino.ponto.y ==  VOA.at(k).origem.ponto.y) && (VOA.at(i).destino.ponto.z ==  VOA.at(k).origem.ponto.z)   )
                                  )

                                ){
                                 VOA.at(i).faceDireita = VOA.at(k).faceEsquerda;
                                 VOA.at(i).dirProx = VOA.at(k).esqProx;
                                 VOA.at(i).dirAnt = VOA.at(k).esqAnt;
                                break;
                            }
                       }
                    }

            possivelDesenhar = true;
            ficheiro.close();
        }
        else{
           cout <<"Erro ficheiro!";

           getch();
        }
    }

    void escreverFicheiro(){
         ofstream fout;
         fout.open("ficheiro.off");

         //primeira linha
        fout << VOV.size()  << " ";
        fout << VOF.size() << " ";
        fout << nArestas  << "\n";

        //proximas linhas (coordenadas dos vertices)
        for(unsigned int i = 0; i < VOV.size();i++){
            fout << VOV.at(i).ponto.x << " " << VOV.at(i).ponto.y << " " << VOV.at(i).ponto.z <<"\n";
        }

        //proximas linhas (faces)
        Aresta *auxA;
        for(unsigned int i = 0; i < VOF.size(); i++){ // percorrer cada face

           if( VOF.at(i).aresta->marca == -1 ){ // primeira aresta ex: 4: 0 1 2 3 4 (corresponde ao 0)
                auxA = VOF.at(i).aresta;
                fout << auxA->tamanho;
                fout << " "<< auxA->id;
                auxA = auxA->esqProx;
           }


            while(auxA != VOF.at(i).aresta){ //arestas do meio ex: 4: 0 1 2 3 4 (corresponde a 1 2 3 4)
                fout << " "<< auxA->id; //escrever a posicao de cada vertice


                if(auxA->marca == 1)//escrever cor quando chegar na ultima aresta da face
                   fout <<" " <<VCOR.at(i).corR <<" "<<VCOR.at(i).corG  << " "<< VCOR.at(i).corB <<"\n";

                auxA = auxA->esqProx;
            }
        }

        cout <<"\n" << "ficheiro.off criado" <<"\n";
    }

    void desenhar(){

        Aresta *auxA;

        for(unsigned int i = 0; i < VOF.size(); i++){ //percorrer todas as faces

            if( VOF.at(i).aresta->marca == -1 ){
                auxA = VOF.at(i).aresta;
                glBegin(GL_POLYGON);

                if(auxA->esqAnt->marca == 1){//cor esta guardado na ultima aresta da face, caso haja erro cor=>vermelha
                    glColor3f(VCOR.at(i).corR,VCOR.at(i).corG,VCOR.at(i).corB);
                }
                else{
                    glColor3f(1.0f,0.0f,0.0f);
                }

                glVertex3f(auxA->origem.ponto.x , auxA->origem.ponto.y, auxA->origem.ponto.z);
                auxA = auxA->esqProx;

           }



            while(auxA != VOF.at(i).aresta){
                auxA = auxA->esqProx;
                glVertex3f(auxA->origem.ponto.x , auxA->origem.ponto.y, auxA->origem.ponto.z);
            }

            glEnd();


          }


          for(unsigned int i = 0; i < VOF.size(); i++){

           if( VOF.at(i).aresta->marca == -1 ){
                auxA = VOF.at(i).aresta;
                glBegin(GL_POLYGON);
                if(auxA->esqAnt->marca == 1){//cor esta guardado na ultima aresta da face, caso haja erro cor=>vermelha
                    glColor3f(VCOR.at(i).corR,VCOR.at(i).corG,VCOR.at(i).corB);
                }
                else{
                    glColor3f(1.0f,0.0f,0.0f);
                }

                glVertex3f(auxA->origem.ponto.x , auxA->origem.ponto.y, auxA->origem.ponto.z);
                auxA = auxA->esqAnt;

           }



            while(auxA != VOF.at(i).aresta){
                auxA = auxA->esqAnt;
                glVertex3f(auxA->origem.ponto.x , auxA->origem.ponto.y, auxA->origem.ponto.z);
            }

             glEnd();

          }
    }
};

void initGL() {

   glClearColor(0.0f, 0.0f, 0.0f, 1.0f);// fundo preto
   glClearDepth(1.0f);
   glEnable(GL_DEPTH_TEST);
   glDepthFunc(GL_LEQUAL);
   glShadeModel(GL_SMOOTH);
   glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

}

void display() {

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glRotatef( rotate_x, 1.0, 0.0, 0.0 );
    glRotatef( rotate_y, 0.0, 1.0, 0.0 );
    glScalef(zoom, zoom, 1.0f);
    mesh->desenhar();

    glFlush();
    glutSwapBuffers();

}

void Keys( int key, int x, int y )
{

  if (key == GLUT_KEY_RIGHT)
    rotate_y += 10;

  else if (key == GLUT_KEY_LEFT)
    rotate_y -= 10;

  else if (key == GLUT_KEY_UP)
    rotate_x -= 10;

  else if (key == GLUT_KEY_DOWN)
    rotate_x += 10;

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

void reshape (int w, int h)
{
    glViewport (0,0, (GLsizei) w, (GLsizei) h);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    glMatrixMode(GL_MODELVIEW);

}

int main(int argc, char** argv) {

    string nomeFicheiro;
    cout << "Introduza nome do ficheiro: ";
    cin >> nomeFicheiro;
    mesh = new MESH(nomeFicheiro);

    mesh->escreverFicheiro();

    if(possivelDesenhar){
        glutInit(&argc,argv);
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
        glutInitWindowSize(640, 480);
        glutInitWindowPosition (150, 150);
        glutCreateWindow(title);
        glEnable(GL_DEPTH_TEST);
        glutDisplayFunc(display);
        glutReshapeFunc(reshape);
        glutSpecialFunc(Keys);
        glutMouseWheelFunc(MouseWheel); //funcao vem do freeglut
        glutMainLoop();
    }

    return 0;
}
