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

#undef EXAMPLE_TEST
#undef VERBOSE

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

    double time = omp_get_wtime();

    //printing how much threads will be used to solve how much equations
#ifdef VERBOSE
    printf("%9s = %s\n%9s = %s\n","equations", argv[1], "threads", argv[2]);
#endif

    //N is the amount of equations
#ifdef EXAMPLE_TEST
    int N = 3;
#else
    int N = MAX(2, (int) strtol(argv[1], NULL, 0));
#endif

    //T is the amount of threads
    int T = MAX(1, (int) strtol(argv[2], NULL, 0));
    omp_set_num_threads(T);

    //error of the next x (x_new)
    double mr = DBL_MAX;

    //counters to walk by A and B
    int lin = 0, col = 0;

    //summation of each line of the A_star matrix
    double line_sum = 0;
//matrix A [N x N] of the linear system
    double *A;
    //column vector B [N x 1] of the linear system
    double *B;
    //manipulated A matrix [N x N]
    double *A_star;
    //manipulated B column vector [N x 1]
    double *B_star;
    //current column vector x [N x 1] of the linear system
    double *x;
    //next (new) column vector x [N x 1] of the linear system
    double *x_new;
    //column vector Dif [N x 1], difference between x and x_new
    double *Dif;
    //column vector B that stores A \times x_new
    double *B_new;

#pragma omp parallel sections default(none) shared(N, A, B, A_star, B_star, x, x_new, Dif, B_new)
    {
#pragma omp section
        {
            A = (double *) calloc(N * N, sizeof(double));
        }
#pragma omp section
        {
            B = (double *) calloc(N, sizeof(double));
        }
#pragma omp section
        {
            A_star = (double *) calloc(N * N, sizeof(double));
        }
#pragma omp section
        {
            B_star = (double *) calloc(N, sizeof(double));
        }
#pragma omp section
        {
            x = (double *) calloc(N, sizeof(double));
        }
#pragma omp section
        {
            x_new = (double *) calloc(N, sizeof(double));
        }
#pragma omp section
        {
            Dif = (double *) calloc(N, sizeof(double));
        }
#pragma omp section
        {
            B_new = (double *) calloc(N, sizeof(double));
        }
    };

#ifdef EXAMPLE_TEST
    double test_A[] = { 4,  2,  1,  1,  3,  1,  2,  3,  6};
    double test_B[] = { 7, -8,  6};
#endif

#ifdef EXAMPLE_TEST
#pragma omp parallel for schedule(static) firstprivate(line_sum, col) shared(N, A, A_star, B, B_star, x, test_A, test_B) default(none)
#else
#pragma omp parallel for schedule(static) firstprivate(line_sum, col) shared(N, A, A_star, B, B_star, x) default(none)
#endif
    for (lin = 0; lin < N; lin++) {

        for (col = 0; col < N; ++col) {

#ifdef EXAMPLE_TEST
            A[lin * N + col] = test_A[lin * N + col];
#else
            A[lin * N + col] = floor(10 * ((rand() % 1000) / 1000.0));
#endif

            line_sum += ((int) (lin != col)) * A[lin * N + col];
        }

        //Convergence is assured if, in a line, the sum of non-diagonal element is less than the diagonal element
        //So, to assure convergence, a random number is sumed to the diagonal element until it is greater than the line non-diagonal elements sum
        while (A[lin * N + lin] <= line_sum) {
            A[lin * N + lin] += floor(10 * ((rand() % 1000) / 1000.0));
        }

        for (col = 0; col < N; col++) {
            A_star[lin * N + col] = ((int) (lin != col)) * A[lin * N + col] / A[lin * (N + 1)];
        }
#ifdef EXAMPLE_TEST
        B[lin] = test_B[lin];
#else
        B[lin] = 10 * ((rand() % 1000) / 1000.0);
#endif

        B_star[lin] = B[lin] / A[lin * (N + 1)];

        x[lin] = B_star[lin];
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

    double max_Dif;
    double max_x_new;
    int flag = 0;

    do {
        max_Dif = -DBL_MAX;
        max_x_new = -DBL_MAX;
#pragma omp parallel for firstprivate(col) shared(flag, N, A_star, B_star, x, x_new, Dif) default(none) reduction(max:max_Dif, max_x_new) schedule(static)
            for (lin = 0; lin < N; ++lin) {
                x_new[lin] = B_star[lin];
                for (col = 0; col < N; ++col) {
                    x_new[lin] -= A_star[lin * N + col] * x[col];
                }
                Dif[lin] = fabs(x_new[lin] - x[lin]);

                max_Dif = Dif[lin];
                max_x_new = fabs(x_new[lin]);
            }
        
        mr = max_Dif / max_x_new;
#ifdef VERBOSE
        printf("%lf\n", mr);
#endif
#pragma omp parallel for default(none) schedule(static) shared(x, x_new, N)
        for(lin = 0; lin < N; lin++)
        {
            x[lin] = x_new[lin];
        }
    } while (mr >= 1e-4);

    time = omp_get_wtime() - time;

    double B_max_error = 0;

#pragma omp parallel for schedule(static) private(col) shared(B_new, B, N, A, x, Dif) default(none) reduction(max:B_max_error)
    for (lin = 0; lin < N; ++lin) {
        B_new[lin] = 0;

        for (col = 0; col < N; ++col) {
            B_new[lin] += A[lin * N + col] * x[col];
        }

        B_max_error = fabs(B_new[lin] - B[lin]);

#ifdef VERBOSE
        printf("%10.5lf | %10.5lf\n", B[lin], B_new[lin]);
#endif
    }


    printf("N\t%d\t", N);
    printf("T\t%d\t", T);
    printf("Time\t%10.5lf\t", time*1e5);
    printf("B max error\t%10.5lf\t\n", B_max_error);

    free(Dif);
    free(B_new);
    free(A_star);
    free(B_star);
    free(A);
    free(B);
    free(x);
    free(x_new);
}