#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <algorithm>
#include <map>

class CubeGame
{
public:
    CubeGame() : sumValidGames(0)
    {};
    void calcSumValidGames();
    unsigned long int getValidGamesSumValue() const {
        return sumValidGames;
    };

private:
    int getGameIDFromLine(std::string& line);
    void getResultsFromGame(std::string& line);
    void appendGameCubeNumberColor(std::string& line);
    bool isGameValid() const;

    std::ifstream inputFile{"input.txt"};
    // std::ifstream inputFile{"test.txt"};
    unsigned long int sumValidGames;
    static constexpr int NUM_RED_CUBES = 12;
    static constexpr int NUM_GREEN_CUBES = 13;
    static constexpr int NUM_BLUE_CUBES = 14;
    std::vector<std::string> cubeColors = {
        "red", "green", "blue"
    };
    std::map<std::string, std::vector<int>> gameResultsMap = 
    {
        {"red", {}},
        {"green", {}},
        {"blue", {}}
    };
};

/**
 * @brief Obtain the game ID from the line & remove it from the line
 * 
 * @param line 
 * @return int game ID
 */
int CubeGame::getGameIDFromLine(std::string& line)
{
    std::string endGameIDDelimiter = ":";
    std::string startGameIDDelimiter = " ";
    size_t endPos = line.find(endGameIDDelimiter);
    size_t startPos = line.find(startGameIDDelimiter);
    // std::cout << "found ':' at position " << endPos << " in line: " << line << "\n";
    // std::cout << "found ' ' at position " << startPos << " in line: " << line << "\n";
    std::string asciiString{line, startPos+1, endPos-startPos}; //copy the numerical digits only
    // std::cout << "ascii string " << asciiString << "\n";
    int number = std::stoi(asciiString); 
    // std::cout << "The converted integer is: " << number << std::endl;
    line.erase(0, endPos + endGameIDDelimiter.length());
    // std::cout << "Game XXX: erased\n" << line << "\n";
    return number;
}

/**
 * @brief Get the Substrings From String object
 * 
 * @param delimiter string of single characters to delimit line by
 * @param line 
 * @return std::vector<std::string> vector of substrings after delimiting
 */
std::vector<std::string> getSubstringFromString(std::string delimiter, std::string& line)
{
    std::size_t prev = 0, pos;
    std::vector<std::string> delimitedSubstrings = {};

    while ((pos = line.find_first_of(delimiter, prev)) != std::string::npos)
    {
        if (pos > prev)
            delimitedSubstrings.push_back(line.substr(prev, pos-prev));
        prev = pos+1;
    }
    if (prev < line.length())
        delimitedSubstrings.push_back(line.substr(prev, std::string::npos));

    return delimitedSubstrings;
}

/**
 * @brief Append appropriate number to the vector in map of colors
 * 
 * @param line 
 */
void CubeGame::getResultsFromGame(std::string& line)
{
    /* Go through line & create sub strings delimited on ';' 
    to get each round's results for this game */
    auto gameResults = getSubstringFromString(";", line);

    if(gameResults.size() > 0)
    {
        // std::cout << "there are: " << gameResults.size() << " rounds in this game\n";

        /* Parse through this round's result and append to appropriate 
        vector the number of cubes drawn for each color */
        /* further break down the round result into sub-strings (for each color) */
        // std::cout << "LINE:" << line << "\n";
        auto drawsFromGame = getSubstringFromString(",;", line);

        // std::cout << "draws From round: " << drawsFromGame.size() << "\n";
        for(auto draw : drawsFromGame)
        {
            draw.erase(remove(draw.begin(), draw.end(), ' '), draw.end());
            std::cout << "no whitespace substring: " << draw << "\n";
            appendGameCubeNumberColor(draw);
        }
        // for (const auto &color : gameResultsMap)
        // {
        //     std::cout << "Color: " << color.first << "\ttimes drawn this round: " << color.second.size() << "\n";
        //     for (auto num : color.second)
        //     {
        //         std::cout << "Number drawn: " << num << "\n";
        //     }
        // }
    }
}

