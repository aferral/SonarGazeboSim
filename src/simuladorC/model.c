#include <stdio.h>
#include <stdlib.h>
#include "model.h"
#include <math.h>

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


void InitFieldfloat(float*** x, int xSize,int ySize){
	int i,j;
	*x = malloc(xSize * sizeof(float*));

	for (i=0; i<xSize; i++){
	(*x)[i] = malloc(ySize * sizeof(float));
	}

	//Now initialize all values with 0
	for (i=0; i<xSize; i++){
		for (j=0; j<ySize; j++){
		(*x)[i][j] = 0.0;
		}
	}


	return;
}

void InitFieldint(int*** x, int xSize,int ySize){
	int i,j;
	(*x) = malloc(xSize * sizeof(int*));

	for (i=0; i<xSize; i++){
	(*x)[i] = malloc(ySize * sizeof(int));
	}

	//Now initialize all values with 0
	for (i=0; i<xSize; i++){
		for (j=0; j<ySize; j++){
		(*x)[i][j] = 0;
	}
	}

	return;
}

void UpdatePV(
	float*** vx,
	float*** vy,
	float*** P,
	float* rho, 
	float* kappa,
	int ** model,
	float dt_over_dx,
	int NX,
	int NY) {

	//Update VX,VY
	int i,j,t1,t2;
	for(i=1;i<NX;i++){
		for(j=0;j<NY;j++){
			t1 = model[i][j];
			t2 = model[i-1][j];
			(*vx)[i][j] += - dt_over_dx / ( (rho[t1]+rho[t2])/2.0 ) * ( (*P)[i][j] - (*P)[i-1][j] );
		}
	}

	for(i=0;i<NX;i++){
		for(j=1;j<NY;j++){
			t1 = model[i][j];
			t2 = model[i][j-1];
			(*vy)[i][j] += - dt_over_dx / ( (rho[t1]+rho[t2])/2.0 ) * ( (*P)[i][j] - (*P)[i][j-1] );
		}
	}

	//Update P

	for(i=0;i<NX;i++){
		for(j=0;j<NY;j++){
			t1 = model[i][j];
			(*P)[i][j] += - ( kappa[t1] * dt_over_dx ) * ( ( (*vx)[i+1][j] - (*vx)[i][j] ) + ( (*vy)[i][j+1] - (*vy)[i][j] ) );
		}
	}

}



