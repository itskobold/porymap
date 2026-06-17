#include "block.h"
#include "bitpacker.h"
#include "config.h"

// Upper limit for metatile ID, collision, and elevation masks. Used externally.
const uint16_t Block::maxValue = 0xFFFF;

// In this project's format the metatile id occupies a full 16-bit word (map.bin),
// while collision, elevation and location are stored in a separate 8-bit attribute byte
// (attributes.bin): collision in bits 0-1, elevation in bits 2-5, location in bits 6-7.
static BitPacker bitsMetatileId = BitPacker(0xFFFF);
static BitPacker bitsCollision = BitPacker(0x03);
static BitPacker bitsElevation = BitPacker(0x3C);
static BitPacker bitsLocation = BitPacker(0xC0);

Block::Block() :
    m_metatileId(0),
    m_collision(0),
    m_elevation(0),
    m_location(0)
{  }

Block::Block(uint16_t metatileId, uint16_t collision, uint16_t elevation) :
    m_metatileId(metatileId),
    m_collision(collision),
    m_elevation(elevation),
    m_location(0)
{  }

// Constructs a block from a raw 16-bit map.bin word, which is entirely the metatile id.
// Collision, elevation and location come from the separate attribute byte (see setAttributes).
Block::Block(uint16_t data) :
    m_metatileId(bitsMetatileId.unpack(data)),
    m_collision(0),
    m_elevation(0),
    m_location(0)
{  }

Block::Block(const Block &other) :
    m_metatileId(other.m_metatileId),
    m_collision(other.m_collision),
    m_elevation(other.m_elevation),
    m_location(other.m_location)
{  }

Block &Block::operator=(const Block &other) {
    m_metatileId = other.m_metatileId;
    m_collision = other.m_collision;
    m_elevation = other.m_elevation;
    m_location = other.m_location;
    return *this;
}

// The raw value stored in map.bin: a full 16-bit metatile id.
uint16_t Block::rawValue() const {
    return bitsMetatileId.pack(m_metatileId);
}

// The raw value stored in attributes.bin: collision, elevation and location in one byte.
uint8_t Block::attributesValue() const {
    return static_cast<uint8_t>(bitsCollision.pack(m_collision)
                              | bitsElevation.pack(m_elevation)
                              | bitsLocation.pack(m_location));
}

// Unpacks collision, elevation and location from an attributes.bin byte.
void Block::setAttributes(uint8_t data) {
    m_collision = bitsCollision.unpack(data);
    m_elevation = bitsElevation.unpack(data);
    m_location = bitsLocation.unpack(data);
}

void Block::setLayout() {
    bitsMetatileId.setMask(projectConfig.blockMetatileIdMask);
    bitsCollision.setMask(projectConfig.blockCollisionMask);
    bitsElevation.setMask(projectConfig.blockElevationMask);
    bitsLocation.setMask(projectConfig.blockLocationMask);
}

bool Block::operator ==(Block other) const {
    return (m_metatileId == other.m_metatileId)
        && (m_collision == other.m_collision)
        && (m_elevation == other.m_elevation)
        && (m_location == other.m_location);
}

bool Block::operator !=(Block other) const {
    return !(operator ==(other));
}

void Block::setMetatileId(uint16_t metatileId) {
    m_metatileId = bitsMetatileId.clamp(metatileId);
}

void Block::setCollision(uint16_t collision) {
    m_collision = bitsCollision.clamp(collision);
}

void Block::setElevation(uint16_t elevation) {
    m_elevation = bitsElevation.clamp(elevation);
}

void Block::setLocation(uint16_t location) {
    m_location = bitsLocation.clamp(location);
}

uint16_t Block::getMaxMetatileId() {
    return bitsMetatileId.maxValue();
}

uint16_t Block::getMaxCollision() {
    return bitsCollision.maxValue();
}

uint16_t Block::getMaxElevation() {
    return bitsElevation.maxValue();
}

uint16_t Block::getMaxLocation() {
    return bitsLocation.maxValue();
}
