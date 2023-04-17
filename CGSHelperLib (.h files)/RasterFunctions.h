//allows Us the usage of Shadders 
#include "Shaders.h"

/*		*******Drawing Tools That Can Be Used to Draw To The Raster Correctly********		*/

//Converts BGRA to ARGB
unsigned int ConvertBGRAtoARGB(unsigned int colorToConvert, Point drawPos) {
	//uses bitwise operaterators to first isolate the desired channel 
	//then shifts them to the left or right for proper formatting
	unsigned int blueChannel = ((colorToConvert & 0xFF000000) >> 24);
	unsigned int greenChannel = ((colorToConvert & 0x00FF0000) >> 8);
	unsigned int redChannel = ((colorToConvert & 0x0000FF00) << 8);
	unsigned int alphaChannel = ((colorToConvert & 0x000000FF) << 24);

	//Joins them together to produce the proper color and returns the value
	return (alphaChannel | redChannel | greenChannel | blueChannel);
}

//Does the Linear interpolation for the alpha blend
unsigned int Blend(float alphaRatio, int rasterColor, int tileColor) { return ((tileColor - rasterColor) * alphaRatio + rasterColor); }

//Alpha Blend takes the background color and blends it with alpha value given by tile
unsigned int AlphaBlend(unsigned int rasterColor, unsigned int tileColor) {

	//Breaks the rasterColor into its indvidual channels
	//rasterColorAlpha does not actually matter as it has already been drawn to the screen so the alpha channel will always be 0
	unsigned int rasterColorAlpha =	((rasterColor & ALPHA) >> 24);
	unsigned int rasterColorRed	=	((rasterColor & RED) >> 16);
	unsigned int rasterColorGreen =	((rasterColor & GREEN) >> 8);
	unsigned int rasterColorBlue =	(rasterColor & BLUE);

	//Breaks the tileColor into its indvidual channels
	unsigned int tileColorAlpha = ((tileColor & ALPHA) >> 24);
	unsigned int tileColorRed = ((tileColor & RED) >> 16);
	unsigned int tileColorGreen = ((tileColor & GREEN) >> 8);
	unsigned int tileColorBlue = (tileColor & BLUE);

	//Calulates the ratio for the amount of blending needed for spicfic tilecolor for this pixel
	float alphaRatio = (float)tileColorAlpha / MAX_COLOR_VALUE;

	//blends all of channels with respect to the alpha ratio
	unsigned int blendedAlpha = Blend(alphaRatio, rasterColorAlpha, tileColorAlpha);
	unsigned int blendedRed = Blend(alphaRatio, rasterColorRed, tileColorRed);
	unsigned int blendedGreen = Blend(alphaRatio, rasterColorGreen, tileColorGreen);
	unsigned int blendedBlue = Blend(alphaRatio, rasterColorBlue, tileColorBlue);

	//combines all color channels with the blending now in effect and returns the value
	return ((blendedAlpha << 24) | (blendedRed << 16) | (blendedGreen << 8) | blendedBlue );

}

