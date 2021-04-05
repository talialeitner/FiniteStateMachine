# include <stdio.h>
# define test 0 // 1 to print test statements and 0 not to print test statements
# define unit 0 // 1 for unit tests and 0 not for unit tests

// declare functions
int inputFunc(char inputArray[], int *i, int *inputCount, char *prev);
void convert(int start, int i, int index, int ten, int j, char line[], int *temp);
void setState(int j, int trans, int defArray[][3], char curInput, int prevState, int *curState);
void testFunctions();

// thank you stack overflow - argc is the number of input files, argv is an array of the input files
int main(int argc, char *argv[])
{

  // run the unit test function if unit is defined as 1
  if (unit == 1)
    testFunctions();
  
  // error if more than two input files other than the fsm program -- exit the program
  if (argc > 3){
    printf("Error: too many input files passed\n");
    return 0;
      }
  
  // the initial state is always 0 - all states are >= 0
  int curState = 0;
  int prevState= 0;

  // open the two input files
  FILE *stateStream;
  FILE *inputStream;
  stateStream = fopen(argv[1], "r");
  inputStream = fopen(argv[2], "r");

  // array to hold all of the inputs -- max 250
  char inputArray[250];
  int i=0;
  int inputCount = 0;
  char prev = '\n';
  // value returned by the input function
  int test1;
  if (inputStream != NULL){
    // inputArray can only hold 250 chars so while loop can only loop 250 times
    while (fscanf(inputStream, "%c", &inputArray[i]) != EOF && inputCount < 250){
      
      test1 = inputFunc(inputArray, &i, &inputCount, &prev);
      
      // if inputFunc returns 0 then there was an error
      if (test1==0)
	return 0;
    }
    fclose(inputStream);
  }
  // error if inputs file couldn't be opened -- exit the program
  else{
    printf("Error: program was unsuccessful in opening inputs file\n");
    return 0;
  }

  // 2D array to hold all of the definitions
  // if 50 states and 26 inputs - max 1300 instructions
  // string = number : letter > number -- 100 characters should be large enough to hold but can be changed below
  int MAX = 100;
  // string array to hold the current line
  char line[MAX];
  // int array to hold the current state, ascii value of input, and the new state
  int defArray[1300][3];
  // keep track of the number of transitions - the number of lines in the defintion file
  int trans = 0;
  // keep track of important indexes
  int index1;
  int index2;
  int index3;
  // set temps to 0 at begining of each loop
  int temp1 = 0;
  int temp2 = 0;
  // the index in defArray
  int ind = 0;
  // these vars will be used to convert the chars into ints
  int ten;
  int j;
  if (stateStream != NULL){
    // fgets takes all characters until the end of line or until MAX number of characters
    while (fgets(line, MAX, stateStream) != NULL){
      
      // test statement to see if the string holds the new line
      if (test == 1){
	if (ind==0)
	  printf("Definitions:\n");
	// note that '\n' at end of string
	printf("%s", line);
      }
      
      // find the index for the current state's end, which is signaled by the ':'
      // find the index for the new state's start, which is signaled by the '>', and its end, which is signaled by the '\n'
      for (i=0; i<MAX; i++){
	if (line[i] == ':')
	  index1 = i;
	else if (line[i] == '>')
	  index2 = i;
	else if (line[i] == '\n'){
	  index3 = i;
	  break;
	}
      }

      // error if missing a ':' or '>' -- exit the program
      // : should be at least at index 1 because state :
      // > should be at least at index 3 because state : input >
      if (index1 < 1 || index2 < 3){
	printf("Error: invalid definitions file\n");
	return 0;
      }
      
      // convert the current state's digits into an int
      // the current state is from index 0 until index1 in line
      convert(0, i, index1, ten, j, line, &temp1);
      
      // save the current state at index 0
      defArray[ind][0] = temp1;
      // reset temp1 for next loop
      temp1=0;

      // convert the new state's digits into an int
      // the new state is from index2+1 until index3 in line
      convert(index2+1, i, index3, ten, j, line, &temp2);
      
      // save the input value as an ascii value
      defArray[ind][1] = (int)line[index2-1];
      // save the new state at index 2
      defArray[ind][2] = temp2;
      // reset temp2 for next loop
      temp2=0;

      // test statement to make sure all three numbers were saved in defArray
      if (test==1){
	for (i=0; i<3; i++)
	  printf("place %d : %d ",i, defArray[ind][i]);
      }
      
      // add one to the outer index of defArray and one to the transition count
      ind++;
      trans++;
    }
    fclose(stateStream);
  }
  // error if definitions file couldn't be opened -- exit the program
  else{
    printf("Error: program was unsuccessful in opening definitions file\n");
    return 0;
  }

  
  // at the BEGINNING of the program:
  
  printf("processing FSM definition file %s\n", argv[1]);
  printf("FSM has %d transitions\n", trans);
  printf("processing FSM inputs file %s\n", argv[2]);


  // in the MIDDLE of the program:
  
  // keep track of what number transition it is up to
  int transCount = 0;
  // keep track of the most recent input
  char curInput = 'a';
  for (i=0; i < inputCount; i++){
    // make sure it's a valid input value (ie. it's between a-z)
    if (inputArray[i] >= 79 && inputArray[i] <= 122){
      
      // set the current input
      curInput = inputArray[i];
      // test statement to make sure input was set
      if (test==1)
	printf("input: %c\n",curInput);

      // change the previous state
      prevState = curState;
      // test statment to make sure state was set
      if (test==1)
	printf("previous state: %d\n",prevState);

      // change the current state
      setState(j, trans, defArray, curInput, prevState, &curState);
      
      printf("\tat step %d, input %c transitions FSM from state %d to state %d\n", transCount, curInput, prevState, curState);
      transCount++;
    }
    // error if it's an invalid input value (ie. it's not a-z)
    else{
      printf("Error: %c is an invalid input\n", inputArray[i]);
    }
  }


  // at the END of the program:
  printf("after %d steps, state machine finished successfully at state %d\n", transCount, curState);

}

