`include "Adder.v"
`include "PC.v"
`include "Instruction_Memory.v"
`include "Sign_Extend.v"
`include "Control.v"
`include "ALU_Control.v"
`include "ALU.v"
`include "MUX32.v"
`include "Registers.v"

module CPU(clk_i, rst_i,start_i);

// Ports
input               clk_i;
input               rst_i;
input               start_i;

wire [31:0] PC_in,PC_out;
wire [31:0] instruction;
wire [31:0] read_Reg1, read_Reg2;
wire [31:0] ExtendedSign_imm;
wire [31:0] ALU_data;

wire [2:0] ALUctrl_ALU;
wire zero_if;
wire [31:0] ALU_out;

wire Control_Regwrite;
wire [1:0] ALUop;
wire ALUsrc;

Control Control(
    .Op_i       (instruction[6:0]),
    .ALUOp_o    (ALUop),
    .ALUSrc_o   (ALUsrc),
    .RegWrite_o (Control_Regwrite)
);



Adder Add_PC(
    .data1_in   (PC_out),
    .data2_in   (32'd4),
    .data_o     (PC_in)
);


PC PC(
    .clk_i      (clk_i),
    .rst_i      (rst_i),
    .start_i    (start_i),
    .pc_i       (PC_in),
    .pc_o       (PC_out)
);

Instruction_Memory Instruction_Memory(
    .addr_i     (PC_out), 
    .instr_o    (instruction)
);

Registers Registers(
    .clk_i      (clk_i),
    .RS1addr_i   (instruction[19:15]),
    .RS2addr_i   (instruction[24:20]),
    .RDaddr_i   (instruction[11:7]), 
    .RDdata_i   (ALU_out),
    .RegWrite_i (Control_Regwrite), 
    .RS1data_o   (read_Reg1), 
    .RS2data_o   (read_Reg2) 
);


MUX32 MUX_ALUSrc(
    .data1_i    (read_Reg2),
    .data2_i    (ExtendedSign_imm),
    .select_i   (ALUsrc),
    .data_o     (ALU_data)
);



Sign_Extend Sign_Extend(
    .data_i     (instruction[31:20]),
    .data_o     (ExtendedSign_imm)
);

  

ALU ALU(
    .data1_i    (read_Reg1),
    .data2_i    (ALU_data),
    .ALUCtrl_i  (ALUctrl_ALU),
    .data_o     (ALU_out),
    .Zero_o     (zero_to_if)
);



ALU_Control ALU_Control(
    .funct_i    ({instruction[31:25], instruction[14:12]}),
    .ALUOp_i    (ALUop),
    .ALUCtrl_o  (ALUctrl_ALU)
);


endmodule

