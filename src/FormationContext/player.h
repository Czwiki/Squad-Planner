struct player
{
    char* name;
    int age;
    int overall_rating;
    int potential_rating;
    int own_rating;
    int* positions; // Array of position IDs the player can play
    void* next; // Pointer to the next player in the linked list
    void* prev; // Pointer to the previous player in the linked list
}typedef player;
