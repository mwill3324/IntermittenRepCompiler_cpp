#ifndef _PARSEGEN_H_
#define _PARSEGEN_H_
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include "compiler.h"
#define MAX_TOKEN_LENGTH 255

string token;
struct StatementNode* States = new StatementNode;
struct StatementNode* parseList();

Token ttype;

struct GotoStatement* goto_stmt;

struct StatementNode* Vars = new StatementNode[255];

int numVars = 0;
int varCount = 0;

ValueNode* initialVars = new ValueNode[255];

Token peek()
{
    Token a = lexer.GetToken();
    lexer.UngetToken(a);
    return a;
}

struct ValueNode* Search(string name)
{
    bool found = false;
    struct ValueNode* var = new ValueNode;
    for(int i = 0;i<numVars;i++)
    {
        if(Vars[i].type == ASSIGN_STMT)
        {
            //cout<<"             COMPARING:  "<<name<<" : "<< Vars[i].assign_stmt->left_hand_side->name<<endl;
            if(Vars[i].assign_stmt->left_hand_side->name == name)
            {
                found = true;
                var = Vars[i].assign_stmt->left_hand_side;
            }
        }
    }
    //cout<<"a"<<endl;
    if(found == false)
    {
        //cout<<"FOUND = FALSE"<<endl;
        for(int j = 0; j < varCount; j++)
        {
            if(initialVars[j].name == name)
            {
                *var = initialVars[j];
                return var;
            }
        }
    }
    //cout<<"return"<<endl;

    return var;
}

struct ValueNode* Primexpr()
{
    struct ValueNode* primar = new ValueNode;
    ttype = peek();
    lexer.GetToken();
    if (ttype.token_type == NUM)
    {
        primar->value = atoi(ttype.lexeme.c_str());
    }
    return primar;
}

bool isop()
{
    if (ttype.token_type == PLUS||ttype.token_type == MINUS||ttype.token_type == MULT ||ttype.token_type == DIV)
        { return true;
        }
    else return false;
}

struct AssignmentStatement* parse_exp()
{
    struct AssignmentStatement* exp = new AssignmentStatement;
    ttype = peek();
    if (ttype.token_type == NUM||ttype.token_type == ID)
    {
        if(ttype.token_type == ID)
        {
               lexer.GetToken();
               exp->operand1 = Search(ttype.lexeme);
        }
        else////IS NUM
        {
            //is a NUM
            exp->operand1 = Primexpr();
        }
        ttype = peek();
        if (isop())
        {
            lexer.GetToken();
            if(ttype.token_type == PLUS)
            {
                exp->op = OPERATOR_PLUS;
            }
            if(ttype.token_type == MINUS)
            {
                exp->op = OPERATOR_MINUS;
            }
            if(ttype.token_type == MULT)
            {
                exp->op = OPERATOR_MULT;
            }
            if(ttype.token_type == DIV)
            {
                exp->op = OPERATOR_DIV;
            }
                ttype = peek();
            if ((ttype.token_type == ID)|(ttype.token_type == NUM))
            {
                if(ttype.token_type == ID)
                {
                    lexer.GetToken();
                    exp->operand2 = Search(ttype.lexeme);
                }
                else
                {
                    exp->operand2 = Primexpr();
                }
            }
            else
            {
                lexer.GetToken();
            }
        }
        else if (ttype.token_type == SEMICOLON)
        {
            exp->operand2 = NULL;
            exp->op = OPERATOR_NONE;
        }
    }
    else
    {
        lexer.GetToken();
    }
    return exp;
}

