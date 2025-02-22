//This is where we will write stuff 

#include <fstream>
#include <iostream>
#include <string>
#include <math.h>
#include <vector>
#include <cstdlib>
#include <iomanip>

using namespace std; 

string getText(string fileName); 

int deduplicate(double nums[], int n );

void bubbleSort(double nums[],int n);

int countRows(string line);

double** voltageArray(char labels[],double numsArr[],int rows);

double** gen2DArray(int row, int col);
    
double** createIncidence(double nodesCountArr[],int &nodes,int branches);

double** concatenateCol(double** intialMatrix,int rows,int columns,double** toConcatenate,int concatColumns);

double** negateMat(double** intialMatrix,int rows,int columns);

double** transpose(double** intialMatrix,int row,int col);

double** iCoefficients(double numsArr[],char labels[],int rows);

double** createSparce(double** incident,double** currentCoef,int nodes,int rows,double** equalsCol);

double** sparceToCSC(double** sparce,int row,int col,int &countElmt);

double** createEqualsColumn(char labels[],double numsArr[],int rows, int nodes);

double** solveCSC(double** CSC,int col,int row, int &countElmt,double** swapTracker);

void swapRow(double** CSC,int row1index,int row2index,int cols,double** swapTracker);

void sortCSC(double** CSC,int cols);

void deleteCSCzeros(double** CSC,int &col);

void eliminateRow(double** CSC,int topRowIndex,int lowRowIndex,int operationCol,int &cols);

int main(){

    //get the string form the file 
    string line = getText("netlist.txt");
    
    //Count the number of rows
    int countRow = countRows(line);
    
    //Create a netlist with an order
    char* elementLabels = new char[countRow];
    
    //Create a dynamic array for the node labels and elements 
    double* numsArr = new double[countRow*3];
    

    int lmtpointer = 0;
    int valPtr = 0;
    int numsArrSze = 0;

    int i = 0; 

    //Parse the netlist here because putting in functions will cause massive problems
    //With the returned arrays

    while(i<line.length()){
        string temp; 
        
        if(isalpha(line[i])){
            //Copy the letter either V or R
            if(line[i] != 'R' && line[i] != 'V'){
                cout<<"Bad circuit, check element names for L,C, only use V,R"<<endl;
                return 0;
            }

            temp += line[i++];
           

            //Get the element labels v1,r1,r2 etc
            while ((isdigit(line[i])||ispunct(line[i])) &&  i<line.length()){
                temp += line[i++];
                
            }
            elementLabels[lmtpointer++] = temp[0];
            
            //Go to the next number 
            while(line[i] == ' ' && i<line.length()){
                i++;    
            }
        }
        
            //Get elements until you hit the end of the "row" 
        if((isdigit(line[i])) && i<line.length()){
            
            string tempNum;
                
            //cout<<"test1"<<endl;
            //cout<<line[i]<<endl;
                
            while ((isdigit(line[i])||line[i] == '.') &&  i<line.length()){
                tempNum += line[i++];
            }
            //cout<<tempNum<<endl;
            numsArr[valPtr++] = stod(tempNum);
            numsArrSze++;
            while(line[i] == ' ' && i<line.length()){
                i++;
            }
        }
        if((line[i] == ','||line[i] == ' ') && i<line.length()){
            i++;
        }

    }
    
    double* nodesCountArr = new double[countRow*2];
    
    //Only for deduplicate
    double* nodesCountCopy = new double[countRow*2];

    int j = 0;
    for(int i = 0; i<numsArrSze;i++){
        if( (i+1) % 3 != 0){
            nodesCountArr[j] = numsArr[i];
            nodesCountCopy[j] = nodesCountArr[j];
            j++;
        }
    }
    
    int nodesCnt = deduplicate(nodesCountCopy,countRow*2);

    delete [] nodesCountCopy;
   
    double** incidentMatrix = createIncidence(nodesCountArr,nodesCnt,countRow);
    
    double** iCoeff = iCoefficients(numsArr,elementLabels,countRow);

    double** equalsColumn = gen2DArray(countRow*2+nodesCnt,1);

    double** equalsCol = createEqualsColumn(elementLabels,numsArr,countRow,nodesCnt);

    double** sparce = createSparce(incidentMatrix,iCoeff,nodesCnt,countRow,equalsCol);
    
    int countElmt = 0;

    double** CSC = sparceToCSC(sparce,countRow*2+nodesCnt,countRow*2+nodesCnt+1,countElmt); 

    //This will track the swaps, so you know where variables end up
    double** swapTracker = gen2DArray(countRow*2+nodesCnt,1);
    for(int i = 0;i<countRow*2+nodesCnt;i++){
        swapTracker[i][0] = i;
    }


    swapRow(CSC,0,2,countElmt,swapTracker);
    //cout here

    eliminateRow(CSC,0,3,0,countElmt);
    
    cout<<"sdfghjkl"<<endl<<endl;
    for(int i = 0;i<3;i++){
        for(int j = 0;j<countElmt;j++){
            cout <<CSC[i][j]<<" ";
        }
        cout<<endl;
    }

    return 0; 
}   

