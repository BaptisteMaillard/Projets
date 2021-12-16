//Améliorer l'interface
// //Afficher le temps, le nom de la chanson ect...
//Comment gérer le rythme de la visualisation : pk Hymne Européen va si vite
//Pour améliorer : mettre un autre mode de visualisation genre des barres verticales
//Marquer des pauses quand on change de musique
//Indiquer quelle chanson est jouée dans l'interface
//bouton strat, stop et restart
//Mettre un délai avant de lancer la visualisation
//Laissez le choix de la première musique
//Mettre un délai avant de lancer la visualisation
//Amélioration : aller chercher ou faire un drag and drope à partir des dossiers
//Changer les cylindre en disque

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

#define N 1024
int increment = 0;

/* Liste d'audio */
#define HYMNE                 1
#define MEGAMAN               2
#define TIME_LAPS             3
int audiotype = HYMNE;
int ancienAudiotype = 0;

/* Mode de visualisation */
#define MODE1                 1
#define MODE2                 2
#define MODE3                 3
int modetype = MODE1;

float timer = 0.;
float* ptr_timer = &timer;
int max_increment = 100000;
int* ptr_max_increment = &max_increment;


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



void vTimerIdle(int i)
{
    if (increment == 1) cout << "C'est parti !" << endl;
    if (increment < *ptr_max_increment) increment++;
    else {
        cout << "L'ecoute est finie" << endl;
        exit(0);
    }
    //cout << increment << endl;
    glutPostRedisplay();
    glutTimerFunc((int)*ptr_timer, vTimerIdle, i);
}

float valeurAbsolue(float f) {
    if (f < 0) return f;
    return f;
}

/*
   Handle the main menu
*/
void HandleMainMenu(int whichone)
{
    switch (whichone) {
    case 9:
        exit(0);
        break;
    }
}

/*
   Handle the audio menu
*/
void HandleAudioMenu(int whichone)
{
    audiotype = whichone;
}

/*
   Handle the mode menu
*/
void HandleModeMenu(int whichone)
{
    modetype = whichone;
}


// Déclarations des fonctions de rappel (callbacks)
GLvoid affichage();
GLvoid clavier(unsigned char touche, int x, int y);

int main(int argc, char* argv[])
{
    int mainmenu, modemenu, audiomenu;


    // Initialisation de GLUT
    glutInit(&argc, argv);
    // Choix du mode d'affichage (ici RVB)
    glutInitDisplayMode(GLUT_RGB);
    // Position initiale de la fenetre GLUT
    glutInitWindowPosition(50, 50);
    // Taille initiale de la fenetre GLUT
    glutInitWindowSize(400, 400);
    // Creation de la fenetre GLUT
    glutCreateWindow("Audio visualisation");

    // Définition de la couleur d'effacement du framebuffer
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);


    // Définition des fonctions de callbacks
    glutDisplayFunc(affichage);
    glutKeyboardFunc(clavier);


    glutTimerFunc((int)*ptr_timer, vTimerIdle, 1);



    // Set up the audio menu
    audiomenu = glutCreateMenu(HandleAudioMenu);
    glutAddMenuEntry("Hymne europeen", HYMNE);
    glutAddMenuEntry("Megaman", MEGAMAN);
    glutAddMenuEntry("Time Laps", TIME_LAPS);

    // Set up the mode menu 
    modemenu = glutCreateMenu(HandleModeMenu);
    glutAddMenuEntry("Mode 1", MODE1);
    glutAddMenuEntry("Mode 2", MODE2);

    // Set up the main menu
    mainmenu = glutCreateMenu(HandleMainMenu);
    glutAddSubMenu("Mode", modemenu);
    glutAddSubMenu("Audio", audiomenu);
    glutAddMenuEntry("Quit", 9);
    glutAttachMenu(GLUT_RIGHT_BUTTON);


    // Lancement de la boucle infinie GLUT
    glutMainLoop();

    return 0;
}

