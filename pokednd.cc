//==================================================================//
// PokeDnD.cpp														//
// Jacob Ramsey		September 22, 2017								//
// This program is a tool for converting Pokemon to a d20 system.	//
//==================================================================//

#include<iostream>
#include<string>
#include<fstream>

using namespace std;

void clean(ifstream& fin, string moves[], int& arrLength);
	/*=== removes extra characters from the input file of moves =====
	===== and extracts each move, adding them to an array       ===*/

void find_cat(const string& input, int& pos);
	/*=== helper function for clean(), find the category		=====
	===== keyword in each line which is used as a reference		=====
	===== point for finding the names of the moves				===*/

string cut_move(const string& input, int& pos);
	/*=== helper function for clean(). finds the name of the	=====
	===== move in each line and returns a string of the name	=====
	===== which is then added to an array						===*/

void rm_duplicates(string moves[], int& arrLength);
	/*=== helper function for clean(). removes duplicate noves	=====
	===== from the array										===*/

void to_upper(string moves[], const int& arrLength);
	/*=== helper funtion for clean(). makes the name uppercase	=====
	===== so that is matches the base move library				===*/

void create_move_library(const string moves[], const int& arrLength);
	/*=== opens the base move libraries and passes to			=====
	===== copy_from_library()									===*/

void copy_from_library(const int& level, ifstream& fin, const string moves[], const int& arrLength);
	/*=== helper function for create_move_library(). checks		=====
	===== each base library for every move in the array and		=====
	===== copies the info for matching cases to output files	===*/


int main()
{
	char junk;/*=======REMOVE======*/

	string filename;
	ifstream fileIn;

	cout << "Welcome to the Pokemon Dungeons and Dragons Creation Toolkit!\n";

	fileIn.open("input.txt");
	if (fileIn.fail()) {
		cout << "File failed to open.\nCheck to make sure that you matched the name exactly.\n";
		return 0;
	}
	else {
		string moves[200]; //stores the moves after they are cut out of the file
		int arrLength = 0; //stores the length of the array
		clean(fileIn, moves, arrLength);
		fileIn.close();
		create_move_library(moves, arrLength);
	}
	
	cin >> junk;/*=======REMOVE======*/

	return 0;
}

void clean(ifstream& fin, string moves[], int& arrLength) {
	string input;

	while (!fin.eof()) {
		getline(fin, input);
		int pos = -1;

		find_cat(input, pos);
		if (pos > 0) { //if a keyword was found
			moves[arrLength] = cut_move(input, pos);
			arrLength++;
		}

	}
	rm_duplicates(moves, arrLength);
	to_upper(moves, arrLength);

}

void find_cat(const string& input, int& pos) { //finds the category of the move; this is used as a reference point for string manipulation
	
	pos = input.find("Physical", 0);
	if (pos > 0) return;

	pos = input.find("Status", 0);
	if (pos > 0) return;

	pos = input.find("Special", 0);
	if (pos > 0) return;

}

string cut_move(const string& input, int& pos) { //finds the move in the string and returns the name
	string move;

	bool done = false;
	for (int i = 1; !done; i++) {
		if (isalpha(input[pos - i])) {
			done = true;
			pos -= i;
		}
	}
	done = false;
	for (int i = 1; !done; i++) {
		if (!isalpha(input[pos - i])) {
			done = true;
			pos -= i;
		}
	}
	done = false;
	for (int i = 1; !done; i++) {
		if (isalpha(input[pos - i])) {
			done = true;
			pos -= i;
		}
	}
	int word2Start, word2Length;
	string word2;
	done = false;
	for (int i = 1; !done; i++) {
		if (pos - i <= 0 || !isalpha(input[pos - i])) {
			done = true;
			if (pos - i <= 0) {
				word2Start = pos - i;
				word2Length = i -1;
			}
			else {
				word2Start = pos - i + 1;
				word2Length = i;
			}
			word2 = input.substr(word2Start, word2Length); //first word found
			pos -= i;
		}
	}
	move = word2;

	int offset = 1;
	while (pos - offset > 0 && isalpha(input[pos - offset]))
		offset++;
	if (pos - offset > 0 && !isdigit(input[pos - offset]) && !isalpha(input[pos - offset])) { //second word found, if it exists
		int word1Start, word1Length;
		string word1;
		word1Start = pos - offset + 1;
		word1Length = offset;
		word1 = input.substr(word1Start, word1Length);
		move = word1 + word2;
	}

	return move;
}

