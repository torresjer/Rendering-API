//Allows Us To Use The Currated Definitions 
#include "Define.h"
/*

-------- ******** 4X4 Matrix Diagram For Reference ******** --------
	  ____											 ____
	 |												     |
	 |		A			B			C			D		 |
	 |		 [0][0]		 [0][1]		 [0][2]		 [0][3]	 |
	 |												     |
	 |		E			F			G			H	     |
	 |		 [1][0]		 [1][1]		 [1][2]		 [1][3]	 |
	 |													 |
	 |		I			J			K			L		 |
	 |		 [2][0]		 [2][1]		 [2][2]		 [2][3]	 |
	 |													 |
	 |		M			N			O			P		 |
	 |		 [3][0]		 [3][1]		 [3][2]		 [3][3]	 |
	 |	   												 |
	 |___											 ____|

	 Matrix 1

	  ____											 ____
	 |												     |
	 |		a			b			c			d		 |
	 |		 [0][0]		 [0][1]		 [0][2]		 [0][3]	 |
	 |												     |
	 |		e			f			g			h	     |
	 |		 [1][0]		 [1][1]		 [1][2]		 [1][3]	 |
	 |													 |
	 |		i			j			k			l		 |
	 |		 [2][0]		 [2][1]		 [2][2]		 [2][3]	 |
	 |													 |
	 |		m			n			o			p		 |
	 |		 [3][0]		 [3][1]		 [3][2]		 [3][3]	 |
	 |	   												 |
	 |___											 ____|


	 Matrix 2
	  ____											 ____
	 |												     |
	 |		A			E			I			D		 |
	 |		 [0][0]		 [0][1]		 [0][2]		 [0][3]	 |
	 |												     |
	 |		B			F			J			H	     |
	 |		 [1][0]		 [1][1]		 [1][2]		 [1][3]	 |
	 |													 |
	 |		C			G			K			L		 |
	 |		 [2][0]		 [2][1]		 [2][2]		 [2][3]	 |
	 |													 |
	 |		M			N			O			P		 |
	 |		 [3][0]		 [3][1]		 [3][2]		 [3][3]	 |
	 |	   												 |
	 |___											 ____|

	  Transposed Positions for the postion for the 4x4 matrix


				 ---- ******** Resualt of the above two Matrixes multipliying in Row Major Order ******** ----
	  ____																											 ____
	 |																												     |
	 |		Aa + Be + Ci + Dm			Ab + Bf + Cj + Dn			Ac + Bg + Ck + Do			Ad + Bh + Cl + Dp		 |
	 |						[0][0]						[0][1]						[0][2]						[0][3]	 |
	 |																													 |
	 |		Ea + Fe + Gi + Hm			Eb + Ff + Gj + Hn			Ec + Fg + Gk + Ho			Ed + Fh + Gl + Hp	     |
	 |						[1][0]						[1][1]						[1][2]					    [1][3]	 |
	 |																													 |
	 |		Ia + Je + Ki + Lm			Ib + Jf + Kj + Ln			Ic + Jg + Kk + Lo			Id + Jh + Kl + Lp		 |
	 |						[2][0]						[2][1]						[2][2]						[2][3]	 |
	 |																													 |
	 |		Ma + Ne + Oi + Pm			Mb + Nf + Oj + Pn			Mc + Ng + Ok + Po			Md + Nh + Ol + Pp		 |
	 |						[3][0]						[3][1]						[3][2]						[3][3]	 |
	 |	   																												 |
	 |___																											 ____|

*/

//Converts a PosX and a PosY into a 1D coordinate based on the width of the raster
unsigned int TwoDToOneDConverstion(unsigned int widthOfRaster, Point Pos) { return Pos.posY * widthOfRaster + Pos.posX; }