//Fetch the netlist string (this works)
string getText(string fileName){

    ifstream myFile;
    myFile.open(fileName,ios::in); //read
    if(myFile.is_open()){
        cout<<"File read complete "<<endl; 
    }
    else{
        cout<<"Error: Bad input file"<<endl;
    }
    string line; 
    
    getline(myFile,line);
    cout<<line<<endl;    

    return line; 
}

//These are used to find the number of unique nodes or kill the program is a circuit is bad
int deduplicate(double nums[], int n ){
    
    bubbleSort(nums,n);

    int dplctCnt = 0;
    
    //Count the duplicates
    for(int i = 0; i<n-1;i++){
        if(nums[i] != nums[i-1]){ 
            int j = 1+i;
            int tempDupCount = 0; 
            while(nums[i] == nums[j] && j<n){
                dplctCnt++; 
                tempDupCount++; 
                j++;
              
            }
            if(tempDupCount == 0 || dplctCnt == 0){
                throw invalid_argument("Bad circuit, no open circuits are allowed");
                return -1;
            }
        }
    }

    //Create the new array to copy to and determine new size
    int size = n-dplctCnt;
    //Fixed
    double* newArr = new double[size];

    int j = 0;
    int i = 0;
    //This loops through the original array and only copies an element if the an element is unique
    while(i<n){ 
        //Check for uniques
        while(nums[i] == nums[i+1]){ 
            i++;
        }
        //copy uniques to new array
        newArr[j] = nums[i];

        i++;
        j++;
    }
    //Copy the data back to the array 
    for(int i = 0;i<size;i++){
        nums[i] = newArr[i];
    }
    
    delete [] newArr;

    return size;
}

void bubbleSort(double nums[],int n){
    double temp;

    //Loop that decrements the "maximum element" which is sorted
    for(int i = 0;i<n-1;i++){
        //Bubble through the loop once
        for(int j = 0;j<n-1-i;j++){
            //Swap values if needed
            if(nums[j] < nums[j+1]){
                temp = nums[j]; 
                nums[j] = nums[j+1];
                nums[j+1] = temp; 
            }
        }
    }
    return;
}

//Count the number of rows
int countRows(string line){
    int countRow = 0;
        
    for(int i = 0; i<line.length();i++){
        if(isalpha(line[i])){
            countRow++; 
        }
    }
    return countRow; 
}

//Creates a 2D dynamic array to be used for the matrix math
//|||||ENSURE TO DELETE ARRAY WHEN FINISHED TO PREVENT MEMORY LEAKS!!!|||||
//Delete subarrays first then the full array
//-----***-----
double** gen2DArray(int row, int col){

    //Creates a pointer array of pointers of the double data type (2D array pointer)
    double** totArray = new double*[row];

    //Interates across the all of the rows and inserts another array within each index
    for(int i = 0; i < row; i++){
        totArray[i] = new double[col];

        //Fills the pointer array with zeros
        for(int j = 0; j < col; j++){
            totArray[i][j] = 0.0;
        }
    }

    //Returns the final array
    return totArray;

}
//-----***-----

