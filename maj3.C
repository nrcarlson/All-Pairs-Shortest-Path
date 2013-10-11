/*
Author: Nick Carlson
Assignment: Major #3 - Trains (Graphs)
Due Date: 4/17/2012
Class: CSC 300 - Data Structures
Instructor: Dr. Krebsbach

Description: This program implements several functions to build
and access a graph, as well as a main driver to test them.
*/

//******************************
//INCLUDES
//******************************
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstring>
#include <cstdlib>
using namespace std;

//================================================================================================
//GLOBAL CONSTANT
//================================================================================================
const char train_file_name[] = "c:/cygwin/home/DSU/csc300data/trains.dat";		//used when reading in from this file
const char station_file_name[] = "c:/cygwin/home/DSU/csc300data/stations.dat";

//================================================================================================
//GLOBAL VARIABLES
//================================================================================================
int mid_count = 1;				//used for keeping track of the number of intermediate nodes
int station_count = 0;			//used for keeping track of the number of stations

//================================================================================================
//STRUCTURES
//================================================================================================
struct INTERMEDIATE{			//holds the intermediate values
	int interK;					//could also just be implemented as a global array, but I chose to do it this way
};

struct TRAINNODE{				//creates records for the cost matrix
	int DS;						//departure station
	int AS;						//arrival station
	int DT;						//departure time
	int AT;						//arrival time
	int cost;					//calculated cost
};

struct trainREC{				//creates records for the train station names and ID numbers
	int trainID;
	char stationName[50];
};

//=================================================================================================
//PROTOTYPES
//=================================================================================================
void hit_continue();
void clearScreen();
bool valid_menu_input(const int convert_flag, const int min, const int max, const char * valchar_list, const char c, const int n);
int console_reader(char * prompt, int input_type, char &c1, int &i1, double &d1);
int print_menu();
int from_menu();
int to_menu();
void BUILD(TRAINNODE cost[][100]);
void FILL(TRAINNODE c[][100], int DS,int AS, int DT, int AT);
void SHORTEST(int A[][100],TRAINNODE C[][100],int P[][100]);
void PATH(int i,int j,int P[][100],INTERMEDIATE kValues[100]);
void load_text_records(trainREC list[], int &size);
void parse_it(char line[], int &id, char name[]);
char* find_station_name(const int id,trainREC list[]);
int train_time(int A[][100],INTERMEDIATE kValues[]);
void clock_converter(int absolute_time);

//===================================================================================
// hit_continue : prompts the user to press a button before going back to main menu
//===================================================================================
void hit_continue(){
	char junk[11];									//an array for the input to go into
	cout<<""<<endl;
	cout<<"Press enter to continue:"<<endl;			//the user is told to hit enter but any character would work
	cin.getline(junk,10,'\n');						//the input is taken in and the program moves on
}

//========================================================================================
// clearScreen : prints enough space to clear the screen, used when displaying main menu
//========================================================================================
void clearScreen()
{
	cout << "\033[2J"; //this gives enough space to clear the screen
}

//========================================================================
// valid_menu_input : prevents bad user input from crashing the program
//========================================================================
bool valid_menu_input(const int convert_flag, const int min, const int max, const char * valchar_list, const char c, const int n)
{
    int length = strlen(valchar_list);
   // each case results in the function returning !!!!!
   switch (convert_flag)
   {
      case 0: return false;  // was invalid to start with
	  case 3:  // check double range
	             return true;
			 
	  case 2: if ((n >= min) && (n <= max)) // check int range
	             return true;
			  else
			     return false;
	  case 1: for( int i = 0; i < length; i++) // check for valid char in list
              {
                 if (valchar_list[i] == c)
                      return true;
              }
              return false;
    }
}

