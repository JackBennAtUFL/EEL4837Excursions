 
 
 //first copy the top left corner and top middle (always zeros) so don't actually copy 
    //Scan the first row of the disjointed big matrix
    int colCnt = 1;
    int j = 0;
    double** columnToConcatenate = gen2DArray(1,3);
    for(int ctrl = 0;ctrl<nodes+rows*2;ctrl++){
        
        //This will handle the three matricies at the top of the sparce
        if(ctrl<nodes){
            for(int i = 0;i<rows;i++){
                if(incident[ctrl][i] != 0 && j == 0){
                    sparceRepresentation[j][0]  = 0;
                    sparceRepresentation[j][1] = rows+nodes+;
                    sparceRepresentation[j][2] = incident[ctrl][i]; 
                    
                }
                else{
                    columnToConcatenate[0][0]  = 0;
                    columnToConcatenate[0][1] = rows+nodes+i;
                    columnToConcatenate[0][2] = incident[ctrl][i]; 
                    concatenateCol(sparceRepresentation,3,colCnt,columnToConcatenate,1);
                    colCnt++;
                }
            }
        }
        //This handles the center matricies A^-T, 1 and 0
        else if(ctrl>=nodes && ctrl < nodes+rows){
            for(int i = 0;i<nodes+rows;i++){
                //This will copy the transposed incident matrix
                if(i<nodes && leftMiddle[ctrl-nodes][i]!= 0){
                    columnToConcatenate[0][0]  = ctrl;
                    columnToConcatenate[0][1] = colCnt;
                    columnToConcatenate[0][2] = leftMiddle[ctrl-nodes][i];
                    concatenateCol(sparceRepresentation,3,colCnt,columnToConcatenate,1);
                    colCnt++;
                }
                 //This will copy the identity matrix
                else{
                    if(center[ctrl-nodes][i-rows]!= 0){
                        columnToConcatenate[0][0]  = ctrl;
                        columnToConcatenate[0][1] = colCnt;
                        columnToConcatenate[0][2] = center[ctrl-nodes][i-rows];
                        concatenateCol(sparceRepresentation,3,colCnt,columnToConcatenate,1);
                        colCnt++;
                    } 
                }
            }
        }
    }