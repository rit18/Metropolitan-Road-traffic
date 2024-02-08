#include<GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <windows.h>
#include<math.h>
#include<mmsystem.h>
#include<bits/stdc++.h>
#include "BmpLoader.h"
#include <fstream>
#include <iostream>
#define GL_CLAMP_TO_EDGE 0x812F
using namespace std;


const int window_width = 1200;
const int window_height = 700;
GLfloat eyeX=0;
GLfloat eyeY=40;
GLfloat eyeZ=50;

GLfloat lookX=0;
GLfloat lookY=40;
GLfloat lookZ=-200;

bool light_switch_0=false;
bool light_switch_1=false;
bool spot_light_switch=false;
bool main_light_switch=false;




float carx=-300, carz=0;
float busx=0, busz=-300;
bool bus_switch=false;
bool car_switch=true;

float rot = 0;

unsigned int ID;

const double PI = 3.14159265389;


/* GLUT callback Handlers */


int anglex= 0, angley = 0, anglez = 0;          //rotation angles
int window;
int wired=0;
int shcpt=1;
int animat = 0;
const int L=20;
const int dgre=3;
int ncpt=L+1;
int clikd=0;
const int nt = 40;				//number of slices along x-direction
const int ntheta = 20;


GLfloat ctrlpoints[L+1][3] =
{
{7.625,5.4,0},
{7.325,4.775,0},
{7.225,4.05,0},
{6.975,3.4,0},
{6.675,2.85,0},
{6.275,2.15,0},
{5.85,1.725,0},
{4.85,1.15,0},
{3.975,0.9,0},
{3.325,0.85,0},
{2.9,0.375,0},
{2.775,-0.175,0},
{2.725,-0.7,0},
{3.225,-0.925,0},
{3.875,-0.9,0},
{5.175,-1.375,0},
{5.825,-1.975,0},
{6.5,-2.725,0},
{6.925,-3.925,0},
{7.325,-5.05,0},
{7.6,0.175,0},
};




float wcsClkDn[3],wcsClkUp[3];
///////////////////////////////
class point1
{
public:
    point1()
    {
        x=0;
        y=0;
    }
    int x;
    int y;
} clkpt[2];
int curve_flag=0;
GLint viewport[4]; //var to hold the viewport info
GLdouble modelview[16]; //var to hold the modelview info
GLdouble projection[16]; //var to hold the projection matrix info

//////////////////////////
void scsToWcs(float sx,float sy, float wcsv[3] );
void processMouse(int button, int state, int x, int y);
void matColor(float kdr, float kdg, float kdb,  float shiny, int frnt_Back=0, float ambFactor=1.0, float specFactor=1.0);
///////////////////////////

void scsToWcs(float sx,float sy, float wcsv[3] )
{

    GLfloat winX, winY, winZ; //variables to hold screen x,y,z coordinates
    GLdouble worldX, worldY, worldZ; //variables to hold world x,y,z coordinates

    //glGetDoublev( GL_MODELVIEW_MATRIX, modelview ); //get the modelview info
    glGetDoublev( GL_PROJECTION_MATRIX, projection ); //get the projection matrix info
    glGetIntegerv( GL_VIEWPORT, viewport ); //get the viewport info

    winX = sx;
    winY = (float)viewport[3] - (float)sy;
    winZ = 0;

    //get the world coordinates from the screen coordinates
    gluUnProject( winX, winY, winZ, modelview, projection, viewport, &worldX, &worldY, &worldZ);
    wcsv[0]=worldX;
    wcsv[1]=worldY;
    wcsv[2]=worldZ;


}
void processMouse(int button, int state, int x, int y)
{
    if(button==GLUT_LEFT_BUTTON && state==GLUT_DOWN)
    {
        if(curve_flag!=1)
        {
            curve_flag=1;
            clkpt[0].x=x;
            clkpt[0].y=y;
        }


        scsToWcs(clkpt[0].x,clkpt[0].y,wcsClkDn);
        //cout<<"\nD: "<<x<<" "<<y<<" wcs: "<<wcsClkDn[0]<<" "<<wcsClkDn[1];
        cout<<"{"<<wcsClkDn[0]<<","<<wcsClkDn[1]<<",0},"<<endl;
    }
    else if(button==GLUT_LEFT_BUTTON && state==GLUT_UP)
    {
        if (curve_flag==1)
        {
            clkpt[1].x=x;
            clkpt[1].y=y;
            curve_flag=0;
        }
        float wcs[3];
        scsToWcs(clkpt[1].x,clkpt[1].y,wcsClkUp);
        //cout<<"\nU: "<<x<<" "<<y<<" wcs: "<<wcsClkUp[0]<<" "<<wcsClkUp[1];

        clikd=!clikd;
    }
}

//control points
long long nCr(int n, int r)
{
    if(r > n / 2) r = n - r; // because C(n, r) == C(n, n - r)
    long long ans = 1;
    int i;

    for(i = 1; i <= r; i++)
    {
        ans *= n - r + i;
        ans /= i;
    }

    return ans;
}

//polynomial interpretation for N points
void BezierCurve ( double t,  float xy[2])
{
    double y=0;
    double x=0;
    t=t>1.0?1.0:t;
    for(int i=0; i<=L; i++)
    {
        int ncr=nCr(L,i);
        double oneMinusTpow=pow(1-t,double(L-i));
        double tPow=pow(t,double(i));
        double coef=oneMinusTpow*tPow*ncr;
        x+=coef*ctrlpoints[i][0];
        y+=coef*ctrlpoints[i][1];

    }
    xy[0] = float(x);
    xy[1] = float(y);

    //return y;
}


static GLfloat v_cube[8][3] =
{
    {0,0,0},
    {0,0,1},
    {0,1,0},
    {0,1,1},

    {1,0,0},
    {1,0,1},
    {1,1,0},
    {1,1,1}
};

static GLubyte c_ind[6][4] =
{


    {3,1,5,7},
    {2,0,1,3},
    {7,5,4,6},
    {2,3,7,6},
    {1,0,5,4},
    {6,4,0,2},


};
static void getNormal3p(GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x2, GLfloat y2, GLfloat z2, GLfloat x3, GLfloat y3, GLfloat z3)
{
    GLfloat Ux, Uy, Uz, Vx, Vy, Vz, Nx, Ny, Nz;

    Ux = x2-x1;
    Uy = y2-y1;
    Uz = z2-z1;

    Vx = x3-x1;
    Vy = y3-y1;
    Vz = z3-z1;

    Nx = Uy*Vz - Uz*Vy;
    Ny = Uz*Vx - Ux*Vz;
    Nz = Ux*Vy - Uy*Vx;

    glNormal3f(Nx,Ny,Nz);
}

