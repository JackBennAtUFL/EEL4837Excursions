#include <fstream>
#include <iostream>
#include <string>
#include <math.h>
#include <vector>
#include <sstream>
#include <iomanip>

using namespace std; 

//Create an empty 2d vector with sizes
vector<vector<double>> create2dVec(int row, int col){
    return vector<vector<double>>(row, vector<double>(col,0.0));
}

//Take the CSC and print the results
string extractResultString(vector<vector<double>> CSC){

    string result;
    //Scan through the solved matrix
    for(int i = 0;i<CSC[0].size();i++){
        ostringstream temp; 
        string tempString;
        int prc = 3;
        //Extract the result values ignore rref 1s
        if( (int)CSC[0][i] != (int)CSC[1][i]){

            //Returns the rounded number from the array
            //Multiplying the value by a factor ensures the proper number of decimal places
            temp << fixed<<setprecision(prc)<<((int)round((CSC[2][i]*pow(10, prc)))/pow(10, prc));
           
            tempString = temp.str();
            //Now search through temp and strip back decimals for pretty outputs
            while(tempString[tempString.length()-1] == '0' && prc >= 0){
                temp << fixed<<setprecision(prc)<<((int)round((CSC[2][i]*pow(10, prc)))/pow(10, prc));
                tempString.erase();
                tempString = temp.str();
                temp.str("");
                prc--; 
            }
        }

        result = result + " " + tempString;
    }
    return result;
}

//Important note, must have same rows the initial matrix will be to the left and the matrix "to concatenate" will be added on the right
vector<vector<double>> concatenateCol(vector<vector<double>> intialMatrix,vector<vector<double>>toConcat){

    //Create a destination matrix with the right size 
    vector<vector<double>> concatenated = create2dVec(intialMatrix.size(),(intialMatrix[0].size()+toConcat[0].size()));
    
    //copy the intial matrix
    for(int i = 0;i<intialMatrix.size();i++){
        for(int j = 0;j<intialMatrix[0].size()+toConcat[0].size();j++){
            if(j<intialMatrix[0].size()){
                concatenated[i][j] = intialMatrix[i][j];
            }else{ 
                concatenated[i][j] = toConcat[i][j-intialMatrix[0].size()];
            }
        }
    }
    return concatenated;   
}

//Important note, the initial matrix will be on top and the matrix "to concatenate" will be added on the bottom must have same cols
vector<vector<double>> concatenateRow(vector<vector<double>> intialMatrix,vector<vector<double>>toConcat){
    //Create a destination matrix with the right size 
    vector<vector<double>> concatenated = create2dVec(intialMatrix.size()+toConcat.size(),intialMatrix[0].size());
    
    //copy the intial matrix, this is an exact inversion of concat col
    for(int i = 0;i<intialMatrix[0].size();i++){
        for(int j = 0;j<intialMatrix.size()+toConcat.size();j++){
            if(j<intialMatrix.size()){
                concatenated[j][i] = intialMatrix[j][i];
            }else{ 
                concatenated[j][i] = toConcat[j-intialMatrix.size()][i];
            }
        }
    }
    return concatenated;
}

//This flips the sign of all non zero array elements
vector<vector<double>> negateMat(vector<vector<double>> intialMatrix,int rows,int columns){
    vector<vector<double>> negation = create2dVec(rows, columns);

    for(int i = 0;i<rows;i++){
        for(int j = 0;j<columns;j++){
            //I'm afraid of signed 0  
            if(intialMatrix[i][j] != 0) negation[i][j]  = -intialMatrix[i][j];
        }
    }
    return negation; 
}

//Matrix tools, this creates a transpose of a matrix
vector<vector<double>> transpose(vector<vector<double>> intialMatrix,int row,int col){
    vector<vector<double>> transpose = create2dVec(col, row);

    for(int i = 0;i<intialMatrix.size();i++){
        for(int j = 0;j<intialMatrix[0].size();j++){
            transpose[j][i] = intialMatrix[i][j];      
        }
    }
    return transpose; 
}

//Scan through the list of elements and extract the resistors
vector<vector<double>> iCoefficients(double numsArr[],char labels[],int rows){
    vector<vector<double>> icoeff = create2dVec(rows,rows);
    
    //Only copy a coeffcient if the corresponding element is a resistor, rows is the nums arr size
    for(int i = 0; i<rows;i++){
        if(labels[i] == 'R') icoeff[i][i] = -1*numsArr[3*i+2];
        if(icoeff[i][i] > 0) return vector<vector<double>>(1, vector<double>(1, 100));
    }
    return icoeff;
}

