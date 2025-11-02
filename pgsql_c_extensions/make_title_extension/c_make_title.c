/* -------------------------------------------------
 * c_make_title.c
 * -------------------------------------------------
 */
#include "postgres.h"
#include "utils/builtins.h"
#include "c.h"

#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "fmgr.h"
#include "varatt.h"

/**
 * to_title_case will convert a string to title case, where the first letter of
 * each word is uppercase and the rest are lowercase.
 *
 * @param str The string to convert (modified in-place)
 * @param len Length of the string
 */
static void to_title_case(char* str, int32 len) {
    int new_word = 1;  // Flag to track word boundaries
    int i;

    for (i = 0; i < len; i++) {
        if (isspace(str[i]) || ispunct(str[i])) {
            new_word = 1;  // Next character will be the start of a new word
            str[i] = str[i];
        } else {
            if (new_word) {
                str[i] = toupper(str[i]);  // Convert first letter of word to uppercase
                new_word = 0;
            } else {
                str[i] = tolower(str[i]);  // Convert rest of letters to lowercase
            }
        }
    }
}

PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(c_convert_to_title);

Datum
c_convert_to_title(PG_FUNCTION_ARGS)
{
    text  *arg1 = PG_GETARG_TEXT_PP(0);             // extract arg 0
    int32 arg1_size = VARSIZE_ANY_EXHDR(arg1);      // get length
    int32 new_text_size = arg1_size + VARHDRSZ;     // calc len for return value
    text *new_text = (text *) palloc(new_text_size); // allocate for return

    SET_VARSIZE(new_text, new_text_size);           // Set len for return
    memcpy(VARDATA(new_text), VARDATA_ANY(arg1), arg1_size); // copy in data

    to_title_case(VARDATA(new_text), arg1_size);    // modify string to be title

    PG_RETURN_TEXT_P(new_text);
}