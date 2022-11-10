# Multithreaded Word Puzzle in C
This program for Systems Programming (COP 4338) uses pthreads (multithreading) in C to search a 2,000,000,000 long char buffer for valid words. Also, this program times how long it will take to intitalize variables, memory, pthreads, and more in nanoseconds.

The Makefile is included for compiling this program.

## Instructions
1. To run this program, you need to compile it using make.
2. After compiling the program, the user will need to run randomgen and pass it a value of 2000000000 as a command-line argument. 
3. Then, you must pass the buffer/output of randomgen to wordpuzzle and add command-line arguments to wordpuzzle.

- wordpuzzle has multiple command-line arguments.
  -   -nthreads # (The user may specify the number of pthreads)
  -   -len # (The user may specify the length of a valid word to be searched (8, 9 is the default values))
  -   -time (The user may time how long it takes to binary search the dictionary)
  -   -verbose (The user may also specify to view additional details) 

- Example of running the program
```
./randomgen 2000000000 |./wordpuzzle -verbose -nthreads 16 -len 8
```
## Results
Here is an image of the total runtime (in nanoseconds) of the program based on the number of threads used:
![totalruntime](https://user-images.githubusercontent.com/25393705/201081796-270a49ff-7a62-4207-b610-eece71761390.JPG)