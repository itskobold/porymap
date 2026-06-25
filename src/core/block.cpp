#include "block.h"
#include "bitpacker.h"
#include "config.h"

// Upper limit for metatile ID, collision, and elevation masks. Used externally.
const uint16_t Block::maxValue = 0xFFFF;

// In this project's format the map.bin word packs the metatile id (bits 0-9), the
// location (bits 10-11) and the biome (bits 12-15). The remaining per-tile data is stored
// in a separate 16-bit attribute value (attributes.bin, 2 bytes/tile): elevation in bits
// 0-7, cliff collision in bit 8, collision in bit 9, bgMaterial in bits 10-13. (These
// default masks are overridden by setLayout() from config.)
static BitPacker bitsMetatileId = BitPacker(0x03FF);
static BitPacker bitsLocation = BitPacker(0x0C00);
static BitPacker bitsBiome = BitPacker(0xF000);
static BitPacker bitsCollision = BitPacker(0x0200);
static BitPacker bitsCliffCollision = BitPacker(0x0100);
static BitPacker bitsElevation = BitPacker(0x00FF);
static BitPacker bitsBgMaterial = BitPacker(0x3C00);

Block::Block() :
    m_metatileId(0),
    m_collision(0),
    m_cliffCollision(0),
    m_elevation(0),
    m_location(0),
    m_biome(0),
    m_bgMaterial(0)
{  }

Block::Block(uint16_t metatileId, uint16_t collision, uint16_t elevation) :
    m_metatileId(metatileId),
    m_collision(collision),
    m_cliffCollision(0),
    m_elevation(elevation),
    m_location(0),
    m_biome(0),
    m_bgMaterial(0)
{  }

// Constructs a block from a raw 16-bit map.bin word, unpacking the metatile id, location
// and biome. Collision and elevation come from the separate attribute byte (see setAttributes).
Block::Block(uint16_t data) :
    m_metatileId(bitsMetatileId.unpack(data)),
    m_collision(0),
    m_cliffCollision(0),
    m_elevation(0),
    m_location(bitsLocation.unpack(data)),
    m_biome(bitsBiome.unpack(data)),
    m_bgMaterial(0)
{  }

Block::Block(const Block &other) :
    m_metatileId(other.m_metatileId),
    m_collision(other.m_collision),
    m_cliffCollision(other.m_cliffCollision),
    m_elevation(other.m_elevation),
    m_location(other.m_location),
    m_biome(other.m_biome),
    m_bgMaterial(other.m_bgMaterial)
{  }

Block &Block::operator=(const Block &other) {
    m_metatileId = other.m_metatileId;
    m_collision = other.m_collision;
    m_cliffCollision = other.m_cliffCollision;
    m_elevation = other.m_elevation;
    m_location = other.m_location;
    m_biome = other.m_biome;
    m_bgMaterial = other.m_bgMaterial;
    return *this;
}

// The raw value stored in map.bin: metatile id, location and biome packed into one word.
uint16_t Block::rawValue() const {
    return static_cast<uint16_t>(bitsMetatileId.pack(m_metatileId)
                               | bitsLocation.pack(m_location)
                               | bitsBiome.pack(m_biome));
}

// The raw value stored in attributes.bin: elevation, cliff collision, collision and
// bgMaterial packed into one 16-bit word.
uint16_t Block::attributesValue() const {
    return static_cast<uint16_t>(bitsCollision.pack(m_collision)
                               | bitsCliffCollision.pack(m_cliffCollision)
                               | bitsElevation.pack(m_elevation)
                               | bitsBgMaterial.pack(m_bgMaterial));
}

// Unpacks elevation, cliff collision, collision and bgMaterial from an attributes.bin word.
void Block::setAttributes(uint16_t data) {
    m_collision = bitsCollision.unpack(data);
    m_cliffCollision = bitsCliffCollision.unpack(data);
    m_elevation = bitsElevation.unpack(data);
    m_bgMaterial = bitsBgMaterial.unpack(data);
}

void Block::setLayout() {
    bitsMetatileId.setMask(projectConfig.blockMetatileIdMask);
    bitsCollision.setMask(projectConfig.blockCollisionMask);
    bitsCliffCollision.setMask(projectConfig.blockCliffCollisionMask);
    bitsElevation.setMask(projectConfig.blockElevationMask);
    bitsLocation.setMask(projectConfig.blockLocationMask);
    bitsBiome.setMask(projectConfig.blockBiomeMask);
    bitsBgMaterial.setMask(projectConfig.blockBgMaterialMask);
}

bool Block::operator ==(Block other) const {
    return (m_metatileId == other.m_metatileId)
        && (m_collision == other.m_collision)
        && (m_cliffCollision == other.m_cliffCollision)
        && (m_elevation == other.m_elevation)
        && (m_location == other.m_location)
        && (m_biome == other.m_biome)
        && (m_bgMaterial == other.m_bgMaterial);
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

void Block::setCliffCollision(uint16_t cliffCollision) {
    m_cliffCollision = bitsCliffCollision.clamp(cliffCollision);
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

void Block::setBgMaterial(uint16_t bgMaterial) {
    m_bgMaterial = bitsBgMaterial.clamp(bgMaterial);
}

uint16_t Block::getMaxMetatileId() {
    return bitsMetatileId.maxValue();
}

uint16_t Block::getMaxCollision() {
    return bitsCollision.maxValue();
}

uint16_t Block::getMaxCliffCollision() {
    return bitsCliffCollision.maxValue();
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

uint16_t Block::getMaxBgMaterial() {
    return bitsBgMaterial.maxValue();
}
