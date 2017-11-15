

void InitFieldfloat(float*** x, int xSize,int ySize);

void InitFieldint(int*** x,int xSize,int ySize);



void UpdatePV(
	float*** vx,
	float*** vy,
	float*** P,
	float* rho, 
	float* kappa,
	int ** model,
	float dt_over_dx,
	int NX,
	int NY);

void Mur2nd(
	float*** dMur_X1,
	float*** dMur_X2,
	float*** dMur_Y1,
	float*** dMur_Y2,
	float*** P,
	float* rho, 
	float* kappa,
	int ** Model,
	float dt,
	float dx,
	int NX,
	int NY); 
