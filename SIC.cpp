#include <iostream> // cout, endl
#include <fstream> // open, is_open, close, ignore
#include <string> // string, find_last_of, substr
#include <string.h> // string, find_last_of, substr
#include <vector> // vector, push_back
#include <cstdlib> // system, atoi
#include <sstream>
#include <stdio.h>
#include <cstring>
#include <strstream>
#include <iomanip>

using namespace std;

typedef struct tb {
	int number = 0 ;
	string token ;
} table;

typedef struct Ins {
	int i_Line = 0 ;
	int i_Loc = 0 ;
	string i_token = "" ;
	string i_code = "";
	string unprocess_label = "" ; // 未處理的Label 
	bool is_X = false ;
	bool is_end = false ;
} Instruction;

typedef struct Lt {
	string label ;
	int l_Loc = 0 ;
} Labeltable;

vector<table> Table[4] ; //創建七個放table的vector 
vector<Instruction> instruction ; // 存放檔案中各行切下來的token 
string input ;

vector<Labeltable> labeltable ;
int Line = 0 ;
int Loc = 0 ;

//查詢opcode
string opcode[] = { "18", "58", "90", "40", "B4", "28", "88", "A0", "24", "64", "9C", "C4", "C0", "F4", "3C",
	"30", "34", "38", "48", "00", "68", "50", "70", "08", "6C", "74", "04", "E0", "20", "60", "98", "C8",
	"44", "D8", "AC", "4C", "A4", "A8", "F0", "EC", "0C", "78", "54", "80", "D4", "14", "7C", "E8", "84",
	"10", "1C", "5C", "94", "B0", "E0", "F8", "2C", "B8", "DC" } ;


void readtable(){
	string tempname ;
	
	for(int i = 1 ; i < 5 ; i ++ ) {
		stringstream ss;
  		ss << i;
 		string tablenum = ss.str();
 		
		string tempname = "Table" + tablenum + ".table" ; //開啟的table檔 

		table temp ;
		fstream file ;
    	file.open( tempname.c_str(), fstream::in) ;
    	int num = 0 ; 
    	while(!file.eof()) {
    		temp.number = num + 1 ;
    		file >> temp.token ;

    		Table[i-1].push_back(temp) ;
    		num = num + 1 ;
		}
    	
    	file.close() ;
    	
	}	
	
}


bool isSymbol(char temp) { //判斷是否為符號(Table4) 
	for(int i = 0 ; i < Table[3].size() ; i ++ ) {
		if(temp == Table[3][i].token[0])
			return true ;
	}
	return false ;
}

bool isTable1(string temp, int &num) { // 判斷是否為Table1 
	for(int i = 0 ; i < Table[0].size() ; i ++ ) {
		if(temp == Table[0][i].token){
			num = Table[0][i].number ;
			return true ;
		}

	}
	return false ;	
}

bool isTable2(string temp, int &num) { // 判斷是否為Table2 
	for(int i = 0 ; i < Table[1].size() ; i ++ ) {

		if(temp == Table[1][i].token){
			num = Table[1][i].number ;
			return true ;
		}
	}
	return false ;		
}

bool isTable3(string temp, string next, int &num) { // 判斷是否為Table3
	if(temp == "X" && next == "\'" )
		return false ;
		
	for(int i = 0 ; i < Table[2].size() ; i ++ ) {
		if(temp == Table[2][i].token){
			num = Table[2][i].number ;
			return true ;
		}
	}
	return false ;		
}

bool isTable4(string temp, int &num) {// 判斷是否為Table4
	for(int i = 0 ; i < Table[3].size() ; i ++ ) {
		if(temp == Table[3][i].token){
			num = Table[3][i].number ;
			return true ;
		}
	}
	return false ;		
}

bool IsNum(string str) { // 判斷是否為數字    
    for(int i = 0; i < str.size() ; i++) { // 如果不是就回傳false 
            int tmp = (int)str[i];
            
			if (tmp < 48 || tmp > 57)
        		return false ;
    }
    
    return true ;
}

string change_small(string token) { //由大寫轉小寫 
	string temp = token ;
	for(int i = 0 ; i < temp.size(); i++) {
		if(temp[i] >= 65 && temp[i] <= 90) { // 代表為大寫 
			temp[i] = temp[i] + 32 ; // 轉小寫 
		}
	}
	return temp ; 
}

string change_big(string token) { //由小寫轉大寫 
	string temp = token ;
	for(int i = 0 ; i < temp.size(); i++) {
		if(temp[i] >= 97 && temp[i] <= 122) { // 代表為小寫 
			temp[i] = temp[i] - 32 ; // 轉大寫 
		}
	}
	return temp ; 
}

