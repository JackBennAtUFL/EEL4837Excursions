#include <fstream>
#include <iostream>
#include <string>
#include <math.h>
#include <vector>
#include <sstream>
#include <iomanip>
// #include <bits/stdc++.h> //I had a weird include path error so i commented out for now

using namespace std; 

struct logicNode
{
    ///#this will be !,+,*, or the letter input
    public:
    string net;
    char function; 
    logicNode* left; 
    logicNode* right; 

    logicNode(string n){
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

logicNode* enTree(string line){

    //create the head 
    int output = line.find("OUTPUT")-2;
    string out = line.substr(output,line.size()-output);

    cout<<out<<endl;

    logicNode*head = new logicNode(out); 

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
        head->left = new logicNode(next);
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
                head -> left = new logicNode(left);
            }
            if(spaceCount == 4){
                int point = element.find(' ', i);
                string right = element.substr(i,point-i);
                head -> right = new logicNode(right);
            }
        }
    }    
    //Inset code to constrtuct the tree 


    return head; 
}



//Splits a given string by the given delimeter
//-----***-----
vector<string> splitString(string inStr, char del){

    int splitPos;
    string tempStr = inStr;
    vector<string> output;


    do{
        splitPos = tempStr.find(del);

        output.push_back(tempStr.substr(0,splitPos));
        tempStr = tempStr.substr(splitPos+1, tempStr.length());

    } while (splitPos != string::npos); //Repeats while there is a delimeter in the string
    
    return output;

}
//-----***-----


//Returns the Net ID for the output point
//-----***-----
string getOutputNet(vector<vector<string>> & masterList){

    for(int i = 0; i < masterList.size(); i++){

        if(masterList[i][1] == "OUTPUT"){

            //Removes the item from the master list and returns the netlabel
            string outputNet = masterList[i][0];
            masterList.erase(masterList.begin()+i);
            return outputNet;
        }

    }

    return NULL;

}
//-----***-----

//Returns the string vector containing the node information from passing the net label
//-----***-----
vector<string> getNetInfo(string netName, vector<vector<string>> & masterList){

    vector<string> errorVect;
    errorVect.push_back("Not Found");

    for(int i = 0; i < masterList.size(); i++){

        if(masterList[i][0] == netName){

            //Removes the item from the master list and returns the net info
            vector<string> netInfo = masterList[i];
            masterList.erase(masterList.begin() + i);
            return netInfo;
        }

    }

    return errorVect;

}
//-----***-----

//Helper recursive function to generate nodes
//-----***-----
logicNode* genNode(vector<string> netData, vector<vector<string>> & masterList){


    //All logic runs on the assumption of corrent data placement
    // inputs: netlabel at [0] | "INPUT at [1]"
    // logic: netLabel at [0] | "=" at [1] | operation at [2] | first input to op at [3] | second input to op at [4]
    // direct assignment: netLabel at [0] | "=" at [1] | assignment net at [2]


    logicNode* head = new logicNode(netData[0]);

    //BASECASE
    if(netData[1] == "INPUT" ){

        head->left = NULL;
        head->right = NULL;
        head->function = netData[0][0]; //Assigns the character input

        return head;
    }
    else if(netData[1] == "="){
        if(netData[2] == "NOT"){
            head->function = '!';

            //Adds the NOT node to the left branch of the tree
            head->left = genNode(getNetInfo(netData[3], masterList), masterList);
            head->right = NULL;

            return head;

        }
        else if(netData[2] == "AND"){
            head->function = '*';
            //Adds the first point to the left branch and the second point to the right branch
            head->left = genNode(getNetInfo(netData[3], masterList), masterList);
            head->right = genNode(getNetInfo(netData[4], masterList), masterList);

            return head;
        }
        else if(netData[2] == "OR"){
            head->function = '+';
            //Adds the first point to the left branch and the second point to the right branch
            head->left = genNode(getNetInfo(netData[3], masterList), masterList);
            head->right = genNode(getNetInfo(netData[4], masterList), masterList);

            return head;
        }

        //If the proceeding operation is not a logic comparison (direct assignment)
        head->function = '=';

        head->left = genNode(getNetInfo(netData[2], masterList), masterList);
        head->right = NULL;

        return head;




    }

    return NULL;
    

}
//-----***-----

//Generates the logic tree given the input file
//-----***-----
logicNode* genLogicTree(string inpfile){

    //Generates the master list from the input file
    string line;
    ifstream myfile(inpfile);

    vector<vector<string>> masterList;

    while(getline(myfile,line)){
        masterList.push_back(splitString(line, ' '));
    }

    //Gets the output information to start the logic tree
    string outputNet = getOutputNet(masterList);
    vector<string> outputData = getNetInfo(outputNet, masterList);

    logicNode* treeHead = genNode(outputData, masterList);

    return treeHead;

}
//-----***-----


int main(){

    //get the string form the file 
    //string line = getText();
    //Count the number of rows

    // string line = "a INPUT b INPUT c INPUT d INPUT E OUTPUT t1 = AND a b t2 = AND c d E = OR t1 t2";

    // node *head = enTree(line);

    // outText("12");

    // string inpfile = "input.txt";

    //Handels test case 1 a bit weird
    //There are some nodes that are reused but this would increase the required number of gates so for now it just ignores
    //Repeating instances of a node 
    //ie. m5 = AND t2 t3 but m4 = AND t1 t2 earlier
    string inpfile = "TC1.txt";


    genLogicTree(inpfile);





    return 0;
}