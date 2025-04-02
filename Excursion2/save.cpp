
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
void convertGate(node* &root){
    
    if(root->function == '*'){

        cout<<"convert AND "<<root->net<<endl; 
        //The AND node becomes a not node
        root->function = '!';

        //The name should not matter at this point
        node* nandInsert = new node(root->net+"NAND");

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
        //The OR node becomes a NAND2
        root->function = '@';

        //The name should not matter at this point
        node* not1 = new node(root->net+"not1");
        node* not2 = new node(root->net+"not2");

        //This assigns the right function to not1 and not2
        not1->function = '!';
        not2->function = '!';

        //NOTs before the NAND now points to what AND inputs were going down
        not1->left = root->left;
        not2->left = root->right;
        
        //repoint the root after reassigning it's function 
        root->left = not1;
        root->right = not2;
    }
    //Don't do anything, no conversion needed, here for completeness 
    else if(root->function == '!'){
        cout<<"skipping not gate"<<endl;
    }
    return;
}


//Recursivly convert the tree
void convertToNandNot(node* root){
    
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


void printTree(node* root){
    if(root == nullptr){
        return;
    }
    if((root->left == nullptr && root->right == nullptr)){
        cout<<root->net<<endl;
        return; 
    }

    cout<<"node name: "<<root->net<<"function: "<<root->function<<endl;


    printTree(root->left);
   
    printTree(root->right);
    
}