//Converts 3D NDC coordinates to 2D Pixel coordinates
void ThreeDtoTwoDPixelCoordinates(Vertex &currentNDCPos) { 

	//Converts the current NDC x value to pixel space
	currentNDCPos.posXposYposZ_W[0] = (1 + currentNDCPos.posXposYposZ_W[0]) * RASTER_CENTER.posX;
	//Converts the current NDC y value to pixel space
	currentNDCPos.posXposYposZ_W[1] = (1 - currentNDCPos.posXposYposZ_W[1]) * RASTER_CENTER.posY;

	currentNDCPos.posXposYposZ_W[0] = floor(currentNDCPos.posXposYposZ_W[0]);
	currentNDCPos.posXposYposZ_W[1] = floor(currentNDCPos.posXposYposZ_W[1]);

}

//Converts degrees to Radians
void DegreesToRads(float& degrees) { degrees = degrees* (PI / STRAIGHT_ANGLE); }

//Gives degrees based on the amount of frames happening per second
float EvenlyRotateDegreesPerSec(float amountOfFramesPerSec) { return TOTAL_DEGREES_TO_ROTATE_IN_TIME_SEC  / (TOTAL_CUBE_ROTATION_TIME_IN_SEC * amountOfFramesPerSec); }

//Gives the Cotangent of the given degrees
float Cotangent(float degrees) {

	DegreesToRads(degrees);
	float tempCos = cos(degrees);
	float tempSin = sin(degrees);
	return (tempCos / tempSin);
}

//Gives a value between a and b based on the ratio passed through.
int LinearInterpolation(float ratio, int a, int b) { return ((b - a) * ratio + a); }

//Mutliplys a Given Vertex positon by a 4x4 matrix
Vertex VertextMult4x4Matrix(Vertex vertex, Matrix matrix) {

	//takes the orignal positions for all of the vetex elements 
	float orignalXpos = vertex.posXposYposZ_W[0];
	float orignalYpos = vertex.posXposYposZ_W[1];
	float orignalZpos = vertex.posXposYposZ_W[2];
	float orignalWpos = vertex.posXposYposZ_W[3];

	//This statment sets pos x of the vertex's array to XA + YE + ZI + WM
	vertex.posXposYposZ_W[0] = (orignalXpos * matrix.matrixElements[0][0]) + (orignalYpos * matrix.matrixElements[1][0]) + (orignalZpos * matrix.matrixElements[2][0]) +  matrix.matrixElements[3][0];
	//This statment sets pos y of the vertex's array to XB + YF + ZJ + WN
	vertex.posXposYposZ_W[1] = (orignalXpos * matrix.matrixElements[0][1]) + (orignalYpos * matrix.matrixElements[1][1]) + (orignalZpos * matrix.matrixElements[2][1]) +  matrix.matrixElements[3][1];
	//This statment sets pos z of the vertex's array to XC + YG + ZK + WO
	vertex.posXposYposZ_W[2] = (orignalXpos * matrix.matrixElements[0][2]) + (orignalYpos * matrix.matrixElements[1][2]) + (orignalZpos * matrix.matrixElements[2][2]) +  matrix.matrixElements[3][2];
	////This statment sets pos W of the vertex's array to XD + YH + ZL + WP
	vertex.posXposYposZ_W[3] = (orignalXpos * matrix.matrixElements[0][3]) + (orignalYpos * matrix.matrixElements[1][3]) + (orignalZpos * matrix.matrixElements[2][3]) + (orignalWpos * matrix.matrixElements[3][3]);

	//returns array after calculations are done
	return vertex;
}

//Mutliplys a Given Vertex positon by a 3x3 matrix
Vertex VertextMult3x3PortionOfMatrix(Vertex vertex, Matrix matrix) {
	//takes the orignal positions for all of the vetex elements 
	float orignalXpos = vertex.posXposYposZ_W[0];
	float orignalYpos = vertex.posXposYposZ_W[1];
	float orignalZpos = vertex.posXposYposZ_W[2];


	//This statment sets pos x of the vertex's array to XA + YD + ZG 
	vertex.posXposYposZ_W[0] = (orignalXpos * matrix.matrixElements[0][0]) + (orignalYpos * matrix.matrixElements[1][0]) + (orignalZpos * matrix.matrixElements[2][0]);
	//This statment sets pos y of the vertex's array to XB + YE + ZH 
	vertex.posXposYposZ_W[1] = (orignalXpos * matrix.matrixElements[0][1]) + (orignalYpos * matrix.matrixElements[1][1]) + (orignalZpos * matrix.matrixElements[2][1]);
	//This statment sets pos z of the vertex's array to XC + YF + ZI
	vertex.posXposYposZ_W[2] = (orignalXpos * matrix.matrixElements[0][2]) + (orignalYpos * matrix.matrixElements[1][2]) + (orignalZpos * matrix.matrixElements[2][2]);


	//returns array after calculations are done
	return vertex;
}
 
