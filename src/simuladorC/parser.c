#include <stdio.h>
#include <stdlib.h>
#include "parser.h"

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


Result * parseTxt(char * fileName){

	int c;
	FILE *fd;

	fd = fopen(fileName,"r");
	printf("About to open %s \n",fileName);

	//Read entire file content


	fseek(fd, 0, SEEK_END);
	long fsize = ftell(fd);
	fseek(fd, 0, SEEK_SET);  //same as rewind(f);

	char *string = malloc(fsize + 1);
	fread(string, fsize, 1, fd);
	fclose(fd);

	//Parse content by ;
	char *lineStr;
	char *token2;
	int i;
	char * x;
	char * y;
	int * xList;
	int * yList;
	int line;
	line = 0;
	Result * r = malloc(sizeof(Result));

	lineStr = strsep(&string, ";");

	//First line is gridP,sourceX,sourceY,sensX,sensY,nPoints

	r->gridSize = atoi((char*)strsep(&lineStr, ","));
	r->sourceX = atoi((char*)strsep(&lineStr, ","));
	r->sourceY = atoi((char*)strsep(&lineStr, ","));
	r->sensorX = atoi((char*)strsep(&lineStr, ","));
	r->sensorY = atoi((char*)strsep(&lineStr, ","));
	r->nPoints = atoi((char*)strsep(&lineStr, ","))-1;


	xList = malloc(sizeof(int)*(r->nPoints) );
	yList = malloc(sizeof(int)*(r->nPoints) );

	//All the other lines are points X,Y
	for (line = 0; line<(r->nPoints);line++){
		if ((lineStr = strsep(&string, ";")) != NULL){

		x = (char*)strsep(&lineStr, ",");
		y = (char*)strsep(&lineStr, ",");
		xList[line] = atoi(x);
		yList[line] = atoi(y);
		//printf("Point %s %s \n", x,y);
		
		}

	}
	free(string);
	printf("There are  %d lines \n",line);

	

	r->xcords = xList;
	r->ycords = yList;

	return r;

}


