#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <getopt.h>

#include "mylib.h"
#include "tree.h"

static void print_info(int freq, char *word);
void print_help();

/**
 * The main method of the program. Creates a tree structure and uses
 * command line arguements to determine functionality from there.
 * Can be used as a spell checker if a dictionary text file is passed to it,
 * otherwise an elaborate way to store and organise strings with efficient
 * insert, delete, and search complexity.
 * 
 * @param argc - the number of command line arguments passed to the program.
 * @param argv - array to point to each argument passed to the program.
 * @return - integer value. 0 if program exits successfully.
 */
int main(int argc, char **argv)
{
    const char *optstring = "c:df:orh";
    char option;
    char *dot_output_file_name = NULL;
    char *dictionary_file_name = NULL;
    char read_in_word[256];
    int print_dot = 0;
    int tree_depth_check = 0;
    int unknown_words = 0;
    tree t;
    tree_t type = BST;
    FILE *check = NULL;
    FILE *dot_output_file = NULL;
    clock_t start_fill, end_fill, start_search, end_search;

    while ((option = getopt(argc, argv, optstring)) != EOF)
    {
        switch (option)
        {
        case 'c':
            dictionary_file_name = argv[2];
            break;

        case 'd':
            tree_depth_check = 1;
            break;

        case 'f':
            dot_output_file = (fopen(optarg, "w"));
            dot_output_file_name = argv[2];
            break;

        case 'o':
            print_dot = 1;
            break;

        case 'r':
            type = RBT;
            break;

        case 'h':
        default:
            print_help();
            return EXIT_SUCCESS;
        }
    }
    /* Construct and fill tree structure. */
    t = tree_new(type);
    start_fill = clock();
    while (getword(read_in_word, sizeof read_in_word, stdin) != EOF)
    {
        t = tree_insert(t, read_in_word);
        t = root_check(t);
    }
    end_fill = clock();

    /* Run additional operations based on command line input. */
    if (dictionary_file_name != NULL)
    {
        if (NULL == (check = fopen(dictionary_file_name, "r")))
        {
            fprintf(stderr, "Can't open file '%s' using mode r.\n", dictionary_file_name);
            return EXIT_FAILURE;
        }

        start_search = clock();
        while (getword(read_in_word, sizeof read_in_word, check) != EOF)
        {
            /* If word is not found in tree struct(the dictionary)
            print the word to stdout and increase count. */
            if (tree_search(t, read_in_word) != 1)
            {
                printf("%s\n", read_in_word);
                unknown_words++;
            }
        }
        end_search = clock();

        /* Print stats. */
        fprintf(stderr, "Fill time     : %f \n"
                        "Search time   : %f\n"
                        "Unknown words = %i\n",
                (end_fill - start_fill) /
                    (double)CLOCKS_PER_SEC,
                (end_search - start_search) / (double)CLOCKS_PER_SEC, unknown_words);
    }
    else if (tree_depth_check)
    {
        printf("%i\n", (tree_depth(t) - 1));
    }
    else if (print_dot)
    {
        if (dot_output_file == NULL)
        {
            dot_output_file = fopen("tree-view.dot", "w");
            dot_output_file_name = "tree-view.dot";
        }
        printf("Creating dot file '%s'\n", dot_output_file_name);
        tree_output_dot(t, dot_output_file);
        fclose(dot_output_file);
    }
    else
    {
        tree_preorder(t, print_info);
    }
    /* Free memory and exit. */
    tree_free(t);
    return EXIT_SUCCESS;
}

/**
 * Helper function that prints tree info with specific formatting.
 *
 * @param freq the frequency of the string.
 * @param word the string pulled from the tree.
 */
static void print_info(int freq, char *word)
{
    printf("%-4d %s\n", freq, word);
}

/**
 * Prints the help instructions to stdout.
 */
void print_help()
{
    printf("Usage: ./asgn2 [OPTION]... <STDIN>\n\n"
           "Perform various operations using a binary tree.  By "
           "default, words\nare read from stdin and added to the tree, "
           "before being printed out\nalongside their frequencies to "
           "stdout.\n\n -c FILENAME  Check spelling of words in FILENAME"
           " using words\n              read from stdin as the dictionary. ");
    printf(" Print timing\n              info & unknown words to stderr "
           "(ignore -d & -o)\n -d           Only print the tree depth"
           " (ignore -o)\n -f FILENAME  Write DOT output to FILENAME "
           "(if -o given)\n -o           Output the tree in DOT form "
           "to file 'tree-view.dot'\n -r           Make the tree an RBT"
           " (the default is a BST)\n\n -h           Print this message\n\n");
}
