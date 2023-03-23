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
#include<bitset>

using namespace std;

typedef struct tb {
	int number = 0 ;
	string token ;
} table;

typedef struct Ins {
	vector<string> temptoken ;
	bool finish = true ;
	int i_Line = 0 ;
	int i_Loc = 0 ;
	string i_token = "" ;
	string i_code = "";
	bool is_Format4 = false ;
	bool is_end = false ; //結尾 
} Instruction;

typedef struct Lt {
	bool is_string = false ;
	string label ;
	int bit = 0 ;
	int l_Loc = 0 ;
} Labeltable;

vector<table> Table[4] ; //創建七個放table的vector 
vector<Instruction> instruction ; // 存放檔案中各行切下來的token 
string input ;

vector<Labeltable> labeltable ;
vector<Labeltable> literal ;
int Line = 0 ;
int Loc = 0 ;
string base = "" ;

//查詢opcode
string opcode[] = { "18", "58", "90", "40", "B4", "28", "88", "A0", "24", "64", "9C", "C4", "C0", "F4", "3C",
	"30", "34", "38", "48", "00", "68", "50", "70", "08", "6C", "74", "04", "E0", "20", "60", "98", "C8",
	"44", "D8", "AC", "4C", "A4", "A8", "F0", "EC", "0C", "78", "54", "80", "D4", "14", "7C", "E8", "84",
	"10", "1C", "5C", "94", "B0", "E0", "F8", "2C", "B8", "DC" } ;
	
string format[] = { "3", "3", "2", "3", "2", "3", "3", "2", "3", "3", "2", "1", "1", "1", "3",
	"3", "3", "3", "3", "3", "3", "3", "3", "3", "3", "3", "3", "3", "3", "3", "2", "1",
	"3", "3", "2", "3", "2", "2", "1", "3", "3", "3", "3", "3", "3", "3", "3", "3", "3",
	"3", "3", "3", "2", "2", "3", "1", "3", "2", "3" } ;


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

string int_to_string(int num) {
	
    stringstream aa;
	aa << num ;	
	return aa.str() ;
	
}

