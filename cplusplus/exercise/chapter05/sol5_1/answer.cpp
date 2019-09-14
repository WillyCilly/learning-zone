#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <iterator>
#include <string>

using namespace std;

//------------------------------- *struct SortWords* ---------------------------
// Please go to *sort_lines* function for more details.
struct SortWords{
  string word;
  int offset;
  int local_id;
};

//------------------------------- *read_lines* ---------------------------------
// Reads input from cin and return "vector<string> lines". Each string is a line.
void read_lines(vector<string>& lines){
  string word;
  while(getline(cin, word)){
    lines.push_back(word);
  }
}

//------------------------------ *sort_lines* ----------------------------------
// Creates sorted lines starts with each word in the input and returns it as a
// vector<SortWords> sorted_word_vec.
void sort_lines(vector<string>& word_vec,
                vector<int>& line_size,
                vector<SortWords>& sorted_word_vec){
  SortWords singleword;
  // Eg: input:
  //---------------------------------
  // The softmax activation function
  // is used in neural networks
  // --------------------------------
  // The input has 2 lines, the first line has 4 words, the second line has 5.
  // The word_vec = ["The","softmax","activation","function","is","used","in",
  //                 "neural","networks"];
  // The line_size = [4, 5];
  // For word "activation", the local_id = 2, the offset = 0,
  //                        the word = "activation function The softmax".
  // For word "neural", the local_id = 3, the offset = 4,
  //                    the word = "neural networks is used in".
  for (auto iter=word_vec.begin(); iter!=word_vec.end(); ++iter){
    singleword.local_id = iter - word_vec.begin(); // computes from global_id
    singleword.offset = 0;
    int i = 0;
    // finds the local_id and offset of the word
    while(singleword.local_id - line_size[i] >= 0){
      singleword.offset += line_size[i];
      singleword.local_id -= line_size[i];
      i += 1;
    }
    singleword.word = "";
    int size = line_size[i];
    int id = 0;
    // creates word that starts from each word
    for(int j=0; j<size; j++){
      id = singleword.offset + (singleword.local_id+j)%size;
      singleword.word += word_vec[id];
      singleword.word += " ";
    }
    sorted_word_vec.push_back(singleword);
  }

  cout << "--- Before sort: " << endl;
  for (auto iter=sorted_word_vec.begin(); iter!=sorted_word_vec.end(); ++iter){
    cout << "word: " << (*iter).word
         << " position: " << (*iter).local_id << endl;
  }
  // sorts the sorted_word_vec by word.
  SortWords tmp;
  for(int i=0; i<sorted_word_vec.size()-1; ++i){
    for(int j=i+1; j<sorted_word_vec.size(); ++j){
      if(sorted_word_vec[i].word > sorted_word_vec[j].word){
        tmp = sorted_word_vec[i];
        sorted_word_vec[i] = sorted_word_vec[j];
        sorted_word_vec[j] = tmp;
      }
    }
  }
  cout << "--- After sort: " << endl;
  for (auto iter=sorted_word_vec.begin(); iter!=sorted_word_vec.end(); ++iter){
    cout << "word: " << (*iter).word
         << " position: " << (*iter).local_id << endl;
  }
}

//------------------------------- *break_lines* --------------------------------
// 1. Breaks the input from read_lines into vector<string>.
//    Creates "vector<string> word_vec". Each string is a word.
// 2. Creates "vector<int> line_size". Each int is the number of words per line.
// 3. Calls *sort_lines* function to return vector<string>.
//    The "vector<string> sorted_lines" is the sorted lines of input. Each stri-
//    -ng is a line.
// 4. Returns the maximum line size and sorted lines.
int break_lines(vector<string> lines,
                vector<SortWords>& sorted_lines){
  cout << endl;
  vector<string> word_vec;
  vector<int> line_size;
  int max_line_size = 0;
  for(auto it = lines.begin(); it!= lines.end(); it++){
    // breaks the string(wordline) into vector<string>(word)
    istringstream line_word_ss((*it));
    vector<string> line_word_vec((istream_iterator<string>{line_word_ss}), istream_iterator<string>());
    // computes the maximum wordline size
    if(max_line_size < (*it).size()){
      max_line_size = (*it).size();
    }
    // creates a vector of the number of words per line
    line_size.push_back(line_word_vec.size());
    // creates a vector of words of all lines
    word_vec.insert(word_vec.end(), line_word_vec.begin(), line_word_vec.end());
  }
  sort_lines(word_vec, line_size, sorted_lines);

  return max_line_size;
}

//------------------------------ *print_lines* ---------------------------------
// Prints the sorted_word_vec from the start word of the original wordline.
void print_lines(vector<SortWords>& sortedline,
                 int max_line_size){
  string tmp;
  for(auto it = sortedline.begin(); it!=sortedline.end(); ++it){
    // transfers string(wordline) to vector<string>(word)
    istringstream line_word_ss((*it).word);
    vector<string> line_word_vec((istream_iterator<string>{line_word_ss}), istream_iterator<string>());
    // finds the start word of the original wordline
    cout << setw(max_line_size+1);
    tmp = "";
    for(int i=0; i<(*it).local_id; ++i){
      // prints from the start word of the original wordline to the start word
      // of the current wordline
      tmp = tmp + line_word_vec[i+(line_word_vec.size()-(*it).local_id)] + " ";
    }
    cout << tmp;
    cout << "\t";
    // prints from the start word of the current wordline to the end
    for(int i=(*it).local_id; i<line_word_vec.size(); ++i){
      cout << line_word_vec[i-(*it).local_id] << " ";
    }
    cout << endl;
  }
}

//----------------------------------- *main* -----------------------------------
int main(){
  cout << "Please enter lines of words only in small letter and space"
       << " (end by ctrl-D): "
       << endl;
  vector<string> lines;
  read_lines(lines);
  vector<SortWords> sortedline;
  int max_line_size;
  max_line_size = break_lines(lines, sortedline);
  print_lines(sortedline, max_line_size);

  return 0;
}
