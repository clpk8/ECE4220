//
//  main.cpp
//  LeetCOdete3st
//
//  Created by linChunbin on 2/17/18.
//  Copyright © 2018 linChunbin. All rights reserved.
//

#include <iostream>
#include <string>
#include <vector>
using namespace std;
class Solutionword {
public:
    bool exist(vector<vector<char>>& board, string word) {
        if(board.size() == 0) return false;
        for(int i=0; i < board.size(); i++){
            for(int j = 0; j < board[i].size(); j++){
                if(search(board, word, i, j, 0)) return true;
            }
        }
        return false;
    }
    bool search(vector<vector<char>>& board, string word, int i, int j, int pos){
        
        if(pos == word.size()) return true;
        
        if((i<0) || (i>board.size()) || (j<0) ||(j>board[i].size())) return false;
        
        char c = board[i][j];
        if(c == word[pos]){
            board[i][j] = '#';
            
            if(search(board, word, i-1, j, pos+1)) return true;
            if(search(board, word, i+1, j, pos+1)) return true;
            if(search(board, word, i, j-1, pos+1)) return true;
            if(search(board, word, i, j+1, pos+1)) return true;
            
            board[i][j] = c;

        }
        return false;
    }
    
};


class Solution {
public:
    string longestCommonPrefix(vector<string>& strs) {
        if(strs.size() == 0)return "";
        string result = strs[0];
        
        for(auto& s:strs){
            if(s.length() == 0) return "";
            int i = 0;
            for(i; i < s.length() && i < result.length();i++){
                if(s[i]!=result[i]) break;
            }
            result = result.substr(0,i);
        }
        return result;
        
        
    }
};


bool isAnagram(int s, int t) {
    vector<int> count(9);
    string s1 = to_string(s);
    string s2 = to_string(t);
    if(s1.length() != s2.length()) return false;
    for(int i = 0; i < s1.length(); i++){
        count[s1[i] - '0']++;
        count[s2[i] - '0']--;
    }
    for(int i = 0; i < 9; i++){
        if(count[i]) return false;
    }
    return true;
}
int main(int argc, const char * argv[]) {
    // insert code here...
    
    
    cout << isAnagram(9033, 3193);
    
    return 0;
}



