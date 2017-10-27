//======================================================================//
// PokeDnD.cc								//
// Jacob Ramsey		October 26, 2017				//
// This program is a tool for converting Pokemon to a d20 system.	//
//======================================================================//

#include<iostream>
#include<string>
#include<fstream>

using namespace std;

void clean(ifstream& fin, string moves[], int& arrLength);
	/*=== removes extra characters from the input file of moves 	=====
	===== and extracts each move, adding them to an array       	===*/

void find_cat(const string& input, int& pos);
	/*=== helper function for clean(), find the category		=====
	===== keyword in each line which is used as a reference		=====
	===== point for finding the names of the moves			===*/

string cut_move(const string& input, int& pos);
	/*=== helper function for clean(). finds the name of the	=====
	===== move in each line and returns a string of the name	=====
	===== which is then added to an array				===*/

void rm_duplicates(string moves[], int& arrLength);
	/*=== helper function for clean(). removes duplicate noves	=====
	===== from the array						===*/

void to_upper(string moves[], const int& arrLength);
	/*=== helper funtion for clean(). makes the name uppercase	=====
	===== so that is matches the base move library			===*/

void create_move_library(const string moves[], const int& arrLength);
	/*=== opens the base move libraries and passes to		=====
	===== copy_from_library()					===*/

void copy_from_library(const int& level, ifstream& fin, const string moves[], const int& arrLength, const string& pokemonName);
	/*=== helper function for create_move_library(). checks		=====
	===== each base library for every move in the array and		=====
	===== copies the info for matching cases to output files	===*/

void remove_newline(string& input);
	/*=== removes newlines from the beginning of the string and	=====
	===== removes newlines, spaces, tabs, and anything other than	=====
	===== numbers and letters from the end of the string		===*/

void convert_stats();


int main()
{

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

	convert_stats();

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

	if (!isalpha(move[0])) //removing the tab that gets added to the beginning of one word moves
		move = move.substr(1, move.length() - 1);

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
	string pokemonName;

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

	cout << "What is the name of the Pokemon?\n";
	cin >> pokemonName;

	for (int i = 0; i < 3; i++) {
		if (i == 0)
			copy_from_library(0, basic, moves, arrLength, pokemonName);
		if (i == 1)
			copy_from_library(1, level1, moves, arrLength, pokemonName);
		if (i == 2)
			copy_from_library(2, level2, moves, arrLength, pokemonName);
	}
	cout << "\ndone\n\n";
}

void copy_from_library(const int& level, ifstream& fin, const string moves[], const int& arrLength, const string& pokemonName) {
	ofstream fout;
	string input;

	if (level == 0) {
		fout.open((pokemonName + "_basic.txt").c_str());

		for (int i = 0; i < arrLength; i++) {
			cout << "Searching..._basicMoves.txt for: " << moves[i] << endl;
			while (!fin.eof()) {
				getline(fin, input);
				remove_newline(input);

				if (input == moves[i]) {
					cout << "    found\n";
					fout << input << endl;
					getline(fin, input);
					getline(fin, input);
					while (input != "") {
						fout << input << endl;
						getline(fin, input);
						remove_newline(input);
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
		fout.open((pokemonName + "_lvl1.txt").c_str());

		for (int i = 0; i < arrLength; i++) {
			cout << "Searching..._level1Moves.txt for: " << moves[i] << endl;
			while (!fin.eof()) {
				getline(fin, input);
				remove_newline(input);

				if (input == moves[i]) {
					cout << "    found\n";
					fout << input << endl;
					getline(fin, input);
					getline(fin, input);
					while (input != "") {
						fout << input << endl;
						getline(fin, input);
						remove_newline(input);
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
		fout.open((pokemonName + "_lvl2.txt").c_str());

		for (int i = 0; i < arrLength; i++) {
			cout << "Searching..._level2Moves.txt for: " << moves[i] << endl;
			while (!fin.eof()) {
				getline(fin, input);
				remove_newline(input);

				if (input == moves[i]) {
					cout << "    found\n";
					fout << input << endl;
					getline(fin, input);
					getline(fin, input);
					while (input != "") {
						fout << input << endl;
						getline(fin, input);
						remove_newline(input);
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

void remove_newline(string& input){

	while(input[0] == '\n')
		input.erase(0);
	while(input.length() > 0
		&& (input[input.length() - 1] == '\n'
		|| input[input.length() - 1] == ' '
		|| input[input.length() - 1] == '	'
		|| !(isalpha(input[input.length() - 1]) || isdigit(input[input.length() - 1]))))
			input.erase(input.length() - 1);

	return;

}

void convert_stats(){

	const double HP_RATIO = 0.071;
    	const double ATK_RATIO = 0.126;
    	const double DEF_RATIO = 0.12;
    	const double SP_ATK_RATIO = 0.135;
    	const double SP_DEF_RATIO = 0.135;
    	const double SPEED_RATIO_1 = 0.132;
    	const double SPEED_RATIO_2 = 0.07;
    	int hp, atk, def, sp_atk, sp_def, speed;
    	double con, str, dex, intel, wis, cha, spd, hit_points, hp_per_lvl;

	cout << "Enter stats\n";
    	cin >> hp >> atk >> def >> sp_atk >> sp_def >> speed;

    	con = (((hp + 100) * HP_RATIO) + ((def + 30) * DEF_RATIO) + ((sp_def + 20) * SP_DEF_RATIO)) / 3;
    	str = (atk + 20) * ATK_RATIO;
    	dex = (((atk + 20) * ATK_RATIO) + ((speed + 25) * SPEED_RATIO_1)) / 2;
    	intel = (sp_atk + 20) * SP_ATK_RATIO;
    	wis = (((sp_atk + 20) * SP_ATK_RATIO) + ((sp_def + 20) * SP_DEF_RATIO)) / 2;
    	cha = (((sp_atk + 20) * SP_ATK_RATIO) + (((sp_def + 20) * SP_DEF_RATIO)) * 2) / 3;
    	spd = (speed + 20) * SPEED_RATIO_2;
    	hit_points = (hp + 100) * HP_RATIO;
    	hp_per_lvl = hit_points / 3;

    	cout << "\nCon: " << con << "\nStr: " << str << "\nDex: " << dex
        	<< "\nInt: " << intel << "\nWis: " << wis << "\nCha: " << cha
        	<< "\nSpeed: " << spd << "\nHP: " << hit_points << "\nHP/Lvl " << hp_per_lvl << endl;

    return;


}
