

//Peut-être faire en 3D
//Savoir pourquoi à la fin c'est plus rapide
//Jouer la musique en même temps
//Peut-être inverser les couleurs
//Gérer le temps par rapport à la longueur de l'audio et peut-être changer les 25ms... 
//Pouvoir changer les audios
//Peut-être ajouter une interface
//Le truc s'arrête vers 1min alors que la musique fait 2min34




//
//  main.cpp
//  ANREC_OGL_FirstEx
//
//  Created by Jean-Marie Normand on 24/09/2014.
//  Copyright (c) 2014 Centrale Innovation. All rights reserved.
//


#include <Windows.h>
#include <iostream>
using namespace std;
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sndfile.hh>
#include <fftw3.h>
#include <glew.h>
#include <glut.h>
// Fichiers d'en-têtes pour OpenGL et GLUT
#ifdef __APPLE__
#include <OPENGL/gl.h>
#else
#include <GL/gl.h>
#endif

#define N 512
int increment = 0;


void vTimerIdle(int i)
{
    increment++;
    glutPostRedisplay();
    glutTimerFunc(25, vTimerIdle, i);    // On choisit un timer de 25ms
}

float valeurAbsolue(float f) {
    if (f < 0) return f;
    return f;
}



// Helper function to find next power of 2 using bit twiddling
int next_pow_2(int x) {
    x--; // pour tout x, renvoie la puissance de 2 supérieure la plus proche.
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    x++;
    return x;
}


// Quelques variables globales (c'est pas bien)
GLfloat pointSize = 1.0f;

// Déclarations des fonctions de rappel (callbacks)
GLvoid affichage();
GLvoid clavier(unsigned char touche, int x, int y);

int main(int argc, char* argv[])
{
	// Initialisation de GLUT
	glutInit(&argc, argv);
	// Choix du mode d'affichage (ici RVB)
	glutInitDisplayMode(GLUT_RGB);
	// Position initiale de la fenetre GLUT
	glutInitWindowPosition(50, 50);
	// Taille initiale de la fenetre GLUT
	glutInitWindowSize(400, 400);
	// Creation de la fenetre GLUT
	glutCreateWindow("Premier Exemple");

	// Définition de la couleur d'effacement du framebuffer
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// Définition des fonctions de callbacks
	glutDisplayFunc(affichage);
	glutKeyboardFunc(clavier);


    glutTimerFunc(25, vTimerIdle, 1); //25 ms

	// Lancement de la boucle infinie GLUT
	glutMainLoop();

	return 0;
}