//Mutliplys a Given Matrix by a given matrix and returns the result
Matrix MatrixMultMatrix(Matrix latestEffectApplied, Matrix currentMatrix) {

	//intializes Matrix to store result
	Matrix resultingMatrix;


	/*Calculates row one by doing the following calculations */

	//Aa + Be + Ci + Dm								
	resultingMatrix.matrixElements[0][0] =
		//Aa
		(latestEffectApplied.matrixElements[0][0] * currentMatrix.matrixElements[0][0]) +
		//Be
		(latestEffectApplied.matrixElements[0][1] * currentMatrix.matrixElements[1][0]) +
		//Ci
		(latestEffectApplied.matrixElements[0][2] * currentMatrix.matrixElements[2][0]) +
		//Dm
		(latestEffectApplied.matrixElements[0][3] * currentMatrix.matrixElements[3][0]);

	//Ab + Bf + Cj + Dn	
	resultingMatrix.matrixElements[0][1] =
		//Ab
		(latestEffectApplied.matrixElements[0][0] * currentMatrix.matrixElements[0][1]) +
		//Bf
		(latestEffectApplied.matrixElements[0][1] * currentMatrix.matrixElements[1][1]) +
		//Cj
		(latestEffectApplied.matrixElements[0][2] * currentMatrix.matrixElements[2][1]) +
		//Dn
		(latestEffectApplied.matrixElements[0][3] * currentMatrix.matrixElements[3][1]);

	//Ac + Bg + Ck + Do
	resultingMatrix.matrixElements[0][2] =
		//Ac
		(latestEffectApplied.matrixElements[0][0] * currentMatrix.matrixElements[0][2]) +
		//Bg
		(latestEffectApplied.matrixElements[0][1] * currentMatrix.matrixElements[1][2]) +
		//Ck
		(latestEffectApplied.matrixElements[0][2] * currentMatrix.matrixElements[2][2]) +
		//Do
		(latestEffectApplied.matrixElements[0][3] * currentMatrix.matrixElements[3][2]);

	//Ad + Bh + Cl + Dp
	resultingMatrix.matrixElements[0][3] =
		//Ad
		(latestEffectApplied.matrixElements[0][0] * currentMatrix.matrixElements[0][3]) +
		//Bh
		(latestEffectApplied.matrixElements[0][1] * currentMatrix.matrixElements[1][3]) +
		//Cl
		(latestEffectApplied.matrixElements[0][2] * currentMatrix.matrixElements[2][3]) +
		//Dp
		(latestEffectApplied.matrixElements[0][3] * currentMatrix.matrixElements[3][3]);


	/*Calculates row two by doing the following calculations */

	//Ea + Fe + Gi + Hm								
	resultingMatrix.matrixElements[1][0] =
		//Ea
		(latestEffectApplied.matrixElements[1][0] * currentMatrix.matrixElements[0][0]) +
		//Fe
		(latestEffectApplied.matrixElements[1][1] * currentMatrix.matrixElements[1][0]) +
		//Gi
		(latestEffectApplied.matrixElements[1][2] * currentMatrix.matrixElements[2][0]) +
		//Hm
		(latestEffectApplied.matrixElements[1][3] * currentMatrix.matrixElements[3][0]);

	//Eb + Ff + Gj + Hn
	resultingMatrix.matrixElements[1][1] =
		//Eb
		(latestEffectApplied.matrixElements[1][0] * currentMatrix.matrixElements[0][1]) +
		//Ff
		(latestEffectApplied.matrixElements[1][1] * currentMatrix.matrixElements[1][1]) +
		//Gj
		(latestEffectApplied.matrixElements[1][2] * currentMatrix.matrixElements[2][1]) +
		//Hn
		(latestEffectApplied.matrixElements[1][3] * currentMatrix.matrixElements[3][1]);

	//Ec + Fg + Gk + Ho	
	resultingMatrix.matrixElements[1][2] =
		//Ec
		(latestEffectApplied.matrixElements[1][0] * currentMatrix.matrixElements[0][2]) +
		//Fg
		(latestEffectApplied.matrixElements[1][1] * currentMatrix.matrixElements[1][2]) +
		//Gk
		(latestEffectApplied.matrixElements[1][2] * currentMatrix.matrixElements[2][2]) +
		//Ho
		(latestEffectApplied.matrixElements[1][3] * currentMatrix.matrixElements[3][2]);

	//Ed + Fh + Gl + Hp
	resultingMatrix.matrixElements[1][3] =
		//Ed
		(latestEffectApplied.matrixElements[1][0] * currentMatrix.matrixElements[0][3]) +
		//Fh
		(latestEffectApplied.matrixElements[1][1] * currentMatrix.matrixElements[1][3]) +
		//Gl
		(latestEffectApplied.matrixElements[1][2] * currentMatrix.matrixElements[2][3]) +
		//Hp
		(latestEffectApplied.matrixElements[1][3] * currentMatrix.matrixElements[3][3]);

	//ROW THREE
	
	//Ia + Je + Ki + Lm									
	resultingMatrix.matrixElements[2][0] =
		//Ia
		(latestEffectApplied.matrixElements[2][0] * currentMatrix.matrixElements[0][0]) +
		//Je
		(latestEffectApplied.matrixElements[2][1] * currentMatrix.matrixElements[1][0]) +
		//Ki
		(latestEffectApplied.matrixElements[2][2] * currentMatrix.matrixElements[2][0]) +
		//Lm
		(latestEffectApplied.matrixElements[2][3] * currentMatrix.matrixElements[3][0]);

	//Ib + Jf + Kj + Ln
	resultingMatrix.matrixElements[2][1] =
		//Ib
		(latestEffectApplied.matrixElements[2][0] * currentMatrix.matrixElements[0][1]) +
		//Jf
		(latestEffectApplied.matrixElements[2][1] * currentMatrix.matrixElements[1][1]) +
		//Kj
		(latestEffectApplied.matrixElements[2][2] * currentMatrix.matrixElements[2][1]) +
		//Ln
		(latestEffectApplied.matrixElements[2][3] * currentMatrix.matrixElements[3][1]);

	//Ic + Jg + Kk + Lo
	resultingMatrix.matrixElements[2][2] =
		//Ic
		(latestEffectApplied.matrixElements[2][0] * currentMatrix.matrixElements[0][2]) +
		//Jg
		(latestEffectApplied.matrixElements[2][1] * currentMatrix.matrixElements[1][2]) +
		//Kk
		(latestEffectApplied.matrixElements[2][2] * currentMatrix.matrixElements[2][2]) +
		//Lo
		(latestEffectApplied.matrixElements[2][3] * currentMatrix.matrixElements[3][2]);

	//Id + Jh + Kl + Lp
	resultingMatrix.matrixElements[2][3] =
		//Id
		(latestEffectApplied.matrixElements[2][0] * currentMatrix.matrixElements[0][3]) +
		//Jh
		(latestEffectApplied.matrixElements[2][1] * currentMatrix.matrixElements[1][3]) +
		//Kl
		(latestEffectApplied.matrixElements[2][2] * currentMatrix.matrixElements[2][3]) +
		//Lp
		(latestEffectApplied.matrixElements[2][3] * currentMatrix.matrixElements[3][3]);

	//ROW Four
	
	//Ma + Ne + Oi + Pm									
	resultingMatrix.matrixElements[3][0] =
		//Ma
		(latestEffectApplied.matrixElements[3][0] * currentMatrix.matrixElements[0][0]) +
		//Ne
		(latestEffectApplied.matrixElements[3][1] * currentMatrix.matrixElements[1][0]) +
		//Oi
		(latestEffectApplied.matrixElements[3][2] * currentMatrix.matrixElements[2][0]) +
		//Pm
		(latestEffectApplied.matrixElements[3][3] * currentMatrix.matrixElements[3][0]);
	
	//Mb + Nf + Oj + Pn
	resultingMatrix.matrixElements[3][1] =
		//Mb
		(latestEffectApplied.matrixElements[3][0] * currentMatrix.matrixElements[0][1]) +
		//Nf
		(latestEffectApplied.matrixElements[3][1] * currentMatrix.matrixElements[1][1]) +
		//Oj
		(latestEffectApplied.matrixElements[3][2] * currentMatrix.matrixElements[2][1]) +
		//Pn
		(latestEffectApplied.matrixElements[3][3] * currentMatrix.matrixElements[3][1]);

	//Mc + Ng + Ok + Po
	resultingMatrix.matrixElements[3][2] =
		//Mc
		(latestEffectApplied.matrixElements[3][0] * currentMatrix.matrixElements[0][2]) +
		//Ng
		(latestEffectApplied.matrixElements[3][1] * currentMatrix.matrixElements[1][2]) +
		//Ok
		(latestEffectApplied.matrixElements[3][2] * currentMatrix.matrixElements[2][2]) +
		//Po
		(latestEffectApplied.matrixElements[3][3] * currentMatrix.matrixElements[3][2]);

	//Md + Nh + Ol + Pp
	resultingMatrix.matrixElements[3][3] =
		//Md
		(latestEffectApplied.matrixElements[3][0] * currentMatrix.matrixElements[0][3]) +
		//Nh
		(latestEffectApplied.matrixElements[3][1] * currentMatrix.matrixElements[1][3]) +
		//Ol
		(latestEffectApplied.matrixElements[3][2] * currentMatrix.matrixElements[2][3]) +
		//Pp
		(latestEffectApplied.matrixElements[3][3] * currentMatrix.matrixElements[3][3]);

	//Returns the product of the Matrix Multiplication  
	return resultingMatrix;
}