void material_property(float R, float G, float B)
{
    GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat mat_ambient[] = { R, G, B, 1.0 };
    GLfloat mat_diffuse[] = { R, G, B, 1.0 };
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess[] = {255};

    glMaterialfv( GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv( GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv( GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv( GL_FRONT, GL_SHININESS, mat_shininess);

}




void cube(float R=0.5, float G=0.5, float B=0.5 )
{
    material_property(R,G,B);
    glBegin(GL_QUADS);
    for (GLint i = 0; i <6; i++)
    {

        getNormal3p(v_cube[c_ind[i][0]][0], v_cube[c_ind[i][0]][1], v_cube[c_ind[i][0]][2],
                    v_cube[c_ind[i][1]][0], v_cube[c_ind[i][1]][1], v_cube[c_ind[i][1]][2],
                    v_cube[c_ind[i][2]][0], v_cube[c_ind[i][2]][1], v_cube[c_ind[i][2]][2]);


        glTexCoord2f(1,1);
        glVertex3fv(&v_cube[c_ind[i][0]][0]);
        glTexCoord2f(1,0);
        glVertex3fv(&v_cube[c_ind[i][1]][0]);
        glTexCoord2f(0,0);
        glVertex3fv(&v_cube[c_ind[i][2]][0]);
        glTexCoord2f(0,1);
        glVertex3fv(&v_cube[c_ind[i][3]][0]);

    }
    glEnd();
}



void LoadTexture(const char*filename, int rep = 1)
{
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);
    glPixelStorei(GL_UNPACK_ALIGNMENT, ID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if(rep)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    BmpLoader bl(filename);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, bl.iWidth, bl.iHeight, GL_RGB, GL_UNSIGNED_BYTE, bl.textureData );
}

void buiding()
{
    // building 1
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,1);

    glPushMatrix();
    glTranslatef(0,30,0);
    glScalef(20,60,20);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);


    //roof 1
    glPushMatrix();
    glTranslatef(0,60,0);
    glScalef(20,1,20);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();


    // building 2
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,2);

    glPushMatrix();
    glTranslatef(50,30,0);
    glScalef(20,60,20);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);

    //roof 2
    glPushMatrix();
    glTranslatef(50,60,0);
    glScalef(20,1,20);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();

    // building 3
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,3);

    glPushMatrix();
    glTranslatef(100,30,0);
    glScalef(20,60,20);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);

    //roof 3
    glPushMatrix();
    glTranslatef(100,60,0);
    glScalef(20,1,20);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();


    // building 4
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,4);

    glPushMatrix();
    glTranslatef(0,30,-30);
    glScalef(20,60,20);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    //roof 4
    glPushMatrix();
    glTranslatef(0,60,-30);
    glScalef(20,1,20);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();


    // building 5
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,5);

    glPushMatrix();
    glTranslatef(50,30,-30);
    glScalef(20,60,20);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);


    //roof 5
    glPushMatrix();
    glTranslatef(50,60,-30);
    glScalef(20,1,20);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();

    // building 6
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,6);

    glPushMatrix();
    glTranslatef(100,30,-30);
    glScalef(20,60,20);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    //roof 6
    glPushMatrix();
    glTranslatef(100,60,-30);
    glScalef(20,1,20);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();

    // building 7
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,5);

    glPushMatrix();
    glTranslatef(0,30,-60);
    glScalef(20,60,20);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    //roof 7
    glPushMatrix();
    glTranslatef(0,60,-60);
    glScalef(20,1,20);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();

    // building 8
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,2);

    glPushMatrix();
    glTranslatef(50,30,-60);
    glScalef(20,60,20);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    //roof 8
    glPushMatrix();
    glTranslatef(50,60,-60);
    glScalef(20,1,20);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();

    // building 9
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,4);

    glPushMatrix();
    glTranslatef(100,30,-60);
    glScalef(20,60,20);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    //roof 9
    glPushMatrix();
    glTranslatef(100,60,-60);
    glScalef(20,1,20);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();

     // bank
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,29);

    glPushMatrix();
    glTranslatef(-100,30,-60);
    glScalef(20,60,20);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    //bank
    glPushMatrix();
    glTranslatef(-100,60,-60);
    glScalef(20,1,20);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();

   // mini
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,30);

    glPushMatrix();
    glTranslatef(0,20,80);
    glScalef(30,60,20);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);


    //mini
    glPushMatrix();
    glTranslatef(0,20,80);
    glScalef(20,1,20);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();


}
void playground()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,7);
    glPushMatrix();
    glTranslatef(210,2,-15);
    glScalef(80,1,50);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();

    glPopMatrix();

    glDisable(GL_TEXTURE_2D);

}

void road()
{
    //main road
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,10);

    glPushMatrix();
    glTranslatef(0,0.5,20);
    glScalef(900,1,15);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);

    // left side road
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,10);

    glPushMatrix();
    glTranslatef(-50,0.8,0);
    glRotatef(90,0,1,0);
    glScalef(300,1,20);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);

    //right side road
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,10);

    glPushMatrix();
    glTranslatef(135,0.8,0);
    glRotatef(90,0,1,0);
    glScalef(300,1,20);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
}
void trees()
{
    // tree 1

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,12);

    glPushMatrix();
    glTranslatef(-20,30,0);
    glScalef(20,60,20);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);

    // tree 2

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,12);

    glPushMatrix();
    glTranslatef(-70,30,0);
    glScalef(20,60,20);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);



}
void wood_leaf()
{

    // wood
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,13);

    glPushMatrix();
    glTranslatef(30,30,0);
    glScalef(2,30,2);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    //leaf

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,14);

    glPushMatrix();
    glTranslatef(30,50,0);
    glScalef(8,12,8);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);


}

