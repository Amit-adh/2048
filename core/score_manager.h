/*
 * score_manager.h
 * Declares ScoreTracker (abstract base) and ScoreManager (derived).
 * Handles current score, best score tracking, and score reset operations.
 */

#pragma once

class ScoreTracker {
protected:
    int m_currentScore;

public:
    ScoreTracker();
    ScoreTracker(int initialScore);
    virtual ~ScoreTracker();

    virtual void addPoints(int points);
    virtual void reset() = 0;
    int getCurrentScore() const;
};

class ScoreManager : public ScoreTracker {
private:
    static int s_bestScore;

public:
    ScoreManager();
    ScoreManager(int initial);
    ~ScoreManager() override;

    void addPoints(int points) override;
    void reset() override;

    static int getBestScore();
    static void updateBestScore(int score);
};
