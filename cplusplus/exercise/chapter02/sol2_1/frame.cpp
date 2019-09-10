#include <iostream>
#include <string>

using namespace std;

int main(){
  string name;
  cout << "Please enter your first name: ";
  cin >> name;

  const string hello_sentence = "Hello " + name + "!";
  size_t line_size = hello_sentence.size() + 2;
  string border_line(line_size, '*');

  cout << border_line << endl;
  cout << "*" << hello_sentence << "*" << endl;
  cout << border_line << endl;

  return 0;
}