void trees_beside_building()
{

    for (int i=10; i<=100; i+=20)
    {
        glPushMatrix();
        glTranslatef(0,-20,-i);
        wood_leaf();
        glPopMatrix();

    }

    // Tree 2

    for (int i=10; i<=100; i+=20)
    {
        glPushMatrix();
        glTranslatef(45,-20,-i);
        wood_leaf();
        glPopMatrix();

    }
}
void traffic_signal()
{
    // base
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,19);

    glPushMatrix();
    glTranslatef(-20,30,0);
    glScalef(2,30,2);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    // Signal light

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,8);

    glPushMatrix();
    glTranslatef(-20,50,0);
    glScalef(5,10,5);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    // roof off
    glPushMatrix();
    glTranslatef(-20,55,0);
    glScalef(5,2,5);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();

}

void signboard()
{
    // base
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,19);

    glPushMatrix();
    glTranslatef(-50,30,0);
    glScalef(2,30,2);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    // Signal light

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,9);

    glPushMatrix();
    glTranslatef(-50,50,0);
    glScalef(20,20,20);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    // roof off
    glPushMatrix();
    glTranslatef(-50,55,0);
    glScalef(5,2,5);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();

}
void shop()
{
    // shop 1
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,11);

    glPushMatrix();
    glTranslatef(-130,30,0);
    glScalef(30,50,20);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);

    // roof off
    //roof
    glPushMatrix();
    glTranslatef(-130,50,0);
    glScalef(30,2,20);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();

    // shop 2
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,18);

    glPushMatrix();
    glTranslatef(-170,30,0);
    glScalef(30,50,20);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);

    // roof off
    //roof
    glPushMatrix();
    glTranslatef(-170,50,0);
    glScalef(30,2,20);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();


    // shop 3
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,26);

    glPushMatrix();
    glTranslatef(-130,30,-30);
    glScalef(30,50,20);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);

    // roof off 2

    glPushMatrix();
    glTranslatef(-130,50,-30);
    glScalef(30,2,20);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();


    // shop 4
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,27);

    glPushMatrix();
    glTranslatef(-170,30,-30);
    glScalef(30,50,20);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);

    // roof off 2

    glPushMatrix();
    glTranslatef(-170,50,-30);
    glScalef(30,2,20);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();

     // shop 5
     glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,28);

    glPushMatrix();
    glTranslatef(-230,22,-20);
    glScalef(20,40,20);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);

    // roof off 2

    glPushMatrix();
    glTranslatef(-230,22,-20);
    glScalef(20,2,20);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();
}

void road_light()
{
    // base
    glPushMatrix();

    glPushMatrix();
    glTranslatef(80,30,0);
    glScalef(1,30,1);
    glTranslatef(-0.5,-0.5,-0.5);
    cube(0.1,0.1,0.1);
    glPopMatrix();

    // light stand
    glPushMatrix();
    glTranslatef(85,42,0);
    glScalef(10,1,1);
    glTranslatef(-0.5,-0.5,-0.5);
    cube(0.8,0.6,0.2);
    glPopMatrix();

    // Bulb line
    glPushMatrix();
    glTranslatef(85,38,0);
    glScalef(1,7,1);
    glTranslatef(-0.5,-0.5,-0.5);
    cube(0.9,0.9,0.9);
    glPopMatrix();

    // texture light glutsolidshere


    glPushMatrix();
    glTranslatef(85,35,0);
    glutSolidSphere(2,16,16);
    glPopMatrix();

    glPopMatrix();

}
spot_lighting()
{
    // base
    glPushMatrix();

    glPushMatrix();
    glTranslatef(-100,20,0);
    glScalef(1,45,1);
    glTranslatef(-0.5,-0.5,-0.5);
    cube(1.0,1.0,0.1);
    glPopMatrix();

    // light stand
    glPushMatrix();
    glTranslatef(-105,42,0);
    glScalef(10,1,1);

    glTranslatef(-0.5,-0.5,-0.5);
    cube(0.8,0.6,0.2);
    glPopMatrix();


    // Bulb line
    glPushMatrix();
    glTranslatef(-105,38,0);
    glScalef(1,7,1);
    glTranslatef(-0.5,-0.5,-0.5);
    cube(0.1,1.0,0.1);
    glPopMatrix();
    //Solud sphere
    glPushMatrix();
    glTranslatef(-105,35,0);
    glutSolidSphere(2,16,16);
    glPopMatrix();

    glPopMatrix();

}