//Defines an identity Matrix and returns it
Matrix IdentityMatrix() {
	//defines the identity Matrix and Returns it
	Matrix identityMatrix = { { { 1, 0, 0, 0 } ,
							    { 0, 1, 0, 0 } ,
							    { 0, 0, 1, 0 } ,
							    { 0, 0, 0, 1 } }
	};

	return identityMatrix;
}

//Defines a Rotation Matrix around the X-axis with the given degrees
Matrix RotationMatrixX(float degrees) {

	//Converts degrees to radians and returns a reference
	DegreesToRads(degrees);

	//Defines the matrix
	Matrix rotationXMatrix = { { { 1,	0,				0,			  0 } ,
								 { 0,	cos(degrees),  -sin(degrees), 0 } ,
								 { 0,	sin(degrees),   cos(degrees), 0 } ,
								 { 0,	0,				0,			  1 } }
	};

	return rotationXMatrix;

}

//Defines a Rotation Matrix around the Y-axis with the given degrees
Matrix RotationMatrixY(float degrees) {

	//Converts degrees to radians and returns a reference
	DegreesToRads(degrees);

	//Defines the matrix
	Matrix rotationYMatrix = { { { cos(degrees),	0,	 sin(degrees),  0 } ,
								 { 0,				1,	 0,			    0 } ,
								 { -sin(degrees),	0,   cos(degrees),  0 } ,
								 { 0,				0,	 0,			    1 } }
	};

	return rotationYMatrix;

}

