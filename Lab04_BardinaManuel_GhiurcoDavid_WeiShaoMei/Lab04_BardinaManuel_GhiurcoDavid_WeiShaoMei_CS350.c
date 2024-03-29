// Manuel Bardina, David Ghiurco, and  Wei Shao Mei Section 2

// CS 350, Fall 2014
// Lab 4: SDC Simulator
//
// Illinois Institute of Technology, (c) 2014, James Sasaki

#include <stdio.h>
#include <stdlib.h>       // For error exit()

// Prototypes [note the functions are also declared in this order]
//
	int main(int argc, char *argv[]);
	void initialize_control_unit(int reg[], int nreg);
	void initialize_memory(int argc, char *argv[], int mem[], int memlen);
	FILE *get_datafile(int argc, char *argv[]);

	void dump_control_unit(int pc, int ir, int running, int reg[], int nreg);
	void dump_memory(int mem[], int memlen);
	void dump_registers(int reg[], int nreg);

	int read_execute_command(int reg[], int nreg, int mem[], int memlen);
	int execute_command(char cmd_char, int reg[], int nreg, int mem[], int memlen);
	void help_message(void);
	void many_instruction_cycles(int nbr_cycles, int reg[], int nreg, int mem[], int memlen);
	void one_instruction_cycle(int reg[], int nreg, int mem[], int memlen);
void exec_HLT(int reg_R, int addr_MM, int instr_sign, int reg[], int nreg, int mem[], int memlen);
	// replace comment with code

// CPU declarations
//
#define NREG 10
#define MEMLEN 100
// Note horrible use of global variables
	int pc;               // Program counter
	int ir;               // Instruction register
        int running;          // true iff instruction cycle is active
	int reg[NREG];        // general-purpose registers
	int mem[MEMLEN];      // main memory
        char *datafile_nameglobe;


// Main program: Initialize the cpu, read initial memory values,
// and execute the read-in program starting at location 00.
//
int main(int argc, char *argv[]) {
	printf("CS 350 Lab 4\n Bardina_Manuel, Mei_WeiShao, Ghiurco_David");
	initialize_control_unit(reg, NREG);
	initialize_memory(argc, argv, mem, MEMLEN);

	char *prompt = "> ";
	printf("\nBeginning execution; type h for help\n%s", prompt);

	int done = read_execute_command(reg, NREG, mem, MEMLEN);
	while (!done) {
		printf("%s", prompt);
		done = read_execute_command(reg, NREG, mem, MEMLEN);
	}
	printf("At termination\n");
	dump_control_unit(pc, ir, running, reg, NREG);
	printf("\n");
        dump_memory(mem, MEMLEN);
	return 0;
}


// Initialize the control registers (pc, ir, running flag) and
// the general-purpose registers
//
void initialize_control_unit(int reg[], int nreg) {
  //do you guys even read the code?	
  
  pc = 0;
  ir = 0;
  running = 1;
  int i;
  for (i = 0; i < NREG; i++){
    reg[i] = 0;
  }


	printf("\nInitial control unit:\n");
	dump_control_unit(pc, ir, running, reg, nreg);
	printf("\n");
}

