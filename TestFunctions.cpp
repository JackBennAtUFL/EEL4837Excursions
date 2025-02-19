
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

int main(){

    int rows = 5;
    int col = 3;

    double** testArr = gen2DArray(rows,col);

    testArr[2][1] = 2.1;
    testArr[2][2] = 2.2;
    testArr[4][0] = 4.0;
    testArr[0][0] = 0.0;

    cout << testArr[2][1] << endl;
    cout << testArr[0][0] << endl;
    cout << testArr[4][0] << endl;
    cout << testArr[2][2] << endl;
    cout << testArr[3][1] << endl;

    for(int i = 0; i<rows; i++){
        delete [] testArr[i];

    }
    delete [] testArr;


    return 0;
}

