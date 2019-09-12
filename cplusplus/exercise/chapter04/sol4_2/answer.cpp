#include <iostream>
#include <iomanip>

using namespace std;
int main(){

  const int max_value = 100;
  for(int i=0; i<=max_value; ++i){
    cout << setw(3) << i << setw(6) << (i*i) << endl;
  }

}
