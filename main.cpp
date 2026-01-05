#include <iostream>
#include <string>
#include <cstdlib>

class Position {
public:
};
// positions: GK, LB, LCB, CB, RCB, RB, LWB, RWB, CDM, LCDM, RCDM, LM, RM, LCM, CM, RCM, CAM, LCAM, RCAM, LAM, RAM, ST, LST, RST
// number of positions: 1 (GK), 7 (DEF) incl. FV, 3 (DM), 5 (M) incl. LM/RM, 5 (OM), 3 (ST) = 24

int main() {
    std::system("clear");
    std::cout << "Hello, User! Welcome to the Squad Planner!" << std::endl;
    std::string userInput;
    while (true)
    {
        std::cout << "your input: ";
        std::getline(std::cin, userInput); // same as in c without newline
        std::cout << "You entered: " << userInput << std::endl;
    }
    


    return 0;
}