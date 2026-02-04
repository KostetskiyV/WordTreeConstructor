#ifndef TEXT_MANAGER_H__
#define TEXT_MANAGER_H__

#include <stddef.h>

int word_cmp(const void *word1, const void *word2);
int begins_with(const void *beg, const void *word);

size_t count_words(const char *line);
void split_words(char *line, char **words);
char *parse_line(FILE *f, char ***words, size_t *words_n);
int print_words_with_filter(FILE *f_out, char **words, size_t words_n, int (*filter)(const char*, const char*), char *param);

#endif
