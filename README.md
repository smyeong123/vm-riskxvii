# VM-RISKXVII
![C](https://img.shields.io/badge/c-%2300599C.svg?style=flat-square&logo=c&logoColor=white)
![Linux](https://img.shields.io/badge/Linux-FCC624?style=flat-square&logo=linux&logoColor=black)
<a href="https://github.com/fan2goa1/best-of-usyd" title="best of USYD"><img alt="best of USYD" src="https://img.shields.io/endpoint?url=https://raw.githubusercontent.com/fan2goa1/best-of-usyd/main/assets/badge/v1.json"></a>

## Introduction

Vm-riskxvii is a simple virtual machine that implements the RISK-XVII instruction set architecture. It is designed to execute 32-bit instructions, manage memory using heap banks, and handle virtual routines for input and output operations.

This project is based on the specifications provided in the COMP2017/COMP9017 Assignment 2 at the University of Sydney. The virtual machine executes binary programs compiled for the RISK-XVII architecture and supports memory management, branching, and arithmetic operations.

## Features
- 32-bit RISK-XVII Instruction Set: Supports arithmetic, logical, memory access, and control flow operations.
- Memory Management: Implements instruction memory, data memory, virtual routines, and dynamically allocated heap banks.
- Virtual Routines: Supports standard I/O operations such as reading/writing characters and integers.
- Error Handling: Implements error detection for unknown instructions, illegal operations, and memory violations.
- Instruction Decoding: Decodes and executes machine code from a binary memory image.

## Getting Started
### Prerequisites
**Ensure** that you have the following dependencies installed:

- C Compiler (GCC/Clang)
- Make
- Standard C Libraries
- Unix-based OS (Linux/macOS recommended)

### Installation
```bash
git clone https://github.com/smyeong123/vm-riskxvii.git
cd vm-riskxvii
make
```
This command will clone the repository to the local environment and generate an executable binary file named `vm_riskxvii` after changing the current directory to `vm-riskxvii/`. 

## Usage
To run the virtual machine, provide a RISK-XVII binary memory image as input:
```bash
./vm_riskxvii <memory_image_binary>
```

For example:
```bash
./vm_riskxvii test_program.mi
```

This will load and execute the RISK-XVII binary program.

## Memory Architecture
The virtual machine's memory is divided into several regions:

| Address Range | Description |
|--------------|-------------|
| `0x0000 - 0x03FF` | Instruction Memory (1024 bytes) |
| `0x0400 - 0x07FF` | Data Memory (1024 bytes) |
| `0x0800 - 0x08FF` | Virtual Routines |
| `0xB700+` | Heap Banks (128 banks × 64 bytes each) |

Each **register** is **32-bit wide**, and the virtual machine has **32 general-purpose registers** plus a **program counter (PC)**.

---

## Supported Instructions

The **RISK-XVII** virtual machine supports the following instruction types:

### **Arithmetic & Logic Operations**
| Instruction | Operation |
|-------------|-----------|
| `add` | `R[rd] = R[rs1] + R[rs2]` |
| `sub` | `R[rd] = R[rs1] - R[rs2]` |
| `addi` | `R[rd] = R[rs1] + imm` |
| `and` | `R[rd] = R[rs1] & R[rs2]` |
| `or` | `R[rd] = R[rs1] | R[rs2]` |
| `xor` | `R[rd] = R[rs1] ^ R[rs2]` |
| `sll` | `R[rd] = R[rs1] << R[rs2]` |
| `srl` | `R[rd] = R[rs1] >> R[rs2]` |

### **Memory Access Operations**
| Instruction | Operation |
|-------------|-----------|
| `lw` | `R[rd] = M[R[rs1] + imm]` |
| `sw` | `M[R[rs1] + imm] = R[rs2]` |
| `lb` | Load byte (sign-extended) |
| `lbu` | Load byte (unsigned) |
| `lh` | Load halfword (sign-extended) |
| `lhu` | Load halfword (unsigned) |
| `sh` | Store halfword |

### **Program Flow Operations**
| Instruction | Operation |
|-------------|-----------|
| `beq` | Branch if equal |
| `bne` | Branch if not equal |
| `blt` | Branch if less than |
| `bge` | Branch if greater or equal |
| `jal` | Jump and link |
| `jalr` | Jump and link register |

---

## Virtual Routines

### **I/O Operations**
| Address | Operation |
|---------|-----------|
| `0x0800` | Console Write Character |
| `0x0804` | Console Write Signed Integer |
| `0x0808` | Console Write Unsigned Integer |
| `0x080C` | Halt (terminate execution) |
| `0x0812` | Console Read Character |
| `0x0816` | Console Read Signed Integer |

### **Debugging and Memory Management**
| Address | Operation |
|---------|-----------|
| `0x0820` | Dump PC |
| `0x0824` | Dump Register Banks |
| `0x0828` | Dump Memory Word |
| `0x0830` | `malloc` (allocate heap memory) |
| `0x0834` | `free` (deallocate heap memory) |

---

## Example Programs

### **Printing a Character**
The following RISK-XVII assembly program prints the letter `'H'`:
```assembly
lui a5, 0x1
addi a4, x0, 72
sb a4, -2048(a5)
```

### **Adding Two Numbers**
```assembly
lui a5, 0x1
addi a4, x0, 5
addi a3, x0, 10
add a2, a4, a3
sw a2, -2044(a5)
```
This program reads two numbers, adds them, and prints the result.

## License
This project is licensed under the USYD.
