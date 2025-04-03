#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <limits>
#include <stdlib.h>

#define INT_MAX 2147483647
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
    int costHere;
    logicNode(string n){
        net = n;
        costHere = INT_MAX;
    }
};

//Create an empty 2d vector with sizes
vector<vector<double>> create2dVec(int row, int col){
    return vector<vector<double>>(row, vector<double>(col,0.0));
}

//Fetch the netlist string (this works)
string getText() {
    // Read the vector from the file
    cout<<"getting text now"<<endl;
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

//Splits a given string by the given delimeter
//-----***-----
vector<string> splitString(string inStr, char del){

    cout<<"is inStr in the room with us? "<<inStr<<endl;

    int splitPos;
    string tempStr = inStr;
    vector<string> output;


    do{
        splitPos = tempStr.find(del);

        cout<<"why is auto not wroking here"<<tempStr.substr(0,splitPos)<<endl;

        output.push_back(tempStr.substr(0,splitPos));
        tempStr = tempStr.substr(splitPos+1, tempStr.length());

    } while (splitPos != string::npos); //Repeats while there is a delimeter in the string
    
    //cout<<"outputSize is"<<output.size()<<endl;

    for(int i =0;i<output.size();i++){
        cout<<"split is"<<output[i]<<endl;
    }
    return output;

}
//-----***-----


//Returns the Net ID for the output point
//-----***-----
string getOutputNet(vector<vector<string>> &masterList){

    for(int i = 0; i < masterList.size(); i++){

        if(masterList[i][1] == "OUTPUT"){
            
            //Removes the item from the master list and returns the netlabel
            string outputNet = masterList[i][0];
            masterList.erase(masterList.begin()+i);
            return outputNet;
        }
    }

    return "segmentation fault? Trivago";

}
//-----***-----

//Returns the string vector containing the logicNode information from passing the net label
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

//Helper recursive function to generate logicNodes
//-----***-----
logicNode* genlogicNode(vector<string> netData, vector<vector<string>> & masterList){


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

            //Adds the NOT logicNode to the left branch of the tree
            head->left = genlogicNode(getNetInfo(netData[3], masterList), masterList);
            head->right = NULL;

            return head;

        }
        else if(netData[2] == "AND"){
            head->function = '*';
            //Adds the first point to the left branch and the second point to the right branch
            head->left = genlogicNode(getNetInfo(netData[3], masterList), masterList);
            head->right = genlogicNode(getNetInfo(netData[4], masterList), masterList);

            return head;
        }
        else if(netData[2] == "OR"){
            head->function = '+';
            //Adds the first point to the left branch and the second point to the right branch
            head->left = genlogicNode(getNetInfo(netData[3], masterList), masterList);
            head->right = genlogicNode(getNetInfo(netData[4], masterList), masterList);

            return head;
        }

        //If the proceeding operation is not a logic comparison (direct assignment)
        head->function = '=';

        head->left = genlogicNode(getNetInfo(netData[2], masterList), masterList);
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
    ifstream myfile;
    
    vector<vector<string>> masterList;

    myfile.open(inpfile, ios::in);

    while(getline(myfile,line)){
        masterList.push_back(splitString(line, ' '));
        
        cout<<"print current master list"<<endl;
        for(int i = 0; i<masterList.size();i++){
            for(int j = 0; j<masterList[0].size();j++){
            
                cout<<masterList[i][j]<<" ";
            }   
            cout<<endl;     
        }
    }

    cout<<masterList.size()<<endl;
    for(int i = 0;i < masterList.size(); i++){
        for(int j = 0;j < masterList[0].size(); j++){
            cout<<masterList[i][j]<<" ";
        }
        cout<<endl;
    }

    string outputNet;
    //Gets the output information to start the logic tree
    outputNet = getOutputNet(masterList);

    cout<<endl<<endl<<"get here"<<endl<<outputNet<<endl<<endl;

    vector<string> outputData = getNetInfo(outputNet, masterList);

    logicNode* treeHead = genlogicNode(outputData, masterList);

    return treeHead;
}
//-----***-----


//Convert the logic tree to NAND-NOT 

/*
example
----\          ----NOT----\ 
     OR ---  =              NAND -----
----/          ----NOT----/ 

----\          ----\ 
     AND ---  =     NAND ----NOT------   NAND2 will be represented as '@'
----/          ----/ 

----NOT---- = ----NOT---- 
*/
//works
void convertGate(logicNode* &root){
    
    if(root->function == '*'){

        cout<<"convert AND "<<root->net<<endl; 
        //The AND logicNode becomes a not logicNode
        root->function = '!';

        //The name should not matter at this point
        logicNode* nandInsert = new logicNode(root->net+"NAND");

        //This calls it nand2 
        nandInsert->function = '@';
        

        //NAND now points to what AND inputs were
        nandInsert->left = root->left;
        nandInsert->right = root->right;

        root->left = nandInsert;
        //The right input to a not gate does not exist
        root->right  = nullptr; 

    }

    //convert AND gates
    else if(root->function == '+'){
        cout<<"convert OR"<<root->net<<endl; 
        //The OR logicNode becomes a NAND2
        root->function = '@';

        //The name should not matter at this point
        logicNode* not1 = new logicNode(root->net+"not1");
        not1->function = '!';
        not1->left = root->left;
        not1->right  = nullptr;
        root->left = not1;


        logicNode* not2 = new logicNode(root->net+"not2");
        not2->function = '!';
        not2->left = root->right;
        not2->right  = nullptr;
        
        root->right = not2;

        //This assigns the right function to not1 and not2
        
    

        //NOTs before the NAND now points to what AND inputs were going down
        
        
        
        //repoint the root after reassigning it's function 
        
    }
    //Don't do anything, no conversion needed, here for completeness 
    else if(root->function == '!'){
        cout<<"skipping not gate"<<endl;
    }
    return;
}


