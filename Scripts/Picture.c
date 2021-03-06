#include <stdlib.h>
#include "../Headers/Header.h"

// Sort started
bool threadStarted = false;

static pthread_t threadID;

static Texture2D picture = {0};

// Array of positions of picture tiles
PictureTile tilePositions[horizontalTiles * verticalTiles];

// Function prototype
static void SetTilePositions(void);

void GetImage(void)
{
    // If a picture is already open
    if (picture.height > 0)
        // Close it
        UnloadTexture(picture);

    char imgPath[512];
    // If external image path is not empty
    if (strlen(imagePath) > 0)
        // Copy external image path to local variable
        strcpy(imgPath, imagePath);
    else
        // Get a random image from the resources folder 
        sprintf(imgPath, "resources/%d.png", GetRandomValue(1, 10));

    // Load image
    Image img = LoadImage(imgPath);
    // Resize image
    ImageResize(&img, screenWidth, screenHeight);
    // Load texture from resized image
    picture = LoadTextureFromImage(img);
    // Free image from memory cause it's not used anymore
    UnloadImage(img);

    SetTilePositions();
}

static void SetTilePositions(void)
{
    // Go through height and width of image
    for (int y = 0; y < verticalTiles; y++)
    {
        for (int x = 0; x < horizontalTiles; x++)
        {
            // Get 1D index
            int index = (y * horizontalTiles) + x;
            // Setup array 
            tilePositions[index] = (PictureTile) { .picturePos = (Vector2) {x * tileSize, y * tileSize}, // Position in image
                                                    .actualPos = (Vector2) {x * tileSize, y * tileSize}, // Actual position on the screen
                                                    .index = index }; // Save index for sorting purposes
        }
    }
    
    // Shuffle array so it can be sorted
    ShufflePositions();
}

void CallSort(int algoIndex)
{
    // Disallow from starting a new thread when one has started
    if (!threadStarted)
    {
        threadStarted = true;

        StartTimer();

        // Pointer to algorithm function (default to bubble sort)
        void (*SortAlgo) (void*) = BubbleSort;

        // Choose algroithm based on index
        switch (algoIndex)
        {
            case COMBSORT:
                SortAlgo = CombSort;
                break;

            case SELECTIONSORT:
                SortAlgo = SelectionSort;
                break;

            case DSELECTIONSORT:
                SortAlgo = DoubleSelectionSort;
                break;

            case INSERTIONSORT:
                SortAlgo = InsertionSort;
                break;
                
            case MERGESORT:
                SortAlgo = MergeSort;
                break;

            case QUICKSORT:
                SortAlgo = QuickSort;
                break;
        }
        
        int *args = NULL;
        // Pass parameters for Merge and Quick Sort only
        if (algoIndex == MERGESORT || algoIndex == QUICKSORT)
        {
            // Allocate memory for two integers and initialize them to zero
            args = calloc(2, sizeof(int));
            // Set the value of the second item to end of tilePosition array
            args[1] = tilesNo - 1;
        }
        // Start thread with chosen function
        pthread_create(&threadID, NULL, (void *) *SortAlgo, (void *) args);
    }
}

void DrawImageTiles(void)
{
    // Go through height and width of image
    for (int y = 0; y < verticalTiles; y++)
    {
        for (int x = 0; x < horizontalTiles; x++)
        {
            // Get 1D index
            int index = (y * horizontalTiles) + x;
            // Width, height and position of current tile (position gotten from tilePositions array)
            Rectangle tileRec = (Rectangle) {.width = tileSize, .height = tileSize, .x = tilePositions[index].picturePos.x, .y = tilePositions[index].picturePos.y};
            // Position on the screen
            Vector2 tilePos = (Vector2) {x * tileSize, y * tileSize};
            // Draw individual image tile
            DrawTextureRec(picture, tileRec, tilePos, WHITE);
            // Draw index number
            DrawText(TextFormat("%i", tilePositions[index].index + 1), tilePos.x + (tileSize / 2), tilePos.y + (tileSize / 2), 15, RAYWHITE);
            
            // If shuffled
            if (shuffled)
            {
                // Stop shuffling when last tile reached
                if (y == verticalTiles - 1 && x == horizontalTiles - 1)
                    shuffled = false;
                
                // Set the new actual position on the screen
                tilePositions[index].actualPos = (Vector2) {tilePos.x, tilePos.y};
            }
        }
    }
}

void FreeImage(void)
{
    // Free image from memory
    UnloadTexture(picture);
}
