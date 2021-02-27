#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
struct Matrix {
    char *name;
    int **matrix;
    int row;
    int column;
};
struct Vector {
    char *name;
    int *vector;
    int size;
};

void readCommands(char *filename, char *fileout, char *argument);

void printMatrix(struct Matrix m, int x, int y, FILE *out) ;

void printVector(struct Vector v, int x, FILE *out) ;

void matRead(char *file, int *matrix_count, FILE *out, struct Matrix *matrices);

void vecRead(char *file, int *vector_count, FILE *out, struct Vector *vectors);

void matZeros(FILE *fptr, char *scanbuf, int *matrix_count, FILE *out, struct Matrix *matrices);

void vecZeros(FILE *fptr, char *scanbuf, int *vector_count, FILE *out, struct Vector *vectors);

void vecSlice(FILE *fptr, char *file, int *vector_count, FILE *out, struct Vector *vectors);

void vecStack(FILE *fptr, char *scanbuf, int *matrix_count, int *vector_count, FILE *out, struct Matrix *matrices,
              struct Vector *vectors);

void matSliceCol(FILE *fptr, char *file, int *matrix_count, int *vector_count, FILE *out, struct Matrix *matrices,
                 struct Vector *vectors);

void matSliceRow(FILE *fptr, char *file, int *matrix_count, int *vector_count, FILE *out, struct Matrix *matrices,
                 struct Vector *vectors);

void matSlice(FILE *fptr, char *file, int *matrix_count, FILE *out, struct Matrix *matrices);

void matStack(FILE *fptr, char *file, int *matrix_count, FILE *out, struct Matrix *matrices);

void mvStack(FILE *fptr, char *file, int *matrix_count, int *vector_count, FILE *out, struct Matrix *matrices,
             struct Vector *vectors);

void multiply(FILE *fptr, char *file, int *matrix_count, FILE *out, struct Matrix *matrices);

void add(FILE *fptr, char *file, int *matrix_count, FILE *out, struct Matrix *matrices);

void subtract(FILE *fptr, char *file, int *matrix_count, FILE *out, struct Matrix *matrices);


int main(int argc, char *argv[]) {
    //read commands
    readCommands(argv[2], argv[3],argv[1]);

}

