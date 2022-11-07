#include "raylib.h"
#include "main.h"

// Sort indices
#define BUBBLESORT 0
#define COMBSORT 1
#define SELECTIONSORT 2
#define DSELECTIONSORT 3
#define INSERTIONSORT 4
#define MERGESORT 5
#define QUICKSORT 6

// Stores individual tile data
typedef struct PictureTile
{
    Vector2 picturePos, actualPos;
    int index;
} PictureTile;

// List of tiles
extern PictureTile tilePositions[tilesNo];