void rm_duplicates(string moves[], int& arrLength) { //compares each item to every other item and deletes the duplicates

	for (int i = 0; i < arrLength; i++) {
		for (int j = 0; j <= arrLength; j++) {
			if (moves[i] == moves[j] && i != j) {
				if (j == arrLength)
					arrLength--;
				else {
					for (int k = j; k < arrLength; k++)
						moves[k] = moves[k + 1];
					arrLength--;
				}
			}
		}
	}

}

void to_upper(string moves[], const int& arrLength) {
	for (int i = 0; i < arrLength; i++) {
		for (int j = 0; moves[i][j] != 0; j++)
			if (moves[i][j] <= 122 && moves[i][j] >= 97)
				moves[i][j] -= 32;
	}

}

void create_move_library(const string moves[], const int& arrLength) {

	ifstream basic, level1, level2;

	basic.open("_basicMoves.txt");
	if (basic.fail()) {
		cout << "Failed to open basic move library\n";
		return;
	}

	level1.open("_level1Moves.txt");
	if (level1.fail()) {
		cout << "Failed to open level 1 move library\n";
		return;
	}

	level2.open("_level2Moves.txt");
	if (level2.fail()) {
		cout << "Failed to open level 2 move library\n";
		return;
	}

	for (int i = 0; i < 3; i++) {
		if (i == 0)
			copy_from_library(0, basic, moves, arrLength);
		if (i == 1)
			copy_from_library(1, level1, moves, arrLength);
		if (i == 2)
			copy_from_library(2, level2, moves, arrLength);
	}
	cout << "\ndone\n\n";
}

void copy_from_library(const int& level, ifstream& fin, const string moves[], const int& arrLength) {
	ofstream fout;
	string input;

	if (level == 0) {
		fout.open("basic.txt");

		for (int i = 0; i < arrLength; i++) {
			cout << "Searching..._basicMoves.txt for: " << moves[i] << endl;
			while (!fin.eof()) {
				getline(fin, input);
				if (input == moves[i]) {
					cout << "    found\n";
					fout << input << endl;
					getline(fin, input);
					getline(fin, input);
					while (input != ">") {
						fout << input << endl;
						getline(fin, input);
					}
					fout << endl;
				}
			}
			fin.close();
			fin.open("_basicMoves.txt");
		}

		fout.close();
	}

	if (level == 1) {
		fout.open("lvl1.txt");

		for (int i = 0; i < arrLength; i++) {
			cout << "Searching..._level1Moves.txt for: " << moves[i] << endl;
			while (!fin.eof()) {
				getline(fin, input);
				if (input == moves[i]) {
					cout << "    found\n";
					fout << input << endl;
					getline(fin, input);
					getline(fin, input);
					while (input != ">") {
						fout << input << endl;
						getline(fin, input);
					}
					fout << endl;
				}
			}
			fin.close();
			fin.open("_level1Moves.txt");
		}

		fout.close();
	}

	if (level == 2) {
		fout.open("lvl2.txt");

		for (int i = 0; i < arrLength; i++) {
			cout << "Searching..._level2Moves.txt for: " << moves[i] << endl;
			while (!fin.eof()) {
				getline(fin, input);
				if (input == moves[i]) {
					cout << "    found\n";
					fout << input << endl;
					getline(fin, input);
					getline(fin, input);
					while (input != ">") {
						fout << input << endl;
						getline(fin, input);
					}
					fout << endl;
				}
			}
			fin.close();
			fin.open("_level2Moves.txt");
		}

		fout.close();
	}
}