void readCommands(char *filename, char *fileout, char *argument) {
    struct Matrix *matrices;
    struct Vector *vectors;
    //initial allocation
    matrices = malloc(sizeof(struct Matrix));
    vectors = malloc(sizeof(struct Vector));
    int matrix_count = 0, vector_count = 0, m, i;
    FILE *fptr;
    FILE *out;
    int len;
    fptr = fopen(filename, "r");
    out = fopen(fileout, "w");
    fseek(fptr, 0, SEEK_END);
    len = ftell(fptr);
    fseek(fptr, 0, SEEK_SET);
    char *scanbuf = malloc(sizeof(char) * len);
    int control = 0;
    char c;
    while (!feof(fptr)) {
        fscanf(fptr, "%s", scanbuf);
        c = getc(fptr);
        if (!strcmp(scanbuf, "") || !strcmp(scanbuf, "\n")) { //empty line

        } else if (!strcmp(scanbuf, "veczeros")) {
            vecZeros(fptr, scanbuf, &vector_count, out, vectors);
        } else if (!strcmp(scanbuf, "matzeros")) {
            matZeros(fptr, scanbuf, &matrix_count, out, matrices);
        } else if (!strcmp(scanbuf, "vecread")) {
            //vecread
            char *vec_name = NULL;
            vector_count++; // a new matrix, count is increased
            vectors = realloc(vectors, vector_count * sizeof(struct Vector)); // increase the space

            fscanf(fptr, "%s", scanbuf); // get name
            vectors[vector_count - 1].name = malloc(sizeof(strlen(scanbuf))); // allocate  name space
            chdir(argument);
            vecRead(scanbuf, &vector_count, out, vectors);
            vec_name = strtok(scanbuf, ".");
            strcpy(vectors[vector_count - 1].name, vec_name); // put name

        } else if (!strcmp(scanbuf, "matread")) {
            //matread
            char *mat_name = NULL;
            matrix_count++; // a new matrix, count is increased
            matrices = realloc(matrices, matrix_count * sizeof(struct Matrix)); // increase the space

            fscanf(fptr, "%s", scanbuf); // get name
            matrices[matrix_count - 1].name = malloc(sizeof(strlen(scanbuf))); // allocate  name space
            chdir(argument);
            matRead(scanbuf, &matrix_count, out, matrices);
            mat_name = strtok(scanbuf, ".");
            strcpy(matrices[matrix_count - 1].name, mat_name); // put name
        } else if (!strcmp(scanbuf, "multiply")) {
            multiply(fptr, scanbuf, &matrix_count, out, matrices);
        } else if (!strcmp(scanbuf, "add")) {
            add(fptr, scanbuf, &matrix_count, out, matrices);
        } else if (!strcmp(scanbuf, "subtract")) {
            subtract(fptr, scanbuf, &matrix_count, out, matrices);
        } else if (!strcmp(scanbuf, "vecslice")) {
            vecSlice(fptr, scanbuf, &vector_count, out, vectors);
        } else if (!strcmp(scanbuf, "vecstack")) {
            vecStack(fptr, scanbuf, &matrix_count, &vector_count, out, matrices, vectors);
        } else if (!strcmp(scanbuf, "matslicecol")) {
            matSliceCol(fptr, scanbuf, &matrix_count, &vector_count, out, matrices, vectors);
        } else if (!strcmp(scanbuf, "matslicerow")) {
            matSliceRow(fptr, scanbuf, &matrix_count, &vector_count, out, matrices, vectors);
        } else if (!strcmp(scanbuf, "mvstack")) {
            mvStack(fptr, scanbuf, &matrix_count, &vector_count, out, matrices, vectors);
        } else if (!strcmp(scanbuf, "matstack")) {
            matStack(fptr, scanbuf, &matrix_count, out, matrices);
        } else if (!strcmp(scanbuf, "matslice")) {
            matSlice(fptr, scanbuf, &matrix_count, out, matrices);
        } else if(strlen(scanbuf) > 1 && control == 0){
            fprintf(out,"error\n");
            control = 1;
        } if(c == '\n' || c == '\r')
            control = 0;
    }
    free(scanbuf);
    for (m = 0; m < matrix_count; m++) {
        free(matrices[m].name);
        for (i = 0; i < matrices[m].row; i++) {
            free(matrices[m].matrix[i]);
        }
        free(matrices[m].matrix);
    }
    free(matrices);
    for (m = 0; m < vector_count; m++) {
        free(vectors[m].name);
        free(vectors[m].vector);
    }
    free(vectors);
    fclose(fptr);
    fclose(out);
}

void vecZeros(FILE *fptr, char *scanbuf, int *vector_count, FILE *out, struct Vector *vectors) {
    //veczeros
    fprintf(out, "Created vector ");
    (*vector_count)++; // a new vector, count is increased
    vectors = realloc(vectors, *vector_count * sizeof(struct Vector)); // increase the space

    fscanf(fptr, "%s", scanbuf); // get name
    fprintf(out, "%s ", scanbuf);
    vectors[*vector_count - 1].name = malloc(sizeof(strlen(scanbuf))); // allocate  name space
    strcpy(vectors[*vector_count - 1].name, scanbuf); // put name

    fscanf(fptr, "%s", scanbuf); // get length
    fprintf(out, "%s\n", scanbuf);
    int len = atoi(scanbuf); // string to integer
    vectors[*vector_count - 1].size = len;
    vectors[*vector_count - 1].vector = calloc(len, sizeof(int)); // allocate vector space with zeros
    printVector(vectors[*vector_count - 1], len, out);
}

