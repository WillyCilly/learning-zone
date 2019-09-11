#include <iostream>
#include <string>

using namespace std;

int main(){
  cout << "Product of the numbers in range of [1, 10)." << endl;

  int product = 1;
  for (int i=1; i<10; ++i){
     product = product * i;
  }
  cout << product << endl;
  return 0;
}