//A function that takes in a 2d cordinate and draws a pixel in position
void DrawPixel(unsigned int* rasterArray, unsigned int widthOfRaster, Point drawPos, unsigned int color) {
	
	//Checks to see if the current draw position is within the range of the Raster to avoid out of range exception 
	if ((((drawPos.posX <= RASTER_WIDTH_INT) && (drawPos.posY <= RASTER_HEIGHT_INT)) && (( drawPos.posX >= 0 ) && (drawPos.posY >= 0)))) {
		//converts the 2d coordinates into 1d index for the raster array
		unsigned int rasterIndex = TwoDToOneDConverstion(widthOfRaster, drawPos);
		//Gets the value for color to check if alpha blending is necessary to save on proccessing 
		unsigned int alphaValue = ((color & ALPHA) >> 24);
		//Checks to see if alpha blending is necessary 
		if (alphaValue < MAX_COLOR_VALUE) {
			//Alpha blending was necessary and is done to the current color before drawing it to the Raster, 
			//then we break out of the fuction using the return
			rasterArray[rasterIndex] = AlphaBlend(rasterArray[rasterIndex], color);
			return;
		}
	
		//Alpha blending was not necessary and the current color is drawn to the Raster
		rasterArray[rasterIndex] = color;
	}
	
}
//A function that takes in a 2d cordinate and draws a pixel in position, as well as storing the z-pos for that pixel
void DrawPixel(unsigned int* rasterArray, float* zBuffer, unsigned int widthOfRaster, Point drawPos, unsigned int color, unsigned int zValue) {
	
	//Makes a copy of the orignal color value 
	unsigned int copyColor = color;
	//Checks to see if there is an active pixel shader
	if (ActivePixelShader != nullptr)
		//applies pixel shader to colorCopy
		ActivePixelShader(copyColor);
	//Checks to see if the current draw position is within the range of the Raster to avoid out of range exception 
	if ((((drawPos.posX <= RASTER_WIDTH_INT) && (drawPos.posY <= RASTER_HEIGHT_INT)) && ((drawPos.posX >= 0) && (drawPos.posY >= 0)))) {
		//converts the 2d coordinates into 1d index for the raster array
		unsigned int rasterIndex = TwoDToOneDConverstion(widthOfRaster, drawPos);
		//Gets the value for color to check if alpha blending is necessary to save on proccessing 
		unsigned int alphaValue = ((copyColor & ALPHA) >> 24);
		//Checks to see if alpha blending is necessary 
		if (alphaValue < MAX_COLOR_VALUE) {
			//Alpha blending was necessary and is done to the current color before drawing it to the Raster, 
			//then we break out of the fuction using the return
			rasterArray[rasterIndex] = AlphaBlend(rasterArray[rasterIndex], copyColor);
			return;
		}

		//Alpha blending was not necessary and the current color is drawn to the Raster
		rasterArray[rasterIndex] = copyColor;
		//Assigns the value from zValue to the zBuffer array at the same location as the color buffer
		zBuffer[rasterIndex] = zValue;
	}

}

/**Block Image Transfer** Takes in the following arguments:
	-canvus to which you will draw on
	-the source image for the tile
	-the width of that source image
	-The width and height of the tile you wish to draw
	-The width and height of the canvus you wish to draw on
	-The starting pixel for the tile you wish to draw
	-The starting pixel you wish to draw the tile on the canvus
*/
void BLIT(unsigned int* rasterArray, const unsigned int* sourceImage, unsigned int sourceImageWidth, Dimensions TileImageDi, Dimensions rasterDi, Point TileStartPixelPos, Point drawPos) {
	//Get the origanal x positions for both where we would like to draw and the tile.
	//This allows us to cyle back to the correct position when changing the y direction.
	unsigned int origanalDrawPosX = drawPos.posX;
	unsigned int origanalTileStartPosX = TileStartPixelPos.posX;

	//Setting temporary points for when Defined points are used and cannot be manipulated.
	Point tempDrawPos = drawPos;
	Point tempTilePos = TileStartPixelPos;

	//Starts the Block image Transfer by setting the counters for Height and Width to Zero.
	for(unsigned int i = 0; i <= TileImageDi.height; i++) {

		for(int j = 0; j <= TileImageDi.width; j++) {
			//Converts Pixel to the proper color channel before drawing 
			unsigned int currentTileColor = ConvertBGRAtoARGB(sourceImage[TwoDToOneDConverstion(sourceImageWidth, tempTilePos)], tempDrawPos);
		
			//Draws a Pixel at the current draw position to the raster (canvus) 
			DrawPixel(rasterArray, rasterDi.width, tempDrawPos, currentTileColor);

			//increments the draw and tile position by one unit to draw the next pixel from source image, 
			//this is done for the entirety of the width of the tile
			tempDrawPos.posX++;
			tempTilePos.posX++;
		}
		//Increments the draw and tile position by one unit to draw the next row of pixels from source image,
		//this is once the the entirety of the width of tiles is done 
		tempDrawPos.posY++;
		tempTilePos.posY++;

		//Sets the x positions for the tile and draw positions to where they began in this cycle 
		//this ensures we are starting at the beginning of the next row.
		tempTilePos.posX = origanalTileStartPosX;
		tempDrawPos.posX = origanalDrawPosX;
	}
}

//Clears raster of the current display and fills with choosen color 
void ClearRaster(unsigned int* rasterArray, unsigned int TOTAL_NUM_RASTER_PIXELS, unsigned int color) {

	//Cycles through raster array and sets all pixels to defined color
	for (unsigned int i = 0; i <= TOTAL_NUM_RASTER_PIXELS; i++) {
		rasterArray[i] = color;
	}
}

