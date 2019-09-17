/******************************************************************************************
Given a string, find the length of the longest substring without repeating
characters.

Example 1:

Input: "abcabcbb"
Output: 3
Explanation: The answer is "abc", with the length of 3.

Example 2:

Input: "bbbbb"
Output: 1
Explanation: The answer is "b", with the length of 1.

Example 3:

Input: "pwwkew"
Output: 3
Explanation: The answer is "wke", with the length of 3.
             Note that the answer must be a substring, "pwke" is a subsequence
             and not a substring.

https://leetcode.com/submissions/detail/261794445/
********************************************************************************/
#include <iostream>
#include <list>

using namespace std;

class Solution {
public:
    int lengthOfLongestSubstring(string s) {
      if(s.empty()){
        return 0;
      }
      if (s.size() == 1){
        return 1;
      }

      // Use a list<char> *s_list* to remember the previous characters.
      // Iterate the string *s*:
      //   If the character *c* in *s* can be found in the *s_list*:
      //     remove all the characters before *c* in the *s_list* and *c*.
      //     push back the *c* into the *s_list*.
      //   *max_count* will be the max *s_list* size.

      // Corner cases I didn't realize first time:
      // 1. s is empty.
      // 2. s has only 1 character.
      // 3. all characters in s are non-repeated.

      int max_count = 0;
      list<char> s_list;
      string::iterator iter = s.begin();
      s_list.push_back((*iter));
      for(iter=s.begin()+1; iter!=s.end(); ++iter){
        int index = 0;
        for(list<char>::iterator iter_list=s_list.begin(); iter_list!=s_list.end(); ++iter_list, ++index){
          if((*iter)==(*iter_list)){
            if(max_count < s_list.size()){
                max_count = s_list.size();
            }
            for(int i=0; i<index+1; ++i){
              s_list.pop_front();
            }
            break;
          }
        }
        s_list.push_back((*iter));
      }
      if(max_count < s_list.size()){
        max_count = s_list.size();
      }
      return max_count;
    }
};

int main(){
  Solution solution;
  int max_count;
  max_count = solution.lengthOfLongestSubstring("au");
  cout << max_count << endl;
  max_count = solution.lengthOfLongestSubstring("au");
  cout << max_count << endl;
  max_count = solution.lengthOfLongestSubstring("au");
  cout << max_count << endl;
  return 0;
}
