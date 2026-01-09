struct command
{
    int id;
    char *name;
    char **options;
    char **args;

}typedef command;

command* parse_command(const char *line, int* ret_val);