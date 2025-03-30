#include <fstream>
#include <iostream>
#include <string>
#include <math.h>
#include <vector>
#include <sstream>
#include <iomanip>
#include <bits/stdc++.h>

using namespace std; 

class node
{
    ///#this will be !,+,*, or the letter input
    public:
    string net;
    char function; 
    node* left; 
    node* right; 

    node(string n){
        net = n;
    }
};

//Create an empty 2d vector with sizes
vector<vector<double>> create2dVec(int row, int col){
    return vector<vector<double>>(row, vector<double>(col,0.0));
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

node* enTree(string line){

    //create the head 
    int output = line.find("OUTPUT")-2;
    string out = line.substr(output,line.size()-output);

    cout<<out<<endl;

    node*head = new node(out); 

    head->net = out;

    //Find the 2nd occurance of the output in the string
    bool found1st = false;
    int outGatePos = 0; 
    for(int  i = 0; i <line.size();i++){
        if(line[i] == out[0] && !found1st){
            found1st = true;
        }
        else if(line[i] == out[0] && found1st){
            outGatePos = i; 
            cout<<"does go here?"<<i<<endl;
            break;
        }
        cout<<"i is:"<<i<<endl;
    }

    string element = line.substr(outGatePos,line.size() - outGatePos); 
    cout<<outGatePos<<"element is"<<element; 

    //This block does the head, there are two ways you can do the head, it may
    //equal a logic gate or it could be a variable name 

    //check to see if out put is a gate or connected to one (F = t1)
    if(element.find("NOT") == string::npos && element.find("OR") == string::npos &&  element.find("AND") == string::npos){
        head->function = '=';
        string next = element.substr(4,2); 
        head->left = new node(next);
    }
    //go here if F = AND t2 t3 for example  
    else{
        if(element.find("NOT") != string::npos){
            head->function = '!';
        }
        else if(element.find("AND") != string::npos){
            head->function = '*';
        }
        else if(element.find("OR") != string::npos){
            head->function = '+';
        }

        int spaceCount = 0;
        bool foundL = false;
        for(int i = 0;i<element.size();i++){
            if(element[i] == ' '){
                spaceCount +=1; 
            }
            if(spaceCount == 3 && !foundL){
                foundL = true;
                i++;
                int point = element.find(' ', i);
                string left = element.substr(i,point-i);
                head -> left = new node(left);
            }
            if(spaceCount == 4){
                int point = element.find(' ', i);
                string right = element.substr(i,point-i);
                head -> right = new node(right);
            }
        }
    }    
    //Inset code to constrtuct the tree 


    return head; 
}

int main(){

    //get the string form the file 
    //string line = getText();
    //Count the number of rows

    string line = "a INPUT b INPUT c INPUT d INPUT E OUTPUT t1 = AND a b t2 = AND c d E = OR t1 t2";

    node *head = enTree(line);

    outText("12");

    return 0;
}