//========================================================================
// console_reader : takes in data from user and tests it for valid types
//========================================================================
int  console_reader(char * prompt, int input_type, char &c1, int &i1, double &d1)
{
   int convert_flag = 0;  					//set to invalid 
   char line_buffer[121]; 					//used to read a line of text up to 120 useable characters;
   cout << prompt;
   cin.getline(line_buffer,120,'\n');
   
   c1 = 'X';  i1 = -999; d1 = -999.99;		//clear output parameters
   
   char fchar = line_buffer[0]; 			//first character entered by user
   
   if (!isalnum(fchar))  					//check first character for letter or number
        return convert_flag; 				//invalid user input
   
   if (isalpha(fchar))  					//user typed a letter
     input_type = 1;
   
   switch (input_type)
   {
      case 1:								//expecting a single character
	          c1 = fchar;
			  convert_flag = 1;
			  break;
			  
	  case 2: 								//expecting an int
	          if (isdigit(fchar))
			  {
					i1 = atoi(line_buffer);
					convert_flag = 2;
              }
	          break;
			  
	  case 3: 								//expecting a double
	          if (isdigit(fchar))
			  {
			     d1 = atof(line_buffer);
			     convert_flag = 3;
              }				 
	          break;
			  
	  default:
	          cout << "Prog ERROR invalid input_type  CONTACT DEVELOPER !" << endl;
              exit(1);   					//violation of pre-condition
	}
    
	return convert_flag; 					//what was actually converted
}

//===============================================================
// print_menu : displays the main menu and accepts valid input
//===============================================================
int print_menu(){
	int flag;												//used in valid_menu_input
	int num;												//used for returning a value
    int n = 0;												//goes into console_reader
    char c = 'x';											//goes into console_reader
    double d = 999.99;										//goes into console_reader
	
	cout<<"Main Menu"<<endl;								//displays menu information
	cout<<"{Nick Carlson}"<<endl<<endl;						//author's name :)
	cout<<"1) List Stations and Numbers"<<endl;
	cout<<"2) Find Shortest Travel Time on Trains"<<endl;
	cout<<"3) Find Shortest Time Overall"<<endl;
	cout<<"4) EXIT"<<endl;
	
	do{
	flag = console_reader("Please enter your selection from the menu [1-4]: ",2,c,n,d);	//take in valid input
	}while(!valid_menu_input(flag,1,4,"",c,n));				//while input is valid
	
	if(flag == 2){
		num = n;											//pass back user's choice to move on
		return num;
	}else{
		return 0;											//if value is of incorrect type, return 0
	}
}

//================================================================================
// from_menu() : lets the user select the station they are traveling from
//================================================================================
int from_menu(){
	int flag;												//used in valid_menu_input
	int num;												//used for returning a value
    int n = 0;												//goes into console_reader
    char c = 'x';											//goes into console_reader
    double d = 999.99;										//goes into console_reader
	
	cout<<"From which station would you like to travel?"<<endl;
	cout<<"There are "<<station_count<<" stations to choose from. [0-"<<station_count-1<<"]"<<endl;	//let the user know what the choices are
	cout<<""<<endl;
	
	do{
	flag = console_reader("Please enter the station NUMBER: ",2,c,n,d);			//take in valid input
	}while(!valid_menu_input(flag,0,99,"",c,n));								//while input is valid
	
	if(flag == 2){
		num = n;																//pass back user's choice to move on
		return num;
	}else{
		return 0;																//if value is of incorrect type, return 0
	}
}

//================================================================================
//	to_menu() : lets the user select the station they are traveling to
//================================================================================
int to_menu(){
	int flag;												//used in valid_menu_input
	int num;												//used for returning a value
    int n = 0;												//goes into console_reader
    char c = 'x';											//goes into console_reader
    double d = 999.99;										//goes into console_reader
	
	cout<<"To which station would you like to travel?"<<endl;
	cout<<"There are "<<station_count<<" stations to choose from. [0-"<<station_count-1<<"]"<<endl;		//let the user know what the choices are
	cout<<""<<endl;
	
	do{
	flag = console_reader("Please enter the station NUMBER: ",2,c,n,d);		//take in valid input
	}while(!valid_menu_input(flag,0,99,"",c,n));							//while input is valid
	
	if(flag == 2){
		num = n;															//pass back user's choice to move on
		return num;
	}else{
		return 0;															//if value is of incorrect type, return 0
	}
}

// =========================================================================================================================
// parse_it : parsing function (would need to modify to handle particular needs in terms of argument number and types !!!
//==========================================================================================================================
void parse_it(char line[], int &id, char name[])
{
  char f1[50];
  char f2[50];
  sscanf(line,"%[^ ] %s",f1,f2);  	// use %s for last argument
                                    // [^,] - read all BUT the comma char
  id = atoi(f1);   					// convert c-string to integer
  strcpy(name,f2);
}