void main_light()
{
    GLfloat no_light[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat light_ambient[]  = {1.0, 1.0, 1.0, 1.0};
    GLfloat light_diffuse[]  = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_position[] = { 0.0, 150.0, 0, 1.0 };

    glEnable( GL_LIGHT7);

    glLightfv( GL_LIGHT7, GL_AMBIENT, light_ambient);
    glLightfv( GL_LIGHT7, GL_DIFFUSE, light_diffuse);
    glLightfv( GL_LIGHT7, GL_SPECULAR, light_specular);

    glEnable( GL_LIGHT7);
    if (main_light_switch)
    {
        glLightfv( GL_LIGHT7, GL_AMBIENT, light_ambient);
        glLightfv( GL_LIGHT7, GL_DIFFUSE, light_diffuse);
        glLightfv( GL_LIGHT7, GL_SPECULAR, light_specular);

    }
    else
    {
        glLightfv( GL_LIGHT7, GL_AMBIENT, no_light);
        glLightfv( GL_LIGHT7, GL_DIFFUSE, no_light);
        glLightfv( GL_LIGHT7, GL_SPECULAR, no_light);

    }


    glLightfv( GL_LIGHT7, GL_POSITION, light_position);

}

void light_function_0(float x, float y, float z)
{
    // Light Specification
    GLfloat no_light[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat light_ambient[] = {0.1, 0.1, 0.1, 1.0};
    GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1 };
    GLfloat light_specular[] = { 1, 1, 1, 1 };
    GLfloat light_position[] = { x, y, z, 1.0 };

    glEnable( GL_LIGHT0);
    if (light_switch_0)
    {
        glLightfv( GL_LIGHT0, GL_AMBIENT, light_ambient);
        glLightfv( GL_LIGHT0, GL_DIFFUSE, light_diffuse);
        glLightfv( GL_LIGHT0, GL_SPECULAR, light_specular);

    }
    else
    {
        glLightfv( GL_LIGHT0, GL_AMBIENT, no_light);
        glLightfv( GL_LIGHT0, GL_DIFFUSE, no_light);
        glLightfv( GL_LIGHT0, GL_SPECULAR, no_light);

    }


    glLightfv( GL_LIGHT0, GL_POSITION, light_position);

}

void light_function_1(float x, float y, float z)
{
    // Light Specification
    GLfloat no_light[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat light_ambient[] = {0.1, 0.1, 0.1, 1.0};
    GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1 };
    GLfloat light_specular[] = { 1, 1, 1, 1 };
    GLfloat light_position[] = { x, y, z, 1.0 };

    glEnable( GL_LIGHT1);
    if (light_switch_1)
    {
        glLightfv( GL_LIGHT1, GL_AMBIENT, light_ambient);
        glLightfv( GL_LIGHT1, GL_DIFFUSE, light_diffuse);
        glLightfv( GL_LIGHT1, GL_SPECULAR, light_specular);

    }
    else
    {
        glLightfv( GL_LIGHT1, GL_AMBIENT, no_light);
        glLightfv( GL_LIGHT1, GL_DIFFUSE, no_light);
        glLightfv( GL_LIGHT1, GL_SPECULAR, no_light);

    }


    glLightfv( GL_LIGHT1, GL_POSITION, light_position);

}


void spot_light_function(float x, float y, float z)
{
    // Light Specification
    GLfloat no_light[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat light_ambient[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat light_diffuse[] = { 0.1, 1.0, 0.1, 1 };
    GLfloat light_specular[] = { 1, 1, 1, 1 };
    GLfloat light_position[] = { x, y, z, 1.0 };

    glEnable( GL_LIGHT2);
    if (spot_light_switch)
    {
        glLightfv( GL_LIGHT2, GL_AMBIENT, light_ambient);
        glLightfv( GL_LIGHT2, GL_DIFFUSE, light_diffuse);
        glLightfv( GL_LIGHT2, GL_SPECULAR, light_specular);

    }
    else
    {
        glLightfv( GL_LIGHT2, GL_AMBIENT, no_light);
        glLightfv( GL_LIGHT2, GL_DIFFUSE, no_light);
        glLightfv( GL_LIGHT2, GL_SPECULAR, no_light);

    }


    glLightfv( GL_LIGHT2, GL_POSITION, light_position);
    GLfloat direction[]= {0,-1,0,1};
    GLfloat cut_off=70;
    glLightfv(GL_LIGHT2,GL_SPOT_DIRECTION,direction);
    glLightf(GL_LIGHT2,GL_SPOT_CUTOFF,cut_off);

}
void axes()
{
    float length = 10;
    float width = 0.3;

    // X-axis
    glPushMatrix();
    glTranslatef(length/2,0,0);
    glScalef(length,width,width);
    glTranslatef(-0.5,-0.5,-0.5);
    cube(1,0,0);
    glPopMatrix();

    // Y-axis
    glPushMatrix();
    glTranslatef(0,length/2,0);
    glScalef(width,length,width);
    glTranslatef(-0.5,-0.5,-0.5);
    cube(0,1,0);
    glPopMatrix();

    // Z-axis
    glPushMatrix();
    glTranslatef(0,0,length/2);
    glScalef(width,width,length);
    glTranslatef(-0.5,-0.5,-0.5);
    cube(0,0,1);
    glPopMatrix();
}


void car_wheels()
{
     // wheel 1
    glPushMatrix();
    glTranslatef(-5,5,18);
    glutSolidTorus(1,2,16,16);
    glPopMatrix();

    // wheel 2
    glPushMatrix();
    glTranslatef(5,5,18);
    glutSolidTorus(1,2,16,16);
    //glutSolidSphere(2,16,16);
    glPopMatrix();

    // wheel 3
    glPushMatrix();
    glTranslatef(5,5,22);
    glutSolidTorus(1,2,16,16);
    //glutSolidSphere(2,16,16);
    glPopMatrix();

    // wheel 4
    glPushMatrix();
    glTranslatef(-5,5,22);
    glutSolidTorus(1,2,16,16);
    //glutSolidSphere(2,10,10);
    glPopMatrix();
}
void cars()
{

    //base
    //glEnable(GL_TEXTURE_2D);
    //glBindTexture(GL_TEXTURE_2D,16);

    glPushMatrix();
    glTranslatef(0,15,20);
    glScalef(20,15,5);
    glTranslatef(-0.5,-0.5,-0.5);
    cube(0.0,0.5,4.0);
    glPopMatrix();
    //glDisable(GL_TEXTURE_2D);

    // Car front view
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,25);
    glPushMatrix();
    glTranslatef(10,15,20);
    glScalef(1,15,6);
    glTranslatef(-0.5,-0.5,-0.5);
    cube(1.0,1.0,1.0);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    // Front Lights 1
    material_property(0.5,0.5,0.5);
    glPushMatrix();
    glTranslatef(11,12,22);
    glTranslatef(-0.5,-0.5,-0.5);
    glRotatef(90,0,1,0);
    glutSolidTorus(0.2,.5,16,16);
    glPopMatrix();

    // Front Lights 2
    material_property(0.5,0.5,0.5);
    glPushMatrix();
    glTranslatef(11,12,18);
    glTranslatef(-0.5,-0.5,-0.5);
    glRotatef(90,0,1,0);
    glutSolidTorus(0.2,.5,16,16);
    glPopMatrix();


// upper base car
    glPushMatrix();
    glTranslatef(0,23,20);
    glScalef(20,1,8);
    glTranslatef(-0.5,-0.5,-0.5);
    cube(0.0,0.5,4.0);
    glPopMatrix();


// Left side car
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,24);
     glPushMatrix();
    glTranslatef(0,15,23);
    glScalef(20,15,0.2);
    glTranslatef(-0.5,-0.5,-0.5);
    cube(1.0,1.0,1.0);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    // right side car
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,24);
     glPushMatrix();
    glTranslatef(0,15,17);
    glScalef(20,15,0.2);
    glTranslatef(-0.5,-0.5,-0.5);
    cube(1.0,1.0,1.0);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

   car_wheels();



}
void car_animation()
{
    if(carx<=300)
    {

        carx +=0.2;
    }
    if(carx>=300)
    {
        carx = -300;

    }

    glutPostRedisplay();

}
void car_move()
{
    for(int i=1; i<=100; i+=140)
    {
        glPushMatrix();
        glTranslatef(carx+i,0,0);
        cars();
        if (car_switch){

        car_animation();
        }
        glPopMatrix();
    }

}

