#include "nailtool.h"
#include <cstdio>
#include <fstream>
#define error(...) {fprintf(stderr,__VA_ARGS__); exit(-1);}
FILE *infile(int argc,char **argv){
        char commandbuffer[1024];
        FILE *infile;
        if(argc<2){
                fprintf(stderr,"Usage %s <grammar file> <options>\n", argv[0]);
                exit(-2);
        }
        snprintf(commandbuffer,sizeof commandbuffer,"cpp -I/usr/include/nail/generator < \"%s\" |sed '/^\\#/d'",argv[1]);
        infile = popen(commandbuffer,"r");
        if(!infile) error ("Cannot open pipe\n");
        return infile;
}
int main(int argc, char**argv)
{
    uint8_t input[102400];
    size_t inputsize;
    size_t outputsize;
    char *out;
    struct grammar *result;
    std::string filename; 
    std::string header_filename;
    inputsize = fread(input, 1, sizeof(input), infile(argc,argv));
    //fprintf(stderr, "inputsize=%zu\ninput=", inputsize);
    //fwrite(input, 1, inputsize, stderr);  
    // print_parser_invocation(parse_parser_invocation(input,inputsize),stdout,0);
    // exit(0);
     result =  parse_trace_grammar(input,inputsize,stderr,">");
    //    result = parse_grammar(input,inputsize);
    //    if(result)
    //      print_grammar(result, stderr,0);    exit(1);
     std::string headerfilename = std::string(argv[1]) + ".h";
     std::string implfilename = std::string(argv[1]) + ".c";
     
     if(result) {
       std::ofstream header(headerfilename);
       std::ofstream impl(implfilename);
       if(!header.is_open())
         error("Cannot open output file %s\n",headerfilename.c_str());
       if(!impl.is_open())
         error("Cannot open output file %s\n",implfilename.c_str());
       emit_header(&header,result);
       impl << "#include \""<< headerfilename << "\""<<std::endl;
       emit_parser(&impl,result);
       emit_generator(&impl,result);
       impl << std::endl;
       header << std::endl;       
       return 0;
     } else {
       return 1;
     }
}
