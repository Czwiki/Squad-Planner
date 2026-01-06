#include <iostream>
#include <string>
#include <cstdlib>

enum class Main_Command {
    help,
    exit,
    open,
    new_squad,
    unknown
};

Main_Command parse_main_command(const std::string& input) {
    if (input == "help") return Main_Command::help;
    if (input == "exit") return Main_Command::exit;
    if (input == "open") return Main_Command::open;
    if (input == "new_squad") return Main_Command::new_squad;
    return Main_Command::unknown;
}

// positions: GK, LB, LCB, CB, RCB, RB, LWB, RWB, CDM, LCDM, RCDM, LM, RM, LCM, CM, RCM, CAM, LCAM, RCAM, LAM, RAM, ST, LST, RST
// number of positions: 1 (GK), 7 (DEF) incl. FV, 3 (DM), 5 (M) incl. LM/RM, 5 (OM), 3 (ST) = 24

void help_cli() {
    std::cout << "Available commands:" << std::endl << std::endl;
    std::cout << "  help - Show this help message" << std::endl;
    std::cout << "  exit - Exit the Squad Planner" << std::endl;
    std::cout << "  open - Open an existing squad file" << std::endl;
    std::cout << "  new_squad - Create a new squad" << std::endl << std::endl;
    std::cout << "for more information on explicit commands, enter 'command_name --help'" << std::endl;
    // Add more commands as needed
}

int main() {
    std::system("clear");
    std::cout << "Hello, User! Welcome to the Squad Planner!" << std::endl << "Enter \"help\" to see available commands" << std::endl;
    std::string userInput;
    while (true)
    {
        std::cout << "Squad planner: ";
        std::getline(std::cin, userInput); // same as in c without newline
        Main_Command command = parse_main_command(userInput);
        if (command == Main_Command::help) {
            help_cli();
            continue;
        }
        else if (command == Main_Command::exit) {
            std::cout << "Exiting Squad Planner. Goodbye!" << std::endl;
            break;
        }
        else if (command == Main_Command::open) {
            std::cout << "Open command selected. (Functionality not implemented yet)" << std::endl;
            continue;
        }
        else if (command == Main_Command::new_squad) {
            std::cout << "New Squad command selected. (Functionality not implemented yet)" << std::endl;
            continue;
        }
        else if (command == Main_Command::unknown) {
            std::cout << "Unknown command. Type 'help' for a list of commands." << std::endl;
            continue;
        }
    }
    return 0;
}