int string_to_int(string operand) {
	int temp = 0 ;
	strstream ss;
    ss << strtoul(operand.c_str(),NULL,10);
   	ss >> temp;	
	return temp ;
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

void process_literal() {
	for(int i = 0 ; i < literal.size() ; i ++ ) {
		if(literal[i].l_Loc == 0) {
			literal[i].l_Loc = Loc ;
			Instruction one_instruction ;
			one_instruction.i_Line = Line ;
			one_instruction.i_Loc = Loc ;
			one_instruction.i_token = literal[i].label ;
			Loc = Loc + literal[i].bit ;
			Line = Line + 5 ;
			
			if(literal[i].is_string){
				for(int j = 0 ; j < literal[i].label.size() ; j++ ) {
					int	tempnum = literal[i].label[j] ;
					stringstream ss;
	  				ss << hex << uppercase << tempnum;
					one_instruction.i_code = one_instruction.i_code + ss.str() ;			
				}
							
			}
			else{
				one_instruction.i_code = literal[i].label ;
			}

			instruction.push_back(one_instruction) ;

		}
	}
	

	
}

void Lexical_analysis(Ins &one_instruction,vector<string> tempinstruction,bool &is_literal){

	Line = Line + 5 ;
	one_instruction.i_Line = Line ;
	one_instruction.i_Loc = Loc ;
	Loc = Loc + 3 ;
	
	//----------------------------------------------------
	
	string Opcode = "" ;	
	string r1 = "0" ;	
	string r2 = "0" ;			
	
	//----------------------------------------------------	
	
	for(int i = 0 ; i < tempinstruction.size() ; i ++ ){

		int num = 0 ;
		string next = "" ;
		if( i + 1 < tempinstruction.size() ){
			next = tempinstruction[i+1] ;
		}
		
		//----------------------判斷format-------------------------------------
		
		string testnext = "" ;
		if(next != "") {
			testnext = change_small(next) ;
		}
		
		if(tempinstruction[i] == "+" && isTable1(testnext,num)) {
			one_instruction.is_Format4 = true ;
			
			i = i + 1 ;
		}
		
		//-----------------------------------------------------------
				
		string small = change_small(tempinstruction[i]) ;
		string big = change_big(tempinstruction[i]) ;
		
		if(isTable1(small,num)){ //若為instruction則查OPCODE 

	 		Opcode = opcode[num-1] ; // 設定opcode 
			
			if(format[num-1] == "1" ) { // format1
				Loc = Loc - 3 + 1 ; //8bits
				one_instruction.i_code = Opcode ; 
				break ;
			}
			else if (format[num-1] == "2") { // format2
				Loc = Loc - 3 + 2 ; //16bits
				i = i + 1 ;

				string tablenum = "" ;
				
				big = change_big(tempinstruction[i]) ;
				
				if(isTable3(big,"",num)) { //第一個register存r1 
					num = num - 1 ;
			 		tablenum = int_to_string(num) ;
					r1 = tablenum ;
				}
				i = i + 2 ;
				if( i < tempinstruction.size() ) {
					big = change_big(tempinstruction[i]) ;	
							
					if(isTable3(big,"",num)) { //第二個register存r2 
						num = num - 1 ;

			 			tablenum = int_to_string(num) ;
						r2 = tablenum ;
					}					
				}

 				one_instruction.i_code = Opcode + r1 + r2 ;	
									
				break ;
			}
			else if (format[num-1] == "3") {

												
					if(i  != tempinstruction.size() - 1 && tempinstruction[i+1] == "=") {
						if(tempinstruction[i+2] == "c" || tempinstruction[i+2] == "C" || tempinstruction[i+2] == "x" || tempinstruction[i+2] == "X" ){
							bool is_exit = false ;
							for(int d = 0 ; d < literal.size() ; d ++ ) {
								if(tempinstruction[i+4] == literal[d].label)
									is_exit = true ;
							}
							if(!is_exit){
								Labeltable tmp ;
								tmp.label =  tempinstruction[i+4] ;	
								if(tempinstruction[i+2] == "c" || tempinstruction[i+2] == "C") {
									tmp.bit = tempinstruction[i+4].size() ;
									tmp.is_string = true ;
								}
									
								else{
								
									if(tempinstruction[i+4].size() % 2 == 0 )
										tmp.bit = tempinstruction[i+4].size() / 2 ;
									else 
										tmp.bit = tempinstruction[i+4].size() / 2 + 1 ;					
								}
								literal.push_back(tmp) ;
							}
						} 
						else if(i  != tempinstruction.size() - 1){
							bool is_exit = false ;
							for(int d = 0 ; d < literal.size() ; d ++ ) {
								if(tempinstruction[i+2] == literal[d].label)
									is_exit = true ;
							}
							if(!is_exit){
								Labeltable tmp ;
								tmp.label =  tempinstruction[i+2] ;		
								tmp.bit = 3 ;
								literal.push_back(tmp) ;
							}
						}										
					} 
					one_instruction.temptoken = tempinstruction ; //因為還沒處理 所以先存切好的token 方便運作 
					one_instruction.finish = false ;
				
					if(one_instruction.is_Format4)
						Loc = Loc - 3 + 4 ; //32bits
					break ;
									
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
			else if (big == "LTORG") {
				Loc = Loc - 3 ;
				one_instruction.is_end = true ;
				is_literal = true ;
				break ; 				
			}
			else if (big == "END")	{ //暫且跳過label的部分 
				Loc = Loc - 3 ;
				one_instruction.is_end = true ;
				is_literal = true ;
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
					
					if(temp_Loc > 1){
						Loc = Loc - 3 ;
						one_instruction.i_code = one_instruction.i_code + " But it is illegal." ;
					}	
					else
						Loc = Loc - 3 + temp_Loc ;
					
				} 
				
				break ;
			}
			else if (big == "WORD")	{  
				i = i + 1 ;
				int tempnum = string_to_int(tempinstruction[i]) ;
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
				int tempnum = string_to_int(tempinstruction[i]) ;
				Loc = Loc - 3 + tempnum	;		    
				break ;
			}	
			else if (big == "RESW")	{  //保留word空間 
			    i = i + 1 ;
				int tempnum = string_to_int(tempinstruction[i]) ;
    			tempnum = tempnum * 3 ; 
				Loc = Loc - 3 + tempnum	;
				break ;
			}
			else if (big == "EQU")	{  //未處理
				Loc = Loc - 3	;
				if(tempinstruction[i+1] == "*") {
					one_instruction.i_Loc = 0 ;
				}
				else if(IsNum(tempinstruction[i+1])) {
					int tempnum = string_to_int(tempinstruction[i+1]) ;
					one_instruction.i_Loc = tempnum ;
					if(i != 0) {
						labeltable[labeltable.size()-1].l_Loc = one_instruction.i_Loc ; //更新label地址 
					}
					
				}
				else { // label-label
					one_instruction.temptoken = tempinstruction ;
					one_instruction.finish = false ;
				}

				break ;
			}
			else if(big == "BASE") {
				one_instruction.is_end = true ;
				Loc = Loc - 3	;
				base = tempinstruction[i+1] ;
				break ;
			}							
		}
		else if (isTable3(big,next,num)) { // 未處理 
			tempinstruction[i] = big ;
	 		string tablenum = int_to_string(num) ;
		
		}
		else if(isTable4(tempinstruction[i],num)) {
			if(tempinstruction[i] == "."){
				Loc = Loc - 3 ;
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

		}	

	}
	
}

bool  relative_mode(int &disp,int index,int SYMBOL_ADDR ) {
	int PC = 0 ;
	char temp;

	PC = instruction[index+1].i_Loc ;  // PC point to next instruction.
	disp = SYMBOL_ADDR - PC ;
	
	if((disp >= -2048) && (disp < 2048)){
		return true ;
	}

	int k = 0 ; 
	while(k < labeltable.size() && labeltable[k].label !=  base ){
		k = k + 1 ;
	}

	disp = SYMBOL_ADDR - labeltable[k].l_Loc ;
	
	if(disp < 4096){
		return false ;
	}

}

int caculate(int address1,int address2, string token ){

	int finaladdress = 0 ;
	if(token == "+")
		finaladdress = address1 + address2 ;
	else if(token == "-") 
		finaladdress = address1 - address2 ;
	else if(token == "*")
		finaladdress = address1 * address2 ;
	else if(token == "/")
		finaladdress = address1 / address2 ;
		
	return finaladdress ;
	
}


void pass2(){

	for(int g = 0 ; g < instruction.size() ; g ++ ){

		if (instruction[g].temptoken.size()!= 0 && instruction[g].temptoken.size() >= 2 && !instruction[g].finish && instruction[g].temptoken[1] == "EQU" ) {

			int z = 0 ;			
			if(instruction[g].temptoken.size() == 3 ){ //只有一個label 

				while(z < labeltable.size() && labeltable[z].label !=  instruction[g].temptoken[2]) {
					z = z + 1 ;
				}
				if(z == labeltable.size() )	{
					instruction[g].i_code = "can not find symbol." ;
				} 
				else {
					instruction[g].i_Loc = labeltable[z].l_Loc ;
				}
			}
			else if(instruction[g].temptoken.size() == 5 ) { //label operand label
				int add1 = 0 ;
				int add2 = 0 ; 
				int z1 = 0 ;
				while(z < labeltable.size() && labeltable[z].label !=  instruction[g].temptoken[2]) {
					z = z + 1 ;
				}			
					
				while(z1 < labeltable.size() && labeltable[z1].label !=  instruction[g].temptoken[4]) {
					z1 = z1 + 1 ;
				}
				

				
				if(z == labeltable.size() || z1 == labeltable.size() )	{
					instruction[g].i_code = "can not find symbol." ;
				} 	
				else {
					add1 = labeltable[z].l_Loc ;
					add2 = labeltable[z1].l_Loc ;
					if(instruction[g].temptoken[3] == "-" && add1 < add2 ) {
						int tmp = caculate(add1,add2,instruction[g].temptoken[3]) ;
						stringstream aa;
	  					aa << hex << uppercase << tmp ;
	  					string newpc = aa.str() ;
	  					string temppc = "" ;
	  					for(int y = 4 ; y > 0 ; y --){
	  						temppc =  temppc + newpc[newpc.size() - y] ;
						}
						instruction[g].i_code = "its pc is : " + temppc ;
					}
					else
						instruction[g].i_Loc = caculate(add1,add2,instruction[g].temptoken[3]) ;

				}		
			}
			
		}

		else if( !instruction[g].finish )  {

			string Opcode = "" ;
			string n = "" ; 
			string i = "" ;
			string x = "" ;
			string b = "" ;
			string p = "" ;
			string e = "" ;
			int disp = 0 ;
			string address = "" ;
			
			for(int m = 0 ; m <instruction[g].temptoken.size() ; m ++ ) {
				string big = change_big(instruction[g].temptoken[0]) ;
				if( big == "RSUB" ) {
					Opcode = "4C" ;
					n = "1";
					i = "1";
					x = "0";
					b = "0" ;
					p = "0" ;
					e = "0" ;
					
					disp = 0 ;
				}
				else {
					int num = 0 ;
					string small = change_small( instruction[g].temptoken[m] ) ;

					if(isTable1(small,num)){ //若為instruction則查OPCODE 
	
	 					Opcode = opcode[num-1] ; // 設定opcode 	
	 				
	 					if(instruction[g].temptoken[m+1] == "#"){
							n = "0";
							i = "1";
						} 
						else if(instruction[g].temptoken[m+1] == "@"){
							n = "1";
							i = "0";
						}
						else{
							n = "1";
							i = "1";
						}
					
					
						if( m + 3 < instruction[g].temptoken.size() && instruction[g].temptoken[m+2] == "," && instruction[g].temptoken[m+3] == "X"){
							x = "1";
						}
						else{
							x = "0";
						}
	
						int z = 0 ;
						if(instruction[g].temptoken[m] == "J" && instruction[g].temptoken[m+1] == "@" ){
							while(z < labeltable.size() && labeltable[z].label !=  instruction[g].temptoken[m+2]) {
								z = z + 1 ;
							}								
						}
						else {
							while(z < labeltable.size() && labeltable[z].label !=  instruction[g].temptoken[m+1]) {
								z = z + 1 ;
							}							
						}
				
						 			
						if(!instruction[g].is_Format4) { // format3
							e = "0" ;	

							if((instruction[g].temptoken[m+1] == "#") && (instruction[g].temptoken[m+2][0] >= '0') && (instruction[g].temptoken[m+2][0] <= '9')){
								b = "0";
								p = "0";
							}
							else if ( z == labeltable.size() ){
								b = "0";
								p = "0";
								string tmp_disp = instruction[g].temptoken[m+1] ; 
								strstream s1;
    							s1 << tmp_disp;
    							s1 >> hex >> disp;
							}
							else if(relative_mode(disp,g, labeltable[z].l_Loc )){
								b = "0";
								p = "1";
							} 	
							else{
								b = "1";
								p = "0";
							}			

						}
						else { // format4
							e = "1" ;		
							b = "0" ;
							p = "0" ;
 
							stringstream a1;
	  						a1 << hex << uppercase << labeltable[z].l_Loc ;
	  						address = a1.str() ;					
	
						}				
				
				
					}				
				} 
			}

				int token_num = 0 ;
				while( token_num < instruction[g].temptoken.size() && instruction[g].temptoken[token_num] != "#"){
					token_num = token_num + 1 ;
								
				}

				if(token_num < instruction[g].temptoken.size() && instruction[g].temptoken[token_num] == "#") { // 立即值 
					if(IsNum(instruction[g].temptoken[token_num+1])) {
						disp = string_to_int(instruction[g].temptoken[token_num+1]) ;
						int tmp = string_to_int(instruction[g].temptoken[token_num+1]) ;
						stringstream ff;
	  					ff << hex << uppercase << tmp ; //轉回16進位string	
				 	 	address = ff.str() ;						
					}
					else {

						int w = 0 ;
						while(w < labeltable.size() && labeltable[w].label !=  instruction[g].temptoken[token_num+1]) {
							w = w + 1 ;
						}	
						relative_mode( disp, g, labeltable[w].l_Loc ) ;
						stringstream ff;
	  					ff << hex << uppercase << disp ; //轉回16進位string	
				 	 	address = ff.str() ;
						if(instruction[g].temptoken[token_num+1] == base){

							p = "1" ;	
						}
		
						
					}

				}	 
	
				token_num = 0 ;
				while( token_num < instruction[g].temptoken.size() && instruction[g].temptoken[token_num] != "="){
					token_num = token_num + 1 ;
								
				}

				if(token_num < instruction[g].temptoken.size() && instruction[g].temptoken[token_num] == "=") { // 立即值 
					int w = 0 ;
					if(instruction[g].temptoken[token_num+1] == "c" || instruction[g].temptoken[token_num+1] == "C" || instruction[g].temptoken[token_num+1] == "x" || instruction[g].temptoken[token_num+1] == "X" ) {
						
						while(w < literal.size() && literal[w].label !=  instruction[g].temptoken[token_num+3]) {
							w = w + 1 ;
						}						
						
					}    
					else{
						while(w < literal.size() && literal[w].label !=  instruction[g].temptoken[token_num+1]) {
							w = w + 1 ;
						}	
					}		
					relative_mode( disp, g, literal[w].l_Loc ) ;
					stringstream ff;
	  				ff << hex << uppercase << disp ; //轉回16進位string	
				 	address = ff.str() ;							
				
				}

//------------將n和i和opcode合起來------------ 
				string temp_string = n + i ;
				int temp_int = 0 ;
				strstream s2;			
    			s2 <<  strtoul(temp_string.c_str(),NULL,2);
    			s2 >> temp_int;			
				
				
				int temp_opcode = 0 ;
				
				strstream s3;
    			s3 << strtoul(Opcode.c_str(),NULL,16);
    			s3 >> temp_opcode;			
				temp_opcode = temp_opcode + temp_int ;
				
				
				
				stringstream a2;
	  			a2 << hex << uppercase << temp_opcode ; //轉回16進位string	
				  
					
				instruction[g].i_code = a2.str() ; 
//------------將 x ~ e 合起來------------ 		

				temp_string = x + b + p + e ;
				strstream s4;
				s4 << strtoul(temp_string.c_str(),NULL,2);
    			s4 >> temp_int;
    			stringstream a3;
	  			a3 << hex << uppercase << temp_int ; //轉回16進位string		
				instruction[g].i_code = instruction[g].i_code + a3.str() ; 			
//---------------------------------------
				if(!instruction[g].is_Format4) { // format3
					stringstream a4;
					a4 << hex << uppercase << disp ;
					string temp_disp = a4.str() ;
					if( temp_disp.size() < 3) {
					
						for(int a = temp_disp.size() ; a < 3 ; a ++ ){
							temp_disp = "0" + temp_disp ;
						}
					}
					else {
						string tmp = "" ;
						for(int a = 3 ; a > 0 ; a -- ){

							tmp = tmp + temp_disp[temp_disp.size()-a]  ;
						}	
						temp_disp = tmp ;					
					}
					
					instruction[g].i_code = instruction[g].i_code + temp_disp ;
					if(instruction[g].i_code.size() == 5 ) {
						instruction[g].i_code = "0" + instruction[g].i_code ;
					}
				}
				else{
					for(int m = address.size() ; m < 5 ; m ++ ){
						address = "0" + address ;
					}
					instruction[g].i_code = instruction[g].i_code + address ;				
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
		bool is_literal = false ;
		string temp = total + "\n" ;
	
		vector<string> tempinstruction ;
		Analysis(temp,tempinstruction) ; //切好token

		Ins one_instruction ;
		one_instruction.i_token = total ;
		if ( tempinstruction.size() != 0 ) 				
			Lexical_analysis(one_instruction,tempinstruction,is_literal) ;

		instruction.push_back(one_instruction) ;
		
		if(is_literal)
			process_literal() ;
	}	

	file.close() ;  
   
    return true ;	
	 
}

void outputfile() {
	fstream outputfile ;
	string outputname = input + "_output.txt" ;
	outputfile.open( outputname.c_str(), ios::out) ; //開啟寫入模式 
    outputfile << "Line  Location  Source code                              Object code\n" ;
    outputfile << "----  -------- -------------------------                 -----------\n" ;
    
    for(int i = 0 ; i < instruction.size() ; i ++ ) {
		if ( instruction[i].i_token == ""  ){
			outputfile << instruction[i].i_token << endl ;
		}
		else {
    		outputfile << right << setw(3) << instruction[i].i_Line << "	";
			stringstream aa;
	  		aa << hex << uppercase << instruction[i].i_Loc ;
	  		string temp_aa = aa.str() ;

	  		for(int k = 0 ; k < 4 - aa.str().size() ; k++){

	  			temp_aa = "0" + temp_aa ;
			}
	  		
			if (instruction[i].i_token[0] != '.' && !instruction[i].is_end )
				outputfile << setw(4) << temp_aa << "	"  ;
			else 	
				outputfile << setw(4) << " " << "	"  ;
				
			outputfile << left << setw(25) << instruction[i].i_token << "	" ;
			outputfile << right << setw(20) << instruction[i].i_code << endl ; 
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

