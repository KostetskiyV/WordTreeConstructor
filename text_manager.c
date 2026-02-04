#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "text_manager.h"
#include "error_codes.h"

int word_cmp(const void *word1, const void *word2)
{
    const char *l = (const char*) word1;
    const char *r = (const char*) word2;
    return strcmp(l, r);
}

int begins_with(const void *beg, const void *word)
{
    const char l = *((const char*) beg);
    const char *r = (const char*) word;
    return r[0] - l;
}

size_t count_words(const char *line)
{
    size_t words = 0, letters = 0;
    for (size_t i = 0; line[i] != '\0'; i++)
    {
        if (line[i] == ' ' && letters > 0)
        {
            words++;
            letters = 0;
        }
        else
            letters++;
    }
    return words + (int) (letters > 0);
}

void split_words(char *line, char **words)
{
    size_t cur = 0, letters = 0, i;
    for (i = 0; line[i] != '\0'; i++)
    {
        if (line[i] == ' ' && letters > 0)
        {
            words[cur++] = &(line[i - letters]);
            line[i] = '\0';
            letters = 0;
        }
        else
            letters++;
    }
    if (letters > 0)
        words[cur] = &(line[i - letters]);
}

char *parse_line(FILE *f, char ***words, size_t *words_n)
{
    char *buf = NULL;
	size_t buf_len = 0;
	ssize_t read;
    read = getline(&buf, &buf_len, f);
    if (read == -1)
    {
        free(buf);
        return NULL;
    }

    buf[read-1] = '\0';
    *words_n = count_words(buf);

    char **tmp = realloc(*words, *words_n * sizeof(char*));
    if (tmp)
        *words = tmp;
    else
    {
        free(buf);
        return NULL;
    }

    split_words(buf, *words);

	return buf;
}

int print_words_with_filter(FILE *f_out, char **words, size_t words_n, int (*filter)(const char*, const char*), char *param)
{
    int rc = OK;
    for (size_t i = 0; i < words_n && rc == OK; i++)
    {
        if (filter(param, words[i]) != 0)
        {
            if (fprintf(f_out, "%s ", words[i]) < 0)
                rc = ERR_IO;
        }
    }
    if (rc == OK && fprintf(f_out, "\n") >= 0)
        return OK;
    return ERR_IO;
}
