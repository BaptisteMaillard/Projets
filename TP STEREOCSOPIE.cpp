

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <io.h>
#include <GL/glut.h>
#include "anaglyph.h"

int CHOIX = 1;

/* Misc globals */
int debug = FALSE;
int fullscreen = FALSE;
int currentbutton = -1;
double dtheta = 1.0;         /* Camera rotation angle increment */
CAMERA camera;
XYZ origin = { 0.0,0.0,0.0 };
double alpha = 0,betha=0,pasHorizontal= 0.01,pasVertical= 0.01;

/* Image saving options */
int movierecord = FALSE;

/* Model types */
#define POINT              1
#define CUBE               2
#define LIGNE_HORIZONTALE  3
#define LIGNE_VERTICALE    4
int modeltype = CUBE;


/* Image(s) à afficher     */
#define BOTH               1
#define GAUCHE             2
#define DROITE             3
int image = BOTH;


/*
   This is the basic display callback routine
   It creates the geometry, lighting, and viewing position
*/
void HandleDisplay(void)
{
    XYZ r;
    double ratio, radians, wd2, ndfl;
    double left, right, top, bottom, near = 0.1, far = 10000;

    /* Clip to avoid extreme stereo */
    near = camera.focallength / 5;

    /* Derive the the "right" vector */
    CROSSPROD(camera.vd, camera.vu, r);
    Normalise(&r);
    r.x *= camera.eyesep / 2.0;
    r.y *= camera.eyesep / 2.0;
    r.z *= camera.eyesep / 2.0;

    /* Misc stuff */
    ratio = camera.screenwidth / (double)camera.screenheight;
    radians = DTOR * camera.aperture / 2;
    wd2 = near * tan(radians);
    ndfl = near / camera.focallength;

    /* Set the buffer for writing and reading */
    glDrawBuffer(GL_BACK);
    glReadBuffer(GL_BACK);

    /* Clear things */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClear(GL_ACCUM_BUFFER_BIT); /* Not strictly necessary */

    glViewport(0, 0, camera.screenwidth, camera.screenheight);


    switch (image) {
    case BOTH:
        /* Left eye filter */
        glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);

        /* Create the projection */
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        left = -ratio * wd2 + 0.5 * camera.eyesep * ndfl;
        right = ratio * wd2 + 0.5 * camera.eyesep * ndfl;
        top = wd2;
        bottom = -wd2;
        glFrustum(left, right, bottom, top, near, far);

        /* Create the model */
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(camera.vp.x - r.x, camera.vp.y - r.y - alpha, camera.vp.z -  r.z - betha,
            camera.vp.x - r.x + camera.vd.x,
            camera.vp.y - r.y - alpha + camera.vd.y,
            camera.vp.z -  r.z - betha + camera.vd.z,
            camera.vu.x, camera.vu.y, camera.vu.z);
        CreateWorld();
        glFlush();
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

        /* Write over the accumulation buffer */
        glAccum(GL_LOAD, 1.0); /* Could also use glAccum(GL_ACCUM,1.0); */

        glDrawBuffer(GL_BACK);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /*The projection*/
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        left = -ratio * wd2 - 0.5 * camera.eyesep * ndfl;
        right = ratio * wd2 - 0.5 * camera.eyesep * ndfl;
        top = wd2;
        bottom = -wd2;
        glFrustum(left, right, bottom, top, near, far);

        /* Right eye filter*/
        glColorMask(GL_FALSE, GL_FALSE, GL_TRUE, GL_TRUE);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(camera.vp.x + r.x, camera.vp.y + r.y + alpha, camera.vp.z +  r.z + betha,
            camera.vp.x + r.x + camera.vd.x,
            camera.vp.y + r.y + alpha + camera.vd.y,
            camera.vp.z +  r.z + betha + camera.vd.z,
            camera.vu.x, camera.vu.y, camera.vu.z);
        CreateWorld();
        glFlush();
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

        /* Addin the new image and copy the result back */
        glAccum(GL_ACCUM, 1.0);
        glAccum(GL_RETURN, 1.0);


        /* Let's look at it */
        glutSwapBuffers();




        break;
    case DROITE:
        /* Left eye filter */
        glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);

        /* Create the projection */
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        left = -ratio * wd2 + 0.5 * camera.eyesep * ndfl;
        right = ratio * wd2 + 0.5 * camera.eyesep * ndfl;
        top = wd2;
        bottom = -wd2;
        glFrustum(left, right, bottom, top, near, far);

        /* Create the model */
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(camera.vp.x - r.x, camera.vp.y - r.y, camera.vp.z -  r.z,
            camera.vp.x - r.x + camera.vd.x,
            camera.vp.y - r.y + camera.vd.y,
            camera.vp.z -  r.z + camera.vd.z,
            camera.vu.x, camera.vu.y, camera.vu.z);
        CreateWorld();
        glFlush();
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

        /* Write over the accumulation buffer */
        glAccum(GL_LOAD, 1.0); /* Could also use glAccum(GL_ACCUM,1.0); */

        glDrawBuffer(GL_BACK);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* Addin the new image and copy the result back */
        glAccum(GL_ACCUM, 1.0);
        glAccum(GL_RETURN, 1.0);


        /* Let's look at it */
        glutSwapBuffers();


        break;

    case GAUCHE:
        /* Write over the accumulation buffer */
        glAccum(GL_LOAD, 1.0); /* Could also use glAccum(GL_ACCUM,1.0); */

        glDrawBuffer(GL_BACK);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* The projection */
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        left = -ratio * wd2 - 0.5 * camera.eyesep * ndfl;
        right = ratio * wd2 - 0.5 * camera.eyesep * ndfl;
        top = wd2;
        bottom = -wd2;
        glFrustum(left, right, bottom, top, near, far);

        /* Right eye filter */
        glColorMask(GL_FALSE, GL_FALSE, GL_TRUE, GL_TRUE);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(camera.vp.x + r.x, camera.vp.y + r.y, camera.vp.z +  r.z,
            camera.vp.x + r.x + camera.vd.x,
            camera.vp.y + r.y + camera.vd.y,
            camera.vp.z + r.z + camera.vd.z,
            camera.vu.x, camera.vu.y, camera.vu.z);
        CreateWorld();
        glFlush();
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

        /* Addin the new image and copy the result back */
        glAccum(GL_ACCUM, 1.0);
        glAccum(GL_RETURN, 1.0);


        /* Let's look at it */
        glutSwapBuffers();

        break;
    }


}


