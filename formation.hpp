struct Formation {
    int goalkeepers[1] = {0};
    int defenders[7] = {0,0,0,0,0,0,0}; // FV, LB, LCB, CB, RCB, RB, FV
    int defensive_midfielders[3] = {0,0,0}; // CDM, LCDM, RCDM
    int midfielders[5] = {0,0,0,0,0}; // LM, RM, LCM, CM, RCM
    int offensive_midfielders[5] = {0,0,0,0,0}; // CAM, LCAM, RCAM, LAM, RAM
    int strikers[3] = {0,0,0}; // ST, LST, RST
};