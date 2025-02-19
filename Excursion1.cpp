//This is where we will write stuff 

#include <fstream>
#include <iostream>
#include <string>
#include <math.h>
#include <vector>
#include <cstdlib>

using namespace std; 

string getText(string fileName); 

int deduplicate(double nums[ ], int n );

void bubbleSort(double nums[],int n);

int countRows(string line);

int main(){

    //get the string form the file 
    string line = getText("netlist.txt");
    
    //Count the number of rows
    int countRow = countRows(line);
    
    //Create a netlist with an order
    vector<string> elementLabels(countRow,"X");
    
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
            elementLabels[lmtpointer++] = temp;
            
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
    
    /*
    //looking at the string vector, 
    for(int i = 0;i<countRow;i++){
        cout<<elementLabels[i]<<endl;
    }
    */
    // for(int i = 0;i<numsArrSze;i++){
    //     cout<<"element:"<<i<<": "<<numsArr[i]<<endl;
    // }
    // cout<<"the nums size is: "<<numsArrSze<<endl;

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

    int* incidentMatrix = new int[countRow*nodesCnt];
    //Make the matrix all zeros
    for(int i = 0;i<countRow*nodesCnt;i++){
        incidentMatrix[i] = 0;
    }
    
    //Create the incident matrix
    for(int i = 0;i<countRow*2;i++){
        if(i % 2 == 0 ){
            incidentMatrix[(int)(nodesCountArr[i]) * countRow + i/2] = 1; 
        }
        else{
            incidentMatrix[(int)(nodesCountArr[i]) * countRow + i/2] = -1;
        }
    } 
    
    cout<<"incidentMatrix:"<<endl; 
    for(int i = 0;i<countRow*nodesCnt;i++){
        
        cout<<incidentMatrix[i]<<" "; 
        if((i+1) % countRow == 0){
            cout<<""<<endl;
        }
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
  
    // for(int i = 0; i<n;i++){
    //     cout<<nums[i]<<endl;
    // }
    
    bubbleSort(nums,n);

    int dplctCnt = 0;
    
    
    // for(int i = 0; i<n;i++){
    //     cout<<nums[i]<<endl;
    // }
    

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
