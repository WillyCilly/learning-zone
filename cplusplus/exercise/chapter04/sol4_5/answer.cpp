#include <iostream>
#include <map>
#include <vector>

using namespace std;

istream& read_word(istream& input, vector<string>& word){
  if(input){
    word.clear();

    string str;
    while(input >> str){
      word.push_back(str);
    }
    // clear the error state (like eof, or some input failure for the next input)
    in.clear();
  }
  return input;
}

void find_occurance(vector<string>& input_str, map<string, int>& map_str){
  map<string, int>::iterator it;
  for(vector<string>::iterator iter=input_str.begin(); iter!=input_str.end(); ++iter){
    it = map_str.find(*iter);
    if(it != map_str.end()){
      map_str[*iter] += 1;
    }else{
      map_str.insert(std::pair<string,int>(*iter,1));
    }
  }
}

int main(){
  vector<string> word;
  map<string, int> word_map;
  cout << "Please enter a list of string one by one(end the list by ctrl-d): " <<endl;
  read_word(cin, word);
  find_occurance(word, word_map);
  cout << endl << "The word and its freqency is:" << endl;
  map<string, int>::iterator it;
  for(it=word_map.begin(); it!=word_map.end(); ++it){
    cout << it->first << ": " << it->second << endl;
  }

  return 0;

}
