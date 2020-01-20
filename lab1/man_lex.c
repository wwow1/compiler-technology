#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<stdbool.h>
#include<string.h>
#define MAX_SIZE 1000
#define LETTER 0
#define DIGIT 1
#define SIGN 2
#define UNKNOWN 999
#define POINT 3
#define CHAR 4
#define STR 5
#define NOTE 6
//enum??
char nextChar,next2Char;
FILE * inFile;
int type=-1;
int line=1;
int lexLen=0;
int keywordsLen=0;
int charClass;
int nextToken;
char lexeme[MAX_SIZE];

enum{INI=258,LONG,FLOAT,DOUBLE,ID,STRING,INC,DEC,AND,OR,RS,LS,RSE,LSE,URS,ERROR,FZF,START};
char* whichtype[]={"int","long","float","double","id","string","char","symbol","keywords"," "};
char* keywords[]={"abstract","boolean","break","byte","case","catch","assert",\
"char","class","const","continue","default","do","double","else","else","enum",\
"extends","final","finally","float","for","goto","if","implements","import",\
"instanceof","int","interface","long","native","new","package","private",\
"protected","public","return","short","static","String","strictfp","super","switch",\
"synchronized","this","throw","throws","transient","try","void","volatile",\
"while","END"};
void getChar(){
	static int firstRun=1;
	if(firstRun){
		nextChar=getc(inFile);
		next2Char=getc(inFile);
		firstRun=0;
	}
	else{
		nextChar=next2Char;
		next2Char=getc(inFile);
	}
	if(nextChar==EOF){
		charClass=EOF;
	}
	else{
		if(isalpha(nextChar)){
			charClass=LETTER;
		}
		else if(isdigit(nextChar) || nextChar=='0'){
			charClass=DIGIT;
		}
		else if(nextChar=='_' || nextChar=='$'){  // add RMB sign
			charClass=SIGN;
		}
		else if(nextChar=='.'){
			charClass=POINT;
		}
		else if(nextChar=='"'){ ////////
			charClass=STR;
		}
		else if(nextChar=='.'){
			charClass=CHAR;
		}
		else if(nextChar=='/'){
			charClass=NOTE;
		}
		else{
			charClass=UNKNOWN;
		}
	}
}
void getSpace(){
	while(isspace(nextChar)){
		if(nextChar=='\n') line++;
		getChar();
	}
}

