module Control(Op_i, ALUOp_o, ALUSrc_o, RegWrite_o);
    
    input   [6:0] Op_i;
    output  [1:0] ALUOp_o;
    output  ALUSrc_o, RegWrite_o;    
    
    reg [1:0] tmp_ALUOp;
    reg tmp_ALUSrc, tmp_RegWrite;

    assign ALUOp_o = tmp_ALUOp;
    assign ALUSrc_o = tmp_ALUSrc;

    assign RegWrite_o = 1'b1;

    always @ (Op_i)
    begin 

        if(Op_i == 7'b0010011)
            begin 
                tmp_ALUOp = 2'b00;
                tmp_ALUSrc = 1'b1;
            end
        else
            begin
                tmp_ALUOp = 2'b10;
                tmp_ALUSrc = 1'b0;
            end
    end

    endmodule
