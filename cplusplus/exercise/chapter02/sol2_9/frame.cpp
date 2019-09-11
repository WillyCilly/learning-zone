#include <iostream>
#include <string>

using namespace std;

int main(){

  // 1. enter two numbers
  int first, second;
  cout << "Please enter two numbers." << endl;
  cout << "First number: ";
  cin >> first;
  cout << "Second number: ";
  cin >> second;

  if (first > second){
    cout << "The larger number is the first: " << first << endl;
  }else if (first == second){
    cout << "The two numbers are the same " << endl;
  }else{
    cout << "The larger number is the second: " << second << endl;
  }

  return 0;
}