void addChar(){
	if(lexLen <= MAX_SIZE-2){
		lexeme[lexLen++]=nextChar;
		lexeme[lexLen]='\0';
	}
	else{
		printf("ERROR:lexeme is too long.\n");
		exit(0);
	}
}
void checkSymbol(){
	bool wrongtarg=true;
	char firstChar;
	char lastChar;
	if(nextChar=='~'  || nextChar==';'|| nextChar==':' || nextChar=='?' || \
		nextChar=='(' || nextChar==')' || nextChar=='{' || nextChar=='}' || nextChar=='.' \
		|| nextChar==',' || nextChar=='@' || nextChar=='[' || nextChar==']'){
		addChar();
		nextToken=nextChar;
		getChar();
	}
	else if(nextChar=='*' || nextChar=='/' || nextChar=='=' || nextChar=='^' || nextChar=='%'){
		addChar();
		nextToken=nextChar;
		getChar();
		if(nextChar=='='){
			addChar();
			nextToken+=128;
			getChar();
		}
	}
	else if(nextChar=='+' || nextChar=='-' || nextChar=='&' || nextChar=='|'){
		firstChar=nextChar;
		addChar();
		nextToken=nextChar;
		getChar();
		if(nextChar=='='){  //!!!
			addChar();
			nextToken+=128;    //such as += -= ^= >=  their token number is the ascii number of the left number
			getChar();   // add the 128   so the number will not be repeated
		}
		else if(nextChar==firstChar){
			addChar();
			switch(firstChar){
				case '+':
					nextToken=INC;
					break;
				case '-':
					nextToken=DEC;
					break;
				case '&':
					nextToken=AND;
					break;
				case '|':
					nextToken=OR;
					break;
				default:
					printf("ERROR:invalid symbol %s.\n",lexeme);
					nextToken=ERROR;
					wrongtarg=false;
			}
			getChar();
		}
	}
	else if(nextChar=='>' || nextChar=='<'){
		firstChar=nextChar;
		lastChar=nextChar;
		addChar();
		nextToken=nextChar;
		getChar();
		if(nextChar=='='){
			addChar();
			nextToken+=128;
			getChar();
		}
		else if(nextChar==lastChar){
			addChar();
			if(firstChar=='>')
				nextToken=RS;
			else if(firstChar=='<')
				nextToken=LS;
			getChar();
			if(nextChar=='='){
				addChar();
				if(lastChar=='>')
					nextToken=RSE;
				else if(lastChar='<')
					nextToken=LSE;
				getChar();
			}
			else if(firstChar=='>' &&  nextChar=='>'){
				addChar();
				getChar();
				nextToken=URS;
			}
		}
	}
	else{
		printf("ERROR:unknown character %c.\n",nextChar);
		nextToken=ERROR;
		wrongtarg=false;
		type=9;
	}
	if(wrongtarg){
		type=7;
	}
	else
		type=9;
}
void checkKeywords(){
	int i=0;
	while(strcmp(keywords[i],"END")){
		if(strcmp(keywords[i],lexeme)==0){
			nextToken=START+i;  //change the token
			type=8;
			return;
		}
		i++;
	}
}
void lexer(){
	type=-1; //reset the type
	int times=0;
	char tmp;
	bool E=false;
	bool floatMark=false;
	lexLen=0;
	getSpace();
	switch(charClass){
		case NOTE:
			if(next2Char=='/'){
				getChar();
				while(nextChar!='\n'){
					getChar();
				}
				getChar();
				line++;
				getSpace();
			}
			else if(next2Char=='*'){
				getChar();getChar();
				while(!(nextChar=='*' && next2Char=='/')){
					if(nextChar=='\n') line++;
					getChar();
				}
				getChar();getChar();
				getSpace();
			}
		case LETTER:
			addChar(); //the first word can not be digit
			getChar();
			while(charClass==LETTER || charClass==DIGIT || charClass==SIGN){
				addChar();
				getChar();
			}
			nextToken=ID; //!!!
			type=4;
			checkKeywords(); //!!
			break;
		case SIGN:
			addChar();
			getChar();
			while(charClass==LETTER || charClass==DIGIT || charClass==SIGN){
				addChar();
				getChar();
			} 
			nextToken=ID;
			type=4;
			break;
		case DIGIT:
			if(nextChar=='0' && next2Char=='x'){
				addChar();getChar();
				addChar();getChar();
				while(charClass==DIGIT){
					addChar();
					getChar();
				}
				nextToken=INI;
				type=0;
				break;
			}
			else{
				while(charClass==DIGIT || (charClass==POINT && !times && !E) || (nextChar=='E' && !E)){
					if(charClass==POINT){   //but if the point is in the  tail??
						floatMark=true;
						times=1;
						if(!isdigit(next2Char)){  // if the point is in the tail,then it isn't the float
							nextToken=INI;    // we should not use add and get function,because the '.' is not
							type=0;
							return;             //a part of INI;
						}
					}
					if(nextChar=='E'){
						if(next2Char=='+' || next2Char=='-' || isdigit(next2Char)){
							addChar(); getChar();
							floatMark=true;
						}
					}
					addChar();
					getChar();
				}
				nextToken=INI;
				type=0;
				if(nextChar=='L' || nextChar=='l'){
					addChar();
					getChar();
					nextToken=LONG;
					type=1;
				}	
				if(floatMark)	{
					nextToken=FLOAT;
					type=2;
					if(nextChar=='f' || nextChar=='F' || nextChar=='D'){
						addChar();
						getChar();
						if(nextChar=='D'){
							nextToken=DOUBLE;
							type=3;
						}
					}
				}
				break;
			}
		case UNKNOWN:
			checkSymbol();
			//we have done getChar in Function//!!!!
			break;
		case POINT:
			checkSymbol();
			break; 
		case CHAR:
			addChar();
			getChar();
			addChar();
			tmp=nextChar;
			getChar();
			if(nextChar=='\''){
				addChar();
				getChar();
				nextToken=tmp;
				type=6;
			}
			else{
				printf("ERROR:too much words before \'"); ///!!!
				nextToken=ERROR;
				type=9;
			}
			break;
		case STR:
			addChar();
			getChar();
			while(nextChar!='"'){
				addChar();
				getChar();
			}
			addChar();
			getChar();
			nextToken=STRING;
			type=5;
			break;
		case EOF:
			lexeme[0]='E';
			lexeme[1]='O';
			lexeme[2]='F';
			lexeme[3]='\0';
			nextToken=EOF;
			type=9;
			break;
	}
	printf("line %4d   <%6d , %s , %s>\n",line,nextToken,lexeme,whichtype[type]);
}
int main(int argc,char* argv[]){
	//char path[100];
	//printf("Please input the file path: ");
	//scanf("%s",path);
	
	if(argc<2){
		printf("ERROR:input file name is needed.\n");
		exit(0);
	}
	inFile = fopen(argv[1],"r");
	
	//inFile=fopen(path,"r");
	if(inFile==NULL){
		printf("ERROR:can not open file.\n");
		exit(0);
	}

	getChar();
	while(nextToken!=EOF){
		lexer();
	}
	return 0;
}
