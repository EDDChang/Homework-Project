module MUX32(data1_i, data2_i, select_i, data_o);

    input [31:0] data1_i,data2_i;
    input select_i;
    output [31:0] data_o;

    reg [31:0] tmp;    
    assign data_o = tmp;

    always @ (data1_i or data2_i or select_i)
    begin
        if(select_i == 1'b0)
            tmp = data1_i;
        else
            tmp = data2_i;
    end
endmodule
