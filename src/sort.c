#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include "raylib.h"
#include "sort.h"
#include "main.h"
#include "picture.h"

bool updatedTilePos = false;

// Timer stuff
static bool timerStarted = false;
static float timer = 0.0f;
static clock_t startTime = 0;

// Array for drawing boxes
static Vector2 boxes[tilesNo * 2];
static Color boxColors[tilesNo * 2];

static void SwapItems(PictureTile *one, PictureTile *two)
{
    // Store value of one
    PictureTile oneTemp = *one;
    // Change value of one to two
    *one = *two;
    // Change value of two to temp (stores one)
    *two = oneTemp;

    // Swap the actual position
    // Save actual position of one
    Vector2 oneTempPos = one->actualPos;
    // Change actual pos of one to two's actual pos
    one->actualPos = two->actualPos;
    // Change actual pos of two to one's actual pos
    two->actualPos = oneTempPos;
}

static PictureTile *CreateSubArray(int start, int end)
{
    // Allocate enough memory for a new array
    PictureTile *subArray = malloc(sizeof(PictureTile) * ((end - start) + 1));
    // Copy the needed items to sub array
    for (int i = start, x = 0; i <= end; i++, x++)
        subArray[x] = tilePositions[i];

    return subArray;
}

static void Delay(int milliseconds)
{
    // Time to wait (in seconds)
    float waitTime = milliseconds / 1000.0f;
    // Starting time
    float startTime = (float) clock() / CLOCKS_PER_SEC;

    // Wait until the current time is greater than start time + time to wait
    while (((float) clock() / CLOCKS_PER_SEC) < startTime + waitTime);
}

void ShufflePositions(void)
{
    // Go through list
    for (int i = 0; i < tilesNo; i++)
    {
        // Choose a random index from current index to end of list
        int r = GetRandomValue(i, tilesNo - 1);
        SwapItems(&tilePositions[i], &tilePositions[r]);
    }

    updatedTilePos = true;
}

void StartTimer(void)
{
    // Reset timer
    timer = 0.0f;
    // Update timer text
    timerStarted = true;
    // Update start time
    startTime = clock();
}

static void SetupBoxes(Vector2 *v, Color *c, int len)
{
    // Set vectors and colors the amount of times inputted
    for (int i = 0; i < len; i++)
    {
        boxes[i] = v[i];
        boxColors[i] = c[i];
    }
}

void DrawBoxes(void)
{
    // Draw boxes until color is null
    for (int i = 0; boxColors[i].a != 0; i++)
    {
        // Get current color
        Color currentColor = boxColors[i];
        // Reduce opacity
        currentColor.a = 150;
        // Draw a rectangle at specified position with specified color
        DrawRectangleV(boxes[i], (Vector2) {tileSize, tileSize}, currentColor);
    }
}

static void ClearDrawingBoxes(void)
{
    // Stop drawing boxes
    for (int i = 0; boxColors[i].a != 0; i++)
    {
        boxes[i] = (Vector2) {0.0f, 0.0f};
        boxColors[i] = (Color) {0, 0, 0, 0};
    }
}

void DrawTimer(void)
{
    // If the timer is running
    if (timerStarted)
        // Timer is equal to current time minus start time
        timer = (float) (clock() - startTime) / CLOCKS_PER_SEC;
    
    // Setup timer text
    const char *timeText = TextFormat("Time: %.2fs", timer);
    // Measure width of string
    int textWidth = MeasureText(timeText, 15) / 2;

    // Draw text at the middle horizontally and 50 above the screen height
    DrawText(timeText, screenWidth / 2 - textWidth, screenHeight - 50, 15, RAYWHITE);
}

static void EndSort(void)
{
    // End timer
    timerStarted = false;

    // Make all boxes green
    for (int i = 0; i < tilesNo; i++)
    {
        boxes[i] = tilePositions[i].picturePos;
        boxColors[i] = GREEN;
    }

    // Wait for max time
    Delay(maxWaitTime);

    ClearDrawingBoxes();

    updatedTilePos = true;

    // End thread
    threadStarted = false;
    pthread_exit(NULL);
}

// Bubble sort (swap greater item in front of current item until done, sorts the greatest items every n iterations)
void BubbleSort(void* arg)
{
    int len = tilesNo, lastSortedIndex = 0, lastBox = 1, last = len - 1;

    // While there is more than one item to sort 
    while (len > 1)
    {
        // Index of last sorted item
        lastSortedIndex = 0;
        // Go through list, excluding the last item
        for (int i = 0; i < len - 1; i++)
        {
            // If current tile index is greater than next tile index
            if (tilePositions[i].index > tilePositions[i + 1].index)
            {
                SwapItems(&tilePositions[i], &tilePositions[i + 1]);
                // The swapped index (i + 1) is the last sorted index
                lastSortedIndex = i + 1;
            }

            // Draw a box at current index
            boxes[0] = tilePositions[i].actualPos;
            boxColors[0] = BLUE;

            Delay(sortDelay);
        }

        // Draw box at last sorted index
        boxes[lastBox] = tilePositions[last--].picturePos;
        boxColors[lastBox++] = GREEN;

        // New length should be the last sorted index because it doesn't need to be sorted again
        len = lastSortedIndex;
    }

    EndSort();
}

