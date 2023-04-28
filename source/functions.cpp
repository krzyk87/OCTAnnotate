#include "functions.h"
#include "qmath.h"

LayerName decodeLayer(QString layer){
    LayerName l = NONE;
    if ((layer == "PCV") || (layer == "PSH")){
        l = PCV;
    } else if (layer == "ERM"){
        l = ERM_UP;
    } else if (layer == "ILM"){
        l = ILM;
    } else if (layer == "GCL"){
        l = NFL_GCL;
    } else if (layer == "IPL"){
        l = GCL_IPL;
    } else if (layer == "INL"){
        l = IPL_INL;
    } else if (layer == "OPL"){
        l = INL_OPL;
    } else if (layer == "ONL"){
        l = OPL_ONL;
    } else if (layer == "ELM"){
        l = ELM;
    } else if (layer == "MEZ"){
        l = MEZ;
    } else if (layer == "IOS"){
        l = IS_OS;
    } else if (layer == "RPE"){
        l = OS_RPE;
    } else if (layer == "CHR"){
        l = RPE_CHR;
    }
    return l;
}

QString encodeLayer(LayerName layer){
    QString s = "";

    switch (layer) {
    case PCV:
        s = "PCV";
        break;
    case ERM_UP:
        s = "ERM";
        break;
    case ILM:
        s = "ILM";
        break;
    case NFL_GCL:
        s = "GCL";
        break;
    case GCL_IPL:
        s = "IPL";
        break;
    case IPL_INL:
        s = "INL";
        break;
    case INL_OPL:
        s = "OPL";
        break;
    case OPL_ONL:
        s = "ONL";
        break;
    case ELM:
        s = "ELM";
        break;
    case MEZ:
        s = "MEZ";
        break;
    case IS_OS:
        s = "IOS";
        break;
    case OS_RPE:
        s = "RPE";
        break;
    case RPE_CHR:
        s = "CHR";
        break;
    case NONE:
        s = "NONE";
    }
    return s;
}

QList<LayerName> getAllLayers(){
    QList<LayerName> list;
    list.append(PCV);
    list.append(ERM_UP);
    list.append(ILM);
    list.append(NFL_GCL);
    list.append(GCL_IPL);
    list.append(IPL_INL);
    list.append(INL_OPL);
    list.append(OPL_ONL);
    list.append(ELM);
    list.append(MEZ);
    list.append(IS_OS);
    list.append(OS_RPE);
    list.append(RPE_CHR);
    return list;
}

double calculateDistance(QPoint p1, QPoint p2, double dx, double dy){
    double deltaX = p1.x() - p2.x();
    double deltaY = p1.y() - p2.y();
    if (dx != 0){
        deltaX = deltaX * dx;
    }
    if (dy != 0){
        deltaY = deltaY * dy;
    }
    double dist = qSqrt(qPow(deltaX,2) + qPow(deltaY,2));
    return dist;
}