void matZeros(FILE *fptr, char *scanbuf, int *matrix_count, FILE *out, struct Matrix *matrices) {
    int i;
    //matzeros
    fprintf(out, "Created matrix ");
    (*matrix_count)++; // a new matrix, count is increased
    matrices = realloc(matrices, *matrix_count * sizeof(struct Matrix)); // increase the space

    fscanf(fptr, "%s", scanbuf); // get name
    fprintf(out, "%s ", scanbuf);
    matrices[*matrix_count - 1].name = malloc(sizeof(strlen(scanbuf))); // allocate  name space
    strcpy(matrices[*matrix_count - 1].name, scanbuf); // put name

    fscanf(fptr, "%s", scanbuf); // get x
    matrices[*matrix_count - 1].row = atoi(scanbuf);
    fprintf(out, "%s ", scanbuf);
    int lenx = atoi(scanbuf); // string to integer
    matrices[*matrix_count - 1].matrix = calloc(lenx, sizeof(int *)); // allocate matrix x
    fscanf(fptr, "%s", scanbuf); // get y
    matrices[*matrix_count - 1].column = atoi(scanbuf);
    fprintf(out, "%s\n", scanbuf);
    int leny = atoi(scanbuf); // string to integer
    for (i = 0; i < lenx; i++) {
        matrices[*matrix_count - 1].matrix[i] = calloc(leny, sizeof(int)); // allocate matrix columns with zeros
    }
    printMatrix(matrices[*matrix_count - 1], lenx, leny, out);
}

void vecRead(char *file, int *vector_count, FILE *out, struct Vector *vectors) {
    fprintf(out, "read vector %s ", file);
    char ch;
    int size = 0, m;
    FILE *fptr;
    fptr = fopen(file, "r");
    while ((ch = getc(fptr)) != EOF) {
        if (ch == ' ') {
            size++;
        }
    }
    fseek(fptr, 0, SEEK_SET);
    size++;
    vectors[*vector_count - 1].vector = malloc(sizeof(int) * size);
    int l;
    int *scanbuf = malloc(sizeof(int) * size);
    for (l = 0; l < size; l++) {
        fscanf(fptr, "%d", &scanbuf[l]);
    }
    for (m = 0; m < size; m++) {
        vectors[*vector_count - 1].vector[m] = scanbuf[m];
    }
    vectors[*vector_count - 1].size = size;
    fprintf(out, "%d\n", vectors[*vector_count - 1].size);
    printVector(vectors[*vector_count - 1], vectors[*vector_count - 1].size, out);
    free(scanbuf);
    fclose(fptr);
}

void matRead(char *file, int *matrix_count, FILE *out, struct Matrix *matrices) {
    fprintf(out, "read matrix %s ", file);
    int row_number = 0, column_number = 0, m, x, y, count = 0;
    char ch;
    FILE *fptr;
    fptr = fopen(file, "r");
    while ((ch = getc(fptr)) != EOF) {
        if (ch == '\n') {
            row_number++;
        }
        if (ch == ' ') {
            column_number++;
        }
    }
    fseek(fptr, 0, SEEK_SET);
    column_number = (column_number / (row_number + 1)) + 1;
    int *scanbuf = malloc(sizeof(int) * ((row_number + 1) * column_number));
    int l;
    for (l = 0; l < ((row_number + 1) * column_number); l++) {
        fscanf(fptr, "%d", &scanbuf[l]);
    }
    matrices[*matrix_count - 1].matrix = malloc(sizeof(int *) * (row_number + 1));
    for (m = 0; m < (row_number + 1); m++) {
        matrices[*matrix_count - 1].matrix[m] = malloc(sizeof(int) * column_number);
    }
    for (x = 0; x < (row_number + 1); x++) {
        for (y = 0; y < column_number; y++) {
            matrices[*matrix_count - 1].matrix[x][y] = scanbuf[count];
            count++;
        }
    }

    matrices[*matrix_count - 1].row = row_number + 1;
    matrices[*matrix_count - 1].column = column_number;
    fprintf(out, "%d %d\n", matrices[*matrix_count - 1].row, matrices[*matrix_count - 1].column);
    printMatrix(matrices[*matrix_count - 1], matrices[*matrix_count - 1].row, matrices[*matrix_count - 1].column, out);
    free(scanbuf);
    fclose(fptr);
}

