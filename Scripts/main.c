#include "../Headers/Header.h"

// Import raygui with icons
#define RAYGUI_IMPLEMENTATION
#define RAYGUI_SUPPORT_ICONS
#include "../Headers/raygui.h"

// Avoid including raygui implementation again
#undef RAYGUI_IMPLEMENTATION
// Import file dialog
#define GUI_FILE_DIALOG_IMPLEMENTATION
#include "../Headers/gui_file_dialog.h"

// Holds dropdown items
static const char dropDownText[] = "Bubble Sort;Comb Sort;Selection Sort;Double Selection Sort;Insertion Sort;Merge Sort;Quick Sort";

char imagePath[512] = "";
// In milliseconds
int sortDelay = 175;

// Current dropdown item
static int activeSort = 0;
static bool dropDownOpen = false;

// GUI positions
static Rectangle sortDelaySliderPos = (Rectangle) { screenWidth / 2 - (175 / 2),  screenHeight - 30, 175, 20};
static Rectangle dropDownPos = (Rectangle) {screenWidth - (160.0f + (160.0f / 2.5f)), 30, 160, 20};
static Rectangle imageButtonPos = (Rectangle) {65, 30, 100, 30};
static Rectangle randomImageButtonPos = (Rectangle) {165 + 15, 30, 30, 30};
static Rectangle shuffleButtonPos = (Rectangle) {65, screenHeight - 40, 30, 30};

// File dialog
static GuiFileDialogState imageDialog = {0};

// Function protoype
static void GetImagePath(void);
void UpdateDrawFrame(void); // For web

int main()
{
    InitWindow(screenWidth, screenHeight, "Picture Sorting Visualizer");
    // Load custom gui style
    GuiLoadStyle("resources/gui/jungle.rgs");

    // Load image
    GetImage();

    // Setup file dialog
    imageDialog = InitGuiFileDialog(screenWidth / 2, screenHeight / 2, GetWorkingDirectory(), false);

    // Web build
    #if defined(PLATFORM_WEB)
        emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
    #else
        // Normal build
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
    // Get chosen image from file dialog
    GetImagePath();

    BeginDrawing();

        ClearBackground(BLACK);
        DrawImageTiles();
        DrawBoxes();

        // GUI

            // Lock GUI when file dialog is open
            if (imageDialog.fileDialogActive)
                GuiLock();

            // Sort delay slider
            sortDelay = GuiSliderBar(sortDelaySliderPos, NULL, TextFormat("%ims", sortDelay), sortDelay, 0, maxWaitTime);
            // Don't show image options if on web
            #if !defined(PLATFORM_WEB)
                // File dialog button (don't show while sorting)
                if (!threadStarted && GuiButton(imageButtonPos, GuiIconText(RICON_FILE_OPEN, "Open Image")))
                    // Open file dialog
                    imageDialog.fileDialogActive = true;
                // Random image button (don't show while sorting)
                if (!threadStarted && GuiButton(randomImageButtonPos, GuiIconText(RICON_FILETYPE_IMAGE, "")))
                {
                    // Empty external image path
                    memset(imagePath, 0, sizeof imagePath);
                    GetImage();
                }
            #endif
            // Shuffle button (don't show while sorting)
            if (!threadStarted && GuiButton(shuffleButtonPos, GuiIconText(RICON_SHUFFLE, "")))
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

            GuiUnlock();

            // Show file dialog
            GuiFileDialog(&imageDialog);

            DrawTimer();

        // -----------------------------

    EndDrawing();
}

static void GetImagePath(void)
{
    // If a file is selected in the file dialog
    if (imageDialog.SelectFilePressed)
    {
        // If it has a .png/.jpg/.jpeg extension
        if (IsFileExtension(imageDialog.fileNameText, ".png") || IsFileExtension(imageDialog.fileNameText, ".jpg") || IsFileExtension(imageDialog.fileNameText, ".jpeg"))
        {
            // Get chosen image path
            const char *newImagePath = TextFormat("%s/%s", imageDialog.dirPathText, imageDialog.fileNameText);
            // If image path is less capacity of storage variable
            if (strlen(newImagePath) < 512)
            {
                // Copy the path to variable
                strcpy(imagePath, newImagePath);
                // Get new image
                GetImage();
            }
        }

        // Unselect file
        imageDialog.SelectFilePressed = false;
    }
}