/*
   Create the geometry for the point
*/
void MakePoint(void)
{
    glColor3f(1.0, 1.0, 1.0);
    glutSolidSphere(0.9, 16, 8);
}

/*
   Create the geometry for a cube
*/
void MakeCube(void)
{
    typedef struct {
        float x;
        float y;
        float z;
        float r;
        float g;
        float b;
        //float a;
    } vertex;
    //jkujkujk

    // Création des sommets du cube
    vertex cube[8] = {
        {-3.0f,-3.0f, 3.0f,1.0f,1.0f,1.0f},
        {-3.0f, 3.0f, 3.0f,1.0f,1.0f,1.0f},
        { 3.0f, 3.0f, 3.0f,1.0f,1.0f,1.0f},
        { 3.0f,-3.0f, 3.0f,1.0f,1.0f,1.0f},
        {-3.0f,-3.0f,-3.0f,1.0f,1.0f,1.0f},
        {-3.0f, 3.0f,-3.0f,1.0f,1.0f,1.0f},
        { 3.0f, 3.0f,-3.0f,1.0f,1.0f,1.0f},
        { 3.0f,-3.0f,-3.0f,1.0f,1.0f,1.0f}
    };

    // Création des faces du cube
    int face[6][4] = {
        {0,1,2,3},
        {3,2,6,7},
        {4,5,6,7},
        {0,1,5,4},
        {1,5,6,2},
        {0,4,7,3}
    };
    // Dessin d'un cube colore
    // face par face
    for (int i = 0; i < 6; i++) {
        glBegin(GL_POLYGON);
        for (int j = 0; j < 4; j++) {
            glColor3f(cube[face[i][j]].r, cube[face[i][j]].g, cube[face[i][j]].b);
            glVertex3f(cube[face[i][j]].x, cube[face[i][j]].y, cube[face[i][j]].z);
        }
        glEnd();
    }
}

