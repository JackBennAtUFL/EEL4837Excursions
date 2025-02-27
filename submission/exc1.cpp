
#include <fstream>
#include <iostream>
#include <string>
#include <math.h>
#include <vector>
#include <cstdlib>
#include <iomanip>
#include <sstream>


using namespace std; 

//Create the icoefficients matrix
vector<vector<double>> iCoefficients(double numsArr[], char labels[], int rows);

vector<vector<double>> createEqualsColumn(char labels[], double numsArr[], int rows, int nodes);

vector<vector<double>> createIncidence(double nodesCountArr[], int &nodes, int branches);

string getText(string fileName);

void outText(string outputStr);

int deduplicate(double nums[], int n);

void bubbleSort(double nums[], int n);

int countRows(string line);

vector<vector<double>> create2dVec(int row, int col);

vector<vector<char>> create2dVecChar(int row, int col);

vector<vector<double>> concatenateCol(vector<vector<double>> intialMatrix,vector<vector<double>>toConcat);
vector<vector<double>> concatenateRow(vector<vector<double>> intialMatrix,vector<vector<double>>toConcat);

vector<vector<double>> negateMat(vector<vector<double>> intialMatrix,int rows,int columns);

vector<vector<double>> transpose(vector<vector<double>> intialMatrix,int row,int col);

vector<vector<double>> createIncidence(double nodesCountArr[],int &nodes,int branches);

vector<vector<double>> createidentity(int size);

vector<vector<double>> createSparce(vector<vector<double>> incident,vector<vector<double>> currentCoef,vector<vector<double>> eqCol,int nodes,int rows);

vector<vector<double>> sparceToCSC(vector<vector<double>> sparce);

void solveCSC(vector<vector<double>> &CSC);

void swapRow(vector<vector<double>> &CSC,int row1index,int row2index);

void sortCSC(vector<vector<double>> &CSC);

void eliminateRowDown(vector<vector<double>> &CSC,int topRowIndex,int lowRowIndex,int operationCol);

//Takes a sorted array and deletes the zeros from the front 
void deleteCSCzeros(vector<vector<double>> &CSC);

string extractResultString(vector<vector<double>> CSC);

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

    vector<vector<double>> incidentMatrix = createIncidence(nodesCountArr,nodesCnt,countRow);
    
    vector<vector<double>> iCoef = iCoefficients(numsArr,elementLabels,countRow);

    vector<vector<double>> eqCol = createEqualsColumn(elementLabels,numsArr,countRow,nodesCnt);

    vector<vector<double>> sparce = createSparce(incidentMatrix,iCoef,eqCol,nodesCnt,countRow);

    vector<vector<double>> CSC = sparceToCSC(sparce);
    
    //These are useful to have
    int sparceHeight = countRow*2+nodesCnt;
    int sparceWidth = sparceHeight; 

    for(int i = 0;i<sparce.size();i++){
        for(int j = 0;j<sparce[0].size();j++){
            cout<<setw(4)<<sparce[i][j]<<" ";
        }
        cout<<endl;
    }

    //This works
    solveCSC(CSC);
    
    string result = extractResultString(CSC);

    outText(result);

    return 0;
}


