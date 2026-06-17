#include "block.h"
#include "bitpacker.h"
#include "config.h"

// Upper limit for metatile ID, collision, and elevation masks. Used externally.
const uint16_t Block::maxValue = 0xFFFF;

// In this project's format the map.bin word packs the metatile id (bits 0-9), the
// location (bits 10-11) and the biome (bits 12-15). Collision and elevation are stored
// in a separate 8-bit attribute byte (attributes.bin): collision in bit 0, elevation in
// bits 1-7.
static BitPacker bitsMetatileId = BitPacker(0x03FF);
static BitPacker bitsLocation = BitPacker(0x0C00);
static BitPacker bitsBiome = BitPacker(0xF000);
static BitPacker bitsCollision = BitPacker(0x01);
static BitPacker bitsElevation = BitPacker(0xFE);

Block::Block() :
    m_metatileId(0),
    m_collision(0),
    m_elevation(0),
    m_location(0),
    m_biome(0)
{  }

Block::Block(uint16_t metatileId, uint16_t collision, uint16_t elevation) :
    m_metatileId(metatileId),
    m_collision(collision),
    m_elevation(elevation),
    m_location(0),
    m_biome(0)
{  }

// Constructs a block from a raw 16-bit map.bin word, unpacking the metatile id, location
// and biome. Collision and elevation come from the separate attribute byte (see setAttributes).
Block::Block(uint16_t data) :
    m_metatileId(bitsMetatileId.unpack(data)),
    m_collision(0),
    m_elevation(0),
    m_location(bitsLocation.unpack(data)),
    m_biome(bitsBiome.unpack(data))
{  }

Block::Block(const Block &other) :
    m_metatileId(other.m_metatileId),
    m_collision(other.m_collision),
    m_elevation(other.m_elevation),
    m_location(other.m_location),
    m_biome(other.m_biome)
{  }

Block &Block::operator=(const Block &other) {
    m_metatileId = other.m_metatileId;
    m_collision = other.m_collision;
    m_elevation = other.m_elevation;
    m_location = other.m_location;
    m_biome = other.m_biome;
    return *this;
}

// The raw value stored in map.bin: metatile id, location and biome packed into one word.
uint16_t Block::rawValue() const {
    return static_cast<uint16_t>(bitsMetatileId.pack(m_metatileId)
                               | bitsLocation.pack(m_location)
                               | bitsBiome.pack(m_biome));
}

// The raw value stored in attributes.bin: collision and elevation in one byte.
uint8_t Block::attributesValue() const {
    return static_cast<uint8_t>(bitsCollision.pack(m_collision)
                              | bitsElevation.pack(m_elevation));
}

// Unpacks collision and elevation from an attributes.bin byte.
void Block::setAttributes(uint8_t data) {
    m_collision = bitsCollision.unpack(data);
    m_elevation = bitsElevation.unpack(data);
}

void Block::setLayout() {
    bitsMetatileId.setMask(projectConfig.blockMetatileIdMask);
    bitsCollision.setMask(projectConfig.blockCollisionMask);
    bitsElevation.setMask(projectConfig.blockElevationMask);
    bitsLocation.setMask(projectConfig.blockLocationMask);
    bitsBiome.setMask(projectConfig.blockBiomeMask);
}

bool Block::operator ==(Block other) const {
    return (m_metatileId == other.m_metatileId)
        && (m_collision == other.m_collision)
        && (m_elevation == other.m_elevation)
        && (m_location == other.m_location)
        && (m_biome == other.m_biome);
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

void Block::setBiome(uint16_t biome) {
    m_biome = bitsBiome.clamp(biome);
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

uint16_t Block::getMaxBiome() {
    return bitsBiome.maxValue();
}