// Comb sort (same as bubble sort but with a greater gap (shrinks until 1))
void CombSort(void *arg)
{
    int gap = tilesNo;
    float shrinkFactor = 1.3f;
    bool sorted = false;

    // While not sorted
    while (!sorted)
    {
        // Reduce gap by shrink factor
        gap = floorf(gap / shrinkFactor);
        if (gap <= 1)
        {
            gap = 1;
            // It might be sorted;
            sorted = true;
        }

        // Loop while index (i) + gap is less than array size
        for (int i = 0; (i + gap) < tilesNo; i++)
        {
            // Draw boxes at i index and (gap + i) index
            Vector2 vecs[] = {tilePositions[i].actualPos, tilePositions[i + gap].actualPos};
            SetupBoxes(vecs, (Color[]) {BLUE, PURPLE}, 2);

            // If current item is greater than (gap + i) item
            if (tilePositions[i].index > tilePositions[i + gap].index)
            {
                // It's not sorted 
                sorted = false;
                SwapItems(&tilePositions[i], &tilePositions[i + gap]);
            }

            Delay(sortDelay);
        }
    }

    EndSort();
}

// Selection sort (gets the least item and puts it at the front of the list)
void SelectionSort(void* arg)
{
    int lastBox = 2;

    // Go through list, excluding the last item
    for (int i = 0; i < tilesNo - 1; i++)
    {
        // Stores least item index
        int minIndex = i;
        // Go through list starting from next i index
        for (int j = i + 1; j < tilesNo; j++)
        {
            // If current j is less than smallest index
            if (tilePositions[j].index <= tilePositions[minIndex].index)
                // Set j to be the smallest index
                minIndex = j;
    
            // Draw boxes at current index (j) and min index
            Vector2 vecs[] = {tilePositions[j].actualPos, tilePositions[minIndex].actualPos};
            SetupBoxes(vecs, (Color[]) {BLUE, PURPLE}, 2);

            Delay(sortDelay);
        }

        // Swap current item (i) with smallest item
        SwapItems(&tilePositions[i], &tilePositions[minIndex]);

        // Draw box at last sorted index
        boxes[lastBox] = tilePositions[i].picturePos;
        boxColors[lastBox++] = GREEN;
    }

    EndSort();
}

// Double selection sort (gets the least and greatest item and puts it at the front and end of the list)
void DoubleSelectionSort(void *arg)
{
    int len = tilesNo, lastBox = 3;

    // Go through list, excluding the last item
    for (int i = 0; i < len - 1; i++)
    {
        // Stores least and highest index
        int minIndex = i, maxIndex = i;
        // Go through list starting from next i index
        for (int j = i + 1; j < len; j++)
        {
            // Draw boxes at current index (j), max index and min index
            Vector2 vecs[] = {tilePositions[j].actualPos, tilePositions[maxIndex].actualPos, tilePositions[minIndex].actualPos};
            SetupBoxes(vecs, (Color[]){BLUE, PINK, PURPLE}, 3);

            // If current j is less than smallest index
            if (tilePositions[j].index < tilePositions[minIndex].index)
                // Set j to be smallest index
                minIndex = j;
            // If current j is greater than biggest index
            else if (tilePositions[j].index > tilePositions[maxIndex].index)
                // Set j to be the biggest index
                maxIndex = j;

            Delay(sortDelay);
        }

        // Swap current item (i) with smallest item
        SwapItems(&tilePositions[i], &tilePositions[minIndex]);
        // Swap last item with highest item (check if current item is the highest)
        SwapItems(&tilePositions[len - 1], &tilePositions[(i == maxIndex) ? minIndex : maxIndex]);

        // Exclude last item becuase it's already sorted
        len--;

        // Draw completed boxes
        // Top completed box
        boxes[lastBox] = tilePositions[i].picturePos;
        boxColors[lastBox++] = GREEN;
        // Bottom completed box
        boxes[lastBox] = tilePositions[tilesNo - 1 - i].picturePos;
        boxColors[lastBox++] = GREEN;
    }

    EndSort();
}

