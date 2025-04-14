#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

// Virus detection program: Identifies and locates malicious codes and palindromes in transmission files, analyzing their similarity.
// Students: Javier Davila Ruiz-A00829870, José Daniel Cantu-A01284664, Miguel Ponce Zertuche-A01383176. //Creation date: April 8, 2024.

//-------Part 1: Detect mcode within transmissions------
// Function to create the KMP prefix table (Longest Prefix Suffix, LPS)
// Complexity: O(m), where m is the length of the pattern
vector<int> kmpProcess(const string &pattern) {
  int m = pattern.length(); 
  vector<int> lps(m, 0);    
  int len = 0;              // Length of the current longest prefix/suffix
  int i = 1;                

  // Build the LPS table
  while (i < m) {
    if (pattern[i] == pattern[len]) {
      // If there's a match, increment the length and assign the value in LPS
      len++;
      lps[i] = len;
      i++;
    } else {
      // If there's no match
      if (len != 0) {
        // Update the length of the current longest prefix/suffix
        len = lps[len - 1];
      } else {
        // If there are no previous matches
        lps[i] = 0;
        i++;
      }
    }
  }

  return lps; // Return the LPS table
}

// Function to check if a code is present in a transmission using KMP algorithm
// By combining LPS table creation with the KMP algorithm, we get a complexity of O(n + m), where n is the length of the transmission and m is the length of the pattern.
bool containsCode(const string &transmission, const string &code,
                  int &position) {
  int n = transmission.length();      // Length of the transmission
  int m = code.length();              // Length of the code
  vector<int> lps = kmpProcess(code); // Get the LPS table for the code

  int i = 0; // Index for transmission
  int j = 0; // Index for code

  // Iterate through the transmission to search for the code
  while (i < n) {
    if (code[j] == transmission[i]) {
      // If there's a match, increment both indices
      j++;
      i++;
    }

    if (j == m) {
      // If a complete match has been found
      position = i - j + 1; // Calculate the starting position of the match
      return true;          // Return true
    } else if (i < n && code[j] != transmission[i]) {
      // If there's no match
      if (j != 0) {
        // Update the index in code using the LPS table
        j = lps[j - 1];
      } else {
        // Move the index in transmission
        i = i + 1;
      }
    }
  }

  return false; // Return false if the code is not found
}



//-----Part 2: Find the longest palindrome in a transmission----
// Complexity: O(n), where n is the length of the transmission
pair<int, int> findLongestPalindrome(const string &transmission) {
    // Preprocessing of the string:
    // - Create a new string 's' by adding a special character ('#') between each pair of characters in the original 'transmission' string.
    // - Add a special character ('#') at the beginning and end of the new string 's' to avoid special cases at the edges.
    string s = "#";
    for (char c : transmission) {
        s += c;
        s += '#';
    }

    int n = s.length();

    // Initialization of variables:
    // - Create a vector 'p' of size 'n' to store the length of palindromes centered at each position.
    // - Initialize variables 'center' and 'right' to 0, representing the center and right boundary of the longest palindrome found so far.
    // - Initialize variables 'maxLength' and 'centerIndex' to 0, which will store the length and center index of the longest palindrome found.
    vector<int> p(n, 0);
    int center = 0, right = 0;
    int maxLength = 0, centerIndex = 0;

    // Iteration over the string 's':
    for (int i = 1; i < n - 1; i++) {
        // Calculate the position of the "mirror" of 'i' with respect to the current center.
        int mirror = 2 * center - i;

        // If 'i' is within the current right boundary, we can reuse the already calculated value of 'p[mirror]'.
        // Take the minimum between 'right - i' and 'p[mirror]' to avoid extending beyond the current right boundary.
        if (i < right) {
            p[i] = min(right - i, p[mirror]);
        }

        // Expand the palindrome centered at 'i' in both directions as long as characters match and we don't go out of bounds.
        while (i + p[i] + 1 < n && i - p[i] - 1 >= 0 && s[i + p[i] + 1] == s[i - p[i] - 1]) {
            p[i]++;
        }

        // If position 'i' plus the length of the palindrome found exceeds the current right boundary, update the center and right boundary.
        if (i + p[i] > right) {
            center = i;
            right = i + p[i];
        }

        // If the length of the palindrome found at 'i' is greater than the current maximum length, update 'maxLength' and 'centerIndex'.
        if (p[i] > maxLength) {
            maxLength = p[i];
            centerIndex = i;
        }
    }

    // Calculate the position of the longest palindrome:
    // - Calculate the start and end positions of the longest palindrome in the original 'transmission' string using 'centerIndex' and 'maxLength'.
    // - Calculate the starting position of the second part of the palindrome by adding half of the maximum length to the start.
    int start = (centerIndex - maxLength) / 2;
    int end = start + maxLength - 1;
    int secondPartStart = start + maxLength / 2;

    // Return a pair (secondPartStart, end) representing the starting position of the second part and the end position of the longest palindrome.
    return make_pair(secondPartStart, end);
}

