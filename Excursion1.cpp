//This is where we will write stuff 

#include <fstream>
#include <iostream>
#include <string>
#include <math.h>
#include <vector>
#include <cstdlib>

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

double** createSparce(double** incident,double** currentCoef,int nodes,int rows);

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

    double** sparce = createSparce(incidentMatrix,iCoeff,nodesCnt,countRow);
    

    for(int i = 0;i<nodesCnt+countRow*2;i++){
        for(int j = 0;j<nodesCnt+countRow*2;j++){
            cout <<sparce[i][j]<<" ";
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

double** createidentity(int size ){

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

double** createSparce(double** incident,double** currentCoef,int nodes,int rows){
  
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
    double** sparce = concatenateRow(row1and2,nodes+rows,nodes+rows*2,row3,rows);

    return sparce; 
}