void multiply(FILE *fptr, char *file, int *matrix_count, FILE *out, struct Matrix *matrices) {
    int first_matrix = 0, second_matrix = 0, k, m, i, x;
    fscanf(fptr, "%s", file); // get name of the first matrix
    for (k = 0; k < *matrix_count; k++) {
        if (strcmp(matrices[k].name, file) == 0) {
            first_matrix = k;
        }
    }
    fscanf(fptr, "%s", file); // get name of the second matrix
    for (m = 0; m < *matrix_count; m++) {
        if (strcmp(matrices[m].name, file) == 0) {
            second_matrix = m;
        }
    }
    if (matrices[first_matrix].row != matrices[second_matrix].row ||
        matrices[first_matrix].column != matrices[second_matrix].column)
        fprintf(out, "error\n");
    else {
        for (i = 0; i < matrices[first_matrix].row; i++) {
            for (x = 0; x < matrices[first_matrix].column; x++) {
                matrices[first_matrix].matrix[i][x] =
                        matrices[first_matrix].matrix[i][x] * matrices[second_matrix].matrix[i][x];
            }
        }

        fprintf(out, "multiply %s %s\n", matrices[first_matrix].name, matrices[second_matrix].name);
        printMatrix(matrices[first_matrix], matrices[first_matrix].row, matrices[first_matrix].column, out);
    }
}

void add(FILE *fptr, char *file, int *matrix_count, FILE *out, struct Matrix *matrices) {
    int first_matrix = 0, second_matrix = 0, k, m, i, x;
    fscanf(fptr, "%s", file); // get name of the first matrix
    for (k = 0; k < *matrix_count; k++) {
        if (strcmp(matrices[k].name, file) == 0) {
            first_matrix = k;
        }
    }
    fscanf(fptr, "%s", file); // get name of the second matrix
    for (m = 0; m < *matrix_count; m++) {
        if (strcmp(matrices[m].name, file) == 0) {
            second_matrix = m;
        }
    }
    if (matrices[first_matrix].row != matrices[second_matrix].row ||
        matrices[first_matrix].column != matrices[second_matrix].column)
        fprintf(out, "error\n");
    else {
        for (i = 0; i < matrices[first_matrix].row; i++) {
            for (x = 0; x < matrices[first_matrix].column; x++) {
                matrices[first_matrix].matrix[i][x] =
                        matrices[first_matrix].matrix[i][x] + matrices[second_matrix].matrix[i][x];
            }
        }

        fprintf(out, "add %s %s\n", matrices[first_matrix].name, matrices[second_matrix].name);
        printMatrix(matrices[first_matrix], matrices[first_matrix].row, matrices[first_matrix].column, out);
    }
}

void subtract(FILE *fptr, char *file, int *matrix_count, FILE *out, struct Matrix *matrices) {
    int first_matrix = 0, second_matrix = 0, k, m, i, x;
    fscanf(fptr, "%s", file); // get name of the first matrix
    for (k = 0; k < *matrix_count; k++) {
        if (strcmp(matrices[k].name, file) == 0) {
            first_matrix = k;
        }
    }
    fscanf(fptr, "%s", file); // get name of the second matrix
    for (m = 0; m < *matrix_count; m++) {
        if (strcmp(matrices[m].name, file) == 0) {
            second_matrix = m;
        }
    }
    if (matrices[first_matrix].row != matrices[second_matrix].row ||
        matrices[first_matrix].column != matrices[second_matrix].column)
        fprintf(out, "error\n");
    else {
        for (i = 0; i < matrices[first_matrix].row; i++) {
            for (x = 0; x < matrices[first_matrix].column; x++) {
                matrices[first_matrix].matrix[i][x] =
                        matrices[first_matrix].matrix[i][x] - matrices[second_matrix].matrix[i][x];
            }
        }

        fprintf(out, "subtract %s %s\n", matrices[first_matrix].name, matrices[second_matrix].name);
        printMatrix(matrices[first_matrix], matrices[first_matrix].row, matrices[first_matrix].column, out);
    }
}

