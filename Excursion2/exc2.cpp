#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <bits/stdc++.h>

//Autograder does not like INT_MAX
#define INT_MA 2147483647
using namespace std; 

struct logicNode
{
    ///#this will be !,+,*, or the letter input
    string net;
    char function; 
    logicNode* left; 
    logicNode* right; 
    //Count the number of inputs at a node
    logicNode(string n){
        net = n;
    }
};

//Fetch the netlist string (this works)
string getText(string inFile) {
    // Read the vector from the file
    string line;
    string newLine = "";
    vector<char> net;
    ifstream myFile;
    myFile.open(inFile, ios::in);

    while (getline(myFile, line)) {
        for (char c : line) net.push_back(c);
        net.push_back(' ');
    }
       
    //pass stuff through a vector because autograder madness
    vector<char> fixedNet;
    for (char c : net) {
        if (c != '\r')  fixedNet.push_back(c); 
    }
    myFile.close();
        
    for(int i = 0;i<fixedNet.size();i++) newLine += fixedNet[i];
    
    return newLine;
}

//Write to the output text file
void outText(string outputStr){
    //write out the results
    ofstream myFile;
    myFile.open("output.txt",ios::out); 
    myFile<<outputStr<<endl; 
    myFile.close();
    return; 
}

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
    //this is useless and is only to prevent errors?
    return "failed to find";
}
//-----***-----

//Returns the string vector containing the logicNode information from passing the net label
//-----***-----
vector<string> getNetInfo(string netName, vector<vector<string>> & masterList){

    vector<string> errorVect = {netName};

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
    if( netData.size() == 1 || netData[1] == "INPUT"){
        head->left = nullptr;
        head->right = nullptr;
        head->function = netData[0][0]; //Assigns the character input
        return head;
    }
    else if(netData[2] == "NOT"){
        head->function = '!';

        //Adds the NOT logicNode to the left branch of the tree
        head->left = genlogicNode(getNetInfo(netData[3], masterList), masterList);
        head->right = nullptr;
        return head;
    }
    else if(netData[2] == "AND" || (netData[2] == "OR") ){
        if(netData[2] == "AND") head->function = '*';
        else  head->function = '+';
        //Adds the first point to the left branch and the second point to the right branch
        head->left = genlogicNode(getNetInfo(netData[3], masterList), masterList);
        head->right = genlogicNode(getNetInfo(netData[4], masterList), masterList);
        return head;
    }
    //If the proceeding operation is not a logic comparison (direct assignment)
    head->function = '=';

    head->left = genlogicNode(getNetInfo(netData[2], masterList), masterList);
    head->right = nullptr;

    return head;
}
//-----***-----

//Generates the logic tree given the input file
//-----***-----
logicNode* genLogicTree(string fileName){

    //Generates the master list from the input file
    ifstream myfile;
    
    string text = getText(fileName);

    //Take the first row because it has to be input this gives us a reliable vector to beat segment fault
    vector<vector<string>> masterList = {{text.substr(0,1),text.substr(2,5)}};

    //takes care of the inputs
    for(int i = 8;i<text.find("OUTPUT")-2;i++){
        string tempStr; 
        vector<string> row; 
        
        row.push_back(text.substr(i,1));
        i+=2;
        while (text[i] != ' ') tempStr.push_back(text[i++]);

        row.push_back(tempStr);
        masterList.push_back(row);
    }

    //Take care of the output row of the text file
    vector<string> row1; 
    string keyVal = text.substr(text.find("OUTPUT")-2,1);
    row1.push_back(keyVal);
    row1.push_back("OUTPUT");
    masterList.push_back(row1);

    //now do the logic gates
    for(int i = text.find("OUTPUT")+7;i<text.length();i++){

        string nodeName;
        vector<string> row; 

        //is the net list 1 or 2 charecters long?
        if(text[i+1] != ' ') nodeName = text.substr(i++,2);
        else nodeName = text.substr(i,1);
        
        row.push_back(nodeName);
        i+=2;
        
        string function;
        string in1;
        string in2;
        //Go here once you find the key value, which is the output
        //key value output at the end         
        row.push_back(text.substr(i,1));    
        
        i+=2;
        //if there is a NOT gate
        if(text[i] == 'N'){
            row.push_back("NOT");
            i+=4;
            while(text[i] != ' ') in1.push_back(text[i++]);

            row.push_back(in1);
            masterList.push_back(row);
        }
        else{ 
            if(text[i] == 'A' || text[i] == 'O'){
                if(text[i] == 'A'){
                    row.push_back("AND");
                    i++;
                }
                else row.push_back("OR");
                i+=3;

                while(text[i] != ' ') in1.push_back(text[i++]);
                i++;
                while(text[i] != ' ') in2.push_back(text[i++]);
                
                row.push_back(in1);
                row.push_back(in2);
                masterList.push_back(row);
            }
        }
    }

    //This deals with direct assignment in the last row
    string lastRow;
    vector<string> end;
    if(masterList[masterList.size()-1][0] != keyVal){
        end.push_back(keyVal);
        end.push_back("=");

        for(int i = text.length()-2;1;i--){
            if(text[i] == ' ') break;
            lastRow.push_back(text[i]);
        }
        //deal with scanning backwards
        reverse(lastRow.begin(),lastRow.end());

        end.push_back(lastRow);
        masterList.push_back(end);
    }
    //Gets the output information to start the logic tree and generate the tree
    return genlogicNode(getNetInfo(getOutputNet(masterList), masterList), masterList);
}
//-----***-----

