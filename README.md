# Virus Detection System 🦠

## Overview
A sophisticated C++ program designed to detect and analyze malicious code patterns in transmission files. The system implements three main detection mechanisms using advanced algorithms for pattern matching, palindrome detection, and similarity analysis.

## Key Features 🔍

### 1. Malicious Code Detection
- Uses Knuth-Morris-Pratt (KMP) algorithm
- Complexity: O(n + m)
- Identifies exact positions of malicious patterns
- Real-time pattern matching capabilities

### 2. Palindrome Analysis
- Implements Manacher's Algorithm
- Complexity: O(n)
- Detects mirrored malicious codes
- Verifies palindromic signatures

### 3. Transmission Similarity
- Dynamic Programming approach
- Complexity: O(nm)
- Finds longest common substrings
- Analyzes pattern similarities

## Technical Details 💻

### Algorithms
- KMP for pattern matching
- Manacher's for palindrome detection
- Dynamic Programming for similarity analysis

### Data Structures
- Vectors
- Strings
- Pairs
- Dynamic arrays

## File Requirements 📁
The program requires the following input files:
- transmission1.txt
- transmission2.txt
- mcode1.txt
- mcode2.txt
- mcode3.txt

## Usage 🚀

Compile the program:# Network_Advanced_DSA_Project
