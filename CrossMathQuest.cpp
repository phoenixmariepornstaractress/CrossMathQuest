#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <chrono>
#include <fstream>
#include <string>
#include <algorithm>
#include <map>

using namespace std;
using namespace std::chrono;

// Structure to hold user information
struct User {
    string username;
    string password;
    int highScore;
};

// Structure to hold achievement information
struct Achievement {
    string name;
    string description;
    bool achieved;
};

// Function to print the grid
void printGrid(const vector<vector<int>>& grid) {
    for (const auto& row : grid) {
        for (int cell : row) {
            if (cell == -1) {
                cout << " _ ";
            } else {
                cout << " " << cell << " ";
            }
        }
        cout << endl;
    }
}

// Function to check if the current grid satisfies the equations
bool checkSolution(const vector<vector<int>>& grid, const vector<int>& rowSums, const vector<int>& colSums) {
    for (size_t i = 0; i < grid.size(); ++i) {
        int rowSum = 0;
        for (int cell : grid[i]) {
            rowSum += cell;
        }
        if (rowSum != rowSums[i]) {
            return false;
        }
    }

    for (size_t j = 0; j < grid[0].size(); ++j) {
        int colSum = 0;
        for (size_t i = 0; i < grid.size(); ++i) {
            colSum += grid[i][j];
        }
        if (colSum != colSums[j]) {
            return false;
        }
    }

    return true;
}

// Function to generate a random grid
vector<vector<int>> generateGrid(int size, int maxValue) {
    vector<vector<int>> grid(size, vector<int>(size, -1));

    srand(time(0));
    for (size_t i = 0; i < size; ++i) {
        for (size_t j = 0; j < size; ++j) {
            grid[i][j] = rand() % (maxValue + 1);
        }
    }

    return grid;
}

// Function to generate sums for rows and columns from a solution grid
pair<vector<int>, vector<int>> generateSumsFromSolution(const vector<vector<int>>& solution) {
    int gridSize = solution.size();
    vector<int> rowSums(gridSize, 0);
    vector<int> colSums(gridSize, 0);

    for (int i = 0; i < gridSize; ++i) {
        for (int j = 0; j < gridSize; ++j) {
            rowSums[i] += solution[i][j];
            colSums[j] += solution[i][j];
        }
    }

    return {rowSums, colSums};
}

// Function to get user input safely
void getUserInput(int& x, int& y, int& value, int gridSize, int maxValue) {
    while (true) {
        cout << "Enter row (0-" << gridSize - 1 << "), column (0-" << gridSize - 1 << "), and value (0-" << maxValue << "): ";
        cin >> x >> y >> value;

        if (cin.fail() || x < 0 || x >= gridSize || y < 0 || y >= gridSize || value < 0 || value > maxValue) {
            cout << "Invalid input. Please try again." << endl;
            cin.clear(); // clear the error flag
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // discard invalid input
        } else {
            break;
        }
    }
}

// Function to give a hint by revealing a cell
void giveHint(vector<vector<int>>& grid, const vector<vector<int>>& solution) {
    for (size_t i = 0; i < grid.size(); ++i) {
        for (size_t j = 0; j < grid[0].size(); ++j) {
            if (grid[i][j] == -1) {
                grid[i][j] = solution[i][j];
                return;
            }
        }
    }
}

// Function to save the game state to a file
void saveGame(const vector<vector<int>>& grid, const vector<int>& rowSums, const vector<int>& colSums, const string& filename) {
    ofstream outFile(filename);
    for (const auto& row : grid) {
        for (int cell : row) {
            outFile << cell << " ";
        }
        outFile << endl;
    }
    for (int sum : rowSums) {
        outFile << sum << " ";
    }
    outFile << endl;
    for (int sum : colSums) {
        outFile << sum << " ";
    }
    outFile << endl;
}

// Function to load the game state from a file
void loadGame(vector<vector<int>>& grid, vector<int>& rowSums, vector<int>& colSums, const string& filename) {
    ifstream inFile(filename);
    for (auto& row : grid) {
        for (int& cell : row) {
            inFile >> cell;
        }
    }
    for (int& sum : rowSums) {
        inFile >> sum;
    }
    for (int& sum : colSums) {
        inFile >> sum;
    }
}

// Function to update the leaderboard with a new score
void updateLeaderboard(int score, const string& filename) {
    vector<int> leaderboard;
    ifstream inFile(filename);
    int previousScore;
    while (inFile >> previousScore) {
        leaderboard.push_back(previousScore);
    }
    inFile.close();
    
    leaderboard.push_back(score);
    sort(leaderboard.rbegin(), leaderboard.rend());
    ofstream outFile(filename);
    for (int score : leaderboard) {
        outFile << score << endl;
    }
}

// Function to display the leaderboard
void displayLeaderboard(const string& filename) {
    ifstream inFile(filename);
    int score;
    cout << "Leaderboard:" << endl;
    while (inFile >> score) {
        cout << score << endl;
    }
}

