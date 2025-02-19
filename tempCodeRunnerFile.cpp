 cout<<"incidentMatrix:"<<endl; 
    for(int i = 0;i<countRow*nodesCnt;i++){
        cout<<incidentMatrix[i]<<" "; 
        if((i+1) % countRow == 0){
            cout<<""<<endl;
        }
    }