// Definition de la fonction d'affichage
GLvoid affichage() {
    // Effacement du frame buffer
    glClear(GL_COLOR_BUFFER_BIT);
    //glMatrixMode(GL_MODELVIEW);


    cout << "Appel des fichiers d'entrée" << endl;
    string entree = "megaman_mono.wav";

    cout << "Conversion en objet soundfile" << endl;
    SndfileHandle audio = SndfileHandle(entree);
    if (audio.channels() != 1) {
        cout << "ERROR: Only taking mono files for this example" << endl;
        exit(EXIT_FAILURE);
    }

    cout << "--------------" << endl;
    cout << " CALCUL DE LA FREQUENCE D'ECHANTILLONNAGE " << endl;
    cout << "Donnees :" << endl;
    cout << "taille de l'audio : " << audio.frames() << endl;
    int duree_s = 1;
    cout << "duree de l'audio en secondes : " << duree_s << endl;
    cout << "frequence d'echantillonnage (en Hz) : " << audio.frames() / duree_s << endl;



    int padded_length = next_pow_2(audio.frames());
    cout << "Creation d'une ARRAY FFTW3. Adding 2 to do in-place FFT" << endl;
    float* input_buffer = fftwf_alloc_real((size_t)padded_length + 2);

    cout << "On remplit l'ARRAY avec l'audio" << endl;
    audio.readf(input_buffer, audio.frames());
    for (int i = audio.frames(); i < padded_length; i++) {
        input_buffer[i] = 0.0f;
    }
    float* input_buf = fftwf_alloc_real((size_t)N);
    for (int i = 0; i < N; i++) {
        input_buf[i] = input_buffer[i + N * increment] * pow(10, 5);
    }


    cout << "Creation du FFT plan" << endl;
    float* output_buf = fftwf_alloc_real((size_t)N);
    fftwf_plan in_plan = fftwf_plan_dft_r2c_1d(N, input_buf, (fftwf_complex*)output_buf,
        FFTW_ESTIMATE);

    cout << "Execution" << endl;
    fftwf_execute(in_plan);
    

    
    float moyenne[8] = { 0,0,0,0,0,0,0,0 };//contient les moyennes des amplitudes pour les 8 plages
    for (int i = 0; i < 8; i++) {
        for (int k = 0; k < N / 8; k++) {
            moyenne[i] += valeurAbsolue(output_buf[k+i*N/8]);
        }
        moyenne[i] *=  8./N;
    }



    float max[8][2];
    for (int k = 0; k < 8; k++) {
        max[k][0] = k;
        max[k][1] = moyenne[k];
    }
    //max contient l'indice et la valeur ranger par ordre décroissant par rapport à la valeur de l'amplitude

    int temp[2];
    for (int i = 0; i < 7; i++){
        for (int j = i + 1; j < 8; j++){
            if (max[i][1] < max[j][1]) {
                temp[1] = max[i][1];
                temp[0] = max[i][0];
                max[i][1] = max[j][1];
                max[i][0] = max[j][0];
                max[j][1] = temp[1];
                max[j][0] = temp[0];
            }
        }
    }


    GLUquadric* quadric = gluNewQuadric();
    for (int k = 0; k < 8; k++) {
        switch ((int)max[k][0])
        {
            case 0:
                glColor3ub(255, 0, 0); // rouge
                gluCylinder(quadric, 0.8, 0.9, 2-max[k][1]/ max[0][1], 50, 50);
                break;
            case 1:
                glColor3ub(255, 165, 0); // orange
                gluCylinder(quadric, 0.7, 0.8, 2 - max[k][1] / max[0][1], 50, 50);
                break;
            case 2:
                glColor3ub(255, 255, 0); // jaune
                gluCylinder(quadric, 0.6, 0.7, 2 - max[k][1] / max[0][1], 50, 50);
                break;
            case 3:
                glColor3ub(0, 255, 0); // vert
                gluCylinder(quadric, 0.5, 0.6, 2 - max[k][1] / max[0][1], 50, 50);
                break;
            case 4:
                glColor3ub(23, 101, 125); // bleu-vert
                gluCylinder(quadric, 0.4, 0.5, 2 - max[k][1] / max[0][1], 50, 50);
                break;
            case 5:
                glColor3ub(0, 0, 255); // bleu
                gluCylinder(quadric, 0.3, 0.4, 2 - max[k][1] / max[0][1], 50, 50);
                break;
            case 6:
                glColor3ub(127, 0, 255); // violet
                gluCylinder(quadric, 0.2, 0.3, 2 - max[k][1] / max[0][1], 50, 50);
                break;
            case 7:
                glColor3ub(139, 0, 139); // mauve
                gluCylinder(quadric, 0.1, 0.2, 2 - max[k][1] / max[0][1], 50, 50);
                break;
        }
             
    
    }
  

	//ROUGE=BASSE FREQ AU CENTRE
	glFlush();
}

// Definition de la fonction gerant les interruptions clavier
GLvoid clavier(unsigned char touche, int x, int y) {

	// Suivant les touches pressees, nous aurons un comportement different de l'application
	// ESCAPE ou 'q' : fermera l'application
	// 'p' : affichage du carre plein
	// 'f' : affichage du carre en fil de fer
	// 's' : affichage des sommets du carre

	switch (touche) {
	case 'p': // carre plein
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case 'f': // fil de fer

		break;
	case 's': // sommets du carre

		break;

	case '+':
		// Augmenter la taille des sommets affiches
		increment++;
		break;
	case '-':
		// Diminuer la taille des sommets affiches
		increment--;
		break;


	case 'q': // quitter
	case 27:
		exit(0);
		break;
	}

	// Demande a GLUT de reafficher la scene
	glutPostRedisplay();
}



