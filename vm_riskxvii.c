# include "vm_riskxvii.h"

Blob blob;
unsigned int rds[32];
unsigned char *pc;
char instruction[9];
char instruction_bin[33];
const char EMPTY_BITS[33] = "00000000000000000000000000000000";
unsigned int occpuied[128][64];

int get_int(char *bin, int len) {
    int val = 0;
    for(int i = 0; i < len; i++) {
        int base;
        if (bin[i] == '1') {
            base = 1;
        }else {
            base = 0;
        }
        val += base * pow(2, len-1-i);
    }
    return val;
}

int get_signed_int(char *bin, int len) {
    int val = 0;
    for(int i = 0; i < len; i++) {
        int base;
        if (bin[i] == '1') {
            base = 1;
        }else {
            base = 0;
        }
        if (i == 0) {
            val = -(base * pow(2, len-1-i));
            continue;
        }
        val += base * pow(2, len-1-i);
    }
    return val;
}

void update_hex_instruction() {
    snprintf(instruction, 9, "%02x%02x%02x%02x", *(pc + 3), *(pc + 2), *(pc + 1), *pc);
}

void convert_hex_binary(char *instruction) {
    for(int i = 0; i < 8; i++) {
        char c = instruction[i];
        switch (c)
        {
            case '0' : strcat(instruction_bin, "0000"); break;
            case '1' : strcat(instruction_bin, "0001"); break;
            case '2' : strcat(instruction_bin, "0010"); break;
            case '3' : strcat(instruction_bin, "0011"); break;
            case '4' : strcat(instruction_bin, "0100"); break;
            case '5' : strcat(instruction_bin, "0101"); break;
            case '6' : strcat(instruction_bin, "0110"); break;
            case '7' : strcat(instruction_bin, "0111"); break;
            case '8' : strcat(instruction_bin, "1000"); break;
            case '9' : strcat(instruction_bin, "1001"); break;
            case 'a' : strcat(instruction_bin, "1010"); break;
            case 'b' : strcat(instruction_bin, "1011"); break;
            case 'c' : strcat(instruction_bin, "1100"); break;
            case 'd' : strcat(instruction_bin, "1101"); break;
            case 'e' : strcat(instruction_bin, "1110"); break;
            case 'f' : strcat(instruction_bin, "1111"); break;
        }        
    }
}

//! debug function
void print_rds() {
    printf("PC = 0x%08lx;\n",(int)(pc - blob.inst_mem) / sizeof(char));
    for(int i = 0; i < 32; i++) {
        printf("R[%d] = 0x%08x;\n", i, rds[i]);
    }
}
void print_imm(char *imm, int len) {
    printf("imm: ");
    for (int i = 0; i < len; i++) {
        printf("%c",imm[i]);
    }
    printf("\n");
    printf("int of imm is %d\n",get_signed_int(imm, len));
}
void not_implemented() {
    printf("Instruction Not Implemented: 0x%s\n", instruction);
    print_rds();
    exit(0);    
}
void illegal_operation() {
    printf("Illegal Operation: 0x%s\n", instruction);
    print_rds();
    exit(0);
}

void handle_memory_access_load(int address_int, int dest, int num_bytes, int is_signed) {
    if (address_int>=0 && address_int <= 1024) {
        if (num_bytes == 1) {
            rds[dest] = blob.inst_mem[address_int];
        } else if (num_bytes == 2) {
            rds[dest] = (blob.inst_mem[address_int+1] << 8) | blob.inst_mem[address_int];
        } else if (num_bytes == 4) {
            rds[dest] = blob.inst_mem[address_int] + (blob.inst_mem[address_int+1] << 8) + (blob.inst_mem[address_int+2] << 16) + (blob.inst_mem[address_int+3] << 24);
        }
        // rds[dest] = blob.inst_mem[address_int];
    } else if (address_int>=1024 && address_int <= 2047) {
        if (address_int+num_bytes-1 > 2047) {
            illegal_operation();
        }else {
            if (num_bytes == 1) {
                rds[dest] = blob.data_mem[address_int-1024];
            } else if (num_bytes == 2) {
                rds[dest] = (blob.data_mem[address_int-1024+1] << 8) | blob.data_mem[address_int-1024];
            } else if (num_bytes == 4) {
                rds[dest] = blob.data_mem[address_int-1024] + (blob.data_mem[address_int-1024+1] << 8) + (blob.data_mem[address_int-1024+2] << 16) + (blob.data_mem[address_int-1024+3] << 24);
            }
        }
        //accessing to load data memory
    } else if (address_int >= 2048 && address_int <= 2303) {
        //attempting to load virtual routines
        if (address_int == 2066) {
            char input_char;
            scanf("%c", &input_char);
            
        } else if (address_int == 2070) {
            int input_int;
            scanf("%d", &input_int);
            if (num_bytes == 4 && dest != 0) {
                rds[dest] = input_int;
            } else if (num_bytes == 2  && dest != 0) {
                rds[dest] = input_int & 0xffff;
            } else {
                if (dest != 0) {
                    rds[dest] = input_int & 0xff;
                }
            }
        } else {
            illegal_operation();
        }
    } else if (address_int >= 46848 && address_int <= 55040) {
        //accessing heap bank
        
    } else {
        illegal_operation();
    }
}

