

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
double alpha = 0, betha = 0, pasHorizontal = 0.01, pasVertical = 0.01;

/* Image saving options */
int movierecord = FALSE;

/* Model types */
#define POINT              1
#define CUBE               2
#define LIGNE_HORIZONTALE  3
#define LIGNE_VERTICALE    4
#define MESH               5
#define SPHERE             6
#define PULSAR             7
#define KNOT               8
#define LORENZ             9
#define TORUS              10
#define TEAPOT	           11
int modeltype = CUBE;


/* Image(s) à afficher     */
#define BOTH               1
#define GAUCHE             2
#define DROITE             3
int image = BOTH;


#define CUBSIZE 3
#define OBJSIZE 7.0f

int fillMode = GL_LINE;
float pointSize = 1.0f;


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
        gluLookAt(camera.vp.x - r.x, camera.vp.y - r.y - alpha, camera.vp.z - r.z - betha,
            camera.vp.x - r.x + camera.vd.x,
            camera.vp.y - r.y - alpha + camera.vd.y,
            camera.vp.z - r.z - betha + camera.vd.z,
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
        gluLookAt(camera.vp.x + r.x, camera.vp.y + r.y + alpha, camera.vp.z + r.z + betha,
            camera.vp.x + r.x + camera.vd.x,
            camera.vp.y + r.y + alpha + camera.vd.y,
            camera.vp.z + r.z + betha + camera.vd.z,
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
        gluLookAt(camera.vp.x - r.x, camera.vp.y - r.y, camera.vp.z - r.z,
            camera.vp.x - r.x + camera.vd.x,
            camera.vp.y - r.y + camera.vd.y,
            camera.vp.z - r.z + camera.vd.z,
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
        gluLookAt(camera.vp.x + r.x, camera.vp.y + r.y, camera.vp.z + r.z,
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
   Create the geometry for a sphere
*/
void MakeSphere(int mode)
{
    int i, j, n = 32;
    double t1, t2, t3, r = 4;
    XYZ e, p, c = { 0,0,0 };

    glPolygonMode(GL_FRONT_AND_BACK, mode);

    // glColor3f(1.0,1.0,1.0);
    for (j = 0; j < n / 2; j++) {
        t1 = -PID2 + j * PI / (n / 2);
        t2 = -PID2 + (j + 1) * PI / (n / 2);

        glBegin(GL_QUAD_STRIP);
        for (i = 0; i <= n; i++) {
            t3 = i * TWOPI / n;

            e.x = cos(t1) * cos(t3);
            e.y = sin(t1);
            e.z = cos(t1) * sin(t3);
            p.x = c.x + r * e.x;
            p.y = c.y + r * e.y;
            p.z = c.z + r * e.z;
            glNormal3f(e.x, e.y, e.z);
            glTexCoord2f(i / (double)n, 2 * j / (double)n);
            glVertex3f(p.x, p.y, p.z);

            e.x = cos(t2) * cos(t3);
            e.y = sin(t2);
            e.z = cos(t2) * sin(t3);
            p.x = c.x + r * e.x;
            p.y = c.y + r * e.y;
            p.z = c.z + r * e.z;

            glNormal3f(e.x, e.y, e.z);
            glTexCoord2f(i / (double)n, 2 * (j + 1) / (double)n);
            glVertex3f(p.x, p.y, p.z);

        }
        glEnd();
    }
}

/*
   Create the geometry for the knot
*/
void MakeKnot(int mode)
{
    int i, n = 200;
    double x, y, z;
    double mu;

    glLineWidth(2.0);
    glColor3f(1.0, 1.0, 1.0);

    glBegin(GL_LINE_STRIP);
    for (i = 0; i <= n; i++) {
        mu = i * TWOPI / (double)n;
        x = 10 * (cos(mu) + cos(3 * mu)) + cos(2 * mu) + cos(4 * mu);
        y = 6 * sin(mu) + 10 * sin(3 * mu);
        z = 4 * sin(3 * mu) * sin(5 * mu / 2) + 4 * sin(4 * mu) - 2 * sin(6 * mu);
        glVertex3f(x / 4, y / 4, z / 4);
    }
    glEnd();

    glLineWidth(1.0);
}

/*
   Create the geometry for the lorenz attractor
*/
void MakeLorenz(int mode)
{
    int i, n = 10000;
    double x0 = 0.1, y0 = 0, z0 = 0, x1, y1, z1;
    double h = 0.005;
    double a = 10.0;
    double b = 28.0;
    double c = 8.0 / 3.0;


    glPolygonMode(GL_FRONT_AND_BACK, mode);

    glPointSize(pointSize);

    // glColor3f(1.0,1.0,1.0);
    glBegin(GL_POINTS);
    for (i = 0; i <= n; i++) {
        x1 = x0 + h * a * (y0 - x0);
        y1 = y0 + h * (x0 * (b - z0) - y0);
        z1 = z0 + h * (x0 * y0 - c * z0);
        x0 = x1;
        y0 = y1;
        z0 = z1;
        if (i > 100)
        {
            glColor3f((float)(4 * i) / (float)(n), (float)(4 * i) / (float)(n), (float)(4 * i) / (float)(n));
            glVertex3f((x0 - 0.95) / 5, (y0 - 1.78) / 5, (z0 - 26.7) / 5);
        }
    }
    glEnd();
}

/*
   Create the geometry for the mesh
*/
void MakeMesh0(void)
{
    int i, j, n = 1, w = 4;

    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_LINES);
    for (i = -w; i <= w; i += n) {
        for (j = -w; j < w; j += n) {
            glVertex3f(0.0, (double)i, (double)j);
            glVertex3f(0.0, (double)i, (double)j + n);
        }
    }
    for (j = -w; j <= w; j += n) {
        for (i = -w; i < w; i += n) {
            glVertex3f(0.0, (double)i, (double)j);
            glVertex3f(0.0, (double)i + n, (double)j);
        }
    }
    glEnd();
}

void MakeMesh(int mode)
{
    int i, j, step = 1, w = 7;


    glPolygonMode(GL_FRONT_AND_BACK, mode);

    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_QUADS);
    for (i = -w; i <= w; i += step)
    {
        for (j = -w; j < w; j += step)
        {
            glVertex3f(0.0f, (double)i, (double)j);
            glVertex3f(0.0f, (double)i + step, (double)j);
            glVertex3f(0.0f, (double)i + step, (double)j + step);
            glVertex3f(0.0f, (double)i, (double)j + step);
        }
    }
    glEnd();
}




/*
   Create the geometry for the pulsar
*/
void MakePulsar(int mode)
{
    int i, j, k;
    double cradius = 1;            /* Final radius of the cone */
    double clength = 5;            /* Cone length */
    double sradius = 1.3;         /* Final radius of sphere */
    double r1 = 1.4, r2 = 1.8;         /* Min and Max radius of field lines */
    double x, y, z;
    XYZ p[4], n[4];
    static double rotateangle = 0.0;

    glPolygonMode(GL_FRONT_AND_BACK, mode);

    /* Top level rotation  - spin */
    glPushMatrix();
    glRotatef(rotateangle, 0.0, 1.0, 0.0);

    /* Rotation about spin axis */
    glPushMatrix();
    glRotatef(45.0, 0.0, 0.0, 1.0);

    /* Light in center */
    glutSolidSphere(0.9, 16, 8);

    /* Spherical center */
    for (i = 0; i < 360; i += 5) {
        for (j = -80; j < 80; j += 5) {

            p[0].x = sradius * cos(j * DTOR) * cos(i * DTOR);
            p[0].y = sradius * sin(j * DTOR);
            p[0].z = sradius * cos(j * DTOR) * sin(i * DTOR);
            n[0] = p[0];

            p[1].x = sradius * cos((j + 5) * DTOR) * cos(i * DTOR);
            p[1].y = sradius * sin((j + 5) * DTOR);
            p[1].z = sradius * cos((j + 5) * DTOR) * sin(i * DTOR);
            n[1] = p[1];

            p[2].x = sradius * cos((j + 5) * DTOR) * cos((i + 5) * DTOR);
            p[2].y = sradius * sin((j + 5) * DTOR);
            p[2].z = sradius * cos((j + 5) * DTOR) * sin((i + 5) * DTOR);
            n[2] = p[2];

            p[3].x = sradius * cos(j * DTOR) * cos((i + 5) * DTOR);
            p[3].y = sradius * sin(j * DTOR);
            p[3].z = sradius * cos(j * DTOR) * sin((i + 5) * DTOR);
            n[3] = p[3];

            glBegin(GL_POLYGON);
            if (i % 20 == 0)
                glColor3f(1.0, 1.0, 1.0);
            else
                glColor3f(0.5, 0.5, 0.5);
            for (k = 0; k < 4; k++) {
                glNormal3f(n[k].x, n[k].y, n[k].z);
                glVertex3f(p[k].x, p[k].y, p[k].z);
            }
            glEnd();
        }
    }

    /* Draw the cones */
    for (j = -1; j <= 1; j += 2) {
        for (i = 0; i < 360; i += 10) {

            p[0] = origin;
            n[0] = p[0];
            n[0].y = -1;

            p[1].x = cradius * cos(i * DTOR);
            p[1].y = j * clength;
            p[1].z = cradius * sin(i * DTOR);
            n[1] = p[1];
            n[1].y = 0;

            p[2].x = cradius * cos((i + 10) * DTOR);
            p[2].y = j * clength;
            p[2].z = cradius * sin((i + 10) * DTOR);
            n[2] = p[2];
            n[2].y = 0;

            glBegin(GL_POLYGON);
            if (i % 30 == 0)
                glColor3f(0.2, 0.2, 0.2);
            else
                glColor3f(0.5, 0.5, 0.5);
            for (k = 0; k < 3; k++) {
                glNormal3f(n[k].x, n[k].y, n[k].z);
                glVertex3f(p[k].x, p[k].y, p[k].z);
            }
            glEnd();
        }
    }

    /* Draw the field lines */
    for (j = 0; j < 360; j += 20) {
        glPushMatrix();
        glRotatef((double)j, 0.0, 1.0, 0.0);
        glBegin(GL_LINE_STRIP);
        glColor3f(0.7, 0.7, 0.7);
        for (i = -140; i < 140; i++) {
            x = r1 + r1 * cos(i * DTOR);
            y = r2 * sin(i * DTOR);
            z = 0;
            glVertex3f(x, y, z);
        }
        glEnd();
        glPopMatrix();
    }

    glPopMatrix(); /* Pulsar axis rotation */
    glPopMatrix(); /* Pulsar spin */
}


#define INNER_RADIUS	1
#define OUTER_RADIUS	3
#define NSIDES		30
#define RINGS		30


void MakeTorus(int mode)
{
    glPolygonMode(GL_FRONT_AND_BACK, mode);

    switch (mode)
    {
    case GL_LINE:

        glutWireTorus(INNER_RADIUS, OUTER_RADIUS, NSIDES, RINGS);
        break;

    case GL_FILL:

        glutSolidTorus(INNER_RADIUS, OUTER_RADIUS, NSIDES, RINGS);
        break;
    }
}

void MakeTeapot(int mode)
{
    glPolygonMode(GL_FRONT_AND_BACK, mode);

    switch (mode)
    {
    case GL_LINE:

        glutWireTeapot(CUBSIZE);
        break;

    case GL_FILL:

        glutSolidTeapot(CUBSIZE);
        break;
    }
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
    case CUBE:               MakeCube();               break;
    case LIGNE_HORIZONTALE:  MakeLigneHorizontale();   break;
    case LIGNE_VERTICALE:    MakeLigneVerticale();     break;
    case MESH:     MakeMesh(fillMode);     break;
    case SPHERE:   MakeSphere(fillMode);   break;
    case KNOT:     MakeKnot(fillMode);     break;
    case PULSAR:   MakePulsar(fillMode);   break;
    case LORENZ:   MakeLorenz(fillMode);   break;
    case TORUS:	  MakeTorus(fillMode);    break;
    case TEAPOT:  MakeTeapot(fillMode);   break;


        
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
        fillMode = GL_FILL;
        break;
    case 'f':
    case 'F':
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        fillMode = GL_LINE;
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
        pasHorizontal += 0.01;
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
    glutAddMenuEntry("Mesh", MESH);
    glutAddMenuEntry("Sphere", SPHERE);
    glutAddMenuEntry("Pulsar model", PULSAR);
    glutAddMenuEntry("Knot", KNOT);
    glutAddMenuEntry("Lorenz Attractor", LORENZ);
    glutAddMenuEntry("Torus", TORUS);
    glutAddMenuEntry("Teapot", TEAPOT);


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