//============================================================================================
// load_text_records : loads in the station names and ID numbers and puts them into records
//============================================================================================
void load_text_records(trainREC list[], int &size)
{  
    int key_value;  // value read in from file
	int count = 0;  // number of numbers read

   // 1) DECLARE FILE VARIABLE (FV)
      fstream infile;
   
   // 2) ASSIGN PHYSIC FILE TO FV
   // 3) OPEN FILE IN CORRECT MODE
      infile.open(station_file_name,ios::in);
	  if (!infile)
	  { cout << "FILE  test2.txt  NOT OPENED ! " << endl;
	    exit(1);
	  }
	 
   // 4) USE FV IN I/O
        char line_buffer[120]; // used to read a line of text up to 119 useable characters;
          
	   infile.getline(line_buffer,120,'\n');  // attempt to read the first line of text (Prime the pump method)
       while (!infile.fail() && ( infile.gcount() > 2))  // handles blank lines with eoln char on them at end of the file
	   {  //parse the line
	      parse_it(line_buffer,list[count].trainID,list[count].stationName);		//information goes into records
		  count++;
		  infile.getline(line_buffer,120,'\n');  
	   }
	  
	   size = count;  // update the count
	   
   // 5) CLOSE FILE
       infile.close();
}

//======================================================================
// SHORTEST : Finds shortest path costs, builds A[][100] and P[][100]
//======================================================================
void SHORTEST(int A[][100],TRAINNODE C[][100],int P[][100]){
	int i = 0;
	int j = 0;
	int k = 0;									//variables to be used in for loops
	
	for(i = 0; i < station_count; i++){			//I ended up using station_count because the count from BUILD() wasn't...
		for(j = 0; j < station_count; j++){		//...accounting for the addition/deletion of stations
			A[i][j] = C[i][j].cost;				//copy costs from C to A
			P[i][j] = -1;						//establish flags of -1 in P
		}
	}
	
	for(i = 0; i < station_count; i++){
		A[i][i] = 0;							//distance to self is zero
	}
	
	for(k = 0; k < station_count; k++){
		for(i = 0; i < station_count; i++){
			for(j = 0; j < station_count; j++){
				if(A[i][k] + A[k][j] < A[i][j]){
					A[i][j] = A[i][k] + A[k][j];	//set shortest path
					P[i][j] = k;					//record the middle point
				}
			}
		}
	}
}

//================================================================
// PATH : finds intermediate values and stores them in an array
//================================================================
void PATH(int i,int j,int P[][100],INTERMEDIATE kValues[]){
	int k = P[i][j];							//set k to midpoint of i and j
	if(k == -1){
		//cout << "GROUNDED" << endl;
		return;									//ground if direct path exists
	}
	PATH(i,k,P,kValues);						//get midpoint between i and k
	kValues[mid_count].interK = k;				//process the midpoint
	mid_count++;								//increment count
	PATH(k,j,P,kValues);						//get the midpoint between k and j
}

//==================================================================================
// BUILD : Reads in data from trains.dat and builds the cost matrix using FILL()
//==================================================================================
void BUILD(TRAINNODE cost[][100]){
	int DS;				//variables to put read-in values into
	int AS;
	int DT;
	int AT;
	
	//1)
	fstream fv;
	//2) and 3)
	fv.open(train_file_name,ios::in);
	if(!fv){
		cout << "ERROR - FILE NOT FOUND!!!" << endl;
		exit(1);
	}
	//4)
	//Read in the values
	fv >> DS;  // use fv just like cin
	fv >> AS;
	fv >> DT;
	fv >> AT;
	
	while(!fv.fail()){
		FILL(cost,DS,AS,DT,AT);		//process DS thru AT
											//put into records of cost matrix
	    // attempted to read next pline of data
			fv >> DS;  						// use fv just like cin
	        fv >> AS;
	        fv >> DT;
	        fv >> AT;
	}
	//5)
	fv.close();
}

//======================================================================================
// FILL : Processes values into the correct places in TRAINNODE records for BUILD()
//======================================================================================
void FILL(TRAINNODE c[][100], int DS,int AS, int DT, int AT){
	c[DS][AS].DS = DS;			//assign values into correct places in TRAINNODE structure
	c[DS][AS].AS = AS;
	c[DS][AS].DT = DT;
	c[DS][AS].AT = AT;
	c[DS][AS].cost = AT-DT;		//calculate cost
}