struct IfStatement* parseCond()
{
    struct IfStatement* con = new IfStatement;
    string op1;
    string op2;
    ttype = peek();
    op1 = ttype.lexeme;
    if (ttype.token_type == ID || ttype.token_type == NUM)
    {
        if(ttype.token_type == NUM)
        {
            con->condition_operand1 = Primexpr();
        }
        else
        {
            lexer.GetToken();
            con->condition_operand1 = Search(ttype.lexeme);
        }
        ttype = peek();
        lexer.GetToken();
        if(ttype.token_type <= LESS && ttype.token_type >= NOTEQUAL)
        {
            if(ttype.token_type == NOTEQUAL)
            {
                con->condition_op =  CONDITION_NOTEQUAL;
            }
            if(ttype.token_type == GREATER)
            {
                con->condition_op =  CONDITION_GREATER;
            }
            if(ttype.token_type == LESS)
            {
                con->condition_op =  CONDITION_LESS;
            }
            ttype = peek();
            op2 = ttype.lexeme;
            if (ttype.token_type == ID || ttype.token_type == NUM)
            {
                if(ttype.token_type == ID)
                {
                    lexer.GetToken();
                    con->condition_operand2 = Search(ttype.lexeme);
                }
                else
                {
                    con->condition_operand2 = Primexpr();
                }
            }
            else
            {
                lexer.GetToken();
            }
        }
    }
    //cout<<"           IF Conditions:  "<<op1<<"   "<<op2<<"       "<<endl;
    return con;
}

struct PrintStatement* print()
{
    struct PrintStatement* prin = new PrintStatement;
    ttype = peek();
    lexer.GetToken();
    //cout<<"p"<<endl;
    if(ttype.token_type == PRINT)
    {
        //cout<<"is p"<<endl;
        ttype = peek();
        lexer.GetToken();
        if (ttype.token_type == ID)
        {
            //cout<<"id in p"<<endl;
            prin->id = Search(ttype.lexeme);
        }
    }
    //cout<<"return print()"<<endl;
    return prin;
}

struct AssignmentStatement* parse_assign()
{
    struct ValueNode* lhs = new ValueNode;
    struct AssignmentStatement* mainState = new AssignmentStatement;
    ttype = peek();
    lexer.GetToken();//garbage
    token = ttype.lexeme;
    if (ttype.token_type == ID)
    {
        lhs = Search(ttype.lexeme);
        mainState->left_hand_side = lhs;
        ttype = peek();//
        lexer.GetToken();//garbage
        if (ttype.token_type == EQUAL)
        {
            mainState = parse_exp();
            mainState->left_hand_side = lhs;
            ttype = peek();
            if (ttype.token_type == SEMICOLON)
            {
                return mainState;
            }
            else
            {
                lexer.GetToken(); //garbage
            }
        }
    }
    return 0;
}

//BODY OF IF------
struct StatementNode* ifBody() {
    struct StatementNode* noopTemp = new StatementNode; //StatementNode();
    struct StatementNode* stl = new StatementNode;//StatementNode();
    //cout << "        Parse if BODY \n" ;
    noopTemp->type = NOOP_STMT;
    ttype = peek();
    lexer.GetToken();//garb
    //cout<<ttype.token_type<<" in bod {22 "<<ttype.lexeme<<endl;
    if (ttype.token_type == LBRACE)
    {
        //cout<<"plist"<<endl;
        stl = parseList();
        //cout<<"plist"<<endl;
        ttype = peek();
        lexer.GetToken();//garb
        //cout<<"bod"<<endl;
        if (ttype.token_type == RBRACE)
        {
            //cout<<"return body RBRACE"<<endl;
            //cout<<"RBRACE"<<endl;
            return stl;//->next->next->next;
        }
    }
   // cout<<"return body, no Rbrace"<<endl;
    return stl;
}

bool isstate()
{
    if(ttype.token_type == FOR||ttype.token_type == DEFAULT ||ttype.token_type == ID || ttype.token_type == IF || ttype.token_type == WHILE || ttype.token_type == PRINT || ttype.token_type == SWITCH||ttype.token_type== CASE)
            {
                return true;
            }
    else return false;
}

struct ValueNode* defval = new ValueNode;


