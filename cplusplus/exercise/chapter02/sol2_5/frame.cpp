#include <iostream>
#include <string>

using namespace std;

int main(){
  // 1. choose the shape
  int keep_asking = 1;
  int shape_id;
  while(keep_asking == 1){
    cout << "Choose the shape type: " << endl;
    cout << "(1) Square" << endl;
    cout << "(2) Rectangle" << endl;
    cout << "(3) Triangle" << endl;

    cout << "Shape type (enter 1/2/3): ";
    cin >> shape_id;

      // if shape_type != 1/2/3, keep asking
    if(shape_id == 1 || shape_id == 2 || shape_id ==3){
      keep_asking = 0;
    }
  }

  // 2. choose the length
  int height=0, width=0;
  cout << "Choose the shape height (enter a number): ";
  cin >> height;

  if(shape_id == 2){
    cout << "Choose the shape width (endter a number): ";
    cin >> width;
  }else{
    width = height;
  }

  // 3. square/rectangle
  if(shape_id == 1 || shape_id == 2){
    for (int h=0; h<height; ++h){
      for (int w=0; w<width; ++w){
        cout << "*";
      }
      cout << endl;
    }
  }

  // 4. triangle
  if(shape_id == 3){
    for (int h=0; h<height; ++h){
      for (int blank=0; blank < (height-h); ++blank){
        cout << " ";
      }
      for (int star=0; star < 2*h+1; ++star){
        cout << "*";
      }
      for (int blank=0; blank < (height-h); ++blank){
        cout << " ";
      }
      cout << endl;
    }
  }

  return 0;
}
