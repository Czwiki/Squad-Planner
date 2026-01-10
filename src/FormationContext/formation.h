#include "position.h"

char ** positions = {"GK", "LWB", "LB", "LCB", "CB", "RCB", "RDB", "RWB", "LCDM", "CDM", "RCDM", "LM", "LCM", "CM", "RCM", "RM", "LAM", "LCAM", "CAM", "RCAM", "RAM", "LS", "ST", "RS"};

struct formation
{
    int map_of_positions[24];
    position positions[11];
}typedef formation;