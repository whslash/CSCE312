//Huai Wu
//Hack Assembler
#include <vector>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <cctype>
#include <bitset>
#include <algorithm>
using namespace std;

class Lable {
public:
	int romAddress;
	int position;
	string lable;
	Lable(int a, int b, string c):romAddress(a), position(b), lable(c){};
};

//get each line
void handle(vector<string>& t, string& s, int& counter, vector<Lable>& flag) {
	string str_dest = "xxx";
	string str_jump = "xxx";
	string str_comp = "xxxxxxx";
//delete comments and whitespaces
	s = s.substr(0,s.find("//"));
	s.erase(remove_if(s.begin(), s.end(), [](char c){return isspace(static_cast<unsigned char>(c));}),s.end());
	while(!s.empty()) {
	//deal with (LABLE)
		if(s[0] == '(') {
			s.erase(s.begin());
			s = s.substr(0,s.find(")"));
			for(int j = 0; j < flag.size(); j++) {
				if(s.compare(flag[j].lable)==0) {
					flag[j].position = counter;
					return;
				}
			}	
			Lable at(-1, counter, s);
			flag.push_back(at);		
			return;	
		}	

	//deal with A-Instruction		
		else if(s[0] == '@') {
			s.erase(s.begin());
			
		//Predefined Symbols
			if(s.compare("SP")==0) t.push_back("0000000000000000");
			else if(s.compare("LCL")==0) t.push_back("0000000000000001");
			else if(s.compare("ARG")==0) t.push_back("0000000000000010");
			else if(s.compare("THIS")==0) t.push_back("0000000000000011");
			else if(s.compare("THAT")==0) t.push_back("0000000000000100");
			else if(s.compare("SCREEN")==0) t.push_back("0100000000000000");
			else if(s.compare("KBD")==0) t.push_back("0110000000000000");
			else if(s.compare("R0")==0) t.push_back("0000000000000000");
			else if(s.compare("R1")==0) t.push_back("0000000000000001");
			else if(s.compare("R2")==0) t.push_back("0000000000000010");
			else if(s.compare("R3")==0) t.push_back("0000000000000011");
			else if(s.compare("R4")==0) t.push_back("0000000000000100");
			else if(s.compare("R5")==0) t.push_back("0000000000000101");
			else if(s.compare("R6")==0) t.push_back("0000000000000110");
			else if(s.compare("R7")==0) t.push_back("0000000000000111");
			else if(s.compare("R8")==0) t.push_back("0000000000001000");
			else if(s.compare("R9")==0) t.push_back("0000000000001001");
			else if(s.compare("R10")==0) t.push_back("0000000000001010");
			else if(s.compare("R11")==0) t.push_back("0000000000001011");
			else if(s.compare("R12")==0) t.push_back("0000000000001100");
			else if(s.compare("R13")==0) t.push_back("0000000000001101");
			else if(s.compare("R14")==0) t.push_back("0000000000001110");
			else if(s.compare("R15")==0) t.push_back("0000000000001111");
			//covert decimal number to binary
			else if(isdigit(s[0])) {
					int num = atoi(s.c_str());
					t.push_back("0" + bitset<15>(num).to_string());
			}
			
			//deal with @LABLE	
			else {
				for(int j = 0; j < flag.size(); j++) {
				//find exist symbol name
					if(s.compare(flag[j].lable)==0) {
						if(flag[j].position!=-1 && flag[j].romAddress!=-1) {
							Lable at(counter, flag[j].position, s);
							flag.push_back(at);
							t.push_back("XXXXXXXXXXXXXXXX");
							counter++;
							return;
						}
					//multi variable name
						else if(flag[j].position==-1 && flag[j].romAddress!=-1) {
							Lable at(counter, -1, s);
							flag.push_back(at);
							t.push_back("XXXXXXXXXXXXXXXX");
							counter++;
							return;
						}
					//get romAddress for (LABLE)
						else {
							flag[j].romAddress = counter;
							t.push_back("XXXXXXXXXXXXXXXX");
							counter++;
							return;
						}
					}
				}
			//new lable
				Lable at(counter, -1, s);
				flag.push_back(at);
				t.push_back("XXXXXXXXXXXXXXXX");
			}
		counter++;
		return;	
		}
		
		
		
		
		
	//deal with C-Instruction	
		else {
			int equal_position = s.find_first_of('=');
			int semicolon_position = s.find_first_of(';');
		//dest	
			if(equal_position < 0) str_dest = "000";
			else {
				string dest = s.substr(0, equal_position);
				if(dest.compare("M")==0) str_dest = "001";
				else if(dest.compare("D")==0) str_dest = "010";
				else if(dest.compare("MD")==0) str_dest = "011";
				else if(dest.compare("A")==0) str_dest = "100";
				else if(dest.compare("AM")==0) str_dest = "101";
				else if(dest.compare("AD")==0) str_dest = "110";
				else if(dest.compare("AMD")==0) str_dest = "111";
			}
		//jump	
			if(semicolon_position < 0) str_jump = "000";
			else {
				string jump = s.substr(semicolon_position+1, s.length()-semicolon_position-1);
				if(jump.compare("JGT")==0) str_jump = "001";
				else if(jump.compare("JEQ")==0) str_jump = "010";
				else if(jump.compare("JGE")==0) str_jump = "011";
				else if(jump.compare("JLT")==0) str_jump = "100";
				else if(jump.compare("JNE")==0) str_jump = "101";
				else if(jump.compare("JLE")==0) str_jump = "110";
				else if(jump.compare("JMP")==0) str_jump = "111";
			}
		//comp	
			string comp;
			if(equal_position < 0 && semicolon_position < 0) comp = s;
			else if(equal_position <= s.length() && semicolon_position < 0) comp = s.substr(equal_position+1, s.length()-equal_position-1);
			else if(equal_position < 0 && semicolon_position <= s.length()) comp = s.substr(0, semicolon_position);
			else comp = s.substr(equal_position+1, semicolon_position-equal_position-1);
			
			if(comp.compare("0")==0) str_comp = "0101010";
			else if(comp.compare("1")==0) str_comp = "0111111";
			else if(comp.compare("-1")==0  ) str_comp = "0111010";
			else if(comp.compare("D")==0) str_comp = "0001100";
			else if(comp.compare("A")==0) str_comp = "0110000";
			else if(comp.compare("!D")==0) str_comp = "0001101";
			else if(comp.compare("!A")==0) str_comp = "0110001";
			else if(comp.compare("-D")==0) str_comp = "0001111";
			else if(comp.compare("-A")==0) str_comp = "0110011";
			else if(comp.compare("D+1")==0) str_comp = "0011111";
			else if(comp.compare("A+1")==0) str_comp = "0110111";
			else if(comp.compare("D-1")==0) str_comp = "0001110";
			else if(comp.compare("A-1")==0) str_comp = "0110010";
			else if(comp.compare("D+A")==0 ) str_comp = "0000010";
			else if(comp.compare("A-D")==0)  str_comp = "0000111";
			else if(comp.compare("D&A")==0) str_comp = "0000000";
			else if(comp.compare("D|A")==0) str_comp = "0010101";
			else if(comp.compare("M")==0) str_comp = "1110000";
			else if(comp.compare("!M")==0) str_comp = "1110001";
			else if(comp.compare("-M")==0) str_comp = "1110011";
			else if(comp.compare("M+1")==0) str_comp = "1110111";
			else if(comp.compare("M-1")==0) str_comp = "1110010";
			else if(comp.compare("D+M")==0) str_comp = "1000010";
			else if(comp.compare("D-M")==0) str_comp = "1010011";
			else if(comp.compare("M-D")==0) str_comp = "1000111";
			else if(comp.compare("D&M")==0) str_comp = "1000000";
			else if(comp.compare("D|M")==0) str_comp = "1010101";
			
			t.push_back("111" + str_comp + str_dest + str_jump);
			counter++;
			return;
		}
	}
	return;
}

