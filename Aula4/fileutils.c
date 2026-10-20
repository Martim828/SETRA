#include fileutils.h
#include <stdio.h>
#include <stdlib.h>
   
int count_lines(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file for reading");
        return;

        int line_count = 0;
        char ch;
        while ((ch = fgetc(file)) != EOF) {
            if (ch == '\n') {
                line_count++;
            }
        }
        return line_count;
    }
    fclose(file);
}

int count_words(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file for reading");
        return -1;

    int word_count = 0;
        char ch;
        while ((ch = fgetc(file)) != EOF) {
            if (ch == ' ' || ch == '\t' || ch == '\n') {
                word_count++;
            }
        }
        return word_count;    
    
    }
    fclose(file);
}

int count_characters(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file for reading");
        return -1;
    }

    int char_count = 0;
    char ch;
    while ((ch = fgetc(file)) != EOF) {
        char_count++;
    }
    fclose(file);
    return char_count;
    
}

void print_file_report(const char *filename) {
    int lines = count_lines(filename);
    int words = count_words(filename);
    int characters = count_characters(filename);

    if (lines == -1 || words == -1 || characters == -1) {
        fprintf(stderr, "Error counting file contents\n");
        return;
    }

    printf("File: %s\n", filename);
    printf("Lines: %d\n", lines);
    printf("Words: %d\n", words);
    printf("Characters: %d\n", characters);
}





    


