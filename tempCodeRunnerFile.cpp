int countNodes(double nums[], int n){
    //Sort the nums array so you can count uniques
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
  
    

    int dplctCnt = 0;
    //Count the duplicates
    for(int i = 0; i<n-1;i++){
        if((int)nums[i] != (int)nums[i-1]){ 
            int j = 1+i;
            int tempDupCount = 0; 
            while((int)nums[i] == (int)nums[j] && j<n){
                dplctCnt++; 
                tempDupCount++; 
                j++;
            }
            if(tempDupCount == 0 || dplctCnt == 0){
                outText("bad circuit");
                throw("Invalid circuit");
            }
        }
    }
    cout<<endl<<"cound node prob"<<endl;
    return n-dplctCnt;
}