/**
 * @brief Parse through a single draw from round and append the number for that color to the vector in map
 * 
 * @param line 
 */
void CubeGame::appendGameCubeNumberColor(std::string& line)
{
    //check if there's a match for all colors
    std::size_t prev = 0, pos = 0;
    while (pos != std::string::npos)
    {
        for(auto colorItr = cubeColors.begin(); colorItr != cubeColors.end(); colorItr++)
        {
            // std::cout << "color: " << *colorItr << "\n";
            pos = line.find(*colorItr, prev);
            // std::cout << "start search at prev: " << prev << "\tfound pos at: " << pos << "\n";

            if (pos == std::string::npos) //color is not in this line, move onto next color
            {
                // std::cout << "COLOR NOT FOUND\n";
                pos = prev;
            }
            else if (pos > prev) {
                // std::cout << "pos: " << pos << "\tprev: " << prev << "\n";
                std::string asciiString{line, prev, pos-prev}; //only copy the numerical digits
                // std::cout << "ascii string: " << asciiString << "\tLength: " << asciiString.length() << "\n";
                int number = std::stoi(asciiString);
                if(gameResultsMap.count(*colorItr) > 0) { //key exists for map, append number of cubes of color to vector
                    gameResultsMap[*colorItr].push_back(number);
                    // std::cout << "appending to map color: " << *colorItr << " drew: " << number << "\n";
                }
                return;
            }

            if(colorItr == cubeColors.begin() + (cubeColors.size() - 1)) {
                // std::cout << "SEARCHED LAST COLOR. NO COLORS IN THIS ENTRY\n";
                return;
            }
        }
    }
}

/**
 * @brief Check max number of cubes drawn for each color and determine if it is possible or not
 * 
 * @return true game is valid, we should add game ID to total
 * @return false game is invalid, do not add game ID to total
 */
bool CubeGame::isGameValid() const
{
    bool isValid = true;
    for(auto color : gameResultsMap)
    {
        auto max = *max_element(color.second.begin(), color.second.end());
        // std::cout << "Max value: " << max << " for color: " << color.first << "\n";
        if(color.first == "red") {
            isValid &= max <= NUM_RED_CUBES;
        }
        else if (color.first == "green") {
            isValid &= max <= NUM_GREEN_CUBES;
        }
        else if (color.first == "blue") {
            isValid &= max <= NUM_BLUE_CUBES;
        }
        else
        {
            std::cout << "NO COLORS MAX!!!\n";
            isValid = false;
            break;
        }
    }
    return isValid;
}

/**
 * @brief Calculate the total sum of valid Game ID values for the input file
 * 
 */
void CubeGame::calcSumValidGames()
{
    std::string line;
    while (std::getline(inputFile, line))
    {
        std::cout << line << "\n";

        auto gameID = getGameIDFromLine(line);
        // std::cout << "ID: " << gameID << "\n";

        /* Get each 'sub string' to obtain the number for each color to check validity of report */
        getResultsFromGame(line);

// FOR PART 2
    unsigned long int powerSet = 1;
    for(auto color : gameResultsMap)
    {
        auto max = *max_element(color.second.begin(), color.second.end());
        // std::cout << "Max value: " << max << " for color: " << color.first << "\n";
        if(color.first == "red") {
            powerSet *= max;
        }
        else if (color.first == "green") {
            powerSet *= max;
        }
        else if (color.first == "blue") {
            powerSet *= max;
        }
        else
        {
            std::cout << "NO COLORS MAX!!!\n";
            break;
        }
    }
    sumValidGames += powerSet;
//FOR PART 1
        // auto isValid = isGameValid();
        // if(isValid){
        //     sumValidGames += gameID;
        // }
        for(auto& color : gameResultsMap) 
        {
            color.second.clear(); //reset vector for each game
            // std::cout << "vector size: " << color.second.size() << "\n";
        }
    }
}


int main() {
    CubeGame game;

    game.calcSumValidGames();

    auto validGamesSum = game.getValidGamesSumValue();
    std::cout << "FINAL VALUE: " << validGamesSum;

    return 0;
}