//Clears zBuffer of current values and fills them with the furtherst value
void ClearZBuffer(float* zBuffer, unsigned int TOTAL_NUM_RASTER_PIXELS) {

	//Cycles through raster array and sets all pixels to defined color
	for (unsigned int i = 0; i <= TOTAL_NUM_RASTER_PIXELS; i++) {
		zBuffer[i] = MAX_COLOR_VALUE;
	}
}

//Draws a line to the Raster from left to Right Using Parametric line algorithim 
void ParametricLineMovingLeftToRight(unsigned int* rasterArray, int deltaX, int deltaY, Point startOfLine, Point endOfLine, unsigned int color) {
	//Sets Copys for the startOfLine Point and the endOfLine Point's X & Y
	int orignalStartPosX = startOfLine.posX;
	int orignalStartPosY = startOfLine.posY;
	int orignalEndPosX = endOfLine.posX;
	int orignalEndPosY = endOfLine.posY;
	
	//Sets the current X and Y to the starting points X & Y
	int currentX = startOfLine.posX;
	int currentY = startOfLine.posY;

	//Checks to see if the change in Y is Less than or equal to the change in X
	if (deltaY <= deltaX) {
		//if true we begin to cycle trough the line from the starting X pos to the End X Pos
		for (int i = orignalStartPosX; i <= orignalEndPosX; i++) {

			//Intializes line Drawn with how much of the line has already been drawn 
			int lineDrawn = currentX - orignalStartPosX;

			//Calulates a Ratio for how much the line has been drawn over the overall length (change in X AKA deltaX) 
			float currentRatio = (float)lineDrawn / deltaX;

			//Using the current Ratio we interpolate to figure out around where the current Y should be on this line
			currentY = LinearInterpolation(currentRatio, orignalStartPosY, orignalEndPosY);

			//Sets the Point to the current X and the Y positon rounded to the nearest whole number
			Point currentPoint = { currentX, floor(currentY + 0.5f) };
			//Draws the pixel at the current point
			DrawPixel(rasterArray, RASTER_WIDTH_INT, currentPoint, color);
			//Itterates to the next X positon 
			currentX++;
		}
		//Breaks out of function after line has been drawn
		return;
	}

	//The change in Y was greater than the change in X 
	//so we now cycle through the line the starting Y pos to the End Y Pos
	for (int i = orignalStartPosY; i <= orignalEndPosY; i++) {

		//Intializes line Drawn with how much of the line has already been drawn 
		int lineDrawn = currentY - orignalStartPosY;

		//Calulates a Ratio for how much the line has been drawn over the overall length (change in Y AKA deltaY) 
		float currentRatio = (float)lineDrawn / deltaY;

		//Using the current Ratio we interpolate to figure out around where the current X should be on this line
		currentX = LinearInterpolation(currentRatio, orignalStartPosX, orignalEndPosX);

		//Sets the Point to the current Y and the X positon rounded to the nearest whole number
		Point currentPoint = { floor(currentX + 0.5f), currentY };
		//Draws the pixel at the current point
		DrawPixel(rasterArray, RASTER_WIDTH_INT, currentPoint, color);
		//Itterates to the next Y positon 
		currentY++;
	}
}
//Draws a line to the Raster from left to Right Using Parametric line algorithim 
void ParametricLineMovingLeftToRight(unsigned int* rasterArray, float* zBuffer, int deltaX, int deltaY, Vertex startOfLine, Vertex endOfLine, unsigned int color) {
	//Sets Copys for the startOfLine Point and the endOfLine Point's X & Y
	int orignalStartPosX = startOfLine.posXposYposZ_W[0];
	int orignalStartPosY = startOfLine.posXposYposZ_W[1];
	int orignalEndPosX = endOfLine.posXposYposZ_W[0];
	int orignalEndPosY = endOfLine.posXposYposZ_W[1];

	//Sets the current X and Y to the starting points X & Y
	int currentX = startOfLine.posXposYposZ_W[0];
	int currentY = startOfLine.posXposYposZ_W[1];

	//Checks to see if the change in Y is Less than or equal to the change in X
	if (deltaY <= deltaX) {
		//if true we begin to cycle trough the line from the starting X pos to the End X Pos
		for (int i = orignalStartPosX; i <= orignalEndPosX; i++) {

			//Intializes line Drawn with how much of the line has already been drawn 
			int lineDrawn = currentX - orignalStartPosX;

			//Calulates a Ratio for how much the line has been drawn over the overall length (change in X AKA deltaX) 
			float currentRatio = (float)lineDrawn / deltaX;

			//Using the current Ratio we interpolate to figure out around where the current Y should be on this line
			currentY = LinearInterpolation(currentRatio, orignalStartPosY, orignalEndPosY);
			//Using the current Ratio we interpolate the values to figure out around where the Z value is for this Pixel
			float currentZValue = LinearInterpolation(currentRatio, startOfLine.posXposYposZ_W[2], endOfLine.posXposYposZ_W[2]);
			//Sets the Point to the current X and the Y positon rounded to the nearest whole number
			Point currentPoint = { currentX, floor(currentY + 0.5f) };
			//Draws the pixel at the current point
			DrawPixel(rasterArray, zBuffer, RASTER_WIDTH_INT, currentPoint, color, currentZValue);
			//Itterates to the next X positon 
			currentX++;
		}
		//Breaks out of function after line has been drawn
		return;
	}

	//The change in Y was greater than the change in X 
	//so we now cycle through the line the starting Y pos to the End Y Pos
	for (int i = orignalStartPosY; i <= orignalEndPosY; i++) {

		//Intializes line Drawn with how much of the line has already been drawn 
		int lineDrawn = currentY - orignalStartPosY;

		//Calulates a Ratio for how much the line has been drawn over the overall length (change in Y AKA deltaY) 
		float currentRatio = (float)lineDrawn / deltaY;

		//Using the current Ratio we interpolate to figure out around where the current X should be on this line
		currentX = LinearInterpolation(currentRatio, orignalStartPosX, orignalEndPosX);
		//Using the current Ratio we interpolate the values to figure out around where the Z value is for this Pixel
		float currentZValue = LinearInterpolation(currentRatio, startOfLine.posXposYposZ_W[2], endOfLine.posXposYposZ_W[2]);
		//Sets the Point to the current Y and the X positon rounded to the nearest whole number
		Point currentPoint = { floor(currentX + 0.5f), currentY };
		//Draws the pixel at the current point
		DrawPixel(rasterArray, zBuffer, RASTER_WIDTH_INT, currentPoint, color, currentZValue);
		//Itterates to the next Y positon 
		currentY++;
	}
}

