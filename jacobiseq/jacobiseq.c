//
// Created by grilo on 06/10/2019.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <omp.h>
#include <time.h>

#define MAX(A, B) ((A) > (B) ? (A) : (B))

#undef EXAMPLE_TEST //to run only the test
#undef VERBOSE //to print a lot of stuff (A LOT)
#define USER_TEST // to allow user to test one of the problem equations

void printColumnVector(double* vec, int N, char* name){

    int lin;

    printf("%s:", name);

    for(lin = 0; lin < N; lin++)
    {
        printf("\n");

        printf("%10.4lf", vec[lin]);
    }
}

void printMatrix(double* matrix, int N, char* name){

    int lin, col;

    printf("%s:", name);
    for(lin = 0; lin < N; lin++)
    {
        printf("\n");
        for(col = 0; col < N; col++)
        {
            printf("%10.4lf", matrix[lin * N + col]);
        }
    }
}

int main(int argc, char* argv[])
{

    srand((int)time(NULL));

    //help message for when program is missused
    char help_message[300];
    sprintf(help_message,
            "Usage:\n"
            "\t%s <N>\n\n"
            "Where:\n"
            "\tN is the amount of equations pseudorandomly generated\n",
            argv[0]);

    //if there is something different than 2 command-line arguments, is missuse
    if( argc != 2)
    {
        printf("%s", help_message);
        exit(0);
    }

    double time = omp_get_wtime();

    //printing how much threads will be used to solve how much equations
#ifdef VERBOSE
    printf("%9s = %s\n%9s = %s\n","equations", argv[1], "threads", argv[2]);
#endif

    //N is the amount of equations
    int N = MAX(2, (int) strtol(argv[1], NULL, 0));
#ifdef EXAMPLE_TEST
    N = 3;
#endif

    //matrix A [N x N] of the linear system
    double* A = (double*) calloc(N * N, sizeof(double));

    //column vector B [N x 1] of the linear system
    double* B = (double*) calloc(N, sizeof(double));

    //manipulated A matrix [N x N]
    double* A_star = (double*) calloc(N * N, sizeof(double));

    //manipulated B column vector [N x 1]
    double* B_star = (double*) calloc(N * N, sizeof(double));

    //current column vector x [N x 1] of the linear system
    double* x = (double*)calloc( N, sizeof(double));

    //next (new) column vector x [N x 1] of the linear system
    double* x_new = (double*)calloc( N, sizeof(double));

    //column vector Dif [N x 1], difference between x and x_new
    double* Dif = (double*)calloc( N, sizeof(double));

    //error of the next x (x_new)
    double mr;

    //counters to walk by A and B
    int lin = 0, col = 0;

    //summation of each line of the A_star matrix
    double* line_sum = (double*)calloc(N, sizeof(double));

    //column vector B that stores A \times x_new
    double* B_new = (double*)calloc(N, sizeof(double));

#ifdef EXAMPLE_TEST
    double test_A[] = { 4,  2,  1,  1,  3,  1,  2,  3,  6};
    double test_B[] = { 7, -8,  6};
#endif

    //initializing A and B with random values
    for(lin = 0; lin < N; lin++)
    {
        line_sum[lin] = 0;

        for(col = 0; col < N; col++)
        {
            A[lin * N + col] = floor(10*((rand() % 1000) / 1000.0));
#ifdef EXAMPLE_TEST
            A[lin * N + col] = test_A[lin * N + col];
#endif

            line_sum[lin] += ((int)(lin != col)) * A[lin * N + col];
        }
        //Convergence is assured if, in a line, the sum of non-diagonal element is less than the diagonal element
        //So, to assure convergence, a random number is sumed to the diagonal element until it is greater than the line non-diagonal elements sum
        while(A[lin * N + lin] <= line_sum[lin])
        {
            A[lin * N + lin] += floor(10*((rand() % 1000) / 1000.0));
        }
        B[lin] = 10 * ((rand() % 1000) / 1000.0);
#ifdef EXAMPLE_TEST
        B[lin] = test_B[lin];
#endif
    }
#ifdef VERBOSE
    //printing A
    printf("\n");
    printMatrix(A, N, "A");
    printf("\n");

    //printing B
    printf("\n");
    printColumnVector(B, N, "B");
    printf("\n");

    //Analysing convergence
    printf("\nConverges? Lets see...\n");
#endif

    for(lin = 0; lin < N; lin++)
    {
        //line_sum[lin] = 0;
        for(col = 0; col < N; col++)
        {
            A_star[lin * N + col] = ((int)(lin != col))* A[lin * N + col] / A[lin * (N + 1)];
            //line_sum[lin] += A_star[lin * N + col];
        }

        B_star[lin] = B[lin] / A[lin * (N + 1)];
        x[lin] = B_star[lin];
    }

#ifdef VERBOSE
    //printing A_star
    printf("\n");
    printMatrix(A_star, N, "A*");
    printf("\n");

    //printing line_sum
    printf("\n");
    printColumnVector(line_sum, N, "line_sum");
    printf("\n");

    //printing B_star
    printf("\n");
    printColumnVector(B_star, N, "B*");
    printf("\n");

    //printing x
    printf("\n");
    printColumnVector(x, N, "x");
    printf("\n");
#endif
    double max_Dif;
    double max_x_new;

    //int k = 0;

    do
    {
        max_Dif = -DBL_MAX;
        max_x_new = -DBL_MAX;

        for(lin = 0; lin < N; lin++)
        {
            x_new[lin] = B_star[lin];
            for(col = 0; col < N; col++)
            {
                x_new[lin] -= A_star[lin * N + col] * x[col];
            }

            Dif[lin] = fabs(x_new[lin] - x[lin]);

            max_Dif = MAX(max_Dif, Dif[lin]);
            max_x_new = MAX(max_x_new, fabs(x_new[lin]));
        }

        mr = max_Dif / max_x_new;

        //printf("\t\tmr = %lf\t\tk = %d\n\n", mr, k);
        //printf("\t\tmr = %lf\n\n", mr);

        //k++;

        memcpy(x, x_new, N * sizeof(double));
        //TODO memcpy aqui??
        /*for (lin = 0; lin < N; ++lin)
        {
            x[lin] = x_new[lin];
        }*/

    }while (mr >= 1e-4);

    time = omp_get_wtime() - time;

#ifdef VERBOSE
    printf("\n");
    printColumnVector(x_new, N, "x_new");
    printf("\n");
#endif

    double B_max_error = -DBL_MAX;

    for (lin = 0; lin < N; ++lin) {
        B_new[lin] = 0;

        for (col = 0; col < N; ++col) {
            B_new[lin] += A[lin * N + col] * x[col];
        }

        B_max_error = MAX(B_max_error, fabs(B_new[lin] - B[lin]));
    }

    printf("N\t%d\t", N); 
    printf("Time\t%10.5lf\t", time);
    printf("B max error: %10.5lf\n", B_max_error);

#ifdef USER_TEST

    char input[256];
    int index;
    double result = 0;
    unsigned char askagain = 1;

    do
    {
        printf("\n\nEscolha uma equacao para avaliar a exatidao do resultado calculado\nInsira um numero inteiro de 0 a %d:\n",
               N - 1);
        if (fgets(input, sizeof(input), stdin))
        {
            if (1 == sscanf(input, "%d", &index))
            {
                if(0 <= index && index < N)
                {
                    askagain = 0;
                    for (col = 0; col < N; col++)
                    {
                        result += A[index * N + col] * x[col];
                    }

                    printf("\nResultado:"
                           "\n\tB[%d] original: %10.5lf"
                           "\n\tRespectivo B calculado com x = %10.5lf"
                           "\n\tDiferenca entre ambos: %10.5lf\n", index, B[index], result, fabs(B[index] - result));
                }
                else
                {
                    askagain = 1;
                }
            }
            else
            {
                askagain = 1;
            }
        }
        else
        {
            askagain = 1;
        }

        if(askagain == 1)
        {
            printf("\n\nPor favor, insira um numero inteiro de 0 a %d\n\n", N-1);
        }
    } while (askagain == 1);
#endif
    free(Dif);
    free(B_new);
    free(line_sum);
    free(A_star);
    free(B_star);
    free(A);
    free(B);
    free(x);
    free(x_new);
}