void vecSlice(FILE *fptr, char *file, int *vector_count, FILE *out, struct Vector *vectors) {
    int first_index = 0, second_index = 0;
    int k = 0, i, m;
    fscanf(fptr, "%s", file);
    for (i = 0; i < *vector_count; i++) {
        if (strcmp(vectors[i].name, file) == 0) {
            (*vector_count)++;
            vectors = realloc(vectors, *vector_count * sizeof(struct Vector)); // increase the space
            vectors[*vector_count - 1].vector = malloc(sizeof(int));
            fscanf(fptr, "%s", file);
            first_index = atoi(file);
            fscanf(fptr, "%s", file);
            second_index = atoi(file);
            fscanf(fptr, "%s", file);
            vectors[*vector_count - 1].name = malloc(sizeof(strlen(file))); // allocate  name space
            strcpy(vectors[*vector_count - 1].name, file); // put name
            for (m = first_index; m < second_index; m++) {
                vectors[*vector_count - 1].vector = realloc(vectors[*vector_count - 1].vector, (k + 1) * sizeof(int));
                vectors[*vector_count - 1].vector[k] = vectors[i].vector[m];
                k++;
            }
            break;
        }
    }
    vectors[*vector_count - 1].size = second_index - first_index;
    fprintf(out, "vector sliced %s %d\n", vectors[*vector_count - 1].name, vectors[*vector_count - 1].size);
    printVector(vectors[*vector_count - 1], vectors[*vector_count - 1].size, out);
}

void vecStack(FILE *fptr, char *file, int *matrix_count, int *vector_count, FILE *out, struct Matrix *matrices,
              struct Vector *vectors) {
    int first_vec = 0, second_vec = 0, i, m, k;
    fscanf(fptr, "%s", file);
    for (i = 0; i < *vector_count; i++) {
        if (strcmp(vectors[i].name, file) == 0) {
            first_vec = i;
        }
    }
    fscanf(fptr, "%s", file);
    for (m = 0; m < *vector_count; m++) {
        if (strcmp(vectors[m].name, file) == 0) {
            second_vec = m;
        }
    }
    if (vectors[first_vec].size != vectors[second_vec].size) {
        fprintf(out, "error\n");
    } else {
        fscanf(fptr, "%s", file);
        if (strcmp(file, "row") == 0) {
            (*matrix_count)++;
            matrices = realloc(matrices, *matrix_count * sizeof(struct Matrix));
            matrices[*matrix_count - 1].matrix = malloc(sizeof(int *));
            matrices[*matrix_count - 1].matrix[0] = malloc(sizeof(int) * vectors[first_vec].size);
            fscanf(fptr, "%s", file);
            matrices[*matrix_count - 1].name = malloc(sizeof(strlen(file))); // allocate  name space
            strcpy(matrices[*matrix_count - 1].name, file);
            matrices[*matrix_count - 1].row = 2;
            matrices[*matrix_count - 1].column = vectors[first_vec].size;
            for (k = 0; k < vectors[first_vec].size; k++) {
                matrices[*matrix_count - 1].matrix[0][k] = vectors[first_vec].vector[k];
            }
            matrices[*matrix_count - 1].matrix[1] = malloc(sizeof(int) * vectors[first_vec].size);
            for (k = 0; k < vectors[first_vec].size; k++) {
                matrices[*matrix_count - 1].matrix[1][k] = vectors[second_vec].vector[k];
            }
            fprintf(out, "vectors concatenated %s %d %d\n", matrices[*matrix_count - 1].name,
                    matrices[*matrix_count - 1].row,
                    matrices[*matrix_count - 1].column);
            printMatrix(matrices[*matrix_count - 1], matrices[*matrix_count - 1].row,
                        matrices[*matrix_count - 1].column,
                        out);
        }
        if (strcmp(file, "column") == 0) {
            (*matrix_count)++;
            fscanf(fptr, "%s", file);
            matrices[*matrix_count - 1].name = malloc(sizeof(strlen(file))); // allocate  name space
            strcpy(matrices[*matrix_count - 1].name, file);
            matrices[*matrix_count - 1].row = vectors[first_vec].size;
            matrices[*matrix_count - 1].column = 2;
            matrices[*matrix_count - 1].matrix = malloc(matrices[*matrix_count - 1].row * sizeof(int *));


            for (k = 0; k < vectors[first_vec].size; k++) {
                matrices[*matrix_count - 1].matrix[k] = malloc(2 * sizeof(int));
            }
            for (k = 0; k < vectors[first_vec].size; k++) {
                matrices[*matrix_count - 1].matrix[k][0] = vectors[first_vec].vector[k];
            }
            for (m = 0; m < vectors[first_vec].size; m++) {
                matrices[*matrix_count - 1].matrix[m][1] = vectors[second_vec].vector[m];
            }
            fprintf(out, "vectors concatenated %s %d %d\n", matrices[*matrix_count - 1].name,
                    matrices[*matrix_count - 1].row,
                    matrices[*matrix_count - 1].column);
            printMatrix(matrices[*matrix_count - 1], matrices[*matrix_count - 1].row,
                        matrices[*matrix_count - 1].column,
                        out);
        }
    }

}

