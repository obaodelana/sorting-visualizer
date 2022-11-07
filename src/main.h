// For web
#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

#define screenWidth 1200
#define screenHeight 600

#define tileSize 150

#define horizontalTiles screenWidth / tileSize
#define verticalTiles screenHeight / tileSize

#define tilesNo horizontalTiles * verticalTiles

// In milliseconds
#define maxWaitTime 500

// Sort indices
#define BUBBLESORT 0
#define COMBSORT 1
#define SELECTIONSORT 2
#define DSELECTIONSORT 3
#define INSERTIONSORT 4
#define MERGESORT 5
#define QUICKSORT 6

// Thread started - sorting started
extern bool threadStarted, updatedTilePos;
extern int sortDelay;