double** voltageArray(char labels[],double numsArr[],int rows){
    
    int row = 0;

    for(int i = 0;i<rows;i++){
        
        //Count the number of voltage sources 
        if(labels[i] == 'V'){
            row++;
        }
    }

    double** voltageSources = gen2DArray(rows, 1);

    int j = 0; 
    for(int i = 0;i<rows;i++){

        if(labels[i] == 'V'){
            voltageSources[j][0] = numsArr[3*i+2];
            j++;
        }

    }

    return voltageSources; 
}

double** createIncidence(double nodeCountArr[],int &nodes,int branches){
    
    double** incidentMatrix = gen2DArray(nodes-1,branches);
    
    cout<<branches<<endl;

    //Create the incident matrix
    for(int i = 0;i<branches*2;i++){
        if(nodeCountArr[i] == 0){

        }
        else if(i % 2 == 0 ){
            incidentMatrix[(int)(nodeCountArr[i]-1)][i/2] = 1; 
        }
        else{
            incidentMatrix[(int)(nodeCountArr[i]-1)][i/2] = -1; 
        }
    } 
    nodes--; 

    return incidentMatrix;
}

//Important note, the initial matrix will be to the left and the matrix "to concatenate" will be
//added on the right
double** concatenateCol(double** intialMatrix,int rows,int columns,double** toConcatenate,int concatColumns){

    //Create a destination matrix with the right size 
    double** concatenated = gen2DArray(rows,columns+concatColumns);
    
    //copy the intial matrix
    for(int i = 0;i<rows;i++){
        for(int j = 0;j<columns;j++){
                concatenated[i][j] = intialMatrix[i][j];
        }
    
        for(int j = 0;j<concatColumns;j++){
            concatenated[i][j+columns] = toConcatenate[i][j];
        }
    }
    
    return concatenated;
   
}

//Important note, the initial matrix will be on top and the matrix "to concatenate" will be
//added on the bottom
double** concatenateRow(double** intialMatrix,int rows,int columns,double** toConcatenate,int concatRows){

    //Create a destination matrix with the right size 
    double** concatenated = gen2DArray(rows+concatRows,columns);
    
    //copy the intial matrix
    for(int i = 0;i<rows;i++){
        for(int j = 0;j<columns;j++){
                concatenated[i][j] = intialMatrix[i][j];
        }
    }
    for(int i = 0;i<concatRows;i++){
        for(int j = 0;j<columns;j++){
            concatenated[i+rows][j] = toConcatenate[i][j];
        }
    }
    
    return concatenated;
   
}

//This flips the sign of all non zero array elements
double** negateMat(double** intialMatrix,int rows,int columns){
    double** negation = gen2DArray(rows, columns);

    for(int i = 0;i<rows;i++){
        for(int j = 0;j<columns;j++){
            //I'm afraid of signed 0 
            if( -1*intialMatrix[i][j] != 0){
                negation[i][j]  = -1*intialMatrix[i][j];
            }
        }
    }
    return negation; 

}

double** transpose(double** intialMatrix,int row,int col){
    double** transpose = gen2DArray(col, row);

    for(int i = 0;i<row;i++){
        for(int j = 0;j<col;j++){
            //I'm afraid of signed 0 
           
                transpose[j][i]  = -1*intialMatrix[i][j];
           
        }
    }
    return transpose; 

}

double** createidentity(int size){

    double** identity = gen2DArray(size,size);

    for(int i = 0;i<size;i++){
        identity[i][i] = 1;
    }
    return identity; 

}

double** iCoefficients(double numsArr[],char labels[],int rows){

    double** icoeff = gen2DArray(rows,rows);
    
    for(int i = 0; i<rows;i++){
        if(labels[i] == 'R'){
            icoeff[i][i] = -1*numsArr[3*i+2];
        }
    }
    return icoeff;

}

