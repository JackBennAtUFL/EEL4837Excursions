#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <limits>
#include <bits/stdc++.h>
#include <stdlib.h>

#define INT_MA 2147483647
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
    //Count the number of inputs at a node
    int visited = 0;
    //This flag tracks counted nodes
    bool counted = false;
    int costHere;
    logicNode(string n){
        net = n;
        costHere = INT_MA;
    }
};

//Fetch the netlist string (this works)
string getText(string inFile) {
    // Read the vector from the file
    cout<<"getting text now"<<endl;
    string line;
    string newLine = "";
    vector<char> net;
    ifstream myFile;
    myFile.open(inFile, ios::in);

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
    
    cout<<newLine<<endl;
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

/* Gradescope did not like this for some reason
//Splits a given string by the given delimeter
//-----***-----
vector<string> splitString(string inStr, char del){

    cout<<"is inStr in the room with us? "<<inStr<<endl;

    int splitPos;
    string tempStr = inStr;
    vector<string> output;

    do{
        splitPos = tempStr.find(del);
        output.push_back(tempStr.substr(0,splitPos));
        tempStr = tempStr.substr(splitPos+1, tempStr.length());

    } while (splitPos != string::npos); //Repeats while there is a delimeter in the string
    
    //cout<<"outputSize is"<<output.size()<<endl;

    for(int i =0;i<output.size();i++){
        cout<<"split is "<<output[i]<<endl;
    }
    return output;

}
-----***-----
*/

//Returns the Net ID for the output point
//-----***-----
string getOutputNet(vector<vector<string>> &masterList){

    for(int i = 0; i < masterList.size(); i++){

        cout<<masterList[i][1]<<endl;
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

            cout<<netInfo.size()<<" net size was"<<endl;
            
            for(int i = 0;i<netInfo.size();i++){
                cout<<netInfo[i]<<" ";
            }
            cout<<"catchSp"<<endl;

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

    cout<<netData.size()<<" net size was"<<endl;
            
            for(int i = 0;i<netData.size();i++){
                cout<<netData[i]<<" ";
            }
            cout<<"catchSp"<<endl;

    logicNode* head = new logicNode(netData[0]);

    if(netData.size() == 1){
        cout<<"data size = 1"<<endl;

        head->left = nullptr;
        head->right = nullptr;
        head->function = netData[0][0];
        return head;
    }

    //BASECASE
    if(netData[1] == "INPUT" ){

        head->left = nullptr;
        head->right = nullptr;
        head->function = netData[0][0]; //Assigns the character input

        return head;
    }
    else if(netData[1] == "="){
        cout<<"found an = "<<endl;

        cout<<"net 2 is "<<netData[2]<<endl;

        if(netData[2] == "NOT"){
            head->function = '!';

            //Adds the NOT logicNode to the left branch of the tree
            cout<<"is theis failing here???"<<endl;
            head->left = genlogicNode(getNetInfo(netData[3], masterList), masterList);
           
            head->right = nullptr;

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
            
            cout<<"is it going here"; 
            head->function = '+';
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
    return nullptr;
}
//-----***-----

//Generates the logic tree given the input file
//-----***-----
logicNode* genLogicTree(string fileName){

    //Generates the master list from the input file
    string line;
    ifstream myfile;
    
    string text = getText(fileName);

    //Take the first row because it has to be input this gives us a reliable vector to beat segment fault
    vector<vector<string>> masterList = {{text.substr(0,1),text.substr(2,5)}};

    //takes care of the inputs
    for(int i = 8;i<text.find("OUTPUT")-2;i++){
        
        string tempStr1 = text.substr(i,1);
        string tempStr2; 
        vector<string> row; 
        
        row.push_back(text.substr(i,1));
        i+=2;
        while (text[i] != ' ')
        {
            tempStr2.push_back(text[i]);
            i++;
        }
        row.push_back(tempStr2);

        masterList.push_back(row);
    }

    //Take care of the output
    vector<string> row1; 
    string keyVal = text.substr(text.find("OUTPUT")-2,1);
    row1.push_back(keyVal);
    row1.push_back(text.substr(text.find("OUTPUT"),6));
    masterList.push_back(row1);

    //cout<<"key value is"<<keyVal<<"5"<<endl;

    //now do the logic gates
    for(int i = text.find("OUTPUT")+7;i<text.length();i++){
    

        string nodeName;
        vector<string> row; 

        if(text[i+1] != ' '){
            nodeName = text.substr(i,2);
            cout<<nodeName<<endl;
            row.push_back(nodeName);
            i+=3;
        }
        else{
            cout<<"does it go here"<<endl;
            nodeName = text.substr(i,1);
            
            cout<<nodeName<<endl;
            row.push_back(nodeName);
            i+=2;
        }
        
        string function;
        string in1;
        string in2;
        
        vector<string> rowKey;
 
        cout<<"this value: "<<text[i]<<endl;
        //key value output at the end         
        if(text.substr(i,1) == keyVal){
            cout<<"found key"<<endl;

            rowKey.push_back(text.substr(i,1));

           
            //push the equals
            i+=2;
            rowKey.push_back(text.substr(i,1));
            i+=2;
            
             //if there is a NOT gate
            if(text[i] == 'N'){
                rowKey.push_back("NOT");
                i+= 4;

                while(text[i] != ' '){
                    in1.push_back(text[i++]);
                }
                rowKey.push_back(in1);
            
                masterList.push_back(rowKey);

            }
            else if(text[i] == 'A'){
                
                rowKey.push_back("AND");
                i+= 4;
                while(text[i] != ' '){
                    in1.push_back(text[i++]);
                }
                i++;
                while(text[i] != ' '){
                    in2.push_back(text[i++]);
                }
                
                rowKey.push_back(in1);
                rowKey.push_back(in2);

                masterList.push_back(rowKey);

            }
            else if(text[i] == 'O'){
                
                rowKey.push_back("OR");
                i+= 3;
                while(text[i] != ' '){
                    in1.push_back(text[i]);
                    i++;
                }
                i++;
                while(text[i] != ' '){
                    in2.push_back(text[i++]);
                }
                
                rowKey.push_back(in1);
                rowKey.push_back(in2);

                masterList.push_back(rowKey);
                
            }
        

            cout<<"Text is"<<text[i];
            break;
        }
        
        //node name a = AND x z
        //          ^

        cout<<"end up here"<<endl;

        //push the equals
        row.push_back(text.substr(i,1));

        i+=2;
    
        //if there is a NOT gate
        if(text[i] == 'N'){
            row.push_back("NOT");
            i+= 4;

            while(text[i] != ' '){
                in1.push_back(text[i++]);
            }
            row.push_back(in1);
            
            masterList.push_back(row);
        }
        else if(text[i] == 'A'){
            
            row.push_back("AND");
            i+= 4;
            while(text[i] != ' '){
                in1.push_back(text[i++]);
            }
            i++;
            while(text[i] != ' '){
                in2.push_back(text[i++]);
            }
            
            row.push_back(in1);
            row.push_back(in2);

            masterList.push_back(row);
        }
        else if(text[i] == 'O'){
            row.push_back("OR");
            i+= 3;
            while(text[i] != ' '){
                in1.push_back(text[i]);
                i++;
            }
            i++;
            while(text[i] != ' '){
                in2.push_back(text[i++]);
            }
            
            row.push_back(in1);
            row.push_back(in2);

            masterList.push_back(row);
        }

    }


    string lastRow;
    vector<string> end;
    if(masterList[masterList.size()-1][0] != keyVal){
        cout<<"go here"<<endl;
        end.push_back(keyVal);
        end.push_back("=");


        for(int i = text.length()-2;1;i--){
            if(text[i] == ' '){
                break;
            }
            lastRow.push_back(text[i]);
            cout<<"last row is"<<lastRow<<endl;
        }
        reverse(lastRow.begin(),lastRow.end());

        end.push_back(lastRow);
        masterList.push_back(end);

    }

    cout<<"print master List"<<endl;
    cout<<masterList.size()<<endl;
    for(int i = 0;i < masterList.size(); i++){
            
        cout<<"row size is: "<<masterList[i].size()<<" ";
        for(int j = 0;j < masterList[i].size(); j++){
            cout<<masterList[i][j]<<" ";
        }
        cout<<endl;
    }

    string outputNet;
    //Gets the output information to start the logic tree
    outputNet = getOutputNet(masterList);

    cout<<"OUtput net is"<<endl<<outputNet[0]<<" "<<outputNet[1]<<" "<<outputNet[2]<<endl<<endl;

    vector<string> outputData = getNetInfo(outputNet, masterList);

    logicNode* treeHead = genlogicNode(outputData, masterList);

    cout<<treeHead->function;

    return treeHead;
}
//-----***-----

//Convert the logic tree to NAND-NOT 
/*
examples
----\          ---NOT---\ 
     OR---  =          NAND---
----/          ---NOT---/ 
examples
----\          ----\ 
     AND--- =     NAND----NOT---   NAND2 will be represented as '@'
----/          ----/ 
*/
//works
void convertGate(logicNode* &root){
    
    if(root->function == '*'){
        root->visited += 1;
        cout<<"convert AND "<<root->net<<endl; 
        //The AND logicNode becomes a not logicNode
        root->function = '!';

        //The name should not matter at this point
        logicNode* nandInsert = new logicNode(root->net+"NAND");

        //This calls it nand2 
        nandInsert->function = '@';
        nandInsert->visited = 1;
        //NAND now points to what AND inputs were
        nandInsert->left = root->left;
        nandInsert->right = root->right;

        root->left = nandInsert;
        //The right input to a not gate does not exist
        root->right  = nullptr; 
    }

    //convert AND gates
    else if(root->function == '+'){

        root->visited += 1;
        cout<<"convert OR"<<root->net<<endl; 
        //The OR logicNode becomes a NAND2
        root->function = '@';

        //Add not gates
        logicNode* not1 = new logicNode(root->net+"not1");
        not1->function = '!';
        not1->left = root->left;
        not1->right  = nullptr;
        root->left = not1;
        not1->visited = 1;

        logicNode* not2 = new logicNode(root->net+"not2");
        not2->function = '!';
        not2->left = root->right;
        not2->right  = nullptr;
        root->right = not2;
        not2->visited = 1;
        
    }
    //Don't do anything, no conversion needed, here for completeness 
    else if(root->function == '!'){
        root->visited += 1;
        cout<<"skipping not gate"<<endl;
    }
    else{
        cout<<"skipped ="<<endl;
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

int optimizeLogic(logicNode* root){
    
    //don't double count stuff
    // if(root->visited == 2 && root->counted == true){
    //     return root->costHere;
    // }

    //change visited flag
    if(root->counted == false){
        root->counted == true;
    }
    //Return 0 if you get to the end
    if(root == nullptr){
        return 0;
    }
    if(root->left == nullptr && root->right == nullptr){
        //cout<<root->net<<endl;
        return 0;
    }

    cout<<root->function<<root->net<<endl;

    //Return minimum value at gave if already calculated invalid if INT_MA
    if(root->costHere != INT_MA){
        return root->costHere;
    }

    int cost[7];

    //use INT MAX to avoid invalid paths
    for (int i = 0; i < 7; i++) cost[i] = INT_MA;
    
    //now check for match the cost 
    //the best result will be taken at each stage going up
    
    //NOT
    if(root->function == '='){
        optimizeLogic(root->left);
    }
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
    
    int localMin = INT_MA; 
    for(int i = 0;i<7;i++){
        if(cost[i] < localMin) localMin = cost[i];
    }
    
    outText(to_string(localMin));

    return localMin;
}

int main(){


    //Handels test case 1 a bit weird
    //There are some logicNodes that are reused but this would increase the required number of gates so for now it just ignores
    //Repeating instances of a logicNode 
    //ie. m5 = AND t2 t3 but m4 = AND t1 t2 earlier
    //string inpfile = "TC1.txt";

    logicNode* head = genLogicTree("input.txt");

    cout<<endl<<"now print the tree"<<endl;


    printTree(head);

    cout<<"now convert"<<endl<<endl<<endl<<endl<<endl;

    convertToNandNot(head);

    cout<<"post convert"<<endl;

    printTree(head);

    int result = optimizeLogic(head);

    cout<<"the cost is: "<<endl;
   
    outText(to_string(result));

    return 0;
}