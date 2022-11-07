#include <string.h>
#include <stdio.h>
#include "raylib.h"

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


// Thread started - sorting started
extern bool threadStarted, updatedTilePos;
extern int sortDelay;

// Picture module
void GetImage(void);
void CallSort(int algoIndex);
void DrawImageTiles(void);
void FreeImage(void);

// Sort module
void ShufflePositions(void);
void StartTimer(void);
void DrawBoxes(void);
void DrawTimer(void);
// Sort algorithms
void BubbleSort(void* arg);
void CombSort(void *arg);
void SelectionSort(void* arg);
void DoubleSelectionSort(void *arg);
void InsertionSort(void *arg);
void MergeSort(void *arg);
void QuickSort(void *arg);
// ----------------------------------