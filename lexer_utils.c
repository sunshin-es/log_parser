
#include "lexer.h"
#include "parser.h"

/* Create a lexer for the provided file */
yyinput_t * create_lexer(FILE * file, const size_t size, const size_t maxfill)
{
	yyinput_t * input = NULL;

	input = (yyinput_t *) malloc(sizeof(yyinput_t));

	if (input == NULL)
	{
        /* Unable to allocate enough memory for the lexer */
	}
	else
	{
		memset(input, 0x00, sizeof(yyinput_t));

		input->buffer = (iutf8_t *) malloc(sizeof(iutf8_t) * (size + maxfill));

		if (input->buffer == NULL)
		{
            /* Unable to allocate enough memory for the actual string buffer used by the lexer */
		}
		else
		{
			memset(input->buffer, YYINVAILD, (size + maxfill));

	    	input->limit   = input->buffer + size;
   			input->cursor  = input->limit;
    		input->token   = input->limit;
    		input->eof     = false;
    		input->size    = size;
    		input->maxfill = maxfill;
    		input->file    = file;
		}
	}
    
    return input;
}

/* Get the value of the current token from within the lexers buffer */
void get_token_value(yyinput_t * input, tok_t token, YYSTYPE output)
{
    size_t length = input->cursor - input->token;
    strncpy( output, input->token, length );
    output[length] = '\0';
}

void destroy_lexer(yyinput_t * lexer)
{
    free(lexer->buffer);
    free(lexer);
}

/* Explaination of fill - taken directly from: http://re2c.org/examples/example_03.html
 *
 * The idea of YYFILL is fairly simple: the lexer is stuck upon the fact that (YYLIMIT - YYCURSOR) < n
 * and YYFILL must either invert this condition or stop lexing. A disaster will happen if YYFILL fails to provide
 * at least n characters, yet resumes lexing. Technically YYFILL must somehow "extend" the input by
 * at least n characters: after YYFILL, all input pointers must point to the exact same characters, except YYLIMIT,
 * which must be advanced at least n positions. Since we want to use a fixed amount of memory,
 * we have to shift buffer contents: discard characters that have already been lexed, move the
 * remaining characters to the beginning of the buffer, and fill the vacant space with new characters.
 * All the pointers must be decreased by the length of the discarded input,
 * except YYLIMIT (it must point at the end of buffer).
 */
bool fill(yyinput_t * input, const size_t need)
{
    const size_t remaining = input->token - input->buffer;
    size_t nread;
    
    if (input->eof != false)
    {
        return false;
    }
    else if (remaining < need)
    {
        return false;
    }
    else
    {
        /* buffer is the entire memory buffer
           token  is the location of the current char within buffer
           (limit - token) is the amount of remaining unprocessed data that will be move from the current
           location (token) to the start of the buffer so processing can continue.
         */
        memmove(input->buffer, input->token, input->limit - input->token);
        input->limit  -= remaining;
        input->cursor -= remaining;
        input->token  -= remaining;
        
        nread = fread(input->limit, (size_t) 1, remaining, input->file);
        input->limit += nread; 
        
        if (nread < remaining)
        {
            /* Either we've reached the end of the input file, or failed to read enough data to continue,
             * in both cases we're done here...
             */
            input->eof = true;
            memset(input->limit, YYINVAILD, input->maxfill);
            input->limit += input->maxfill;
        }
        
        return true;
    }
}

tok_t exit_success(yyinput_t * input)
{
	tok_t result = LEXER_EXIT_ERROR;

	if (input->maxfill == ((size_t) (input->limit - input->token)))
	{
        /* The current token is exaclty at the end of the input buffer. So if we have lexed to an end we have
         * sucessfully lexed the entire input.
         */
		result = TOKEN_END;
	}
	else
	{
        
	}
	
	return result;
}