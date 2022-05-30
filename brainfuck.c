#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>

typedef enum {
    RUN_OK,
    RUN_FINISHED,
    RUN_ERROR
} Result;

typedef struct {

    char* cells;
    int pointer;
    int ip;

} Tape;


char* readFile(const char* path) {
    FILE* f = fopen(path, "rb");
    if (f == NULL) {
        printf("Could not open file \"%s\"\n", path);
        exit(2);
    }

    fseek(f, 0L, SEEK_END);
    size_t fileSize = ftell(f);
    rewind(f);

    char* buffer = (char*)malloc(fileSize + 1);
    if (buffer == NULL) {
        printf("Not enough memory to read file \"%s\"\n", path);
        exit(2);
    }

    size_t bytesRead = fread(buffer, sizeof(char), fileSize, f);
    if (bytesRead < fileSize) {
        printf("Could not read file \"%s\"\n", path);
        exit(2);
    }

    buffer[bytesRead] = '\0';
    fclose(f);
    return buffer;
}

Result runChar(const char* source, Tape* t);

Result loop(const char* source, Tape* t) {

    int startIndex = t->ip;
    int endIndex = -1;
    int rScope = 0;
    t->ip++;
    //finding the end of the loop, taking into account nested loops
    for(;;) {    
        if (source[t->ip] == '[') {
            rScope++;
        }
        else if (source[t->ip] == ']') {
            if (rScope == 0) {
                endIndex = t->ip;
                break;
            } else {
                rScope--;
            }
        }
        else if (source[t->ip] == '\0') {
            return RUN_ERROR;
        }

        t->ip++;
    }

    if (endIndex == -1) { //idk what case this would happen in, but just to be safe imma do this.
        return RUN_ERROR;
    }

    if (t->cells[t->pointer] == 0) {
        t->ip = endIndex + 1;
        return RUN_OK;
    } else {
        t->ip = startIndex + 1;
        for (;;) {
            Result result = runChar(source, t);

            if (result == RUN_ERROR) {
                return result;
            }

            if (t->ip == endIndex) {
                if (t->cells[t->pointer] == 0) {
                    t->ip += 1;
                    return RUN_OK;
                } else {
                    t->ip = startIndex + 1;
                }
            }
        }
    }
}

Result runChar(const char* source, Tape* t) {

    char i = source[t->ip];
    Result result = RUN_OK;

    switch(i) {
        case '+':
            if (t->cells[t->pointer] == 255) {
                t->cells[t->pointer] = 0;
            } else {
                t->cells[t->pointer]++;
            }
            t->ip++;
            break;
        case '-':
            if (t->cells[t->pointer] == 0) {
                t->cells[t->pointer] = 255;
            } else {
                t->cells[t->pointer]--;
            }
            t->ip++;
            break;
        case '>':
            if (t->pointer == 29999) {
                t->pointer = 0;
            } else {
                t->pointer++;
            }
            t->ip++;
            break;
        case '<':
            if (t->pointer == 0) {
                t->pointer = 29999;
            } else {
                t->pointer--;
            }
            t->ip++;
            break;
        case '.':
            printf("%c", t->cells[t->pointer]);
            t->ip++;
            break;
        case ',':
            char line[4096];
            fgets(line, sizeof(line), stdin);
            t->cells[t->pointer] = line[0];
            t->ip++;
            break;
        case '[':
            result = loop(source, t);
            break;
        case ']':
            result = RUN_ERROR;
            break;
        case '\0':
            result = RUN_FINISHED;
            break;
        default:
            t->ip++;
            break;
    }

    return result;
}

void run(const char* source) {

    Tape t;
    t.pointer = 0;
    t.cells = (char*)calloc(30000, sizeof(char));
    t.ip = 0;

    for (;;) {

        Result result = runChar(source, &t);
        if (result == RUN_FINISHED) {
            return;
        } else if (result == RUN_ERROR) {
            printf("PLACEHOLDER ERROR");
            return;
        }
    }

}


int main(int argc, const char* argv[]) {

    if (argc == 2) {
        char* source = readFile(argv[1]);
        run(source);
        free(source);
    } else {
        printf("Usage: brainfuck [filepath]\n");
    }

    return 0;
}