/*
   Create the geometry for the point
*/
void MakeLigneHorizontale(void)
{
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_LINES);
    glVertex3f(0, 0, 3);
    glVertex3f(0, 0, -3);
    glEnd();
}

/*
   Create the geometry for the point
*/
void MakeLigneVerticale(void)
{
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_LINES);
    glVertex3f(0, 3, 0);
    glVertex3f(0, -3, 0);
    glEnd();
}


/*
   Create one of the possible models
   Handle the rotation of the model, about the y axis
*/
void CreateWorld(void)
{

    glPushMatrix();
    switch (modeltype) {
    case POINT:              MakePoint();              break;
    case CUBE:               MakeCube();              break;
    case LIGNE_HORIZONTALE:  MakeLigneHorizontale();   break;
    case LIGNE_VERTICALE:    MakeLigneVerticale();     break;
    }
    glPopMatrix();

}




/*
   Deal with plain key strokes
*/
void HandleKeyboard(unsigned char key, int x, int y)
{
    switch (key) {
    case ESC:                            /* Quit */
    case 'Q':
    case 'q':
        exit(0);
        break;
    case 'h':                           /* Go home     */
    case 'H':
        CameraHome(0);
        break;
    case '[':                           /* Roll anti clockwise */
        RotateCamera(0, 0, -1);
        break;
    case ']':                           /* Roll clockwise */
        RotateCamera(0, 0, 1);
        break;
    case 'i':                           /* Translate camera up */
    case 'I':
        TranslateCamera(0, 1);
        break;
    case 'k':                           /* Translate camera down */
    case 'K':
        TranslateCamera(0, -1);
        break;
    case 'j':                           /* Translate camera left */
    case 'J':
        TranslateCamera(-1, 0);
        break;
    case 'l':                           /* Translate camera right */
    case 'L':
        TranslateCamera(1, 0);
        break;
    case 'p':
    case 'P':
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        break;
    case 'f':
    case 'F':
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        break;
    case '6':
        betha += pasHorizontal;
        break;
    case '4':
        betha -= pasHorizontal;
        break;
    case '8':
        alpha += pasVertical;
        break;
    case '2':
        alpha -= pasVertical;
        break;
    case '+':
        pasHorizontal += 0.01 ;
        break;
    case '-':
        pasHorizontal -= 0.01;
        break;
    case '9':
        pasVertical += 0.01;
        break;
    case '7':
        pasVertical -= 0.01;
        break;
    }
}

/*
   Deal with special key strokes
*/
void HandleSpecialKeyboard(int key, int x, int y)
{
    switch (key) {
    case GLUT_KEY_LEFT:
        RotateCamera(-1, 0, 0);
        break;
    case GLUT_KEY_RIGHT:
        RotateCamera(1, 0, 0);
        break;
    case GLUT_KEY_UP:
        RotateCamera(0, 1, 0);
        break;
    case GLUT_KEY_DOWN:
        RotateCamera(0, -1, 0);
        break;
    }
}