// Read and dump initial values for memory
//
void initialize_memory(int argc, char *argv[], int mem[], int memlen) {
	FILE *datafile = get_datafile(argc, argv);

	// Will read the next line (words_read = 1 if it started
	// with a memory value). Will set memory location loc to
	// value_read
	//
	int value_read, words_read, loc = 0, done = 0;

	// Each getline automatically reallocates buffer and resets
	// buffer_len so that we can read in the whole line of
	// input. bytes_read is 0 at end-of-file.  Note we must
	// free the buffer once we're done with this file.
	//
	// See linux command man 3 getline for details.
	//
	char *buffer = NULL;
	size_t buffer_len = 0, bytes_read = 0;

	// Read in first and succeeding memory values. Stop when we
	// hit a sentinel value, fill up memory, or hit end-of-file.
	//
	bytes_read = getline(&buffer, &buffer_len, datafile);
	while (bytes_read != -1 && !done) {
		// If the line of input begins with an integer, treat
		// it as the memory value to read in.  Ignore junk
		// after the number and ignore blank lines and lines
		// that don't begin with a number.
		//
		words_read = sscanf(buffer, "%d", &value_read);

		if(words_read == 1){
		  if(loc > 99 || loc < 0){
		    printf("location is out of range \n");
		    break;
		      }
		  else if(value_read < -9999 || value_read > 9999){
		    printf("sentinel %d found at location %d\n", value_read, loc);
		    break;
		  }
		  else mem[loc++] = value_read;
		}

		// *** STUB *** set memory value at current location to
		// value_read and increment location.  Exceptions: If
		// loc is out of range, complain and quit the loop. If
		// value_read is outside -9999...9999, then it's a
		// sentinel and we should say so and quit the loop.
		
		// Get next line and continue the loop
		//
		bytes_read = getline(&buffer, &buffer_len, datafile);
		done = !bytes_read;
	}
	free(buffer);  // return buffer to OS
	
	// Initialize rest of memory
	//
	while (loc < memlen) {
		mem[loc++] = 0;
	}
	dump_memory(mem, memlen);
}

// Get the data file to initialize memory with.  If it was
// specified on the command line as argv[1], use that file
// otherwise use default.sdc.  If file opening fails, complain
// and terminate program execution with an error.
// See linux command man 3 exit for details.
//
FILE *get_datafile(int argc, char *argv[]) {
	char *default_datafile_name = "default.sdc";
	char *datafile_name;

	// set datafile name to argv[1] or default

	if(!argv[1])
	  datafile_name = default_datafile_name;
	else
	datafile_name = argv[1];
	datafile_nameglobe=datafile_name;
	FILE *datafile = fopen(datafile_name, "r");
	if(!datafile){
	  printf("File failed to open \n");
	    exit(EXIT_FAILURE);
	}
	// if the open failed, complain and call
	// exit(EXIT_FAILURE)
	return datafile;
}

// dump_control_unit(pc, ir, running, reg): Print out the
// control and general-purpose registers
// 
void dump_control_unit(int pc, int ir, int running, int reg[], int nreg) {
  printf("pc :    %02d   IR :   %04d  RUNNING : %d\n", pc, ir, running);
	dump_registers(reg, nreg);
}

// dump_memory(mem, memlen): Print memory values in a table,
// ten per row, with a space between each group of five columns
// and with a header column of memory locations.
//
void dump_memory(int mem[], int memlen) {
	int loc = 0;
	int row, col;
	for (row = 0; row < 100; row +=10)
	  {
	    printf("%02d:",row);
	    for(col=row; col < row +10; col++)
	      {
		if(col%10 == 5) printf(" ");
		printf("%6d",mem[col]);
	      }
	    printf("\n");
	  }
}

// dump_registers(reg, nreg): Print register values in two rows of five.
//
void dump_registers(int reg[], int nreg) {
          int i;
	  for (i = 0; i < 10; i++) {
	    if(i%5 == 0) printf("\n");
	    printf("R%d :%6d  ", i, reg[i]);
	  }

}

// Read a simulator command from the keyboard ("h", "?", "d", number,
// or empty line) and execute it.  Return true if we hit end-of-input
// or execute_command told us to quit.  Otherwise return false.
//
int read_execute_command(int reg[], int nreg, int mem[], int memlen) {
	// Buffer for the command line from the keyboard, plus its size
	//
	char *cmd_buffer = NULL;
	size_t cmd_buffer_len = 0, bytes_read = 0;

	// Values read using sscanf of command line
	//
	int nbr_cycles;
	char cmd_char;
	size_t words_read;	// number of items read by sscanf call

	int done = 0;	// Should simulator stop?

	bytes_read = getline(&cmd_buffer, &cmd_buffer_len, stdin);
	if (bytes_read == -1) {
		done = 1;   // Hit end of file
	}
	
	words_read = sscanf(cmd_buffer, "%d", &nbr_cycles);
	if (words_read == 1)
	  many_instruction_cycles(nbr_cycles, reg, NREG, mem, MEMLEN);
	  else  {
	    words_read = sscanf(cmd_buffer, "%c", &cmd_char);
	    done = execute_command(cmd_char, reg, NREG, mem, MEMLEN);
	  }
	// If we found a number, do that many
	// instruction cycles.  Otherwise sscanf for a character
	// and call execute_command with it.  (Note the character
	// might be '\n'.)

	free(cmd_buffer);
	return done;
}