void bus_front()
{

    //base
    glPushMatrix();
    glTranslatef(0,10,0);
    glScalef(20,20,2);
    cube();
    glPopMatrix();

    // glass
    glPushMatrix();
    glTranslatef(2,18,2);
    glScalef(15,10,1);
    cube(0.0,1.0,1.0);
    glPopMatrix();

    // Lights 1
    glPushMatrix();
    glTranslatef(5,15,2);
    glutSolidTorus(0.5,1,16,16);
    glPopMatrix();

    // Lights 2
    glPushMatrix();
    glTranslatef(15,15,2);
    glutSolidTorus(0.5,1,16,16);
    glPopMatrix();

}
void bus_left_side()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,24);
    glPushMatrix();
    glTranslatef(-1,10,-40);
    glScalef(1,15,40);
    cube(0.5,0.5,0.5);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

}

void bus_right_side()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,25);
    glPushMatrix();
    glTranslatef(20,10,-40);
    glScalef(1,15,40);
    cube(0.5,0.5,0.5);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

}
void bus_back_side()
{
    //base
    glPushMatrix();
    glTranslatef(0,10,-41);
    glScalef(20,20,1);
    cube(.6,.8,.7);
    glPopMatrix();

}
void bus_wheels()
{


// wheel 1
    glPushMatrix();
    glTranslatef(2,5,-10);
    glRotatef(90,0,1,0);
    glutSolidTorus(2,4,16,16);
    glPopMatrix();

// wheel 2
    glPushMatrix();
    glTranslatef(20,5,-10);
    glRotatef(90,0,1,0);
    glutSolidTorus(2,4,16,16);
    glPopMatrix();

    // wheel 3
    glPushMatrix();
    glTranslatef(2,5,-35);
    glRotatef(90,0,1,0);
    glutSolidTorus(2,4,16,16);
    glPopMatrix();

// wheel 4
    glPushMatrix();
    glTranslatef(20,5,-35);
    glRotatef(90,0,1,0);
    glutSolidTorus(2,4,16,16);
    glPopMatrix();



}
void bus()
{
    glPushMatrix();
    // Body of Bus
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,16);
    glPushMatrix();
    glTranslatef(0,10,-40);
    glScalef(20,20,40);
    cube(1.0,1.0,1.0);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    //FRONT


    bus_front();



    // Bus left side

    bus_left_side();


    // Bus right side

    bus_right_side();


    // bus back side

    bus_back_side();


    // bus wheels

    bus_wheels();


    glPopMatrix();




}
void bus_animation()
{
    if(busz<=300)
    {

        busz +=0.20;
    }
    if(busz>=300)
    {
        busz = -300;

    }

    glutPostRedisplay();

}
void bus_move()
{
    for(int i=1; i<=150; i+=120)
    {
        glPushMatrix();
        glTranslatef(0,0,busz+i);
        bus();
        if (bus_switch)
        {

            bus_animation();
        }

        glPopMatrix();
    }


}

void bus_control()
{


        //bus_move();
        glPushMatrix();
        glTranslatef(-55,0,0);
        glScalef(0.5,0.5,0.5);
        bus_move();
        glPopMatrix();



}
void car_control()
{

        glPushMatrix();
        car_move();
        glPopMatrix();


}

void base_floor()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,20);

    glPushMatrix();
    glTranslatef(0,0,0);
    glScalef(600,1,500);
    glTranslatef(-0.5,-0.5,-0.5);
    cube(0.1,2.2,0.2);
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);

}



void park()
{
// wood
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,13);

    glPushMatrix();
    glTranslatef(160,30,-60);
    glScalef(2,30,2);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    //leaf

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,17);

    glPushMatrix();
    glTranslatef(160,50,-60);
    glScalef(8,12,8);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}
int sec_angle =90, min_angle= 30, hour_angle= 0;
int sec_inc_en = 1, min_inc_en = 1;
void sec_inc_enable(int a)
{
    sec_inc_en = a;
}
void sec_inc()
{
    if(sec_inc_en)
    {
        sec_angle -= 6;
        sec_inc_en = 0;
        glutTimerFunc(1000,sec_inc_enable,1);
    }

    glutPostRedisplay();
}
void min_inc_enable(int b)
{
    min_inc_en = b;
}
void min_inc()
{
    if(min_inc_en)
    {
        min_angle -= 6;
        min_inc_en = 0;
        glutTimerFunc(60000,min_inc_enable,1);
    }

    glutPostRedisplay();
}

void watch_stand()
{
    // Watch Base
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,24);
    glPushMatrix();
    glScalef(10,50,2);
    cube();
    glPopMatrix();
}
void wall_watch()
{


    //Draw Torus
    glPushMatrix();
    material_property(.2,0.8,0.8);
    glutSolidTorus(4,25,16,16);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    //center
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,23);

    glPushMatrix();
    glTranslatef(0,0,-0.5);
    glScalef(40,40,1);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);

    //sec_cube
    glPushMatrix();
    glRotatef(sec_angle,0,0,1);
    glTranslatef(10,0,0);
    glScalef(20,0.5,0.1);
    glTranslatef(-0.5,-0.5,-0.5);
    cube(1,0,0);
    sec_inc();
    glPopMatrix();

    //min_cube
    glPushMatrix();
    glRotatef(min_angle,0,0,1);
    glTranslatef(4,0,0);
    glScalef(8,0.5,0.1);
    glTranslatef(-0.5,-0.5,-0.5);
    cube(1,0,1);
    min_inc();
    glPopMatrix();

    //hour_cube
    glPushMatrix();
    glRotatef(hour_angle,0,0,1);
    glTranslatef(2.5,0,0);
    glScalef(5,0.5,0.1);
    glTranslatef(-0.5,-0.5,-0.5);
    cube(1,1,0);
    glPopMatrix();



}
void park_with_tree()
{
    // park tree 1
    for (int i=10; i<=100; i+=20)
    {
        glPushMatrix();
        glTranslatef(i,-20,0);
        park();
        glPopMatrix();

    }

    // park tree 2
    for (int i=10; i<=100; i+=20)
    {
        glPushMatrix();
        glTranslatef(i,-20,15);
        park();
        glPopMatrix();

    }

    // park tree 3
    for (int i=10; i<=100; i+=20)
    {
        glPushMatrix();
        glTranslatef(i,-20,-15);
        park();
        glPopMatrix();

    }

}

