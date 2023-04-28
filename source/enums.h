#ifndef ENUMS_H
#define ENUMS_H

enum OCTDevice{
    COPERNICUS = 0,
    AVANTI = 1
};

enum LayerName{
    NONE = -1,
    PCV = 0,
    ERM_UP = 1, // IB_ERM = 1, OB_ERM = 2,
    ILM = 2,
    NFL_GCL = 3,
    GCL_IPL = 4,
    IPL_INL = 5,
    INL_OPL = 6,
    OPL_ONL = 7,
    ELM = 8,
    MEZ = 9,
    IS_OS = 10,     // IB_OPR = 12,
    OS_RPE = 11,    // IB_RPE = 13,
    RPE_CHR = 12    // OB_RPE = 14
};

#endif // ENUMS_H