void matSliceCol(FILE *fptr, char *file, int *matrix_count, int *vector_count, FILE *out, struct Matrix *matrices,
                 struct Vector *vectors) {
    fscanf(fptr, "%s", file);
    int k = 0, mat_name = 0, start_val = 0, stop_val = 0, column = 0, m, j;
    for (m = 0; m < *matrix_count; m++) {
        if (strcmp(matrices[m].name, file) == 0) {
            mat_name = m;
        }
    }
    fscanf(fptr, "%s", file);
    column = atoi(file);
    fscanf(fptr, "%s", file);
    start_val = atoi(file);
    fscanf(fptr, "%s", file);
    stop_val = atoi(file);
    fscanf(fptr, "%s", file);
    if (column > matrices[mat_name].column || start_val >= matrices[mat_name].row ||
        stop_val >= matrices[mat_name].row) {
        fprintf(out, "error\n");
    } else {
        (*vector_count)++;
        vectors = realloc(vectors, *vector_count * sizeof(struct Vector)); // increase the space
        vectors[*vector_count - 1].vector = malloc(sizeof(int));
        vectors[*vector_count - 1].name = malloc(sizeof(strlen(file))); // allocate  name space
        strcpy(vectors[*vector_count - 1].name, file); // put name
        vectors[*vector_count - 1].size = (stop_val - start_val);
        for (j = 0; j < (stop_val - start_val); j++) {
            vectors[*vector_count - 1].vector = realloc(vectors[*vector_count - 1].vector, (k + 1) * sizeof(int));
            vectors[*vector_count - 1].vector[j] = matrices[mat_name].matrix[j][column - 1];
            k++;
        }
        fprintf(out, "vector sliced %s %d\n", vectors[*vector_count - 1].name, vectors[*vector_count - 1].size);
        printVector(vectors[*vector_count - 1], vectors[*vector_count - 1].size, out);
    }
}

void matSliceRow(FILE *fptr, char *file, int *matrix_count, int *vector_count, FILE *out, struct Matrix *matrices,
                 struct Vector *vectors) {
    fscanf(fptr, "%s", file);
    int k = 0, mat_name = 0, start_val = 0, stop_val = 0, row = 0, m, j;
    for (m = 0; m < *matrix_count; m++) {
        if (strcmp(matrices[m].name, file) == 0) {
            mat_name = m;
        }
    }
    fscanf(fptr, "%s", file);
    row = atoi(file);
    fscanf(fptr, "%s", file);
    start_val = atoi(file);
    fscanf(fptr, "%s", file);
    stop_val = atoi(file);
    fscanf(fptr, "%s", file);
    if (row > matrices[mat_name].row || start_val >= matrices[mat_name].column ||
        stop_val >= matrices[mat_name].column) {
        fprintf(out, "error\n");
    } else {
        (*vector_count)++;
        vectors = realloc(vectors, *vector_count * sizeof(struct Vector)); // increase the space
        vectors[*vector_count - 1].name = malloc(sizeof(strlen(file))); // allocate  name space
        strcpy(vectors[*vector_count - 1].name, file); // put name
        vectors[*vector_count - 1].size = (stop_val - start_val);
        vectors[*vector_count - 1].vector = malloc(vectors[*vector_count - 1].size * sizeof(int));
        for (j = start_val; j < stop_val; j++) {
            vectors[*vector_count - 1].vector[k] = matrices[mat_name].matrix[row][j];
            k++;
        }
        fprintf(out, "vector sliced %s %d\n", vectors[*vector_count - 1].name, vectors[*vector_count - 1].size);
        printVector(vectors[*vector_count - 1], vectors[*vector_count - 1].size, out);
    }
}

