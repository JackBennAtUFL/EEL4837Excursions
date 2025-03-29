This is the readme file for excursion 1 for EEL4837 of Spring 2025
Jack Benn and Joshua Kwak were the authors of this program

--Compiling the program
Compiling the program should be fairly straight forward. As the entire code is encapsulated within a single c++ file, there should be no issues with including header files. Similarly, only base libraries were used so as long as a proper include path has been designated to the standard included libraries (vector, iostream, ect.) there should not be an major file issues.
To compile the file into an .exe file, run the line

g++ exc1.cpp -o ex1

The file was compiled with g++ version 13.1.0 but should not be limited by previous versions.

--Executing the file
After compiling the cpp file, the .exe file can be run easily within the directory of the files in the command prompt by running

ex1.exe

This run will only work if the executable was given the proper files to reference. Running the actual program does not require any parameters to be passed directly, but it does require a netlist.txt file to be present in the same directory as the executable. This netlist stores the input information given the format from the instructions. This should then create (if not already present) or change the output.txt file within the same directory that holds the output information from the program.

--Running the file
The program runs entirely automatically so there is no user input needed to interface with the program after the initial execution

--Bonus Points
We decided to implement the extra credit challenge for this program. Here, we implemented the solving of the functions through sparce matrices represented with a dynamic vector. Using gaussian elimination, we were able to successfully implement the algorithm given this different method of representation.