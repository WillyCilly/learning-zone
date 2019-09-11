#include <iostream>
#include <algorithm>

using namespace std;
int main(){

  // 1. Define the size of the integer set
  int size;
  cout << "This program calculates the quartiles of a set of integers." << endl;
  cout << "Enter the size of the integer set: ";
  cin >> size;

  // 2. Create the random integer set
  cout << "Randomly generated a set of integers ranges from [0, 64):" << endl;
  vector<int> values(size);
  for (vector<int>::iterator iter = values.begin(); iter < values.end(); ++iter){
    *iter = rand() % 64;
    cout << *iter << " ";
  }
  cout << endl;

  // 3. Sort the integer set
  sort(values.begin(), values.end());
  cout << "Sort the integer set:" << endl;
  for (vector<int>::iterator iter = values.begin(); iter < values.end(); ++iter){
    cout << *iter << " ";
  }
  cout << endl;

  // 3. Calculate the position of quartiles
  int mod_size = size%4;
  float q0, q1, q2;
  if(mod_size == 1){
    q0 = values[(size-1)/4]/1.0;
    q1 = values[(size-1)/2]/1.0;
    q2 = values[(size-1)*3/4]/1.0;
  }
  else if(mod_size == 3){
    q0 = (values[(size+1)/4] + values[(size-3)/4])/2.0;
    q1 = values[(size-1)/2]/1.0;
    q2 = (values[(size*3-5)/4] + values[(size*3-1)/4])/2.0;
  }
  else if (mod_size == 0){
    q0 = (values[size/4] + values[(size-4)/4])/2.0;
    q1 = (values[size/2] + values[(size-2)/2])/2.0;
    q2 = (values[size*3/4] + values[size*3/4-1])/2.0;
  }
  else{ // mode_size == 2
    q0 = values[(size-2)/4]/1.0;
    q1 = (values[size/2] + values[(size-2)/2])/2.0;
    q2 = values[(size*3-2)/4]/1.0;
  }
  cout << "The quartiles are:" << endl;
  cout << "q0: " << q0 << endl;
  cout << "q1: " << q1 << endl;
  cout << "q2: " << q2 << endl;
}