// Execute a nonnumeric command; complain if it's not 'h', '?', 'd', 'q' or '\n'
// Return true for the q command, false otherwise
//
int execute_command(char cmd_char, int reg[], int nreg, int mem[], int memlen) {
	if (cmd_char == '?' || cmd_char == 'h') {
		help_message();
	}
	else if(cmd_char=='q')
	  {
	    printf("Quit \n");
	    return 1;
	  }
	else if(cmd_char =='d') 
	  {
	    dump_control_unit(pc,ir,running,reg,NREG);
	    dump_memory(mem,MEMLEN); 
	  }
	else if (cmd_char == '\n')
	  {
	    printf("CPU halted\n");
	  }
	else
	  printf("%c not a valid command \n",cmd_char);

	return 0;
}

// Print standard message for simulator help command ('h' or '?')
//
void help_message(void) {
	printf("h or ? for help (prints this message)\n");
	printf("d to dump the control unit\n");
	printf("An integer > 0 to execute that many instruction cycles\n");
	printf("Or just return, which executes one instruction cycle\n\n");
	printf("SDC Instruction set:\n");
	printf("0xxx: HALT\n");
	printf("1RMM: Load Reg[R] <- M[MM]\n");
	printf("2RMM: Store M[MM] <- Reg[R]\n");
	printf("3RMM: Add M[MM] to Reg[R]\n");
	printf("4Rxx: Negate Reg[R]\n");
	printf("5RMM: Load Immediate Reg[R] <- MM\n");
	printf("6RMM: Add Immediate Reg[R] <- Reg[R] +/- MM\n");
	printf("7xMM: Branch to MM\n");
	printf("8RMM: Branch Conditional to MM\n");
	printf("90xx: Read char into R0\n");
	printf("91xx: Print char in R0\n");
	printf("92MM: Print string starting at MM\n");
	printf("93MM: Dump control unit\n");
	printf("94MM: Dump memory\n");
}

// Execute a number of instruction cycles.  Exceptions: If the
// number of cycles is <= 0, complain and return; if the CPU is
// not running, say so and return; if the number of cycles is
// insanely large, complain and substitute a saner limit.
//
// If, as we execute the many cycles, the CPU stops running,
// then return.
//
void many_instruction_cycles(int nbr_cycles, int reg[], int nreg, int mem[], int memlen) {
	if(nbr_cycles <= 0)
	{
		printf("Number of instruction cycles > 0\n");
		return;
	}
	if(running == 0)
	{
		printf("CPU has been halted\n");
		return;
	}
	if(nbr_cycles > 100)
	{
		printf("%d is too large for cycle; doing %d \n",nbr_cycles,MEMLEN);
	}
	int count;
	for(count = 0; count < nbr_cycles; count++)
	{
		if(running == 0) break;
			
		one_instruction_cycle(reg,NREG,mem,MEMLEN);
	}
	return;
}