struct StatementNode* parseStatement()
{
    struct StatementNode* mainState = new StatementNode;
    ttype = lexer.GetToken();
   // cout<<"parse state"<<endl;
    if (ttype.token_type == ID)
    {
        cout<<"assign"<<endl;
        lexer.UngetToken(ttype);
        mainState->assign_stmt = parse_assign();
        mainState->type = ASSIGN_STMT;
        if(1)
        {
            //cout<<"new Var"<<endl;
           // cout<<mainState->assign_stmt->left_hand_side->name<<endl;
            //cout<<mainState->assign_stmt->operand1->value<<endl;

            Vars[numVars] = *mainState;
            numVars++;
        }
        ttype =  peek();
        lexer.GetToken();
        if (ttype.token_type == SEMICOLON)
        {
            ttype = peek();
            if(isstate())
            {
                return mainState;
            }
            else if(ttype.token_type == RBRACE)
            {
                mainState->next = NULL; //no changes
            }
            return mainState;
        }
    }
    else if (ttype.token_type == PRINT)
    {
        cout<<"print"<<endl;
        lexer.UngetToken(ttype);
        mainState->print_stmt = print();
        //cout<<"after print()"<<endl;
        mainState->type = PRINT_STMT;
        //////vvvv fixed to set correct location///////
        //////////////////////////////////////////////
        ttype = peek();
        lexer.GetToken();
        if (ttype.token_type == SEMICOLON)
        {
            ttype = peek();
            if(isstate())
            {
                return mainState;
            }
            else if(ttype.token_type == RBRACE)
            {
               mainState->next = NULL; // no changes?
            }
            return mainState;
        }
    }
    else if (ttype.token_type == IF)
    {
       // cout<<"START OF ------- IF IF IF IF IF ----- START"<<endl;
       // cout<<"START OF ------- IF IF IF IF IF ----- START"<<endl;
        mainState->type = IF_STMT;
        struct IfStatement* iftemp = new IfStatement;//IfStatement();
        struct StatementNode* if2 = new StatementNode;
        struct StatementNode* noopTemp = new StatementNode;
        mainState->if_stmt = iftemp;
        ////////////////////////
        iftemp = parseCond();
        iftemp->true_branch = ifBody();

        if2 = iftemp->true_branch;
        noopTemp->type = NOOP_STMT;
        //***while loop appends the noop to the
        // end of all of the if statement..
        // else prog will not continue past true if.
        while(1)
        {
            if(iftemp->true_branch->next == NULL)
            {
                iftemp->true_branch->next = noopTemp;
                iftemp->true_branch = if2;
                break;
            }
            iftemp->true_branch=iftemp->true_branch->next;
        }
        if(iftemp->true_branch == NULL)
        {
            exit(0);
        }

        iftemp->false_branch = noopTemp;
        mainState->if_stmt = iftemp;
        mainState->next = noopTemp;
       // cout<<"END OF ------- IF IF IF IF IF ----- END\n"<<endl;
        return mainState;
    }
    else if (ttype.token_type == WHILE)
    {
        //cout << "WHILE State" << endl;
        mainState->type = IF_STMT;
        struct IfStatement* iftemp = new IfStatement; //if stmt
        struct StatementNode* goNode = new StatementNode;
        struct GotoStatement* g2Node = new GotoStatement;
        struct StatementNode* tempStmt = new StatementNode;

        iftemp = parseCond();//condition
        mainState->if_stmt = iftemp; //
        iftemp->true_branch = ifBody();//populate the true branch

        goNode->goto_stmt = g2Node;
        goNode->type = GOTO_STMT; //set type

        g2Node->target = mainState;//set target to go back to

        tempStmt = iftemp->true_branch;
        while(1)
        {
            if(tempStmt->next == NULL)
            {
                break;
            }
            tempStmt = tempStmt->next;
        }
        tempStmt->next = goNode;

        struct StatementNode* noopTemp = new StatementNode;
        noopTemp->type = NOOP_STMT;
        iftemp->false_branch = noopTemp;
        mainState->next = noopTemp;
        return mainState;
    }
    else if (ttype.token_type == FOR)
    {
        cout<<"FOR"<<endl;
        //struct StatementNode* mainNext = new StatementNode;
        struct StatementNode* ifNext = new StatementNode;
        struct StatementNode* ifNext2 = new StatementNode;

        struct StatementNode* ifStart = new StatementNode;
        struct StatementNode* ifHolder = new StatementNode;
        struct IfStatement* iftemp = new IfStatement; //if stmt

        struct IfStatement* assif = new IfStatement; //if stmt
        struct ValueNode* assval1 = new ValueNode;
        struct ValueNode* assval2 = new ValueNode;


        struct StatementNode* goNode = new StatementNode;
        struct GotoStatement* g2Node = new GotoStatement;
        struct StatementNode* tempStmt = new StatementNode;
        //first for
        ttype = lexer.GetToken();//consume (

        assval1->value = 1;
        assval2->value = 2;

        ////The assif is just to hold an assign statement, in the
        ////mainState, so it can do the int a = 0; part of the for loop
        ////first and do it only once, then the true branch of the assif
        ////is the actual for loop.

        assif->condition_operand1 = assval1;
        assif->condition_operand2 = assval2;
        assif->condition_op = CONDITION_NOTEQUAL;
        mainState->type = IF_STMT;
        mainState->if_stmt = assif;

        assif->true_branch = parseStatement();

        iftemp = parseCond();//condition

        assif->true_branch->next = new StatementNode; //mainNext;//just to fill with memory?
        assif->true_branch->next->if_stmt = iftemp;
        assif->true_branch->next->type = IF_STMT;


        ttype = lexer.GetToken();//consume ; , parsecond doesnt consume it ;
        ifHolder = parseStatement();// this takes in the i++ part of a for loop
        ttype = lexer.GetToken();//consume)
        ifNext = ifBody(); //gets the body
        ifNext2 = ifNext;
        ifStart = ifNext; // saves body start node

        while(1)//this loop goes to the end of the body
        {
            if(ifNext2->next == NULL)
            {
                ifNext2->next = ifHolder;// this adds the i++ part to the end of the body..
                ifNext2 = ifNext2->next;
                break;
            }
            ifNext2 = ifNext2->next;
        }
        iftemp->true_branch = ifStart; //sets true branch to the start of the body
        goNode->goto_stmt = g2Node;
        goNode->type = GOTO_STMT; //set type
        g2Node->target = assif->true_branch->next;//g2Node->target = mainState->next;//set target to go back to if loop to loop through

        tempStmt = iftemp->true_branch;
        while(1)
        {
            if(tempStmt->next == NULL)
            {
                break;
            }
            tempStmt = tempStmt->next;
        }
        tempStmt->next = goNode;

        struct StatementNode* noopTemp = new StatementNode;
        noopTemp->type = NOOP_STMT;
        iftemp->false_branch = noopTemp;
        assif->false_branch = noopTemp;
        mainState->next = noopTemp;//mainState->next->next = noopTemp;
        return mainState;
    }
    else if(ttype.token_type == SWITCH)
    {
         //cout<<"START OF ------- switch ----- START"<<endl;
       // cout<<"START OF ------- IF IF IF IF IF ----- START"<<endl;
        mainState->type = IF_STMT;
        struct ValueNode* var = new ValueNode;
        struct IfStatement* iftemp = new IfStatement;//IfStatement();
        struct StatementNode* if2 = new StatementNode;
        struct StatementNode* ifend = new StatementNode;

        struct StatementNode* tempStmt = new StatementNode;
        struct StatementNode* noopTemp = new StatementNode;

        struct StatementNode* goNode = new StatementNode;
        struct GotoStatement* g2Node = new GotoStatement;

        mainState->if_stmt = iftemp;
        //cout<<"a"<<endl;
        ///////////condition always true//////
        ttype = lexer.GetToken(); //get VAR
        //////////////////////////////////////
        //cout<<"lexeme "<<ttype.lexeme<<endl;
        ////////iftemp will hold all of the other cases////
        //cout<<"Search?"<<endl;
        var = Search(ttype.lexeme);
        defval = var;

        iftemp->condition_operand1 = var;
        iftemp->condition_operand2 = var;
        iftemp->condition_op = CONDITION_NOTEQUAL;
        //cout<<"end search & if ops"<<endl;
        //////var is the switch variable////////
        iftemp->false_branch = ifBody();
        //cout<<"a"<<endl;
        if2 = iftemp->false_branch;
        tempStmt = iftemp->false_branch;
        noopTemp->type = NOOP_STMT;


        goNode->goto_stmt = g2Node;
        goNode->type = GOTO_STMT; //set type
        g2Node->target = noopTemp;
        //***while loop appends the noop to the
        // end of all of the if statement..
        // else prog will not continue past true if.
        //cout<<"while in switch"<<endl;
        ////////////This does all of the cases
        while(1)
        {
            if( iftemp->false_branch->if_stmt!= NULL)
            {
                iftemp->false_branch->if_stmt->condition_operand2 = var;
            }
            if(iftemp->false_branch->next == NULL)
            {
                iftemp->false_branch->next = noopTemp;
                ifend = iftemp->false_branch;
                iftemp->false_branch = if2;
                break;
            }
            //cout<<"a"<<endl;
            iftemp->false_branch = iftemp->false_branch->next;
        }
        ///////////////sets next to noop///////////////////////
        ///////////////////////////////////////////////////////
        iftemp->false_branch = if2;
        struct StatementNode* startfalse = new StatementNode;
        struct StatementNode* go3 = new StatementNode;
        struct GotoStatement* gNode = new GotoStatement;


        go3->goto_stmt = gNode;
        go3->type = GOTO_STMT; //set type
        gNode->target = ifend;

        while(1)
        {
            if(iftemp->false_branch->if_stmt != NULL)
            {
                startfalse = iftemp->false_branch->if_stmt->false_branch;
                while(1)
                {
                    if(iftemp->false_branch->if_stmt->false_branch->next->type == GOTO_STMT)
                    {
                        iftemp->false_branch->if_stmt->false_branch->next->goto_stmt->target = ifend;
                        iftemp->false_branch->if_stmt->false_branch = startfalse;
                        break;
                    }
                    if(iftemp->false_branch->if_stmt->false_branch->next == NULL)
                    {
                        break;
                    }
                    iftemp->false_branch->if_stmt->false_branch = iftemp->false_branch->if_stmt->false_branch->next;
                }
            }
            if(iftemp->false_branch->next == NULL)
            {
                break;
            }
            iftemp->false_branch = iftemp->false_branch->next;
        }
        iftemp->false_branch = if2;
        if(iftemp->false_branch == NULL)
        {
            exit(0);
        }

        iftemp->true_branch = noopTemp;
        mainState->if_stmt = iftemp;
        mainState->next = noopTemp;
       // cout<<"END OF ------- IF IF IF IF IF ----- END\n"<<endl;
        return mainState;
    }
    else if (ttype.token_type == CASE)
    {
        cout<<"CASE"<<endl;
        mainState->type = IF_STMT;
        struct ValueNode* casenum = new ValueNode;
        struct IfStatement* iftemp = new IfStatement;//IfStatement();
        struct StatementNode* tempStmt = new StatementNode;
        struct StatementNode* noopTemp = new StatementNode;
        struct StatementNode* goNode = new StatementNode;
        struct GotoStatement* g2Node = new GotoStatement;
        mainState->if_stmt = iftemp;
        ////////////////////////
        casenum = Primexpr();
        iftemp->condition_operand1 = casenum;

        iftemp->condition_op = CONDITION_NOTEQUAL;
        ttype = lexer.GetToken();// CONSUME COLON

        iftemp->false_branch = ifBody();//iftemp->true_branch = ifBody();

        tempStmt = iftemp->false_branch;//

        noopTemp->type = NOOP_STMT;
        //***while loop appends the noop to the
        // end of all of the if statement..
        // else prog will not continue past true if.
        goNode->goto_stmt = g2Node;
        goNode->type = GOTO_STMT; //set type
        g2Node->target = noopTemp;//mainState->next;//set target to go back to if loop to loop through

        while(1)
        {
            cout<<"aaaa"<<endl;
            if(tempStmt->next == NULL)
            {
                tempStmt->next = goNode;
                //tempStmt = if2;
                break;
            }
            tempStmt=tempStmt->next;
        }
        //tempStmt->next = goNode;
        //iftemp->false_branch->next = goNode;

        if(iftemp->false_branch == NULL)
        {
            exit(0);
        }

        iftemp->true_branch = noopTemp;
        mainState->if_stmt = iftemp;
        mainState->next = noopTemp;
       // cout<<"END OF ------- IF IF IF IF IF ----- END\n"<<endl;
        return mainState;
    }
    else if (ttype.token_type==DEFAULT)
    {
        cout<<"DEFAULT"<<endl;
        mainState->type = IF_STMT;
        struct IfStatement* iftemp = new IfStatement;//IfStatement();
        struct StatementNode* tempStmt = new StatementNode;
        struct StatementNode* noopTemp = new StatementNode;
        struct StatementNode* goNode = new StatementNode;
        struct GotoStatement* g2Node = new GotoStatement;
        mainState->if_stmt = iftemp;
        ////////////////////////
        iftemp->condition_operand1 = defval;

        iftemp->condition_op = CONDITION_NOTEQUAL;
        ttype = lexer.GetToken();// CONSUME COLON

        cout<<"COL "<<ttype.token_type<<endl;
        iftemp->false_branch = ifBody();//iftemp->true_branch = ifBody();

        tempStmt = iftemp->false_branch;//

        noopTemp->type = NOOP_STMT;
        //***while loop appends the noop to the
        // end of all of the if statement..
        // else prog will not continue past true if.
        goNode->goto_stmt = g2Node;
        goNode->type = GOTO_STMT; //set type
        g2Node->target = noopTemp;//mainState->next;//set target to go back to if loop to loop through

        while(1)
        {
            if(tempStmt->next == NULL)
            {
                tempStmt->next = goNode;
                break;
            }
            tempStmt=tempStmt->next;
        }
        //tempStmt->next = goNode;
        //iftemp->false_branch->next = goNode;
        if(iftemp->false_branch == NULL)
        {
            exit(0);
        }

        iftemp->true_branch = noopTemp;
        mainState->if_stmt = iftemp;
        mainState->next = noopTemp;
       // cout<<"END OF ------- IF IF IF IF IF ----- END\n"<<endl;
        return mainState;
    }
    else if(ttype.token_type == RBRACE)
    {
        lexer.UngetToken(ttype);
    }
    return mainState;
}


