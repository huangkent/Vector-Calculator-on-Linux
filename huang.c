/*	Name:	Weijun Huang
*/


/*	Arithmatic Calculator for Vectors

    Read, Store, and Evaluate input expressions
    Save and Print out their results

    Primary data structures employed:
    struct array SymTable -- vectors of all possible variables
    binary parse tree Exprsn -- input statement                 */



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define LineLeng 100
#define NumLeng  10
#define MaxNumElement 10



enum {  /* codes for input expressions */
	/* (the range of the op field of Tree structure */
  INITIAL,
  ASSIGN,
  NUMBER,
  VARIABLE,
  VECTOR,
  ADD,
  SUBSTRACT,
  MULTIPLY,
  DIVIDE,

};


typedef struct Symbol Symbol;
typedef struct Tree Tree;

struct Symbol {

	char   name;
        int    size;
	double vector[MaxNumElement];

}  SymTable[26];    // table: names and vectors of variables

struct Tree {

	int    op;                     // codes for input expressions
	double value;                  // number value, or # of elements in a vector
	double vector[MaxNumElement];
	Symbol *symbol;                // variable
	Tree   *left;
	Tree   *right;
  
};


Tree   *Exprsn = NULL;			  /* the pointer to the parse tree */
Tree   *curnode = NULL, *newnode = NULL;
int    curtype = -1, newtype = -1;	  /* the types of the current and the new characters */
double S[MaxNumElement];


Tree *createnode(int op);
Tree *insertnode(Tree *treep, Tree *currentp, Tree *newp, int type);
int evaluate(Tree *t, double *S);
void freetree(Tree *t);




