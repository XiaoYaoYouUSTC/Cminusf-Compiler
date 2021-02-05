#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<lexical_analyzer.h>


/// \brief process all *.cminus file
///
/// note that: use relative path for all i/o operations
int main(int argc, char **argv){
    if (argc != 3){
        printf("usage: lexer input_file output_file\n");
        return 0;
    }
    char* input_file = argv[1];
    char* output_file = argv[2];
    

    //token_stream 至多存储 MAX_NUM_TOKEN_NODE 个Token_Node
    Token_Node token_stream[MAX_NUM_TOKEN_NODE];
    memset(token_stream, 0, sizeof(Token_Node) * MAX_NUM_TOKEN_NODE);

    analyzer(input_file, token_stream);
    
    //wrtie file
    FILE* fp;
    if(!(fp = fopen(output_file,"w+"))){
        printf("[ERR] Cannot create output file\n");
        exit(1);
    }
    int index = 0;
    while(token_stream[index].token != 0){
        fprintf(fp, "%s\t%d\t%d\t%d\t%d\n",token_stream[index].text, token_stream[index].token,
                token_stream[index].lines, token_stream[index].pos_start, token_stream[index].pos_end);
        index++;
    }


    return 0;
}