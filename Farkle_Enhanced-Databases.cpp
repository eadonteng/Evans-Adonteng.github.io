// Farkle Game - Enhanced Version
 // Author: Evans Adonteng
 // Date: 06 April 2024
 // Version: 3.0

// Farkle Game with SQLite Database Integration for High Scores
//
// Description:
// This C++ console application enhances the traditional dice game, Farkle, by integrating SQLite database functionality
// to manage high scores. This enhancement not only demonstrates proficiency in database usage within software applications
// but also enriches the user experience by providing a persistent record of high scores across game sessions.
//
// Core Gameplay:
// - Farkle is a dice game where players roll six dice to accumulate points, aiming to be the first to reach 10,000 points.
// - The game requires strategic decision-making on risking additional rolls for more points versus banking current points to avoid losing them.
//
// Database Integration:
// - Utilizes SQLite, a lightweight, disk-based database, for simplicity and direct integration without requiring a separate server process.
// - Manages a 'HighScores' table within the game to store and retrieve player names and their scores, allowing for persistence of records.
//
// Key Functionalities:
// 1. Database Initialization (`initializeDatabase`):
//    - Prepares the database and high scores table at the start, ensuring the game is ready to record scores.
// 2. Inserting High Scores (`insertHighScore`):
//    - Inserts the winner's name and score into the database after each game, demonstrating data insertion.
// 3. Displaying High Scores (`displayHighScores`):
//    - Allows viewing the top 10 high scores, showcasing data querying and sorting within a database context.
//
// Enhancements Overview:
// - Adds persistence to the game by maintaining high scores between sessions, introducing competitive longevity.
// - Adheres to software engineering practices like modularity, error handling, and enhancing user experience through clear instructions and feedback.
//


#include <iostream>
#include <vector>
#include <unordered_map>
#include <cstdlib>
#include <ctime>
#include <algorithm> 
#include <string>
#include <limits>
#include "sqlite3.h" // Include SQLite for database operations

constexpr int SCORE_TO_ENTER = 1000;// Score needed to start accumulating points
constexpr int WINNING_SCORE = 10000; // Score needed to win the game

// Player structure to hold player details
struct Player {
    std::string name;
    int score = 0;
    bool isInGame = false; // Tracks if the player has entered the game based on SCORE_TO_ENTER

    // Method to update player's score
    void updateScore(int roundScore) {
        score += roundScore;
        if (score >= SCORE_TO_ENTER && !isInGame) {
            isInGame = true; // Player starts accumulating points after reaching SCORE_TO_ENTER
        }
    }
};


std::vector<Player> players; // Player starts accumulating points after reaching SCORE_TO_ENTER

// Function to initialize the database and create the HighScores table if it doesn't exist
void initializeDatabase() {
    sqlite3* db;
    char* errMsg = nullptr;
    int rc = sqlite3_open("farkle.db", &db); // Open or create the database file
    if (rc != SQLITE_OK) {
        std::cerr << "Error opening database: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return;
    }

    // SQL command to create the HighScores table
    const char* sql =
        "CREATE TABLE IF NOT EXISTS HighScores ("
        "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
        "Name TEXT NOT NULL, "
        "Score INTEGER NOT NULL);";

    // Execute the SQL command
    rc = sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }

    sqlite3_close(db); // Close the database connection
}

// Function to insert a high score into the database
void insertHighScore(const std::string& name, int score) {
    sqlite3* db;
    char* errMsg = nullptr;
    sqlite3_open("farkle.db", &db); // Open the database

    // SQL command to insert the high score
    std::string sql = "INSERT INTO HighScores (Name, Score) VALUES ('" + name + "', " + std::to_string(score) + ");";
    if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }

    sqlite3_close(db);
}
// Function to display the top 10 high scores from the database
void displayHighScores() {
    sqlite3* db;
    sqlite3_stmt* stmt;
    sqlite3_open("farkle.db", &db);

    // SQL command to select the top 10 high scores
    const char* sql = "SELECT Name, Score FROM HighScores ORDER BY Score DESC LIMIT 10;";
    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

    std::cout << "\nTop High Scores:\n";
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        int score = sqlite3_column_int(stmt, 1);
        std::cout << name << " - " << score << std::endl;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

void displayRules() {
    std::cout << "\nWelcome to Farkle! Here are the rules:\n"
        << "1. Roll six dice to start your turn.\n"
        << "2. Set aside scoring dice and roll the rest if you wish.\n"
        << "3. If no dice score, you Farkle and lose points for the turn.\n"
        << "4. First to " << WINNING_SCORE << " wins.\n";
}

std::vector<int> rollDice(int diceCount) {
    std::vector<int> rolls(diceCount);
    std::generate(rolls.begin(), rolls.end(), []() { return rand() % 6 + 1; });
    return rolls;
}

int calculateScore(const std::vector<int>& diceRolls) {
    // Simplified scoring logic for demonstration
    int score = 0;
    for (int roll : diceRolls) {
        if (roll == 1) score += 100;
        else if (roll == 5) score += 50;
    }
    return score;
}

bool askRollAgain() {
    std::string input;
    std::cout << "Would you like to roll again? (yes/no): ";
    std::cin >> input;
    return input == "yes" || input == "y";
}

void playRound(int playerIndex) {
    std::cout << "\n" << players[playerIndex].name << "'s turn.\n";
    bool turnEnded = false;
    int roundScore = 0;

    while (!turnEnded) {
        auto rolls = rollDice(6); // Example: Always rolling 6 dice for simplicity
        std::cout << "Rolled: ";
        for (int roll : rolls) std::cout << roll << " ";
        std::cout << "\n";

        int score = calculateScore(rolls);
        std::cout << "Score this roll: " << score << "\n";
        if (score == 0) {
            std::cout << "Farkle! No points this turn.\n";
            turnEnded = true;
        }
        else {
            roundScore += score;
            std::cout << "Total score this turn: " << roundScore << "\n";
            turnEnded = !askRollAgain();
        }
    }

    players[playerIndex].updateScore(roundScore);
    std::cout << players[playerIndex].name << "'s total score: " << players[playerIndex].score << "\n";
}

void setupGame(int numPlayers) {
    srand(static_cast<unsigned int>(time(nullptr))); // Seed the random number generator
    players.clear();

    for (int i = 0; i < numPlayers; ++i) {
        std::cout << "Enter name for player " << i + 1 << ": ";
        std::string name;
        std::cin >> name;
        players.emplace_back(Player{ name });
    }
}

void playGame(int numPlayers) {
    setupGame(numPlayers);
    displayRules();

    int currentPlayerIndex = 0;
    bool gameWon = false;
    while (!gameWon) {
        playRound(currentPlayerIndex);
        if (players[currentPlayerIndex].score >= WINNING_SCORE) {
            std::cout << players[currentPlayerIndex].name << " wins the game with " << players[currentPlayerIndex].score << " points!\n";
            insertHighScore(players[currentPlayerIndex].name, players[currentPlayerIndex].score);
            gameWon = true;
        }
        currentPlayerIndex = (currentPlayerIndex + 1) % numPlayers;
    }

    displayHighScores();
}

int main() {
    initializeDatabase(); // Ensure the database is ready before the game starts

    std::cout << "Enter the number of players: ";
    int numPlayers;
    std::cin >> numPlayers;
    if (numPlayers < 2) {
        std::cout << "Invalid input. Please enter a valid number of players (2 or more).\n";
        return 1;
    }

    playGame(numPlayers);

    return 0;
}