string extractResultString(vector<vector<double>> CSC){

    string result;

    for(int i = 0;i<CSC[0].size();i++){
        ostringstream temp; 
        string tempString;
        int prc = 3;
        if( (int)CSC[0][i] != (int)CSC[1][i]){    
            temp << fixed<<setprecision(prc)<<CSC[2][i];
           
            tempString = temp.str();
            //Now search through temp    
            while(tempString[tempString.length()-1] == '0' && prc >= 0){
                temp << fixed<<setprecision(prc)<<CSC[2][i];
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

vector<vector<double>> sparceToCSC(vector<vector<double>> sparce){

    int row = sparce.size();
    int col = sparce[0].size();
    int cscWidth = 0;

    cout<<"row"<<col<<endl;
    //Count the non 0 elements in the sparce matrix
    for(int i = 0;i<row;i++){
        for(int j = 0;j<col;j++){
            if(sparce[i][j] != 0){
                cscWidth++;
            }
        }
    }
    //Generate destination array for the CSC
    vector<vector<double>> CSC = create2dVec(3,cscWidth);
    
    
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

vector<vector<double>> createSparce(vector<vector<double>> incident,vector<vector<double>> currentCoef,vector<vector<double>> eqCol,int nodes,int rows){

    vector<vector<double>> leftTop = create2dVec(nodes,nodes);
    vector<vector<double>> leftMiddle = negateMat(transpose(incident,nodes,rows),rows,nodes);
    vector<vector<double>> Leftbottom = create2dVec(rows,nodes);  
    vector<vector<double>> middletop = create2dVec(nodes,rows);
    vector<vector<double>> center = createidentity(rows);
    vector<vector<double>> middleBottom = createidentity(rows);
    vector<vector<double>> rightMiddle  = create2dVec(rows,rows);

      //This creates 3 rows in the large sparce matrix
    /*
    [0    ,0   ,A]
    [-A^T ,1   ,0]
    [0    ,M   ,N]
    */

    //[0    ,0   ,A]
    vector<vector<double>> row1step1 = concatenateCol(leftTop,middletop);
    vector<vector<double>> row1 = concatenateCol(row1step1,incident);
    //[-A^T ,1   ,0]
    vector<vector<double>> row2step1 = concatenateCol(leftMiddle,center);
    vector<vector<double>> row2 = concatenateCol(row2step1,rightMiddle);
    //[0    ,M   ,N]

    vector<vector<double>> row3step1 = concatenateCol(Leftbottom,center);
    vector<vector<double>> row3 = concatenateCol(row3step1,currentCoef);

    /*
    [0    ,0   ,A]
    [-A^T ,1   ,0]
    */
    vector<vector<double>> row1and2 = concatenateRow(row1,row2);
    
    /*
    [0    ,0   ,A]
    [-A^T ,1   ,0]
    [0    ,M   ,N]
    */
    vector<vector<double>> row123 = concatenateRow(row1and2,row3);

    vector<vector<double>> sparce = concatenateCol(row123,eqCol); 

    return sparce;
}

vector<vector<double>> iCoefficients(double numsArr[],char labels[],int rows){

    vector<vector<double>> icoeff = create2dVec(rows,rows);
    
    //Only copy a coeffcient if the corresponding element is a resistor
    for(int i = 0; i<rows;i++){
        if(labels[i] == 'R'){
            icoeff[i][i] = -1*numsArr[3*i+2];
        }
    }
    return icoeff;
}

vector<vector<double>> createEqualsColumn(char labels[],double numsArr[],int rows,int nodes){

    vector<vector<double>> equalsColumn =create2dVec(rows*2+nodes,1);

    for(int i = 0;i<rows;i++){
        if(labels[i] == 'V'){
            equalsColumn[i+rows+nodes][0] = numsArr[i*3+2];
        }
    }
    return equalsColumn;
}
//Creates the in cident matrix 
vector<vector<double>> createIncidence(double nodesCountArr[],int &nodes,int branches){
    
    vector<vector<double>> incidentMatrix = create2dVec(nodes - 1,branches);
    
    //Create the incident matrix
    for(int i = 0;i<branches*2;i++){
        if(nodesCountArr[i] == 0){

        }
        else if(i % 2 == 0 ){
            incidentMatrix[(int)(nodesCountArr[i]-1)][i/2] = 1; 
        }
        else{
            incidentMatrix[(int)(nodesCountArr[i]-1)][i/2] = -1; 
        }
    } 
    nodes--; 

    return incidentMatrix;
}

vector<vector<double>> createidentity(int size){

    vector<vector<double>> identity = create2dVec(size,size);

    for(int i = 0;i<size;i++){
        identity[i][i] = 1;
    }
    return identity; 

}

//Important note, the initial matrix will be to the left and the matrix "to concatenate" will be
//added on the right
vector<vector<double>> concatenateCol(vector<vector<double>> intialMatrix,vector<vector<double>>toConcat){

    int rows = intialMatrix.size();
    int colsL = intialMatrix[0].size();  
    int colsR = toConcat[0].size();  

    //Create a destination matrix with the right size 
    vector<vector<double>> concatenated = create2dVec(rows,colsL+colsR);
    
    
    
    //copy the intial matrix
    for(int i = 0;i<rows;i++){
        for(int j = 0;j<colsL;j++){
                concatenated[i][j] = intialMatrix[i][j];
        }
    }
    //Add the concatenated matrix appropriantely
    for(int i = 0;i<rows;i++){
        for(int j = 0;j<colsR;j++){
            concatenated[i][j+colsL] = toConcat[i][j];
        }
    }
    return concatenated;   
}

//Important note, the initial matrix will be on top and the matrix "to concatenate" will be
//added on the bottom
vector<vector<double>> concatenateRow(vector<vector<double>> intialMatrix,vector<vector<double>>toConcat){

    int rowsT = intialMatrix.size();
    int cols = intialMatrix[0].size();  
    int rowsB = toConcat.size();  

    //Create a destination matrix with the right size 
    vector<vector<double>> concatenated = create2dVec(rowsT+rowsB,cols);
    
    //copy the intial matrix
    for(int i = 0;i<rowsT;i++){
        for(int j = 0;j<cols;j++){
                concatenated[i][j] = intialMatrix[i][j];
        }
    }
    for(int i = 0;i<rowsB;i++){
        for(int j = 0;j<cols;j++){
            concatenated[i+rowsT][j] = toConcat[i][j];
        }
    }
    
    return concatenated;
   
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

//Using bubble sort to make it easier
void sortCSC(vector<vector<double>> &CSC){

    double tempRow;
    double tempCol;
    double tempVal;
    //This sorts the row columns then the columns

    //Loop that decrements the "maximum element" which is sorted
    for(int i = 0;i<CSC[0].size()-1;i++){
        //Bubble through the loop once 
        for(int j = 0;j<CSC[0].size()-1-i;j++){
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
    for(int i = 0;i<CSC[0].size()-1;i++){
        //Bubble through the loop once 
        for(int j = 0;j<CSC[0].size()-1-i;j++){
            //Swap values if needed
            if(CSC[0][j] == CSC[0][j+1] && CSC[1][j] > CSC[1][j+1]){
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

void eliminateRowDown(vector<vector<double>> &CSC,int topRowIndex,int lowRowIndex,int operationCol){

    int cols = CSC[0].size();


    double diagElement = 1; 
    //out<<"Diag El"<<diagElement;
    //Find the top left element to use
    for(int i = 0;i<cols;i++){
        if((int)CSC[0][i] == topRowIndex && (int)CSC[1][i] == operationCol){
            diagElement = CSC[2][i];
        }
    }

    double multiplicand = 0; 
    //Find the element to eliminate 
    for(int i = 0;i<cols;i++){
        if( (int)CSC[0][i] == lowRowIndex && (int)CSC[1][i] == operationCol){
            multiplicand = CSC[2][i]/diagElement;
        }
    }

    int countCorrectRow = 0;
    //count the elements with the initial row index
    for(int i = 0;i<cols;i++){
        if((int)CSC[0][i] == topRowIndex){
            countCorrectRow++; 
        }
        if(CSC[0][i] != topRowIndex && countCorrectRow != 0){
            break; 
        }
    }
    
    vector<vector<double>> copyFixedRow = create2dVec(3,countCorrectRow);
    
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
    
    vector<vector<double>> copyOperRow = create2dVec(3,countOperRow);

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
    vector<vector<double>> storeUnique = create2dVec(3,1);
    int dstPointer = 0; 
    int counter = 0;

    
    for(int i = 0; i<countCorrectRow;i++){
        //Eliminate a row if possible or add elements with same value
        if(copyFixedRow[1][i] == copyOperRow[1][dstPointer]){
            copyOperRow[2][dstPointer] = copyFixedRow[2][i]+copyOperRow[2][dstPointer];
            dstPointer++;
        }
        else{
            vector<vector<double>> storeUnique = create2dVec(3,1);

            storeUnique[0][0] = lowRowIndex;
            storeUnique[1][0] = copyFixedRow[1][i];
            storeUnique[2][0] = copyFixedRow[2][i];

            CSC = concatenateCol(CSC,storeUnique);

        }
    
    } 
    for(int i = 0;i<countOperRow;i++){
        for(int j = 0;j<cols;j++){
            //Check if row and col value match if so overwrite the values
            if(copyOperRow[0][i] == CSC[0][j] && copyOperRow[1][i] == CSC[1][j]){
                CSC[2][j] = copyOperRow[2][i];
            }
        }
    }
    
    sortCSC(CSC);

    deleteCSCzeros(CSC);

    return;
}

//This takes a sorted CSC matrix
void solveCSC(vector<vector<double>> &CSC){

    //Find the max row and column values
    int maxRow = 0;
    int maxCol = 0;
    for(int i = 0;i<CSC[0].size();i++){
        if((int)CSC[0][i] >maxRow) maxRow = (int)CSC[0][i];
        if((int)CSC[1][i] > maxCol) maxCol = (int)CSC[1][i];
    }

    //This does Row echelon form and solves down 
    int count = 0;
    //Create the row echelon matrix  
    //look down each col (metaphorically)
    for(int sweepCol = 0;sweepCol<maxCol-1;sweepCol++){
        
        bool found1st = false;

        //find the first column index in each row
        for(int i = 0;i<CSC[0].size();i++){
        
            //skip elements that are above the diagonal
            if(CSC[0][i] < CSC[1][i]){
            }
                //see if the diagonal has a non zero
            else if (CSC[1][i] == sweepCol && CSC[0][i] == sweepCol &&  0 != CSC[2][i]){
                found1st = true;
            }
            //swap once you find the first element in the column swap 
            else if (CSC[1][i] == sweepCol && found1st == false){
                swapRow(CSC,sweepCol,(int)CSC[0][i]);
                found1st = true;
            }
            else if (CSC[1][i] == sweepCol && found1st == true){
                eliminateRowDown(CSC,sweepCol,(int)CSC[0][i],sweepCol);
            }
        }
        
    }

    double rowFactor = 0;
    double row = 0; 
    //now do reduced row, divide the diagonals and get rref
    for(int i = 0;i<CSC[0].size();i++){
        row = CSC[0][i]; 
        if(CSC[0][i] == CSC[1][i]){
            rowFactor = CSC[2][i]; 
        }
        int j = i;
        while(j<CSC[0].size() && CSC[0][j] == row){
            
            i = j;
            CSC[2][j] = CSC[2][j]/rowFactor; 
            j++;
        }        
    }



    ///////////////////////////////////Problem somewhere in here 
    //next do the back substitution
    vector<vector<double>> solutions = create2dVec(maxRow,1);

    vector<vector<double>> storeUnique = create2dVec(3,1);

    //Iterate through all of the rows to solve
    for(int i = maxRow;i>=0;i--){
        //scan until you hit the right row value 
        for(int cscPointer = CSC[0].size()-1;cscPointer >= 0;cscPointer--){
            cout<<cscPointer<<endl;
            cout<<i<<endl;
            if(CSC[0][cscPointer] == i){
                if(( CSC[1][cscPointer] != maxCol)){
                    
                    storeUnique[0][0] = i;
                    storeUnique[1][0] = maxCol;
                    storeUnique[2][0] = 0; 

                    double tempElim = 0;
                    //Substitute the values needed and the 0 out the stuff in the left side of the matrix
                    while(CSC[0][cscPointer] == i && CSC[0][cscPointer] != CSC[1][cscPointer] && cscPointer>=0){
                        if(CSC[1][cscPointer] == maxCol){
                            cscPointer--;
                        }
                        else{
                            //search for row and column return value of the prior rows
                            double val; 
                            for(int search = CSC[0].size()-1;search>= 0;search--){
                                if(CSC[1][cscPointer] == CSC[0][search] && CSC[1][search] == maxCol){
                                    val =  CSC[2][search];
                                    break;
                                }
                            }
                        
                            tempElim = tempElim - CSC[2][cscPointer]*val; 
                            CSC[2][cscPointer] = 0; 
                            cscPointer--; 
                        }
                    }
                    //cout<<tempElim<<endl;
                    //concatenate the unique row and sort 
                    cout<<"did it get here"<<endl;
                    storeUnique[2][0]  = tempElim;
                    CSC = concatenateCol(CSC,storeUnique);
                    sortCSC(CSC);
                    deleteCSCzeros(CSC);

                }
                else{
                    double tempElim = 0;
                    double val = 0; 
                    //Substitute the values needed and the 0 out the stuff in the left side of the matrix
                    while(CSC[0][cscPointer] == i && (CSC[0][cscPointer] != CSC[1][cscPointer]) && cscPointer>=0){
                        if(CSC[1][cscPointer] == maxCol){
                            cscPointer--;
                        }
                        else{
                            //search for row and column return value of the prior rows
                            for(int search = CSC[0].size()-1;search>= 0;search--){
                                if(CSC[1][cscPointer] == CSC[0][search] && CSC[1][search] == maxCol){
                                    val =  CSC[2][search];
                                    cout<<"val is"<<val<<endl;
                                    cout<<"search is"<<search<<endl;
                                    break;
                                }
                            }

                            cout<<"telemetry"<<endl;
                            cout<<"temp elim pre is: "<<tempElim<<endl;
                            tempElim = tempElim - CSC[2][cscPointer]*val;
                            cout<<"temp elim post is: "<<tempElim<<endl;
                            // deleteCSCzeros(CSC);
                            // sortCSC(CSC); 
                            CSC[2][cscPointer] = 0; 
                            cscPointer--; 
                        }
                    }
                    //copy the new value to the equals column 
                    cout<<"here"<<endl;
                    for(int j = 0;j<CSC[0].size()-1;j++){
                        if(CSC[0][j] == i && CSC[1][j] == maxCol){
                            CSC[2][j] = CSC[2][j]+tempElim;
                        }
                    }
                    
                    
                }
                cout<<"break"<<endl;
                int k = 0;
                for(int j = 0;j<3;j++){
                    for(int i = 0;i<CSC[0].size();i++){
                        cout<<setw(7)<<setprecision(3)<<CSC[j][i];
                    }
                    cout<<endl;
                }
                break;
            }
        }
    }

    return;
}

//Takes a sorted array and deletes the zeros from the front 
void deleteCSCzeros(vector<vector<double>> &CSC){
    
    int col = CSC[0].size();
    int colCountNew = 0; 
    for(int i = 0;i<col;i++){
        
        if(CSC[2][i] != 0){
            //left shift everything
            CSC[0][colCountNew] = CSC[0][i];
            CSC[1][colCountNew] = CSC[1][i];
            CSC[2][colCountNew] = CSC[2][i];
            colCountNew++;
        }
    }
    for(int i = 0;i<3;i++){
        CSC[i].resize(colCountNew);
    }
    return; 
}

//Matrix tools
vector<vector<double>> transpose(vector<vector<double>> intialMatrix,int row,int col){
    vector<vector<double>> transpose = create2dVec(col, row);

    for(int i = 0;i<row;i++){
        for(int j = 0;j<col;j++){
            //I'm afraid of signed 0 
                transpose[j][i]  = 1*intialMatrix[i][j];      
        }
    }
    return transpose; 

}

//This flips the sign of all non zero array elements
vector<vector<double>> negateMat(vector<vector<double>> intialMatrix,int rows,int columns){
    vector<vector<double>> negation = create2dVec(rows, columns);

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

void outText(string outputStr){

    ofstream myFile;
    myFile.open("output.txt",ios::out); //read
    if(myFile.is_open()){
        myFile<<outputStr<<endl; 
        myFile.close();
    }
    else{
        cout<<"file write failed"<<endl;
    }  

    return; 
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

vector<vector<double>> create2dVec(int row, int col){
    vector<vector<double>> v  = vector<vector<double>>(row, vector<double>(col,0.0));
    return v;
}
vector<vector<char>> create2dVecChar(int row, int col){
    vector<vector<char>> v  = vector<vector<char>>(row, vector<char>(col,'0'));
    return v;
}

