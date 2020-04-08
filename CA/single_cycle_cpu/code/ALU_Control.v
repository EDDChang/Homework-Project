`define AND 3'b000
`define OR  3'b001
`define ADD 3'b010
`define MUL 3'b011
`define SUB 3'b110

module ALU_Control(funct_i, ALUOp_i, ALUCtrl_o);

    input [9:0] funct_i;
    input [1:0] ALUOp_i;
    output [2:0] ALUCtrl_o;

    reg [2:0]   tmp_ALUcontrol;

    assign ALUCtrl_o = tmp_ALUcontrol;

    always @ (funct_i, ALUOp_i)
    begin

        if(ALUOp_i == 2'b00)
            tmp_ALUcontrol = `ADD;
        else
            begin
                case(funct_i)
                    10'b0000000110: tmp_ALUcontrol = `OR;
                    10'b0000000111: tmp_ALUcontrol = `AND;
                    10'b0000000000: tmp_ALUcontrol = `ADD;
                    10'b0100000000: tmp_ALUcontrol = `SUB;
                    10'b0000001000: tmp_ALUcontrol = `MUL;
                endcase
             end
    end
endmodule