void mvStack(FILE *fptr, char *file, int *matrix_count, int *vector_count, FILE *out, struct Matrix *matrices,
             struct Vector *vectors) {
    fscanf(fptr, "%s", file);
    int mat_name = 0, vec_name = 0, x = 0, y = 0, m, j;
    for (m = 0; m < *matrix_count; m++) {
        if (strcmp(matrices[m].name, file) == 0) {
            mat_name = m;
        }
    }
    fscanf(fptr, "%s", file);
    for (j = 0; j < *vector_count; j++) {
        if (strcmp(vectors[j].name, file) == 0) {
            vec_name = j;
        }
    }
    fscanf(fptr, "%s", file);
    if (strcmp(file, "d") == 0) {
        if (matrices[mat_name].column == vectors[vec_name].size) {
            matrices[mat_name].row = matrices[mat_name].row + 1;
            matrices[mat_name].matrix = realloc(matrices[mat_name].matrix, sizeof(int *) * (matrices[mat_name].row));
            matrices[mat_name].matrix[matrices[mat_name].row - 1] = malloc(sizeof(int) * vectors[vec_name].size);
            for (x = 0; x < vectors[vec_name].size; x++) {
                matrices[mat_name].matrix[matrices[mat_name].row - 1][x] = vectors[vec_name].vector[x];
            }
            fprintf(out, "matrix and vector concatenated %s %d %d\n", matrices[mat_name].name, matrices[mat_name].row,
                    matrices[mat_name].column);
            printMatrix(matrices[mat_name], matrices[mat_name].row, matrices[mat_name].column, out);
        } else {
            fprintf(out, "error\n");
        }
    } else if (strcmp(file, "r") == 0) {
        if (matrices[mat_name].row == vectors[vec_name].size) {
            for (x = 0; x < matrices[mat_name].row; x++) {
                matrices[mat_name].matrix[x] = realloc(matrices[mat_name].matrix[x], matrices[mat_name].column + 1);
            }
            for (y = 0; y < vectors[vec_name].size; y++) {
                matrices[mat_name].matrix[y][matrices[mat_name].column] = vectors[vec_name].vector[y];
            }
            matrices[mat_name].column = matrices[mat_name].column + 1;
            fprintf(out, "matrix and vector concatenated %s %d %d\n", matrices[mat_name].name, matrices[mat_name].row,
                    matrices[mat_name].column);
            printMatrix(matrices[mat_name], matrices[mat_name].row, matrices[mat_name].column, out);
        }
    }
}

void matSlice(FILE *fptr, char *file, int *matrix_count, FILE *out, struct Matrix *matrices) {
    fscanf(fptr, "%s", file);
    int k = 0, m, j, i, x = 0, y = 0, mat1_name = 0, start_row = 0, stop_row = 0, start_col = 0, stop_col = 0;
    for (m = 0; m < *matrix_count; m++) {
        if (strcmp(matrices[m].name, file) == 0) {
            mat1_name = m;
        }
    }
    fscanf(fptr, "%s", file);
    start_col = atoi(file);
    fscanf(fptr, "%s", file);
    stop_col = atoi(file);
    fscanf(fptr, "%s", file);
    start_row = atoi(file);
    fscanf(fptr, "%s", file);
    stop_row = atoi(file);
    (*matrix_count)++;
    matrices = realloc(matrices, *matrix_count * sizeof(struct Matrix));
    matrices[*matrix_count - 1].matrix = (int **) malloc(sizeof(int *) * (stop_row - start_row));
    fscanf(fptr, "%s", file);
    matrices[*matrix_count - 1].name = malloc(sizeof(strlen(file)) * sizeof(char)); // allocate  name space
    strcpy(matrices[*matrix_count - 1].name, file);
    matrices[*matrix_count - 1].row = stop_row - start_row;
    matrices[*matrix_count - 1].column = stop_col - start_col;
    for (k = 0; k < matrices[*matrix_count - 1].row; k++) {
        matrices[*matrix_count - 1].matrix[k] = malloc(matrices[*matrix_count - 1].column * sizeof(int));
    }
    for (j = start_row; j < stop_row; j++) {
        for (i = start_col; i < stop_col; i++) {
            matrices[*matrix_count - 1].matrix[x][y] = matrices[mat1_name].matrix[j][i];
            y++;
        }
        y = 0;
        x++;
    }
    fprintf(out, "matrix sliced %s %d %d\n", matrices[*matrix_count - 1].name, matrices[*matrix_count - 1].row,
            matrices[*matrix_count - 1].column);
    printMatrix(matrices[*matrix_count - 1], matrices[*matrix_count - 1].row, matrices[*matrix_count - 1].column,
                out);

}

