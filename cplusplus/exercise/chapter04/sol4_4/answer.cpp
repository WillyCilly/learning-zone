#include <iostream>
#include <iomanip>

using namespace std;

int main(){

  const double max_value = 5.0;
  const int step = 100;

  const int precision_col1 = 5;
  const int precision_col2 = 5;

  for(double i=0; i<=max_value; i+=max_value/step){
    cout << setw(precision_col1+2) << setprecision(precision_col1) << i
         << setw(precision_col2+2) << setprecision(precision_col2) << (i*i) << endl;
  }

}
