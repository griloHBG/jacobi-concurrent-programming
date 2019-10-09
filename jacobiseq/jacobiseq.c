//
// Created by grilo on 06/10/2019.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define MAX(A, B) A > B ? A : B
#define MIN(A, B) A < B ? A : B

int main(int argc, char* argv[])
{
    double* A;
    double* B;

    //help message for when program is missused
    char help_message[300];
    sprintf(help_message,
            "Usage:\n"
            "\t%s <N> <T>\n\n"
            "Where:\n"
            "\tN is the amount of equations pseudorandomly generated\n"
            "\tT is the amount of POSIX threads that will be used\n",
            argv[0]);

    //if there is something different than 2 command-line arguments, is missuse
    if( argc != 3)
    {
        printf("%s", help_message);
        exit(0);
    }

    //printing how much threads will be used to solve how much equations
    printf("%9s = %s\n%9s = %s","equations", argv[1], "threads", argv[2]);

    //N is the amount of equations
    int N = 3;//MAX(2, (int) strtol(argv[1], NULL, 0));
    //T is the amount of threads
    int T = 1;//MAX(1, (int) strtol(argv[2], NULL, 0));

    //matrix A [N x N] of the linear system
    A = (double*) calloc(N * N, sizeof(double));

    //column vector B [N x 1] of the linear system
    B = (double*) calloc(N, sizeof(double));

    //counters to walk by A and B
    int lin = 0, col = 0;

    double* line_sum = (double*)calloc(N, sizeof(double));

    //initializing A and B with random values
    for(lin = 0; lin < N; lin++)
    {
        line_sum[lin] = 0;

        for(col = 0; col < N; col++)
        {
            A[lin * N + col] = floor(10*((rand() % 1000) / 1000.0));

            line_sum[lin] += ((lin == col) ? 0 : A[lin * N + col]);
        }
        //Convergence is assured if, in a line, the sum of non-diagonal element is less than the diagonal element
        //So, to assure convergence, a random number is sumed to the diagonal element until it is greater than the line non-diagonal elements sum
        while(A[lin * N + lin] < line_sum[lin])
        {
            A[lin * N + lin] += floor(10*((rand() % 1000) / 1000.0));
        }
        B[lin] = 10 * ((rand() % 1000) / 1000.0);
    }

    //printing A
    printf("\nA:\n");
    for(lin = 0; lin < N; lin++)
    {
        for(col = 0; col < N; col++)
        {
            printf("%7.2lf", A[lin * N + col]);
        }
        printf("\t\tline sum: %7.2lf", line_sum[lin]);

        printf("\n");
    }

    //printing B
    printf("\nB:\n");
    for(lin = 0; lin < N; lin++)
    {
        printf("%7.2lf", B[lin]);
        printf("\n");
    }


    //Analysing convergence
    printf("Converges? Lets see...\n");

    double* A_star = (double*) calloc(N * N, sizeof(double));

    printf("\nA*:\n");
    for(lin = 0; lin < N; lin++)
    {
        for(col = 0; col < N; col++)
        {
            A_star[lin * N + col] = ((lin == col) ? 0 : A[lin * N + col] / A[lin * (N + 1)]);
        }

        B[lin] = B[lin] / A[lin * (N + 1)];
    }

    for(lin = 0; lin < N; lin++)
    {
        line_sum[lin] = 0;
        for(col = 0; col < N; col++)
        {
            printf("%7.2lf", A_star[lin * N + col]);
            line_sum[lin] += A_star[lin * N + col];
        }

        printf("\t\tline sum: %7.2lf", line_sum[lin]);

        printf("\n");
    }

    free(line_sum);
    free(A_star);
    free(A);
    free(B);
}