void Analysis(string token,vector<string> &tempinstruction) {
	
	for(int i = 0 ; token[i] != '\n' ; i ++ ) {
		string temp = "" ;
		if(token[i] != ' ' && token[i] != '\t') {
			while( token[i] != ' ' && token[i] != '\t' && !isSymbol(token[i]) && token[i] != '\n' ){

				temp = temp + token[i] ;
				i = i + 1 ;

			}				

			if( temp == "" && isSymbol(token[i])) { //代表第一個讀到的就是符號 
				temp =  token[i] ;
				tempinstruction.push_back(temp) ;
				if( token[i]=='\''){ //代表為string or 16進位 
					string tempstring = "" ;
					i = i + 1 ;
					while(token[i]!='\''){
						tempstring = tempstring + token[i] ;
						i = i + 1 ;
					}
					tempinstruction.push_back(tempstring) ;
					tempinstruction.push_back(temp) ; // 把最後的'也push進去 
					i = i + 1 ;
				} 
				else if(token[i]=='.') { //代表為註解 
					break ;
				}
			}
			else if (isSymbol(token[i])) { //代表有先讀到token遇到符號 
				tempinstruction.push_back(temp) ;
								
				if(token[i] != '\'' ){
	
					temp = token[i] ;
					tempinstruction.push_back(temp) ;
				}
				
				else{
					i = i - 1 ;
				}
						
				if(token[i]=='.') {
					break ;
				}	
			}
			else { //只有token 
				tempinstruction.push_back(temp) ;
			}			
		}
		
		if(token[i] == '\n')
			break ;
	}


}

void Lexical_analysis(Ins &one_instruction,vector<string> tempinstruction){
	Line = Line + 5 ;
	one_instruction.i_Line = Line ;
	one_instruction.i_Loc = Loc ;
	Loc = Loc + 3 ;
	for(int i = 0 ; i < tempinstruction.size() ; i ++ ){
		int num = 0 ;
		string next = "" ;
		if( i + 1 < tempinstruction.size() ){
			next = tempinstruction[i+1] ;
		}
		
		string small = change_small(tempinstruction[i]) ;
		string big = change_big(tempinstruction[i]) ;
		
		if(isTable1(small,num)){ //若為instruction則查OPCODE 
		
	 		one_instruction.i_code = opcode[num-1] ;
	 		if(tempinstruction[i] == "RSUB") {
	 			one_instruction.i_code = one_instruction.i_code + "0000" ;
			 }
		}
		else if (isTable2(big,num)) {
			if(big == "START") {
				i = i + 1 ;
    			strstream ss;
    			ss << tempinstruction[i];
    			ss >> hex >> Loc;
    			one_instruction.i_Loc = Loc ;
				break ;
			}
			else if (big == "END")	{ //暫且跳過label的部分 
				Loc = Loc - 3 ;
				one_instruction.is_end = true ;
				break ; 
			}
			else if (big == "BYTE")	{ 
				i = i + 3 ; //跳到' '之間的內容 
				if(tempinstruction[i-2] == "c" || tempinstruction[i-2] == "C" ){
					string tempcode = "" ;
					for(int j = 0 ; j < tempinstruction[i].size() ; j++ ) {
						int	tempnum = tempinstruction[i][j] ;
						stringstream ss;
	  					ss << hex << uppercase << tempnum;
						tempcode = tempcode + ss.str() ;			
					}
					one_instruction.i_code = tempcode ;
					
					Loc = Loc - 3 + tempinstruction[i].size() ;
					
				}
				else if(tempinstruction[i-2] == "x" || tempinstruction[i-2] == "X" ){
					one_instruction.i_code = tempinstruction[i] ;
					
					int temp_Loc = 0 ;
					if(tempinstruction[i].size() % 2 == 0 )
						temp_Loc = tempinstruction[i].size() / 2 ;
					else 
						temp_Loc = tempinstruction[i].size() / 2 + 1 ;
						
					Loc = Loc - 3 + temp_Loc ;
					
				} 
				
				break ;
			}
			else if (big == "WORD")	{  
				i = i + 1 ;
				int tempnum = 0 ;
    			strstream ss;
    			ss << tempinstruction[i] ;
    			ss >> tempnum ;	
				stringstream aa;
	  			aa << hex << uppercase << tempnum;
	  			one_instruction.i_code = aa.str() ;
				int size = one_instruction.i_code.size() ;
				
				for(int k = 0 ; k < 6 - size ; k ++ )   {
					one_instruction.i_code = "0" + one_instruction.i_code ;
				}
					
				break ;			
			}
			else if (big == "RESB")	{ //保留空間 
			    i = i + 1 ;
				int tempnum = 0 ;
    			strstream ss;
    			ss << tempinstruction[i] ;
    			ss >> tempnum ;	
				Loc = Loc - 3 + tempnum	;		    
				break ;
			}	
			else if (big == "RESW")	{  
				break ;
			}							
		}
		else if (isTable3(big,next,num)) { // 未處理 
			tempinstruction[i] = big ;
			stringstream ss;
	  		ss << num;
	 		string tablenum = ss.str();
		
		}
		else if(isTable4(tempinstruction[i],num)) {
			if(tempinstruction[i] == "."){
				Loc = Loc - 3 ;
				break ;				
			}
			else if (tempinstruction[i] == "," && tempinstruction[i+1] == "X"){
				one_instruction.is_X = true ;
				break ;
			}
		
		}
		else { // symbol
			if( i == 0 ) {
				Labeltable templabel ;
				templabel.l_Loc = one_instruction.i_Loc ;
				templabel.label = tempinstruction[i] ;
				labeltable.push_back(templabel) ;
			}
			else{
				one_instruction.unprocess_label = tempinstruction[i] ;
			}
		}	

	}
			
}