void tableBezier()
{
    int i, j;
    float x, y, z, r;				//current coordinates
    float x1, y1, z1, r1;			//next coordinates
    float theta;

    const float startx = 0, endx = ctrlpoints[L][0];
    //number of angular slices
    const float dx = (endx - startx) / nt;	//x step size
    const float dtheta = 2*PI / ntheta;		//angular step size

    float t=0;
    float dt=1.0/nt;
    float xy[2];
    BezierCurve( t,  xy);
    x = xy[0];
    r = xy[1];
    //rotate about z-axis
    float p1x,p1y,p1z,p2x,p2y,p2z;
    for ( i = 0; i < nt; ++i )  			//step through x
    {
        theta = 0;
        t+=dt;
        BezierCurve( t,  xy);
        x1 = xy[0];
        r1 = xy[1];

        //draw the surface composed of quadrilaterals by sweeping theta
        glBegin( GL_QUAD_STRIP );
        for ( j = 0; j <= ntheta; ++j )
        {
            theta += dtheta;
            double cosa = cos( theta );
            double sina = sin ( theta );
            y = r * cosa;
            y1 = r1 * cosa;	//current and next y
            z = r * sina;
            z1 = r1 * sina;	//current and next z

            //edge from point at x to point at next x
            glVertex3f (x, y, z);

            if(j>0)
            {
                getNormal3p(p1x,p1y,p1z,p2x,p2y,p2z,x, y, z);
            }
            else
            {
                p1x=x;
                p1y=y;
                p1z=z;
                p2x=x1;
                p2y=y1;
                p2z=z1;

            }
            glVertex3f (x1, y1, z1);

            //forms quad with next pair of points with incremented theta value
        }
        glEnd();
        x = x1;
        r = r1;
    } //for i

}

static void idle(void)
{
    glutPostRedisplay();
}


void showControlPoints()
{
    glPointSize(5.0);
    glColor3f(1.0, 0.0, 1.0);
    glBegin(GL_POINTS);
    for (int i = 0; i <=L; i++)
        glVertex3fv(&ctrlpoints[i][0]);
    glEnd();
}
void chair()
{

    float length=20;
    float width=1;

    //base seat
    glPushMatrix();
    glTranslatef(0,length/2,0);
    glScalef(length,width,length);
    glTranslatef(-0.5,-0.5,-0.5);
    cube();
    glPopMatrix();

    // leg base 1
    glPushMatrix();
    glTranslatef(length/2 -width/2,0,length/2-width/2);
    glScalef(width,length,width);
    glTranslatef(-0.5,-0.5,-0.5);
    cube(7,0,0);
    glPopMatrix();
    // leg base 2
    glPushMatrix();
    glTranslatef(length/2 -width/2,0,- length/2 +width/2);
    glScalef(width,length,width);
    glTranslatef(-0.5,-0.5,-0.5);
    cube(7,0,0);
    glPopMatrix();
    // leg base 3
    glPushMatrix();
    glTranslatef(-length/2 +width/2,0,+ length/2 -width/2);
    glScalef(width,length,width);
    glTranslatef(-0.5,-0.5,-0.5);
    cube(7,0,0);
    glPopMatrix();
    // leg  base 4
    glPushMatrix();
    glTranslatef(-length/2 +width/2,0,- length/2 +width/2);
    glScalef(width,length,width);
    glTranslatef(-0.5,-0.5,-0.5);
    cube(7,0,0);
    glPopMatrix();

    // upper  1
    glPushMatrix();
    glTranslatef(length/2 -width/2,length,length/2-width/2);
    glScalef(width,length,width);
    glTranslatef(-0.5,-0.5,-0.5);
    cube(4,0,5);
    glPopMatrix();
    // upper 2
    glPushMatrix();
    glTranslatef(-length/2 -width/2,length,length/2+width/2);
    glScalef(width,length,width);
    glTranslatef(-0.5,-0.5,-0.5);
    cube(4,0,5);
    glPopMatrix();

    // upper close 1
    glPushMatrix();
    glTranslatef(0,length,length/2);
    glScalef(length,length/6,0);
    glTranslatef(-0.5,-0.5,-0.5);
    cube(7,0,0);
    glPopMatrix();
    // upper close 2
    glPushMatrix();
    glTranslatef(0,length+5,length/2);
    glScalef(length,length/6,0);
    glTranslatef(-0.5,-0.5,-0.5);
    cube(4,0,5);
    glPopMatrix();

    // upper close 3
    glPushMatrix();
    glTranslatef(0,length-5,length/2);
    glScalef(length,length/6,0);
    glTranslatef(-0.5,-0.5,-0.5);
    cube(4,0,5);
    glPopMatrix();


}


void chair_table()
{
    // CURVED CHAIR TABLE PART
    glPushMatrix();
    material_property(0.5,0.4,0.3);
    glRotatef( 90, 0.0, 0.0, 1.0);
    //glRotatef( 180, 1.0, 0.0, 1.0);
    glTranslated(-22,0,0);
    glScalef(3,3,3);
   tableBezier();
    glPopMatrix();

     // 1st chair
    glPushMatrix();
    glTranslatef(0,-5,-20);
    glRotatef(180,0,1,0);
    glScalef(0.5,0.5,0.5);
    chair();
    glPopMatrix();

    //2nd chair
    glPushMatrix();

    glTranslatef(0,-5,20);
    //glRotatef(180,0,1,0);
    glScalef(0.5,0.5,0.5);
    chair();
    glPopMatrix();

    //3rd chair
    glPushMatrix();
    glTranslatef(-22,-5,0);
    glRotatef(-90,0,1,0);
    glScalef(0.5,0.5,0.5);
    chair();
    glPopMatrix();
    //4th chair
    glPushMatrix();
    glTranslatef(22,-5,0);
    glRotatef(90,0,1,0);
    glScalef(0.5,0.5,0.5);
    chair();
    glPopMatrix();


}




void sky()
{
    GLfloat m_amb[] = {0.529, 0.808, 0.922, 7.0, 1.0};
    GLfloat m_diff[] = {0.529, 0.808, 0.922, 1.0, 1.0};
    GLfloat m_spec[] = {0.529, 0.808, 0.922, 1.0, 1.0};
    GLfloat m_sh[] = {100};

    glMaterialfv(GL_FRONT, GL_AMBIENT, m_amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, m_diff);
    glMaterialfv(GL_FRONT, GL_SPECULAR, m_spec);
    glMaterialfv(GL_FRONT, GL_SHININESS, m_sh);


    glPushMatrix();
        glTranslatef(0, 0, 450);
        ///glTranslatef(0, 0, -20);
        ///glTranslatef(0, 0, -25);
        glutSolidSphere(600, 600, 600);
    glPopMatrix();
}


