#ifndef EMULATOR_H
#define EMULATOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* type definitions */
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;

/* Structures for emulators */

/* Structure for the cc flags */
typedef struct ConditionCodes
{
	uint8_t z:1;
	uint8_t s:1;
	uint8_t p:1;
	uint8_t cy:1;
	uint8_t ac:1;
	uint8_t pad:3;
} ConditionCodes;


/* Current state of the program */
typedef struct State8080
{
	/* the 7 registers */
	uint8_t a;
	uint8_t b;
	uint8_t c;
	uint8_t d;
	uint8_t e;
	uint8_t h;
	uint8_t l;
	/* Stack pointer */
	uint16_t sp;
	/* Program Counter */
	uint16_t pc;

	uint8_t * memory;
	struct ConditionCodes cc;
	uint8_t int_enable;
} State8080;

/* function declarations */
void emulate8080(State8080* state, int num_cycles);
uint8_t parity(uint16_t number);
void generate_interrupt(State8080* state, int interrupt_num);

/* Function pointers for port OUT and IN */
void (*port8080_out)(uint8_t port, State8080* state);
uint8_t (*port8080_in)(uint8_t port, State8080* state);

/* Macros */
#define MVI(x) x = opcode[1]; state->pc++; break;	

/*** CALL ***/
#define CALL() 		\
do{								\
	state->memory[state->sp-1] = ((state->pc+3) >> 8) & 0xFF ;		\
	state->memory[state->sp-2] = (state->pc+3) & 0xFF;		\
	state->sp -= 2;						\
	state->pc = (opcode[2] << 8) | opcode[1];		\
	state->pc--;						\
}while(0)

/*** RET ***/
#define RET()			\
do{				\
	state->pc = state->memory[state->sp] | (state->memory[state->sp+1] << 8);	\
	state->sp += 2;						\
	state->pc--;						\
}while(0);

/***  JMP ***/
#define JMP() \
do{		\
	state->pc = (opcode[2] << 8) | opcode[1];	\
     	state->pc--;				\
}while(0);

#define MOV(a, b) a = b; break;

#define MOV_TO_M(x) adr = (state->h << 8) | state->l; x = state->memory[adr]; break;

#define MOV_FROM_M(x)  adr = (state->h << 8) | state->l;state->memory[adr] = x; break;

#define DCR(x)			\
do{ 					\
	x--;				\
	/* set flags */			\
	set_zsp(x);\
}while(0)


#define INR(x)			\
do {				\
	x++;			\
	/* set flags */		\
	set_zsp(x);\
} while(0)


#define XRA(x)		\
do{ 			\
	state->a ^= x;	\
	set_zsp(state->a);\
	state->cc.cy = 0;	\
}while(0)

#define ANA(x)		\
do{ 			\
	state->a &= x;	\
	set_zsp(state->a);\
	state->cc.cy = 0;	\
}while(0)


#define ORA(x)	\
do{		\
	state->a |= x;\
	set_zsp(state->a);\
	state->cc.cy = 0;	\
}while(0)

#define ADD(x)	\
do{		\
	answer = (uint16_t)state->a+ (uint16_t)x;\
	set_zsp(answer);\
	state->cc.cy = (answer > 0xFF);	\
	state->a = answer;		\
}while(0)

#define ADC(x)	\
do{		\
	answer = (uint16_t)state->a+ (uint16_t)x + state->cc.cy;\
	set_zsp(answer);\
	state->cc.cy = (answer > 0xFF);	\
	state->a = answer;		\
}while(0)


#define SUB(x)	\
do{		\
	answer = (uint16_t)state->a- (uint16_t)x;\
	set_zsp(answer);\
	state->cc.cy = !(answer > 0xFF);	\
	state->a = answer;		\
}while(0)


#define SBB(x)	\
do{		\
	answer = (uint16_t)state->a- (uint16_t)x - state->cc.cy;\
	set_zsp(answer);\
	state->cc.cy = !(answer > 0xFF);	\
	state->a = answer;		\
}while(0)

#define CMP(x)	\
do{		\
	answer = (uint16_t)state->a- (uint16_t)x;\
	set_zsp(answer);\
	state->cc.cy = !(answer > 0xFF);	\
}while(0)

#define set_zsp(x)	\
do{			\
	state->cc.z = ((x & 0xFF) == 0);	\
	state->cc.s = ((x & 0x80) != 0);	\
	state->cc.p = parity(x);		\
}while(0)



#endif
