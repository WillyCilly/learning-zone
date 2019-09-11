#include <iostream>
#include <vector>
#include <map>

using namespace std;
int main(){

  // 1. Enter a list of string
  vector<string> input_str;
  string x;
  cout << "Please enter a list of string one by one(end the list by ctrl-d): " <<endl;
  while(cin >> x){
    input_str.push_back(x);
  }
  cout << "End of list." << endl;

  // 2. Map the string with the number of occurance
  map<string, int> map_str;
  map<string, int>::iterator it;
  for(vector<string>::iterator iter=input_str.begin(); iter!=input_str.end(); ++iter){
    it = map_str.find(*iter);
    if(it != map_str.end()){
      map_str[*iter] += 1;
    }else{
      map_str.insert(std::pair<string,int>(*iter,1));
    }
  }

  // 3. Print the result
  cout << endl << "The word and its freqency is:" << endl;
  for(it=map_str.begin(); it!=map_str.end(); ++it){
    cout << it->first << ": " << it->second << endl;
  }

  return 0;
}
