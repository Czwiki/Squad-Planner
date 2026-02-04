struct player
{
    char* name;
    int age;
    int overall_rating;
    int potential_rating;
    int own_rating;
    struct player* next;
}typedef player;
