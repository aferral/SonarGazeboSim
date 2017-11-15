
typedef struct Result {
	int gridSize;
	int sourceX;
	int sourceY;
	int sensorX;
	int sensorY;
	int nPoints;
	int * xcords;
	int * ycords;
} Result;

Result * parseTxt(char * fileName);