//get the machine code of symbols
void symbol(vector<string>& t, vector<Lable>& flag) {
	int address = 16;
	for(int j = 0; j < flag.size(); j++) {
	//ignore unlocated lables
		if(flag[j].romAddress == -1) continue;
	//get variable positions starting at ROM[16]
		if(flag[j].position == -1) {
			int c = 0;	//flag
		//variable address already took	
			for(int q = 0; q < j; q++){
				if((flag[j].lable).compare(flag[q].lable)==0) {
					c=1;
					t[flag[j].romAddress] = t[flag[q].romAddress];
				}
			}
		
		//give a new romAddress
			if(c==0){
				t[flag[j].romAddress] = "0" + bitset<15>(address).to_string();
				address++;
			}
		}
	//get LABLE positions
		else {
			t[flag[j].romAddress] = "0" + bitset<15>(flag[j].position).to_string();
		}
	}
}

int main()
try {
	cout << "Howdy! Please put your symbolic Hack assembly files with this assembler program." << endl;
	
	while(true) {	
	//input .asm file	
		cout << "Which file you want to convert?(Put the name without '.asm')" << endl;;
		string fileName = "";
		cin >> fileName;
		string fileNameAsm = fileName + ".asm"; 
		ifstream ist {fileNameAsm};
		if (!ist) throw string("Can't open the file.");

		string s = "";
		vector<string> t;
		vector<Lable> flag;
		int counter = 0;
	//get each line		
		while(getline(ist,s)) {
			handle(t,s,counter,flag);
		}
	
		symbol(t,flag);
	
	//creat .hack file	
		string fileNameHack = fileName + ".hack";
		ofstream ost {fileNameHack};
		for(int i = 0; i < t.size(); i++) {
			ost << t[i] << endl;
		}
		
		cout << "Successful conversion. Lines:" << t.size() << endl;
	}
	
	return 0;
}
catch(string& e) {
	cerr << "error: " << e << '\n';
	return 1;
}

catch(exception& e) {
	cerr << "error: " << e.what() << '\n';
	return 2;
}

catch(...) {
	cerr << "Oops: unknown exception!\n";
	return 3;
}