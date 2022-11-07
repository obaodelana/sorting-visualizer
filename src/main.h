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

extern bool threadStarted, updatedTilePos;
extern int sortDelay;
