#ifndef STDIOFN_H
#define STDIOFN_H

/**
 * # print
 * 
 * Prints a null-terminated string to the standard output.
 * 
 * ## Parameters
 * - `format` : A pointer to the null-terminated string to print.
 * 
 * ## Example
 * ```c
 * print("Hello World!");
 * ```
 */
extern void print(char *format);

/**
 * # print_int
 * 
 * Prints a long long integer to the standard output.
 * 
 * ## Parameters
 * - `data` : The integer value to print.
 * 
 * ## Example
 * ```c
 * print_int(42);
 * ```
 */
extern void print_int(long long data);

/**
 * # print_char
 * 
 * Prints a single character to the standard output.
 * 
 * ## Parameters
 * - `c` : The character to print.
 * 
 * ## Example
 * ```c
 * print_char('A');
 * ```
 */
extern void print_char(char c);

/**
 * # read_input
 * 
 * Reads input from the user into the provided buffer.
 * 
 * ## Parameters
 * - `buf` : The buffer to store input.
 * - `max_len` : Maximum number of characters to read.
 * 
 * ## Returns
 * - Number of bytes read.
 * 
 * ## Example
 * ```c
 * char buf[100];
 * int read = read_input(buf, 100);
 * ```
 */
extern int read_input(char *buf, int max_len);

/**
 * # nline
 * 
 * Prints a newline (CR + LF) to the output.
 * 
 * ## Example
 * ```c
 * print("Hello");
 * nline();
 * print("World");
 * ```
 */
void nline()
{
    char newline[3] = {13, 10, 0}; // CR + LF + null terminator
    print(newline);
}

/**
 * # printf
 * 
 * Simplified C-style formatted output function.
 * Supports `%s` (string), `%d` (int), `%c` (char), and `%%`.
 * 
 * ## Parameters
 * - `format` : Format string with specifiers.
 * - `args` : Array of pointers to arguments matching the specifiers.
 * 
 * ## Example
 * ```c
 * int a = 42;
 * char *name = "Alice";
 * char ch = 'X';
 * void *args[] = { &name, &a, &ch };
 * printf("Hello %s, score %d, char %c\n", args);
 * ```
 */
void printf(char *format, void **args)
{
    char *c = format;
    int arg_index = 0;

    while (*c != 0)
    {
        if (*c == '%')
        {
            c++;

            if (*c == 's')
            {
                char *str = (char *)args[arg_index];
                print(str);
                arg_index++;
                c++;
            }
            else if (*c == 'd')
            {
                int *num_ptr = (int *)args[arg_index];
                print_int(*num_ptr);
                arg_index++;
                c++;
            }
            else if (*c == 'c')
            {
                char *char_ptr = (char *)args[arg_index];
                print_char(*char_ptr);
                arg_index++;
                c++;
            }
            else if (*c == '%')
            {
                print_char('%');
                c++;
            }
            else
            {
                print_char('%');
                print_char(*c);
                c++;
            }
        }
        else
        {
            print_char(*c);
            c++;
        }
    }
}

/**
 * # is_whitespace
 * 
 * Checks if a character is a whitespace character.
 * 
 * ## Parameters
 * - `c` : The character to check.
 * 
 * ## Returns
 * - `1` if the character is space, tab, newline, or carriage return.
 * - `0` otherwise.
 */
