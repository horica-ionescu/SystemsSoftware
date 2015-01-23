#include <stdio.h>
#include <stdlib.h>

typedef struct{
    int op;
    int l;
    int m;
}instruction;

int base(int* stack, int l, int base){
    int b1 = base;
    while (l > 0){
        b1 = stack[b1 + 1];
        l--;
    }
    return b1;
}

void fetch(instruction* code, int* PC, instruction* IR){
    *IR = code[*PC];
    *PC += 1;
}

void execute(instruction* code, int* stack, int* BP, int* SP, int* PC, instruction IR){

    switch (IR.op){
        //lit
        case 1:
            *SP+=1;
            stack[*SP] = IR.m;
            break;
        //opr
        case 2:
            switch(IR.m){
                //ret
                case 0:
                    *SP = *BP - 1;
                    *PC = stack[*SP + 3];
                    *BP = stack[*SP + 2];
                    if (*SP == -1)
                        exit(0);
                    break;
                //neg
                case 1:
                    stack[*SP] = -stack[*SP];
                    break;
                //add
                case 2:
                    *SP -= 1;
                    stack[*SP] += stack[*SP + 1];
                    break;
                //sub
                case 3:
                    *SP -= 1;
                    stack[*SP] -= stack[*SP + 1];
                    break;
                //mul
                case 4:
                    *SP -= 1;
                    stack[*SP] *= stack[*SP + 1];
                    break;
                //div
                case 5:
                    *SP -= 1;
                    stack[*SP] /= stack[*SP + 1];
                    break;
                //odd
                case 6:
                    stack[*SP] %= 2;
                    break;
                //mod
                case 7:
                    *SP -= 1;
                    stack[*SP] = stack[*SP] % stack[*SP + 1];
                    break;
                //eql
                case 8:
                    *SP -= 1;
                    stack[*SP] = stack[*SP] == stack[*SP + 1];
                    break;
                //neq
                case 9:
                    *SP -= 1;
                    stack[*SP] = stack[*SP] != stack[*SP + 1];
                    break;
                //lss
                case 10:
                    *SP -= 1;
                    stack[*SP] = stack[*SP] < stack[*SP + 1];
                    break;
                //leq
                case 11:
                    *SP -= 1;
                    stack[*SP] = stack[*SP] <= stack[*SP + 1];
                    break;
                //gtr
                case 12:
                    *SP -= 1;
                    stack[*SP] = stack[*SP] > stack[*SP + 1];
                    break;
                //geq
                case 13:
                    *SP -= 1;
                    stack[*SP] = stack[*SP] >= stack[*SP + 1];
                    break;
                default:
                    printf("You have made an error\n");
                    break;
            }
                break;
        //lod
        case 3:
            *SP += 1;
            stack[*SP] = stack[base(stack, IR.l, *BP) + IR.m];
            break;
        //sto
        case 4:
            stack[base(stack, IR.l, *BP) + IR.m] = stack[*SP];
            printf("%d\n", *BP);
            *SP-=1;
            break;
        //cal
        case 5:
            stack[*SP + 1] = base(stack, IR.l, *BP);
            stack[*SP + 2] = *BP;
            stack[*SP + 3] = *PC;
            *BP = *SP + 1;
            *PC = IR.m;
            break;
        //inc
        case 6:
            *SP+=IR.m;
            break;
        //jmp
        case 7:
            *PC = IR.m;
            break;
        //jpc
        case 8:
            if (stack[*SP] == 0)
                *PC = IR.m;
            *SP -= 1;
            break;
        //out
        case 9:
            printf("%d\n", stack[*SP]);
            *SP -= 1;
            break;
        //in
        case 10:
            *SP+=1;
            scanf("%d", &stack[*SP]);
            break;
        default: printf("Incorrect code\n");
    }
}

void opcode(FILE *ofp, instruction IR){
    switch(IR.op){
        case 1:
            fprintf(ofp, "lit");
            break;
        case 2:
            fprintf(ofp, "opr");
            break;
        case 3:
            fprintf(ofp, "lod");
            break;
        case 4:
            fprintf(ofp, "sto");
            break;
        case 5:
            fprintf(ofp, "cal");
            break;
        case 6:
            fprintf(ofp, "inc");
            break;
        case 7:
            fprintf(ofp, "jmp");
            break;
        case 8:
            fprintf(ofp, "jpc");
            break;
        case 9:
            fprintf(ofp, "out");
            break;
        case 10:
            fprintf(ofp, "in");
            break;
        default:
            fprintf(ofp, "Incorrect code");
            break;
    }
}

int main(){

    FILE *ifp = fopen("mcode.txt", "r");
    FILE *ofp = fopen("stacktrace.txt", "w");

    instruction code[500];
    int stack[2000] = {0};
    int BP = 0; //base pointer
    int SP = -1; //stack pointer
    int PC = 0; //program counter
    instruction IR; //instruction register
    int i = 0, j, length, step;

    while (fscanf(ifp, "%d", &code[i].op) != EOF){
        fscanf(ifp, "%d", &code[i].l);
        fscanf(ifp, "%d", &code[i].m);
        i++;
    }

    fclose(ifp);

    length = i;

    fprintf(ofp, "Line\tOP\tL\tM\n");

    for (i = 0; i < length; i++){
        fprintf(ofp, "%d\t", i);
        opcode(ofp, code[i]);
        fprintf(ofp, "\t%d\t%d\n", code[i].l, code[i].m);
    }

    fprintf(ofp, "\n\t\t\t\tpc\tbp\tsp\n");
    fprintf(ofp, "Initial values\t\t\t%d\t%d\t%d\n", PC, BP, SP);

    printf("Output:\n");

    while(1){
        //Fetch Step
        step = PC;
        fetch(&code, &PC, &IR);


        //Execute Step
        execute(&code, &stack, &BP, &SP, &PC, IR);
        fprintf(ofp, "%d\t", step);
        opcode(ofp, IR);
        fprintf(ofp, "\t%d\t%d\t%d\t%d\t%d", IR.l, IR.m, PC, BP, SP);
        fprintf(ofp, "\t");
        for (j = 0; j <= SP; j++){
            //The next if and while statements separate activation records
            if (j != 0 && j == BP)// || j != 0 && stack[BP + 1] == j)
                fprintf(ofp, "|  ");
            i = BP;
            while (stack[i + 1] != 0){
                if (j == stack[i + 1])
                    fprintf(ofp, "|  ");
                i = stack[i + 1];

            }
            fprintf(ofp, "%d  ", stack[j]);
        }
        fprintf(ofp, "\n");

    }

    fclose(ofp);

    return 0;
}
