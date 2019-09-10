#include <iostream>
#include <string>

using namespace std;

int main(){
  string name;
  int pad_vert, pad_horz;
  cout << "Please enter your first name: ";
  cin >> name;

  cout << "Please enter the padding on top/bottom (0-9): ";
  cin >> pad_vert;
  if(pad_vert > 10 || pad_vert < 0){
    cerr << "ERROR: Please check the padding on top/bottom. It should between 0-9." << endl;
    exit(1);
  }

  cout << "Please enter the padding on sides (0-9): ";
  cin >> pad_horz;
  if(pad_horz > 10 || pad_horz < 0){
    cerr << "ERROR: Please check the padding sides. It should between 0-9." << endl;
    exit(1);
  }

  const string hello_sentence = "Hello " + name + "!";

  size_t line_size = hello_sentence.size() + 2 + pad_horz*2;
  const string border_line(line_size, '*');

  string blank_line = "*";
  blank_line.append(line_size-2, ' ');
  blank_line.push_back('*');

  string name_line = "*";
  name_line.append(pad_horz, ' ');
  name_line.append(hello_sentence);
  name_line.append(pad_horz, ' ');
  name_line.push_back('*');

  int num_of_lines = pad_vert*2 + 1 + 2;

  for(int i=0; i<num_of_lines; ++i){
    if(i==0 || i==num_of_lines-1){
      cout << border_line << endl;
    }
    else if(i==pad_vert+1){
      cout << name_line << endl;
    }
    else{
      cout << blank_line << endl;
    }
  }

  return 0;
}