int is_whitespace(char c)
{
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

/**
 * # is_digit
 * 
 * Checks if a character is a decimal digit.
 * 
 * ## Parameters
 * - `c` : The character to check.
 * 
 * ## Returns
 * - `1` if the character is '0'..'9'.
 * - `0` otherwise.
 */
int is_digit(char c)
{
    return c >= '0' && c <= '9';
}

/**
 * # skip_whitespace
 * 
 * Skips whitespace in a buffer starting from a given position.
 * 
 * ## Parameters
 * - `buf` : Input buffer.
 * - `pos` : Starting position.
 * 
 * ## Returns
 * - The new position after skipping whitespace.
 */
int skip_whitespace(char *buf, int pos)
{
    while (buf[pos] != 0 && is_whitespace(buf[pos]))
        pos++;
    return pos;
}

/**
 * # parse_int
 * 
 * Parses an integer from a buffer starting at a given position.
 * Supports optional '+' or '-' signs.
 * 
 * ## Parameters
 * - `buf` : Input buffer.
 * - `pos` : Pointer to starting position, updated after parsing.
 * - `result` : Pointer to store parsed value (long long).
 * 
 * ## Returns
 * - `1` if parsing succeeds.
 * - `0` if no valid integer found.
 */
int parse_int(char *buf, int *pos, long long *result)
{
    int p = *pos;
    int sign = 1;
    long long value = 0;
    int found_digit = 0;

    p = skip_whitespace(buf, p);

    if (buf[p] == '-') { sign = -1; p++; }
    else if (buf[p] == '+') { p++; }

    while (buf[p] != 0 && is_digit(buf[p]))
    {
        value = value * 10 + (buf[p] - '0');
        p++;
        found_digit = 1;
    }

    if (!found_digit) return 0;

    *result = value * sign;
    *pos = p;
    return 1;
}

/**
 * # parse_string
 * 
 * Parses a string (up to next whitespace) from a buffer.
 * 
 * ## Parameters
 * - `buf` : Input buffer.
 * - `pos` : Pointer to starting position, updated after parsing.
 * - `dest` : Destination buffer for parsed string.
 * - `max_len` : Maximum length of string including null terminator.
 * 
 * ## Returns
 * - `1` if parsing succeeds.
 * - `0` if no string found.
 */
int parse_string(char *buf, int *pos, char *dest, int max_len)
{
    int p = *pos;
    int i = 0;

    p = skip_whitespace(buf, p);

    while (buf[p] != 0 && !is_whitespace(buf[p]) && i < max_len - 1)
    {
        dest[i++] = buf[p++];
    }

    dest[i] = 0;

    if (i == 0) return 0;

    *pos = p;
    return 1;
}

/**
 * # parse_char
 * 
 * Parses a single non-whitespace character from a buffer.
 * 
 * ## Parameters
 * - `buf` : Input buffer.
 * - `pos` : Pointer to starting position, updated after parsing.
 * - `result` : Pointer to store character as long long.
 * 
 * ## Returns
 * - `1` if character found.
 * - `0` if buffer is empty.
 */
int parse_char(char *buf, int *pos, long long *result)
{
    int p = *pos;
    p = skip_whitespace(buf, p);

    if (buf[p] == 0) return 0;

    *result = buf[p];
    *pos = p + 1;
    return 1;
}

/**
 * # scanf
 * 
 * Simplified C-style input function.
 * Supports `%d` (int), `%s` (string), `%c` (char).
 * Reads input from user, parses according to format, stores in provided pointers.
 * 
 * ## Parameters
 * - `format` : Format string.
 * - `args` : Array of pointers to memory locations for storing parsed values.
 * 
 * ## Returns
 * - Number of successfully parsed items.
 * 
 * ## Example
 * ```c
 * int a;
 * char name[100];
 * char ch;
 * void *args[] = { &a, name, &ch };
 * int parsed = scanf("%d %s %c", args);
 * ```
 */
int scanf(char *format, void **args)
{
    char input_buf[256];
    int bytes_read = read_input(input_buf, 256);

    if (bytes_read <= 0) return 0;

    char *fmt = format;
    int arg_index = 0;
    int buf_pos = 0;
    int items_parsed = 0;

    while (*fmt != 0)
    {
        if (*fmt == '%')
        {
            fmt++;

            if (*fmt == 'd')
            {
                long long temp = 0;

                if (parse_int(input_buf, &buf_pos, &temp))
                {
                    int *int_dest = (int*)args[arg_index];
                    *int_dest = (int)temp;
                    items_parsed++;
                }
                else return items_parsed;

                arg_index++;
                fmt++;
            }
            else if (*fmt == 's')
            {
                char *str_dest = (char*)args[arg_index];

                if (!parse_string(input_buf, &buf_pos, str_dest, 100))
                    return items_parsed;

                items_parsed++;
                arg_index++;
                fmt++;
            }
            else if (*fmt == 'c')
            {
                long long temp = 0;

                if (parse_char(input_buf, &buf_pos, &temp))
                {
                    char *char_dest = (char*)args[arg_index];
                    char_dest[0] = (char)(temp & 0xFF);
                    items_parsed++;
                }
                else return items_parsed;

                arg_index++;
                fmt++;
            }
            else
            {
                fmt++;
            }
        }
        else if (is_whitespace(*fmt))
        {
            buf_pos = skip_whitespace(input_buf, buf_pos);
            fmt++;
        }
        else
        {
            buf_pos = skip_whitespace(input_buf, buf_pos);

            if (input_buf[buf_pos] == *fmt)
            {
                buf_pos++;
                fmt++;
            }
            else
            {
                return items_parsed;
            }
        }
    }

    return items_parsed;
}

#endif
