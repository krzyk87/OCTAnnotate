#include "layer.h"
#include "functions.h"

// ----------------------------------------------------------------------------
// Layer class ----------------------------------------------------------------
// ----------------------------------------------------------------------------

Layer::Layer(int bscanWidth, int bscansNumber, int index)
{
    this->index = index;
    this->layer = (Layers)index;
    this->name = encodeLayer(this->layer);
    this->bscanWidth = bscanWidth;
    this->bscansNumber = bscansNumber;
}
