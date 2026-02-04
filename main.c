#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include "text_manager.h"
#include "node.h"
#include "tree.h"
#include "error_codes.h"

#define ARGC 2
#define COPY_PATH "data/text_reduced.txt"

#define OUT_FILE1 "data/tree_input.gv"
#define OUT_FILE2 "data/tree_final.gv"
#define TREE_INPUT_NAME "tree_input"
#define TREE_FINAL_NAME "tree_final"

unsigned long int calc_elapsed_time(const struct timespec *beg,
                         const struct timespec *end) {
    return ((end->tv_sec - beg->tv_sec) * 1000 * 1000 * 1000 +
        (end->tv_nsec - beg->tv_nsec));
}

int tree_export(const char *path, node_t root, const char *tree_name)
{
    FILE *f_out = fopen(path, "w");
    if (!f_out)
        return ERR_OPEN;

    fprintf(f_out, "digraph %s {\n", tree_name);
    print_tree(f_out, root);
    fprintf(f_out, "}\n");

    fclose(f_out);

    printf("prefix:\n\t");
    print_tree_prefix(root);

    printf("infix:\n\t");
    print_tree_infix(root);

    printf("postfix:\n\t");
    print_tree_postfix(root);

    return OK;
}

int main(int argc, char **argv)
{
    unsigned long int tree_time, file_time = 0;
    struct timespec start, end;

    FILE *f = NULL, *f_copy = NULL;
    node_t root = NULL;
    int rc = OK;
    char **words = NULL;
    size_t words_n;

    if (argc != ARGC)
        rc = ERR_ARGC;
    if (rc == OK)
    {
        f = fopen(argv[1], "r");
        if (!f)
            rc = ERR_OPEN;
    }

    if (rc == OK)
    {
        char *buf = parse_line(f, &words, &words_n);
        while (buf && rc == OK)
        {
            for (size_t i = 0; rc == OK && i < words_n; i++)
            {
                node_t new_node = create_node(words[i], strlen(words[i]) + 1);
                if (!new_node)
                    rc = ERR_MEM;
                else
                {
                    node_t tmp = insert_node(root, new_node, word_cmp);
                    if (tmp)
                        root = tmp;
                    else
                    {
                        free_node(new_node);
                        rc = ERR_EXISTED_NODE;
                    }
                }
            }
            free(buf);
            buf = parse_line(f, &words, &words_n);
        }
        free(buf);
        free(words);
        words = NULL;
    }
    if (f)
    {
        fclose(f);
        f = NULL;
    }

    if (rc == OK)
        rc = tree_export(OUT_FILE1, root, TREE_INPUT_NAME);

    char l;
    if (rc == OK)
    {
        printf("Введите начальную букву удаляемых слов:\n");
        if (scanf("%s", &l) != 1)
            rc = ERR_IO;
        else
        {
            clock_gettime(CLOCK_MONOTONIC_RAW, &start);
            node_t del = find_in_tree(root, (void*) &l, begins_with);
            while (del)
            {
                root = remove_node(root, del, word_cmp);
                del = find_in_tree(root, (void*) &l, begins_with);
            }
            clock_gettime(CLOCK_MONOTONIC_RAW, &end);
            tree_time = calc_elapsed_time(&start, &end);
        }
    }
    
    if (rc == OK)
        rc = tree_export(OUT_FILE2, root, TREE_FINAL_NAME);
    clear_tree(root);

    if (rc == OK)
    {
        f = fopen(argv[1], "r");
        if (!f)
            rc = ERR_OPEN;
        if (rc == OK)
        {
            f_copy = fopen(COPY_PATH, "w");
            if (!f_copy)
                rc = ERR_OPEN;
        }

        char *buf = parse_line(f, &words, &words_n);
        while (buf && rc == OK)
        {
            clock_gettime(CLOCK_MONOTONIC_RAW, &start);
            rc = print_words_with_filter(f_copy, words, words_n, (int (*)(const char*, const char*)) begins_with, &l);
            clock_gettime(CLOCK_MONOTONIC_RAW, &end);
            file_time += calc_elapsed_time(&start, &end);

            free(buf);
            buf = parse_line(f, &words, &words_n);
        }
        free(buf);
        free(words);

        if (f)
            fclose(f);
        if (f_copy)
            fclose(f_copy);
    }

    if (rc == OK)
        printf("TIME TO DELETE:\n\tWITH TREE: %lu ns\n\tWITH FILE: %lu ns\n", tree_time, file_time);
    else if (rc == ERR_ARGC)
        printf("Неверное число параметров запуска\n");
    else if (rc == ERR_IO)
        printf("Ошибка считывания данных\n");
    else if (rc == ERR_OPEN)
        printf("Ошибка открытия файла\n");
    else if (rc == ERR_MEM)
        printf("Out of memory!\n");
    else if (rc == ERR_EXISTED_NODE)
        printf("Найдено повторяющееся слово\n");

    return rc;
}