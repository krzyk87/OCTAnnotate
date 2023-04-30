#include "functions.h"
#include "qmath.h"

LayerName decodeLayer(QString layer){
    LayerName l = NONE;
    if ((layer == "PCV") || (layer == "PSH")){
        l = PCV;
    } else if ((layer == "IB_ERM") || (layer == "ERM")){
        l = IB_ERM;
    } else if (layer == "OB_ERM"){
        l = OB_ERM;
    } else if (layer == "ILM"){
        l = ILM;
    } else if ((layer == "RNFL_GCL") || (layer == "GCL")){
        l = RNFL_GCL;
    } else if ((layer == "GCL_IPL") || (layer == "IPL")){
        l = GCL_IPL;
    } else if ((layer == "IPL_INL") || (layer == "INL")){
        l = IPL_INL;
    } else if ((layer == "INL_OPL") || (layer == "OPL")){
        l = INL_OPL;
    } else if ((layer == "OPL_ONL") || (layer == "ONL")){
        l = OPL_ONL;
    } else if (layer == "ELM"){
        l = ELM;
    } else if (layer == "MEZ"){
        l = MEZ;
    } else if (layer == "IOS"){
        l = IS_OS;
    } else if (layer == "OPR"){
        l = IB_OPR;
    } else if ((layer == "IB_RPE") || (layer == "RPE")){
        l = IB_RPE;
    } else if ((layer == "OB_RPE") || (layer == "CHR")){
        l = OB_RPE;
    }
    return l;
}

QString encodeLayer(LayerName layer){
    QString s = "";

    switch (layer) {
    case PCV:
        s = "PCV";
        break;
    case IB_ERM:
        s = "IB_ERM";
        break;
    case OB_ERM:
        s = "OB_ERM";
        break;
    case ILM:
        s = "ILM";
        break;
    case RNFL_GCL:
        s = "RNFL_GCL";
        break;
    case GCL_IPL:
        s = "GCL_IPL";
        break;
    case IPL_INL:
        s = "IPL_INL";
        break;
    case INL_OPL:
        s = "INL_OPL";
        break;
    case OPL_ONL:
        s = "OPL_ONL";
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
    case IB_OPR:
        s = "IB_OPR";
        break;
    case IB_RPE:
        s = "IB_RPE";
        break;
    case OB_RPE:
        s = "OB_RPE";
        break;
    case NONE:
        s = "NONE";
    }
    return s;
}

QList<LayerName> getAllLayers(){
    QList<LayerName> list;
    list.append(PCV);
    list.append(IB_ERM);
    list.append(OB_ERM);
    list.append(ILM);
    list.append(RNFL_GCL);
    list.append(GCL_IPL);
    list.append(IPL_INL);
    list.append(INL_OPL);
    list.append(OPL_ONL);
    list.append(ELM);
    list.append(MEZ);
    list.append(IS_OS);
    list.append(IB_OPR);
    list.append(IB_RPE);
    list.append(OB_RPE);
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