double** createSparce(double** incident,double** currentCoef,int nodes,int rows,double** equalsCol){
  
    double** leftTop = gen2DArray(nodes,nodes);
    double** leftMiddle = negateMat(transpose(incident,nodes,rows),rows,nodes);
    double** Leftbottom = gen2DArray(rows,nodes);  
    double** middletop = gen2DArray(nodes,rows);
    double** center = createidentity(rows);
    double** middleBottom = createidentity(rows);

    //Right top is just the incident maxtrix

    double** rightMiddle  = gen2DArray(rows,rows);
    

    //This creates 3 rows in the large sparce matrix
    /*
    [0    ,0   ,A]
    [-A^T ,1   ,0]
    [0    ,M   ,N]
    */

    //[0    ,0   ,A]
    double** row1step1 = concatenateCol(leftTop,nodes,nodes,middletop,rows);
    double** row1 = concatenateCol(row1step1,nodes,nodes+rows,incident,rows);
    //[-A^T ,1   ,0]
    double** row2step1 = concatenateCol(leftMiddle,rows,nodes,center,rows);
    double** row2 = concatenateCol(row2step1,rows,nodes+rows,rightMiddle,rows);
    //[0    ,M   ,N]
    double** row3step1 = concatenateCol(Leftbottom,rows,nodes,center,rows);
    double** row3 = concatenateCol(row3step1,rows,nodes+rows,currentCoef,rows);

    /*
    [0    ,0   ,A]
    [-A^T ,1   ,0]
    */
    double** row1and2 = concatenateRow(row1,nodes,nodes+rows*2,row2,rows);
    
    /*
    [0    ,0   ,A]
    [-A^T ,1   ,0]
    [0    ,M   ,N]
    */
    double** row123 = concatenateRow(row1and2,nodes+rows,nodes+rows*2,row3,rows);

    double** sparce = concatenateCol(row123,nodes+rows*2,nodes+rows*2,equalsCol,1); 


    //Take out the trash: this is in here so it can be collapsed
    do{ 
        for (int i = 0; i < nodes; i++) {
            delete[] leftTop[i];
            delete[] middletop[i];
            delete[] incident[i];
        }
        for (int i = 0; i < rows; i++) {
            delete[] Leftbottom[i];
            delete[] center[i];
            delete[] middleBottom[i];
            delete[] leftMiddle[i];
            delete[] currentCoef[i];
            delete[] rightMiddle[i];
        }
        delete[] Leftbottom;
        delete[] center;
        delete[] middleBottom;
        delete[] leftMiddle;
        delete[] currentCoef;
        delete[] rightMiddle;
        delete[] leftTop;
        delete[] middletop;
        delete[] incident;
    }while(false);

    return sparce; 
}

//This converts the matrix to the CSC form which we will use if we want to do the extra credit
//I passed the element count by reference to "cleverly" return 2 variables without tuples
double** sparceToCSC(double** sparce,int row,int col,int &countElmt){

    //Count the non 0 elements in the sparce matrix
    for(int i = 0;i<row;i++){
        for(int j = 0;j<col;j++){
            if(sparce[i][j] != 0){
                countElmt++;
            }
        }
    }

    //Generate destination array for the CSC
    double** CSC = gen2DArray(3,countElmt);
    
    
    //Traverse the sparce and copy elements that are non-zere
    int CSCPoint = 0;
    for(int i = 0;i<row;i++){
        for(int j = 0;j<col;j++){
            if(sparce[i][j] != 0){
                //column
                CSC[0][CSCPoint] = i;
                //row
                CSC[1][CSCPoint] = j;
                //Value and increment dest column pointer
                CSC[2][CSCPoint++] = sparce[i][j];
            }
        }
    }
    return CSC;
}

//This creates the column with the element voltages, the curretns and the voltage source values
double** createEqualsColumn(char labels[],double numsArr[],int rows,int nodes){

    double** equalsColumn = gen2DArray(rows*2+nodes,1);

    for(int i = 0;i<rows;i++){
        if(labels[i] == 'V'){
            equalsColumn[i+rows+nodes][0] = numsArr[i*3+2];
        }
    }
    return equalsColumn;
}

double** solveCSC(double** CSC,int col,int row, int &countElmt,double** swapTracker){

    //Check
    int countRow = 0;

    //Ignore for now
    for(int sweepCol = 0;sweepCol<row;sweepCol++){
        //look through each row
        bool gotDiag =false;

        for(int sweepRow = countRow; sweepRow<countElmt;sweepRow++){
            if(CSC[sweepRow][sweepCol] != 0){
                
                for(int i = countRow;i<row;i++){
                    if(CSC[sweepRow][i] != 0){
                        swapRow(CSC,sweepRow,i,countElmt,swapTracker);
                    }
                    //go here if subsequent non zero elements are found in the current column
                    else if(CSC[sweepRow][i] != 0){

                    }
                }
                //Then you go and multiply the rows to perform the elimination, ignore the control logic
            }
        }
    }

    return CSC;
}

