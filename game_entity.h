/*
 * game_entity.h
 * Abstract base class for all game entities.
 * Defines the interface for resetting state and checking game-over conditions.
 */

#pragma once

class IGameEntity {
public:
    virtual void reset() = 0;
    virtual bool isGameOver() const = 0;
    virtual ~IGameEntity() {}
};