//This creates the values that the sparce matrix is equal to
vector<vector<double>> createEqualsColumn(char labels[],double numsArr[],int rows,int nodes){

    vector<vector<double>> equalsColumn =create2dVec(rows*2+nodes,1);
    //only care about the voltage values
    for(int i = 0;i<rows;i++){
        if(labels[i] == 'V') equalsColumn[i+rows+nodes][0] = numsArr[i*3+2];
    }
    return equalsColumn;
}
//Creates the incident matrix 
vector<vector<double>> createIncidence(double nodesCountArr[],int &nodes,int branches){
    
    vector<vector<double>> incidentMatrix = create2dVec(nodes-1,branches);
    nodes--; //Eliminate the ground node 

    //Create the incident matrix using position and traverse the nodes array
    for(int i = 0;i<branches*2;i++){
        if(nodesCountArr[i] == 0);

        else if(i % 2 == 0 ) incidentMatrix[(int)(nodesCountArr[i]-1)][i/2] = 1; 
        else incidentMatrix[(int)(nodesCountArr[i]-1)][i/2] = -1; 
    } 
    return incidentMatrix;
}

//Create an identity matrix for the sparce matricies
vector<vector<double>> createidentity(int size){
    //put ones where nessesary
    vector<vector<double>> identity = create2dVec(size,size);
    for(int i = 0;i<size;i++){
        identity[i][i] = 1;
    }
    return identity; 
}

//Convert the sparce matrix to a CSC matrix
vector<vector<double>> sparceToCSC(vector<vector<double>> sparce) {
    //Generate destination array for the CSC
    vector<vector<double>> CSC = create2dVec(3,0);
    
    //Traverse the sparce and copy elements that are non-zere
    for(int i = 0;i<sparce.size();i++){
        for(int j = 0;j<sparce[0].size();j++){
            if(sparce[i][j] != 0){
                CSC[0].push_back(i);
                CSC[1].push_back(j);
                CSC[2].push_back(sparce[i][j]);
            }
        }
    }
    return CSC;
}

//This creates the sparce matrix which will be shrunk down to CSC form
vector<vector<double>> createSparce(vector<vector<double>> incident,vector<vector<double>> currentCoef,vector<vector<double>> eqCol,int nodes,int rows){
    //Create the component arrays
    vector<vector<double>> leftTop = create2dVec(nodes,nodes);
    vector<vector<double>> leftMiddle = negateMat(transpose(incident,nodes,rows),rows,nodes);
    vector<vector<double>> Leftbottom = create2dVec(rows,nodes);  
    vector<vector<double>> middletop = create2dVec(nodes,rows);
    vector<vector<double>> center = createidentity(rows);
    vector<vector<double>> rightMiddle  = create2dVec(rows,rows);

    //This function creates 3 rows in the large sparce matrix then concatenates them
    //[0    ,0   ,A]
    vector<vector<double>> sparce = concatenateCol(concatenateCol(leftTop,middletop),incident);
    //[-A^T ,1   ,0]
    vector<vector<double>> row2 = concatenateCol(concatenateCol(leftMiddle,center),rightMiddle);
    
    
    /*[0    ,0   ,A]
      [-A^T ,1   ,0] */
    sparce = concatenateRow(sparce,row2);

    //[0    ,M   ,N]
    vector<vector<double>> row3 = concatenateCol(concatenateCol(Leftbottom,center),currentCoef);
    
    /*[0    ,0   ,A]
      [-A^T ,1   ,0]
      [0    ,M   ,N] */
    sparce =  concatenateRow(sparce,row3);
    
    //Return the sparce with the equals column
    return concatenateCol(sparce,eqCol); 
}