/*
   Rotate (ix,iy) or roll (iz) the camera about the focal point
   ix,iy,iz are flags, 0 do nothing, +- 1 rotates in opposite directions
   Correctly updating all camera attributes
*/
void RotateCamera(int ix, int iy, int iz)
{
    XYZ vp, vu, vd;
    XYZ right;
    XYZ newvp, newr;
    double radius, dd, radians;
    double dx, dy, dz;

    vu = camera.vu;
    Normalise(&vu);
    vp = camera.vp;
    vd = camera.vd;
    Normalise(&vd);
    CROSSPROD(vd, vu, right);
    Normalise(&right);
    radians = dtheta * PI / 180.0;

    /* Handle the roll */
    if (iz != 0) {
        camera.vu.x += iz * right.x * radians;
        camera.vu.y += iz * right.y * radians;
        camera.vu.z += iz * right.z * radians;
        Normalise(&camera.vu);
        return;
    }

    /* Distance from the rotate point */
    dx = camera.vp.x - camera.pr.x;
    dy = camera.vp.y - camera.pr.y;
    dz = camera.vp.z - camera.pr.z;
    radius = sqrt(dx * dx + dy * dy + dz * dz);

    /* Determine the new view point */
    dd = radius * radians;
    newvp.x = vp.x + dd * ix * right.x + dd * iy * vu.x - camera.pr.x;
    newvp.y = vp.y + dd * ix * right.y + dd * iy * vu.y - camera.pr.y;
    newvp.z = vp.z + dd * ix * right.z + dd * iy * vu.z - camera.pr.z;
    Normalise(&newvp);
    camera.vp.x = camera.pr.x + radius * newvp.x;
    camera.vp.y = camera.pr.y + radius * newvp.y;
    camera.vp.z = camera.pr.z + radius * newvp.z;

    /* Determine the new right vector */
    newr.x = camera.vp.x + right.x - camera.pr.x;
    newr.y = camera.vp.y + right.y - camera.pr.y;
    newr.z = camera.vp.z + right.z - camera.pr.z;
    Normalise(&newr);
    newr.x = camera.pr.x + radius * newr.x - camera.vp.x;
    newr.y = camera.pr.y + radius * newr.y - camera.vp.y;
    newr.z = camera.pr.z + radius * newr.z - camera.vp.z;

    camera.vd.x = camera.pr.x - camera.vp.x;
    camera.vd.y = camera.pr.y - camera.vp.y;
    camera.vd.z = camera.pr.z - camera.vp.z;
    Normalise(&camera.vd);

    /* Determine the new up vector */
    CROSSPROD(newr, camera.vd, camera.vu);
    Normalise(&camera.vu);
}

/*
   Translate (pan) the camera view point
   In response to i,j,k,l keys
   Also move the camera rotate location in parallel
*/
void TranslateCamera(int ix, int iy)
{
    XYZ vp, vu, vd;
    XYZ right;
    double radians, delta;

    vu = camera.vu;
    Normalise(&vu);
    vp = camera.vp;
    vd = camera.vd;
    Normalise(&vd);
    CROSSPROD(vd, vu, right);
    Normalise(&right);
    radians = dtheta * PI / 180.0;
    delta = dtheta * camera.focallength / 90.0;

    camera.vp.x += iy * vu.x * delta;
    camera.vp.y += iy * vu.y * delta;
    camera.vp.z += iy * vu.z * delta;
    camera.pr.x += iy * vu.x * delta;
    camera.pr.y += iy * vu.y * delta;
    camera.pr.z += iy * vu.z * delta;

    camera.vp.x += ix * right.x * delta;
    camera.vp.y += ix * right.y * delta;
    camera.vp.z += ix * right.z * delta;
    camera.pr.x += ix * right.x * delta;
    camera.pr.y += ix * right.y * delta;
    camera.pr.z += ix * right.z * delta;
}

/*
   Handle mouse events
   Right button events are passed to menu handlers
*/
void HandleMouse(int button, int state, int x, int y)
{
    if (state == GLUT_DOWN) {
        if (button == GLUT_LEFT_BUTTON) {
            currentbutton = GLUT_LEFT_BUTTON;
        }
        else if (button == GLUT_MIDDLE_BUTTON) {
            currentbutton = GLUT_MIDDLE_BUTTON;
        }
    }
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
   Handle the model menu
*/
void HandleModelMenu(int whichone)
{
    modeltype = whichone;
}

void HandleImageMenu(int wichone)
{
    image = wichone;
}


/*
   How to handle visibility
*/
void HandleVisibility(int visible)
{
    if (visible == GLUT_VISIBLE)
        HandleTimer(0);
    else
        ;
}

/*
   What to do on an timer event
*/
void HandleTimer(int value)
{
    glutPostRedisplay();
    glutTimerFunc(30, HandleTimer, 0);
}

/*
   Handle a window reshape/resize
   Keep it a power of 2 for the textures
   Keep it square
*/
void HandleReshape(int w, int h)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    camera.screenwidth = w;
    camera.screenheight = h;
}



