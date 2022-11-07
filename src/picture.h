#include "raylib.h"
#include "main.h"

// Stores individual tile data
typedef struct PictureTile
{
	// Stores the position of the tile in the sprite sheet (whole image)
    Vector2 picturePos;
	// Stores the position of the tile on the screen
	Vector2 actualPos;
	// Index of tile (for sorting purposes)
    int index;
} PictureTile;

// List of tiles
extern PictureTile tilePositions[tilesNo];

void GetImage(void);
void DrawImageTiles(void);
void FreeImage(void);
