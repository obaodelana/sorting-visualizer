#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "picture.h"
#include "sort.h"

static Texture2D picture = {0};

// Array of positions of picture tiles
PictureTile tilePositions[horizontalTiles * verticalTiles];

static void SetTilePositions(void);

void GetImage(void)
{
    // If a picture is already open, close it
    if (picture.height > 0) UnloadTexture(picture);

    char imgPath[512 + 1];
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

// Setup [tilePositions]
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

// Draw image tiles at correct x and y positions
void DrawImageTiles(void)
{
    for (int y = 0; y < verticalTiles; y++)
    {
        for (int x = 0; x < horizontalTiles; x++)
        {
            // Get 1D index
            int index = (y * horizontalTiles) + x;
            // Width, height and position of current image in tile sheet
            Rectangle tileRec = (Rectangle) {.width = tileSize, .height = tileSize, .x = tilePositions[index].picturePos.x, .y = tilePositions[index].picturePos.y};
            // Position on the screen
            Vector2 tilePos = (Vector2) {x * tileSize, y * tileSize};
            // Draw individual image tile
            DrawTextureRec(picture, tileRec, tilePos, WHITE);
            // Draw index number on top of image tile
            DrawText(TextFormat("%i", tilePositions[index].index + 1), tilePos.x + (tileSize / 2), tilePos.y + (tileSize / 2), 15, RAYWHITE);
            
            // Shuffling changes the [actualPos] of each tile
            if (updatedTilePos)
            {
                // Reset [justShuffled] when the last tile is reached
                if (y == verticalTiles - 1 && x == horizontalTiles - 1)
                    updatedTilePos = false;
                
                // Set the new position on the screen of each tile
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