// Definition de la fonction d'affichage
GLvoid affichage() {
    // Effacement du frame buffer
    glClear(GL_COLOR_BUFFER_BIT);
    //glMatrixMode(GL_MODELVIEW);

    string entree;
    if (audiotype == HYMNE) {
        entree = "HymneEuropeen.wav";
    }
    else if (audiotype == MEGAMAN) {
        entree = "megaman_mono.wav";
    }
    else if (audiotype == TIME_LAPS) {
        entree = "Time Lapse.wav";
    }

    SndfileHandle audio = SndfileHandle(entree);
    if (audio.channels() != 1) {
        cout << "ERROR: Only taking mono files" << endl;
        exit(EXIT_FAILURE);
    }
    int padded_length = next_pow_2((int)audio.frames());
    //Creation d'une ARRAY FFTW3. Adding 2 to do in-place FFT
    float* input_buffer = fftwf_alloc_real((size_t)padded_length + 2);

    if (audiotype != ancienAudiotype) {
        bool played = 0;
        if (audiotype == HYMNE) played = PlaySound(L"HymneEuropeen.wav", NULL, SND_ASYNC);
        else if (audiotype == MEGAMAN) played = PlaySound(L"megaman_mono.wav", NULL, SND_ASYNC);
        else if (audiotype == TIME_LAPS) played = PlaySound(L"Time Lapse.wav", NULL, SND_ASYNC);
        if (played != 1) {
            cout << "ERROR: Le son n'est pas joué" << endl;
            exit(EXIT_FAILURE);
        }
        Sleep(20);
        int taille_audio = (int)audio.frames();
        *ptr_max_increment = int(-1 + taille_audio / N);
        float duree_audio = float(taille_audio / audio.samplerate());
        *ptr_timer = float(pow(10, 3) * duree_audio / *ptr_max_increment);

        cout << "------------------------------------------------------------" << endl;
        cout << "Donnees de l'audio :" << endl;
        cout << "------------------------------------------------------------" << endl;
        cout << "Taille de l'audio : " << taille_audio << endl;
        cout << "Frequence d'echantillonnage : " << audio.samplerate() << " Hz" << endl;
        cout << "Duree de l'audio : " << duree_audio << " secondes" << endl;
        increment = 0;
    }



    // On remplit l'ARRAY avec l'audio
    audio.readf(input_buffer, audio.frames());
    for (int i = (int)audio.frames(); i < padded_length; i++) {
        input_buffer[i] = 0.0f;
    }


    float* input_buf = fftwf_alloc_real((size_t)N);
    for (int i = 0; i < N; i++) {
        input_buf[i] = input_buffer[i + N * increment];
    }


    //Creation du FFT plan
    float* output_buf = fftwf_alloc_real((size_t)N);
    fftwf_plan in_plan = fftwf_plan_dft_r2c_1d(N, input_buf, (fftwf_complex*)output_buf,
        FFTW_ESTIMATE);

    //Execution
    fftwf_execute(in_plan);



    float moyenne[8] = { 0,0,0,0,0,0,0,0 };//contient les moyennes des amplitudes pour les 8 plages
    for (int i = 0; i < 8; i++) {
        for (int k = 0; k < N / 8; k++) {
            moyenne[i] += valeurAbsolue(output_buf[k + i * N / 8]);
        }
        moyenne[i] *= 8. / N;
    }



    float max[8][2];
    for (int k = 0; k < 8; k++) {
        max[k][0] = (float)k;
        max[k][1] = moyenne[k];
    }
    //max contient l'indice et la valeur rangée par ordre décroissant par rapport à la valeur de l'amplitude

    float temp[2];
    for (int i = 0; i < 7; i++) {
        for (int j = i + 1; j < 8; j++) {
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


    if (modetype == MODE1) {
        //Ici bleu (HF) au centre et rouge (BF) aux extrémités
        for (int k = 0; k < 8; k++) {
            switch ((int)max[k][0])
            {
            case 0:
                glColor3ub(255, 0, 0); // rouge
                gluCylinder(quadric, 0.8, 0.9, 2. - float(max[k][1] / max[0][1]), 50, 50);
                break;
            case 1:
                glColor3ub(255, 165, 0); // orange
                gluCylinder(quadric, 0.7, 0.8, 2. - float(max[k][1] / max[0][1]), 50, 50);
                break;
            case 2:
                glColor3ub(255, 255, 0); // jaune
                gluCylinder(quadric, 0.6, 0.7, 2. - float(max[k][1] / max[0][1]), 50, 50);
                break;
            case 3:
                glColor3ub(0, 255, 0); // vert
                gluCylinder(quadric, 0.5, 0.6, 2. - float(max[k][1] / max[0][1]), 50, 50);
                break;
            case 4:
                glColor3ub(23, 101, 125); // bleu-vert
                gluCylinder(quadric, 0.4, 0.5, 2. - float(max[k][1] / max[0][1]), 50, 50);
                break;
            case 5:
                glColor3ub(0, 0, 255); // bleu
                gluCylinder(quadric, 0.3, 0.4, 2. - float(max[k][1] / max[0][1]), 50, 50);
                break;
            case 6:
                glColor3ub(127, 0, 255); // violet
                gluCylinder(quadric, 0.2, 0.3, 2. - float(max[k][1] / max[0][1]), 50, 50);
                break;
            case 7:
                glColor3ub(139, 0, 139); // mauve
                gluCylinder(quadric, 0.1, 0.2, 2. - float(max[k][1] / max[0][1]), 50, 50);
                break;
            }
        }
    }
    else if (modetype == MODE2) {
        //Ici rouge (HF) au centre et bleu (BF) aux extrémités
        for (int k = 0; k < 8; k++) {
            switch ((int)max[k][0])
            {
            case 0:
                glColor3ub(255, 0, 0); // rouge
                gluCylinder(quadric, 0.1, 0.2, 2. - float(max[k][1] / max[0][1]), 50, 50);
                break;
            case 1:
                glColor3ub(255, 165, 0); // orange
                gluCylinder(quadric, 0.2, 0.3, 2. - float(max[k][1] / max[0][1]), 50, 50);
                break;
            case 2:
                glColor3ub(255, 255, 0); // jaune
                gluCylinder(quadric, 0.3, 0.4, 2. - float(max[k][1] / max[0][1]), 50, 50);
                break;
            case 3:
                glColor3ub(0, 255, 0); // vert
                gluCylinder(quadric, 0.4, 0.5, 2. - float(max[k][1] / max[0][1]), 50, 50);
                break;
            case 4:
                glColor3ub(23, 101, 125); // bleu-vert
                gluCylinder(quadric, 0.5, 0.6, 2. - float(max[k][1] / max[0][1]), 50, 50);
                break;
            case 5:
                glColor3ub(0, 0, 255); // bleu
                gluCylinder(quadric, 0.6, 0.7, 2. - float(max[k][1] / max[0][1]), 50, 50);
                break;
            case 6:
                glColor3ub(127, 0, 255); // violet
                gluCylinder(quadric, 0.7, 0.8, 2. - float(max[k][1] / max[0][1]), 50, 50);
                break;
            case 7:
                glColor3ub(139, 0, 139); // mauve
                gluCylinder(quadric, 0.8, 0.9, 2. - float(max[k][1] / max[0][1]), 50, 50);
                break;
            }
        }
    }
    ancienAudiotype = audiotype;
    glFlush();
}

// Definition de la fonction gérant les interruptions clavier
GLvoid clavier(unsigned char touche, int x, int y) {

    // Suivant les touches pressees, nous aurons un comportement different de l'application
    // ESCAPE ou 'q' : fermera l'application


    switch (touche) {

    case 'q': // quitter
    case 27:
        exit(0);
        break;
    }

    // Demande a GLUT de reafficher la scene
    glutPostRedisplay();
}
