#include "pch.h"

#include "destination.h"

std::ostream & operator << (std::ostream &out, const Destination &dest) {
    out << dest.name << " " << dest.destinationFolder << " " << dest.key;
    return out;
}