//Takes the rows to swap and the CSC matrix also the column count of the CSC
void swapRow(double** CSC,int row1index,int row2index,int cols,double** swapTracker){
        //This swaps the rows of the CSC
        for(int i = 0; i<cols;i++){
            //Check if rows match the desired values
            if(CSC[0][i] == row1index){
                CSC[0][i] = row2index;
            }
            else if(CSC[0][i] == row2index){
                CSC[0][i] = row1index;
            }
        }

        //Update the swap tracker
        swapTracker[row1index][0] = row2index; 
        swapTracker[row2index][0] = row1index; 

        //Re-sort the the swapped columns to preserve order 
        sortCSC(CSC,cols);

       
    return;
}

//Using bubble sort to make it easier
void sortCSC(double** CSC,int cols){

    double tempRow;
    double tempCol;
    double tempVal;
    //Loop that decrements the "maximum element" which is sorted
    for(int i = 0;i<cols-1;i++){
        //Bubble through the loop once
        for(int j = 0;j<cols-1-i;j++){
            //Swap values if needed
            if(CSC[0][j] > CSC[0][j+1]){
                tempRow = CSC[0][j]; 
                tempCol = CSC[1][j];
                tempVal = CSC[2][j];

                CSC[0][j] = CSC[0][j+1];
                CSC[1][j] = CSC[1][j+1];
                CSC[2][j] = CSC[2][j+1];

                CSC[0][j+1] = tempRow;
                CSC[1][j+1] = tempCol;
                CSC[2][j+1] = tempVal; 
            }
        }
    }
    return;
}