//Sort the CSC array and then delete the 0 elements, his sorts the row columns then the columns
void sortCSC(vector<vector<double>> &CSC){

    double temp;
    //Loop that decrements the "maximum element" which is sorted
    for(int i = 0;i<CSC[0].size()-1;i++){
        //Bubble through the loop once 
        for(int j = 0;j<CSC[0].size()-1-i;j++){
            //Swap values if needed, this sorts rows and columns, the right side of the or is column sorting
            if(CSC[0][j] > CSC[0][j+1] || (CSC[0][j] == CSC[0][j+1] && CSC[1][j] > CSC[1][j+1])){
                temp = CSC[0][j]; 
                CSC[0][j] = CSC[0][j+1];
                CSC[0][j+1] = temp;

                temp = CSC[1][j];
                CSC[1][j] = CSC[1][j+1];
                CSC[1][j+1] = temp;

                temp = CSC[2][j];
                CSC[2][j] = CSC[2][j+1];
                CSC[2][j+1] = temp; 
            }
        }
    }
    //Now delete the zero elements from the CSC 
    int colCountNew = 0; 
    for(int i = 0;i<CSC[0].size();i++){ 
        if(CSC[2][i] != 0){
            //left shift everything
            CSC[0][colCountNew] = CSC[0][i];
            CSC[1][colCountNew] = CSC[1][i];
            CSC[2][colCountNew++] = CSC[2][i];
        }
    }
    for(int i = 0;i<3;i++){
        CSC[i].resize(colCountNew);
    }
    return; 
}

//Takes the rows to swap and the CSC matrix also the column count of the CSC
void swapRow(vector<vector<double>> &CSC,int row1index,int row2index){
    //This swaps the rows of the CSC
    for(int i = 0; i<CSC[0].size();i++){
        //Check if rows match the desired values
        if(CSC[0][i] == row1index){
            CSC[0][i] = row2index;
        }
        else if(CSC[0][i] == row2index){
            CSC[0][i] = row1index;
        }
    }
    //Re-sort the the swapped columns to preserve order 
    sortCSC(CSC);
return;
}

//Eliminate rows from a column so this will be called going down 
void eliminateRowDown(vector<vector<double>> &CSC,int topRowIndex,int lowRowIndex,int operationCol){
    double multiplicand = 0;
    double diagElement = 0;
    int countCorrectRow = 0;
    int countOperRow = 0;

    //Find the top left element to use
    for(int i = 0;i<CSC[0].size();i++){
        if((int)CSC[1][i] == operationCol){
            if((int)CSC[0][i] == topRowIndex){
                diagElement = CSC[2][i];
            }
            else if((int)CSC[0][i] == lowRowIndex){
                multiplicand = CSC[2][i]/diagElement;
            } 
        }
        //Incremnet the row copying
        if((int)CSC[0][i] == topRowIndex) countCorrectRow++;
        if((int)CSC[0][i] == lowRowIndex) countOperRow++; 
    }
    
    vector<vector<double>> copyFixedRow = create2dVec(3,countCorrectRow);
    vector<vector<double>> copyOperRow = create2dVec(3,countOperRow);

    //Extract the initial row times by the multiplicand and -1 to prepare of addition 
    int copyPointerFixed =  0;
    int copyPointerOper =  0;

    for(int i = 0;i<CSC[0].size();i++){
        if((int)CSC[0][i] == topRowIndex){
            copyFixedRow[0][copyPointerFixed] = CSC[0][i];
            copyFixedRow[1][copyPointerFixed] = CSC[1][i];
            copyFixedRow[2][copyPointerFixed] = -multiplicand*CSC[2][i];
            copyPointerFixed++; 
        }
        //Extract the row to operate on
        else if((int)CSC[0][i] == lowRowIndex){
            copyOperRow[0][copyPointerOper] = CSC[0][i];
            copyOperRow[1][copyPointerOper] = CSC[1][i];
            copyOperRow[2][copyPointerOper] = CSC[2][i];
            copyPointerOper++; 
        }   
    }

    //Now add the 2 matricies using sum as limit for robustness
    int dstPointer = 0; 

    //Check through the copy and the orignal to find elements with matching columns then combine
    for(int i = 0; i<copyFixedRow[0].size();i++){
        bool notUnique = false;
        for(int j = 0; j<copyOperRow[0].size();j++){
            //Eliminate a row if possible or add elements with same row and col values
            if((int)copyFixedRow[1][i] == (int)copyOperRow[1][j]){
                copyOperRow[2][j] = copyFixedRow[2][i]+copyOperRow[2][j];
        
                notUnique = true;
            }
        }
        if(!notUnique){
            CSC[0].push_back(lowRowIndex);
            CSC[1].push_back(copyFixedRow[1][i]);
            CSC[2].push_back(copyFixedRow[2][i]);
            sortCSC(CSC);
        }   
    }

    //scan through the copied row and copy similar elements into the the CSC 
    for(int i = 0;i<countOperRow;i++){
        for(int j = 0;j<CSC[0].size();j++){
            //Check if row and col value match if so overwrite the values
            if(copyOperRow[0][i] == CSC[0][j] && copyOperRow[1][i] == CSC[1][j]){
                CSC[2][j] = copyOperRow[2][i];
            }
        }
    }
    sortCSC(CSC);
    return;
}