/*
#include <Windows.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sndfile.hh>
#include <fftw3.h>
#include <glew.h>
#include <glut.h>

#define GLM_FORCE_RADIANS
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "shader_utils.h"
#define N 512
using namespace std;
GLuint program;
GLint attribute_coord2d;
GLint uniform_color;
GLint uniform_transform;

float offset_x = 0;
float scale_x = 1;
int increment=0;
struct point {
    GLfloat x;
    GLfloat y;
};

GLuint vbo[3];

const int border = 10;
const int ticksize = 10;


void vTimerIdle(int i)
{
    if(increment<12) increment++;
    glutPostRedisplay();
    glutTimerFunc(25, vTimerIdle, i);    // On choisit un timer de 25ms
}





// Helper function to find next power of 2 using bit twiddling
int next_pow_2(int x) {
    x--; // pour tout x, renvoie la puissance de 2 supérieure la plus proche.
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    x++;
    return x;
}

int init_resources() {
    program = create_program("graph.v.glsl", "graph.f.glsl");
    if (program == 0)
     return 0;

    attribute_coord2d = get_attrib(program, "coord2d");
    uniform_transform = get_uniform(program, "transform");
    uniform_color = get_uniform(program, "color");

    if (attribute_coord2d == -1 || uniform_transform == -1 || uniform_color == -1)
     return 0;

    // Create the vertex buffer object
    glGenBuffers(3, vbo);


    // Create a VBO for the border
    static const point border[4] = { {-1, -1}, {1, -1}, {1, 1}, {-1, 1} };
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof border, border, GL_STATIC_DRAW);

    return 1;
}

// Create a projection matrix that has the same effect as glViewport().
// Optionally return scaling factors to easily convert normalized device coordinates to pixels.
//
glm::mat4 viewport_transform(float x, float y, float width, float height, float* pixel_x = 0, float* pixel_y = 0) {
    // Map OpenGL coordinates (-1,-1) to window coordinates (x,y),
    // (1,1) to (x + width, y + height).

    // First, we need to know the real window size:
    float window_width = glutGet(GLUT_WINDOW_WIDTH);
    float window_height = glutGet(GLUT_WINDOW_HEIGHT);

    // Calculate how to translate the x and y coordinates:
    float offset_x = (2.0 * x + (width - window_width)) / window_width;
    float offset_y = (2.0 * y + (height - window_height)) / window_height;

    // Calculate how to rescale the x and y coordinates:
    float scale_x = width / window_width;
    float scale_y = height / window_height;

    // Calculate size of pixels in OpenGL coordinates
    if (pixel_x)
     *pixel_x = 2.0 / width;
    if (pixel_y)
     *pixel_y = 2.0 / height;

    return glm::scale(glm::translate(glm::mat4(1), glm::vec3(offset_x, offset_y, 0)), glm::vec3(scale_x, scale_y, 1));
}

void display() {
    int window_width = glutGet(GLUT_WINDOW_WIDTH);
    int window_height = glutGet(GLUT_WINDOW_HEIGHT);

    glUseProgram(program);

    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT);


    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);


    int RANG = 512;


    cout << "Appel des fichiers d'entrée" << endl;
    string entree = "megaman_mono.wav";

    cout << "Conversion en objet soundfile" << endl;
    SndfileHandle audio = SndfileHandle(entree);
    if (audio.channels() != 1) {
     cout << "ERROR: Only taking mono files for this example" << endl;
     exit(EXIT_FAILURE);
    }

    cout << "--------------" << endl;
    cout << " CALCUL DE LA FREQUENCE D'ECHANTILLONNAGE " << endl;
    cout << "Donnees :" << endl;
    cout << "taille de l'audio : " << audio.frames() << endl;
    int duree_s = 1;
    cout << "duree de l'audio en secondes : " << duree_s << endl;
    cout << "frequence d'echantillonnage (en Hz) : " << audio.frames() / duree_s << endl;



    int padded_length = next_pow_2(audio.frames());
    cout << "Creation d'une ARRAY FFTW3. Adding 2 to do in-place FFT" << endl;
    float* input_buffer = fftwf_alloc_real((size_t)padded_length + 2);

    cout << "On remplit l'ARRAY avec l'audio" << endl;
    audio.readf(input_buffer, audio.frames());
    for (int i = audio.frames(); i < padded_length; i++) {
     input_buffer[i] = 0.0f;
    }
    float* input_buf = fftwf_alloc_real((size_t)N);
    for (int i = 0; i < N; i++) {
     input_buf[i] = input_buffer[i + N*increment];
    }


    cout << "Les " << RANG << " premiers elmts de input_buf" << endl;
    for (int i = 0; i < RANG; i++) {
     cout << input_buf[i] << endl;
    }


    cout << "Creation du FFT plan" << endl;
    float* output_buf = fftwf_alloc_real((size_t)N);
    fftwf_plan in_plan = fftwf_plan_dft_r2c_1d(N, input_buf, (fftwf_complex*)output_buf,
     FFTW_ESTIMATE);

    cout << "Execution" << endl;
    fftwf_execute(in_plan);
    cout << "Contenu de output_buf" << endl;
    for (int i = 0; i < RANG; i++) {
     cout << output_buf[i] << endl;
    }

    // Create our own temporary buffer
    point graph[N];



    // Fill it in just like an array
    for (int i = 0; i < N; i++) {
     float x = (i - N / 2) / 100.0;

     graph[i].x = x;
     graph[i].y = 10000 * output_buf[i] / 100.0;

    }





    // Tell OpenGL to copy our array to the buffer object
    glBufferData(GL_ARRAY_BUFFER, sizeof graph, graph, GL_STATIC_DRAW);



    // ----------------------------------------------------------------
    // Draw the graph

    // Set our viewport, this will clip geometry
    glViewport(border + ticksize, border + ticksize, window_width - border * 2 - ticksize, window_height - border * 2 - ticksize);

    // Set the scissor rectangle,this will clip fragments
    glScissor(border + ticksize, border + ticksize, window_width - border * 2 - ticksize, window_height - border * 2 - ticksize);

    glEnable(GL_SCISSOR_TEST);

    // Set our coordinate transformation matrix
    glm::mat4 transform = glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(scale_x, 1, 1)), glm::vec3(offset_x, 0, 0));
    glUniformMatrix4fv(uniform_transform, 1, GL_FALSE, glm::value_ptr(transform));

    // Set the color to red
    GLfloat red[4] = { 1, 0, 0, 1 };
    glUniform4fv(uniform_color, 1, red);

    // Draw using the vertices in our vertex buffer object
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);

    glEnableVertexAttribArray(attribute_coord2d);
    glVertexAttribPointer(attribute_coord2d, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glDrawArrays(GL_LINE_STRIP, 0, 2000);

    // Stop clipping
    glViewport(0, 0, window_width, window_height);
    glDisable(GL_SCISSOR_TEST);

    // ----------------------------------------------------------------
    // Draw the borders

    float pixel_x, pixel_y;

    // Calculate a transformation matrix that gives us the same normalized device coordinates as above
    transform = viewport_transform(border + ticksize, border + ticksize, window_width - border * 2 - ticksize, window_height - border * 2 - ticksize, &pixel_x, &pixel_y);

    // Tell our vertex shader about it
    glUniformMatrix4fv(uniform_transform, 1, GL_FALSE, glm::value_ptr(transform));

    // Set the color to black
    GLfloat black[4] = { 0, 0, 0, 1 };
    glUniform4fv(uniform_color, 1, black);

    // Draw a border around our graph
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glVertexAttribPointer(attribute_coord2d, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glDrawArrays(GL_LINE_LOOP, 0, 4);

    // ----------------------------------------------------------------
    // Draw the y tick marks

    point ticks[42];

    for (int i = 0; i <= 20; i++) {
     float y = -1 + i * 0.1;
     float tickscale = (i % 10) ? 0.5 : 1;

     ticks[i * 2].x = -1;
     ticks[i * 2].y = y;
     ticks[i * 2 + 1].x = -1 - ticksize * tickscale * pixel_x;
     ticks[i * 2 + 1].y = y;
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof ticks, ticks, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(attribute_coord2d, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glDrawArrays(GL_LINES, 0, 42);

    // Draw the x tick marks

    float tickspacing = 0.1 * powf(10, -floor(log10(scale_x)));    // desired space between ticks, in graph coordinates
    float left = -1.0 / scale_x - offset_x;    // left edge, in graph coordinates
    float right = 1.0 / scale_x - offset_x;    // right edge, in graph coordinates
    int left_i = ceil(left / tickspacing);    // index of left tick, counted from the origin
    int right_i = floor(right / tickspacing);    // index of right tick, counted from the origin
    float rem = left_i * tickspacing - left;    // space between left edge of graph and the first tick

    float firsttick = -1.0 + rem * scale_x;    // first tick in device coordinates

    int nticks = right_i - left_i + 1;    // number of ticks to show

    if (nticks > 21)
     nticks = 21;    // should not happen

    for (int i = 0; i < nticks; i++) {
     float x = firsttick + i * tickspacing * scale_x;
     float tickscale = ((i + left_i) % 10) ? 0.5 : 1;

     ticks[i * 2].x = x;
     ticks[i * 2].y = -1;
     ticks[i * 2 + 1].x = x;
     ticks[i * 2 + 1].y = -1 - ticksize * tickscale * pixel_y;
    }

    glBufferData(GL_ARRAY_BUFFER, sizeof ticks, ticks, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(attribute_coord2d, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glDrawArrays(GL_LINES, 0, nticks * 2);

    // And we are done.

    glDisableVertexAttribArray(attribute_coord2d);
    glutSwapBuffers();
}

void special(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_LEFT:
     offset_x -= 0.03;
     break;
    case GLUT_KEY_RIGHT:
     offset_x += 0.03;
     break;
    case GLUT_KEY_UP:
     scale_x *= 1.5;
     break;
    case GLUT_KEY_DOWN:
     scale_x /= 1.5;
     break;
    case GLUT_KEY_HOME:
     offset_x = 0.0;
     scale_x = 1.0;
     break;
    }

    glutPostRedisplay();
}

void free_resources() {
    glDeleteProgram(program);
}

int main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB);
    glutInitWindowSize(640, 480);
    glutCreateWindow("My Graph");

    GLenum glew_status = glewInit();

    if (GLEW_OK != glew_status) {
     fprintf(stderr, "Error: %s\n", glewGetErrorString(glew_status));
     return 1;
    }

    if (!GLEW_VERSION_2_0) {
     fprintf(stderr, "No support for OpenGL 2.0 found\n");
     return 1;
    }

    printf("Use left/right to move horizontally.\n");
    printf("Use up/down to change the horizontal scale.\n");
    printf("Press home to reset the position and scale.\n");

    if (init_resources()) {
     glutDisplayFunc(display);
     glutSpecialFunc(special);
     glutTimerFunc(25, vTimerIdle, 1); //25 ms
     glutMainLoop();
    }

    free_resources();
    return 0;
}

*/