// Execute one instruction cycle
//
void one_instruction_cycle(int reg[], int nreg, int mem[], int memlen) {
	// If the CPU isn't running, say so and return.
	// If the pc is out of range, complain and stop running the CPU.
	//
	if(running == 0)
		{
			printf("CPU has been Halted\n");
			return;
		}
		if(pc >= MEMLEN)
		{
			printf("pc is out of range.\n");
			running = 0;
			return;
		}
	// Get instruction and increment pc
	//
	int instr_loc = pc;  // Instruction's location (pc before increment)
	ir = mem[pc++];
	
	// Decode instruction into opcode, reg_R, addr_MM, and instruction sign
	//
	int opcode,reg_R,addr_MM,instr_sign;
	int posir = abs(ir);
	int temp;
	opcode  = posir / 1000;
	reg_R   = (posir /100) % 10;
	addr_MM = posir % 100;
	if(ir > 0) instr_sign = 1;
	else	   instr_sign = -1;

	// Echo instruction
	//
	printf("At %02d instr %d %d %02d: ", instr_loc, opcode, reg_R, addr_MM);

	switch (opcode) {
	case 0: exec_HLT(reg_R, addr_MM, instr_sign, reg, nreg, mem, memlen); break;
	case 1: reg[reg_R] = mem[addr_MM];
			printf("LD   R%d <- M[%d] = %d\n",reg_R,addr_MM,mem[addr_MM]);break;
	case 2: mem[addr_MM] = reg[reg_R];
			printf("ST   M[%d] <- R%d = %d\n",addr_MM,reg_R,reg[reg_R]);break;
	case 3: temp = reg[reg_R];
			reg[reg_R] += mem[addr_MM];
			printf("ADD  R%d <- R%d + M[%d] = %d + %d = %d\n",reg_R,reg_R,addr_MM,temp,mem[addr_MM],temp + mem[addr_MM]);break;
	case 4: reg[reg_R] = -(reg[reg_R]);
			printf("NEG  R%d <- -R%d = %d\n",reg_R,reg_R,reg[reg_R]);break;
	case 5: reg[reg_R] = instr_sign * addr_MM;
			printf("LDM  R%d <- %d\n",reg_R,reg[reg_R]);break;
	case 6: temp = reg[reg_R];
			reg[reg_R] += (instr_sign * addr_MM);
			printf("ADDM R%d <- R%d + %d = %d + %d = %d\n",reg_R,reg_R,instr_sign * addr_MM,temp,instr_sign * addr_MM,temp+(instr_sign * addr_MM));break;
	case 7: pc = addr_MM;
			printf("BR   %d\n",addr_MM);break;
	case 8: if(ir > 0)
			{
				if(reg[reg_R] > 0)
				{
					pc = addr_MM;
					printf("BRC  %d if R%d = %d > 0: Yes\n",addr_MM,reg_R,reg[reg_R]);
				}
				else printf("BRC  %d if R%d = %d > 0: No\n",addr_MM,reg_R,reg[reg_R]);	
			}
			else if (ir < 0)
			{
				if(reg[reg_R] < 0)
				{
					pc = addr_MM;
					printf("BRC  %d if R%d = %d < 0: Yes\n",addr_MM,reg_R,reg[reg_R]);
				}
				else printf("BRC  %d if R%d = %d < 0: No\n",addr_MM,reg_R,reg[reg_R]);
			}
			break;
	case 9: if(reg_R == 0)
			{
				char tem;
				printf("I/O  Read char\n");
				printf("Enter a char (and/or press return):");
				scanf("%c",&tem);
				reg[0]=(int)tem;
				printf("R0 <- %d\n",reg[0]);
			}
			else if (reg_R == 1)
			{
				printf("I/O  1: Print char in R0 (= %d): %c\n",reg[0],reg[0]);
			}
			else if (reg_R == 2)
			{
				int temp = addr_MM;
				printf("I/O  2: Print string: ");
				while(mem[temp] != 0)printf("%c",mem[temp++]);
				printf("\n");
			}
			else if (reg_R == 3)
			{
				printf("I/O  3: Dump Control Unit:\n");
				dump_control_unit(pc,ir,running,reg,NREG);
			}
			else if (reg_R == 4)
			{
				printf("I/O  4: Dump Memory:\n");
				dump_memory(mem,MEMLEN);
			}
			else
			{
				printf("Unknown I/O; skipped\n");
			}
			break;
	default: printf("Bad opcode!? %d\n", opcode);
	}
}

// Execute the halt instruction (make CPU stop running)
//
void exec_HLT(int reg_R, int addr_MM, int instr_sign, int reg[], int nreg, int mem[], int memlen) {
	printf("HALT\nHalting\n");
	running = 0;
}

