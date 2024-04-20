#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <cstdlib>
#include <ctime>

// Constants for game scoring
const int SCORE_TO_ENTER = 1000;
const int WINNING_SCORE = 10000;

// Function prototypes
void displayRulesFromFile(const std::string& filename);

// Player class definition
class Player {
public:
    int score;
    bool isInGame;

    Player() : score(0), isInGame(false) {}

    bool rollAgainDecision() {
        std::string decision;
        std::cout << "Roll again or hold? (roll/hold): ";
        std::cin >> decision;
        return decision == "roll";
    }
};

// Game class definition
class Game {
private:
    std::vector<Player> players;
    int currentPlayerIndex;

    // Rolls a specified number of dice and returns the result
    std::vector<int> rollDice(int diceLeft) {
        std::vector<int> result;
        for (int i = 0; i < diceLeft; ++i) {
            result.push_back(rand() % 6 + 1); // Rolls a die
        }
        return result;
    }

    // Calculates the score based on the roll result according to Farkle rules
    int calculateScore(std::vector<int>& rollResult) {
        // Count occurrences of each die value
        std::vector<int> counts(7, 0); // 7 elements for values 1-6
        for (int value : rollResult) {
            counts[value]++;
        }

        int score = 0;
        // Score for 1's and 5's
        score += counts[1] * 100 + counts[5] * 50;

        // Score for three of a kind
        for (int i = 2; i <= 6; ++i) {
            if (counts[i] >= 3) {
                score += (i == 1) ? 1000 : i * 100;
                counts[i] -= 3; // Remove the three of a kind
            }
        }

        // Remove scored dice from the roll result
        rollResult.erase(std::remove_if(rollResult.begin(), rollResult.end(),
            [&counts](int value) { return value == 1 || value == 5 || counts[value] >= 3; }),
            rollResult.end());

        return score;
    }

public:
    Game(int numPlayers) : currentPlayerIndex(0) {
        players = std::vector<Player>(numPlayers);
    }

    void play() {
        bool finalRoundTriggered = false;
        Player* gameWinner = nullptr;

        while (gameWinner == nullptr) {
            Player& currentPlayer = players[currentPlayerIndex];
            std::cout << "\nPlayer " << currentPlayerIndex + 1 << "'s turn." << std::endl;

            int turnScore = 0;
            std::vector<int> diceRoll;
            bool farkle = false;

            do {
                diceRoll = rollDice(6); // Always roll 6 dice
                std::cout << "Roll: ";
                for (int value : diceRoll) std::cout << value << " ";
                std::cout << std::endl;

                int rollScore = calculateScore(diceRoll);
                if (rollScore == 0) {
                    std::cout << "Farkle! No points this turn." << std::endl;
                    farkle = true;
                    break;
                }

                turnScore += rollScore;
                std::cout << "Current turn score: " << turnScore << std::endl;
            } while (currentPlayer.score + turnScore < SCORE_TO_ENTER || (currentPlayer.isInGame && currentPlayer.rollAgainDecision()));

            if (!farkle) {
                currentPlayer.score += turnScore;
                currentPlayer.isInGame = true;
            }

            std::cout << "Total score for Player " << currentPlayerIndex + 1 << ": " << currentPlayer.score << std::endl;

            if (currentPlayer.score >= WINNING_SCORE && gameWinner == nullptr) {
                gameWinner = &currentPlayer;
                finalRoundTriggered = true;
                std::cout << "Player " << currentPlayerIndex + 1 << " has reached " << WINNING_SCORE << " points. Final round for other players." << std::endl;
            }

            currentPlayerIndex = (currentPlayerIndex + 1) % players.size();

            // If the final round has been triggered, and we are back to the gameWinner, end the game.
            if (finalRoundTriggered && gameWinner == &players[currentPlayerIndex]) {
                break;
            }
        }

        // Determine the winner
        int winnerIndex = std::distance(players.begin(), std::max_element(players.begin(), players.end(),
            [](const Player& a, const Player& b) { return a.score < b.score; }));
        std::cout << "Game Over. Player " << winnerIndex + 1 << " wins with " << players[winnerIndex].score << " points." << std::endl;
    }
};

int main() {
    srand(static_cast<unsigned int>(time(nullptr))); // Seed random number generator

    displayRulesFromFile("farkle_rules.txt");

    int numPlayers;
    std::cout << "Enter the number of players: ";
    std::cin >> numPlayers;
    while (numPlayers < 2) {
        std::cout << "Please enter a valid number of players (at least 2): ";
        std::cin >> numPlayers;
    }

    Game farkleGame(numPlayers);
    farkleGame.play();

    return 0;
}

void displayRulesFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Error opening rules file." << std::endl;
        return;
    }

    std::string line;
    while (getline(file, line)) {
        std::cout << line << std::endl;
    }
    file.close();
}