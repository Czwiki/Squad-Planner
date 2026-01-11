struct command
{
    int id;
    int context;
    int future_context;
    char *name;
    char **options;
    char **args;

}typedef command;