//This takes a sorted CSC matrix
void solveCSC(vector<vector<double>> &CSC ){

    //Find the max row and column values
    int maxRow = 0;
    int maxCol = 0;
    for(int i = 0;i<CSC[0].size();i++){
        if((int)CSC[0][i] >maxRow) maxRow = (int)CSC[0][i];
        if((int)CSC[1][i] > maxCol) maxCol = (int)CSC[1][i];
    }
    
    //This does Row echelon form and solves down 
    //Create the row echelon matrix by looking down each col (metaphorically)
    for(int sweepCol = 0;sweepCol<maxCol-1;sweepCol++){
        bool found1st = false;

        //find the first column index in each row
        for(int i = 0;i<CSC[0].size();i++){
        
            //skip elements that are above the diagonal
            if(CSC[0][i] < CSC[1][i]){
            }
            else if ((int)CSC[1][i] == sweepCol && (int)CSC[0][i] == sweepCol &&  0 != (int)CSC[2][i]){
                found1st = true;
            }
            //swap once you find the first element in the column swap 
            else if ((int)CSC[1][i] == sweepCol && found1st == false){
                swapRow(CSC,sweepCol,(int)CSC[0][i]);                
                found1st = true;
            }
            //eliminate rows once the top row is in place
            else if ((int)CSC[1][i] == sweepCol && found1st == true){
                eliminateRowDown(CSC,sweepCol,(int)CSC[0][i],sweepCol);
                sortCSC(CSC);
            } 
        }      
    }
    
    //zero out division artifacts
    for(int i = 0;i<CSC[0].size();i++){
        if(abs(CSC[2][i]) < 0.000001 ) CSC[2][i] = 0;
    }

    sortCSC(CSC);
    
    double rowFactor, row = 0; 
    //now do reduced row, divide the diagonals and get rref
    for(int i = 0;i<CSC[0].size();i++){
        row = CSC[0][i]; 
        if((int)CSC[0][i] == (int)CSC[1][i]){
            rowFactor = CSC[2][i]; 
        }
        int j = i;
        //divide through the row
        while(j<CSC[0].size() && (int)CSC[0][j] == row){
            i = j;
            CSC[2][j++] /= rowFactor; 
        }        
    }
   
    //Iterate through all of the rows to solve
    for(int i = maxRow;i >= 0;i--){
        //scan until you hit the right row value 
        for(int cscPointer = CSC[0].size()-1;cscPointer >= 0;cscPointer--){
            if( (int)CSC[0][cscPointer] == i){
                //go here if there is not a row in the CSC for the last column i.e. there is a 0 to the right of the equals
                bool createRow = false;
                if(((int)CSC[1][cscPointer] != maxCol)){
                    createRow = true;
                }
                
                double tempElim = 0;
                double val;
                //Substitute the values needed and the 0 out the stuff in the left side of the matrix
                while((int)CSC[0][cscPointer] == i && (int)CSC[0][cscPointer] != (int)CSC[1][cscPointer] && cscPointer>=0){
                    //back up and skip the stuff to the right of the equals sign
                    if(CSC[1][cscPointer] == maxCol)  cscPointer--;
                    else
                    {
                        //search for row and column return value of the prior rows that have been solved
                        for(int search = CSC[0].size()-1;search >= 0;search--){
                            if(CSC[1][cscPointer] == CSC[0][search] && CSC[1][search] == maxCol){
                                val =  CSC[2][search];
                                break;
                            }
                            //this else is safe because we previously rounded off the to 10^-5
                            else val = 0.0000001; 
                        }
                        //Sum the elements of the row that we 
                        tempElim -= CSC[2][cscPointer]*val; 
                        CSC[2][cscPointer--] = 0;  
                    }
                }

                if(createRow == true){
                    //concatenate the unique row and sort 
                    CSC[0].push_back(i);
                    CSC[1].push_back(maxCol);
                    CSC[2].push_back(tempElim);
                    sortCSC(CSC);
                }
                else{
                    //copy the new value to the equals column 
                    for(int j = 0;j<CSC[0].size()-1;j++){
                        if(CSC[0][j] == i && CSC[1][j] == maxCol){
                            CSC[2][j] += tempElim;
                        }
                    }
                }
                //move to the next row
                break;
            }
        }
    }
    //Make sure the bottom right element exists for the string out
    if(CSC[1].back() != maxCol){
        CSC[0].push_back(maxRow);
        CSC[1].push_back(maxCol);
        CSC[2].push_back(0);
    }
    return;
}

