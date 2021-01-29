/* a-Greed (Greed Clone)
 * Freely based on "Greed" by Eric S. Raymond <esr@snark.thyrsus.com>
 * 
 * From this version: Scum Tips <scum.tips@gmail.com>
 * Review by posa
 * Last update: 2021-01-29
 * */
 
#define VERSION 1.0
 
/* Libraries */
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#ifdef _WIN32
	#include <conio.h>
#else
	#include <stdio.h>
#endif

/* Constants */
#define MAX_COL 60			//Might be 60
#define MAX_ROW 15			//Might be 15
#define MAX_STRING 61		//Shouldn't need to be longer than col+1
#define DECIMAL_DIGITS 9	//Should be 9
// default key set
#define KEY_NW '7'
#define KEY_N '8'
#define KEY_NE '9'
#define KEY_E '6'
#define KEY_W '4'
#define KEY_SW '1'
#define KEY_S '2'
#define KEY_SE '3'
#define KEY_ESC 'q'
#define KEY_RESTART 'r'
#define KEY_HELP 'h'
#define KEY_NO_NUMPAD 'c'
//format constants
#define EMPTY 0
#define CURSOR -1
#define BLOCK_VERTICAL '='
#define BLOCK_HORIZONTAL '|'
#ifdef _WIN32
	#define CORNER_NW "."
	#define CORNER_NE "."
	#define CORNER_SW "*"
	#define CORNER_SE "*"
#else
	#define CORNER_NW "╒"
	#define CORNER_NE "╕"
	#define CORNER_SW "╘"
	#define CORNER_SE "╛"
#endif
//printf stylers
#define CURSOR_STYLE		"\x1b[31;47m"	//cursor, highlited
#define ANSII_WHITE_BOLD	"\x1b[37;1m"
#define ANSII_RED			"\x1b[31m"
#define ANSII_GREEN			"\x1b[32m"
#define ANSII_MAGENTA		"\x1b[35m"
#define ANSII_CYAN			"\x1b[36m"
#define ANSII_WHITE			"\x1b[37m"
#define RGB_YELLOW			"\x1b[38;2;200;200;50m"
#define RGB_ORANGE			"\x1b[38;2;200;60;60m"	//code for RGB: ...38;2;R;G;Bm
#define RGB_PURPLE			"\x1b[38;2;127;50;255m"
#define RGB_DARK_BLUE		"\x1b[38;2;50;100;255m"
//#define XIPI_COLOR_RED	"\x1b[31;1m\x1b[47m"
#define STYLE_RESET			"\x1b[0m"

/* Types */
typedef enum {UNDEFINED, NUMPAD, J_BASED}tInputType;
typedef struct{
	tInputType set;
	char NW;
	char N;
	char NE;
	char E;
	char SE;
	char S;
	char SW;
	char W;
	char esc;
	char restart;
	char instructions;
	char mapping;
}tKey;
typedef struct{
	int row;
	int col;
}tCursor;

/* Headers */
void printMatrix(int matrix[MAX_COL][MAX_ROW]);
void printFooter(bool instructions, tKey key);
void printHeader(int points);
void printInstructions(tKey *key);
bool canStillMove(int matrix[MAX_COL][MAX_ROW], tCursor cursor);
bool canMoveThere(tCursor target);
void createMatrix(int matrix[MAX_COL][MAX_ROW]);
void readRaw(char *input);
void clearScreen();
void printLine(bool top);
void printCentered(char string[MAX_STRING]);
void printCenteredComplex(char string[MAX_STRING], char constant, char complexString[MAX_STRING]);
void remapKeys(tKey *key);