//Defines a Rotation Matrix around the Z-axis with the given degrees
Matrix RotationMatrixZ(float degrees) {

	//Converts degrees to radians and returns a reference
	DegreesToRads(degrees);

	//Defines the matrix
	Matrix rotationZMatrix = { { { cos(degrees),	-sin(degrees),	0,	0 } ,
								 { sin(degrees),	 cos(degrees),  0,	0 } ,
								 { 0,				 0,				1,	0 } ,
								 { 0,				 0,				0,	1 } }
	};

	return rotationZMatrix;

}

//Transpose 3x3 portion of Matrix and returns the Matrix
Matrix Transpose3x3PortionOfMatrix(Matrix desiredMatrix) {
	float tempMatrixElement = 0.0f;

	tempMatrixElement = desiredMatrix.matrixElements[0][1];
	desiredMatrix.matrixElements[0][1] = desiredMatrix.matrixElements[1][0];
	desiredMatrix.matrixElements[1][0] = tempMatrixElement;

	tempMatrixElement = desiredMatrix.matrixElements[0][2];
	desiredMatrix.matrixElements[0][2] = desiredMatrix.matrixElements[2][0];
	desiredMatrix.matrixElements[2][0] = tempMatrixElement;

	tempMatrixElement = desiredMatrix.matrixElements[1][2];
	desiredMatrix.matrixElements[1][2] = desiredMatrix.matrixElements[2][1];
	desiredMatrix.matrixElements[2][1] = tempMatrixElement;

	return desiredMatrix;
}

