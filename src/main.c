#include "raylib.h"
#include "main.h"
#include "sort.h"
#include "picture.h"

// Import raygui with icons
#define RAYGUI_IMPLEMENTATION
#define RAYGUI_SUPPORT_ICONS
#include "libs/raygui.h"

// Holds dropdown items
static const char dropDownText[] = "Bubble Sort;Comb Sort;Selection Sort;Double Selection Sort;Insertion Sort;Merge Sort;Quick Sort";

// In milliseconds
int sortDelay = 175;

// Current dropdown item
static int activeSort = 0;
static bool dropDownOpen = false;

// GUI positions
static Rectangle sortDelaySliderPos = (Rectangle) { screenWidth / 2 - (175 / 2),  screenHeight - 30, 175, 20};
static Rectangle dropDownPos = (Rectangle) {screenWidth - (160.0f + (160.0f / 2.5f)), 30, 160, 20};
static Rectangle randomImageButtonPos = (Rectangle) {30, 30, 30, 30};
static Rectangle shuffleButtonPos = (Rectangle) {30, screenHeight - 40, 30, 30};

void UpdateDrawFrame(void);

int main()
{
    InitWindow(screenWidth, screenHeight, "Picture Sorting Visualizer");
    // Load custom gui style
    GuiLoadStyle("resources/gui/jungle.rgs");

    // Load image
    GetImage();

    // Web build
    #if defined(PLATFORM_WEB)
        emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
    #else // Normal build
        SetTargetFPS(60);
        while (!WindowShouldClose())
            UpdateDrawFrame();
    #endif

    // Unload image from memory
    FreeImage();
    
    CloseWindow();

    return 0;
}

void UpdateDrawFrame(void)
{
    BeginDrawing();

        ClearBackground(BLACK);
        DrawImageTiles();
        DrawBoxes();

        // GUI

            // Sort delay slider
            sortDelay = GuiSliderBar(sortDelaySliderPos, NULL, TextFormat("%ims", sortDelay), sortDelay, 0, maxWaitTime);
			
			// Random image button (don't show while sorting)
			if (!threadStarted && GuiButton(randomImageButtonPos, GuiIconText(ICON_FILETYPE_IMAGE, "")))
				GetImage();

            // Shuffle button (don't show while sorting)
            if (!threadStarted && GuiButton(shuffleButtonPos, GuiIconText(ICON_SHUFFLE, "")))
                ShufflePositions();

            // Sort options drop down 
            if(GuiDropdownBox(dropDownPos, dropDownText, &activeSort, dropDownOpen))
            {
                // Disallow interaction when thread started
                if (!threadStarted)
                {
                    // If the drop down is open, call sort function
                    if (dropDownOpen)
                        CallSort(activeSort);
                    // Close / open drop down
                    dropDownOpen = !dropDownOpen;
                }
            }

            DrawTimer();

        // -----------------------------

    EndDrawing();
}