//Convert the logic tree to NAND-NOT 
/*
examples NAND2 becomes @
----\          ----\                 ----\          ---NOT---\ 
     AND--- =     NAND----NOT---          OR---  =          NAND---
----/          ----/                 ----/          ---NOT---/ 
*/
void convertGate(logicNode* &root){
    
    if(root->function == '*'){
        //The AND logicNode becomes a not logicNode
        root->function = '!';

        //The name should not matter at this point
        logicNode* nandInsert = new logicNode(root->net+"NAND");

        //This calls it nand2 NAND now points to what AND inputs were
        nandInsert->function = '@';
        nandInsert->left = root->left;
        nandInsert->right = root->right;
        root->left = nandInsert;
        //The right input to a not gate does not exist
        root->right  = nullptr; 
    }
    //convert AND gates
    else if(root->function == '+'){
        //The OR logicNode becomes a NAND2
        root->function = '@';
        
        //Add not gates
        logicNode* not1 = new logicNode(root->net+"n1");
        not1->function = '!';
        not1->left = root->left;
        not1->right  = nullptr;
        root->left = not1;

        logicNode* not2 = new logicNode(root->net+"n2");
        not2->function = '!';
        not2->left = root->right;
        not2->right = nullptr;
        root->right = not2;
    }
    return;
}

//Recursivly convert the tree to NAND and NOT
void convertToNandNot(logicNode* root){
    
    if(root == nullptr  || (root->left == nullptr && root->right == nullptr)) return;
    convertToNandNot(root->left);
    convertToNandNot(root->right); 
    convertGate(root);
    return;
}

int optimizeLogic(logicNode* root){
    
    //Return 0 if you get to the end
    if(root == nullptr || (root->left == nullptr && root->right == nullptr)) return 0;
 
    //use INT MAX to avoid invalid paths
    int cost[8] = {INT_MA,INT_MA,INT_MA,INT_MA,INT_MA,INT_MA,INT_MA,INT_MA};
    
    //now check for match the cost the best result will be taken at each stage going up
    //Deal with direct assignment
    if(root->function == '=') cost[7] = optimizeLogic(root->left);
    
    ////NOT
    else if(root->function == '!'){
       cost[0] = 2+optimizeLogic(root->left);

       //Check AND2
        if(root->left->function == '@'){
            cost[2] = 4+optimizeLogic(root->left->left) + optimizeLogic(root->left->right);

            //Check NOR2
            if(root->left->left->function == '!' && root->left->right->function == '!' ){
                cost[3] = 6+optimizeLogic(root->left->left->left) + optimizeLogic(root->left->right->left);
            }
            //Check AOI21 NAND on the left
            if(root->left->left->function == '@' && root->left->right->function == '!' ){
                cost[5] = 7+optimizeLogic(root->left->left->left) + optimizeLogic(root->left->left->right)  + optimizeLogic(root->left->right->left);
            }
            //Check AOI21 NAND on the right - these 2 are mutally exclusive
            if(root->left->left->function == '!' && root->left->right->function == '@' ){
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
    
    //calculate the local min by default the min is INT_MAX
    int localMin = INT_MA; 
    for(int i = 0;i<8;i++){
        if(cost[i] < localMin) localMin = cost[i];
    }
    
    outText(to_string(localMin));

    return localMin;
}
//such a nice main function 
int main(){
    //There are some logicNodes that are reused but this would increase the required number of gates so for now it just ignores Repeating instances of a logicNode 
    logicNode* head = genLogicTree("input.txt");
    convertToNandNot(head);
    optimizeLogic(head);
    return 0;
}