int main()  {

    char *line = NULL, *realline = NULL;
    char expression[LineLeng];
    char v[NumLeng];

    int letter;
    int cnt, index, n;
    int i, j, k, start;
    int nbytes = LineLeng;
    int NumberIsDividor = 0;

    for (i = 0; i < 26; i ++)
    {
        SymTable[i].name = 'a' + i;    /* Initialization of SymTable: the variable table */
        SymTable[i].size = 0;
    }
     
    /* Read the 1st input evaluating statement */
    // printf("\nEnter a statement ");
    // printf("(symbol ! indicates stopping): ");
    getline(&line, &nbytes, stdin);


    while (line[0] != '!')
    {

        /* locate the 1st non-blank char in the line */
	start = strspn(line, " ");
        realline = &line[start];


        cnt = 0;
        i = 0;
        while ( realline[i] != '\0' && realline[i] != '\n' )
        {
            if (realline[i] != ' ')
                cnt ++;
            i ++;
        }


        if (cnt == 0)

            goto nextline;


        /* check if printout is requested for a variable */
        if ( cnt == 1 && (realline[0] >= 'a' && realline[0] <= 'z') )
        {
            letter = realline[0] - 'a';           // the index of the variable in SymTable
            n = SymTable[letter].size;            // the size of the variable

            /* print out a variable */

            if (n <= 0)
                printf("\nError:  %c is NOT valued yet.\n", SymTable[letter].name);
            else
            {
                // printf("\n");
                for (i = 0; i < n; i ++ )
                    printf("%lf\n", SymTable[letter].vector[i]);
                printf("\n");
            }

            goto nextline;
        }


	/* check if the statement begins with 'let ' */
        n = 4;
	if ( strncmp(realline, "let ", n) != 0 )
        {
            printf("\nError:  an statement should begin with 'let '.\n");
	    goto nextline;
	}

        /* clear expression[1..LineLeng] to hold the input expression */
        for (i = 0; i < LineLeng; i ++)
            expression[i] = '\0';

        /* extract the expression from the line */
	i = n;
	j = 0;
	while (realline[i] != '\0' && realline[i] != '\n')
        {
            if (realline[i] != ' ')
            {
                expression[j] = realline[i];
                j ++;
            }
            i ++;
	}

	/* check the completeness of the expression */
        if (j <= 0)                                     /* if the expression is empty (its length is 0) */
		goto nextline;
	else if (j < 3)                                 /* if the expression contains only two characters */
	     {
		printf("\nError:  the input expression is incomplete.\n");
		goto nextline;
	     }
	else if ( (expression[j-1] >= '0' && expression[j-1] <= '9') || (expression[j-1] >= 'a' && expression[j-1] <= 'z') )
                 ;
	else
	{
		printf("\nError:  the input expression might be incomplete, or end with an illegal character.\n");
		goto nextline;
	}


        /* the initial of the expression */
        if (expression[0] >= 'a' && expression[0] <= 'z')
        {
            newtype = INITIAL;
	}
        else
        {
            printf("\nError:  the INITIAL should be a lowercase letter.\n");
            goto nextline;
        }
        newnode = createnode(INITIAL);
        newnode->symbol = &SymTable[expression[0]-'a'];
        Exprsn  = insertnode(Exprsn, NULL, newnode, newtype);
        curnode = newnode;
        curtype = newtype;
        newtype = -1;


        /* finish the parse tree */
        i = 1;
        while (expression[i] != '\0')
        {

          /* decide the type of the current character */

            if ( (expression[i] >= '0' && expression[i] <= '9') || expression[i] == '.' )

                if (i > 2)  // if this digit does not appear next to ASSIGN at its right
                            // (actually, both NUMBER and VECTOR will be read in as a whole, once detected)

                    newtype = NUMBER;

                else    // expression[i] may be either NUMBER or VECTOR
                {
                    j = i + 1;

                    // scan the expression
                    while ( (expression[j] >= '0' && expression[j] <= '9') || expression[j] == '.' )
                    {
                        if (j == LineLeng - 1)
                            break;
                        else
                            j ++;
                    }
                    if ( expression[j] == ',' ||                       // the 1st comma found
                         j == LineLeng - 1 || expression[j] == '\0' )  // the vector is a number

                       newtype = VECTOR;

                    else

                        newtype = NUMBER;

                }

            else if (expression[i] >= 'a' && expression[i] <= 'z')

                newtype = VARIABLE;

            else

                switch (expression[i])
                {
                    case '=':
                        newtype = ASSIGN;
                        break;

                    case '+':
                        newtype = ADD;
                        break;

                    case '-':
                        newtype = SUBSTRACT;
                        break;

                    case '*':
                        newtype = MULTIPLY;
                        break;

                    case '/':
                        newtype = DIVIDE;
                        break;

                    default:
                        printf("\nError:  '%c' is an illegal character.\n", expression[i]);
                        goto nextline;
                        // Thus, now newtype == -1.
                }

          /* add the new character into the tree */

            switch (newtype)
            {

                case ASSIGN:

                     if (curtype == INITIAL)
                     {
                         newnode = createnode(newtype);
                         Exprsn = insertnode(Exprsn, curnode, newnode, newtype);
                         curnode = newnode;
                         curtype = newtype;
                         newtype = -1;

                         i ++;
                     }
                     else
                     {
                         printf("\nError:  the ASSIGNMENT should follow the INITIAL variable.\n");
                         goto nextline;
                         // Thus, now newtype == ASSIGN.
                     }

                     break;

                case NUMBER:

                     if ( curtype == ASSIGN || curtype == DIVIDE ||
                          curtype == ADD    || curtype == SUBSTRACT )
                     {
			 for (k = 0; k < NumLeng; k++)
			     v[k] = '\0';

			 /* extract the number, and save it in array v */
			 v[0] = expression[i];
			 k = 1;
			 j = i + 1;    // keep index i for now
			 while ( (expression[j] >= '0' && expression[j] <= '9') || expression[j] == '.' )
			 {
			     if (k >= NumLeng)
			     {
				 k = NumLeng;
				 printf("\nError:  the length of a number can't exceed %d\n", k);
				 goto nextline;
                                 // Thus, now newtype == NUMBER
			     }
			     else
			     {
				 v[k] = expression[j];
				 k++;
				 j++;
			     }
			 }
			 i = j - 1;    // index i now locates the last place of the number

			 /* check the validness of the number */
                         // now v contains only digits or dots
			 cnt = 0;
			 for (j = 0; j < k; j ++)
			     if (v[j] == '.')
				 cnt ++;
			 if (cnt > 1 || v[k-1] == '.')
			 {
			     printf("\nError:  the dot is duplicated in a number, or a number ends with a dot.\n");
			     goto nextline;
                             // Thus, now newtype == NUMBER
			 }

			 if (curtype == DIVIDE)    NumberIsDividor = 1;

                         newnode = createnode(newtype);
			 newnode->value = atof(v);       // convert the extracted number to double type
                         Exprsn = insertnode(Exprsn, curnode, newnode, newtype);
                         curnode = newnode;
                         curtype = newtype;
                         newtype = -1;

                         i ++;
                     }

                     else
                     {
                         printf("\nError:  a NUMBER should follow an ASSIGNMENT, ADD, SUBSTRACT, MULTIPLY, or DIVIDE.\n");
                         goto nextline;
                         // Thus, now newtype == NUMBER.
                     }

                     break;

                case VECTOR:

                     if ( curtype == ASSIGN || curtype == MULTIPLY  ||
                          curtype == ADD    || curtype == SUBSTRACT )
                     {
                         newnode = createnode(newtype);
                         index = 0;

                         // read the whole vector
                         while ( expression[i] != '\0' )   // assumes the vector is the only thing in the right side of =
                         {
                             if (index >= MaxNumElement)
                             {
                                 printf("\nError:  the size of vector exceeds %d\n", MaxNumElement);
                                 free (newnode);
                                 goto nextline;
                             }

                             // clear array v
			     for (k = 0; k < NumLeng; k++)
			         v[k] = '\0';

			     /* extract the number, and save it in array v */
			     v[0] = expression[i];
			     k = 1;
                             j = i + 1;    // keep index i for now
			     while ( (expression[j] >= '0' && expression[j] <= '9') || expression[j] == '.' )
			     {
			         if (k >= NumLeng)
			         {
				     k = NumLeng;
				     printf("\nError:  the length of a number can't exceed %d\n", k);
                                     free (newnode);
				     goto nextline;
                                     // Thus, now newtype == VECTOR.
			         }
			         else
			         {
				     v[k] = expression[j];
				     k++;
				     j++;
			         }
			     }
                             i = j;  // i points to the character following the number

                             /* check the validness of the number */
                             // now v contains only digits or dots
			     cnt = 0;
			     for (j = 0; j < k; j ++)
			         if (v[j] == '.')
				     cnt ++;
			     if (cnt > 1 || v[k-1] == '.')
			     {
			         printf("\nError:  the dot is duplicated in a number, or a number ends with a dot.\n");
                                 free (newnode);
			         goto nextline;
                                 // Thus, now newtype == VECTOR.
			     }

                             // save the current number in the vector
                             newnode->vector[index] = atof(v);
                             index ++;

                             if (expression[i] != '\0' && expression[i] != ',')    // if the delimiter is not ','
                             {
                                 printf("\nError:  a delimiter in a VECTOR should be a COMMA.\n");
                                 free (newnode);
                                 goto nextline;
                             }
                             else
                                 i ++;

                         }  // while loop:  read the whole vector

                         newnode->value = index;
                         Exprsn = insertnode(Exprsn, curnode, newnode, newtype);
                         curnode = newnode;
                         curtype = newtype;
                         newtype = -1;

                         i ++;
                     }

                     else
                     {
                         printf("\nError:  an VECTOR should follow a ASSIGN, ADD, SUBSTRACT, or MULTIPLY.\n");
                         goto nextline;
                     }

                     break;

                case VARIABLE:

                     if ( curtype == ASSIGN || curtype == MULTIPLY  ||
                          curtype == ADD    || curtype == SUBSTRACT )
                     {
                         newnode = createnode(newtype);
                         newnode->symbol = &SymTable[expression[i] - 'a'];
                         newnode->value = newnode->symbol->size;
                         Exprsn  = insertnode(Exprsn, curnode, newnode, newtype);
                         curnode = newnode;
                         curtype = newtype;
                         newtype = -1;

                         i ++;
                     }
                     else
                     {
                         printf("\nError:  a VARIABLE should follow an ASSIGNMENT, ADD, SUBSTRACT, or MULTIPLY.\n");
                         goto nextline;
                         // Thus, now newtype == VARIABLE.
                     }

                     break;

                case ADD:

                     if ( (curtype == NUMBER && NumberIsDividor) || curtype == VECTOR || curtype == VARIABLE )
                     {
                         newnode = createnode(newtype);
                         Exprsn  = insertnode(Exprsn, curnode, newnode, newtype);
                         curnode = newnode;
                         curtype = newtype;
                         newtype = -1;

                         NumberIsDividor = 0;

                         i ++;
                     }
                     else
                     {
                         printf("\nError:  an ADD should follow a VECTOR, VARIABLE, or a scalar dividor.\n");
                         goto nextline;
                         // Thus, now newtype == ADD.
                     } 

                     break;

                case SUBSTRACT:

                     if ( (curtype == NUMBER && NumberIsDividor) || curtype == VECTOR || curtype == VARIABLE )
                     {
                         newnode = createnode(newtype);
                         Exprsn  = insertnode(Exprsn, curnode, newnode, newtype);
                         curnode = newnode;
                         curtype = newtype;
                         newtype = -1;

                         NumberIsDividor = 0;

                         i ++;
                     }
                     else
                     {
                         printf("\nError:  an SUBSTRCT should follow a VECTOR, VARIABLE, or a scalar dividor.\n"); 
                         goto nextline;
                         // Thus, now newtype == SUBSTRACT.
                     } 

                     break;

                case MULTIPLY:

                     if (curtype == NUMBER)
                     {
                         newnode = createnode(newtype);
                         Exprsn  = insertnode(Exprsn, curnode, newnode, newtype);
                         curnode = newnode;
                         curtype = newtype;
                         newtype = -1;

                         i ++;
                     }
                     else
                     {
                         printf("\nError:  a MULTIPLY should follow an NUMBER.\n");
                         goto nextline;
                         // Thus, now newtype == MULTIPLY.
                     }

                     break;

                case DIVIDE:

                     if (curtype == VECTOR || curtype == VARIABLE)
                     {
                         newnode = createnode(newtype);
                         Exprsn  = insertnode(Exprsn, curnode, newnode, newtype);
                         curnode = newnode;
                         curtype = newtype;
                         newtype = -1;

                         i ++;
                     }
                     else
                     {
                         printf("\nError:  a DIVIDE should follow an VECTOR or VARIABLE.\n");
                         goto nextline;
                     }

                     break;

            }  // end of switch:  add the new charater to the tree

        }   // end of the while loop:  finish the partse tree


        /* Evaluate the expression */

        n = evaluate(Exprsn, S);
        // n is the size of the outcome vector
        // S is a temporary array for storing evaluation outcome


	nextline:

        /* free the tree */

        freetree(Exprsn);
	Exprsn = NULL;
	newnode = NULL;
	curnode = NULL;

        /* Read next line */

        // printf("\nEnter a statement: ");
        getline(&line, &nbytes, stdin);
        curtype = newtype = -1;
        realline = NULL;

    }
    /* end of the outer while loop */


    printf("\n");
    return 0;

}