//Recursivly convert the tree
void convertToNandNot(logicNode* root){
    
    if(root == nullptr){
        cout<<"retuning"<<endl;
        return;
    }

    if((root->left == nullptr && root->right == nullptr)){
        
        cout<<"returning is: "<<root->net<<endl;
        return; 
    }

    convertToNandNot(root->left);
    cout<<"just converted: "<<root->net<<endl;
    convertToNandNot(root->right); 
    convertGate(root);
    return;
}


void printTree(logicNode* root){
    if(root == nullptr){
        return;
    }
    if((root->left == nullptr && root->right == nullptr)){
        cout<<root->net<<endl;
        return; 
    }

    cout<<"logicNode name: "<<root->net<<"function: "<<root->function<<endl;

    printTree(root->left);   
    printTree(root->right);
    
}

//This is paired with remove2Not 
int getCost(char symbol){
    switch(symbol) {
        //NOT
        case '!':
            return 2;
        //NAND2
        case '@':
            return 3;
        //AND2
        case '*':
            return 4;
        //NOR2
        case 'x':
            return 6;
        //OR2
        case '+':
            return 4;
        //AOI21
        case '.':
            return 7;
        //AOI22
        case '$':
            return 7;
        default:
            return 0; // Return 0 for inputs or empty
    }

}

int computeCost(logicNode* head){
    if(head == nullptr){
        return 0;
    }
    cout<<"function is"<<head->function<<endl;
    return getCost(head->function) + computeCost(head->left) + computeCost(head->right);
}

int optimizeLogic(logicNode* root){
    //Return 0 if you get to the end
    if(root == nullptr){
        return 0;
    }
    if(root->left == nullptr && root->right == nullptr){
        return 0;
    }

    //Return value if already calculated invalid if INT_MAX
    if(root->costHere != INT_MAX){
        return root->costHere;
    }

    int cost[7];

    //use INT MAX to avoid invalid paths
    for (int i = 0; i < 7; i++) cost[i] = INT_MAX;
    
    //now check for match the cost 
    //the best result will be taken at each stage going up
    
    //NOT
    if(root->function == '!'){
       cost[0] = 2+optimizeLogic(root->left);

       //Check AND2
        if(root->left->function == '@'){
            cost[2] = 4+optimizeLogic(root->left->left) + optimizeLogic(root->left->right);

            //Check NOR2
            if(root->left->left->function == '!' && root->left->right->function == '!'){
                cost[3] = 6+optimizeLogic(root->left->left->left) + optimizeLogic(root->left->right->left);
            }

            //Check AOI21 NAND on the left
            if(root->left->left->function == '@' && root->left->right->function == '!'){
                cost[5] = 7+optimizeLogic(root->left->left->left) + optimizeLogic(root->left->left->right)  + optimizeLogic(root->left->right->left);
            }
            //Check AOI21 NAND on the right - these 2 are mutally exclusive
            if(root->left->left->function == '!' && root->left->right->function == '@'){
                cost[5] = 7+optimizeLogic(root->left->right->left) + optimizeLogic(root->left->right->right)  + optimizeLogic(root->left->left->left);
            }

            //Check for AIOLI (AOI22)
            if(root->left->left->function == '@' && root->left->right->function == '@'){
                cost[6] = 7+optimizeLogic(root->left->right->left) + optimizeLogic(root->left->right->right)  + optimizeLogic(root->left->left->left) + optimizeLogic(root->left->right);
            }
        }
          
    }
    //NAND2 then check OR2 from there
    else if(root->function == '@'){
        cost[1] = 3 + optimizeLogic(root->left) + optimizeLogic(root->right);

        //OR2
        if(root->left->function == '!' && root->right->function == '!'){
            cost[4] = 4+optimizeLogic(root->left->left) + optimizeLogic(root->right->left);
        }
    }
    
    int localMin = INT_MAX; 
    for(int i = 0;i<7;i++){
        if(cost[i] < localMin) localMin = cost[i];
    }
    
    outText(to_string(localMin));

    return localMin;
}

int main(){

    string inpfile = "input.txt";

    //Handels test case 1 a bit weird
    //There are some logicNodes that are reused but this would increase the required number of gates so for now it just ignores
    //Repeating instances of a logicNode 
    //ie. m5 = AND t2 t3 but m4 = AND t1 t2 earlier
    //string inpfile = "TC1.txt";

    logicNode* head = genLogicTree(inpfile);

    printTree(head);

    cout<<"now convert"<<endl;

    convertToNandNot(head);

    cout<<"post convert"<<endl;

    printTree(head);

    cout<<"remove all double negatives"<<endl;

    //Eliminate double negatives because it always improves result (or not) but leave for now
    //remove2Not(head);
    //printTree(head);

    cout<<"the cost is: "<<optimizeLogic(head);

    return 0;
}