/*
#include <windows.h>
#include <gl/gl.h>
#include <freeglut.h>

#define WIDTH 640	// Taille de la fenêtre
#define HEIGHT 480	
#define NBVOITURE 4		// Nombre de voitures dans une file


// Variables du programme

float fLigne = (float)0.1;	// Taille d'une ligne en pourcentage de la taille totale de la fenêtre
float fDistance[NBVOITURE];
float fEcart;

// Déclaration des fonctions
void vDisplay();
void vReshape(int w, int h);
void vTimerIdle(int i);
void vQuadrilatere(float fRouge, float fVert, float fBleu, float fAx, float fAy,
	float fBx, float fBy, float fCx, float fCy, float fDx, float fDy,
	float fHori, float fVerti);

void vTimerIdle(int i)
{
	int k; // Compteur
	for (k = 0; k < NBVOITURE; k++)
	{
		fDistance[k] += 3;
	}
	glutPostRedisplay();
	glutTimerFunc(20, vTimerIdle, i);	// On choisit un timer de 20ms
}


// Programme principal
int main(int argc, char* argv[])
{
	int i; //Compteur

	fEcart = WIDTH / (NBVOITURE - 1);

	for (i = 0; i < NBVOITURE; i++)
	{
		fDistance[i] = i * fEcart; // Réinitialisé lorsque la voiture sort de l'écran
	}

	

	glutInit(&argc, argv);	// initialisation de GLUT : argc et argv sont respectivement 
				// le nombre et la liste des paramètres passées en ligne de commande
	
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	// GLUT_RGBA mode "vraies couleurs" 32 bits
	// GLUT_DOUBLE "double buffering" - deux tampons


	glutInitWindowSize(WIDTH, HEIGHT); // Initialisation de la largeur et de la hauteur de la fenêtre
	glutInitWindowPosition(50, 50); // Position de la fenêtre sur l'écran par rapport au coin haut gauche
	glutCreateWindow("Grenouf"); // Titre de la fenêtre
	glutDisplayFunc(vDisplay); // précise la fonction à utiliser pour l'affichage 
	glutReshapeFunc(vReshape); // précise la fonction à utiliser pour le redimensionnement 
	glutTimerFunc(20, vTimerIdle, 1); //20 ms
	glutMainLoop(); // lance le gestionnaire glut
	return 0;
}

void vDisplay()
{

	//Variables locales	
	int i;



	glClearColor(0, 0, 0, 0); // selectionne la couleur noire (qui est celle par défaut)
	glClear(GL_COLOR_BUFFER_BIT); // efface le frame buffer


// Trottoir de départ
	vQuadrilatere(0.75, 0.75, 0.75, -WIDTH / 2, -HEIGHT / 2,
		WIDTH / 2, -HEIGHT / 2, WIDTH / 2, -(HEIGHT / 2 - fLigne * HEIGHT),
		-WIDTH / 2, -(HEIGHT / 2 - fLigne * HEIGHT),
		0, 0);

	// 1ère file de la route
	vQuadrilatere(0.40, 0.40, 0.40, WIDTH / 2, -(HEIGHT / 2 - fLigne * HEIGHT),
		-WIDTH / 2, -(HEIGHT / 2 - fLigne * HEIGHT), -WIDTH / 2, -(HEIGHT / 2 - 2 * fLigne * HEIGHT),
		WIDTH / 2, -(HEIGHT / 2 - 2 * fLigne * HEIGHT),
		0, 0);

	// Trottoir du milieu
	vQuadrilatere(0.75, 0.75, 0.75, WIDTH / 2, -(HEIGHT / 2 - 2 * fLigne * HEIGHT),
		-WIDTH / 2, -(HEIGHT / 2 - 2 * fLigne * HEIGHT), -WIDTH / 2, -(HEIGHT / 2 - 3 * fLigne * HEIGHT),
		WIDTH / 2, -(HEIGHT / 2 - 3 * fLigne * HEIGHT),
		0, 0);

	// "Grenouille"
	vQuadrilatere(0, 0.46, 0.25, -fLigne * HEIGHT / 2, -HEIGHT / 2, fLigne * HEIGHT / 2, -HEIGHT / 2, fLigne * HEIGHT / 2,
		-(HEIGHT / 2 - fLigne * HEIGHT), -fLigne * HEIGHT / 2, -(HEIGHT / 2 - fLigne * HEIGHT), 0, 0);

	// Boucle de création des voitures
	for (i = 0; i < NBVOITURE; i++)
	{
		// Voiture i
		if (fDistance[i] >= WIDTH + fEcart) fDistance[i] = 0;

		vQuadrilatere(0.25, 0, 0.46, -WIDTH / 2 - fLigne * HEIGHT, -(HEIGHT / 2 - fLigne * HEIGHT),
			-WIDTH / 2, -(HEIGHT / 2 - fLigne * HEIGHT), -WIDTH / 2, -(HEIGHT / 2 - 2 * fLigne * HEIGHT),
			-WIDTH / 2 - fLigne * HEIGHT, -(HEIGHT / 2 - 2 * fLigne * HEIGHT),
			fDistance[i], 0);
	}


	// Deux rectangles sur les bords gauche et droits pour cacher les voitures qui sont hors du terrain 
	// lorsque l'utilisateur redimensionne la fenêtre

	// Gauche
	vQuadrilatere(0, 0, 0, -WIDTH - fEcart, -HEIGHT / 2, -WIDTH / 2, -HEIGHT / 2, -WIDTH / 2, HEIGHT / 2, -WIDTH - fEcart, HEIGHT / 2, 0, 0);

	// Droite
	vQuadrilatere(0, 0, 0, WIDTH / 2, -HEIGHT / 2, WIDTH + fEcart, -HEIGHT / 2, WIDTH + fEcart, HEIGHT / 2, WIDTH / 2, HEIGHT / 2, 0, 0);


	glutSwapBuffers();	//Permutation des 2 buffers
}

void vIdle()

{
	int i; // Compteur
	for (i = 0; i < NBVOITURE; i++)
	{
		fDistance[i] += 3;
	}
	glutPostRedisplay(); // force le réaffichage de la scène
}



void vReshape(int w, int h)
{
	float L;
	float H;

	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION); // Choisit la matrice de projection 
	glLoadIdentity();
	if (w <= h)
	{
		if (w == 0) H = HEIGHT;
		else H = (GLfloat)(WIDTH * h / w);
		L = WIDTH;
	}
	else
	{
		H = HEIGHT;
		if (h == 0) L = WIDTH;
		else L = (GLfloat)(HEIGHT * w / h);
	}
	gluOrtho2D(-L / 2, L / 2, -H / 2, H / 2);
}


void vQuadrilatere(float fRouge, float fVert, float fBleu, float fAx, float fAy,
	float fBx, float fBy, float fCx, float fCy, float fDx, float fDy,
	float fHori, float fVerti)
{
	glBegin(GL_QUADS); //commence le dessin d'un quadrilatère
	glColor3d(fRouge, fVert, fBleu); //  couleur du quadrilatère
	glVertex2d(fAx + fHori, fAy + fVerti); //coordonnées du premier sommet
	glVertex2d(fBx + fHori, fBy + fVerti);
	glVertex2d(fCx + fHori, fCy + fVerti);
	glVertex2d(fDx + fHori, fDy + fVerti); // Ordre des sommets : sens trigonométrique ou sens horaire (ne pas mélanger les sommets!!!)
	glEnd();
}
*/