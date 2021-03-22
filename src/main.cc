#include "main.hh"
#include "parser.hh"
#include "grammar.hh"
#include "lexer.hh"
#include <iostream>
#include <string>
#include <fstream>



// driver function
int main (int argc, char** argv) {

    if (argc == 1) {
        printError();
        return 0;
    }

    bool optg, opto, optp, opti, optv, opts;
    optg = opto = optp = opti = optv  = opts = false;

    char *gfile, *pfile, *ofile, *ifile, *string;
    gfile = pfile = ofile = ifile = string = NULL;

    for (int i = 1; i < argc; i++) {

        if (strcmp("-h", argv[i]) == 0) {
            printHelp();
            return 0;
        }
        else if (strcmp("-g", argv[i]) == 0) {
            optg = true;
            if (i + 1 > argc) {
                printError();
                return 0;
            }
            gfile = argv[++i];
        }
        else if (strcmp("-o", argv[i]) == 0) {
            opto = true;
            if (i + 1 > argc) {
                printError();
                return 0;
            }
            ofile = argv[++i];
        }
        else if (strcmp("-p", argv[i]) == 0) {
            optp = true;
            if (i + 1 > argc) {
                printError();
                return 0;
            }
            pfile = argv[++i];
        }
        else if (strcmp("-i", argv[i]) == 0) {
            opti = true;
            if (i + 1 > argc) {
                printError();
                return 0;
            }
            ifile = argv[++i];
        }
        else if (strcmp("-s", argv[i]) == 0) {
            opts = true;
            if (i + 1 > argc) {
                printError();
                return 0;
            }
            string = argv[++i];
        }
        else if (strcmp("-v", argv[i]) == 0) {
            optv = true;
        }
        else {
            printError();
            return 0;
        }
    }

    Grammar G;
    Parser P (&G, optv);
    bool gloaded;

    // option generate generate parser from grammar
    if (optg) {
        if (optp) {
            std::cerr << "Error: Cannot generate parser and load from file. Pick one." << std::endl;
            printHelp();
            return 0;
        }
        G.readGrammar(gfile);
        if (optv)
            G.printRules();
        P.buildTable();
        gloaded = true;
    }
    // option load parser
    if (optp) {
        if (optg) {
            std::cerr << "Error: Cannot generate parser and load from file. Pick one." << std::endl;
            printHelp();
            return 0;
        }
        P.deserialize(pfile);
        gloaded = true;
        if (optv)
            G.printRules();
    }
    // should have a parser by now
    if (!gloaded) {
        std::cerr << "Error: No grammar loaded" << std::endl;
        return 0;
    }
    // serialize parser to output
    if (opto) {
        P.serialize(ofile);
    }
    //parse file
    if (opti) {
        if (opts) {
            std::cerr << "Error: Cannot file and string. Pick one." << std::endl;
            printHelp();
            return 0;
        }
        std::ifstream in(ifile);
        if (in.fail()) {
            std::cerr << "Error: failed to open " << ifile << std::endl;
            exit(EXIT_FAILURE);
        }
        std::string contents((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
        Lexer L(&G, optv);
        auto tokens = L.tokenize(contents.c_str());
        P.parse(tokens);
    }
    // parse string
    if (opts) {
        if (opti) {
            std::cerr << "Error: Cannot file and string. Pick one." << std::endl;
            printHelp();
            return 0;
        }
        Lexer L(&G, optv);
        auto tokens = L.tokenize(string);
        P.parse(tokens);
    }
}



void printHelp () {
    std::cout << "Options: " << std::endl;
    std::cout << "-h: show options" << std::endl;
    std::cout << "-g [filename]: generate a parser from the given LL1 grammar" << std::endl;
    std::cout << "-o [filename]: save generated parser to file" << std::endl;
    std::cout << "-p [filename]: use the given parser file to parse" << std::endl;
    std::cout << "-i [filename]: parse given file" << std::endl;
    std::cout << "-s \"[input string]\": parse given string" << std::endl;
    std::cout << "-v: verbose mode" << std::endl;
    std::cout << "\nExamples: " << std::endl;
    std::cout << "./parser -g grammar.txt -i input.txt:  generate a parser from grammar.txt and parse input.txt" << std::endl;
    std::cout << "./parser -g grammar.txt -o myparser:   generate a parser from grammar.txt and save it to myparser" << std::endl;
    std::cout << "./parser -p myparser -i \"my string\":   use parser saved in myparser to parse 'my string'" << std::endl;
    std::cout << "./parser -p myparser -i input.txt:     use parser saved in myparser to parse input.txt" << std::endl;
}


void printError () {
    std::cout << "Invalid options. Use -h for help" << std::endl;
}