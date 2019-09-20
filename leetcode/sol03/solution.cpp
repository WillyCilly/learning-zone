/*******************************************************************************
Given a string s, find the longest palindromic substring in s. You may assume
that the maximum length of s is 1000.

Example 1:

Input: "babad"
Output: "bab"
Note: "aba" is also a valid answer.

Example 2:

Input: "cbbd"
Output: "bb"

https://leetcode.com/problems/longest-palindromic-substring/
*******************************************************************************/
#include <iostream>
#include <string>

using namespace std;

class Solution {
public:
    string longestPalindrome(string s) {

      size_t maxlen = 0;
      size_t start = 0;
      bool isPalindrome=true;
      if(s.empty()){
        return "";
      }
      auto id_head0=s.begin();
      for(; id_head0<s.end()-maxlen; ++id_head0){
        for(auto id_tail0=s.end()-1; id_tail0>id_head0; --id_tail0){
          // find the same characters
          if((*id_head0) == (*id_tail0)){
            // check if the characters between them is palindromic
            auto id_check_head=id_head0;
            auto id_check_tail=id_tail0;
            while(id_check_head < id_check_tail){
              if((*id_check_head) == (*id_check_tail)){
                isPalindrome = true;
                ++id_check_head;
                --id_check_tail;
              }else{
                isPalindrome = false;
                break;
              }
            }
            if(isPalindrome){
              string tmp(id_head0, id_tail0+1);
              if(maxlen < tmp.size()){
                maxlen = tmp.size();
                start = id_head0-s.begin();
              }// end if(maxlen < tmp.size())
            }// end if(isPalindrome)
          }// end if(s[id_head0] == s[id_tail0])
        }// end for(id_tail0)
      }// end for(id_head0)
      if(maxlen == 0){
        return s.substr(0, 1);
      }
      return s.substr(start, maxlen);

      /**
      int begin = 0, longest = 0, len = 0, bias = 0;
      for (int i=0; i<s.size(); ++i) {
          if (i > 0 && s[i] == s[i-1]) continue;

          bias = 0;
          for (int j=i; j<s.size()-1; ++j)
              if (s[j] == s[j+1]) ++bias;
              else break;
          if ((s.size() - i)*2 - bias - 1 <= longest) break;
          for (len=1; len<i+1 && len<s.size()-i-bias; ++len) {
              if (s[i-len] != s[i+len+bias])
                  break;
          }
          if (longest < len*2 - 1 + bias) {
              longest = len*2 - 1 + bias;
              begin = i - len + 1;
          }
      }
      return s.substr(begin, longest);
      **/
    }
};

int main(){
  Solution solution;
  string output;
  output = solution.longestPalindrome("");
  cout << output << endl;
  output = solution.longestPalindrome("au");
  cout << output << endl;
  output = solution.longestPalindrome("babad");
  cout << output << endl;
  string str1(499, 'f');
  string str2(501, 'g');
  output = solution.longestPalindrome(str1+str2);
  cout << output << endl;
  return 0;
}
