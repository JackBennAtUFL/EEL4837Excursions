
#include <iostream>
using namespace std;


//Creates a 2D dynamic array to be used for the matrix math
//-----***-----
double** gen2DArray(int row, int col){

    //Creates a pointer array of pointers of the double data type (2D array pointer)
    double** totArray = new double*[row];

    //Interates across the all of the rows and inserts another array within each index
    for(int i = 0; i < row; i++){
        totArray[i] = new double[col];

        //Fills the pointer array with zeros
        for(int j = 0; j < col; j++){
            totArray[i][j] = 0.0;
        }
    }

    //Returns the final array
    return totArray;

}
//-----***-----
double** createidentity(int size ){

    double** identity = gen2DArray(size,size);

    for(int i = 0;i<size;i++){
        identity[i][i] = 1;
    }
    return identity; 

}



int main(){

    int rows = 5;
    int col = 3;

    double** testArr = gen2DArray(rows,col);

    double** testArr2 = gen2DArray(rows,col);

    testArr2[2][1] = 4;
    testArr2[2][2] = 5;
    testArr2[4][0] = 7;
    testArr2[0][0] = 8;
    testArr[2][1] = 2.1;
    testArr[2][2] = 2.2;
    testArr[4][0] = 4.0;
    testArr[0][0] = 0.0;

    double** negated = createidentity(5);


    // for(int i = 0;i<rows;i++){
    //     for(int j = 0;j<col;j++){
    //         cout << testArr2[i][j]<<"   ";
    //     }
    //     cout<<endl;
    // }

    for(int i = 0;i<5;i++){
        for(int j = 0;j<5;j++){
            cout << negated[i][j]<<"   ";
        }
        cout<<endl;
    }

    for (int i = 0; i < rows; i++) {
        delete[] testArr[i];
        delete[] testArr2[i];
        
    }
    delete[] testArr;
    delete[] testArr2;
    



    return 0;
}

