#include <iostream>
#include <vector>
#include <limits>

using namespace std;
int main(){

  // 1. Enter a list of string
  vector<string> input_str;
  string x;
  cout << "Please enter a list of string one by one(end the list by ctrl-d): " <<endl;
  while(cin >> x){
    input_str.push_back(x);
  }
  cout << "End of list." << endl << endl;

  // 2. Find the longest string and shortest string
  int max_length=0, min_length=numeric_limits<int>::max();
  string max_string="\0", min_string="\0";
  int cur_size;
  for(vector<string>::iterator iter=input_str.begin(); iter!=input_str.end(); ++iter){
    cur_size = (*iter).size();
    if(max_length < cur_size){
      max_string = *iter;
      max_length = cur_size;
    }
    if(min_length > cur_size){
      min_string = *iter;
      min_length = cur_size;
    }
  }

  // 3. Print the result
  cout << "The longest world is: " << max_string << ", length " << max_length << endl;
  cout << "The shortest world is: " << min_string << ", length " << min_length << endl;

  return 0;
}
