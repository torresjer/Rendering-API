#include <cmath>
/*		********* Structures *********		*/

//Structure for a 2D Point
struct Point
{
	int posX; 
	int posY;
};

//Structure for the Dimentions of a tile
struct Dimensions
{
	unsigned int width;
	unsigned int height;
};

struct Vertex
{
	float posXposYposZ_W[4];
	unsigned int color;
};

struct Matrix
{
	float matrixElements[4][4];
};

/*		********* Definitions Pertaining to the Raster *********		*/

//Raster Definitions 
#define RASTER_WIDTH_INT					500
#define RASTER_HEIGHT_INT					500
#define RASTER_WIDTH_FLOAT					500.0f
#define RASTER_HEIGHT_FLOAT					500.0f

#define RASTER_DI							(Dimensions {RASTER_WIDTH_INT, RASTER_HEIGHT_INT})

//Postions on Raster
#define	RASTER_CENTER						(Point {RASTER_WIDTH_INT/2, RASTER_HEIGHT_INT/2})
#define RASTER_ORIGIN						(Point {0,0})


//Color Definitions
#define RED									0xFFFF0000
#define GREEN								0xFF00FF00
#define BLUE								0xFF0000FF
#define WHITE								0xFFFFFFFF
#define BLACK								0xFF000000
#define ALPHA								0xFF000000
#define MAX_COLOR_VALUE						255.0f
#define MAX_COLOR_VALUE_NO_ZERO				256

/*		********* Definitions for Animation *********		*/

//FrameRates
#define TWENTY_SEVEN_FPS					(1.0F / 27.0F)
#define THIRTY_FPS							(1.0F / 30.0F)
#define SIXTY_FPS							(1.0F / 60.0F)
#define ONEHUN_TWENTY_FPS					(1.0F / 120.0F)
#define TWENTY_SEVEN_FRAMES					27.0F
#define THIRTY_FRAMES						30.0F
#define SIXTY_FRAMES						60.0F
#define ONEHUN_TWENTY_FRAMES				120.0F

/*		********* Definitions for Testing *********		*/

#define TEST_POINT_A						(Point {200, 120})
#define TEST_POINT_B						(Point {100,120})

#define TEST_VERTEX_A						(Vertex {{-.5f, -.5f, 0.0f, 0.0f}, WHITE})
#define TEST_VERTEX_B						(Vertex {{-0.5f, -0.5f, 0.0f, 0.0f}, WHITE})

#define TEST_MATRIX_A						(Matrix {{{1, 0, 0, 0},{0, 0.309f, 0.951f, 0},{0, -0.951, .309, 0}, {7, -2.282, 9.153, 1}}})
#define TEST_MATRIX_B						(Matrix {{{17, 18, 19, 20},{21, 22, 23, 24},{25, 26, 27, 28}, {29, 30, 31, 32}}})

/*		********* Definitions Pertaining to the Math *********		*/
#define PI									3.14159265359f
#define STRAIGHT_ANGLE						180
/*		********* Definitions For CGSAssignment2 *********		*/

//Definitions for the Perspective Projection Matrix
#define NEAR_PLANE							0.1f
#define FAR_PLANE							10.0f
#define FOV_IN_DEGREES						90
#define HALF_FOV_IN_DEGREES					FOV_IN_DEGREES / 2
#define ASPECT_RATIO						(RASTER_WIDTH_FLOAT / RASTER_HEIGHT_FLOAT)

#define START_OF_GRID_VERTEX				(Vertex {{-0.5f, 0.0f, -0.5f, 0.0f}, WHITE})
#define GRID_DI_X_AND_Y						10
#define GRID_INCREMENT						.1f

#define CAMERA_ORIGNAL_X_ROTATION			18.0F
#define CAMERA_OFFSET						(Vertex {{0.0f, 0.0f, 1.0f, 0.0f}, WHITE})

#define CUBE_WIDTH							.5f
#define HALF_CUBE_WIDTH						CUBE_WIDTH / 2
#define CUBE_OFFSET_TO_GRID					(Vertex {{0.0f, -HALF_CUBE_WIDTH, 0.0f, 0.0f}, WHITE})
#define NUM_CUBE_FACES						6
#define TOTAL_CUBE_ROTATION_TIME_IN_SEC		15
#define TOTAL_DEGREES_TO_ROTATE_IN_TIME_SEC	360
/*		********* Definitions For Line Drawing Assignment *********		*/

#define NUM_SIDES_OCTAGON					8
#define LENGTH_OF_SIDES_IN_PIXELS			100
#define RATIO_FOR_EVEN_LINES				.3f
#define OPPOSITE_POINTS_INDEX_INCREMENT		(NUM_SIDES_OCTAGON / 2)
/*Allows Octagon to strobe with random colors,
	if false draws lines with one set of random colors.*/
#define OCTAGON_STROBE						true

/*		********* Definitions For CGSAssignment 1 *********		*/

//Grass Block Definitions 
#define GRASS_BLOCK_ORG_POSX				282
#define GRASS_BLOCK_ORG_POSY				121
#define GRASS_BLOCK_WIDTH					41
#define GRASS_BLOCK_HEIGHT					44
#define GRASS_BLOCK_DI						(Dimensions {GRASS_BLOCK_WIDTH, GRASS_BLOCK_HEIGHT})
#define GRASS_BLOCK_STARTING_POS			(Point {GRASS_BLOCK_ORG_POSX, GRASS_BLOCK_ORG_POSY})

//House Block Definitions
#define HOUSE_BLOCK_ORG_POSX				128
#define HOUSE_BLOCK_ORG_POSY				32
#define HOUSE_BLOCK_WIDTH					64
#define HOUSE_BLOCK_HEIGHT					67
#define HOUSE_BLOCK_DI						(Dimensions {HOUSE_BLOCK_WIDTH, HOUSE_BLOCK_HEIGHT})
#define HOUSE_BLOCK_STARTING_POS			(Point {HOUSE_BLOCK_ORG_POSX, HOUSE_BLOCK_ORG_POSY})

//Fire Tile Definitions
#define FIRE_ANIMATION_WIDTH				128
#define	FIRE_ANIMATION_HEIGHT				128
#define FIRE_ANIMATION_DI					(Dimensions {FIRE_ANIMATION_WIDTH, FIRE_ANIMATION_HEIGHT})
#define FIRE_ANIMATION_STARTING_POS			(Point {0,0})