//================================================================================================
// find_station_name : essentially a getter which retrieves the name of a station using the ID
//================================================================================================
char* find_station_name(const int id,trainREC list[]){
	int i = 0;
	for(i = 0; i < station_count; i++){		//traverse the trainREC records
		if(list[i].trainID == id){
			return list[i].stationName;		//found the station name
		}
	}
}

//===================================================================================================
// train_time : converts from absolute time to # hours and # minutes, this is total time on trains
//===================================================================================================
int train_time(int A[][100],INTERMEDIATE kValues[]){
	int hour = 0;				//hour and minutes are used later to find calculate and display clock values
	int minutes = 0;
	int time_to_go = 0;			//this is what will be converted
	int i = 0;
	
	while(kValues[i+1].interK != -1){								//dump costs from all legs of the trip into one variable
		time_to_go += A[kValues[i].interK][kValues[i+1].interK];	//use A array to find costs
		i++;
	}
	hour = time_to_go / 60;					//divide by 60 to get hours
	minutes = time_to_go % 60;				//mod 60 to get minutes
	if(hour == 1){																//to avoid saying "1 hours"
		cout << hour << " hour and " << minutes << " minutes." << endl;			//print out
	}else{
		cout << hour << " hours and " << minutes << " minutes." << endl;		//print out
	}
}

//=========================================================================
// clock_converter : converts absolute time to standard clock time AM/PM
//=========================================================================
void clock_converter(int absolute_time){
	int hour = 0;
	int minutes = 0;
	int i = 0;
	hour = absolute_time / 60;						//find hour number
	minutes = absolute_time % 60;					//find minutes number
	
	if(hour > 12){									//if hour > 12, must use PM
		hour = hour - 12;							//hour-12 gives us current hour
		cout << hour << ":";
		if(minutes < 10){										//if displaying single-digit minutes
			cout << setfill('0') << setw(2) << minutes;			//makes sure 12:00 prints instead of 12:0
			cout << " PM ";
		}else{
		cout << minutes << " PM ";
		}
	}else if(hour == 12){										//don't want to print 0:00 PM for noon
		cout << hour << ":";
		if(minutes < 10){										//if displaying single-digit minutes
			cout << setfill('0') << setw(2) << minutes;			//makes sure 12:00 prints instead of 12:0
			cout << " PM ";
		}else{
		cout << minutes << " PM ";
		}
	}else if(hour == 0){										//don't want to print 0:00 PM for noon
		cout << hour+12 << ":";
		if(minutes < 10){										//if displaying single-digit minutes
			cout << setfill('0') << setw(2) << minutes;			//makes sure 12:00 prints instead of 12:0
			cout << " AM ";
		}else{
		cout << minutes << " AM ";
		}
	}else{														//if hour is > 12, time is AM
	cout << hour << ":";
	if(minutes < 10){											//if displaying single-digit minutes
			cout << setfill('0') << setw(2) << minutes;			//makes sure 12:00 prints instead of 12:0
			cout << " AM ";
	}else{
		cout << minutes << " AM ";
		}
	}
}

//=======================================================================================================================
// train_routes : prints information for each leg of the trip using data from array build by PATH() and the cost matrix
//=======================================================================================================================
void train_routes(trainREC list[],TRAINNODE cost[][100],INTERMEDIATE kValues[]){
	for(int i = 0; i < mid_count; i++){
		cout << "Leave " << find_station_name(kValues[i].interK,list) << " at ";	//print departure station
		clock_converter(cost[kValues[i].interK][kValues[i+1].interK].DT);			//print departure time
		cout << "and arrive at ";
		cout << find_station_name(kValues[i+1].interK,list) << " at ";				//print arrival station
		clock_converter(cost[kValues[i].interK][kValues[i+1].interK].AT);			//print arrival time
		cout << endl;
	}
}

