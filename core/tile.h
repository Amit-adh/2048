/*
 * tile.h
 * Declares the Tile class representing a single cell on the game board.
 * Stores an integer value and provides comparison/merge operations.
 */

#pragma once

class Tile {
private:
    int m_value;

public:
    Tile();
    Tile(int value);
    Tile(const Tile& other);
    ~Tile();

    int getValue() const;
    void setValue(int value);
    bool isEmpty() const;

    Tile& operator=(const Tile& other);
    bool operator==(const Tile& other) const;
    bool operator!=(const Tile& other) const;
    Tile operator+(const Tile& other) const;

    static bool compareByValue(Tile t1, Tile t2);
    static bool compareByRef(const Tile& t1, const Tile& t2);
};