void matStack(FILE *fptr, char *file, int *matrix_count, FILE *out, struct Matrix *matrices) {
    fscanf(fptr, "%s", file);
    int mat1_name = 0, mat2_name = 0, x = 0, y = 0, m, j, a = 0;
    for (m = 0; m < *matrix_count; m++) {
        if (strcmp(matrices[m].name, file) == 0) {
            mat1_name = m;
        }
    }
    fscanf(fptr, "%s", file);
    for (j = 0; j < *matrix_count; j++) {
        if (strcmp(matrices[j].name, file) == 0) {
            mat2_name = j;
        }
    }
    fscanf(fptr, "%s", file);
    if (strcmp(file, "d") == 0) {
        if (matrices[mat1_name].column == matrices[mat2_name].column) {
            matrices[mat1_name].matrix = realloc(matrices[mat1_name].matrix,
                                                 sizeof(int *) * (matrices[mat2_name].row + matrices[mat1_name].row));
            int k;
            for (k = 0; k < matrices[mat2_name].row; k++) {
                matrices[mat1_name].matrix[matrices[mat1_name].row + k] = malloc(
                        sizeof(int) * matrices[mat1_name].column);
            }
            for (x = (matrices[mat1_name].row); x < (matrices[mat2_name].row + matrices[mat1_name].row); x++) {
                for (y = 0; y < matrices[mat2_name].column; y++) {
                    matrices[mat1_name].matrix[x][y] = matrices[mat2_name].matrix[a][y];
                }
                a++;
            }
            matrices[mat1_name].column = matrices[mat1_name].column + matrices[mat2_name].column;
            fprintf(out, "matrices concatenated %s %d %d\n", matrices[mat1_name].name, matrices[mat1_name].row,
                    matrices[mat1_name].column);
            printMatrix(matrices[mat1_name], matrices[mat1_name].row, matrices[mat1_name].column, out);
        } else {
            fprintf(out, "error\n");
        }
    } else if (strcmp(file, "r") == 0) {
        if (matrices[mat1_name].row == matrices[mat2_name].row) {
            int k, f, l, q = 0;
            for (k = 0; k < matrices[mat1_name].row; k++) {
                matrices[mat1_name].matrix[k] = realloc(matrices[mat1_name].matrix[k],
                                                        sizeof(int) *
                                                        (matrices[mat1_name].column + matrices[mat2_name].column));
            }
            for (f = 0; f < matrices[mat1_name].row; f++) {
                for (l = (matrices[mat1_name].column);
                     l < (matrices[mat2_name].column + matrices[mat1_name].column); l++) {
                    matrices[mat1_name].matrix[f][l] = matrices[mat2_name].matrix[f][q];
                    q++;
                }
                q = 0;
            }
            matrices[mat1_name].column = matrices[mat1_name].column + matrices[mat2_name].column;
            fprintf(out, "matrices concatenated %s %d %d\n", matrices[mat1_name].name, matrices[mat1_name].row,
                    matrices[mat1_name].column);
            printMatrix(matrices[mat1_name], matrices[mat1_name].row, matrices[mat1_name].column, out);
        } else {
            fprintf(out, "error\n");
        }
    }
}

void printMatrix(struct Matrix m, int x, int y, FILE *out) {
    int i, j;
    for (i = 0; i < x; i++) {
        for (j = 0; j < y; j++) {
            fprintf(out, "%3d ", m.matrix[i][j]);
        }
        fprintf(out, "\n");
    }
}

void printVector(struct Vector v, int x, FILE *out) {
    int i = 0;
    for (i = 0; i < x; i++) {
        fprintf(out, "%3d ", v.vector[i]);
    }
    fprintf(out, "\n");
}


