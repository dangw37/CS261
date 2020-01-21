#include "hashMap.h"
#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * Allocates a string for the next word in the file and returns it. This string
 * is null terminated. Returns NULL after reaching the end of the file.
 * @param file
 * @return Allocated string or NULL.
 */
char* nextWord(FILE* file)
{
    int maxLength = 16;
    int length = 0;
    char* word = malloc(sizeof(char) * maxLength);
    while (1)
    {
        char c = fgetc(file);
        if ((c >= '0' && c <= '9') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= 'a' && c <= 'z') ||
            c == '\'')
        {
            if (length + 1 >= maxLength)
            {
                maxLength *= 2;
                word = realloc(word, maxLength);
            }
            word[length] = c;
            length++;
        }
        else if (length > 0 || c == EOF)
        {
            break;
        }
    }
    if (length == 0)
    {
        free(word);
        return NULL;
    }
    word[length] = '\0';
    return word;
}

// Adapted from Levenshtein distance implementation: https://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/Levenshtein_distance#C
// Description: calculates Levenshtein distance and returns distance as int
int levenshtein(const char* s, const char* t)
{
    int ls = strlen(s), lt = strlen(t);
    int d[ls + 1][lt + 1];

    for (int i = 0; i <= ls; i++)
        for (int j = 0; j <= lt; j++)
            d[i][j] = -1;

    int dist(int i, int j)
    {
        if (d[i][j] >= 0) return d[i][j];

        int x;
        if (i == ls)
            x = lt - j;
        else if (j == lt)
            x = ls - i;
        else if (s[i] == t[j])
            x = dist(i + 1, j + 1);
        else {
            x = dist(i + 1, j + 1);

            int y;
            if ((y = dist(i, j + 1)) < x) x = y;
            if ((y = dist(i + 1, j)) < x) x = y;
            x++;
        }
        return d[i][j] = x;
    }
    return dist(0, 0);
}

/**
 * Loads the contents of the file into the hash map.
 * @param file
 * @param map
 */
void loadDictionary(FILE* file, HashMap* map)
{
    // FIXME: implement
    assert(file != NULL);
    assert(map != NULL);
    char* word = nextWord(file);

    while (word) 
    {
        hashMapPut(map, word, 1);
        free(word);
        word = nextWord(file);
    }
    free(word);
}

/**
 * Checks the spelling of the word provded by the user. If the word is spelled incorrectly,
 * print the 5 closest words as determined by a metric like the Levenshtein distance.
 * Otherwise, indicate that the provded word is spelled correctly. Use dictionary.txt to
 * create the dictionary.
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, const char** argv)
{
    // FIXME: implement
    HashMap* map = hashMapNew(1000);

    FILE* file = fopen("dictionary.txt", "r");
    clock_t timer = clock();
    loadDictionary(file, map);
    timer = clock() - timer;
    printf("Dictionary loaded in %f seconds\n", (float)timer / (float)CLOCKS_PER_SEC);
    fclose(file);

    char inputBuffer[256];
    int quit = 0;
    while (!quit)
    {
        printf("Enter a word or \"quit\" to quit: ");
        scanf("%s", inputBuffer);

        // Implement the spell checker code here..
        for (int i = 0; i < strlen(inputBuffer); i++)
        {
            inputBuffer[i] = tolower(inputBuffer[i]);
        }

        if (hashMapContainsKey(map, inputBuffer))
        {
            if (strcmp(inputBuffer, "quit") == 0)
            {

            }
            else
            {
                printf("%s is spelled correctly\n", inputBuffer);
            }
        }

        else
        {
            printf("%s is spelled incorrectly\n Did you mean?\n", inputBuffer);

            int count = 0;
            int i = 0;
            HashLink* cur;

            for (i = 0; i < map->capacity; i++)
            {
                cur = map->table[i];

                while (cur)
                {
                    hashMapPut(map, map->table[i]->key, levenshtein(inputBuffer, cur->key));

                    if (count > 5)
                    {
                        break;
                    }
                    else if (levenshtein(inputBuffer, cur->key) == 1 && count < 5)
                    {
                        count++;
                        printf("%s\n", cur->key);
                    }
                    cur = cur->next;
                }
            }

            for (i = 0; i < map->capacity; i++)
            {
                cur = map->table[i];

                while (cur)
                {
                    hashMapPut(map, map->table[i]->key, levenshtein(inputBuffer, cur->key));

                    if (count > 5)
                    {
                        break;
                    }
                    else if (levenshtein(inputBuffer, cur->key) == 2 && count < 5)
                    {
                        count++;
                        printf("%s\n", cur->key);
                    }
                    cur = cur->next;
                }
            }

            for (i = 0; i < map->capacity; i++)
            {
                cur = map->table[i];

                while (cur)
                {
                    hashMapPut(map, map->table[i]->key, levenshtein(inputBuffer, cur->key));

                    if (count > 5)
                    {
                        break;
                    }
                    else if (levenshtein(inputBuffer, cur->key) == 3 && count < 5)
                    {
                        count++;
                        printf("%s\n", cur->key);
                    }
                    cur = cur->next;
                }
            }

        }

        if (strcmp(inputBuffer, "quit") == 0)
        {
            quit = 1;
        }
    }

    hashMapDelete(map);
    return 0;
}


