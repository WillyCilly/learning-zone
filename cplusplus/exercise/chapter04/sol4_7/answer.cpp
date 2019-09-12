#include <iostream>
#include <map>
#include <iomanip>
#include <vector>

using namespace std;

istream& read_numbers(istream& input, vector<double>& numbers){
  if(input){
    input.clear();

    double val;
    while(input >> val){
      numbers.push_back(val);
    }
    // clear the error state (like eof, or some input failure for the next input)
    input.clear();
  }
  return input;
}

double calculate_avg(vector<double>& numbers){
  double avg = 0.0;
  auto size = numbers.size();
  for(auto it=numbers.begin(); it!=numbers.end(); it++){
    avg += (*it);
  }
  return avg/size;
}

int main(){
  vector<double> numbers;
  cout << "Please enter a list of numbers one by one(end the list by ctrl-d): " <<endl;
  read_numbers(cin, numbers);
  cout << endl << "The average of the numbers: ";
  double numbers_avg;
  numbers_avg = calculate_avg(numbers);
  cout << endl << setprecision(4) << numbers_avg << endl;

  return 0;

}
