# Software Engineering Interview Homework
# Introduction

As a part of your interview for a software engineering position we would like you to complete the following assignment. Note that this assignment should not take more than 2-3 hours to complete. We suggest putting the resulting code into GitHub and sharing the link with us. For privacy you may use a private repository and add petrohi and mcollinswisc GitHub users as collaborators.

# The goal

Implement the Conway’s Game Of Life algorithm in C++. The Game Of Life is a cellular automaton algorithm. It runs on a universe, which is two-dimensional a grid of cells. Each cell can be in two states: alive or dead. There is an initial state for the universe, which defines alive cells and their positions in the universe. The automaton progresses one step at a time by applying a set of rules to each cell in the universe. There rules are the following: 
    1) Any live cell with two or three live neighbors survives;
    2) Any dead cell with exactly three live neighbors becomes a live cell;
    3) All other live cells die in the next generation. Similarly, all other dead cells stay dead.
You may check the Wikipedia article for more details.
To demonstrate the working algorithm, implement simple terminal visualization. Populate the universe with a glider pattern and animate its progress. 

Additional goals (not required)
    1) Implement the ability to load and store the universe from and to a file.
    2) Implement the “very large” (2^32 by 2^32) universe by using sparse representation.
    3) Implement visualization that automatically pans the view to show the alive pattern(s).
    4) Demonstrate with Gosper’s glider gun.

# The code:

Compile the code with "g++ -o main main.cpp" and then run it with the command "./main gun.csv"

The csv files are encoded sparse representations of the world seed for the game of life. The first line specifies the size of the world to be initialized, then subsequent lines take the format of (y,x) tuples that indicate alive indexes in the world. 

The main function can also be run as "./main glider.csv 50 50" which takes in an unencoded representation of the organism and specifies the number of rows and columns of the world this organism is to be placed in. The organism will be placed at the center of the world and the world size will remain fixed for the entire runtime.