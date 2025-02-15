//This is where we will write stuff 

#include <fstream>
#include <iostream>
#include <string>
#include <math.h>

using namespace std; 

string getText(string fileName); 

int main(){

    //get the string form the file 
    string line = getText("netlist.txt");
    
    //Parse the netlist here because putting in functions will cause massive problems
    //With the returned arrays


    int countRow = 0;
    for(int i = 0; i<line.length();i++){
        if(isalpha(line[i])){
            countRow += 1;
        }
    }
    //Create a dynamic array to copy input string to 
    char* destArray = new char[countRow*4];

    


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
