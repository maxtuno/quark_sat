/*
Copyright (c) 2012-2019 Oscar Riveros. all rights reserved. oscar.riveros@peqnp.science

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <memory.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

struct cpu {
    long n;
    long m;
    long x;
    long y;
    long *X;
    long *Y;
    long *Z;
    long **FF;
};


void decide(struct cpu *cpu) {
    long i;
    for (i = 0; i < cpu->n; i++) {
        if (cpu->X[i] == 0) {
            cpu->X[i] = -(i + 1);
            cpu->x++;
            return;
        }
    }
}

long select(struct cpu *cpu) {
    long i, lit;
    for (i = 0; i < cpu->n; i++) {
        if (cpu->X[i] != 0) {
            lit = cpu->X[i];
            cpu->X[i] = 0;
            cpu->x--;
            return lit;
        }
    }
    return 0;
}

bool backtrack(struct cpu *cpu) {
    long x;
    while (cpu->x != 0) {
        x = select(cpu);
        if (cpu->Y[labs(x) - 1] == x) {
            cpu->Y[labs(x) - 1] = 0;
            cpu->y--;
        } else {
            cpu->X[labs(x) - 1] = -x;
            cpu->x++;
            cpu->Y[labs(x) - 1] = -x;
            cpu->y++;
            return true;
        }
    }
    return false;
}

long exist_conflicts(struct cpu *cpu) {
    long i, j, c;
    for (i = 0; i < cpu->m; i++) {
        c = 0;
        for (j = 0; j < cpu->Z[i]; j++) {
            if (-cpu->FF[i][j] == cpu->X[labs(cpu->FF[i][j]) - 1]) {
                c++;
            } else {
                break;
            }
        }
        if (c == cpu->Z[i]) {
            return true;
        }
    }
    return false;
}

long dpll(struct cpu *cpu) {
    long g = 0;
    for (;;) {
        if (exist_conflicts(cpu) != 0) {
            if (cpu->x == 0 || !backtrack(cpu)) {
                return false;
            }
        } else {
            if (cpu->x > g) {
                g = cpu->x;
                printf("c %li\n", cpu->n - g);
            }
            if (cpu->x == cpu->n) {
                return true;
            } else {
                decide(cpu);
            }
        }
    }
}

int main(int argc, char **argv) {
    long i, j;
    char buffer[32];
    struct cpu cpu;

    if (argc != 2) {
        printf("usage: hess <instance>\n");
        return EXIT_FAILURE;
    }

    FILE *file = fopen(argv[1], "r");
    if (strcmp(buffer, "c") == 0) {
        while (strcmp(buffer, "\n") != 0);
    }
    do {
        fscanf(file, "%s", buffer);
    } while (strcmp(buffer, "p") != 0);
    fscanf(file, " cnf %li %li", &cpu.n, &cpu.m);
    cpu.X = (long *) calloc((size_t) cpu.n, sizeof(long));
    cpu.Y = (long *) calloc((size_t) cpu.n, sizeof(long));
    cpu.Z = (long *) calloc((size_t) cpu.m, sizeof(long));
    cpu.x = 0;
    cpu.y = 0;
    cpu.FF = (long **) calloc((size_t) cpu.m, sizeof(long *));
    for (i = 0; i < cpu.m; i++) {
        j = 0;
        cpu.FF[i] = (long *) calloc((size_t) cpu.n, sizeof(long));
        do {
            fscanf(file, "%s", buffer);
            if (strcmp(buffer, "c") == 0) {
                while (strcmp(buffer, "\n") != 0);
            }
            cpu.FF[i][j++] = atoi(buffer);
        } while (strcmp(buffer, "0") != 0);
        j--;
        cpu.FF[i] = (long *) realloc(cpu.FF[i], j * sizeof(long));
        cpu.Z[i] = j;
    }

    if (dpll(&cpu)) {
        printf("SAT\n");
        for (i = 0; i < cpu.n; i++) {
            if (cpu.X[i] != 0) {
                printf("%li%s", cpu.X[i], (i < cpu.n - 1 ? " " : ""));
            }
        }
        printf(" 0\n");
    } else {
        printf("UNSAT\n");
    }

    return EXIT_SUCCESS;
}