void pass2(){
	for(int i = 0 ; i < instruction.size() ; i ++ ){
		if( instruction[i].unprocess_label != "" )  {
			int k = 0 ;
			while(k < labeltable.size() && labeltable[k].label != instruction[i].unprocess_label) {
				k = k + 1 ;
			}
			
			string tempLoc ; 
			stringstream aa;
	  		aa << hex << uppercase << labeltable[k].l_Loc ;
	  		tempLoc = aa.str() ;
			instruction[i].i_code = instruction[i].i_code + tempLoc ;
			
			if (instruction[i].is_X) {
				int temp = 0 ;
				strstream ss;
    			ss << instruction[i].i_code[2];
    			ss >> hex >> temp;
				temp = temp + 8 ;
				stringstream aa ; 
				aa << hex << uppercase << temp;
				instruction[i].i_code[2] = aa.str()[0] ;
			}
		}
		
	}
	
	
}


bool readfile() {
    cout << "Input your file name. [0: quit]" << endl <<"Ex:XXX.txt (just input file name XXX)" << endl ;
	cin >> input ;
	
	if(input == "0" ) 
		return false ;
		
	string inputname = input + ".txt" ;
    fstream file ;
    file.open( inputname.c_str(), fstream::in) ;	
    
    while( !file.is_open()) {
    	cout << endl << "### File " <<  input << ".txt does not exist! ###" << endl ;
        cout << endl << "Input your file name again. [0: quit]" << endl <<"Ex:XXX.txt (just input file name XXX)" << endl ;
        cin >> input ;
        
		if(input == "0" ) 
			return false ;
			
        inputname = input + ".txt" ;

        file.open( inputname.c_str(), fstream::in);
	}
    
    string total ;
    
    while( getline(file,total) ) {

		string temp = total + "\n" ;
	
		vector<string> tempinstruction ;
		Analysis(temp,tempinstruction) ; //切好token

		Ins one_instruction ;
		one_instruction.i_token = total ;
		if ( tempinstruction.size() != 0 ) 				
			Lexical_analysis(one_instruction,tempinstruction) ;

		instruction.push_back(one_instruction) ;

	}	
	
	file.close() ;  
   
    return true ;	
	 
}

void outputfile() {
	fstream outputfile ;
	string outputname = input + "_output.txt" ;
	outputfile.open( outputname.c_str(), ios::out) ; //開啟寫入模式 
    outputfile << "Line	Loc	Source	statement		Object code\n" ;
    
    for(int i = 0 ; i < instruction.size() ; i ++ ) {
		if ( instruction[i].i_token == ""  ){
			outputfile << instruction[i].i_token << endl ;
		}
		else {
    		outputfile << right << setw(3) << instruction[i].i_Line << "	";
			stringstream aa;
	  		aa << hex << uppercase << instruction[i].i_Loc ;
	  		
			if (instruction[i].i_token[0] != '.' && !instruction[i].is_end )
				outputfile << setw(4) << aa.str() << "	"  ;
			else 	
				outputfile << setw(4) << " " << "	"  ;
				
			outputfile << left << setw(25) << instruction[i].i_token << "	" ;
			outputfile << instruction[i].i_code << endl ; 
		}
	}
    
    outputfile.close() ;  
	
}

int main(void) {
	readtable() ;
	while(readfile()){
		pass2() ;
		outputfile() ;
		cout << endl << "File analysis completed." << endl << endl ;		
	}	

	cout << endl << "End of program." << endl ;
	return 0 ;
} // end main

