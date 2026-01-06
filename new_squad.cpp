# include <iostream>
# include <string>

enum class New_Squad_Command {
    create,
    add_player,
    remove_player,
    set_formation,
    back,
    unknown
};

New_Squad_Command parse_new_squad_command(const std::string& input) {
    if (input == "create") return New_Squad_Command::create;
    if (input == "add_player") return New_Squad_Command::add_player;
    if (input == "remove_player") return New_Squad_Command::remove_player;
    if (input == "set_formation") return New_Squad_Command::set_formation;
    if (input == "back") return New_Squad_Command::back;
    return New_Squad_Command::unknown;
}


void new_squad_cli() {
    // Implementation for creating a new squad
    // 'ptr' can be used to pass any necessary data or context
    
    while (condition)
    {
        /* code */
    }
    
}