//Draws a line to the Raster from left to Right Using Parametric line algorithim 
void ParametricLineMovingRightToLeft(unsigned int* rasterArray, int deltaX, int deltaY, Point startOfLine, Point endOfLine, unsigned int color) {
	//Sets Copys for the startOfLine Point and the endOfLine Point's X & Y
	//Being that the line is moving Right to Left we assign the start positions as the endOfLine Positions 
	//and the end positions as the startOfLine Positions 
	//This is to avoid running into zeros within divions and negitive numbers being set as positions
	int orignalStartPosX = endOfLine.posX;
	int orignalStartPosY = endOfLine.posY;
	int orignalEndPosX = startOfLine.posX;
	int orignalEndPosY = startOfLine.posY;

	//Sets the current X and Y to the starting points X & Y
	int currentX = startOfLine.posX;
	int currentY = startOfLine.posY;

	//Checks to see if the change in Y is Less than or equal to the change in X
	if (deltaY <= deltaX) {
		//if true we begin to cycle trough the line from the starting X pos to the End X Pos
		for (int i = orignalStartPosX; i <= orignalEndPosX; i++) {

			//Intializes line Drawn with how much of the line has already been drawn 
			int lineDrawn = currentX - orignalStartPosX;

			//Calulates a Ratio for how much the line has been drawn over the overall length (change in X AKA deltaX) 
			float currentRatio = (float)lineDrawn / deltaX;

			//Using the current Ratio we interpolate to figure out around where the current Y should be on this line
			currentY = LinearInterpolation(currentRatio, orignalStartPosY, orignalEndPosY);

			//Sets the Point to the current X and the Y positon rounded to the nearest whole number
			Point currentPoint = { currentX, floor(currentY + 0.5f) };
			//Draws the pixel at the current point
			DrawPixel(rasterArray, RASTER_WIDTH_INT, currentPoint, color);
			//Itterates to the next X positon 
			//being that we started with our end position we are itterating backwards through the line
			currentX--;
		}
		//Breaks out of function after line has been drawn
		return;
	}

	//The change in Y was greater than the change in X 
	//so we now cycle through the line the starting Y pos to the End Y Pos
	for (int i = orignalStartPosY; i <= orignalEndPosY; i++) {

		//Intializes line Drawn with how much of the line has already been drawn 
		int lineDrawn = currentY - orignalStartPosY;

		//Calulates a Ratio for how much the line has been drawn over the overall length (change in Y AKA deltaY) 
		float currentRatio = (float)lineDrawn / deltaY;

		//Using the current Ratio we interpolate to figure out around where the current X should be on this line
		currentX = LinearInterpolation(currentRatio, orignalStartPosX, orignalEndPosX);

		//Sets the Point to the current Y and the X positon rounded to the nearest whole number
		Point currentPoint = { floor(currentX + 0.5f), currentY };
		//Draws the pixel at the current point
		DrawPixel(rasterArray, RASTER_WIDTH_INT, currentPoint, color);
		//Itterates to the next Y positon 
		//being that we started with our end position we are itterating backwards through the line
		currentY--;
	}
}
//Draws a line to the Raster from left to Right Using Parametric line algorithim 
void ParametricLineMovingRightToLeft(unsigned int* rasterArray, float* zBuffer, int deltaX, int deltaY, Vertex startOfLine, Vertex endOfLine, unsigned int color) {
	//Sets Copys for the startOfLine Point and the endOfLine Point's X & Y
	//Being that the line is moving Right to Left we assign the start positions as the endOfLine Positions 
	//and the end positions as the startOfLine Positions 
	//This is to avoid running into zeros within divions and negitive numbers being set as positions
	int orignalStartPosX = endOfLine.posXposYposZ_W[0];
	int orignalStartPosY = endOfLine.posXposYposZ_W[1];
	int orignalEndPosX = startOfLine.posXposYposZ_W[0];
	int orignalEndPosY = startOfLine.posXposYposZ_W[1];

	//Sets the current X and Y to the starting points X & Y
	int currentX = startOfLine.posXposYposZ_W[0];
	int currentY = startOfLine.posXposYposZ_W[1];

	//Checks to see if the change in Y is Less than or equal to the change in X
	if (deltaY <= deltaX) {
		//if true we begin to cycle trough the line from the starting X pos to the End X Pos
		for (int i = orignalStartPosX; i <= orignalEndPosX; i++) {

			//Intializes line Drawn with how much of the line has already been drawn 
			int lineDrawn = currentX - orignalStartPosX;

			//Calulates a Ratio for how much the line has been drawn over the overall length (change in X AKA deltaX) 
			float currentRatio = (float)lineDrawn / deltaX;

			//Using the current Ratio we interpolate to figure out around where the current Y should be on this line
			currentY = LinearInterpolation(currentRatio, orignalStartPosY, orignalEndPosY);
			//Using the current Ratio we interpolate the values to figure out around where the Z value is for this Pixel
			float currentZValue = LinearInterpolation(currentRatio, startOfLine.posXposYposZ_W[2], endOfLine.posXposYposZ_W[2]);
			//Sets the Point to the current X and the Y positon rounded to the nearest whole number
			Point currentPoint = { currentX, floor(currentY + 0.5f) };
			//Draws the pixel at the current point
			DrawPixel(rasterArray, zBuffer, RASTER_WIDTH_INT, currentPoint, color, currentZValue);
			//Itterates to the next X positon 
			//being that we started with our end position we are itterating backwards through the line
			currentX--;
		}
		//Breaks out of function after line has been drawn
		return;
	}

	//The change in Y was greater than the change in X 
	//so we now cycle through the line the starting Y pos to the End Y Pos
	for (int i = orignalStartPosY; i <= orignalEndPosY; i++) {

		//Intializes line Drawn with how much of the line has already been drawn 
		int lineDrawn = currentY - orignalStartPosY;

		//Calulates a Ratio for how much the line has been drawn over the overall length (change in Y AKA deltaY) 
		float currentRatio = (float)lineDrawn / deltaY;

		//Using the current Ratio we interpolate to figure out around where the current X should be on this line
		currentX = LinearInterpolation(currentRatio, orignalStartPosX, orignalEndPosX);
		//Using the current Ratio we interpolate the values to figure out around where the Z value is for this Pixel
		float currentZValue = LinearInterpolation(currentRatio, startOfLine.posXposYposZ_W[2], endOfLine.posXposYposZ_W[2]);
		//Sets the Point to the current Y and the X positon rounded to the nearest whole number
		Point currentPoint = { floor(currentX + 0.5f), currentY };
		//Draws the pixel at the current point
		DrawPixel(rasterArray, zBuffer, RASTER_WIDTH_INT, currentPoint, color, currentZValue);
		//Itterates to the next Y positon 
		//being that we started with our end position we are itterating backwards through the line
		currentY--;
	}
}

