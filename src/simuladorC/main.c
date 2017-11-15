#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "parser.h"
#include "model.h"
#include <cv.h>
#include <highgui.h>

#define PI 3.14159265
#define max(x, y) (((x) > (y)) ? (x) : (y))
#define min(x, y) (((x) < (y)) ? (x) : (y))

/* 

THIS CODE IS AN ADAPTATION OF THE CODE FROM

2-D Acoustic FDTD Simulation Demo for Processing 2 (rev. Nov. 27th, 2014)
    by Yoshiki NAGATANI
      http://ultrasonics.jp/nagatani/e/
      https://twitter.com/nagataniyoshiki
   For more detailed information about FDTD method (including 3-D elastic simulation),
   please refer our papers on simulation. ==> http://ultrasonics.jp/nagatani/fdtd/
   Thank you.

*/


int main(){
//-------------------------Get parameters from textFile-------------------------
int sparsePoints;
int NX;					// Spatial Size for X direction [pixels]
int NY;					// Spatial Size for Y direction [pixels]
int PointSourceX;			// X coordinate of Point Sound Source [pixel]
int PointSourceY;			// Y coordinate of Point Sound Source [pixel]
int sensorX;			// X coordinate of Point Sound Source [pixel]
int sensorY;			// Y coordinate of Point Sound Source [pixel]

Result * r =  parseTxt("out.txt");
sparsePoints = r->nPoints;
NX = r->gridSize;
NY = r->gridSize;

PointSourceX = r->sourceY;
PointSourceY = r->sourceX;

sensorX = r-> sensorY;
sensorY = r-> sensorX;

//-------------------------Declare variables------------------------- 
FILE * outF;
int showSim = 0;			//If 0 dont show GUI windows else show GUI windows
float image_intensity = 400;		// Brightness of Acoustic Field (>0)
float model_intensity = 200;		// Brightness of Model (>0)

//old value 0.01
float dx = 0.02;			// Spatial Resolution [m/pixel]
float dt = 15.0e-6;			// Temporal Resolution [s/step]
float dt_over_dx = dt/dx;		// for more efficient calculation

//Era 0.73 ahora 5

float rho[]   = {1.29,	2};		// Densities [kg/m^3]
float kappa[] = {142.0e3, 160.0e3};	// Bulk Moduli [Pa]

float freq = 1.0e3;			// Frequency of Initial Waveform [Hz]


//[NX+1][NY]
float ** Vx;				// Particle Velocity for X direction [m/s]

//[NX][NY+1]
float ** Vy;				// Particle Velocity for Y direction [m/s]

//[NX][NY]
float ** P;				// Sound Pressure [Pa]

//[NX][NY]
int ** Model;				// Model


//[4][NY]
float ** Mur_X1;			// Mur's 2nd-Order Absorption Layer

//[4][NY]
float ** Mur_X2;			// Mur's 2nd-Order Absorption Layer

//[NX][4]
float ** Mur_Y1;			// Mur's 2nd-Order Absorption Layer

//[NX][4]
float ** Mur_Y2;			// Mur's 2nd-Order Absorption Layer



int i,j,x,y;

int n_point = 0;
float sig_point;

float col,col_r,col_g,col_b;

int max_iter = 1000;
int current = 0;

// Image variables
int grayOrJet = 1;

uchar *data;
CvSize imSize;
imSize = cvSize(NX, NY);
IplImage* image = cvCreateImage(imSize, IPL_DEPTH_8U, 3);

int channels;
int step;
int height;
int width;
height    = image->height;
width     = image->width;
step      = image->widthStep;
channels  = image->nChannels;
data      = (uchar *)image->imageData;

//-------------------------Initializa parameters -------------------------------

outF = fopen("outSensor.txt", "w");

/* Initialize Field Values */
InitFieldfloat(&Vx,NX+1,NY);
InitFieldfloat(&Vy,NX,NY+1);
InitFieldfloat(&P,NX,NY);
InitFieldint(&Model,NX,NY);


InitFieldfloat(&Mur_X1,4,NY);
InitFieldfloat(&Mur_X2,4,NY);
InitFieldfloat(&Mur_Y1,NX,4);
InitFieldfloat(&Mur_Y2,NX,4);

/* Create Model */

for(i=0;i<sparsePoints;i++){
	x = (r->xcords)[i];
	y = (r->ycords)[i];
	Model[x][y] = 1;	// This is a wall
}


//  LOOP THE MODEL

while (current < max_iter){

	/* Update the Acoustic Field (This is the main part of FDTD !!)  */
	UpdatePV(&Vx,&Vy,&P,rho,kappa,Model,dt_over_dx,NX,NY);


	/* Mur's 2nd Order Absorption */
	Mur2nd(&Mur_X1,&Mur_X2,&Mur_Y1,&Mur_Y2,&P,rho,kappa,Model,dt,dx,NX,NY);
	//printf("%f \n",Mur_X1[0][0]);

	/* Initial Waveform from a Point Source (1 pulse of sinusoidal wave with Hann window) */
	if( n_point < (1.0/freq)/dt ){
		double res;
		res = (2.0*PI*freq*n_point*dt);

		sig_point = (1.0-cos(res))/2.0 * sin((2.0*PI*freq*n_point*dt));
		P[PointSourceX][PointSourceY] += sig_point;
	}


	/* Draw the Acoustic Field */
	for(i=0; i<NX; i++){
		for(j=0; j<NY; j++){
			col = abs((float)(P[i][j]*image_intensity + Model[i][j]*model_intensity));	// Value in Gray scale

			if (grayOrJet == 0){
				col_r = 255*min(max(col,0),1);
				col_g = col_r;
				col_b = col_r;
			}
			else{
				col = min(4.0,col);	// avoid black color
				col_r = 255*min(max(min(col-1.5,-col+4.5),0),1);
				col_g = 255*min(max(min(col-0.5,-col+3.5),0),1);
				col_b = 255*min(max(min(col+0.5,-col+2.5),0),1);
			}

			data[i*step+j*channels+0] = col_b;
			data[i*step+j*channels+1] = col_g;
			data[i*step+j*channels+2] = col_r;
		}
			
	
	}
	
	fprintf(outF, "%f  \n",P[sensorX][sensorY]);
	cvShowImage("mainWin", image );

	// wait for a key
	cvWaitKey(5);


	//printf("I just ended a iteration \n");
	n_point++;
	current++;
}
fclose(outF);
return 0;
}