void GetVariables()
{
    ttype = peek();
    lexer.GetToken();
    if (ttype.token_type == ID)
    {
        initialVars[varCount].name = ttype.lexeme;
        initialVars[varCount].value = 0;
        varCount++;
        ttype = peek();
        if (ttype.token_type == SEMICOLON)
        {
            lexer.GetToken();
            return;
        }
        else if (ttype.token_type == COMMA)
        {
            lexer.GetToken();
            GetVariables();
        }
    }
}

struct StatementNode* parseList()
{
    struct StatementNode* stmt1 = new StatementNode;   // statement
    struct StatementNode* stmt2 = new StatementNode;   // statement list
    //cout<<"parseState"<<endl;
    stmt1 = parseStatement();
    ttype = peek();
    //cout<<"after statement"<<endl;
    if (!isstate())
    {
        return stmt1;
    }
    else
    {
        stmt2 = parseList();
        if (stmt1->type == IF_STMT)
        {
            stmt1->next->next = stmt2;//->next = stmt2; // NEEDS TO BE FIXED FOR SWITCH
        }
        else //its not just another if st, can procede normally..
        {
            stmt1->next = stmt2;
        }
        return stmt1;
    }
    //cout<<endl;
}

struct StatementNode* parse_generate_intermediate_representation()
{
    GetVariables();
    ttype = lexer.GetToken();
    if (ttype.token_type == LBRACE)
    {
        States = parseList(); //read in body
        return States;
    }
    return 0;
}

#endif
