/*
    Name 1: Zane Freeman
    UTEID 1: ZCF222
*/

/***************************************************************/
/*                                                             */
/*   LC-3b Simulator                                           */
/*                                                             */
/*   EE 460N - Lab 5                                           */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***************************************************************/
/*                                                             */
/* Files:  ucode        Microprogram file                      */
/*         pagetable    page table in LC-3b machine language   */
/*         isaprogram   LC-3b machine language program file    */
/*                                                             */
/***************************************************************/

/***************************************************************/
/* These are the functions you'll have to write.               */
/***************************************************************/

void eval_micro_sequencer();
void cycle_memory();
void eval_bus_drivers();
void drive_bus();
void latch_datapath_values();

/***************************************************************/
/* A couple of useful definitions.                             */
/***************************************************************/
#define FALSE 0
#define TRUE 1

/***************************************************************/
/* Use this to avoid overflowing 16 bits on the bus.           */
/***************************************************************/
#define Low16bits(x) ((x)&0xFFFF)

/***************************************************************/
/* Definition of the control store layout.                     */
/***************************************************************/
#define CONTROL_STORE_ROWS 64
#define INITIAL_STATE_NUMBER 18

/***************************************************************/
/* Definition of bit order in control store word.              */
/***************************************************************/
enum CS_BITS
{
   IRD,
   COND1,
   COND0,
   J5,
   J4,
   J3,
   J2,
   J1,
   J0,
   LD_MAR,
   LD_MDR,
   LD_IR,
   LD_BEN,
   LD_REG,
   LD_CC,
   LD_PC,
   GATE_PC,
   GATE_MDR,
   GATE_ALU,
   GATE_MARMUX,
   GATE_SHF,
   PCMUX1,
   PCMUX0,
   DRMUX,
   SR1MUX,
   ADDR1MUX,
   ADDR2MUX1,
   ADDR2MUX0,
   MARMUX,
   ALUK1,
   ALUK0,
   MIO_EN,
   R_W,
   DATA_SIZE,
   LSHF1,
   /* MODIFY: you have to add all your new control signals */
   COND2,
   LD_USP_SAVED,
   LD_SSP_SAVED,
   SPMUX,
   LD_PSR,
   GATE_PSR,
   DRMUX1,
   SR1MUX1,
   INTEXCMUX,
   LD_VECTOR,
   //NEW FOR LAB 5
   LD_HOLDER,
   LD_PRIVELEDGE,
   CHECK_EXCEPTION,
   LD_EXCV,
   GATE_HOLDER,
   GATE_PSHPOP,
   GATE_SPMUX,
   GATE_SSP,
   GATE_USP,
   GATE_VECTOR,
   CHECK_ALIGNMENT,
   SWITCH,
   LD_PSR_SAVED,
   LD_FRAME,
   FRAMEMUX,
   DATAMUX,
   GATE_PSR_SAVED,
   GATE_PTADR,
   GATE_VA,
   SAVEPATH,
   RESTORE,
   K3,
   K2,
   K1,
   K0,

   CONTROL_STORE_BITS
} CS_BITS;

/***************************************************************/
/* Functions to get at the control bits.                       */
/***************************************************************/
int GetIRD(int *x) { return (x[IRD]); }
int GetJ(int *x) { return ((x[J5] << 5) + (x[J4] << 4) +
                           (x[J3] << 3) + (x[J2] << 2) +
                           (x[J1] << 1) + x[J0]); }
int GetLD_MAR(int *x) { return (x[LD_MAR]); }
int GetLD_MDR(int *x) { return (x[LD_MDR]); }
int GetLD_IR(int *x) { return (x[LD_IR]); }
int GetLD_BEN(int *x) { return (x[LD_BEN]); }
int GetLD_REG(int *x) { return (x[LD_REG]); }
int GetLD_CC(int *x) { return (x[LD_CC]); }
int GetLD_PC(int *x) { return (x[LD_PC]); }
int GetGATE_PC(int *x) { return (x[GATE_PC]); }
int GetGATE_MDR(int *x) { return (x[GATE_MDR]); }
int GetGATE_ALU(int *x) { return (x[GATE_ALU]); }
int GetGATE_MARMUX(int *x) { return (x[GATE_MARMUX]); }
int GetGATE_SHF(int *x) { return (x[GATE_SHF]); }
int GetPCMUX(int *x) { return ((x[PCMUX1] << 1) + x[PCMUX0]); }
int GetADDR1MUX(int *x) { return (x[ADDR1MUX]); }
int GetADDR2MUX(int *x) { return ((x[ADDR2MUX1] << 1) + x[ADDR2MUX0]); }
int GetMARMUX(int *x) { return (x[MARMUX]); }
int GetALUK(int *x) { return ((x[ALUK1] << 1) + x[ALUK0]); }
int GetMIO_EN(int *x) { return (x[MIO_EN]); }
int GetR_W(int *x) { return (x[R_W]); }
int GetDATA_SIZE(int *x) { return (x[DATA_SIZE]); }
int GetLSHF1(int *x) { return (x[LSHF1]); }
/* MODIFY: you can add more Get functions for your new control signals */
int GetCOND(int *x) { return ((x[COND2] << 2) + (x[COND1] << 1) + x[COND0]); }
int GetLD_USP(int *x) { return (x[LD_USP_SAVED]); }
int GetLD_SSP(int *x) { return (x[LD_SSP_SAVED]); }
int GetSPMUX(int *x) { return (x[SPMUX]); }
int GetLD_PSR(int *x) { return (x[LD_PSR]); }
int GetGATE_PSHPOP(int *x) { return (x[GATE_PSHPOP]); }
int GetGATEPSR(int *x) { return (x[GATE_PSR]); }
int GetDRMUX(int *x) { return ((x[DRMUX1] << 1) + x[DRMUX]); }
int GetSR1MUX(int *x) { return ((x[SR1MUX1] << 1) + x[SR1MUX]); }
int GetINTEXCMUX(int *x) { return (x[INTEXCMUX]); }
int GetLD_VECTOR(int *x) { return (x[LD_VECTOR]); }

