/*
 * score_manager.cpp
 * Implements ScoreTracker and ScoreManager — score tracking and best score persistence.
 */

#include "score_manager.h"

ScoreTracker::ScoreTracker() : m_currentScore(0) {}

ScoreTracker::ScoreTracker(int initialScore) : m_currentScore(initialScore) {}

ScoreTracker::~ScoreTracker() {}

// Adds points to the current score
void ScoreTracker::addPoints(int points) {
    m_currentScore += points;
}

// Returns the current score
int ScoreTracker::getCurrentScore() const {
    return m_currentScore;
}

int ScoreManager::s_bestScore = 0;

// Initializes score manager with zero score
ScoreManager::ScoreManager() : ScoreTracker(0) {}

// Initializes score manager with a given starting score
ScoreManager::ScoreManager(int initial) : ScoreTracker(initial) {}

ScoreManager::~ScoreManager() {}

// Adds points and updates best score if current exceeds it
void ScoreManager::addPoints(int points) {
    ScoreTracker::addPoints(points);
    updateBestScore(m_currentScore);
}

// Resets the current score to zero
void ScoreManager::reset() {
    m_currentScore = 0;
}

// Returns the all-time best score
int ScoreManager::getBestScore() {
    return s_bestScore;
}

// Updates best score if the given score is higher
void ScoreManager::updateBestScore(int score) {
    if (score > s_bestScore)
        s_bestScore = score;
}