// this function evaluates each of the lines of the inputs file
// returns 1 if worked and 0 if there was an error
int inputFunc(char inputArray[], int *i, int *inputCount, char *prev){
  // include all chars in the array except for new line chars
      if (inputArray[*i] != '\n'){
	  // test statement - see if array holds all of inputs
	  if (test==1){
	    if (*i==0)
	      printf("Inputs:\n");
	    printf("%c\n",inputArray[*i]);
	  }
	  // if there is more than one char on a line then prev won't be equal to '\n' -- exit the program
	  if (*prev != '\n'){
	    printf("Error: invalid inputs file\n");
	    return 0;
	  }
	  *prev = inputArray[*i];
	  *i = *i+1;
	  *inputCount = *inputCount+1;
	  return 1;
      }
      // set prev equal to the '\n'
      else{
	*prev = inputArray[*i];
	return 1;
      }
}

// this funtion converts an array of char digits into a single int
// the function starts at index "start" of the array and loops until "index"
void convert(int start, int i, int index, int ten, int j, char line[], int *temp){
  for (i = start; i<index; i++){
    
    if ((index-1-i) >0){
      ten = 10;
      // only multiply if 100's place or greater
      for (j=0; j<index-2-i; j++)
	ten *= 10;
      // test statemebt to see if the ten's value is correct
      if (test==1)
	printf("ten: %d\n", ten);
      // subtract 48 to convert the ascii value of the digit's char into the correct int
      *temp += (((int)line[i]-48) * (ten));
      if (test==1)
	printf("temp = %d", *temp);
      //return 1;
    }
    else{
      *temp += ((int)line[i]-48);
      //return 1;
    }
  }
}

// this function finds the defintion that matches the current state and input
// it then changes the current state
void setState(int j, int trans, int defArray[][3], char curInput, int prevState, int *curState){
  for (j=0; j<=trans; j++){
	// test function to see the definitions tested
	if (test==1)
	  printf("0: %d 1: %d 2: %d curInput: %d\n", defArray[j][0], defArray[j][1], defArray[j][2], (int)curInput);
	
	// check whether previous state and current input of the definition match
	if ((prevState == defArray[j][0]) && ((int)curInput == defArray[j][1])){
	  // change the current state
	  *curState = defArray[j][2];
	  break;
	}
  }
}

// this function runs unit tests for each of the other functions called by main
void testFunctions(){

  // unit test for inputFunc:

  // test 1: when two chars are in a row
  char inputArray[250];
  int i = 10;
  int inputCount = i;
  inputArray[i] = 'b';
  char prev = 'a';
  int tests;
  
  tests = inputFunc(inputArray, &i, &inputCount, &prev);

  // should return 0 for error
  if (tests == 0)
    printf("Test 1 of inputFunc was a Success\n");
  else
    printf("Test 1 of inputFunc Failed\n");


  // test 2: when two chars are seperated by a new line char
  prev = '\n';
  
  tests = inputFunc(inputArray, &i, &inputCount, &prev);

  // should return 1 and check if i, inputCount and prev were changed correcly
  if (tests == 1 && i == 11 && inputCount == 11 && prev == 'b')
    printf("Test 2 of inputFunc was a Success\n");
  else{
    printf("Test 2 of inputFunc Failed\n");
    printf("i: %d inputCount: %d prev %c\n", i, inputCount, prev);
  }


  // unit test for convert:

  // test 1: convert part of a string array into a single int
  int index1 = 1;
  int index2 = 3;
  int index3 = 5;
  int ten;
  int j;
  char line[100] = "12345";
  int temp = 0;

  convert(index1, i, index3, ten, j, line, &temp);

  if (temp == 2345)
    printf("Test 1 of convert was a Success\n");
  else{
    printf("Test 1 of convert Failed\n");
    printf("temp: %d\n", temp);
  }

  // test 2: see if program crashes when char is not a digit
  char line2[100] = "1@#$5";
  temp = 0;
  convert(index1, i, index3, ten, j, line2, &temp);
  printf("Test 2 of convert was a Success\n");
  if (test==1)
    printf("temp: %d\n", temp);

  
  // unit test for setState:

  // test 1: see if state was changed
  int trans = 3;
  int defArray[1300][3];
  defArray[1][0] = 100;
  defArray[1][1] = 'a';
  defArray[1][2] = 200;
  char curInput = 'a';
  int prevState = 100;
  int curState = 0;

  setState(j, trans, defArray, curInput, prevState, &curState);
  if (curState == 200)
    printf("Test 1 of setState was a Success\n");
  else{
    printf("Test 1 of setState Failed\n");
    printf("curState: %d", curState);
  }

  // test 2: see if program crashed when matching definition not found
  curInput = 'b';

  setState(j, trans, defArray, curInput, prevState, &curState);
  printf("Test 2 of setState was a Success\n");
  if (test==1)
    printf("curState: %d\n", curState);

}
