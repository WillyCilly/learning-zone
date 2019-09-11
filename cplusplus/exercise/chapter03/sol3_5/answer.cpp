#include <iostream>
#include <vector>
#include <limits>

using namespace std;

int main(){

  vector<string> student_names;
  vector<float>  student_scores;
  const int hw_num = 5;
  string name;
  float final_score;
  cout << "Please enter the student's name:" <<endl;
  while(cin >> name){
    float score;
    cout << "Please enter the scores of 5 homeworks: " << endl;
    final_score = 0;
    for (int i=0; i<hw_num; ++i){
      cin >> score;
      final_score += score;
    }
    final_score = final_score/5.0;
    student_names.push_back(name);
    student_scores.push_back(final_score);
    cout << "Name: " << name << endl;
    cout << "Score: " << final_score << endl << endl;
    cout << "Please enter the next student's name or end by ctrl-D" <<endl;
  }

  return 0;
}
