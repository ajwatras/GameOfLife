## A simple Example of Conway's Game of life. 

# The code:

Compile the code with "g++ -o main main.cpp" and then run it with the command "./main gun.csv"

The csv files are encoded sparse representations of the world seed for the game of life. The first line specifies the size of the world to be initialized, then subsequent lines take the format of (y,x) tuples that indicate alive indexes in the world. 

The main function can also be run as "./main glider.csv 50 50" which takes in an unencoded representation of the organism and specifies the number of rows and columns of the world this organism is to be placed in. The organism will be placed at the center of the world and the world size will remain fixed for the entire runtime.