//Inverses the Matrix and returns the result
Matrix InverseMatrix(Matrix desiredMatrix) {

	Matrix transposed3x3Portion = Transpose3x3PortionOfMatrix(desiredMatrix);

	Vertex positionVertex = { {desiredMatrix.matrixElements[3][0], desiredMatrix.matrixElements[3][1], desiredMatrix.matrixElements[3][2], 1}, WHITE };

	positionVertex = VertextMult3x3PortionOfMatrix(positionVertex, transposed3x3Portion);

	desiredMatrix.matrixElements[3][0] = -positionVertex.posXposYposZ_W[0];
	desiredMatrix.matrixElements[3][1] = -positionVertex.posXposYposZ_W[1];
	desiredMatrix.matrixElements[3][2] = -positionVertex.posXposYposZ_W[2];
		
	return desiredMatrix;
};

//Translates the Matrix by the given xOffset, yOffset, zOffset
Matrix TranslateMatrix(Vertex desiredOffset) {

	Matrix translationMatrix = { { { 1,									0,									0,									0 } ,
								   { 0,									1,									0,									0 } ,
								   { 0,									0,									1,									0 } ,
								   { desiredOffset.posXposYposZ_W[0],	desiredOffset.posXposYposZ_W[1],	desiredOffset.posXposYposZ_W[2],	1 } }
	};

	return translationMatrix;
}

//Perspective Projection Matrix
Matrix GetPerspectiveProjectionMatix() {

	if ((NEAR_PLANE != 0) && (FAR_PLANE > NEAR_PLANE)) {
		
		float Yscale = Cotangent(HALF_FOV_IN_DEGREES);
		float Xscale = Yscale * ASPECT_RATIO;
		Matrix PerspectiveMatrix = { { { Xscale, 0,		 0,														0 } ,
									   { 0,		 Yscale, 0,														0 } ,
									   { 0,		 0,		 FAR_PLANE / (FAR_PLANE - NEAR_PLANE),					1 } ,
									   { 0,		 0,		 -(FAR_PLANE * NEAR_PLANE) / (FAR_PLANE - NEAR_PLANE),	0 } }
		};

		return PerspectiveMatrix;
		
	}
}

//Scales the current Matrix by the given value
Matrix	ScalingMatrix(float scaleMatrixby) {

	Matrix scalingMatrix = { { { scaleMatrixby,	0,				0,				0 } ,
							   { 0,				scaleMatrixby,	0,				0 } ,
							   { 0,				0,				scaleMatrixby,	0 } ,
							   { 0,				0,				0,				1 } }
	};

	return scalingMatrix;

}


