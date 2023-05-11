#ifndef ENUMS_H
#define ENUMS_H

enum OCTDevice{
    COPERNICUS = 0,
    AVANTI = 1
};

enum LayerName{
    NONE = -1,
    PCV = 0,
    IB_ERM = 1,
    OB_ERM = 2,
    ILM = 3,        // (ILM)
    RNFL_GCL = 4,    // (RNFL-GCL)
    GCL_IPL = 5,    // (GCL-IPL)
    IPL_INL = 6,    // (IPL-INL)
    INL_OPL = 7,    // (INL-OPL)
    OPL_ONL = 8,    // (OPL-HFL) OPL-Henles fiber layer
    ELM = 9,
    MEZ = 10,       // (BMEIS) Boundary of myoid and ellipsoid of inner segments
    IS_OS = 11,     // (IS/OSJ) IS/OS junction
    IB_OPR = 12,    // (IB_OPR) Inner boundary of OPR
    IB_RPE = 13,    // (IB_RPE) Inner boundary of RPE
    OB_RPE = 14     // (OB_RPE) Outer boundary of RPE
};

enum Annotation : bool {
    MANUAL = true,
    AUTO = false
};

#endif // ENUMS_H