void curved_animation()
{
    const double t = glutGet(GLUT_ELAPSED_TIME) / 5000.0;
    const double a = t*90.0;
    if(wired)
    {
        glPolygonMode( GL_FRONT, GL_LINE ) ;
        glPolygonMode( GL_BACK, GL_LINE ) ;

    }
    else
    {
        glPolygonMode( GL_FRONT,GL_FILL ) ;
        glPolygonMode( GL_BACK, GL_FILL ) ;
    }

    glPushMatrix();

    if(animat)
        glRotated(a,0,0,1);

    glRotatef( anglex, 1.0, 0.0, 0.0);
    glRotatef( angley, 0.0, 1.0, 0.0);         	//rotate about y-axis
    glRotatef( anglez, 0.0, 0.0, 1.0);

    glRotatef( 90, 0.0, 0.0, 1.0);
    glTranslated(-3.5,0,0);
    glGetDoublev( GL_MODELVIEW_MATRIX, modelview ); //get the modelview info

    //matColor(0,0,0,20);   // front face color
    //matColor(0.0,0,0,20,1);  // back face color
    material_property(0,1,0);


   /*
   // chair table -------------- if i call the chair table from here, the chair table will be up down left right
    glPushMatrix();
    glTranslatef(200,10,-10);
    glScalef(0.5,0.5,0.5);
    chair_table();
    glPopMatrix();
*/

    if(shcpt)
    {
        //matColor(0.0,0.0,0.9,20);
        material_property(0,1,0);

        showControlPoints();
    }

    glPopMatrix();

}
static void key(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'u': // up


        eyeY++;
        lookY++;

        break;
    case 'd': // down

        eyeY--;
        lookY--;
        break;
    case 'a': // look right
        lookX++;

        break;
    case 'b':
        lookX--;
        // look left
        break;

    case 'p': // rotate left
        rot--;
        break;
    case 'q': // rotate right
        rot++;
        break;
    case 'l': // left

        eyeX--;
        lookX--;


        break;
    case 'r': // right

        eyeX++;
        lookX++;

        break;
    case '+': // zoom in
        eyeZ--;
        break;

    case '-':
        //zoom out
        eyeZ++;
        break;
    case 's':
    case 'S':
        shcpt=!shcpt;
        break;



    case 'x':
        anglex = ( anglex + 3 ) % 360;
        break;
    case 'X':
        anglex = ( anglex - 3 ) % 360;
        break;

    case 'y':
        angley = ( angley + 3 ) % 360;
        break;
    case 'Y':
        angley = ( angley - 3 ) % 360;
        break;

    case 'z':
        anglez = ( anglez + 3 ) % 360;
        break;
    case 'Z':
        anglez = ( anglez - 3 ) % 360;
        break;

    case '1':
        light_switch_0 =! light_switch_0;
        break;

    case '2':
        light_switch_1 =! light_switch_1;
        break;

    case '3':
        spot_light_switch =! spot_light_switch;
        break;
    case '4':
        main_light_switch=!main_light_switch;
        break;
    case '8':
        car_switch=!car_switch;
        break;
    case '9':
        bus_switch=!bus_switch;
        break;
    }

    glutPostRedisplay();
}


void display_settings()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    int frustum_window = 8;
    glFrustum(-frustum_window, frustum_window, -frustum_window, frustum_window, 4, 300);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();



    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity() ;

    gluLookAt(eyeX,eyeY,eyeZ, lookX,lookY,lookZ, 0,1,0);
    //glViewport(0, 0, window_width, window_height);

    glRotatef(rot, 0,1,0);

}

static void display(void)
{


    display_settings();
    base_floor();
   // sun_moon();
    main_light();
    //axes();
    buiding();
    road();
    playground();
    shop();
 sky();
    // Wall Watch with base tower
    glPushMatrix();
    glTranslatef(350,0,0);
    glScalef(0.5,0.5,0.5);
    // watch function start
    watch_stand();
    glPushMatrix();
    glTranslatef(5,75,0);

    wall_watch();
    glPopMatrix();
    // watch function end

    glPopMatrix();

    // more floor
    glPushMatrix();
    glTranslatef(200,0,0);
    base_floor();
    glPopMatrix();

    // Sign board 1
    glPushMatrix();
    glTranslatef(-25,-10,-20);
    signboard();
    glPopMatrix();

    // Sign board 2
    glPushMatrix();
    glTranslatef(200,-10,-20);
    signboard();
    glPopMatrix();


    //Car Move


    // BUS
    //bus();
    //bus_car_control();
     bus_control();
     car_control();
    // Swimming Pool
    //swimming_pool();

    glPushMatrix();
    glTranslatef(-205,45,-40);
    glScalef(5,5,5);
    glTranslatef(-0.5,-0.5,-0.5);
    //cube(1.0,0,0);
    glPopMatrix();
    glPushMatrix();
    spot_light_function(-205,45,-40);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-100,0,-40);
    spot_lighting();
    glPopMatrix();
    //spot_light_function(-205,40,-10);
    //spot_light_function(-205,40,-20);
    //spot_light_function(-205,40,-30);
    //spot_light_function(-205,40,-40);
    /*
    for (int i=0;i<=50;i=i+10)
    {
        //glPushMatrix();
        spot_light_function(-205,40,-i);
        //glPopMatrix();
    }
    */

    /*
    for (int i=-10;i<=50;i=i+10)
    {

    glPushMatrix();
    glTranslatef(-100,0,-i);
    spot_lighting();
    glPopMatrix();
    }
    */