//Determines what type of Parametric line needs to be called by comparing the delta values of X & Y against zero
void DrawParametricLine(unsigned int* rasterArray, Point startOfLine, Point endOfLine, unsigned int color) {

	//Calculates the Change in X and Change in Y
	int deltaX= endOfLine.posX - startOfLine.posX;
	int deltaY = endOfLine.posY - startOfLine.posY;

	//Compares the delta values against zero to see if line is moving Right to Left or Left to Right
	if ((deltaX < 0 ) || ( deltaY < 0)){
		//delta values were less than 0 and corrected delta values are calculated to draw the appropriate line
		deltaX = startOfLine.posX - endOfLine.posX;
		deltaY = startOfLine.posY - endOfLine.posY;
		//Draws line using corrected delta values
		ParametricLineMovingRightToLeft(rasterArray, deltaX, deltaY, startOfLine, endOfLine, color);
		//exit function once line is drawn
		return;
	}
	//Delta values are valid meaning line is drawn Left to Right using those values
	ParametricLineMovingLeftToRight(rasterArray, deltaX, deltaY, startOfLine, endOfLine, color);
}
//Determines what type of Parametric line needs to be called by comparing the delta values of X & Y against zero
void DrawParametricLine(unsigned int* rasterArray, float* zBuffer, Vertex startOfLine, Vertex endOfLine, unsigned int color) {

	//Calculates the Change in X and Change in Y
	int deltaX = endOfLine.posXposYposZ_W[0] - startOfLine.posXposYposZ_W[0];
	int deltaY = endOfLine.posXposYposZ_W[1] - startOfLine.posXposYposZ_W[1];

	//Compares the delta values against zero to see if line is moving Right to Left or Left to Right
	if ((deltaX < 0) || (deltaY < 0)) {
		//delta values were less than 0 and corrected delta values are calculated to draw the appropriate line
		deltaX = startOfLine.posXposYposZ_W[0] - endOfLine.posXposYposZ_W[0];
		deltaY = startOfLine.posXposYposZ_W[1] - endOfLine.posXposYposZ_W[1];
	
		//Draws line using corrected delta values
		ParametricLineMovingRightToLeft(rasterArray, zBuffer, deltaX, deltaY, startOfLine, endOfLine, color);
		//exit function once line is drawn
		return;
	}
	//Delta values are valid meaning line is drawn Left to Right using those values
	ParametricLineMovingLeftToRight(rasterArray, zBuffer, deltaX, deltaY, startOfLine, endOfLine, color);
}

