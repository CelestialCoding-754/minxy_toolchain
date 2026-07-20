extern void     ExitProcess(int);
extern void*    CreateFileA(char*, int, int, void*, int, int, void*);
extern int      ReadFile(void*, char*, int, int*, void*);
extern int      WriteFile(void*, char*, int, int*, void*);
extern void*    GetStdHandle(int);
extern void*    GetProcessHeap();
extern void*    HeapAlloc(void*, int, int);
extern int      HeapFree(void*, int, void*);
extern void*    HeapReAlloc(void*, int, void*, int);
extern int      GetLastError();

void*   stdin;
void*   stdout;
void*   stderr;
void*   hheap;

char*   source;
int     source_index;

int*    token_type;
int*    token_nval;
char**  token_sval;
int     token_len;
int     token_cap;

//  TOKEN TYPES
//  
//  000 -> End of token
//  001 -> identifier
//  002 -> number
//  003 -> string
//  100 -> void
//  101 -> int
//  102 -> char
//  103 -> return
//  104 -> if
//  105 -> else
//  106 -> while
//  107 -> break
//  108 -> continue
//  109 -> goto
//  110 -> extern
//  200 -> ;
//  201 -> +
//  202 -> -
//  203 -> *
//  204 -> /
//  205 -> %
//  206 -> =
//  207 -> ~
//  208 -> &
//  209 -> |
//  210 -> ^
//  211 -> !
//  212 -> &&
//  213 -> ||
//  214 -> ==
//  215 -> !=
//  216 -> >
//  217 -> <
//  218 -> >=
//  219 -> <=
//  220 -> (
//  221 -> )
//  222 -> {
//  223 -> }
//  224 -> :
//  225 -> ,

int strlen(char* str)
{
    int i = 0;
    while (*(str + i))
    {
        i = i+1;
    }
    return i;
}

int streq(char* str1, char* str2)
{
    while (*str1 && *str2)
    {
        if (*str1 != *str2)
            return 0;

        str1 = str1 + 1;
        str2 = str2 + 1;
    }
    return *str1 == *str2;
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
        print_char(45);
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
    HeapFree(hheap, 0, ptr);
}

void memcpy(char* src, char* dst, int count)
{
    int i = 0;
    while (i < count)
    {
        *(dst + i) = *(src + i);
        i = i + 1;
    }
}

void read_input_file(void* handle)
{
    int capacity = 1024;
    int length = 0;
    int bytesread;

    source = alloc(capacity);

    while (1)
    {
        if (length == capacity-1)
        {
            capacity = capacity * 2;
            source = realloc(source, capacity);
            if (!source)
                ExitProcess(3);
        }

        if (!ReadFile(handle, source+length, capacity-length-1, &bytesread, 0))
        {
            print_num(GetLastError());
            ExitProcess(2);
        }

        if (bytesread == 0) break;

        length = length + bytesread;
    }

    *(source + length) = 0;
}

int is_alpha(char c)
{
    if (c >= 'A' && c <= 'Z')   return 1;
    if (c >= 'a' && c <= 'z')   return 1;
    return 0;
}

int is_digit(char c)
{
    if (c >= '0' && c <= '9')   return 1;
    return 0;
}

int is_octal(char c)
{
    if (c >= '0' && c <= '7')   return 1;
    return 0;
}

int get_hex(char c)
{
    if (c >= '0' && c <= '9')   return c - '0';
    if (c >= 'a' && c <= 'z')   return c - 'a';
}

int is_hex(char c)
{
    return get_hex(c) != 0;
}

int is_alnum(char c)
{
    if (is_alpha(c)) return 1;
    if (is_digit(c)) return 1;
    return 0;
}

int is_space(char c)
{
    if (c == ' ')   return 1;
    if (c == '\t')  return 1;
    if (c == '\n')  return 1;
    if (c == '\v')  return 1;
    if (c == '\f')  return 1;
    if (c == '\r')  return 1;
    return 0;
}