// Complexity: O(m * n), where m is the length of the mirrored code and n is the number of lines in the file
bool verifyMirroredCode(const string &transmission, int start, int end, const string &mcode) {
    // Extract the mirrored code from the 'transmission' string using the obtained start and end positions.
    string mirroredCode(transmission.begin() + start, transmission.begin() + end + 1);

    // Reverse the mirrored code.
    reverse(mirroredCode.begin(), mirroredCode.end());

    // Verify if the mirrored code matches the malicious code.
    if (mcode.find(mirroredCode) != string::npos) {
        return true;
    }

    // If the mirrored code is not found in the file, return 'false'.
    return false;
}


//--------Part 3: Longest common substring between transmissions------------
// Function that finds the longest common substring between two given strings (Dynamic Programming).
// Complexity is O(n*m), where n is the length of the first string and m is the length of the second string.
// This is because the algorithm needs to compare each character of one string with each character of the other string.
// Returns a pair of integers indicating the initial and final position of the substring in the first string.
void longestCommonSubstring(const string& s1, const string& s2) {
    int m = s1.size(); // Length of the first string
    int n = s2.size(); // Length of the second string
    vector<int> curr(n + 1, 0), prev(n + 1, 0); // Dynamic programming matrix
    int maxLen = 0; // Maximum length found for the common substring
    int endIndex = 0; // Final position of the longest common substring in s1

    // Traverse each character of both strings
    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            // Check if the current characters of both strings are equal
            if (s1[i - 1] == s2[j - 1]) {
                curr[j] = prev[j - 1] + 1; // Extend the common substring
                if (curr[j] > maxLen) { // If the length of the common substring is greater than the maximum found
                    maxLen = curr[j]; // Update the maximum length
                    endIndex = i; // Update the final position
                }
            } else {
                curr[j] = 0; // Reset the current length of the common substring
            }
        }
        prev.swap(curr); // Prepare for the next iteration
    }

    // Calculate the initial position of the longest common substring in s1
    int startIndex = endIndex - maxLen + 1; // endIndex points to the character after the end of the substring

    // Print
    cout << "Start position: " << startIndex << ", End position: " << endIndex << endl;
}

int main() {
    string transmission1, transmission2;
    string mcode1, mcode2, mcode3;

    ifstream file1("transmission1.txt");
    ifstream file2("transmission2.txt");
    ifstream file3("mcode1.txt");
    ifstream file4("mcode2.txt");
    ifstream file5("mcode3.txt");


    if (!file1 || !file2 || !file3) {
        cerr << "Error opening files." << endl;
        return 1;
    }

    getline(file1, transmission1, '\0');
    getline(file2, transmission2, '\0');
    getline(file3, mcode1, '\0');
    getline(file4, mcode2, '\0');
    getline(file5, mcode3, '\0');

    cout << "Part 1" << endl;
    int position;
    if (containsCode(transmission1, mcode1, position)) {
        cout << "true, transmission1 contains mcode1 " << position << endl; 
    } else {
        cout << "false, transmission1 does not contain mcode1 " << endl;
    }

    if (containsCode(transmission1, mcode2, position)) {
        cout << "true, transmission1 contains mcode2 " << position << endl; 
    } else {
        cout << "false, transmission1 does not contain mcode2 " << endl;
    }

    if (containsCode(transmission1, mcode3, position)) {
        cout << "true, transmission1 contains mcode3 " << position << endl; 
    } else {
        cout << "false, transmission1 does not contain mcode3 " << endl;
    }

    if (containsCode(transmission2, mcode1, position)) {
        cout << "true, transmission2 contains mcode1 " << position << endl; 
    } else {
        cout << "false, transmission2 does not contain mcode1 " << endl;
    }

    if (containsCode(transmission2, mcode2, position)) {
        cout << "true, transmission2 contains mcode2 " << position << endl; 
    } else {
        cout << "false, transmission2 does not contain mcode2 " << endl;
    }

    if (containsCode(transmission2, mcode3, position)) {
        cout << "true, transmission2 contains mcode3 " << position << endl; 
    } else {
        cout << "false, transmission2 does not contain mcode3 " << endl;
    }


    cout << endl; 


  
   cout << "Part 2" << endl;

    pair<int, int> palindrome1 = findLongestPalindrome(transmission1);
    pair<int, int> palindrome2 = findLongestPalindrome(transmission2);
  
    bool found1 = false;
    bool found2 = false;
  
    if (palindrome1.first != -1) {
        if (verifyMirroredCode(transmission1, palindrome1.first, palindrome1.second, mcode1)) {
            cout << "True, " << palindrome1.first + 1 << " " << palindrome1.second + 2 << endl;
            cout << "The mirrored code is found in transmission1.txt." << endl;
            found1 = true;
        }
    }
  
    if (!found1) {
        cout << "False, The mirrored code is not found in transmission1.txt." << endl;
    }
  
    if (palindrome2.first != -1) {
        if (verifyMirroredCode(transmission2, palindrome2.first, palindrome2.second, mcode1)) {
            cout << "True, " << palindrome2.first + 1 << " " << palindrome2.second + 2 << endl;
            cout << "The mirrored code is found in transmission2.txt." << endl;
            found2 = true;
        }
    }
  
    if (!found2) {
        cout << "False, The mirrored code is not found in transmission2.txt." << endl;
    }


  cout << endl; 
  
  cout << "Part 3" << endl;
  cout << "For transmission1.txt and transmission2.txt:" << endl;
  longestCommonSubstring(transmission1, transmission2);

  return 0;
}