Tree *createnode(int op)  {

  Tree *newp;

    newp = (Tree *) malloc(sizeof(Tree));
    newp->op = op;
    newp->value = 0;
    newp->symbol = NULL;
    newp->left = NULL;
    newp->right = NULL;
    return newp;

}


Tree *insertnode(Tree *treep, Tree *currentp, Tree *newp, int type)  {

    switch (type)
    {
        case INITIAL:

             return newp;  // this is the 1sr node inserted into the tree

        case ASSIGN:

             newp->left = treep;
             treep = newp;
             return treep;       // ASSIGN is the root; INITIAL is its left child.

        case NUMBER:

             currentp->right = newp;
             return treep;           // NUMBER is the right chiled of =, +, -, or /

        case VARIABLE:

             currentp->right = newp;
             return treep;           // VARIABLE is the right child of =, +, -, or *

        case VECTOR:

             currentp->right = newp;
             return treep;           // VECTOR is the right child of =, +, -, or *

        case ADD:

             newp->left = treep->right;
             treep->right = newp;       // ADD is an operator of the lowest priority
             return treep;              // so it is inserted to the right of the root

        case SUBSTRACT:

             newp->left = treep->right;
             treep->right = newp;       // SUBSTRACT is an operator of the lowest priority
             return treep;              // so it is inserted to the right of the root

        case MULTIPLY:

             newp->left = currentp;

             if (treep->right == currentp)    // if the right child of the root is NUMBER
                 treep->right = newp;
             else                             // otherwise, the right child of the root is ADD or SUBSTRACT
                 treep->right->right = newp;  // and the right child of right child of the root is NUMBER

             return treep;

        case DIVIDE:

             newp->left = currentp;

             if (treep->right == currentp)    // if the right child of the root is VARIABLE or VECTOR
                 treep->right = newp;
             else                             // otherwise, the right child of the root is ADD or SUBSTRACT
                 treep->right->right = newp;  // and the right child of right child of the root is ADD or SUBSTRACT

             return treep;

    }

}