//======================================================================================================
// main : main driver for testing/using functions
//======================================================================================================
int main(void){
	int currMenu = 1;								//used to control menu switch statement
	int operation = 0;								//used to direct the user from the main menu
	int fromStation = 0;							//stores the DS number
	int toStation = 0;								//store the AS number
	trainREC list[100];								//creates a list of trainREC records
	load_text_records(list,station_count);			//fill list with data from stations.dat
	TRAINNODE cost[100][100];						//create the array for the cost matrix
	INTERMEDIATE kValues[100];						//create the array for the intermediate values
	for(int i = 0; i<100; i++){
		kValues[i].interK = -1;						//set intermedate values array to -1 so we can only use what we fill up
	}
	int A[100][100];								//A array for shortest()
	int P[100][100];								//P array for shortest() and path()
	int i = 0;
	int j = 0;
	
	for(i = 0; i < 100; i++){
		for(j = 0; j < 100; j++){
			cost[i][j].cost = 500;					//set all costs to virtual infinity to begin with
		}
	}
	
	BUILD(cost);								//build cost matrix
	SHORTEST(A,cost,P);							//use cost matrix to build A and P
	
	do{clearScreen();
		switch(currMenu){
			case 1: operation = print_menu();		//display main menu
					if(operation == 0){
						currMenu = 0;
					}else{
						operation = operation+1;	//accept user input and move forward
						currMenu = operation;
						}
					break;
			case 2:	cout << endl;
					cout << "Stations: " << "\t" << "\t" << "ID numbers: " << endl;		//formatting for printing stations
					cout << endl;
					for(int n = 0; n < station_count; n++){
						if((list[n].trainID == 0) || (list[n].trainID == 3) || (list[n].trainID == 7)){		//these 3 needed an extra tab
							cout << list[n].stationName << "\t" << "\t" << "\t" <<list[n].trainID << endl;
						}else{
							cout << list[n].stationName << "\t" << "\t" <<list[n].trainID << endl;		//regular print
						}
					}
					hit_continue();	//prompt user to press a button to move on
					currMenu = 1;
					break;
			case 3:	cout << endl;
					fromStation = from_menu();			//ask user for departure station
					clearScreen();
					cout << endl;
					toStation = to_menu();				//ask user for arrival station
					clearScreen();
					mid_count = 1;						//so that we start filling the intermidiate array from index 1 instead of 0
					for(int i = 0; i<100; i++){
						kValues[i].interK = -1;			//reset kValues array for new group of intermediates
					}
					if((fromStation >= station_count) || (toStation >= station_count)){		//if station number entered exceeds number of stations
						cout << endl;
						cout << "REPORT 1: Shortest time riding on Trains." << endl;
						cout << endl;
						cout << "At least one of the stations numbers you've entered is invalid." << endl;	//input is invalid
						cout << "Please try again." << endl;
						cout << endl;
						cout << "END OF REPORT" << endl;
					}else{
						if(A[fromStation][toStation] == 500){		//if no path exists
							cout << endl;
							cout << "REPORT 1: Shortest time riding on Trains." << endl;
							cout << endl;
							cout << "Sorry, you can't get there from here." << endl;
							cout << endl;
							cout << "END OF REPORT" << endl;
						}else if(A[fromStation][toStation] == 0){	//if departure station and arrival station are the same
							cout << endl;
							cout << "REPORT 1: Shortest time riding on Trains." << endl;
							cout << endl;
							cout << "You're already there!" << endl;
							cout << "Time on trains is 0 hours and 0 minutes." << endl;
							cout << endl;
							cout << "END OF REPORT" << endl;
						}else{													//if we can find a shortest path
							PATH(fromStation,toStation,P,kValues);				//fill up intermediate array
							kValues[0].interK = fromStation;					//set first value in path array to departure station
							kValues[mid_count].interK = toStation;				//set last element in array to arrival station
							cout << endl;
							cout << "REPORT 1: Shortest time riding on Trains." << endl;
							cout << endl;
							cout << "To go from " << find_station_name(fromStation,list) << " to " << find_station_name(toStation,list);
							cout << " you will need to ride on trains for" << endl;		//print report
							train_time(A,kValues);				//print time on trains
							cout << endl;
							cout << "You will take the following trains:" << endl;
							cout << endl;
							train_routes(list,cost,kValues);	//prints legs of the trip
							cout << endl;
							cout << "END OF REPORT" << endl;
						}
					}
					hit_continue();
					currMenu = 1;
					break;
			case 4:	cout << endl;
					cout << "This feature is not yet available. Look for it in a future update." << endl;	//no phase II at this time
					hit_continue();
					currMenu = 1;
					break;
			case 5:	currMenu = 0;	//exit the program
					break;
		}
	}while(currMenu != 0);				//while quit option has not been selected
	
	return 0;							//end of program
}