void Mur2nd(
	float*** dMur_X1,
	float*** dMur_X2,
	float*** dMur_Y1,
	float*** dMur_Y2,
	float*** dirP,
	float* rho, 
	float* kappa,
	int ** Model,
	float dt,
	float dx,
	int NX,
	int NY) 
{

	/*
	float** (*dMur_X1);
	float** (*dMur_X2);
	float** (*dMur_Y1);
	float** (*dMur_Y2);
	float** (*dirP);
	*/

	//(*dMur_X1) = *d(*dMur_X1);
	//(*dMur_X2) = *d(*dMur_X2);
	//(*dMur_Y1) = *d(*dMur_Y1);
	//(*dMur_Y2) = *d(*dMur_Y2);
	//(*dirP) = *dirP;
	
	
	float vel;	// Wave velocity at the boundary
	int i,j;

	/* Mur's 2nd Order Absorption */
	for(i=2;i<NX-2;i++){
		vel = sqrt(kappa[Model[i][0]]/rho[Model[i][0]]);
		(*dirP)[i][0] = - (*dMur_Y2)[i][1]
		          + (vel*dt-dx)/(vel*dt+dx) * ( (*dirP)[i][1] + (*dMur_Y2)[i][0] )
		          + (2.0*dx)/(vel*dt+dx) * ( (*dMur_Y1)[i][0] + (*dMur_Y1)[i][1] )
		          + (dx*vel*vel*dt*dt)/(2.0*dx*dx*(vel*dt+dx))
		             * (   (*dMur_Y1)[i+1][0] - 2.0 * (*dMur_Y1)[i][0]
		                 + (*dMur_Y1)[i-1][0] + (*dMur_Y1)[i+1][1]
		                 - 2.0 * (*dMur_Y1)[i][1] + (*dMur_Y1)[i-1][1] );
		vel = sqrt(kappa[Model[i][NY-1]]/rho[Model[i][NY-1]]);
		(*dirP)[i][NY-1] = - (*dMur_Y2)[i][2]
		          + (vel*dt-dx)/(vel*dt+dx) * ( (*dirP)[i][NY-2] + (*dMur_Y2)[i][3] )
		          + (2.0*dx)/(vel*dt+dx) * ( (*dMur_Y1)[i][3] + (*dMur_Y1)[i][2] )
		          + (dx*vel*vel*dt*dt)/(2.0*dx*dx*(vel*dt+dx))
		             * (   (*dMur_Y1)[i+1][3] - 2.0 * (*dMur_Y1)[i][3]
		                 + (*dMur_Y1)[i-1][3] + (*dMur_Y1)[i+1][2]
		                 - 2.0 * (*dMur_Y1)[i][2] + (*dMur_Y1)[i-1][2] );
	}
	for(j=2;j<NY-2;j++){
		vel = sqrt(kappa[Model[0][j]]/rho[Model[0][j]]);
		(*dirP)[0][j] = - (*dMur_X2)[1][j]
		          + (vel*dt-dx)/(vel*dt+dx) * ( (*dirP)[1][j] + (*dMur_X2)[0][j] )
		          + (2.0*dx)/(vel*dt+dx) * ( (*dMur_X1)[0][j] + (*dMur_X1)[1][j] )
		          + (dx*vel*vel*dt*dt)/(2.0*dx*dx*(vel*dt+dx))
		             * (   (*dMur_X1)[0][j+1] - 2.0 * (*dMur_X1)[0][j]
		                 + (*dMur_X1)[0][j-1] + (*dMur_X1)[1][j+1]
		                 - 2.0 * (*dMur_X1)[1][j] + (*dMur_X1)[1][j-1] );
		vel = sqrt(kappa[Model[NX-1][j]]/rho[Model[NX-1][j]]);
		(*dirP)[NX-1][j] = - (*dMur_X2)[2][j]
		          + (vel*dt-dx)/(vel*dt+dx) * ( (*dirP)[NX-2][j] + (*dMur_X2)[3][j] )
		          + (2.0*dx)/(vel*dt+dx) * ( (*dMur_X1)[3][j] + (*dMur_X1)[2][j] )
		          + (dx*vel*vel*dt*dt)/(2.0*dx*dx*(vel*dt+dx))
		             * (   (*dMur_X1)[3][j+1] - 2.0 * (*dMur_X1)[3][j]
		                 + (*dMur_X1)[3][j-1] + (*dMur_X1)[2][j+1]
		                 - 2.0 * (*dMur_X1)[2][j] + (*dMur_X1)[2][j-1] );
	}

	/* Mur's 1st Order Absorption for 4 corners*/
	i = 1;
		vel = sqrt(kappa[Model[i][0]]/rho[Model[i][0]]);
		(*dirP)[i][0] = (*dMur_Y1)[i][1] + (vel*dt-dx)/(vel*dt+dx) * ((*dirP)[i][1] - (*dMur_Y1)[i][0]);
		vel = sqrt(kappa[Model[i][NY-1]]/rho[Model[i][NY-1]]);
		(*dirP)[i][NY-1] = (*dMur_Y1)[i][2] + (vel*dt-dx)/(vel*dt+dx) * ((*dirP)[i][NY-2] - (*dMur_Y1)[i][3]);
	i = NX-2;
		vel = sqrt(kappa[Model[i][0]]/rho[Model[i][0]]);
		(*dirP)[i][0] = (*dMur_Y1)[i][1] + (vel*dt-dx)/(vel*dt+dx) * ((*dirP)[i][1] - (*dMur_Y1)[i][0]);
		vel = sqrt(kappa[Model[i][NY-1]]/rho[Model[i][NY-1]]);
		(*dirP)[i][NY-1] = (*dMur_Y1)[i][2] + (vel*dt-dx)/(vel*dt+dx) * ((*dirP)[i][NY-2] - (*dMur_Y1)[i][3]);
	j = 1;
		vel = sqrt(kappa[Model[0][j]]/rho[Model[0][j]]);
		(*dirP)[0][j] = (*dMur_X1)[1][j] + (vel*dt-dx)/(vel*dt+dx) * ((*dirP)[1][j] - (*dMur_X1)[0][j]);
		vel = sqrt(kappa[Model[NX-1][j]]/rho[Model[NX-1][j]]);
		(*dirP)[NX-1][j] = (*dMur_X1)[2][j] + (vel*dt-dx)/(vel*dt+dx) * ((*dirP)[NX-2][j] - (*dMur_X1)[3][j]);
	j = NY - 2;
		vel = sqrt(kappa[Model[0][j]]/rho[Model[0][j]]);
		(*dirP)[0][j] = (*dMur_X1)[1][j] + (vel*dt-dx)/(vel*dt+dx) * ((*dirP)[1][j] - (*dMur_X1)[0][j]);
		vel = sqrt(kappa[Model[NX-1][j]]/rho[Model[NX-1][j]]);
		(*dirP)[NX-1][j] = (*dMur_X1)[2][j] + (vel*dt-dx)/(vel*dt+dx) * ((*dirP)[NX-2][j] - (*dMur_X1)[3][j]);

	/* Copy Previous Values */
	for(i=0;i<NX;i++){
		/* Copy 1st Old Values to 2nd Old Values*/
		(*dMur_Y2)[i][0] = (*dMur_Y1)[i][0];
		(*dMur_Y2)[i][1] = (*dMur_Y1)[i][1];
		(*dMur_Y2)[i][2] = (*dMur_Y1)[i][2];
		(*dMur_Y2)[i][3] = (*dMur_Y1)[i][3];

		/* Copy Present Values */
		(*dMur_Y1)[i][0] = (*dirP)[i][0];
		(*dMur_Y1)[i][1] = (*dirP)[i][1];
		(*dMur_Y1)[i][2] = (*dirP)[i][NY-2];
		(*dMur_Y1)[i][3] = (*dirP)[i][NY-1];
	}
	for(j=0;j<NY;j++){
		/* Copy 1st Old Values to 2nd Old Values*/
		(*dMur_X2)[0][j] = (*dMur_X1)[0][j];
		(*dMur_X2)[1][j] = (*dMur_X1)[1][j];
		(*dMur_X2)[2][j] = (*dMur_X1)[2][j];
		(*dMur_X2)[3][j] = (*dMur_X1)[3][j];

		/* Copy Present Values */
		(*dMur_X1)[0][j] = (*dirP)[0][j];
		(*dMur_X1)[1][j] = (*dirP)[1][j];
		(*dMur_X1)[2][j] = (*dirP)[NX-2][j];
		(*dMur_X1)[3][j] = (*dirP)[NX-1][j];
	}
}

