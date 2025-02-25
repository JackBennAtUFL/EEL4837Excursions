now do reduced row, divide the diagonals and get rref
    for(int i = 0;i<CSC[0].size();i++){
        rowFactor = CSC[2][i];  
        for(int j = i;j<CSC[0].size();j++){
            if(CSC[0][j] != CSC[0][j]){
                i = j;
                break;  
            }
            CSC[2][j] = CSC[2][j]/rowFactor; 
        }
          
    }