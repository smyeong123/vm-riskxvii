#ifndef VM_RISKXVII_H
#define VM_RISKXVII_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>
#include<math.h>

#define INST_MEM_SIZE 1024
#define DATA_MEM_SIZE 1024

// build command: gcc -g -lm -std=c11 -Wall -Werror -fsanitize=address,leak vm_riskxvii.c -o vm_riskxvii.o

typedef struct Blob {
    unsigned char inst_mem[INST_MEM_SIZE];
    unsigned char data_mem[DATA_MEM_SIZE];
}Blob;

extern Blob blob;
extern unsigned int rds[32];
extern unsigned char *pc;
extern char instruction[9];
extern char instruction_bin[33];
extern const char EMPTY_BITS[33];
extern unsigned int occpuied[128][64];

int get_int(char *bin, int len);
int get_signed_int(char *bin, int len);
void update_hex_instruction();
void convert_hex_binary(char *instruction);
void print_rds();
void print_imm(char *imm, int len);
void not_implemented();
void illegal_operation();
void handle_memory_access_load(int address_int, int dest, int num_bytes, int is_signed);
void handle_memory_access_write(int address_int, int val, int num_bytes);
void parse_R(char *opcode);
void parse_I(char *opcode);
void parse_S(char *opcode);
void parse_SB(char *opcode);
void parse_U(char *opcode);
void parse_UJ(char *opcode);
void parse_bin();

#endif // VM_RISKXVII_H