//Needs a sorted array
//This takes the row you want to eliminate and the row you want to keep and eliminates a value in the correct column
//Pass by reference because you might have to add rows when you do the addition/subtraction 
void eliminateRow(double** CSC,int topRowIndex,int lowRowIndex,int operationCol,int &cols){

    double diagElement = 0; 

    //out<<"Diag El"<<diagElement;
    //Find the top left element to use
    for(int i = 0;i<cols;i++){
        if(CSC[0][i] == topRowIndex && CSC[1][i] == operationCol){
            diagElement = CSC[2][i];
        }
    }

    double multiplicand = 0; 
    //Find the element to eliminate 
    for(int i = 0;i<cols;i++){
        if( CSC[0][i] == lowRowIndex && CSC[1][i] == operationCol){
            multiplicand = CSC[2][i]/diagElement;
        }
    }

    int countCorrectRow = 0;
    //count the elements with the initial row index
    for(int i = 0;i<cols;i++){
        if(CSC[0][i] == topRowIndex){
            countCorrectRow++; 
        }
        if(CSC[0][i] != topRowIndex && countCorrectRow != 0){
            break; 
        }
    }
    
    double** copyFixedRow = gen2DArray(3,countCorrectRow);
    
    //generate array for the row that is being operated on 
    int countOperRow = 0;
    for(int i = 0;i<cols;i++){
        if(CSC[0][i] == lowRowIndex){
            countOperRow++; 
        }
        if(CSC[0][i] != lowRowIndex && countOperRow != 0){
            break; 
        }
    }
    
    double** copyOperRow = gen2DArray(3,countOperRow);

    //Extract the initial row times by the multiplicand and -1 to prepare of addition 
    int copyPointerFixed =  0;
    int copyPointerOper =  0;

    for(int i = 0;i<cols;i++){
        if((int)CSC[0][i] == topRowIndex){
            copyFixedRow[0][copyPointerFixed] = CSC[0][i];
            copyFixedRow[1][copyPointerFixed] = CSC[1][i];
            copyFixedRow[2][copyPointerFixed] = -multiplicand*CSC[2][i];
            copyPointerFixed++; 
        }
        //Extract the row to operate on
        if((int)CSC[0][i] == lowRowIndex){
            copyOperRow[0][copyPointerOper] = CSC[0][i];
            copyOperRow[1][copyPointerOper] = CSC[1][i];
            copyOperRow[2][copyPointerOper] = CSC[2][i];
            copyPointerOper++; 
        }   
    }
  
    //Now add the 2 matricies using sum as limit for robustness
    double** storeUnique = gen2DArray(3,1);
    int dstPointer = 0; 
    int counter = 0;
    for(int i = 0; i<countCorrectRow;i++){
        //Eliminate a row if possible or add elements with same value
        if(copyFixedRow[1][i] == copyOperRow[1][dstPointer]){
            copyOperRow[2][dstPointer++] = copyFixedRow[2][i]+copyOperRow[2][dstPointer];
            counter++;
            cout<<"counter:"<<counter<<endl;
        }
        else{
            
            storeUnique[0][0] = lowRowIndex;
            storeUnique[1][0] = copyFixedRow[1][i];
            storeUnique[2][0] = copyFixedRow[2][i];

            cout<<cols<<endl;

            double** CSC2 = concatenateCol(CSC,3,cols,storeUnique,1);

             for(int i = 0; i<countCorrectRow;i++){
        //Eliminate a row if possible or add elements with same value
        if(copyFixedRow[1][i] == copyOperRow[1][dstPointer]){
            cout<<copyFixedRow[2][i]+copyOperRow[2][dstPointer]<<endl;
            copyOperRow[2][dstPointer++] = copyFixedRow[2][i]+copyOperRow[2][dstPointer];
            counter++;
            cout<<"counter:"<<counter<<endl;
        }
        else{
            
            storeUnique[0][0] = lowRowIndex;
            storeUnique[1][0] = copyFixedRow[1][i];
            storeUnique[2][0] = copyFixedRow[2][i];

            cout<<cols<<endl;

            //////////////////////////////////////////////////////////////////THIS NEEDS FIXING, concatenateCol returns a new array, but changing the 
            ////Array name constantly will cause problems 
            double** CSC2 = concatenateCol(CSC,3,cols,storeUnique,1);

            //This block is to deal with the concatenate col creating a new array
            cols++;
            cout<<cols<<endl;
            for (int i = 0; i < 3; i++) {
                 delete[] CSC[i];
            }
            delete[] CSC;

            cout<<"HERE";
            double** CSC = gen2DArray(3,cols);
            for(int i = 0;i<cols;i++){
                CSC[0][i] = CSC2[0][i];
                CSC[1][i] = CSC2[1][i];
                CSC[2][i] = CSC2[2][i];
            }

        }   
    }

            //This block is to deal with the concatenate col creating a new array
            cols++;
            cout<<cols<<endl;
            for (int i = 0; i < 3; i++) {
                 delete[] CSC[i];
            }
            delete[] CSC;

            cout<<"HERE";
            double** CSC = gen2DArray(3,cols);
            for(int i = 0;i<cols;i++){
                CSC[0][i] = CSC2[0][i];
                CSC[1][i] = CSC2[1][i];
                CSC[2][i] = CSC2[2][i];
            }

        }   
    }
    

    for(int i = 0;i<countOperRow;i++){
        for(int j = 0;j<cols;j++){
            //Check if row and col value match if so overwrite the values
            if(copyOperRow[0][i] == CSC[0][j] && copyOperRow[1][i] == CSC[1][j]){
                CSC[2][i] = copyOperRow[2][j];
            }
        }
    }

    sortCSC(CSC,cols);

    deleteCSCzeros(CSC,cols);

    return;
}

//Takes a sorted array and deletes the zeros from the front 
void deleteCSCzeros(double** CSC,int &col){
    for(int i = 0;i<col-1;i++){
        for(int j = 0;j<col-1;j++){
            //This acts as a break 
            if(CSC[2][i] != 0){
                j = col;
            }
            //left shift everything
            else{
                CSC[0][j] = CSC[0][j+1];
                CSC[1][j] = CSC[1][j+1];
                CSC[2][j] = CSC[2][j+1];
                col--;
            }
        }
    }
}