//ADDED FOR LAB 5
int GetGATE_SSP(int *x) { return (x[GATE_SSP]); }
int GetGATE_USP(int *x) { return (x[GATE_USP]); }
int GetGATE_VECTOR(int *x) { return (x[GATE_VECTOR]); }
int GetLD_PRIVELEDGE(int *x) { return (x[LD_PRIVELEDGE]); }
//Added to help check exceptions
int GetLD_HOLDER(int *x) { return (x[LD_HOLDER]); }
int GetCHECK_EXCEPTION(int *x) { return (x[CHECK_EXCEPTION]); }
int GetLD_EXCV(int *x) { return (x[LD_EXCV]); }
int GetCHECK_ALIGNMENT(int *x) { return (x[CHECK_ALIGNMENT]); }
int GetGATE_HOLDER(int *x) { return (x[GATE_HOLDER]); }
//added to restore the state
int GetGATE_SPMUX(int *x) { return (x[GATE_SPMUX]); }
int GetSWITCH(int *x) { return (x[SWITCH]); }
int GetLD_PSRSAVED(int *x) { return (x[LD_PSR_SAVED]); }
int GetGATE_PSRSAVED(int *x) { return (x[GATE_PSR_SAVED]); }
int GetRESTORE(int *x) { return (x[RESTORE]); }
int GetK(int *x) { return ((x[K3] << 3) + (x[K2] << 2) + (x[K1] << 1) + x[K0]); }
int GetPATH(int *x) { return (x[SAVEPATH]); }
//added to help with virtual memory
int GetLD_FRAME(int *x) { return (x[LD_FRAME]); }
int GetFRAMEMUX(int *x) { return (x[FRAMEMUX]); }
int GetDATAMUX(int *x) { return (x[DATAMUX]); }
int GetGATE_PTADR(int *x) { return (x[GATE_PTADR]); }
int GetGATE_VA(int *x) { return (x[GATE_VA]); }

/***************************************************************/
/* The control store rom.                                      */
/***************************************************************/
int CONTROL_STORE[CONTROL_STORE_ROWS][CONTROL_STORE_BITS];

/***************************************************************/
/* Main memory.                                                */
/***************************************************************/
/* MEMORY[A][0] stores the least significant byte of word at word address A
   MEMORY[A][1] stores the most significant byte of word at word address A
   There are two write enable signals, one for each byte. WE0 is used for
   the least significant byte of a word. WE1 is used for the most significant
   byte of a word. */

#define WORDS_IN_MEM 0x2000 /* 32 frames */
#define MEM_CYCLES 5
int MEMORY[WORDS_IN_MEM][2];

/***************************************************************/

/***************************************************************/

/***************************************************************/
/* LC-3b State info.                                           */
/***************************************************************/
#define LC_3b_REGS 8

int RUN_BIT; /* run bit */
int BUS;     /* value of the bus */

typedef struct System_Latches_Struct
{

   int PC,  /* program counter */
       MDR, /* memory data register */
       MAR, /* memory address register */
       IR,  /* instruction register */
       N,   /* n condition bit */
       Z,   /* z condition bit */
       P,   /* p condition bit */
       BEN; /* ben register */

   int READY; /* ready bit */
              /* The ready bit is also latched as you dont want the memory system to assert it
     at a bad point in the cycle*/

   int REGS[LC_3b_REGS]; /* register file. */

   int MICROINSTRUCTION[CONTROL_STORE_BITS]; /* The microintruction */

   int STATE_NUMBER; /* Current State Number - Provided for debugging */

   /* For lab 4 */
   int INTV; /* Interrupt vector register */
   int EXCV; /* Exception vector register */
   int SSP;  /* Initial value of system stack pointer */
   /* MODIFY: you should add here any other registers you need to implement interrupts and exceptions */
   int I;
   int E;
   int USP;
   int PSR;
   int VECTOR;
   int TRAP_FLAG;

   /* For lab 5 */
   int PTBR; /* This is initialized when we load the page table */
   int VA;   /* Temporary VA register */
   /* MODIFY: you should add here any other registers you need to implement virtual memory */
   int HOLDER;
   int EXCEPTION;
   int PSR_SAVED;
   int RETURN_STATE;
   int SWTCHV;
   int PFN;

} System_Latches;

/* Data Structure for Latch */

System_Latches CURRENT_LATCHES, NEXT_LATCHES;

/* For lab 5 */
#define PAGE_NUM_BITS 9
#define PTE_PFN_MASK 0x3E00
#define PTE_VALID_MASK 0x0004
#define PAGE_OFFSET_MASK 0x1FF

/***************************************************************/
/* A cycle counter.                                            */
/***************************************************************/
int CYCLE_COUNT;

