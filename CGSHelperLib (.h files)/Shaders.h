//Grants access to Math Functionallity 
#include "Math.h"
//Uses time for srand() seed in main.cpp
#include <time.h>

//Function Pointer used for current Pixel shader passes in a color reference
void (*ActivePixelShader)(unsigned int&) = 0;

//Function Pointer used for current Pixel shader passes in a color reference
void (*ActiveVertexShader)(Vertex&) = 0;

//Variables for Shaders
Matrix SV_WorldMatrix;



//Changes Pixel Color to random color using rand()
//You must seed rand using srand() in your main.cpp for different seed usage
void PS_RANDOM_COLOR(unsigned int& randomPixelColor) {

	//Assigns alpha to be the max value so no blending occurs 
	unsigned int alphaChannel = (unsigned int)MAX_COLOR_VALUE;
	//Assigns color channels to a random value between 0 and max value (255)
	unsigned int redChannel = rand() % MAX_COLOR_VALUE_NO_ZERO;
	unsigned int greenChannel = rand() % MAX_COLOR_VALUE_NO_ZERO;
	unsigned int blueChannel = rand() % MAX_COLOR_VALUE_NO_ZERO;

	//Shifts the channels to their proper positions
	alphaChannel = (alphaChannel << 24);
	redChannel	= (redChannel << 16);
	greenChannel = (greenChannel << 8);
	
	//Joins color channels together for the final random color and sets that to the Color passed into the shader
	randomPixelColor = (alphaChannel | redChannel | greenChannel | blueChannel);
}
//Changes Pixel Color to Green
void PS_GREEN(unsigned int& pixelColor) {

	pixelColor = GREEN;
}
//Changes Pixel Color to White
void PS_WHITE(unsigned int& pixelColor) {

	pixelColor = WHITE;
}
//Applies the current world matrix to all vertices
void VS_WORLD(Vertex& currentVertex) { currentVertex = VertextMult4x4Matrix(currentVertex, SV_WorldMatrix); }
//Applies the current view matrix defined in (Define.h) to all vertices
void VS_VIEW(Vertex& currentVertex) {

	Matrix View_Matrix = IdentityMatrix();
	View_Matrix = MatrixMultMatrix(RotationMatrixX(CAMERA_ORIGNAL_X_ROTATION), View_Matrix);
	View_Matrix = MatrixMultMatrix(TranslateMatrix(CAMERA_OFFSET), View_Matrix);
	View_Matrix = InverseMatrix(View_Matrix);

	currentVertex = VertextMult4x4Matrix(currentVertex, View_Matrix);

}
//Applies all needed matrices to our vertices
void VS_PROJECTION(Vertex& currentVertex) {

	VS_WORLD(currentVertex);
	VS_VIEW(currentVertex);
	Vertex tempVertex = VertextMult4x4Matrix(currentVertex, GetPerspectiveProjectionMatix());

	currentVertex.posXposYposZ_W[0] = tempVertex.posXposYposZ_W[0] / tempVertex.posXposYposZ_W[3];
	currentVertex.posXposYposZ_W[1] = tempVertex.posXposYposZ_W[1] / tempVertex.posXposYposZ_W[3];
	currentVertex.posXposYposZ_W[2] = tempVertex.posXposYposZ_W[2] / tempVertex.posXposYposZ_W[3];
	currentVertex.posXposYposZ_W[3] = tempVertex.posXposYposZ_W[3];
	

}