void token_add(int type, char* sval, int nval)
{
    if (token_len >= token_cap)
    {
        token_cap = token_cap * 2;
        token_type = realloc(token_type, token_cap * 4);
        token_nval = realloc(token_nval, token_cap * 4);
        token_sval = realloc(token_sval, token_cap * 8);
    }

    *(token_type + token_len) = type;
    *(token_nval + token_len) = nval;
    *(token_sval + token_len) = sval;

    token_len = token_len + 1;

    return;
}

void token_list()
{
    int i = 0;
    while (i != token_len)
    {
        if (*(token_type + i) == 0)
            print_str("end");
        else if (*(token_type + i) == 1)
        {
            print_str("IDT: ");
            print_str(*(token_sval + i));
        }
        if (*(token_type + i) == 2)
        {
            print_str("NUM: ");
            print_num(*(token_nval + i));
        }
        if (*(token_type + i) == 3)
        {
            print_str("STR: ");
            print_str(*(token_sval + i));
        }
        else if (*(token_type + i) == 100)
            print_str("KEY: void");
        else if (*(token_type + i) == 101)
            print_str("KEY: int");
        else if (*(token_type + i) == 102)
            print_str("KEY: char");
        else if (*(token_type + i) == 103)
            print_str("KEY: return");
        else if (*(token_type + i) == 104)
            print_str("KEY: if");
        else if (*(token_type + i) == 105)
            print_str("KEY: else");
        else if (*(token_type + i) == 106)
            print_str("KEY: while");
        else if (*(token_type + i) == 107)
            print_str("KEY: break");
        else if (*(token_type + i) == 108)
            print_str("KEY: continue");
        else if (*(token_type + i) == 109)
            print_str("KEY: goto");
        else if (*(token_type + i) == 110)
            print_str("KEY: extern");

        else if (*(token_type + i) == 200)
            print_str("CHR: ;");
        else if (*(token_type + i) == 201)
            print_str("CHR: +");
        else if (*(token_type + i) == 202)
            print_str("CHR: -");
        else if (*(token_type + i) == 203)
            print_str("CHR: *");
        else if (*(token_type + i) == 204)
            print_str("CHR: /");
        else if (*(token_type + i) == 205)
            print_str("CHR: %");
        else if (*(token_type + i) == 206)
            print_str("CHR: =");
        else if (*(token_type + i) == 207)
            print_str("CHR: &");
        else if (*(token_type + i) == 208)
            print_str("CHR: |");
        else if (*(token_type + i) == 209)
            print_str("CHR: ~");
        else if (*(token_type + i) == 210)
            print_str("CHR: ^");
        else if (*(token_type + i) == 211)
            print_str("CHR: !");
        else if (*(token_type + i) == 212)
            print_str("CHR: &&");
        else if (*(token_type + i) == 213)
            print_str("CHR: ||");
        else if (*(token_type + i) == 214)
            print_str("CHR: ==");
        else if (*(token_type + i) == 215)
            print_str("CHR: !=");
        else if (*(token_type + i) == 216)
            print_str("CHR: >");
        else if (*(token_type + i) == 217)
            print_str("CHR: >=");
        else if (*(token_type + i) == 218)
            print_str("CHR: <");
        else if (*(token_type + i) == 219)
            print_str("CHR: <=");
        else if (*(token_type + i) == 220)
            print_str("CHR: (");
        else if (*(token_type + i) == 221)
            print_str("CHR: )");
        else if (*(token_type + i) == 222)
            print_str("CHR: {");
        else if (*(token_type + i) == 223)
            print_str("CHR: }");
        else if (*(token_type + i) == 224)
            print_str("CHR: :");
        else if (*(token_type + i) == 225)
            print_str("CHR: ,");

        print_char(10);

        i = i + 1;
    }
}

