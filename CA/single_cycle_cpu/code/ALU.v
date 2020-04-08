`define AND 3'b000
`define OR  3'b001
`define ADD 3'b010
`define MUL 3'b011
`define SUB 3'b110


module ALU(data1_i, data2_i, ALUCtrl_i, data_o, Zero_o);

    input [31:0] data1_i,data2_i;
    input [2:0] ALUCtrl_i;
    output [31:0] data_o;
    output Zero_o;

    reg [31:0] tmp_out;
    reg tmp_zero;

    assign data_o = tmp_out;
    assign Zero_o= tmp_zero;

    always @ (data1_i, data2_i, ALUCtrl_i)
    begin 
        case(ALUCtrl_i)
            `AND:   tmp_out = data1_i & data2_i;
            `OR:    tmp_out = data1_i | data2_i;
            `ADD:   tmp_out = data1_i + data2_i;
            `SUB:   tmp_out = data1_i - data2_i;
            `MUL:   tmp_out = data1_i * data2_i;
        endcase
    
    tmp_zero <= 1'b0;
    if(tmp_out == 32'b0)
        tmp_zero <= 1'b1;
    end
endmodule
