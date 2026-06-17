#include "blockdata.h"

QByteArray Blockdata::serialize() const {
    QByteArray data;
    for (const auto &block : *this) {
        uint16_t word = block.rawValue();
        data.append(static_cast<char>(word & 0xff));
        data.append(static_cast<char>((word >> 8) & 0xff));
    }
    return data;
}

QByteArray Blockdata::serializeAttributes() const {
    QByteArray data;
    for (const auto &block : *this)
        data.append(static_cast<char>(block.attributesValue()));
    return data;
}