/* evaluate:  evaluates the expression saved in the parse tree
              saves its outcome vector in array S
              returns the size of its outcome vector */
int evaluate(Tree *t, double *S)  {

    double scalar, A[MaxNumElement], B[MaxNumElement];
    int    i, size, leftsize, rightsize;
    Symbol *p;

    if (t->op == ASSIGN)
    {
        size = evaluate(t->right, S);
        t->left->value = size;

        p = t->left->symbol;

        /* overwrite SymTable with the INITIAL */
        p->size = size;
        for (i = 0; i < size; i ++)
            p->vector[i] = S[i];

        return size;
    }

    switch (t->op)
    {
        case ADD:

             leftsize = evaluate(t->left, A);
             rightsize = evaluate(t->right, B);
             if (leftsize != rightsize)
                 printf("\nError:  ADDing two mismathing vectors.\n");
             else
                 for (i = 0; i < leftsize; i ++)
                     S[i] = A[i] + B[i];

             return leftsize;

        case SUBSTRACT:

             leftsize = evaluate(t->left, A);
             rightsize = evaluate(t->right, B);
             if (leftsize != rightsize)
                 printf("\nError:  SUBSTRACTion executed on two mismathing vectors.\n");
             else
                 for (i = 0; i < leftsize; i ++)
                     S[i] = A[i] - B[i];

             return leftsize;

        case VARIABLE:

             size = t->value;
             if (size <= 0)
             {
                 printf("\nError:  a VARIABLE is a zero-size vector.\n");
                 return size;
             }
             p = t->symbol;
             for (i = 0; i < size; i ++)
                 S[i] = p->vector[i];

             return size;

        case VECTOR:

             size = t->value;
             if (size <= 0)
             {
                 printf("\nError:  a zero-size vector.\n");
                 return size;
             }
             for (i = 0; i < size; i ++)
                 S[i] = t->vector[i];

             return size;

        case MULTIPLY:

             scalar = t->left->value;
             size   = evaluate(t->right, B);
             for (i = 0; i < size; i ++)
                 S[i] = scalar * B[i];

             return size;

        case DIVIDE:

             scalar = t->right->value;
             size   = evaluate(t->left, A);
             if (scalar != 0)
                 for (i = 0; i < size; i ++)
                     S[i] = A[i] / scalar;

             return size;
    }

}


void freetree(Tree *t)  {

	if (t == NULL)
		return;

	if (t->left != NULL)
		freetree(t->left);
	if (t->right != NULL)
		freetree(t->right);
	free(t);

	return;

}