//Fetch the netlist string (this works)
string getText() {
    // Read the vector from the file
    string line;
    string newLine = "";
    vector<char> net;
    ifstream myFile;
    myFile.open("netlist.txt", ios::in);

    if (myFile.is_open()) {
        bool carReturn = false;
        while (getline(myFile, line)) {
            for (char c : line) {
                net.push_back(c);
            }
            net.push_back(' ');
        }
       
        //pass stuff through a vector because autograder madness
        vector<char> fixedNet;
        for (char c : net) {
            if (c != '\r')  fixedNet.push_back(c); 
        }

        myFile.close();
        
        for(int i = 0;i<fixedNet.size();i++) newLine += fixedNet[i];
        
    } else cout << "Error: Bad input file" << endl;
    
    return newLine;
}

//Write to the output text file
void outText(string outputStr){
    //write out the results
    ofstream myFile;
    myFile.open("output.txt",ios::out); 
    if(myFile.is_open()){
        myFile<<outputStr<<endl; 
        myFile.close();
    }
    else cout<<"file write failed"<<endl;
    return; 
}

//These are used to find the number of unique nodes or kill the program is a circuit is bad
int countNodes(double nums[], int n){
    //Sort the nums array so you can count uniques
    double temp;

    //This is just bubble sort
    for(int i = 0;i<n-1;i++){
        for(int j = 0;j<n-1-i;j++){
            //Swap values if needed
            if(nums[j] < nums[j+1]){
                temp = nums[j]; 
                nums[j] = nums[j+1];
                nums[j+1] = temp; 
            }
        }
    }

    int dplctCnt = 0;
    //Count the duplicates
    for(int i = 0; i<n-1;i++){
        if((int)nums[i] != (int)nums[i-1]){ 
            int j = 1+i;
            while((int)nums[i] == (int)nums[j++] && j<n){
                dplctCnt++; 
            }
        }
    }
    return n-dplctCnt;
}

//Count the number of rows in a string with the netlist
int countRows(string line){
    int countRow = 0;
        
    for(int i = 0; i<line.length();i++){
        if(isalpha(line[i])) countRow++;
    }
    return countRow; 
}

int main(){

    //get the string form the file 
    string line = getText();
    //Count the number of rows
    int countRow = countRows(line);
     
    //Create a dynamic array for the node labels and elements 
    char* elementLabels = new char[countRow]; 
    double* numsArr = new double[countRow*3];

    int lmtpointer,valPtr, strPtr = 0;

    while(strPtr<line.length()){
        //Copy the element labels
        if(isalpha(line[strPtr])){
           
            elementLabels[lmtpointer++] = line[strPtr++];

            //Skip over the net label numbers because that is now encoded in the element label order
            while(isdigit(line[strPtr++]));
        }
        //Get elements until you hit the end of the "row" 
        else if((isdigit(line[strPtr]) || line[strPtr] == '-')){  
        
            string tempNum = "";
        
            while ((isdigit(line[strPtr]) || line[strPtr] == '.' || line[strPtr] == '-')){
                tempNum = tempNum + line[strPtr++];
            }
            numsArr[valPtr++] = atof(tempNum.c_str());
        }
        //This will just catch odd cases or ends of lines 
        else strPtr++;
    }
    
    //Create two arrays, one for incident and one for couting the elements in the inicdent matrix
    double* nodesCountArr = new double[countRow*2];
    double* nodesCountCopy = new double[countRow*2];

    int j = 0;
    for(int i = 0; i<countRow*3;i++){
        if((i+1) % 3 != 0){
            nodesCountArr[j] = numsArr[i];
            nodesCountCopy[j++] = nodesCountArr[j];
        }
    }

    int nodesCnt = countNodes(nodesCountCopy,countRow*2);

    //create the nessesary matricies
    vector<vector<double>> incidentMatrix = createIncidence(nodesCountArr,nodesCnt,countRow);
    vector<vector<double>> iCoef = iCoefficients(numsArr,elementLabels,countRow);
    vector<vector<double>> eqCol = createEqualsColumn(elementLabels,numsArr,countRow,nodesCnt);
    vector<vector<double>> CSC = sparceToCSC(createSparce(incidentMatrix,iCoef,eqCol,nodesCnt,countRow));
   
    //solve the csc
    solveCSC(CSC);
    //output the text
    outText(extractResultString(CSC));

    return 0;
}