/*
   Move the camera to the home position
   Or to a predefined stereo configuration
   The model is assumed to be in a 10x10x10 cube
   Centered at the origin
*/
void CameraHome(int mode)
{
    camera.aperture = 60;
    camera.pr = origin;

    camera.vd.x = 1;
    camera.vd.y = 0;
    camera.vd.z = 0;

    camera.vu.x = 0;
    camera.vu.y = 1;
    camera.vu.z = 0;

    camera.vp.x = -10;
    camera.vp.y = 0;
    camera.vp.z = 0;

    switch (mode) {
    case 0:
    case 2:
    case 4:
        camera.focallength = 10;
        break;
    case 1:
        camera.focallength = 5;
        break;
    case 3:
        camera.focallength = 15;
        break;
    }

    /* Non stressful stereo setting */
    camera.eyesep = camera.focallength / 30.0;
    if (mode == 4)
        camera.eyesep = 0;
}

/*
   Handle mouse motion
*/
void HandleMouseMotion(int x, int y)
{
    static int xlast = -1, ylast = -1;
    int dx, dy;

    dx = x - xlast;
    dy = y - ylast;
    if (dx < 0)      dx = -1;
    else if (dx > 0) dx = 1;
    if (dy < 0)      dy = -1;
    else if (dy > 0) dy = 1;

    if (currentbutton == GLUT_LEFT_BUTTON)
        RotateCamera(-dx, dy, 0);
    else if (currentbutton == GLUT_MIDDLE_BUTTON)
        RotateCamera(0, 0, dx);

    xlast = x;
    ylast = y;
}

/*
   Normalise a vector
*/
void Normalise(XYZ* p)
{
    double length;

    length = sqrt(p->x * p->x + p->y * p->y + p->z * p->z);
    if (length != 0) {
        p->x /= length;
        p->y /= length;
        p->z /= length;
    }
    else {
        p->x = 0;
        p->y = 0;
        p->z = 0;
    }
}




int main(int argc, char** argv)
{
    int mainmenu, modelmenu, cameramenu, imagemenu;


    /* Set things (glut) up */
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_ACCUM | GLUT_RGB | GLUT_DEPTH);

    /* Create the window and handlers */
    glutCreateWindow("Anaglyph simulator");
    camera.screenwidth = 400;
    camera.screenheight = 300;
    glutReshapeWindow(camera.screenwidth, camera.screenheight);
    glutDisplayFunc(HandleDisplay);
    glutReshapeFunc(HandleReshape);
    glutVisibilityFunc(HandleVisibility);
    glutKeyboardFunc(HandleKeyboard);
    glutSpecialFunc(HandleSpecialKeyboard);
    glutMouseFunc(HandleMouse);
    glutMotionFunc(HandleMouseMotion);
    CameraHome(0);

    

    /* Set up the model menu */
    modelmenu = glutCreateMenu(HandleModelMenu);
    glutAddMenuEntry("Point", POINT);
    glutAddMenuEntry("Cube", CUBE);
    glutAddMenuEntry("Ligne horizontale", LIGNE_HORIZONTALE);
    glutAddMenuEntry("Ligne verticale", LIGNE_VERTICALE);



    /* Set up the image menu */
    imagemenu = glutCreateMenu(HandleImageMenu);
    glutAddMenuEntry("Les 2", BOTH);
    glutAddMenuEntry("Gauche", GAUCHE);
    glutAddMenuEntry("Droite", DROITE);


    /* Set up the main menu */
    mainmenu = glutCreateMenu(HandleMainMenu);
    glutAddSubMenu("Model type", modelmenu);
    glutAddSubMenu("Images", imagemenu);
    glutAddMenuEntry("Quit", 9);
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    /* Ready to go! */
    glutMainLoop();
    return(0);
}