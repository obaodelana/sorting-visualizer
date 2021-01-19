# Picture Sorting Visualizer
This is a not-so-informative but kind of good loooking sorting visualizer made in C using Raylib. This project was really fun to make and I learnt a lot:
- Sorting algorithms
- Function pointers
- Threads
- static keyword
- Timing stuff

## Code overview
I'm going to give a quick overview of the code and my thought process.

### Visualization
I'm treating the image like a sprite sheet and displaying each part of it individually (tile). I have a struct called `PictureTile` which has two Vector2's, one is called `picturePos`, it stores the position of the tile in the image (sprite sheet) and another one called `actualPos`, which stores the position of the tile in world space. It also has an integer called `index` which stores the index of the tile in the image (sprite sheet). There's an array of `PictureTile` which I access when drawing each tile. The array can be randomized and sorted, and it will display accordingly. Boxes are drawn to highlight certain processes.

### Sorting
I wrote the sorting functions normally while adding a delay in each iteration. This is where threads come in, if I don't create a thread the main thread will wait for the sorting function to finish thereby casuing the whole program to freeze for the duration of the sort.
- Bubble sort: Compares the current item with the next one and swaps them if the current item is greater than the next item. I optimised it by reducing the list of the array every n iterations since the last item will have been sorted.
- Comb sort: Same as Bubble sort but with a wider gap meaning it doesn't check the next item but (i + gap) item. The gap is reduced every n iterations until it becomes 1 which is just like Bubble sort.
- Selection sort: From i = 0, it gets the smallest item and places it at i, then i is incremented unti i = (length of the array).
- Double selection sort: Same as Selection sort but also gets the biggest item and places it at (len - 1), then len is decremented. It does this until i equals len. The running time is half of Selection sort.
- Insertion sort: From i = 1, it performs a Bubble sort backwards using i as the length of the array. It increases i after each sort until i equals the actual length of the array.
- Merge sort: Mind-boggling divide and conquer recursive algorithm 🥴. Takes an array and divides it into smaller arrays until the array contains only one item (which is already sorted), it then recursively sorts each array on the stack by merging, because it divides the array into very small chunks each chunk is sorted after merging.
- Quick sort: Also a divide and conquer recursive algorithm 😑. Picks an element (last element) as the pivot and puts all smaller items on the left and bigger items on the right. It does the same thing for the left and right hand side of the pivot until one item is left on both sides.

## Testing or building
- [You can access it online](https://obaodelana.github.io/sorting-visualizer/Web/pictureSortingVisualizer.html)
- Or
    - Clone the repo
    - If you're on Linux, run `./pictureSortingVisualizer`
    - Else
        - Download [Raylib](https://www.raylib.com/)
        - Open a terminal in this directory, type `make` and an executable will be created.