void handle_memory_access_write(int address_int, int val, int num_bytes) {
    if (address_int>=1024 && address_int <= 2047) {
        //accessing data memory
        if (address_int+num_bytes-1 > 2047) {
            //overwritting
            illegal_operation();
        }else {
            if (num_bytes == 1) {
                blob.data_mem[address_int-1024] = val & 0xff;
            } else if (num_bytes == 2) {
                blob.data_mem[address_int-1024] = val & 0xff;
                blob.data_mem[address_int-1024 + 1] = (val >> 8) & 0xff;
            } else if (num_bytes == 4) {
                blob.data_mem[address_int-1024] = val & 0xff;
                blob.data_mem[address_int-1024 + 1] = (val >> 8) & 0xff;
                blob.data_mem[address_int-1024 + 2] = (val >> 16) & 0xff;
                blob.data_mem[address_int-1024 + 3] = (val >> 24) & 0xff;
            }
        }
    } else if (address_int >= 2048 && address_int <= 2303) {
        //attempting write virtual routines
        if (address_int == 2048) {
            printf("%c", val);
        } else if (address_int == 2052) {
            printf("%d", val);
        } else if (address_int == 2056) {
            printf("%x", val);
        } else if (address_int == 2060) {
            printf("CPU Halt Requested\n");
            exit(0);
        } else if (address_int == 2080) {
            printf("PC = 0x%08lx\n",(int)(pc - blob.inst_mem) / sizeof(char));
        } else if (address_int == 2084) {
            print_rds();
        } else if (address_int == 2088) {
            printf("0x%08x", val);
        } else if (address_int == 2096) {
            // heap banks
            // make dynamic allocated array for 8192 bytes
            // check if memory is allocated
            if (val <= 0) {
                return;
            }
            int dest_idx = -1;
            int required_num_banks;
            if (val%64 == 0) {
                required_num_banks = val/64;
            }else {
                required_num_banks = val/64 + 1;
            }
            if (required_num_banks > 128) {
                rds[28] = 0;
                return;
            }
            int flag = 1;
            for (int i = 0; i < 128; i++) {
                if ((i + required_num_banks) > 128) {
                    break;
                }
                flag = 1;
                for(int t = i; t < i + required_num_banks; t++) {
                    for (int j = 0; j < 64; j++) {
                        if (occpuied[t][j] == 0) {
                            continue;
                        }else {
                            flag = 0;
                            break;
                        }
                    }
                    if (!flag) {
                        break;
                    }
                }
                if (flag) {
                    dest_idx = i;
                    break;
                }
            }
            if (dest_idx != -1) {
                int number_need_store = val;
                for (int i = dest_idx; i < dest_idx + required_num_banks; i++) {
                    if (number_need_store >= 64) {
                        for(int j = 0; j < 64; j++) {
                            occpuied[i][j] = 1;
                        }
                        number_need_store -= 64;
                    }else {
                        for(int j = 0; j < number_need_store; j++) {
                            occpuied[i][j] = 1;
                        }
                        number_need_store -= number_need_store;
                    }
                    if (number_need_store == 0) {
                        break;
                    }
                }
                rds[28] = 46848 + (dest_idx*64);
            }else {
                rds[28] = 0;
                return;
            }
        } else if (address_int == 2100) {
            // free
            if (val < 46848 || val  > 46848 + (128*64)) {
                return;
            }
            int num_need_move = val;
            int start_index = (val - 46848)/64;
            int start_row = (val - 46848) % 64;
            for (int i = start_index; i < 128; i++) {
                for (int j = start_row; j < 18; j++) {
                    if (occpuied[i][j] != 1) {
                        illegal_operation();
                    }
                    num_need_move--;
                    if (num_need_move == 0) {
                        break;
                    }
                }
                if (num_need_move == 0) {
                    break;
                }
            }
            for (int i = start_index; i < 128; i++) {
                for (int j = start_row; j < 18; j++) {
                    if (occpuied[i][j] == 1) {
                        // removed
                        occpuied[i][j] = -1;
                    }
                    num_need_move--;
                }
            }

        } else if (address_int >= 2128 && address_int <= 2303) {
            // customized virtual routines
        } else {
            illegal_operation();
        }



    } else if (address_int >= 46848 && address_int <= 55039) {
        // attempting to write in heap banks
        // ilegal to write
        illegal_operation();
    }else {
        // invalid address
        illegal_operation();
    }
}
void parse_R(char *opcode) {
    char func7[8] = {'\0'}; //7bits
    char func3[4] = {'\0'}; //3bits
    char rd[6] = "";
    char rs1[6] = "";
    char rs2[6] = "";

    strncpy(func7, instruction_bin, 7);
    strncpy(func3, instruction_bin + 18, 3);

    strncpy(rs2, instruction_bin + 7, 5);
    strncpy(rs1, instruction_bin + 12, 5);
    strncpy(rd, instruction_bin + 20, 5);

    if (strncmp(func3, "000", 3) == 0) {
        if (strncmp(func7, "0000000", 7) == 0) {
            // add
            if (get_int(rd, 5) != 0) {
                rds[get_int(rd, 5)] = rds[get_int(rs1, 5)] + rds[get_int(rs2, 5)];
            }
        } else if (strncmp(func7, "0100000", 7) == 0) {
            // sub
            if (get_int(rd, 5) != 0) {
                rds[get_int(rd, 5)] = rds[get_int(rs1, 5)] - rds[get_int(rs2, 5)];
            }
        } else {
            //! invalid
            not_implemented();
        }
    } else if (strncmp(func3, "100", 3) == 0 && strncmp(func7, "0000000", 7) == 0) {
        // xor
        if (get_int(rd, 5) != 0) {
            rds[get_int(rd, 5)] = rds[get_int(rs1, 5)] ^ rds[get_int(rs2, 5)];
        }
    } else if (strncmp(func3, "110", 3) == 0 && strncmp(func7, "0000000", 7) == 0) {
        // or
        if (get_int(rd, 5) != 0) {
            rds[get_int(rd, 5)] = rds[get_int(rs1, 5)] | rds[get_int(rs2, 5)];
        }
    } else if (strncmp(func3, "111", 3) == 0 && strncmp(func7, "0000000", 7) == 0) {
        // and
        if (get_int(rd, 5) != 0) {
            rds[get_int(rd, 5)] = rds[get_int(rs1, 5)] & rds[get_int(rs2, 5)];
        }
    } else if (strncmp(func3, "001", 3) == 0 && strncmp(func7, "0000000", 7) == 0) {
        // sll
        if (get_int(rd, 5) != 0) {
            rds[get_int(rd, 5)] = rds[get_int(rs1, 5)] << rds[get_int(rs2, 5)];
        }
    } else if (strncmp(func3, "101", 3) == 0) {
        if (strncmp(func7, "0000000", 7) == 0) {
            // srl
            if (get_int(rd, 5) != 0) {
                rds[get_int(rd, 5)] = rds[get_int(rs1, 5)] >> rds[get_int(rs2, 5)];
            }
        } else if (strncmp(func7, "0100000", 7) == 0) {
            // sra
            if (get_int(rd, 5) != 0) {
                rds[get_int(rd, 5)] = rds[get_int(rs1, 5)] >> rds[get_int(rs2, 5)];
                int rms_bit = rds[get_int(rd, 5)] & 0x1;
                int rmved_rds = rds[get_int(rd, 5)] >> 1;
                int num_bit = sizeof(rds[get_int(rd, 5)]) * 8;
                int amt_sft = num_bit -1;
                rds[get_int(rd, 5)] = (rmved_rds << amt_sft) | rms_bit;
            }
        } else {
            //! invalid
            not_implemented();
        }
    } else if (strncmp(func3, "010", 3) == 0 && strncmp(func7, "0000000", 7) == 0) {
        // slt
        if (get_int(rd, 5) != 0) {
            rds[get_int(rd, 5)] = (rds[get_int(rs1, 5)] < rds[get_int(rs2, 5)]) ? 1 : 0;
        }
    } else if (strncmp(func3, "011", 3) == 0 && strncmp(func7, "0000000", 7) == 0) {
        // sltu
        if (get_int(rd, 5) != 0) {
            rds[get_int(rd, 5)] = ((unsigned int)rds[get_int(rs1, 5)] < (unsigned int)rds[get_int(rs2, 5)]) ? 1 : 0;
        }
    } else {
        //! invalid
        not_implemented();
    }
    return;
}
void parse_I(char *opcode) {
    // func3
    char func3[4] = ""; //3bits
    char rd[6] = "";
    char rs1[6] = "";
    char imm_bin[12] = "";
    strncpy(func3, instruction_bin + 17, 3);
    strncpy(rd, instruction_bin+20, 5);
    strncpy(rs1, instruction_bin+12, 5);
    strncpy(imm_bin, instruction_bin, 12);
    // print_imm(imm_bin,12);
    if (strncmp(opcode, "0010011", 7) == 0) {
        if (strncmp(func3, "000", 3) == 0) {
            // addi
            if (get_int(rd, 5) != 0) {
                rds[get_int(rd, 5)] = rds[get_int(rs1, 5)] + get_signed_int(imm_bin, 12);
            }
        } else if(strncmp(func3, "100", 3) == 0) {
            // xori
            if (get_int(rd, 5) != 0) {
                rds[get_int(rd, 5)] = rds[get_int(rs1, 5)] ^ get_signed_int(imm_bin, 12);
            }
        } else if(strncmp(func3, "110", 3) == 0) {
            // ori
            if (get_int(rd, 5) != 0) {
                rds[get_int(rd, 5)] = rds[get_int(rs1, 5)] | get_signed_int(imm_bin, 12);
            }
        } else if(strncmp(func3, "111", 3) == 0) {
            // andi
            if (get_int(rd, 5) != 0) {
                rds[get_int(rd, 5)] = rds[get_int(rs1, 5)] & get_signed_int(imm_bin, 12);
            }
        } else if(strncmp(func3, "010", 3) == 0) {
            // slti
            if (get_int(rd, 5) != 0) {
                rds[get_int(rd, 5)] = (rds[get_int(rs1, 5)] < get_signed_int(imm_bin, 13)) ? 1 : 0;
            }
        } else if(strncmp(func3, "011", 3) == 0) {
            // sltiu
            if (get_int(rd, 5) != 0) {
                rds[get_int(rd, 5)] = ((unsigned int)rds[get_int(rs1, 5)] < (unsigned int)get_signed_int(imm_bin, 13)) ? 1 : 0;
            }
        } else {
            //!invalid
            not_implemented();
        }
    }else if (strncmp(opcode, "0000011", 7) == 0) {
        if (strncmp(func3, "000", 3) == 0) {
            // lb
            int address_int = rds[get_int(rs1,5)] + get_signed_int(imm_bin, 12);
            handle_memory_access_load(address_int, get_int(rd, 5), 1, 1);
        } else if(strncmp(func3, "001", 3) == 0) {
            // lh
            int address_int = rds[get_int(rs1,5)] + get_signed_int(imm_bin, 12);
            handle_memory_access_load(address_int, get_int(rd, 5), 2, 1);
        } else if(strncmp(func3, "010", 3) == 0) {
            // lw
            int address_int = rds[get_int(rs1,5)] + get_int(imm_bin, 12);
            handle_memory_access_load(address_int, get_int(rd, 5), 4, 0);
        } else if(strncmp(func3, "100", 3) == 0) {
            // lbu
            int address_int = (unsigned int)rds[get_int(rs1,5)] + (unsigned int)get_signed_int(imm_bin, 12);
            handle_memory_access_load(address_int, get_int(rd, 5), 1, 0);
        } else if(strncmp(func3, "101", 3) == 0) {
            // lhu
            int address_int = (unsigned int)rds[get_int(rs1,5)] + (unsigned int)get_signed_int(imm_bin, 12);
            handle_memory_access_load(address_int, get_int(rd, 5), 2, 0);
        } else {
            //! invalid
            not_implemented();
        }
    }else {
        // opcode is 1100111
        if (strncmp(func3, "000", 3) == 0) {
            //! todo
            // jalr
            if (get_int(rd, 5) != 0) {
                rds[get_int(rd,5)] = (int)(pc+4 - blob.inst_mem) / sizeof(char);
            }
            pc = &blob.inst_mem[rds[get_int(rs1, 5)] + get_signed_int(imm_bin,12)];
        }else {
            //! invalid
            not_implemented();
        }
    }
    return;
}
void parse_S(char *opcode) {
    // func3
    char func3[4] = ""; //3bits
    char rs1[6] = "";
    char rs2[6] = "";
    char imm_bin[12];
    for (int i = 0; i < 12; i++) {
        imm_bin[i] = '0';
    }
    strncpy(imm_bin, instruction_bin, 7);
    strncpy(imm_bin+7, instruction_bin+20, 5);
    strncpy(func3, instruction_bin + 17, 3);

    strncpy(rs2, instruction_bin+7, 5);
    strncpy(rs1, instruction_bin+12, 5);

    if (strncmp(func3, "000", 3) == 0) {
        // sb
        int address_int = rds[get_int(rs1,5)] + get_signed_int(imm_bin, 12);
        int val = rds[get_int(rs2, 5)] & 0xff;
        handle_memory_access_write(address_int, val, 1);
    } else if (strncmp(func3, "001", 3) == 0) {
        // sh
        int address_int = rds[get_int(rs1,5)] + get_signed_int(imm_bin, 12);
        int val = rds[get_int(rs2, 5)] & 0xffff;
        handle_memory_access_write(address_int, val, 2);

    } else if (strncmp(func3, "010", 3) == 0) {
        // sw
        int address_int = rds[get_int(rs1,5)] + get_signed_int(imm_bin, 12);
        int val = rds[get_int(rs2, 5)];
        handle_memory_access_write(address_int, val, 4);
    } else {
        //! invalid
        not_implemented();
    }
}
void parse_SB(char *opcode) {
    //func3
    char func3[4] = ""; //3bits
    char rs1[6] = "";
    char rs2[6] = "";
    char imm_bin[13];
    for (int i = 0; i < 13; i++) {
        imm_bin[i] = '0';
    }
    strncpy(func3, instruction_bin + 17, 3);
    strncpy(rs1, instruction_bin + 12, 5);
    strncpy(rs2, instruction_bin + 7, 5);

    strncpy(imm_bin, instruction_bin, 1);
    strncpy(imm_bin+1, instruction_bin+24, 1);
    strncpy(imm_bin+2, instruction_bin+1, 6);
    strncpy(imm_bin+8, instruction_bin+20, 4);
    

    if (strncmp(func3, "000", 3) == 0) {
        //beq
        if (rds[get_int(rs1, 5)] == rds[get_int(rs2, 5)]) {
            pc += get_signed_int(imm_bin, 13);
        }
    } else if (strncmp(func3, "001", 3) == 0) {
        //bne
        if (rds[get_int(rs1, 5)] != rds[get_int(rs2, 5)]) {
            pc += get_signed_int(imm_bin, 13);
        }
    } else if (strncmp(func3, "100", 3) == 0) {
        //blt
        if (rds[get_int(rs1, 5)] < rds[get_int(rs2, 5)]) {
            pc += get_signed_int(imm_bin, 13);
        }
    } else if (strncmp(func3, "110", 3) == 0) {
        //bltu
        //TODO
        if ((unsigned int)rds[get_int(rs1, 5)] < (unsigned int)rds[get_int(rs2, 5)]) {
            pc += (unsigned int)get_signed_int(imm_bin, 13);
        }
    } else if (strncmp(func3, "101", 3) == 0) {
        //bge
        if (rds[get_int(rs1, 5)] >= rds[get_int(rs2, 5)]) {
            pc += get_signed_int(imm_bin, 13);
        }
    } else if (strncmp(func3, "111", 3) == 0) {
        //bgeu
        if ((unsigned int)rds[get_int(rs1, 5)] >= (unsigned int) rds[get_int(rs2, 5)]) {
            pc += (unsigned int)get_signed_int(imm_bin, 13);
        }
    } else {
        //! invalid
        not_implemented();
    }
    return;
}
void parse_U(char *opcode) {
    //! todo
    //only LUI
    char imm_bin[32];
    for (int i = 0; i < 32; i++) {
        imm_bin[i] = '0';
    }
    strncpy(imm_bin, instruction_bin, 20);
    char rd[6]; 
    strncpy(rd, instruction_bin+20, 5);
    int rd_int = get_int(rd, 5);
    if (rd_int != 0) {
        rds[rd_int] = get_signed_int(imm_bin, 32);
    }
}
void parse_UJ(char *opcode) {
    //only JAL
    char imm_bin[21];
    char rd[6]; 

    for (int i = 0; i < 21; i++) {
        imm_bin[i] = '0';
    }
    strncpy(imm_bin, instruction_bin, 1);
    strncpy(imm_bin+1, instruction_bin+12, 8);
    strncpy(imm_bin+9, instruction_bin+11, 1);
    strncpy(imm_bin+10, instruction_bin+1, 10);

    strncpy(rd, instruction_bin+20, 5);
    int rd_int = get_int(rd, 5);
    if (rd_int != 0) {
        rds[rd_int] = (int)(pc+4 - blob.inst_mem) / sizeof(char);
    }
    
    pc += (int) get_signed_int(imm_bin, 21);
}
void parse_bin() {
    char opcode[8];
    strncpy(opcode, instruction_bin + 25, 7);
    if (strncmp(opcode, "0110011",sizeof(opcode)) == 0) {
        // printf("Type R\n");
        parse_R(opcode);
    }else if (strncmp(opcode, "0010011",sizeof(opcode)) == 0) {
        // printf("Type I\n");
        parse_I(opcode);
    }else if (strncmp(opcode, "0000011",sizeof(opcode)) == 0) {
        // printf("Type I\n");
        parse_I(opcode);
    }else if (strncmp(opcode, "1100111",sizeof(opcode)) == 0) {
        // printf("Type I\n");
        parse_I(opcode);
    }else if (strncmp(opcode, "0100011",sizeof(opcode)) == 0) {
        // printf("Type S\n");
        parse_S(opcode);
    }else if (strncmp(opcode, "1100011",sizeof(opcode)) == 0) {
        // printf("Type SB\n");
        parse_SB(opcode);
    }else if (strncmp(opcode, "0110111",sizeof(opcode)) == 0) {
        // printf("Type U\n");
        parse_U(opcode);
    }else if (strncmp(opcode, "1101111",sizeof(opcode)) == 0) {
        // printf("Type UJ\n");
        parse_UJ(opcode);
    }else {
        not_implemented();
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("invalid amount of argument\n");
        return -1;
    }

    FILE *fptr = fopen(argv[1], "rb");
    //? get the file size
    fseek(fptr, 0, SEEK_END);
    long f_size = ftell(fptr);
    fseek(fptr, 0, SEEK_SET);
    if ((int) f_size != 2048) {
        printf("file size is not 2048 bytes.\n");
        return -1;
    }
    //! read whole binary image
    size_t num_bytes = fread(blob.inst_mem, 4, 256, fptr);
    if ((int) num_bytes != 256) {
        printf("invalid\n");
        fclose(fptr);
        return 0;
    }   
    pc = &blob.inst_mem[0];
    
    while (pc != NULL) {
        if ((int)(pc - blob.inst_mem) / sizeof(char) >= 1024) {
            break;
        }
        update_hex_instruction();
        convert_hex_binary(instruction);
        if(strncmp(instruction_bin, EMPTY_BITS, sizeof(instruction_bin)) == 0) {
            //? if instruction binary is all 0, break
            memset(instruction_bin, 0, sizeof(instruction_bin));
            break;
        }
        unsigned char *prev_pc = pc;
        parse_bin();
        memset(instruction_bin, 0, sizeof(instruction_bin));
        if (prev_pc == pc) {
            pc += 4;
        }
    }
    fclose(fptr);
    return 0;
}