// Insertion sort (performs bubble sort on i -> 0 items)
void InsertionSort(void *arg)
{
    // Go through list starting from index 1
    for (int i = 1; i < tilesNo; i++)
    {
        // Draw a box at the end of insertion list
        boxes[0] = tilePositions[i].actualPos;
        boxColors[0] = PURPLE;

        // Start from index and go down
        for (int j = i; j > 0; j--)
        {
            // If the current j index is less than previous j index
            if (tilePositions[j].index < tilePositions[j - 1].index)
                SwapItems(&tilePositions[j], &tilePositions[j - 1]);

            // Draw box at current j
            boxes[1] = tilePositions[j].actualPos;
            boxColors[1] = BLUE;

            Delay(sortDelay);
        }
    }

    EndSort();
}

// Merge sort
// Functions merges two sorted arrays into one
static void Merge(PictureTile *arr1, PictureTile *arr2, int start, int end)
{
    // Length of given arrays
    int middle = (start + end) / 2; 
    int arr1Len = middle - start + 1, arr2Len = end - middle;
    
    // Draw boxes
    for (int i = start, x = 0; i <= end; i++, x++)
    {
        boxes[x] = tilePositions[i].actualPos;
        boxColors[x] = (i <= middle) ? BLUE : PURPLE;
    }

    // Go from start to end (inclusive)
    for (int arrIndex = start, arr1Index = 0, arr2Index = 0; arrIndex <= end; arrIndex++)
    {
        // If arr1 has not been fully consumed and arr2 has been
        // Or arr1 has not been fully consumed and it's current item is less than arr2's current item
        if (arr1Index < arr1Len && (arr2Index >= arr2Len || arr1[arr1Index].index <= arr2[arr2Index].index))
            tilePositions[arrIndex] = arr1[arr1Index++];
        // Else if arr1 has been fully consumed
        // or arr2's current item is less than arr1's current item
        else
            tilePositions[arrIndex] = arr2[arr2Index++];
        
        Delay(sortDelay);
    }

    ClearDrawingBoxes();

    // Free memory of given arrays
    free(arr1);
    free(arr2);
}

// Splits the array into small indivisible chunks and sorts it progressively
void MergeSort(void *arg)
{
    // Get first and second items
    int start = *((int *) arg), end = *((int *) arg + 1);

    // If there is more than one item in array
    if (end > start)
    {
        // Mid point of array
        int middle = (start + end) / 2;

        // Split first half recursively
        MergeSort((int[]) {start, middle});
        // Split second half recursively
        MergeSort((int[]) {middle + 1, end});

        // Merge both halves
        Merge(CreateSubArray(start, middle), CreateSubArray(middle + 1, end), start, end);
    }

    // End sort when the initial call finishes
    if (start == 0 && end == tilesNo - 1)
    {
        free(arg);
        EndSort();
    }
}
//---------------------

// Quick sort
// Function moves all smaller items to the left side of the pivot
static int Partition(int start, int end)
{
    // Highlight used boxes
    int bIndex = 0;
    for (int i = start; i <= end; i++, bIndex++)
    {
        boxes[bIndex] = tilePositions[i].actualPos;
        boxColors[bIndex] = WHITE;
    }
    // Draw box at pivot
    boxes[bIndex] = tilePositions[end].actualPos;
    boxColors[bIndex] = PINK;

    // Partition index, pivot item
    int pIndex = start, pivot = tilePositions[end].index;
    // Go from start to end (exclusive)
    for (int i = start; i < end; i++)
    {
        // If current item is less than or equal to pivot
        if (tilePositions[i].index <= pivot)
        {
            // Put current item in partition index
            SwapItems(&tilePositions[i], &tilePositions[pIndex]);
            // Increment partition index
            pIndex++;
        }

        // Draw box at current index (i)
        boxes[bIndex + 1] = tilePositions[i].actualPos;
        boxColors[bIndex + 1] = BLUE;
        // Draw box at pIndex
        boxes[bIndex + 2] = tilePositions[pIndex].actualPos;
        boxColors[bIndex + 2] = PURPLE;

        Delay(sortDelay);
    }

    // Put pivot in partition index
    SwapItems(&tilePositions[end], &tilePositions[pIndex]);

    ClearDrawingBoxes();

    // Return index of pivot
    return pIndex;
}

// Partitions array until sorted
void QuickSort(void *arg)
{
    // Get first and second items
    int start = *((int*) arg), end = *((int*) arg + 1);

    // If there is more than one item in array
    if (end > start)
    {
        // Partition list
        int partitionIndex = Partition(start, end);
        // Sort first half of items before partitioned item
        QuickSort((int[]) {start, partitionIndex - 1});
        // Sort half of items after partitioned item
        QuickSort((int[]) {partitionIndex + 1, end});
    }

    // End sort when the initial call finishes
    if (start == 0 && end == tilesNo - 1)
    {
        free(arg);
        EndSort();
    }
}
