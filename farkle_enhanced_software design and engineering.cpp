/**
 * Farkle Game - Enhanced Version
 * Author: Evans Adonteng
 * Date: 25 March 2024
 * Version: 1.0
 *
 * Description:
 * This implementation of the Farkle game introduces optimized scoring logic,
 * improved game flow, and comprehensive documentation aimed at enhancing
 * readability, modularity, and maintainability. The enhancements demonstrate
 * the application of advanced programming concepts and algorithm optimization,
 * contributing to the overall software engineering and design quality.
 *
 * Each enhancement is designed to make the game more accessible and easier to
 * maintain, showcasing skills in software design and engineering relevant for
 * potential employers and collaborators.
 */

#include <iostream>
#include <vector>
#include <unordered_map>
#include <cstdlib>
#include <ctime>
#include <limits>

 // Constants to define scoring rules and game-winning criteria.
constexpr int SCORE_TO_ENTER = 1000;
constexpr int WINNING_SCORE = 10000;

// Struct to encapsulate player data, enhancing modularity and data encapsulation.
struct Player {
    int score = 0;
    bool isInGame = false;

    void updateScore(int roundScore) {
        score += roundScore;
        if (score >= SCORE_TO_ENTER && !isInGame) {
            isInGame = true;
        }
    }
};

std::vector<Player> players;

// Displays the game rules to the user, improving user experience and clarity.
void displayRules() {
    std::cout << "Welcome to Farkle! Here are the rules:\n"
        << "1. Roll six dice to start your turn.\n"
        << "2. Set aside scoring dice and roll the rest if you wish.\n"
        << "3. If no dice score, you Farkle and lose points for the turn.\n"
        << "4. First to " << WINNING_SCORE << " wins.\n\n";
}

// Function to roll a given number of dice, enhancing the game's interactivity.
std::vector<int> rollDice(int diceCount) {
    std::vector<int> rolls(diceCount);
    for (int& roll : rolls) {
        roll = rand() % 6 + 1; // Rolls between 1 and 6
    }
    return rolls;
}

// Calculates the score based on dice rolls, applying game scoring rules.
int calculateScore(const std::vector<int>& diceRolls) {
    std::unordered_map<int, int> diceCounts;
    for (int roll : diceRolls) {
        ++diceCounts[roll];
    }

    int score = 0;
    for (const auto& pair : diceCounts) {
        int die = pair.first;
        int count = pair.second;

        if (die == 1) {
            score += (count >= 3) ? 1000 + (count - 3) * 100 : count * 100;
        }
        else if (die == 5) {
            score += (count >= 3) ? 500 + (count - 3) * 50 : count * 50;
        }
        else if (count >= 3) {
            score += die * 100;
        }
    }
    return score;
}

// Asks the player if they wish to roll again, facilitating game flow and player decisions.
bool askRollAgain() {
    std::string input;
    std::cout << "Would you like to roll again? (yes/no): ";
    std::cin >> input;
    return input == "yes" || input == "y";
}

// Manages a single round of play for a given player, illustrating structured game flow.
void playRound(int playerIndex) {
    bool turnEnded = false;
    int roundScore = 0;

    while (!turnEnded) {
        auto rolls = rollDice(6); // Assume rolling all dice for simplicity
        int score = calculateScore(rolls);
        std::cout << "You rolled: ";
        for (int roll : rolls) std::cout << roll << " ";
        std::cout << " for a score of " << score << std::endl;

        if (score == 0) {
            std::cout << "Farkle! No points this turn.\n";
            turnEnded = true;
        }
        else {
            roundScore += score;
            std::cout << "Total score this turn: " << roundScore << std::endl;
            turnEnded = !askRollAgain();
        }
    }

    players[playerIndex].updateScore(roundScore);
    std::cout << "Player " << playerIndex + 1 << " total score: " << players[playerIndex].score << std::endl;
}

// Prepares the game for the given number of players, emphasizing the setup process.
void setupGame(int numPlayers) {
    srand(static_cast<unsigned int>(time(nullptr))); // Seed the random number generator
    players = std::vector<Player>(numPlayers);
}

// Orchestrates the overall game play, controlling the main game loop and player turns.
void playGame(int numPlayers) {
    setupGame(numPlayers);
    displayRules();

    int currentPlayerIndex = 0;
    while (true) {
        playRound(currentPlayerIndex);
        // Check if the current player has won the game by reaching the winning score
        if (players[currentPlayerIndex].score >= WINNING_SCORE) {
            std::cout << "Player " << currentPlayerIndex + 1 << " wins the game with " << players[currentPlayerIndex].score << " points!\n";
            break;
        }
        // Move to the next player for the subsequent round
        currentPlayerIndex = (currentPlayerIndex + 1) % numPlayers;
    }
}

int main() {
    std::cout << "Enter the number of players: ";
    int numPlayers;
    std::cin >> numPlayers;
    // Input validation for the number of players, ensuring at least 2 players are part of the game
    if (std::cin.fail() || numPlayers < 2) {
        std::cin.clear(); // Clears the error flag
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discards the rest of the line
        std::cout << "Invalid input. Please enter a valid number of players (2 or more)." << std::endl;
        return 1; // Exit the program due to invalid input
    }

    playGame(numPlayers);

    return 0;
}