void lexer_identifier()
{
    int start = source_index;
    while (is_alnum(*(source + source_index)))
    {
        source_index = source_index + 1;
    }

    int len = source_index - start;
    char* identifier = alloc(len);

    memcpy(source + start, identifier, len);
    *(identifier + len) = '\0';

    if      (streq(identifier, "void"))
        token_add(100, 0, 0);
    else if (streq(identifier, "int"))
        token_add(101, 0, 0);
    else if (streq(identifier, "char"))
        token_add(102, 0, 0);
    else if (streq(identifier, "return"))
        token_add(103, 0, 0);
    else if (streq(identifier, "if"))
        token_add(104, 0, 0);
    else if (streq(identifier, "else"))
        token_add(105, 0, 0);
    else if (streq(identifier, "while"))
        token_add(106, 0, 0);
    else if (streq(identifier, "break"))
        token_add(107, 0, 0);
    else if (streq(identifier, "continue"))
        token_add(108, 0, 0);
    else if (streq(identifier, "goto"))
        token_add(109, 0, 0);
    else if (streq(identifier, "extern"))
        token_add(110, 0, 0);
    else
        token_add(001, identifier, 0);
}

void lexer_number()
{
    int num = 0;
    char c = *(source + source_index);
    while (is_digit(c))
    {
        num = num * 10 + c - 48;
        source_index = source_index + 1;
        c = *(source + source_index);
    }
    token_add(002, 0, num);
}

char lexer_char(char c)
{
    if (c == '\\')
    {
        source_index = source_index + 1;
        c = *(source + source_index);

        if      (c == '\\') return '\\';
        else if (c == 'a')  return '\a';
        else if (c == 'b')  return '\b';
        else if (c == 'f')  return '\f';
        else if (c == 'n')  return '\n';
        else if (c == 'r')  return '\r';
        else if (c == 't')  return '\t';
        else if (c == 'v')  return '\v';
        else if (c == '\'') return '\'';
        else if (c == '\"') return '\"';
        else if (c == '?')  return '\?';
        else if (c == '0')  return '\0';
    }
    else
    {
        return c;
    }
}

void lexer_litchar()
{
    source_index = source_index + 1;
    char c = lexer_char(*(source + source_index));
    source_index = source_index + 1;

    if (*(source + source_index) != '\'')
    {
        print_str("invalid literal character");
        ExitProcess(1);
    }
    source_index = source_index + 1;

    token_add(002, 0, c);
}

void lexer_string()
{
    source_index = source_index + 1;

    int cap = 8;
    int len = 0;
    char* str = alloc(cap);

    if (*(source + source_index) != '\"')
    {
        char c = lexer_char(*(source + source_index));
        source_index = source_index + 1;
        
        if (len >= cap)
        {
            cap = cap * 2;
            str = realloc(cap);
        }

        *(str + len) = c;
        len = len + 1;
    }
}

void skip_space()
{
    while (is_space(*(source + source_index)))
    {
        source_index = source_index + 1;
    }
}

void lexer()
{
    source_index = 0;

    while (*(source + source_index))
    {
        skip_space();

        char c = *(source + source_index);

        if (is_alpha(c))
            lexer_identifier();
        else if (is_digit(c))
            lexer_number();
        else if (c == '\'')
            lexer_litchar();
        else
            source_index = source_index + 1;
    }

    token_add(0, 0, 0);
}

void _start()
{
    stdin       = GetStdHandle(-10);
    stdout      = GetStdHandle(-11);
    stderr      = GetStdHandle(-12);
    hheap       = GetProcessHeap();

    token_type  = alloc(32 * 4);
    token_nval  = alloc(32 * 4);
    token_sval  = alloc(32 * 8);
    token_len   = 0;
    token_cap   = 32;

    read_input_file(CreateFileA("test.c", 0x80000000, 1, 0, 3, 0, 0));

    lexer();
    token_list();

    ExitProcess(0);
}