//Draws a 3D line to the Raster and stores the z posion in the zbuffer
void Draw3DParametricLine(unsigned int* rasterArray, float* zBuffer, Vertex &startOfLine, Vertex &endOfLine, unsigned int color) {

	//copy inputs so orignal data is not manipulated
	Vertex copyStartLine = startOfLine;
	Vertex copyEndLine = endOfLine;

	//checks to see if there is any valid Vertex shader
	if (ActiveVertexShader != nullptr) {
		
		ActiveVertexShader(copyStartLine);
		ActiveVertexShader(copyEndLine);
	}

	//Converts the X and Y Position of startOfLine and endOfLine Vertex to 2d Pixel Coordinates
	ThreeDtoTwoDPixelCoordinates(copyStartLine);
	ThreeDtoTwoDPixelCoordinates(copyEndLine);


	//Draws Line to Raster
	DrawParametricLine(rasterArray, zBuffer, copyStartLine, copyEndLine, color);
}

//Intializes the z buffer for the aplication
float* IntializeZBuffer(unsigned int totalRasterPixels) {

	float* zBuffer = new float[totalRasterPixels];
	return zBuffer;
}

//Draws a 3D grid at any defined dimensions grid (IN Define.h) with .1 increments between vertices to the Raster
//The starting position refers to the bottom left corner 
void DrawGrid(unsigned int* rasterArray, float* zBuffer, Vertex startPos) {
	
	Vertex startPosCopy = startPos;
	Vertex currentPos = startPos;

	float OffsetForEndPos = GRID_INCREMENT * GRID_DI_X_AND_Y;

	for (unsigned int i = 0; i <= GRID_DI_X_AND_Y; i++) {

		currentPos = startPosCopy;
		Vertex endPoint = currentPos;
		endPoint.posXposYposZ_W[2] = currentPos.posXposYposZ_W[2] + OffsetForEndPos;

		Draw3DParametricLine(rasterArray, zBuffer, currentPos, endPoint, currentPos.color);

		startPosCopy.posXposYposZ_W[0] = startPosCopy.posXposYposZ_W[0] + GRID_INCREMENT;

	}

	startPosCopy = startPos;
	currentPos = startPos;
	for (unsigned int i = 0; i <= GRID_DI_X_AND_Y; i++) {

		currentPos = startPosCopy;
		Vertex endPoint = currentPos;
		endPoint.posXposYposZ_W[0] = currentPos.posXposYposZ_W[0] + OffsetForEndPos;

		Draw3DParametricLine(rasterArray, zBuffer, currentPos, endPoint, currentPos.color);

		startPosCopy.posXposYposZ_W[2] = startPosCopy.posXposYposZ_W[2] + GRID_INCREMENT;

	}

}
//Draws a 3D cube With defined dimentions (IN Define.h)
//Starts at the top right vertex of the first face
void DrawCube(unsigned int* rasterArray, float* zBuffer) {

	Vertex currentFaceValues[4];

	for(unsigned int i = 0; i < NUM_CUBE_FACES; i++)
	{
		switch (i)
		{
		case 0:
			/******** FACE 1 *********/

			//UPPER LEFT
			currentFaceValues[0] = (Vertex{ {-HALF_CUBE_WIDTH,  HALF_CUBE_WIDTH, -HALF_CUBE_WIDTH, 0}, WHITE });
			//UPPER RIGHT
			currentFaceValues[1] = (Vertex{ { HALF_CUBE_WIDTH,  HALF_CUBE_WIDTH, -HALF_CUBE_WIDTH, 0}, WHITE });
			//LOWER LEFT
			currentFaceValues[2] = (Vertex{ {-HALF_CUBE_WIDTH, -HALF_CUBE_WIDTH, -HALF_CUBE_WIDTH, 0}, WHITE });
			//LOWER RIGHT
			currentFaceValues[3] = (Vertex{ { HALF_CUBE_WIDTH, -HALF_CUBE_WIDTH, -HALF_CUBE_WIDTH, 0}, WHITE });

			break;
		case 1:
			/******** FACE 2 *********/

			//UPPER LEFT
			currentFaceValues[0] = (Vertex{ { HALF_CUBE_WIDTH,  HALF_CUBE_WIDTH, -HALF_CUBE_WIDTH, 0}, WHITE });
			//UPPER RIGHT
			currentFaceValues[1] = (Vertex{ { HALF_CUBE_WIDTH,  HALF_CUBE_WIDTH,  HALF_CUBE_WIDTH, 0}, WHITE });
			//LOWER LEFT
			currentFaceValues[2] = (Vertex{ { HALF_CUBE_WIDTH, -HALF_CUBE_WIDTH, -HALF_CUBE_WIDTH, 0}, WHITE });
			//LOWER RIGHT
			currentFaceValues[3] = (Vertex{ { HALF_CUBE_WIDTH, -HALF_CUBE_WIDTH,  HALF_CUBE_WIDTH, 0}, WHITE });

			break;

		case 2:
			/******** FACE 3 *********/

			//UPPER LEFT
			currentFaceValues[0] = (Vertex{ {  HALF_CUBE_WIDTH,  HALF_CUBE_WIDTH,  HALF_CUBE_WIDTH, 0}, WHITE });
			//UPPER RIGHT
			currentFaceValues[1] = (Vertex{ { -HALF_CUBE_WIDTH,  HALF_CUBE_WIDTH,  HALF_CUBE_WIDTH, 0}, WHITE });
			//LOWER LEFT
			currentFaceValues[2] = (Vertex{ {  HALF_CUBE_WIDTH, -HALF_CUBE_WIDTH,  HALF_CUBE_WIDTH, 0}, WHITE });
			//LOWER RIGHT
			currentFaceValues[3] = (Vertex{ { -HALF_CUBE_WIDTH, -HALF_CUBE_WIDTH,  HALF_CUBE_WIDTH, 0}, WHITE });

			break;

		case 3:
			/******** FACE 4 *********/

			//UPPER LEFT
			currentFaceValues[0] = (Vertex{ { -HALF_CUBE_WIDTH,  HALF_CUBE_WIDTH,   HALF_CUBE_WIDTH, 0}, WHITE });
			//UPPER RIGHT
			currentFaceValues[1] = (Vertex{ { -HALF_CUBE_WIDTH,  HALF_CUBE_WIDTH,  -HALF_CUBE_WIDTH, 0}, WHITE });
			//LOWER LEFT
			currentFaceValues[2] = (Vertex{ { -HALF_CUBE_WIDTH, -HALF_CUBE_WIDTH,   HALF_CUBE_WIDTH, 0}, WHITE });
			//LOWER RIGHT
			currentFaceValues[3] = (Vertex{ { -HALF_CUBE_WIDTH, -HALF_CUBE_WIDTH,  -HALF_CUBE_WIDTH, 0}, WHITE });

			break;

		case 4:
			/******** FACE 5 TOP OF CUBE *********/

			//UPPER LEFT
			currentFaceValues[0] = (Vertex{ { -HALF_CUBE_WIDTH,  HALF_CUBE_WIDTH,  HALF_CUBE_WIDTH, 0}, WHITE });
			//UPPER RIGHT
			currentFaceValues[1] = (Vertex{ {  HALF_CUBE_WIDTH,  HALF_CUBE_WIDTH,  HALF_CUBE_WIDTH, 0}, WHITE });
			//LOWER LEFT
			currentFaceValues[2] = (Vertex{ { -HALF_CUBE_WIDTH,  HALF_CUBE_WIDTH, -HALF_CUBE_WIDTH, 0}, WHITE });
			//LOWER RIGHT
			currentFaceValues[3] = (Vertex{ {  HALF_CUBE_WIDTH,  HALF_CUBE_WIDTH, -HALF_CUBE_WIDTH, 0}, WHITE });

			break;

		case 5:
			/******** FACE 5 Bottom OF CUBE *********/

			//UPPER LEFT
			currentFaceValues[0] = (Vertex{ { -HALF_CUBE_WIDTH, -HALF_CUBE_WIDTH, -HALF_CUBE_WIDTH, 0}, WHITE });
			//UPPER RIGHT
			currentFaceValues[1] = (Vertex{ {  HALF_CUBE_WIDTH, -HALF_CUBE_WIDTH, -HALF_CUBE_WIDTH, 0}, WHITE });
			//LOWER LEFT
			currentFaceValues[2] = (Vertex{ { -HALF_CUBE_WIDTH, -HALF_CUBE_WIDTH,  HALF_CUBE_WIDTH, 0}, WHITE });
			//LOWER RIGHT
			currentFaceValues[3] = (Vertex{ {  HALF_CUBE_WIDTH, -HALF_CUBE_WIDTH,  HALF_CUBE_WIDTH, 0}, WHITE });

			break;

		default:
			break;
		}
		

		//DRAWS CURRENT FACE

		//Draws Top Line
		Draw3DParametricLine(rasterArray, zBuffer, currentFaceValues[0], currentFaceValues[1], currentFaceValues[0].color);
		//Draws Left Line
		Draw3DParametricLine(rasterArray, zBuffer, currentFaceValues[0], currentFaceValues[2], currentFaceValues[0].color);
		//Draws Right Line
		Draw3DParametricLine(rasterArray, zBuffer, currentFaceValues[1], currentFaceValues[3], currentFaceValues[1].color);
		//Draws Bottom Line
		Draw3DParametricLine(rasterArray, zBuffer, currentFaceValues[2], currentFaceValues[3], currentFaceValues[2].color);
	}


	

}