/***************************************************************/
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : Print out a list of commands.                   */
/*                                                             */
/***************************************************************/
void help()
{
   printf("----------------LC-3bSIM Help-------------------------\n");
   printf("go               -  run program to completion       \n");
   printf("run n            -  execute program for n cycles    \n");
   printf("mdump low high   -  dump memory from low to high    \n");
   printf("rdump            -  dump the register & bus values  \n");
   printf("?                -  display this help menu          \n");
   printf("quit             -  exit the program                \n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle()
{

   eval_micro_sequencer();
   cycle_memory();
   eval_bus_drivers();
   drive_bus();
   latch_datapath_values();

   CURRENT_LATCHES = NEXT_LATCHES;

   CYCLE_COUNT++;

   if (CYCLE_COUNT == 299)
   {
      NEXT_LATCHES.I = 1;
      NEXT_LATCHES.INTV = 0x01;
   }
}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate the LC-3b for n cycles.                 */
/*                                                             */
/***************************************************************/
void run(int num_cycles)
{
   int i;

   if (RUN_BIT == FALSE)
   {
      printf("Can't simulate, Simulator is halted\n\n");
      return;
   }

   printf("Simulating for %d cycles...\n\n", num_cycles);
   for (i = 0; i < num_cycles; i++)
   {
      if (CURRENT_LATCHES.PC == 0x0000)
      {
         RUN_BIT = FALSE;
         printf("Simulator halted\n\n");
         break;
      }
      cycle();
   }
}

/***************************************************************/
/*                                                             */
/* Procedure : go                                              */
/*                                                             */
/* Purpose   : Simulate the LC-3b until HALTed.                 */
/*                                                             */
/***************************************************************/
void go()
{
   if (RUN_BIT == FALSE)
   {
      printf("Can't simulate, Simulator is halted\n\n");
      return;
   }

   printf("Simulating...\n\n");
   while (CURRENT_LATCHES.PC != 0x0000)
      cycle();
   RUN_BIT = FALSE;
   printf("Simulator halted\n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : mdump                                           */
/*                                                             */
/* Purpose   : Dump a word-aligned region of memory to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void mdump(FILE *dumpsim_file, int start, int stop)
{
   int address; /* this is a byte address */

   printf("\nMemory content [0x%0.4x..0x%0.4x] :\n", start, stop);
   printf("-------------------------------------\n");
   for (address = (start >> 1); address <= (stop >> 1); address++)
      printf("  0x%0.4x (%d) : 0x%0.2x%0.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
   printf("\n");

   /* dump the memory contents into the dumpsim file */
   fprintf(dumpsim_file, "\nMemory content [0x%0.4x..0x%0.4x] :\n", start, stop);
   fprintf(dumpsim_file, "-------------------------------------\n");
   for (address = (start >> 1); address <= (stop >> 1); address++)
      fprintf(dumpsim_file, " 0x%0.4x (%d) : 0x%0.2x%0.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
   fprintf(dumpsim_file, "\n");
   fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : rdump                                           */
/*                                                             */
/* Purpose   : Dump current register and bus values to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void rdump(FILE *dumpsim_file)
{
   int k;

   printf("\nCurrent register/bus values :\n");
   printf("-------------------------------------\n");
   printf("Cycle Count  : %d\n", CYCLE_COUNT);
   printf("PC           : 0x%0.4x\n", CURRENT_LATCHES.PC);
   printf("IR           : 0x%0.4x\n", CURRENT_LATCHES.IR);
   printf("STATE_NUMBER : 0x%0.4x\n\n", CURRENT_LATCHES.STATE_NUMBER);
   printf("BUS          : 0x%0.4x\n", BUS);
   printf("MDR          : 0x%0.4x\n", CURRENT_LATCHES.MDR);
   printf("MAR          : 0x%0.4x\n", CURRENT_LATCHES.MAR);
   printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
   printf("Registers:\n");
   for (k = 0; k < LC_3b_REGS; k++)
      printf("%d: 0x%0.4x\n", k, CURRENT_LATCHES.REGS[k]);
   printf("\n");

   /* dump the state information into the dumpsim file */
   fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
   fprintf(dumpsim_file, "-------------------------------------\n");
   fprintf(dumpsim_file, "Cycle Count  : %d\n", CYCLE_COUNT);
   fprintf(dumpsim_file, "PC           : 0x%0.4x\n", CURRENT_LATCHES.PC);
   fprintf(dumpsim_file, "IR           : 0x%0.4x\n", CURRENT_LATCHES.IR);
   fprintf(dumpsim_file, "STATE_NUMBER : 0x%0.4x\n\n", CURRENT_LATCHES.STATE_NUMBER);
   fprintf(dumpsim_file, "BUS          : 0x%0.4x\n", BUS);
   fprintf(dumpsim_file, "MDR          : 0x%0.4x\n", CURRENT_LATCHES.MDR);
   fprintf(dumpsim_file, "MAR          : 0x%0.4x\n", CURRENT_LATCHES.MAR);
   fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
   fprintf(dumpsim_file, "Registers:\n");
   for (k = 0; k < LC_3b_REGS; k++)
      fprintf(dumpsim_file, "%d: 0x%0.4x\n", k, CURRENT_LATCHES.REGS[k]);
   fprintf(dumpsim_file, "\n");
   fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : get_command                                     */
/*                                                             */
/* Purpose   : Read a command from standard input.             */
/*                                                             */
/***************************************************************/
void get_command(FILE *dumpsim_file)
{
   char buffer[20];
   int start, stop, cycles;

   printf("LC-3b-SIM> ");

   scanf("%s", buffer);
   printf("\n");

   switch (buffer[0])
   {
   case 'G':
   case 'g':
      go();
      break;

   case 'M':
   case 'm':
      scanf("%i %i", &start, &stop);
      mdump(dumpsim_file, start, stop);
      break;

   case '?':
      help();
      break;
   case 'Q':
   case 'q':
      printf("Bye.\n");
      exit(0);

   case 'R':
   case 'r':
      if (buffer[1] == 'd' || buffer[1] == 'D')
         rdump(dumpsim_file);
      else
      {
         scanf("%d", &cycles);
         run(cycles);
      }
      break;

   default:
      printf("Invalid Command\n");
      break;
   }
}

/***************************************************************/
/*                                                             */
/* Procedure : init_control_store                              */
/*                                                             */
/* Purpose   : Load microprogram into control store ROM        */
/*                                                             */
/***************************************************************/
void init_control_store(char *ucode_filename)
{
   FILE *ucode;
   int i, j, index;
   char line[200];

   printf("Loading Control Store from file: %s\n", ucode_filename);

   /* Open the micro-code file. */
   if ((ucode = fopen(ucode_filename, "r")) == NULL)
   {
      printf("Error: Can't open micro-code file %s\n", ucode_filename);
      exit(-1);
   }

   /* Read a line for each row in the control store. */
   for (i = 0; i < CONTROL_STORE_ROWS; i++)
   {
      if (fscanf(ucode, "%[^\n]\n", line) == EOF)
      {
         printf("Error: Too few lines (%d) in micro-code file: %s\n",
                i, ucode_filename);
         exit(-1);
      }

      /* Put in bits one at a time. */
      index = 0;

      for (j = 0; j < CONTROL_STORE_BITS; j++)
      {
         /* Needs to find enough bits in line. */
         if (line[index] == '\0')
         {
            printf("Error: Too few control bits in micro-code file: %s\nLine: %d\n",
                   ucode_filename, i);
            exit(-1);
         }
         if (line[index] != '0' && line[index] != '1')
         {
            printf("Error: Unknown value in micro-code file: %s\nLine: %d, Bit: %d\n",
                   ucode_filename, i, j);
            exit(-1);
         }

         /* Set the bit in the Control Store. */
         CONTROL_STORE[i][j] = (line[index] == '0') ? 0 : 1;
         index++;
      }

      /* Warn about extra bits in line. */
      if (line[index] != '\0')
         printf("Warning: Extra bit(s) in control store file %s. Line: %d\n",
                ucode_filename, i);
   }
   printf("\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : init_memory                                     */
/*                                                             */
/* Purpose   : Zero out the memory array                       */
/*                                                             */
/***************************************************************/
void init_memory()
{
   int i;

   for (i = 0; i < WORDS_IN_MEM; i++)
   {
      MEMORY[i][0] = 0;
      MEMORY[i][1] = 0;
   }
}

/**************************************************************/
/*                                                            */
/* Procedure : load_program                                   */
/*                                                            */
/* Purpose   : Load program and service routines into mem.    */
/*                                                            */
/**************************************************************/
void load_program(char *program_filename, int is_virtual_base)
{
   FILE *prog;
   int ii, word, program_base, pte, virtual_pc;

   /* Open program file. */
   prog = fopen(program_filename, "r");
   if (prog == NULL)
   {
      printf("Error: Can't open program file %s\n", program_filename);
      exit(-1);
   }

   /* Read in the program. */
   if (fscanf(prog, "%x\n", &word) != EOF)
      program_base = word >> 1;
   else
   {
      printf("Error: Program file is empty\n");
      exit(-1);
   }

   if (is_virtual_base)
   {
      if (CURRENT_LATCHES.PTBR == 0)
      {
         printf("Error: Page table base not loaded %s\n", program_filename);
         exit(-1);
      }

      /* convert virtual_base to physical_base */
      virtual_pc = program_base << 1;
      pte = (MEMORY[(CURRENT_LATCHES.PTBR + (((program_base << 1) >> PAGE_NUM_BITS) << 1)) >> 1][1] << 8) |
            MEMORY[(CURRENT_LATCHES.PTBR + (((program_base << 1) >> PAGE_NUM_BITS) << 1)) >> 1][0];

      printf("virtual base of program: %04x\npte: %04x\n", program_base << 1, pte);
      if ((pte & PTE_VALID_MASK) == PTE_VALID_MASK)
      {
         program_base = (pte & PTE_PFN_MASK) | ((program_base << 1) & PAGE_OFFSET_MASK);
         printf("physical base of program: %x\n\n", program_base);
         program_base = program_base >> 1;
      }
      else
      {
         printf("attempting to load a program into an invalid (non-resident) page\n\n");
         exit(-1);
      }
   }
   else
   {
      /* is page table */
      CURRENT_LATCHES.PTBR = program_base << 1;
   }

   ii = 0;
   while (fscanf(prog, "%x\n", &word) != EOF)
   {
      /* Make sure it fits. */
      if (program_base + ii >= WORDS_IN_MEM)
      {
         printf("Error: Program file %s is too long to fit in memory. %x\n",
                program_filename, ii);
         exit(-1);
      }

      /* Write the word to memory array. */
      MEMORY[program_base + ii][0] = word & 0x00FF;
      MEMORY[program_base + ii][1] = (word >> 8) & 0x00FF;
      ;
      ii++;
   }

   if (CURRENT_LATCHES.PC == 0 && is_virtual_base)
      CURRENT_LATCHES.PC = virtual_pc;

   printf("Read %d words from program into memory.\n\n", ii);
}

/***************************************************************/
/*                                                             */
/* Procedure : initialize                                      */
/*                                                             */
/* Purpose   : Load microprogram and machine language program  */
/*             and set up initial state of the machine         */
/*                                                             */
/***************************************************************/
void initialize(char *ucode_filename, char *pagetable_filename, char *program_filename, int num_prog_files)
{
   int i;
   init_control_store(ucode_filename);

   init_memory();
   load_program(pagetable_filename, 0);
   for (i = 0; i < num_prog_files; i++)
   {
      load_program(program_filename, 1);
      while (*program_filename++ != '\0')
         ;
   }
   CURRENT_LATCHES.Z = 1;
   CURRENT_LATCHES.STATE_NUMBER = INITIAL_STATE_NUMBER;
   memcpy(CURRENT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[INITIAL_STATE_NUMBER], sizeof(int) * CONTROL_STORE_BITS);
   CURRENT_LATCHES.SSP = 0x3000; /* Initial value of system stack pointer */

   /* MODIFY: you can add more initialization code HERE */

   NEXT_LATCHES = CURRENT_LATCHES;

   RUN_BIT = TRUE;
}

/***************************************************************/
/*                                                             */
/* Procedure : main                                            */
/*                                                             */
/***************************************************************/
int main(int argc, char *argv[])
{
   FILE *dumpsim_file;

   /* Error Checking */
   if (argc < 4)
   {
      printf("Error: usage: %s <micro_code_file> <page table file> <program_file_1> <program_file_2> ...\n",
             argv[0]);
      exit(1);
   }

   printf("LC-3b Simulator\n\n");

   initialize(argv[1], argv[2], argv[3], argc - 3);

   if ((dumpsim_file = fopen("dumpsim", "w")) == NULL)
   {
      printf("Error: Can't open dumpsim file\n");
      exit(-1);
   }

   while (1)
      get_command(dumpsim_file);
}

/***************************************************************/
/* Do not modify the above code, except for the places indicated
   with a "MODIFY:" comment.
   You are allowed to use the following global variables in your
   code. These are defined above.

   CONTROL_STORE
   MEMORY
   BUS

   CURRENT_LATCHES
   NEXT_LATCHES

   You may define your own local/global variables and functions.
   You may use the functions to get at the control bits defined
   above.

   Begin your code here                          */
/***************************************************************/
int memory_access_cycle_counter = 0;
int memory_data = 0;
int adder1_result = 0;
int adder2_result = 0;
int mar_mux_bus_driver = 0;
int pc_bus_driver = 0;
int alu_bus_driver = 0;
int shift_bus_driver = 0;
int mdr_bus_driver = 0;
int holder_bus_driver = 0;
int psr_bus_driver = 0;
int psh_pop_bus_driver = 0;
int spmux_bus_driver = 0;
int ssp_bus_driver = 0;
int usp_bus_driver = 0;
int vector_bus_driver = 0;
int psr_saved_bus_driver = 0;
int ptadr_bus_driver = 0;
int va_bus_driver = 0;

int Extend16bits(int num, int signbit)
{
   int negative = num && (1 << signbit);
   switch (negative)
   {
   case 0:
      return num;
      break;
   case 1:
      num = num << (8 * sizeof(int) - 1 - signbit);
      return num >> (8 * sizeof(int) - 1 - signbit);
      break;
   }
}

int get_bits(int value, int highest_bit, int lowest_bit)
{
   int mask = Low16bits(0);
   for (int i = lowest_bit; i <= highest_bit; i++)
   {
      mask = Low16bits(mask + (0x0001 << i));
   }
   return Low16bits((value & mask) >> lowest_bit);
}

int adder_muxes()
{
   if (GetADDR1MUX(CURRENT_LATCHES.MICROINSTRUCTION))
   {
      if (GetSR1MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
         adder1_result = CURRENT_LATCHES.REGS[(CURRENT_LATCHES.IR >> 6) & 0x07];
      else
         adder1_result = CURRENT_LATCHES.REGS[(CURRENT_LATCHES.IR >> 9) & 0x07];
   }
   else
   {
      adder1_result = CURRENT_LATCHES.PC;
   }
   switch (GetADDR2MUX(CURRENT_LATCHES.MICROINSTRUCTION))
   {
   case 1:
      adder2_result = Extend16bits(CURRENT_LATCHES.IR & 0x003F, 5);
      break;
   case 2:
      adder2_result = Extend16bits(CURRENT_LATCHES.IR & 0x01FF, 8);
      break;
   case 3:
      adder2_result = Extend16bits(CURRENT_LATCHES.IR & 0x07FF, 10);
      break;
   default:
      adder2_result = 0;
      break;
   }
   if (GetLSHF1(CURRENT_LATCHES.MICROINSTRUCTION))
   {
      adder2_result = adder2_result << 1;
   }
   int result = adder1_result + adder2_result;
   return result;
}

/*
* Evaluate the address of the next state according to the
* micro sequencer logic. Latch the next microinstruction.
*/
void eval_micro_sequencer()
{
   //If the IRD bit is high, we know to load a new opcode!
   if (GetIRD(CURRENT_LATCHES.MICROINSTRUCTION))
   {
      NEXT_LATCHES.STATE_NUMBER = Low16bits(get_bits(CURRENT_LATCHES.IR, 15, 12));
      if (Low16bits(get_bits(CURRENT_LATCHES.IR, 15, 12)) == 0x0F)
      {
         NEXT_LATCHES.TRAP_FLAG = 1;
      }
      else
      {
         NEXT_LATCHES.TRAP_FLAG = 0;
      }
   }
   else if (GetRESTORE(CURRENT_LATCHES.MICROINSTRUCTION))
   {
      NEXT_LATCHES.STATE_NUMBER = CURRENT_LATCHES.RETURN_STATE;
   }
   else
   {
      //Tells us whether we go to the next state or stay in this state until a condition is met
      switch (GetCOND(CURRENT_LATCHES.MICROINSTRUCTION))
      {
      case 0:
         //unconditional
         NEXT_LATCHES.STATE_NUMBER = GetJ(CURRENT_LATCHES.MICROINSTRUCTION);
         break;
      case 1:
         //compare the ready bit
         NEXT_LATCHES.STATE_NUMBER = Low16bits(GetJ(CURRENT_LATCHES.MICROINSTRUCTION)) | Low16bits(CURRENT_LATCHES.READY << 1);
         break;
      case 2:
         // compare the branch enable bit
         NEXT_LATCHES.STATE_NUMBER = Low16bits(GetJ(CURRENT_LATCHES.MICROINSTRUCTION)) | Low16bits(CURRENT_LATCHES.BEN << 2);
         break;
      case 3:
         //compare the priority bit
         NEXT_LATCHES.STATE_NUMBER = Low16bits(GetJ(CURRENT_LATCHES.MICROINSTRUCTION)) | Low16bits(get_bits(CURRENT_LATCHES.IR, 11, 11));
         break;
      case 4:
         //compare the interrupt bit
         NEXT_LATCHES.STATE_NUMBER = Low16bits(GetJ(CURRENT_LATCHES.MICROINSTRUCTION)) | Low16bits(CURRENT_LATCHES.I << 3);
         NEXT_LATCHES.I = 0;
         break;
      case 5:
         //compare the exception bit
         if ((CURRENT_LATCHES.EXCEPTION & 0x01) || (CURRENT_LATCHES.EXCEPTION & 0x04))
         {
            NEXT_LATCHES.STATE_NUMBER = Low16bits(GetJ(CURRENT_LATCHES.MICROINSTRUCTION)) | Low16bits(CURRENT_LATCHES.E << 4);
         }
         else
         {
            NEXT_LATCHES.STATE_NUMBER = GetJ(CURRENT_LATCHES.MICROINSTRUCTION);
         }
         break;
      }
   }

   for (int i = 0; i < CONTROL_STORE_BITS; i++)
   {
      *(NEXT_LATCHES.MICROINSTRUCTION + i) = CONTROL_STORE[NEXT_LATCHES.STATE_NUMBER][i];
   }
}

/*
* This function emulates memory and the WE logic.
* Keep track of which cycle of MEMEN we are dealing with.
* If fourth, we need to latch Ready bit at the end of
* cycle to prepare microsequencer for the fifth cycle.
*/
int random_address = 0;
void cycle_memory()
{
   if (GetMIO_EN(CURRENT_LATCHES.MICROINSTRUCTION))
   {
      memory_access_cycle_counter++;
      if (memory_access_cycle_counter == 4)
      {
         NEXT_LATCHES.READY = 1;
      }
      if (GetMIO_EN(CURRENT_LATCHES.MICROINSTRUCTION))
      {
         if (CURRENT_LATCHES.READY)
         {
            if (GetR_W(CURRENT_LATCHES.MICROINSTRUCTION))
            {
               if (GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION))
               {
                  MEMORY[(CURRENT_LATCHES.MAR >> 1)][0] = CURRENT_LATCHES.MDR & 0x00FF;
                  MEMORY[(CURRENT_LATCHES.MAR >> 1)][1] = (CURRENT_LATCHES.MDR >> 8) & 0x00FF;
               }
               else
               {
                  MEMORY[(CURRENT_LATCHES.MAR >> 1)][(CURRENT_LATCHES.MAR & 0x01)] = CURRENT_LATCHES.MDR & 0x00FF;
               }
            }
            else
            {
               memory_data = (MEMORY[CURRENT_LATCHES.MAR >> 1][1] << 8) + (MEMORY[CURRENT_LATCHES.MAR >> 1][0]);
            }
            NEXT_LATCHES.READY = 0;
            memory_access_cycle_counter = 0;
         }
      }
      if (memory_access_cycle_counter >= 5)
      {
         memory_access_cycle_counter = 0;
         NEXT_LATCHES.READY = 0;
      }
   }
}

int mar_gate_value()
{
   switch (GetMARMUX(CURRENT_LATCHES.MICROINSTRUCTION))
   {
   case 0:
      return ((CURRENT_LATCHES.IR & 0x00FF) << 1);
      break;
   case 1:
      return adder_muxes();
      break;
   }
}

int pc_gate_value()
{
   return CURRENT_LATCHES.PC;
}

int alu_gate_value()
{
   int R1;
   int R2;
   if (GetSR1MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
   {
      R1 = (CURRENT_LATCHES.IR >> 6) & 0x07;
   }
   else if (GetSR1MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 2)
   {
      R1 = 6;
   }
   else
   {
      R1 = (CURRENT_LATCHES.IR >> 9) & 0x07;
   }
   if (GetALUK(CURRENT_LATCHES.MICROINSTRUCTION) == 3)
   {
      return Low16bits(CURRENT_LATCHES.REGS[R1]);
   }
   else if (CURRENT_LATCHES.IR & 0x0020)
   {
      R2 = Extend16bits(CURRENT_LATCHES.IR & 0x001F, 4);
      if (GetALUK(CURRENT_LATCHES.MICROINSTRUCTION) == 0)
      {

         return Low16bits(CURRENT_LATCHES.REGS[R1] + R2);
      }
      else if (GetALUK(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
      {

         return Low16bits(CURRENT_LATCHES.REGS[R1] & R2);
      }
      else if (GetALUK(CURRENT_LATCHES.MICROINSTRUCTION) == 2)
      {

         return Low16bits(CURRENT_LATCHES.REGS[R1] ^ R2);
      }
   }
   else
   {
      R2 = (CURRENT_LATCHES.IR & 0x07);
      if (GetALUK(CURRENT_LATCHES.MICROINSTRUCTION) == 0)
      {
         return Low16bits(CURRENT_LATCHES.REGS[R1] + CURRENT_LATCHES.REGS[R2]);
      }
      else if (GetALUK(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
      {
         return Low16bits(CURRENT_LATCHES.REGS[R1] & CURRENT_LATCHES.REGS[R2]);
      }
      else if (GetALUK(CURRENT_LATCHES.MICROINSTRUCTION) == 2)
      {
         return Low16bits(CURRENT_LATCHES.REGS[R1] ^ CURRENT_LATCHES.REGS[R2]);
      }
   }
}

int shift_gate_value()
{
   int R1;
   if (GetSR1MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
      R1 = (CURRENT_LATCHES.IR >> 6) & 0x07;
   else
      R1 = (CURRENT_LATCHES.IR >> 9) & 0x07;
   if (CURRENT_LATCHES.IR & 0x0010)
   {
      if (CURRENT_LATCHES.IR & 0x0020)
      {
         return Low16bits(Extend16bits(CURRENT_LATCHES.REGS[R1], 15) >> (CURRENT_LATCHES.IR & 0x0F));
      }
      else
         return Low16bits(CURRENT_LATCHES.REGS[R1] >> (CURRENT_LATCHES.IR & 0x0F));
   }
   else
      return Low16bits(CURRENT_LATCHES.REGS[R1] << (CURRENT_LATCHES.IR & 0x0F));
}

int mdr_gate_value()
{
   if (GetDATAMUX(CURRENT_LATCHES.MICROINSTRUCTION))
   {
      return Low16bits(CURRENT_LATCHES.MDR);
   }
   else if (GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION))
   {
      return Low16bits(CURRENT_LATCHES.MDR);
   }
   else
   {
      return Low16bits(Extend16bits(CURRENT_LATCHES.MDR & 0x00FF, 7));
   }
}

int holder_gate_value()
{
   return CURRENT_LATCHES.HOLDER;
}

int psr_gate_value()
{
   return CURRENT_LATCHES.PSR;
}

int psh_pop_gate_value()
{
   return (CURRENT_LATCHES.PC - 2);
}
int spmux_gate_value()
{
   if (GetSR1MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 2)
   {
      if (GetSPMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
      {
         return Low16bits(Extend16bits(Low16bits(CURRENT_LATCHES.REGS[6]), 15) + 2);
      }
      else
      {
         return Low16bits(Extend16bits(Low16bits(CURRENT_LATCHES.REGS[6]), 15) - 2);
      }
   }
}
int vector_gate_value()
{
   return CURRENT_LATCHES.VECTOR;
}
int ssp_gate_value()
{
   return CURRENT_LATCHES.SSP;
}
int usp_gate_value()
{
   return CURRENT_LATCHES.USP;
}
int psr_saved_gate_value()
{
   return CURRENT_LATCHES.PSR_SAVED;
}
int ptadr_gate_value()
{
   return (CURRENT_LATCHES.PTBR + ((CURRENT_LATCHES.VA & 0xFE00) >> 8));
}
int va_gate_value()
{
   return CURRENT_LATCHES.VA;
}

/*
* Datapath routine emulating operations before driving the bus.
* Evaluate the input of tristate drivers
*     Gate_MARMUX,
*     Gate_PC,
*     Gate_ALU,
*     Gate_SHF,
*     Gate_MDR,
*     Gate_TEMP,
*     Gate_PSR,
*     Gate_PCM2,
*     Gate_SPMUX,
*     Gate_SSP,
*     Gate_USP,
*     Gate_Vector,
*     Gate_SavePSR,
*     Gate_PTADR,
*     Gate_VA
*/

void eval_bus_drivers()
{
   mar_mux_bus_driver = mar_gate_value();
   pc_bus_driver = pc_gate_value();
   alu_bus_driver = alu_gate_value();
   shift_bus_driver = shift_gate_value();
   mdr_bus_driver = mdr_gate_value();
   holder_bus_driver = holder_gate_value();
   psr_bus_driver = psr_gate_value();
   psh_pop_bus_driver = psh_pop_gate_value();
   spmux_bus_driver = spmux_gate_value();
   vector_bus_driver = vector_gate_value();
   ssp_bus_driver = ssp_gate_value();
   usp_bus_driver = usp_gate_value();
   psr_saved_bus_driver = psr_saved_gate_value();
   ptadr_bus_driver = ptadr_gate_value();
   va_bus_driver = va_gate_value();
}

/*
* Datapath routine for driving the bus from one of the 5 possible
* tristate drivers.
*/
void drive_bus()
{
   if (GetGATE_MARMUX(CURRENT_LATCHES.MICROINSTRUCTION))
   {
      BUS = Low16bits(mar_mux_bus_driver);
   }
   else if (GetGATE_PC(CURRENT_LATCHES.MICROINSTRUCTION))
   {
      BUS = Low16bits(pc_bus_driver);
   }
   else if (GetGATE_ALU(CURRENT_LATCHES.MICROINSTRUCTION))
   {
      BUS = Low16bits(alu_bus_driver);
   }
   else if (GetGATE_SHF(CURRENT_LATCHES.MICROINSTRUCTION))
   {
      BUS = Low16bits(shift_bus_driver);
   }
   else if (GetGATE_MDR(CURRENT_LATCHES.MICROINSTRUCTION))
   {
      BUS = Low16bits(mdr_bus_driver);
   }
   else if (GetGATE_HOLDER(CURRENT_LATCHES.MICROINSTRUCTION))
   {
      BUS = Low16bits(holder_bus_driver);
   }
   else if (GetGATEPSR(CURRENT_LATCHES.MICROINSTRUCTION))
   {
      BUS = Low16bits(psr_bus_driver);
   }
   else if (GetGATE_PSHPOP(CURRENT_LATCHES.MICROINSTRUCTION))
   {
      BUS = Low16bits(psh_pop_bus_driver);
   }
   else if (GetGATE_SPMUX(CURRENT_LATCHES.MICROINSTRUCTION))
   {
      BUS = Low16bits(spmux_bus_driver);
   }
   else if (GetGATE_SSP(CURRENT_LATCHES.MICROINSTRUCTION))
   {
      BUS = Low16bits(ssp_bus_driver);
   }
   else if (GetGATE_USP(CURRENT_LATCHES.MICROINSTRUCTION))
   {
      BUS = Low16bits(usp_bus_driver);
   }
   else if (GetGATE_VECTOR(CURRENT_LATCHES.MICROINSTRUCTION))
   {
      BUS = Low16bits(vector_bus_driver);
   }
   else if (GetGATE_PSRSAVED(CURRENT_LATCHES.MICROINSTRUCTION))
   {
      BUS = Low16bits(psr_saved_bus_driver);
   }
   else if (GetGATE_PTADR(CURRENT_LATCHES.MICROINSTRUCTION))
   {
      BUS = Low16bits(ptadr_bus_driver);
   }
   else if (GetGATE_VA(CURRENT_LATCHES.MICROINSTRUCTION))
   {
      BUS = Low16bits(va_bus_driver);
   }
   else
   {
      BUS = Low16bits(0);
   }
}

/*
* Datapath routine for computing all functions that need to latch
* values in the data path at the end of this cycle.  Some values
* require sourcing the bus; therefore, this routine has to come
* after drive_bus.
*/
void latch_datapath_values()
{
   if (GetLD_MAR(CURRENT_LATCHES.MICROINSTRUCTION))
   {
      if (GetFRAMEMUX(CURRENT_LATCHES.MICROINSTRUCTION))
      {
         NEXT_LATCHES.MAR = ((CURRENT_LATCHES.PFN << 9) + (BUS & 0x01FF));
      }
      else
      {
         NEXT_LATCHES.MAR = Low16bits(BUS);
      }
   }

   if (GetLD_MDR(CURRENT_LATCHES.MICROINSTRUCTION))
   {
      if (GetMIO_EN(CURRENT_LATCHES.MICROINSTRUCTION))
      {
         if (CURRENT_LATCHES.READY)
         {
            if (GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION))
            {
               NEXT_LATCHES.MDR = MEMORY[(CURRENT_LATCHES.MAR >> 1)][0] & 0x00FF;
               NEXT_LATCHES.MDR |= (MEMORY[(CURRENT_LATCHES.MAR >> 1)][1] & 0x00FF) << 8;
            }
            else
            {
               NEXT_LATCHES.MDR = Extend16bits(MEMORY[(CURRENT_LATCHES.MAR >> 1)][(CURRENT_LATCHES.MAR & 0x01)] & 0x00FF, 7);
            }
            NEXT_LATCHES.READY = 0;
            memory_access_cycle_counter = 0;
         }
      }
      else
      {
         if (GetDATAMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
         {
            if (CURRENT_LATCHES.SWTCHV == 1)
            {
               NEXT_LATCHES.MDR = (Low16bits(BUS) | 0x03);
               NEXT_LATCHES.SWTCHV = 0;
            }
            else
            {
               NEXT_LATCHES.MDR = (Low16bits(BUS) | 0x01);
            }
         }
         else if (GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION))
         {
            NEXT_LATCHES.MDR = Low16bits(BUS);
         }
         else
         {
            NEXT_LATCHES.MDR = BUS & 0x00FF;
         }
      }
   }

   if (GetLD_IR(CURRENT_LATCHES.MICROINSTRUCTION))
   {
      NEXT_LATCHES.IR = Low16bits(BUS);
   }

   if (GetLD_REG(CURRENT_LATCHES.MICROINSTRUCTION))
   {
      if (GetDRMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 0)
         NEXT_LATCHES.REGS[(CURRENT_LATCHES.IR >> 9) & 0x07] = Low16bits(BUS);
      else if (GetDRMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 1)
         NEXT_LATCHES.REGS[7] = Low16bits(BUS);
      else
         NEXT_LATCHES.REGS[6] = Low16bits(BUS);
   }

   if (GetLD_CC(CURRENT_LATCHES.MICROINSTRUCTION))
   {
      int bus_value = Extend16bits(Low16bits(BUS), 15);
      if (bus_value < 0)
      {
         NEXT_LATCHES.N = 1;
         NEXT_LATCHES.Z = 0;
         NEXT_LATCHES.P = 0;
         NEXT_LATCHES.PSR = NEXT_LATCHES.PSR & 0x8000;
         NEXT_LATCHES.PSR = NEXT_LATCHES.PSR | 0x04;
      }
      else if (bus_value > 0)
      {
         NEXT_LATCHES.N = 0;
         NEXT_LATCHES.Z = 0;
         NEXT_LATCHES.P = 1;
         NEXT_LATCHES.PSR = NEXT_LATCHES.PSR & 0x8000;
         NEXT_LATCHES.PSR = NEXT_LATCHES.PSR | 0x01;
      }
      else
      {
         NEXT_LATCHES.N = 0;
         NEXT_LATCHES.Z = 1;
         NEXT_LATCHES.P = 0;
         NEXT_LATCHES.PSR = NEXT_LATCHES.PSR & 0x8000;
         NEXT_LATCHES.PSR = NEXT_LATCHES.PSR | 0x02;
      }
   }

   if (GetLD_BEN(CURRENT_LATCHES.MICROINSTRUCTION))
   {
      NEXT_LATCHES.BEN = (CURRENT_LATCHES.N & get_bits(CURRENT_LATCHES.IR, 11, 11)) || (CURRENT_LATCHES.Z & get_bits(CURRENT_LATCHES.IR, 10, 10)) || (CURRENT_LATCHES.P & get_bits(CURRENT_LATCHES.IR, 9, 9));
   }

   if (GetLD_PC(CURRENT_LATCHES.MICROINSTRUCTION))
   {
      switch (GetPCMUX(CURRENT_LATCHES.MICROINSTRUCTION))
      {
      case 0:
         NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2;
         break;
      case 1:
         NEXT_LATCHES.PC = Low16bits(BUS);
         break;
      case 2:
         NEXT_LATCHES.PC = adder_muxes();
         break;
      }
   }
   if (GetLD_VECTOR(CURRENT_LATCHES.MICROINSTRUCTION))
   {
      if (GetINTEXCMUX(CURRENT_LATCHES.MICROINSTRUCTION))
      {
         NEXT_LATCHES.VECTOR = (CURRENT_LATCHES.EXCV << 1) | 0x0200;
      }
      else
      {
         NEXT_LATCHES.VECTOR = (CURRENT_LATCHES.INTV << 1) | 0x0200;
      }
   }
   if (GetLD_PRIVELEDGE(CURRENT_LATCHES.MICROINSTRUCTION))
   {
      NEXT_LATCHES.PSR = NEXT_LATCHES.PSR & 0x7F;
   }

   if (GetSWITCH(CURRENT_LATCHES.MICROINSTRUCTION))
   {
      NEXT_LATCHES.SWTCHV = 1;
   }

   if (GetLD_HOLDER(CURRENT_LATCHES.MICROINSTRUCTION))
   {
      NEXT_LATCHES.HOLDER = Low16bits(BUS);
   }

   if (GetLD_PSR(CURRENT_LATCHES.MICROINSTRUCTION))
   {
      NEXT_LATCHES.PSR = Low16bits(BUS);
      NEXT_LATCHES.N = BUS & 0x04;
      NEXT_LATCHES.Z = BUS & 0x02;
      NEXT_LATCHES.P = BUS & 0x01;
   }

   if (GetLD_USP(CURRENT_LATCHES.MICROINSTRUCTION))
   {
      if (GetSR1MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 2)
      {
         NEXT_LATCHES.USP = Low16bits(CURRENT_LATCHES.REGS[6]);
      }
   }

   if (GetLD_SSP(CURRENT_LATCHES.MICROINSTRUCTION))
   {
      if (GetSR1MUX(CURRENT_LATCHES.MICROINSTRUCTION) == 2)
      {
         NEXT_LATCHES.SSP = Low16bits(CURRENT_LATCHES.REGS[6]);
      }
   }

   if (GetCHECK_EXCEPTION(CURRENT_LATCHES.MICROINSTRUCTION))
   {
      if ((CURRENT_LATCHES.PSR & 0x8000) && (!(NEXT_LATCHES.MDR & 0x08)) && (!CURRENT_LATCHES.TRAP_FLAG))
      {
         NEXT_LATCHES.EXCEPTION = 0x03;
         NEXT_LATCHES.E = 0x01;
      }
      else if (!(NEXT_LATCHES.MDR & 0x04))
      {
         NEXT_LATCHES.EXCEPTION = 0x01;
         NEXT_LATCHES.E = 0x01;
      }
      else
      {
         NEXT_LATCHES.EXCEPTION = 0x00;
         NEXT_LATCHES.E = 0x00;
      }
   }
   else
   {
      NEXT_LATCHES.EXCEPTION = 0;
      NEXT_LATCHES.E = 0x00;
   }
   if (GetLD_EXCV(CURRENT_LATCHES.MICROINSTRUCTION))
   {
      if (GetIRD(CURRENT_LATCHES.MICROINSTRUCTION))
      {
         if (0x07 & CURRENT_LATCHES.EXCEPTION)
         {
            NEXT_LATCHES.E = 0x01;
            NEXT_LATCHES.EXCV = 0x03;
         }
         else if (0x02 & CURRENT_LATCHES.EXCEPTION)
         {
            NEXT_LATCHES.E = 0x01;
            NEXT_LATCHES.EXCV = 0x04;
         }
         else if (CURRENT_LATCHES.EXCEPTION)
         {
            NEXT_LATCHES.E = 0x01;
            NEXT_LATCHES.EXCV = 0x02;
         }
         else
         {
            NEXT_LATCHES.E = 0x01;
            NEXT_LATCHES.EXCV = 0x05;
         }
      }
      else
      {
         NEXT_LATCHES.E = 0x01;
         NEXT_LATCHES.EXCV = 0x05;
      }
   }

   if (GetSWITCH(CURRENT_LATCHES.MICROINSTRUCTION))
   {
      NEXT_LATCHES.VA = Low16bits(BUS);
      if (GetCHECK_ALIGNMENT(CURRENT_LATCHES.MICROINSTRUCTION))
      {
         if ((BUS & 0x01))
         {
            NEXT_LATCHES.E = 0x01;
            NEXT_LATCHES.EXCEPTION = 4;
         }
      }
      switch (GetK(CURRENT_LATCHES.MICROINSTRUCTION))
      {
      case 0:
         NEXT_LATCHES.RETURN_STATE = 23;
         break;
      case 1:
         NEXT_LATCHES.RETURN_STATE = 24;
         break;
      case 2:
         NEXT_LATCHES.RETURN_STATE = 25;
         break;
      case 3:
         NEXT_LATCHES.RETURN_STATE = 28;
         break;
      case 4:
         NEXT_LATCHES.RETURN_STATE = 29;
         break;
      case 5:
         NEXT_LATCHES.RETURN_STATE = 33;
         break;
      case 6:
         NEXT_LATCHES.RETURN_STATE = 48;
         break;
      case 7:
         NEXT_LATCHES.RETURN_STATE = 49;
         break;
      case 8:
         NEXT_LATCHES.RETURN_STATE = 59;
         break;
      case 9:
         NEXT_LATCHES.RETURN_STATE = 45;
         break;
      case 10:
         NEXT_LATCHES.RETURN_STATE = 57;
         break;
      }
   }

   if (GetLD_PSR(CURRENT_LATCHES.MICROINSTRUCTION))
   {
      NEXT_LATCHES.PSR_SAVED = Low16bits(BUS);
   }

   if (GetLD_FRAME(CURRENT_LATCHES.MICROINSTRUCTION))
   {
      NEXT_LATCHES.PFN = (Low16bits(BUS) >> 9) & 0x1F;
   }
}
