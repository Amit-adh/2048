/*
 * tile.cpp
 * Implements the Tile class — constructors, getters/setters, and operators.
 */

#include "tile.h"

// Default constructor: creates an empty tile with value 0
Tile::Tile() : m_value(0) {}

// Parameterized constructor: creates a tile with the given value
Tile::Tile(int value) : m_value(value) {}

// Copy constructor: creates a tile by copying another
Tile::Tile(const Tile& other) : m_value(other.m_value) {}

// Destructor
Tile::~Tile() {}

// Returns the tile's numeric value
int Tile::getValue() const {
    return this->m_value;
}

// Sets the tile's numeric value
void Tile::setValue(int value) {
    this->m_value = value;
}

// Returns true if the tile has no value (empty cell)
bool Tile::isEmpty() const {
    return this->m_value == 0;
}

// Assignment operator: copies value from another tile
Tile& Tile::operator=(const Tile& other) {
    if (this != &other) {
        this->m_value = other.m_value;
    }
    return *this;
}

// Equality operator: compares tile values
bool Tile::operator==(const Tile& other) const {
    return m_value == other.m_value;
}

// Inequality operator
bool Tile::operator!=(const Tile& other) const {
    return !(*this == other);
}

// Addition operator: merges two tiles by summing their values
Tile Tile::operator+(const Tile& other) const {
    return Tile(m_value + other.m_value);
}

// Compares two tiles passed by value (copies are made)
bool Tile::compareByValue(Tile t1, Tile t2) {
    return t1.m_value == t2.m_value;
}

// Compares two tiles passed by const reference (no copies)
bool Tile::compareByRef(const Tile& t1, const Tile& t2) {
    return t1.m_value == t2.m_value;
}
