# LL(1) Parser in C++
## Author: Evan Stella
## CSCI 3313: extra credit assignment

**LL(1) Parser**

This project implements the parse table generation (algorithms 3,4,5) discussed in HW6, as well as a simple lexer and parsing algorithm using a DPDA. The parser assumes it is given a grammar that is unambiguous and free of left recursions. The two following example grammars are provided in tests/example1.txt and test/example2.txt:

*S -> A | (S+A)<br />*
*A -> id<br />*
*id matches [a-z]+<br />*
<br />

*E -> TE'<br />*
*E' -> +TE' | epsilon<br />*
*T -> FT'<br />*
*T' -> \*FT' | epsilon<br />*
*F -> (E) | id<br />*
*id matches [a-z]+<br />*
<br />

**Program flow**

The program first either reads a grammar from a file like the ones specified above and generates a parser, or loads an already generated parser from disk. While the example grammars are simple and their parsers can be generated quickly, I included the option to save/load a generated parser because the parser generation can be slower for much more complicated grammars. The `-g` switch will generate a parser from a given file with a grammar, and the `-p` and `-o` switchs will load and save the parser respectively.

If input is given to parse, the lexer will tokenize the input for parsing, and the parser will parse the tokens. The `-i` switch specifies a given file to parse and `-s` will specifies a string to parser. The lexer tokenizes the input based on the regular expressions specified in the grammar (Note: the lexer ignores whitespace). The parser will then parse the input using a DPDA. The FIRST(), FOLLOW(), and parser generation algorithms can be found in `Parser::first()`, `Parser::follow()`, and `Parser::buildTable()` respectively.

Note: The `-v` is recommended and will enable verbose operations. `-h` will show all of the above options and some examples. 

**Usage**

Compile with `make`. I've included several test cases for each grammar in tests/.

