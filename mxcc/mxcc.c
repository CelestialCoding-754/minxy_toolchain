extern void     ExitProcess(int);
extern int      CreateFileA(char*, int, int, void*, int, int, void*);
extern int      ReadFile(void*, char*, int, int*, void*);
extern int      WriteFile(void*, char*, int, int*, void*);
extern void*    GetStdHandle(int);
extern void*    GetProcessHeap();
extern void*    HeapAlloc(void*, int, int);
extern int      HeapFree(void*, void*);
extern void*    HeapReAlloc(void*, int, void*, int);

void*   stdin;
void*   stdout;
void*   stderr;
void*   hheap;

char*   source;

int*    token_type;
int*    token_nval;
char**  token_sval;
int     token_len;
int     token_cap;

int*    node_type;
int**   node_child;
int*    node_chcnt;
int*    node_vtype;
int*    node_nval;
char*   node_sval;
int*    node_oprt;
int     node_len;
int     node_cap;

int     cdgen_side;
int     cdgen_sp;
int     cdgen_lcl;
int     cdgen_func;

char*** sym_name;
int**   sym_type;
int**   sym_offset;
int*    sym_len;
int*    sym_cap;
int     sym_current;
int     sym_tcap;

/*
TOKEN TYPES

000 -> End of token
001 -> semicolon
002 -> identifier
003 -> number
004 -> string
100 -> void
101 -> int
102 -> char
200 -> +
201 -> -
202 -> *
203 -> /
204 -> %
205 -> =
206 -> ~
207 -> &
208 -> |
209 -> ^
210 -> !
211 -> &&
212 -> ||
213 -> ==
214 -> !=
215 -> >
216 -> <
217 -> >=
218 -> <=
219 -> (
220 -> )
221 -> {
222 -> }
300 -> if
301 -> goto
303 -> return

*/

int strlen(char* str)
{
    int i = 0;
    loop:
        if (*(str + i))
        {
            i = i+1;
            goto loop;
        }
    return i;
}

void print_str(char* str)
{
    int len = strlen(str);
    WriteFile(stdout, str, len, 0, 0);
}

void print_char(char c)
{
    char ch = c;
    WriteFile(stdout, &ch, 1, 0, 0);
}

void print_num(int n)
{
    if (n < 0)
    {
        n = -n;
        print_char('-');
    }

    if (n > 9) {
        print_num(n / 10);
    }
    
    print_char(48 + n % 10);
}

void* alloc(int size)
{
    return HeapAlloc(hheap, 0, size);
}

void* realloc(void* ptr, int size)
{
    return HeapReAlloc(hheap, 0, ptr, size);
}

void free(void* ptr)
{
    HeapFree(hheap, ptr);
}

void read_input_file(void* handle)
{
    int capacity = 2;
    int length = 0;
    int bytesread;

    source = alloc(capacity);

    loop:
        if (length == capacity-1)
        {
            capacity = capacity * 2;
            source = realloc(source, capacity);
        }

        ReadFile(handle, source+length, capacity-length-1, &bytesread, 0);

        if (bytesread == 0) goto end;

        length = length + bytesread;
    goto loop;

    end:

    *(source + length) = 0;
}

int is_symbol(char c)
{
    if (c == 43)  return 1;     /* + */
    if (c == 45)  return 1;     /* - */
    if (c == 42)  return 1;     /* * */
    if (c == 47)  return 1;     /* / */
    if (c == 37)  return 1;     /* % */
    if (c == 38)  return 1;     /* & */
    if (c == 124) return 1;     /* | */
    if (c == 126) return 1;     /* ~ */
    if (c == 33)  return 1;     /* ! */
    if (c == 61)  return 1;     /* = */
    if (c == 62)  return 1;     /* > */
    if (c == 60)  return 1;     /* < */
    if (c == 40)  return 1;     /* ( */
    if (c == 41)  return 1;     /* ) */
    if (c == 123) return 1;     /* { */
    if (c == 125) return 1;     /* } */
    if (c == 59)  return 1;     /* ; */
    return 0;
}

int is_term(char c)
{
    if (c == 32) return 1;
    if (c == 10) return 1;
    if (c == 13) return 1;
    if (c == 0)  return 1;
    return is_symbol(c);
}

void token_add(int type, char* sval, int nval)
{
    if (token_len >= token_cap)
    {
        token_cap = token_cap * 2;
        token_type = realloc(token_type, token_cap);
        token_nval = realloc(token_nval, token_cap);
        token_sval = realloc(token_sval, token_cap);
    }

    *(token_type + token_len) = type;
    *(token_nval + token_len) = nval;
    *(token_sval + token_len) = sval;

    return;
}

void lexer()
{
    int i = 0;
    char* tokval;
    int toklen = 0;
    char c = 255;

    loop1:
        if (c == 0)
            goto end;

        c = *(source + i);

        /* token */
        if (is_term(c))
        {
            if (toklen != 0)
            {
                *(i + source) = 0;
                tokval = source + i - toklen;
                toklen = 0;

                token_add(002, tokval, 0);
            }

            if (c == 43)
            {
                token_add(200, 0, 0);
                goto endif0;
            }
            if (c == 45)
            {
                token_add(201, 0, 0);
                goto endif0;
            }
            if (c == 42)
            {
                token_add(202, 0, 0);
                goto endif0;
            }
            if (c == 47)
            {
                token_add(203, 0, 0);
                goto endif0;
            }
            if (c == 37)
            {
                token_add(204, 0, 0);
                goto endif0;
            }
            if (c == 61)
            {
                token_add(205, 0, 0);
                goto endif0;
            }
            if (c == 126)
            {
                token_add(206, 0, 0);
                goto endif0;
            }
            if (c == 38)
            {
                token_add(207, 0, 0);
                goto endif0;
            }
            if (c == 124)
            {
                token_add(208, 0, 0);
                goto endif0;
            }
            if (c == 94)
            {
                token_add(209, 0, 0);
                goto endif0;
            }
            if (c == 33)
            {
                token_add(210, 0, 0);
                goto endif0;
            }
            if (c == 123)
            {
                token_add(221, 0, 0);
                goto endif0;
            }
            if (c == 125)
            {
                token_add(222, 0, 0);
                goto endif0;
            }
            endif0:

            i = i + 1;
            goto loop1;
        }

        i = i + 1;
        toklen = toklen + 1;
    goto loop1;

    end:
    return;
}

void _start()
{
    stdin       = GetStdHandle(-10);
    stdout      = GetStdHandle(-11);
    stderr      = GetStdHandle(-12);
    hheap       = GetProcessHeap();

    token_type  = alloc(32);
    token_nval  = alloc(32);
    token_sval  = alloc(32);
    token_len   = 0;
    token_cap   = 32;

    node_type   = alloc(32);
    node_child  = alloc(32);
    node_chcnt  = alloc(32);
    node_vtype  = alloc(32);
    node_nval   = alloc(32);
    node_sval   = alloc(32);
    node_oprt   = alloc(32);
    node_len    = 0;
    node_cap    = 32;

    cdgen_side  = 0;
    cdgen_sp    = 0;
    cdgen_lcl   = 0;
    cdgen_func  = 0;

    sym_name    = alloc(4);
    sym_type    = alloc(4);
    sym_offset  = alloc(4);
    sym_len     = alloc(4);
    sym_cap     = alloc(4);
    sym_current = 0;
    sym_tcap    = 4;

    read_input_file(stdin);

    lexer();

    ExitProcess(10);
}