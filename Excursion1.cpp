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

int main(){

    //get the string form the file 
    string line = getText("netlist.txt");
    
    //Parse the netlist here because putting in functions will cause massive problems
    //With the returned arrays

    //Count the number of rows
    int countRow = 0;
    
    for(int i = 0; i<line.length();i++){
        if(isalpha(line[i])){
            countRow++; 
        }
    }

    //Create a netlist with an order
    vector<string> elementLabels(countRow,"X");
    
    //Create a dynamic array for the node labels and elements 
    double* numsArr = new double[countRow*3];

    int lmtpointer = 0;
    int valPtr = 0;
    for(int i = 0; i<line.length();i++){
        string temp; 
        
        int j = 0;
        if(isalpha(line[i])){
            //Copy the letter either V or R
            temp += line[i++];
            j++;

            //Get the element labels v1,r1,r2 etc
            while ((isdigit(line[i])||ispunct(line[i])) &&  i<line.length()){
                temp += line[i++];
                j++;
            }
            elementLabels[lmtpointer++] = temp;
            
           

            //Go to the next number 
            while(line[i] == ' ' && i<line.length()){
                i++;
            }

            //Get elements until you hit the end of the "row" 
            while(line[i] != ',' && i<line.length()){
                string tempNum;
                
                //cout<<"test1"<<endl;
                //cout<<line[i]<<endl;
                
                while ((isdigit(line[i])||line[i] == '.') &&  i<line.length()){
                    tempNum += line[i++];  
                    
                }
                //cout<<tempNum<<endl;
                numsArr[valPtr++] = stod(tempNum);
                
                while(line[i] == ' '){
                    i++;
                }
            }
        }
    }
    
    /*looking at the string vector, 
    for(int i = 0;i<countRow;i++){
        cout<<elementLabels[i]<<endl;
    }
    
    for(int i = 0;i<sizeof(numsArr)+1;i++){
        cout<<"element:"<<numsArr[i]<<endl;
    }*/

    //Branches = rowCount

    double* nodesCountArr = new double[countRow*2];

    int j = 0;
    for(int i = 0; i<sizeof(numsArr)+1;i++){
        if( (i+1) % 3 != 0){
            nodesCountArr[j] = numsArr[i];
            j++;
        }
    }
    int nodesCnt = deduplicate(nodesCountArr,countRow*2);

    cout<<"what?"<<nodesCnt;
    
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

//These are used to find the number unique nodes
int deduplicate(double nums[], int n ){
  
    bubbleSort(nums,n);

    int dplctCnt = 0;
    
    //Count the duplicates
    for(int i = 0; i<n-1;i++){
        if(nums[i] != nums[i-1]){ 
            int j = 1+i;
            while(nums[i] == nums[j] && j<n){
                dplctCnt +=1;   
                j++;
            }
        }
    }

    //cout<<"dplctCnt"<<dplctCnt<<endl;
    //cout<<"n is: "<<n<<endl;
    //Create the new array to copy to and determine new size
    int size = n-dplctCnt;
    double newArr[size];


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

