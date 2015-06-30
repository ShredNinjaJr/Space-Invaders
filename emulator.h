#ifndef EMULATOR_H
#define EMULATOR_H


/* type definitions */
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;

/* function declarations */
int disassemble8080(unsigned char * code_buffer, int pc);
uint8_t parity(uint16_t number);
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

#define MOV_FROM_M(x) adr = (state->h << 8) | state->l; x = state->memory[adr]; break;

#define MOV_TO_M(x)  adr = (state->h << 8) | state->l;state->memory[adr] = x; break;

#define DCR(x)			\
do{ 					\
	x--;				\
	/* set flags */			\
	state->cc.z = (( x & 0xFF) == 0); \
	state->cc.s = ((x & 0x80) != 0);	\
	state->cc.p = parity(x);	\
}while(0)


#define INR(x)			\
do {				\
	x++;			\
	/* set flags */		\
	state->cc.z = (( x & 0xFF) == 0); \
	state->cc.s = ((x & 0x80) != 0);	\
	state->cc.p = parity(x);	\
} while(0)


#define XRA(x)		\
do{ 			\
	state->a ^= x;	\
	state->cc.z = ((x & 0xFF) == 0);	\
	state->cc.s = (x & 0x80) != 0;\
	state->cc.p = parity(x);	\
	state->cc.cy = 0;	\
}while(0)

#define ANA(x)		\
do{ 			\
	state->a &= x;	\
	state->cc.z = ((x & 0xFF) == 0);	\
	state->cc.s = (x & 0x80) != 0;\
	state->cc.p = parity(x);	\
	state->cc.cy = 0;	\
}while(0)

#endif
