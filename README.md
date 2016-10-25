# Vector-Calculator-on-Linux
This project implements a comman-line vector calculator on Linux. It's based on parse tree.

The state analysis file outlines possible valid/invalid steps in building the parse tree. It helps the syntex check of the input.

The state action file outlines what to do in next step in parsing the input.


Employed data structures:
1) A symbol table is used to store names and vectors of variables. Its data structure is defined as

struct Symbol  {
	char     name;
	int        size;
	double vector[MaxNumElement];
}  SymTable[26];

2) The data structure of the parse tree is defined as

struct Tree  {
	int         op; 			/* the codes for input expressions */
	double  value;			/* number value, or # of elements in a vector */
	double  vector[MaxNumElement];	/* vector */
	Symbol *symbol;			/* variable */
	Tree     *left;
	Tree     *right;
};

3) codes for input expressions:

enum  {
	INITIAL,  ASSIGN,    NUMBER,  VARIABLE,  VECTOR,    ADD,  SUBSTRACT,  MULTIPLY,  DIVIDE,
};


The project provides a shell script - huang.bash - as a test framework.