int main(int argc, char **argv){
	/* definitions */
	int matrix[MAX_COL][MAX_ROW];
	tCursor cursor;		//current cursor position
	tCursor target;		//next desired position
	tCursor direction;	//sets movement direction 
	
	char input;			//register user input
	int points;			//score count
	
	tKey key;			//manages key mapping
	
	bool guiInput;		//separates gui inputs from "gaming" ones
	bool instructions; 	//toggles instructions printing
	bool restart;		//handles new games
	bool gameEnd;		//checks for game gameEnd 
	

	/* init */
	srand(time(0));			//defining randomness in execution time
	gameEnd = false;		//controls program flow
	restart = true;			//needs to "restart" in order to start 
	instructions = false;	//defines initial state of help (off by default)
	key.set = UNDEFINED;	//initializes the key set in order to ensure it will get mapped
	remapKeys(&key);		//defines default key mapping
	
	
	/* Game Start */
	while(!gameEnd ){
		
		/* Initialization or restart */
		if(restart){
			points = 0;			//setting score to 0
			restart = false;	//cleans up restart status
			
			/* Creation of the initial matrix */
			createMatrix(matrix);
			/* Initial placement of the cursor */
			cursor.col = rand() % MAX_COL;
			cursor.row = rand() % MAX_ROW;
			
		}
		
		/* Prints */
		matrix[cursor.col][cursor.row] = CURSOR;	//Makes cursor visible
		clearScreen();
		if(!instructions){
			printHeader(points);
			printMatrix(matrix);
			printFooter(instructions, key);
		}
		else{
			printInstructions(&key);
		}
		
		/* User interaction */
		readRaw(&input);	//captures key strokes
		if 		(input == key.NW){								direction.col = -1;	direction.row = -1;	}
		else if	(input == key.N){								direction.col = 0;	direction.row = -1;	}
		else if (input == key.NE){								direction.col = 1;	direction.row = -1;	}
		else if (input == key.E){								direction.col = 1;	direction.row = 0;	}
		else if (input == key.SE){								direction.col = 1;	direction.row = 1;	}
		else if (input == key.S){								direction.col = 0;	direction.row = 1;	}
		else if (input == key.SW){								direction.col = -1;	direction.row = 1;	}
		else if (input == key.W){								direction.col = -1;	direction.row = 0;	}
		else if (input == key.instructions){ 					instructions = !instructions; 			}
		else if (input == key.mapping && instructions){ 		guiInput = true; 	remapKeys(&key); 	}
		else if (input == key.esc){ 							guiInput = true; 	gameEnd = true;		}
		else if (input == key.restart){ 						guiInput = true; 	restart = true; 	}
		else	{												guiInput = true;						}

		/* Automatic actions after User Interaction */
		if(!guiInput && !instructions){
			target.col = cursor.col + direction.col*matrix[cursor.col+direction.col][cursor.row+direction.row];
			target.row = cursor.row + direction.row*matrix[cursor.col+direction.col][cursor.row+direction.row];
			if(canMoveThere(target)){
				while (target.col != cursor.col || target.row != cursor.row){
					matrix[cursor.col][cursor.row] = EMPTY;
					cursor.col += direction.col;
					cursor.row += direction.row;
					if(matrix[cursor.col][cursor.row] != EMPTY){
						points++;
					}
				}
			}
		}
		else{
			guiInput = false;	//resets input boolean for the next round
			direction.col = 0;	//avoids horizontal movement during instructions display
			direction.row = 0;	//avoids vertical movement during instructions display
		}

		/* Completion check */
		if(!canStillMove(matrix, cursor)){
			matrix[cursor.col][cursor.row] = CURSOR;
			while(input != key.restart && input != key.esc){
				clearScreen();
				printHeader(points);
				printMatrix(matrix);
				printf(ANSII_RED "\t\tGame Over! Do you want to (%c)-restart or (%c)-quit?\n" STYLE_RESET, key.restart, key.esc);
				readRaw(&input);
				if (input == key.restart)	{	restart = true; }
				else if (input == key.esc)	{	gameEnd = true; }
			}
		}		
	}//gameEnd game (while)
	
	/* Exiting program */
	clearScreen();
	return 0;
}
/* Creates a matix of numbers (WIP) */
void createMatrix(int matrix[MAX_COL][MAX_ROW]){
	for( int i = 0; i < MAX_ROW; i++){
		for( int j = 0; j < MAX_COL; j++){
			matrix[j][i] = (rand() % DECIMAL_DIGITS) + 1;	//correction of values 0-8 to 1-9
		}
	}
}
/* Prints a formatted vertical cage line */
void printLine(bool top){
	if(top)	printf("\t" CORNER_NW);
	else 	printf("\t" CORNER_SW);
	
	for( int j = 0; j < MAX_COL; j++){
		printf("%c",BLOCK_VERTICAL);
	}
	if(top)	printf(CORNER_NE "\n");
	else 	printf(CORNER_SE "\n");
}
/* Prints the board matrix */
void printMatrix(int matrix[MAX_COL][MAX_ROW]){
	/* Upper border */
	printLine(true);	//true establishes that it's top
	/* Matrix print */
	for( int i = 0; i < MAX_ROW; i++){
		printf(STYLE_RESET"\t%c", BLOCK_HORIZONTAL);
		for( int j = 0; j < MAX_COL; j++){
			switch(matrix[j][i]){
				case CURSOR: printf(CURSOR_STYLE"@"STYLE_RESET); 		break;
				case 1:		printf(ANSII_GREEN "%d", matrix[j][i]);		break;
				case 2:		printf(ANSII_CYAN "%d", matrix[j][i]);		break;
				case 3:		printf(RGB_DARK_BLUE "%d", matrix[j][i]);	break;
				case 4:		printf(RGB_PURPLE "%d", matrix[j][i]);		break;
				case 5:		printf(ANSII_WHITE "%d", matrix[j][i]);		break;
				case 6:		printf(ANSII_MAGENTA "%d", matrix[j][i]);	break;
				case 7:		printf(RGB_YELLOW "%d", matrix[j][i]);		break;
				case 8:		printf(RGB_ORANGE "%d",matrix[j][i]);		break;
				case 9:		printf(ANSII_RED "%d", matrix[j][i]);		break;
				default: printf(" ");
			}//print switch
		}//line for
		/* Line break */
		printf(STYLE_RESET"%c\n", BLOCK_HORIZONTAL);
	}	// row for (whole table)
	/* Lower border */
	printLine(false);	//false establishes that it's bottom
}
/* Prints a centered line (no variables) */
void printCentered(char string[MAX_STRING]){
	printf("\t%*s",	(MAX_COL+strlen(string)+1)/2, string);
}
/* Prints a centered line containing a float variable (1 decimal) */
void printCenteredWithFloat(char string[MAX_STRING], float constant, char complexString[MAX_STRING]){
	printf("\t%*s",	((MAX_COL+strlen(string)+strlen(complexString)-6)/2), string);
	printf("%.1f%s", constant, complexString);
}
/* Prints a centered line containing one char variable */
void printCenteredWithChar(char string[MAX_STRING], char constant, char complexString[MAX_STRING]){
	printf("\t%*s",	((MAX_COL+strlen(string)-strlen(complexString)+2)/2), string);
	printf("%c%s", constant, complexString);
}
/* Prints a centered line containing 3 char variables */
void printCenteredWith3Char(char string[MAX_STRING], char constant, char complexString[MAX_STRING], char constant2, char complexString2[MAX_STRING], char constant3, char complexString3[MAX_STRING]){
	printf("\t%*s",	((MAX_COL+strlen(string)-strlen(complexString)-strlen(complexString2)-strlen(complexString3)-2)/2), string);
	printf("%c%s", constant, complexString);
	printf("%c%s", constant2, complexString2);
	printf("%c%s", constant3, complexString3);
}
/* Prints the game header */
void printHeader(int points){
	/* Header print */
	printCenteredWithFloat(STYLE_RESET"### SCUM TIPS a-greed v",VERSION," ###\n\n");
	printf(STYLE_RESET"\t%*s %4d (%6.2f%%)\n",(MAX_COL-6)/2, "Points:", points, (((float)points*100.0)/((float)(MAX_COL*MAX_ROW-1))));
}
/* Prints the instructions page */
void printInstructions(tKey *key){
	printf("\n");
	printCenteredWithChar("Press (", key->instructions, ") for going back\n\n");
	printCentered("The game is mapped for numberpad directions:\n");
	printCenteredWith3Char(	"(", key->NW,	")(",	key->N,	")(",	key->NE,	")\n");
	printCenteredWith3Char(	"(", key->W,		") ",	'x',	" (",	key->E,	")\n");
	printCenteredWith3Char(	"(", key->SW,	")(",	key->S,	")(",	key->SE,	")\n");
	printCenteredWith3Char(	"(", key->restart,	")restart (",	key->esc,	")quit (", key->mapping,	")alternative mapping\n\n");
	printCentered("Try to clean the screen from those pesky numbers!\n");
	printCentered("Everything found in your way will get deleted\n");
	printCentered("753667     753667\n");
	printCentered("1@2173 //  1__@73\n");
	printCentered("345366     345366\n");
	printCentered("Jump as many positions as the number in your direction\n");
	printCentered(" @1->  //  @4---->\n");
	printCentered("The game ends when there are no more moves available\n\n");
	printCentered("Have fun!\n");
}
/* Prints the game footer */
void printFooter(bool instructions, tKey key){
	printf(ANSII_WHITE_BOLD);
	printCenteredWithChar("Press (", key.instructions, ") for help\n");
	printf(STYLE_RESET);
}
/* Returns a boolean stating if there are still available moves */
bool canStillMove(int matrix[MAX_COL][MAX_ROW], tCursor cursor){
	bool canMove = true;
	
	if(		//E
			 (	(cursor.col == MAX_COL-1	)?true:(	cursor.col + matrix[cursor.col+1][cursor.row] 	>= MAX_COL	||	matrix[cursor.col+1][cursor.row] == 0 )
			 )
			&& //SE
			 (	(cursor.col == MAX_COL-1	)?true:(	cursor.col + matrix[cursor.col+1][cursor.row+1]	>= MAX_COL	)
				||
				(cursor.row == MAX_ROW-1	)?true:(	cursor.row + matrix[cursor.col+1][cursor.row+1]	>= MAX_ROW	||	matrix[cursor.col+1][cursor.row+1] == 0	)
			 )
			&& //S
			 (	(cursor.row == MAX_ROW-1	)?true:(	cursor.row + matrix[cursor.col][cursor.row+1]	>= MAX_ROW	||	matrix[cursor.col][cursor.row+1] == 0 )
			 )
			&& //SW
			 (	(cursor.col == 0			)?true:(	cursor.col - matrix[cursor.col-1][cursor.row+1]	< 0	)
				||
				(cursor.row == MAX_ROW-1	)?true:(	cursor.row + matrix[cursor.col-1][cursor.row+1]	>= MAX_ROW	||	matrix[cursor.col-1][cursor.row+1] == 0	)
			 )
			&& //W
			 (	(cursor.col == 0			)?true:(	cursor.col - matrix[cursor.col-1][cursor.row]	< 0			||	matrix[cursor.col-1][cursor.row] == 0)
			 )
			&& //NW
			 (	(cursor.col == 0			)?true:(	cursor.col - matrix[cursor.col-1][cursor.row-1]	< 0	)	
				||
				(cursor.row == 0			)?true:(	cursor.row - matrix[cursor.col-1][cursor.row-1]	< 0			||	matrix[cursor.col-1][cursor.row-1] == 0)
			 )
			&& //N
			 (	(cursor.row == 0			)?true:(	cursor.row - matrix[cursor.col][cursor.row-1]	< 0			||	matrix[cursor.col][cursor.row-1] == 0)
			 )
			&& //NE
			 (	(cursor.col == MAX_COL-1	)?true:(	cursor.col + matrix[cursor.col+1][cursor.row-1]	>= MAX_COL	)
				||
				(cursor.row == 0			)?true:(	cursor.row - matrix[cursor.col+1][cursor.row-1]	< 0			||	matrix[cursor.col+1][cursor.row-1] == 0)
			 )
		){
			canMove = false;
		}
	return canMove;
}
/* Returns a boolean stating whether a certain target move can be made */
bool canMoveThere(tCursor target){
	bool canMove;
	
	canMove = (target.col >= 0) && (target.col < MAX_COL) && (target.row >= 0) && (target.row < MAX_ROW);
	
	return canMove;
}
/* Allows raw keyboard input */
void readRaw(char *input){
	#ifdef _WIN32
		*input = _getch();
	#else
		system("/bin/stty raw"); //enables raw keyboard input in linux
		*input = getchar();
		system("/bin/stty cooked"); //enables raw keyboard input in linux
	#endif
}
/* Clears the command prompt screen */
void clearScreen(){
	#ifdef _WIN32
		system("cls");
	#else
		system("clear");
	#endif
}
/* Remaps the current key set (useful for keyboards that lack numberpads */
void remapKeys(tKey *key){
	if (key->set != NUMPAD){
		key->set = NUMPAD;
		key->NW = KEY_NW;
		key->N = KEY_N;
		key->NE =KEY_NE;
		key->E = KEY_E;
		key->SE = KEY_SE;
		key->S = KEY_S;
		key->SW = KEY_SW;
		key->W = KEY_W;
		key->mapping = KEY_NO_NUMPAD;
		key->esc = KEY_ESC;
		key->restart = KEY_RESTART;
		key->instructions = KEY_HELP;
	}
	else{
		key->set = J_BASED;
		key->NW = 'y';
		key->N = 'u';
		key->NE ='i';
		key->E = 'k';
		key->SE = ',';
		key->S = 'm';
		key->SW = 'n';
		key->W = 'h';
		key->mapping = KEY_NO_NUMPAD;
		key->esc = KEY_ESC;
		key->restart = KEY_RESTART;
		key->instructions = 'a';
	}
}