park_with_tree();
    // more park
    glPushMatrix();
    glTranslatef(-450,0,-20);
    park_with_tree();
    glPopMatrix();




    // Spot light for shop
    glPushMatrix();
    spot_light_function(-105,40,0);
    glTranslatef(5,0,0);
    spot_lighting();
    glPopMatrix();
    //trees();

    // Tree inside building
    trees_beside_building();


    //wood_leaf();

    // Traffic Signal 1
    glPushMatrix();
    glTranslatef(-10,-20,10);
    traffic_signal();
    glPopMatrix();

    // Traffic Signal 2
    glPushMatrix();
    glTranslatef(140,-20,10);
    traffic_signal();
    glPopMatrix();

    /*
    //light 1
    glPushMatrix();
    light_function_0(40,35,0);
    glTranslatef(40,-20,0);
    road_light();
    //cube();
    glPopMatrix();
    // light 2
    glPushMatrix();
    light_function_1(-100,35,0);
    glTranslatef(-100,0,0);
    road_light();
    //cube();
    glPopMatrix();
    */

    // Road light position 1
    /*
    glPushMatrix();
    glTranslatef(125,20,0);
    glScalef(2,2,2);
    cube(1.0,0,0);
    glPopMatrix();

    */
    for (int i=10; i<=100; i=i+20)
    {
        glPushMatrix();
        light_function_0(125,20,-i);
        glTranslatef(40,-20,-i);
        road_light();
        //cube();
        glPopMatrix();
    }
    // Road light position 2
    /*
    glPushMatrix();
    glTranslatef(-65,20,0);
    cube(1.0,0,0);
    glPopMatrix();
    */
    for (int i=10; i<=100; i=i+20)
    {
        glPushMatrix();
        light_function_1(-65,20,-i);
        glTranslatef(-150,-20,-i);
        road_light();
        //cube();
        glPopMatrix();
    }


//     chair table
    glPushMatrix();
    glTranslatef(300,10,-20);
    glScalef(0.9,0.9,0.9);
    chair_table();
    glPopMatrix();

    // Curved Setting for animation
    curved_animation();



    glFlush();
    glutSwapBuffers();
}
void texture_function()
{
    // Building texture
    LoadTexture("F:\\re\\bmp\\puc.bmp",1);
    LoadTexture("F:\\re\\bmp\\bu6.bmp",2);
    LoadTexture("F:\\re\\bmp\\bu2.bmp",3);
    LoadTexture("F:\\re\\bmp\\bu3.bmp",4);
    LoadTexture("F:\\re\\bmp\\bu4.bmp",5);
    LoadTexture("F:\\re\\bmp\\bu5.bmp",6);

    // Playground grass
    LoadTexture("F:\\re\\bmp\\football1.bmp",7);
    //  Traffic Light
    LoadTexture("F:\\re\\bmp\\traffic2.bmp",8);

    // Signboard
    LoadTexture("F:\\re\\bmp\\drive.bmp",9);

    // roads
    LoadTexture("F:\\re\\bmp\\road.bmp",10);

    // cafe
    LoadTexture("F:\\re\\bmp\\pi.bmp",11);

    // Trees
    LoadTexture("F:\\re\\bmp\\leaf1.bmp",12);

    // wood 1
    LoadTexture("F:\\re\\bmp\\wood.bmp",13);

    LoadTexture("F:\\re\\bmp\\leaf1.bmp",14);

    // road light bulb texture
    LoadTexture("F:\\re\\bmp\\bulb.bmp",15);
    // Cars
    LoadTexture("F:\\re\\bmp\\car1.bmp",16);

    // park lead
    LoadTexture("F:\\re\\bmp\\leaf1.bmp",17);

    // shop 3
    LoadTexture("F:\\re\\bmp\\bata.bmp",18);
    // Chocolate color
    LoadTexture("F:\\re\\bmp\\chocolate.bmp",19);
    // base floor
    LoadTexture("F:\\re\\bmp\\gr.bmp",20);
    // sun moon
    LoadTexture("F:\\re\\bmp\\sun1.bmp",21);
    //swimming pool water
    LoadTexture("F:\\re\\bmp\\water.bmp",22);
    // watch
    LoadTexture("F:\\re\\bmp\\watch1.bmp",23);
    // BUS Texture
    LoadTexture("F:\\re\\bmp\\bus.bmp",24);
    // BUS
    LoadTexture("F:\\re\\bmp\\bus.bmp",25);

     // shop 3
     LoadTexture("F:\\re\\bmp\\c.bmp",26);

     // shop 4
     LoadTexture("F:\\re\\bmp\\re.bmp",27);

      // shop 5
     LoadTexture("F:\\re\\bmp\\ve2.bmp",28);

  // bank
     LoadTexture("F:\\re\\bmp\\ucb.bmp",29);
     // mini
     LoadTexture("F:\\re\\bmp\\mini.bmp",30);



}
int main(int argc, char *argv[])
{

    glutInit(&argc, argv);
    glutInitWindowSize(window_width,window_height);
    glutInitWindowPosition(300,10);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glutCreateWindow("City");

    texture_function();

    cout<<"-------------------------------------------------------------------------------------------"<<endl;
    cout<<"-------------------------------------------- Rithika Saha ---------------------------------"<<endl;
    cout<<"-------------------------------------------- Roll:1883 --------------------------------"<<endl;
    cout<<"-------------------------------------------------------------------------------------------"<<endl;


    cout<<"--------------------------------------------Instruction------------------------------------"<<endl;
    cout<<"\t Press : u -> Move Up"<<endl;
    cout<<"\t Press : d -> Move Down"<<endl;
    cout<<"\t Press : l -> Move Left"<<endl;
    cout<<"\t Press : r -> Move Right"<<endl;

    cout<<"\t Press : p -> Rotate Right"<<endl;
    cout<<"\t Press : q -> Rotate Left"<<endl;

    cout<<"\t Press : a -> Look Right"<<endl;
    cout<<"\t Press : b -> Look Left"<<endl;

    cout<<"\t Press : + -> Zoom In"<<endl;
    cout<<"\t Press : - -> Zoom Out"<<endl;

    cout<<"---------------------------------------------------------------"<<endl;


    cout<<"\t Press : 1 -> ON/OFF Light 1"<<endl;
    cout<<"\t Press : 2 -> ON/OFF Light 2"<<endl;
    cout<<"\t Press : 3 -> ON/OFF Spot Light"<<endl;
    cout<<"\t Press : 4 -> ON/OFF Sun/Moon Light"<<endl;

    cout<<"\t Press : 8 -> START/STOP Car Moving"<<endl;
    cout<<"\t Press : 9 -> START/STOP Bus Moving"<<endl;




 glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutMouseFunc(processMouse);
    glutIdleFunc(idle);




    glShadeModel( GL_SMOOTH );
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHTING);

sndPlaySound("ja.wav", SND_ASYNC);


    glutMainLoop();



    return 0;
}
