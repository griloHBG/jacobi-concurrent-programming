//
// Created by grilo on 06/10/2019.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[])
{
    double* A;
    double* B;

    char help_message[300];
    sprintf(help_message,
            "Usage:\n"
            "\t%s <N> <T>\n\n"
            "Where:\n"
            "\tN is the amount of equations pseudorandomly generated\n"
            "\tT is the amount of POSIX threads that will be used\n",
            argv[0]);

    if( argc != 3)
    {
        printf("%s", help_message);
        exit(0);
    }

    printf("%9s = %s\n%9s = %s","equations", argv[1], "threads", argv[2]);

    int N = (int) strtol(argv[1], NULL, 0);
    int T = (int) strtol(argv[2], NULL, 0);

    A = (double*) calloc(N * N, sizeof(double));
    B = (double*) calloc(N, sizeof(double));

    int lin = 0, col = 0;

    for(lin = 0; lin < N; lin++)
    {
        for(col = 0; col < N; col++)
        {
            A[lin * N + col] = lin + col;
        }
        B[lin] = lin;
    }

    printf("\nA:\n");

    for(lin = 0; lin < N; lin++)
    {
        for(col = 0; col < N; col++)
        {
            printf("%5.2lf", A[lin * N + col]);
        }
        printf("\n");
    }

    printf("\nB:\n");

    for(lin = 0; lin < N; lin++)
    {
        printf("%5.2lf", B[lin]);
        printf("\n");
    }

    free(A);
    free(B);
}