// Function to load users from a file
map<string, User> loadUsers(const string& filename) {
    map<string, User> users;
    ifstream inFile(filename);
    string username, password;
    int highScore;
    while (inFile >> username >> password >> highScore) {
        users[username] = {username, password, highScore};
    }
    return users;
}

// Function to save users to a file
void saveUsers(const map<string, User>& users, const string& filename) {
    ofstream outFile(filename);
    for (const auto& [username, user] : users) {
        outFile << user.username << " " << user.password << " " << user.highScore << endl;
    }
}

// Function to register a new user
User registerUser(map<string, User>& users, const string& filename) {
    string username, password;
    cout << "Register a new user" << endl;
    cout << "Enter username: ";
    cin >> username;
    cout << "Enter password: ";
    cin >> password;

    if (users.find(username) != users.end()) {
        cout << "Username already exists. Please try again." << endl;
        return registerUser(users, filename);
    }

    User newUser = {username, password, 0};
    users[username] = newUser;
    saveUsers(users, filename);
    return newUser;
}

// Function to log in an existing user
User loginUser(const map<string, User>& users) {
    string username, password;
    cout << "Log in" << endl;
    cout << "Enter username: ";
    cin >> username;
    cout << "Enter password: ";
    cin >> password;

    auto it = users.find(username);
    if (it != users.end() && it->second.password == password) {
        return it->second;
    } else {
        cout << "Invalid username or password. Please try again." << endl;
        return loginUser(users);
    }
}

// Function to initialize achievements
vector<Achievement> initializeAchievements() {
    return {
        {"First Puzzle", "Complete your first puzzle", false},
        {"No Hints", "Complete a puzzle without using hints", false},
        {"Speed Solver", "Complete a puzzle in under 2 minutes", false},
        {"High Scorer", "Achieve a score of over 1000", false}
    };
}

// Function to check and unlock achievements
void checkAchievements(vector<Achievement>& achievements, int score, int hintsUsed, int duration) {
    for (auto& achievement : achievements) {
        if (achievement.name == "First Puzzle" && !achievement.achieved) {
            achievement.achieved = true;
            cout << "Achievement unlocked: " << achievement.name << endl;
        } else if (achievement.name == "No Hints" && hintsUsed == 0 && !achievement.achieved) {
            achievement.achieved = true;
            cout << "Achievement unlocked: " << achievement.name << endl;
        } else if (achievement.name == "Speed Solver" && duration < 120 && !achievement.achieved) {
            achievement.achieved = true;
            cout << "Achievement unlocked: " << achievement.name << endl;
        } else if (achievement.name == "High Scorer" && score > 1000 && !achievement.achieved) {
            achievement.achieved = true;
            cout << "Achievement unlocked: " << achievement.name << endl;
        }
    }
}

int main() {
    const string usersFilename = "users.txt";
    map<string, User> users = loadUsers(usersFilename);
    vector<Achievement> achievements = initializeAchievements();

    cout << "Welcome to Crossmath!" << endl;
    cout << "1. Register" << endl;
    cout << "2. Log in" << endl;

    int choice;
    cin >> choice;

    User currentUser;
    if (choice == 1) {
        currentUser = registerUser(users, usersFilename);
    } else if (choice == 2) {
        currentUser = loginUser(users);
    } else {
        cout << "Invalid choice. Exiting." << endl;
        return 1;
    }

    cout << "Welcome, " << currentUser.username << "!" << endl;

    const int gridSize = 3;
    const int maxValue = 9;

    vector<vector<int>> solution = generateGrid(gridSize, maxValue);
    auto [rowSums, colSums] = generateSumsFromSolution(solution);
    vector<vector<int>> grid(gridSize, vector<int>(gridSize, -1));

    printGrid(grid);
    cout << "Row sums: ";
    for (int sum : rowSums) {
        cout << sum << " ";
    }
    cout << endl;

    cout << "Column sums: ";
    for (int sum : colSums) {
        cout << sum << " ";
    }
    cout << endl;

    int hintsUsed = 0;
    auto startTime = steady_clock::now();
    while (true) {
        int x, y, value;
        getUserInput(x, y, value, gridSize, maxValue);
        grid[x][y] = value;
        printGrid(grid);

        if (checkSolution(grid, rowSums, colSums)) {
            cout << "Congratulations! You solved the puzzle!" << endl;
            break;
        }

        char hintChoice;
        cout << "Do you want a hint? (y/n): ";
        cin >> hintChoice;
        if (hintChoice == 'y') {
            giveHint(grid, solution);
            hintsUsed++;
            printGrid(grid);
        }
    }

    auto endTime = steady_clock::now();
    int duration = duration_cast<seconds>(endTime - startTime).count();
    int score = 1000 - duration - hintsUsed * 50;

    checkAchievements(achievements, score, hintsUsed, duration);

    if (score > currentUser.highScore) {
        cout << "New high score!" << endl;
        currentUser.highScore = score;
        users[currentUser.username] = currentUser;
        saveUsers(users, usersFilename);
    }

    updateLeaderboard(score, "leaderboard.txt");
    displayLeaderboard("leaderboard.txt");

    return 0;
}
