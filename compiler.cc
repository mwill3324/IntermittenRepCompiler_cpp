#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cctype>
#include <cstring>
#include "lexer.cc"
#include "compiler.h"
#include "parsegen.h"
#include "ir_debug.h"
#define DEBUG 0     // 1 => Turn ON debugging, 0 => Turn OFF debugging

void debug(const char* format, ...)
{
    va_list args;
    if (DEBUG)
    {
        va_start (args, format);
        vfprintf (stdout, format, args);
        va_end (args);
    }
}

void execute_program(struct StatementNode * program)
{
    struct StatementNode * pc = program;
    int op1, op2, result;
    //cout << "-------------execute_program start----------------"<<endl;
    while (pc != NULL)
    {
        switch (pc->type)
        {
            case NOOP_STMT:
                //cout<<"NOOP_STMT"<<endl;
                pc = pc->next;
                break;
            case PRINT_STMT:
                //cout<<"PRINT_STMT ";
                if (pc->print_stmt == NULL)
                {
                   // cout<<"Error: pc points to a print statement but pc->print_stmt is null.\n"<<endl;
                    exit(1);
                }
                if (pc->print_stmt->id == NULL)
                {
                    //cout<<"Error: print_stmt->id is null.\n"<<endl;
                    exit(1);
                }
                //cout<<pc->print_stmt->id->name<<": "<<pc->print_stmt->id->value<<endl;
                printf("%d\n", pc->print_stmt->id->value);
                pc = pc->next;
                break;
            case ASSIGN_STMT:
                //cout<<"ASSIGN_STMT"<<endl;
                if (pc->assign_stmt == NULL)
                {
                    debug("Error: pc points to an assignment statement but pc->assign_stmt is null.\n");
                    exit(1);
                }
                if (pc->assign_stmt->operand1 == NULL)
                {
                    debug("Error: assign_stmt->operand1 is null.\n");
                    exit(1);
                }
                if (pc->assign_stmt->op != OPERATOR_NONE)
                {
                    if (pc->assign_stmt->operand2 == NULL)
                    {
                        debug("Error: assign_stmt->op requires two operands but assign_stmt->operand2 is null.\n");
                        exit(1);
                    }
                }
                if (pc->assign_stmt->left_hand_side == NULL)
                {
                    debug("Error: assign_stmt->left_hand_side is null.\n");
                    exit(1);
                }
                switch (pc->assign_stmt->op)
                {
                    //cout<<"----op----"<<endl;
                    case OPERATOR_PLUS:
                        //cout<<"var: " <<pc->assign_stmt->left_hand_side->name<< " = ";
                        //cout<<pc->assign_stmt->operand1->name<< "+";
                        //cout<<pc->assign_stmt->operand2->name<<endl;
                        op1 = pc->assign_stmt->operand1->value;
                        op2 = pc->assign_stmt->operand2->value;
                        result = op1 + op2;
                        break;
                    case OPERATOR_MINUS:
                        //cout<<"var: " <<pc->assign_stmt->left_hand_side->name<< " = ";
                        //cout<<pc->assign_stmt->operand1->name<< "-";
                        //cout<<pc->assign_stmt->operand2->name<<endl;
                        op1 = pc->assign_stmt->operand1->value;
                        op2 = pc->assign_stmt->operand2->value;
                        result = op1 - op2;
                        break;
                    case OPERATOR_MULT:
                        //cout<<"var: " <<pc->assign_stmt->left_hand_side->name<< " = ";
                        //cout<<pc->assign_stmt->operand1->name<< "*";
                        //cout<<pc->assign_stmt->operand2->name<<endl;
                        op1 = pc->assign_stmt->operand1->value;
                        op2 = pc->assign_stmt->operand2->value;
                        result = op1 * op2;
                        break;
                    case OPERATOR_DIV:
                        //cout<<"var: " <<pc->assign_stmt->left_hand_side->name<< " = ";
                        //cout<<pc->assign_stmt->operand1->name<< "/";
                        //cout<<pc->assign_stmt->operand2->name<<endl;
                        op1 = pc->assign_stmt->operand1->value;
                        op2 = pc->assign_stmt->operand2->value;
                        result = op1 / op2;
                        break;
                    case OPERATOR_NONE:
                        //cout<<"var: " <<pc->assign_stmt->left_hand_side->name<< " = ";
                        //cout<<pc->assign_stmt->operand1->name<<" "<<endl;
                        op1 = pc->assign_stmt->operand1->value;
                        result = op1;
                        break;
                    default:
                        debug("Error: invalid value for assign_stmt->op (%d).\n", pc->assign_stmt->op);
                        exit(1);
                        break;
                }
                pc->assign_stmt->left_hand_side->value = result;
                //cout<<"RESULT: "<<result<<endl;
                pc = pc->next;
                break;

            case IF_STMT:
                //cout<<"\n      ---IF_STMT---"<<endl;
                if (pc->if_stmt == NULL)
                {
                    //cout<<"Error: pc points to an if statement but pc->if_stmt is null.\n"<<endl;
                    debug("Error: pc points to an if statement but pc->if_stmt is null.\n");
                    exit(1);
                }
                if (pc->if_stmt->true_branch == NULL)
                {
                    //cout<<"Error: if_stmt->true_branch is null.\n"<<endl;
                    debug("Error: if_stmt->true_branch is null.\n");
                    exit(1);
                }
                if (pc->if_stmt->false_branch == NULL)
                {
                    //cout<<"Error: if_stmt->false_branch is null.\n"<<endl;
                    debug("Error: if_stmt->false_branch is null.\n");
                    exit(1);
                }
                if (pc->if_stmt->condition_operand1 == NULL)
                {
                    //cout<<"Error: if_stmt->condition_operand1 is null.\n"<<endl;
                    debug("Error: if_stmt->condition_operand1 is null.\n");
                    exit(1);
                }
                if (pc->if_stmt->condition_operand2 == NULL)
                {
                    //cout<<"Error: if_stmt->condition_operand2 is null.\n"<<endl;
                    debug("Error: if_stmt->condition_operand2 is null.\n");
                    exit(1);
                }
                op1 = pc->if_stmt->condition_operand1->value;
                op2 = pc->if_stmt->condition_operand2->value;
                //cout<<"op1: "<<pc->if_stmt->condition_operand1->name<<" "<<op1;
                //cout<<"  op2: "<<pc->if_stmt->condition_operand2->name<<" "<<op2<<endl;
                switch (pc->if_stmt->condition_op)
                {
                    case CONDITION_GREATER:
                        if (op1 > op2)
                        {
                            //cout<<"       TRUE BRANCH"<<endl;
                            pc = pc->if_stmt->true_branch;
                        }
                        else
                        {
                            //cout<<"       FALSE BRANCH"<<endl;
                            pc = pc->if_stmt->false_branch;
                        }
                        break;
                    case CONDITION_LESS:
                        if (op1 < op2)
                        {
                            //cout<<"       TRUE BRANCH"<<endl;
                            pc = pc->if_stmt->true_branch;
                        }
                        else
                        {
                            //cout<<"       FALSE BRANCH"<<endl;
                            pc = pc->if_stmt->false_branch;
                        }
                        break;
                    case CONDITION_NOTEQUAL:
                        if (op1 != op2)
                        {
                            //cout<<"       TRUE BRANCH"<<endl;
                            pc = pc->if_stmt->true_branch;
                        }
                        else
                        {
                            //cout<<"       FALSE BRANCH"<<endl;
                            pc = pc->if_stmt->false_branch;
                        }
                        break;
                    default:
                        debug("Error: invalid value for if_stmt->condition_op (%d).\n", pc->if_stmt->condition_op);
                        exit(1);
                        break;
                }
                break;
            case GOTO_STMT:
                //cout<<"GOTO_STMT"<<endl;
                if (pc->goto_stmt == NULL)
                {
                    debug("Error: pc points to a goto statement but pc->goto_stmt is null.\n");
                    exit(1);
                }
                if (pc->goto_stmt->target == NULL)
                {
                    debug("Error: goto_stmt->target is null.\n");
                    exit(1);
                }
                pc = pc->goto_stmt->target;
                break;
            default:
                debug("Error: invalid value for pc->type (%d).\n", pc->type);
                exit(1);
                break;
        }
    }
}

int main()
{
    //cout<<"---------------------start--------------------"<<endl;
    struct StatementNode * program;
    program = parse_generate_intermediate_representation();
    //cout<<"end"<<endl;
    if (DEBUG)
    {
        printf("\n-------------------------------------------\n");
        print_program(program);
        printf("\n-------------------------------------------\n");
    }
    //cout<<"execute_program"<<endl;
    execute_program(program);
    //cout<<"finished - execute"<<endl;
    return 0;
}
