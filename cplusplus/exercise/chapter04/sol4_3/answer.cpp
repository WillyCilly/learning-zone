#include <iostream>
#include <iomanip>

using namespace std;

int main(){

  const int max_value = 1200;

  int col1_width=1, col2_width;
  int val = max_value;
  while(val >= 10){
    col1_width += 1;
    val /= 10;
  }
  col2_width = 2*col1_width;

  for(int i=0; i<=max_value; ++i){
    cout << setw(col1_width) << i << setw(col2_width) << (i*i) << endl;
  }

}
