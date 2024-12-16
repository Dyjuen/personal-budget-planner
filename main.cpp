// ReSharper disable CppParameterMayBeConstPtrOrRef
#include <iostream>
#include <unordered_map>
#include <functional>
#include <string>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <iomanip>
#include <limits>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <cmath>

#pragma region Utility

// Converts a decimal to a percentage string, ensuring it stays within 0-100%
std::string formatToPercentage(double decimal) {
    double percentage = decimal * 100;
    if (percentage < 0) {
        percentage = 0;
    } else if (percentage > 100) {
        percentage = 100;
    }
    int intPercentage = static_cast<int>(percentage);
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << intPercentage << "%";
    return oss.str();
}

// Type alias for key-value pairs
using KeyValuePair = std::pair<std::string, std::string>;

// Prints key-value pairs with padding for alignment
void printWithPadding(const std::vector<KeyValuePair> &items) {
    size_t maxKeyLength = 0;
    for (const auto &item: items) {
        maxKeyLength = std::max(maxKeyLength, item.first.length());
    }
    for (const auto &item: items) {
        std::cout << std::left << std::setw(maxKeyLength + 2) << item.first
                  << item.second << std::endl;
    }
}

// Returns the current date in "YYYY-MM-DD" format
std::string getCurrentDate() {
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d");
    return oss.str();
}

// Parses a date string and returns the year and month
std::pair<int, int> parseYearMonth(const std::string &dateStr) {
    int year = std::stoi(dateStr.substr(0, 4));
    int month = std::stoi(dateStr.substr(5, 2));
    return {year, month};
}

// Global currency setting
const std::string &CURRENCY = "IDR";

// Formats a number as Indonesian Rupiah (IDR) with thousands separators
std::string formatIDR(double amount) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(0) << amount;
    std::string numStr = oss.str();
    bool isNegative = (amount < 0);
    int n = numStr.length();
    int insertPosition = n - 3;
    if (isNegative) {
        insertPosition -= 1;
    }
    while (insertPosition > 0) {
        numStr.insert(insertPosition, ".");
        insertPosition -= 3;
    }
    return "IDR " + numStr;
}

// Formats a currency amount based on the CURRENCY setting
std::string formatCurrency(double amount) {
    std::ostringstream oss;
    if (CURRENCY == "IDR") {
        oss << formatIDR(amount);
    } else if (CURRENCY == "USD") {
        oss.imbue(std::locale("en_US.UTF-8"));
        oss << "USD " << std::fixed << std::setprecision(2) << amount;
    } else {
        return "Unsupported currency";
    }
    return oss.str();
}

#pragma endregion Utility

#pragma region Model

// Enumeration for financial item types
enum class ItemType {
    Asset,
    Liability,
    Income,
    Expense
};

// Represents a financial item with various attributes
class FinancialItem {
private:
    std::string category;
    ItemType type;
    std::string name;
    double amount;
    std::string date;
    double probability;

public:
    // Constructor
    FinancialItem(ItemType type, std::string &name, std::string &category, double amount, std::string &date,
                  double probability)
        : type(type), name(name), category(category), amount(amount), date(date), probability(probability) {
    }

    // Displays the item details
    void display() const {
        std::string typeName = getTypeName();
        std::cout << "(" << date << ") " << name << " (" << category << "): " << formatCurrency(amount) << " (" <<
                  typeName << ") " << (probability * 100) << "%\n";
    }

    // Returns the type name as a string
    [[nodiscard]] std::string getTypeName() const {
        switch (type) {
            case ItemType::Asset: return "Asset";
            case ItemType::Liability: return "Liability";
            case ItemType::Income: return "Income";
            case ItemType::Expense: return "Expense";
        }
        return "";
    }

    // Serializes the item to a file
    void serialize(std::ofstream &out) const {
        out << getTypeName() << "," << name << "," << category << "," << amount << "," << date << "," << probability <<
            "\n";
    }

    // Getters for item attributes
    [[nodiscard]] std::string getName() const { return name; }
    [[nodiscard]] ItemType getType() const { return type; }
    [[nodiscard]] double getAmount() const { return amount; }
    [[nodiscard]] std::string getDate() const { return date; }
    [[nodiscard]] double getProbability() const { return probability; }
    [[nodiscard]] std::string getCategory() const { return category; }

    // Deserializes an item from a string
    static FinancialItem deserialize(const std::string &input) {
        std::istringstream in(input);
        std::string typeName, name, category, date, amount, probability;
        std::getline(in, typeName, ',');
        std::getline(in, name, ',');
        std::getline(in, category, ',');
        std::getline(in, amount, ',');
        std::getline(in, date, ',');
        std::getline(in, probability);

        ItemType type;
        if (typeName == "Asset") type = ItemType::Asset;
        else if (typeName == "Liability") type = ItemType::Liability;
        else if (typeName == "Income") type = ItemType::Income;
        else if (typeName == "Expense") type = ItemType::Expense;
        else throw std::invalid_argument("Invalid item type");

        return FinancialItem(type, name, category, std::stod(amount), date, std::stod(probability));
    }
};

// CSV header for serialization
const std::string &HEADER = "Type,Name,Category,Amount,Date,Probability";

// Serializes all financial items to a file
void serializeAllItems(const std::vector<FinancialItem> &items, const std::string &filename) {
    std::ofstream out(filename);
    out << HEADER << "\n";
    for (const auto &item: items) {
        item.serialize(out);
    }
}

// Deserializes all financial items from a file
void deserializeAllItems(std::vector<FinancialItem> &items, const std::string &filename) {
    std::ifstream in(filename);
    while (in.peek() != EOF) {
        std::string line = "";
        std::getline(in, line);
        if (line == HEADER) continue;
        items.push_back(FinancialItem::deserialize(line));
    }
}

// Manages the state of all financial items
struct GlobalState {
    std::vector<FinancialItem> items;

    GlobalState() {
        load();
    }

    // Sorts items by date
    void sortByDate() {
        std::sort(items.begin(), items.end(), [](const FinancialItem &a, const FinancialItem &b) {
            return a.getDate() < b.getDate();
        });
    }

    // Returns all items
    [[nodiscard]] std::vector<FinancialItem> getItems() const {
        return items;
    }

    // Returns items for the current month
    [[nodiscard]] std::vector<FinancialItem> getItemsThisMonth() const {
        std::vector<FinancialItem> itemsThisMonth;
        auto [currentYear, currentMonth] = parseYearMonth(getCurrentDate());

        for (const auto &item: items) {
            auto [year, month] = parseYearMonth(item.getDate());
            if (year == currentYear && month == currentMonth) {
                itemsThisMonth.push_back(item);
            }
        }
        return itemsThisMonth;
    }

    // Returns all items up to the end of the current month
    [[nodiscard]] std::vector<FinancialItem> getAllItemsBeforeEndOfMonth() const {
        std::vector<FinancialItem> itemsBeforeEndOfMonth;
        auto [currentYear, currentMonth] = parseYearMonth(getCurrentDate());

        for (const auto &item: items) {
            auto [year, month] = parseYearMonth(item.getDate());
            if (year < currentYear || (year == currentYear && month <= currentMonth)) {
                itemsBeforeEndOfMonth.push_back(item);
            }
        }
        return itemsBeforeEndOfMonth;
    }

    // Saves the state to disk
    void save() {
        this->sortByDate();
        try {
            serializeAllItems(items, "financial_items.csv");
            serializeAllItems(items, "financial_items_backup.csv");
        } catch (const std::exception &e) {
            std::cerr << "Error saving items: " << e.what() << std::endl;
        }
    }

    // Loads the state from disk
    void load() {
        try {
            items.clear();
            deserializeAllItems(items, "financial_items.csv");
            this->sortByDate();
        } catch (const std::exception &e) {
            std::cerr << "Error loading items: " << e.what() << std::endl;
        }
    }
};

// Global state instance
GlobalState globalState;

#pragma endregion Model

#pragma region CLIUtility

// Clears the console screen
void clearScreen() {
#ifdef _WIN32
    system("CLS");
#else
    system("clear");
#endif
}

// Saves the program state to disk
void saveProgram() {
    globalState.save();
}

// Loads the program state from disk
void loadProgram() {
    globalState.load();
}

// Exits the program after saving the state
void exitProgram() {
    std::cout << "Exiting program...\n";
    saveProgram();
    exit(0);
}

// Pauses execution until the user presses Enter
void waitToContinue() {
    std::cout << std::endl;
    std::cout << "Press Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
    std::cout << std::endl;
}

// Template function to get user input with a default value
template<typename T>
void getInput(const std::string &fieldName, T *variable, const T &defaultValue) {
    std::cout << "Enter " << fieldName << " [" << defaultValue << "]: ";
    std::string input;
    std::getline(std::cin, input);

    if (input.empty()) {
        *variable = defaultValue;
    } else {
        std::istringstream(input) >> *variable;
    }
}

// Displays a menu and executes the selected option
void displayMenu(const std::vector<std::pair<std::string, std::function<void()> > > &menu) {
    std::string choice;
    while (true) {
        std::cout << "\n-------- Main Menu --------\n";
        for (size_t i = 0; i < menu.size(); ++i) {
            std::cout << i + 1 << ". " << menu[i].first << "\n";
        }

        getInput("choice", &choice, std::string(""));

        try {
            int index = std::stoi(choice) - 1;
            if (index >= 0 && index < static_cast<int>(menu.size())) {
                clearScreen();
                menu[index].second();
                break;
            } else {
                std::cout << "Invalid choice. Please try again.\n";
            }
        } catch (const std::exception &) {
            std::cout << "Invalid input. Please enter a number.\n";
        }
        waitToContinue();
        clearScreen();
    }
    waitToContinue();
}

#pragma endregion CLIUtility

#pragma region Application

// Calculates the financial outcome based on item occurrences
double calculateOutcome(const std::vector<FinancialItem> &items, const std::vector<bool> &occurrence) {
    double totalAssets = 0.0;
    for (size_t i = 0; i < items.size(); ++i) {
        if (occurrence[i]) {
            if (items[i].getType() == ItemType::Expense || items[i].getType() == ItemType::Liability) {
                totalAssets -= items[i].getAmount();
            } else {
                totalAssets += items[i].getAmount();
            }
        }
    }
    return totalAssets;
}

// Calculates the probability of a scenario based on item occurrences
double calculateProbability(const std::vector<FinancialItem> &items, const std::vector<bool> &occurrence) {
    double probability = 1.0;
    for (size_t i = 0; i < items.size(); ++i) {
        if (occurrence[i]) {
            probability *= items[i].getProbability();
        } else {
            probability *= (1 - items[i].getProbability());
        }
    }
    return probability;
}

// Evaluates all possible financial scenarios and returns a summary
std::vector<KeyValuePair> evaluateScenarios(const std::vector<FinancialItem> &items) {
    std::vector<FinancialItem> variableItems; // Items with probability less than 1
    double fixedAssets = 0.0; // Total assets from items with probability 1

    // Separate fixed and variable items
    for (const auto &item: items) {
        if (item.getProbability() == 0.0) {
            continue; // Skip impossible events
        }
        if (item.getProbability() == 1.0) {
            if (item.getType() == ItemType::Expense || item.getType() == ItemType::Liability) {
                fixedAssets -= item.getAmount(); // Subtract fixed expenses or liabilities
            } else {
                fixedAssets += item.getAmount(); // Add fixed assets or income
            }
        } else {
            variableItems.push_back(item);
        }
    }

    size_t n = variableItems.size();
    double maxAssets = -std::numeric_limits<double>::infinity();
    double minAssets = std::numeric_limits<double>::infinity();
    double maxProbability = 0.0;
    double minProbability = 1.0;
    double mostLikelyAssets = fixedAssets;
    double leastLikelyAssets = fixedAssets;

    // Iterate over all possible combinations of variable items

    // If n = 3, then 1 in binary is 0001.
    // Shifting left by 3 positions gives 1000, which is 8 in decimal, equivalent to 2^3
    for (size_t i = 0; i < (1 << n); ++i) { //     //  (from 0 to 2^n - 1):
        /**
        For n = 3, the combinations are:
                        financial item1  item2   item3
i = 0 (binary 000): occurrence = {false, false, false}
i = 1 (binary 001): occurrence = {true, false, false}
i = 2 (binary 010): occurrence = {false, true, false}
i = 3 (binary 011): occurrence = {true, true, false}
i = 4 (binary 100): occurrence = {false, false, true}
i = 5 (binary 101): occurrence = {true, false, true}
i = 6 (binary 110): occurrence = {false, true, true}
i = 7 (binary 111): occurrence = {true, true, true}
         */
        std::vector<bool> occurrence(n);
        for (size_t itemIndex = 0; itemIndex < n; ++itemIndex) {
            // Suppose i = 5 (which is 0101 in binary) and itemIndex = 2:
            /*
Step 1: Calculate 1 << itemIndex:
1 << 2 gives 0100.

Step 2: Perform i & (1 << itemIndex):
0101 (binary for i)
0100 (binary for 1 << 2)

Result: 0100 (since only the third bit is 1 in both)
Step 3: Check != 0:

The result 0100 is not zero, so isItemIncluded is true.
            */
            bool isItemIncluded = (i & (1 << itemIndex)) != 0; // Check if the bit is set
            occurrence[itemIndex] = isItemIncluded;
        }

        double currentAssets = fixedAssets + calculateOutcome(variableItems, occurrence);
        double currentProbability = calculateProbability(variableItems, occurrence);

        // Update best and worst asset scenarios
        if (currentAssets > maxAssets) maxAssets = currentAssets;
        if (currentAssets < minAssets) minAssets = currentAssets;

        // Update most and least likely scenarios
        if (currentProbability > maxProbability) {
            maxProbability = currentProbability;
            mostLikelyAssets = currentAssets;
        }
        if (currentProbability < minProbability) {
            minProbability = currentProbability;
            leastLikelyAssets = currentAssets;
        }
    }

    // Prepare results
    std::vector<KeyValuePair> executiveSummary;
    executiveSummary.emplace_back("", "");
    executiveSummary.emplace_back("Scenarios Evaluation of All Budget Items", "");
    executiveSummary.emplace_back("Projected Total Assets by The End", "");
    executiveSummary.emplace_back("Best Case Scenario", ": " + formatCurrency(maxAssets));
    executiveSummary.emplace_back("Worst Case Scenario", ": " + formatCurrency(minAssets));
    executiveSummary.emplace_back("Most Likely Outcome",
                                  ": " + formatCurrency(mostLikelyAssets) + " (" + formatToPercentage(maxProbability) +
                                  ")");
    executiveSummary.emplace_back("Least Likely Outcome",
                                  ": " + formatCurrency(leastLikelyAssets) + " (" + formatToPercentage(minProbability) +
                                  ")");

    return executiveSummary;
}

// Displays a detailed financial summary
void viewDetailedSummary() {
    std::unordered_map<std::string, double> categoryToAmount;
    double totalAssets = 0.0, currentAssets = 0.0;

    for (const auto &item: globalState.getAllItemsBeforeEndOfMonth()) {
        bool isFuture = item.getDate() > getCurrentDate();
        if (item.getType() == ItemType::Expense) {
            categoryToAmount[item.getCategory()] += item.getAmount();
            totalAssets -= item.getAmount();
            if (!isFuture) currentAssets -= item.getAmount();
        }

        if (item.getType() == ItemType::Income) {
            totalAssets += item.getAmount();
            if (!isFuture) currentAssets += item.getAmount();
        }

        if (item.getType() == ItemType::Asset) {
            totalAssets += item.getAmount();
            if (!isFuture) currentAssets += item.getAmount();
        }
    }

    // Sort and print top 3 categories
    std::vector<std::pair<std::string, double>> sortedCategories(categoryToAmount.begin(), categoryToAmount.end());
    std::sort(sortedCategories.begin(), sortedCategories.end(),
              [](const std::pair<std::string, double> &a, const std::pair<std::string, double> &b) {
                  return a.second > b.second;
              });

    std::vector<KeyValuePair> executiveSummary;
    std::ostringstream ossTop3ExpenseCategories;
    for (size_t i = 0; i < std::min(sortedCategories.size(), static_cast<size_t>(3)); ++i) {
        ossTop3ExpenseCategories << i + 1 << ". " << sortedCategories[i].first << ": " << formatCurrency(
            sortedCategories[i].second) << "\n";
    }
    executiveSummary.emplace_back("Summary This Month", "");
    executiveSummary.emplace_back("Top 3 Expense by Categories", ": \n" + ossTop3ExpenseCategories.str());

    executiveSummary.emplace_back("Projected End of Month Assets", ": " + formatCurrency(totalAssets));
    executiveSummary.emplace_back("Current Assets", ": " + formatCurrency(currentAssets));

    auto scenarioResults = evaluateScenarios(globalState.getItems());
    executiveSummary.insert(executiveSummary.end(), scenarioResults.begin(), scenarioResults.end());
    printWithPadding(executiveSummary);
}

// Displays a summary of financial items for the current month
void viewSummary() {
    double totalAssets = 0.0;
    double totalLiabilities = 0.0;
    double totalIncome = 0.0;
    double totalExpenses = 0.0;

    for (const auto &item: globalState.getItemsThisMonth()) {
        switch (item.getType()) {
            case ItemType::Asset:
                totalAssets += item.getAmount();
                break;
            case ItemType::Liability:
                totalLiabilities += item.getAmount();
                break;
            case ItemType::Income:
                totalIncome += item.getAmount();
                break;
            case ItemType::Expense:
                totalExpenses += item.getAmount();
                break;
        }
    }

    printWithPadding({
        {"Summary This Month", ""},
        {"Total Assets", ": " + formatCurrency(totalAssets)},
        {"Total Liabilities", ": " + formatCurrency(totalLiabilities)},
        {"Total Income", ": " + formatCurrency(totalIncome)},
        {"Total Expenses", ": " + formatCurrency(totalExpenses)}
    });
}

// Collects transaction details from the user
void inputTransactionDetails(std::string &category, double &amount, std::string &date, double &probability) {
    getInput("category", &category, category.empty() ? std::string("General") : category);
    getInput("amount", &amount, amount != 0.0 ? amount : 0.0);

    const std::string &currentDate = getCurrentDate();
    getInput("date", &date, date.empty() ? currentDate : date);
    getInput("probability", &probability, probability != 0.0 ? probability : 1.0);
}

// Adds a new transaction
void addTransaction() {
    std::cout << "Adding transaction...\n";
    std::string name, category, date;
    double amount = 0, probability = 1.0;
    int typeInt;

    getInput("type (0: Asset, 1: Liability, 2: Income, 3: Expense)", &typeInt, 0);
    getInput("name", &name, std::string("Unnamed"));
    inputTransactionDetails(category, amount, date, probability);

    auto type = static_cast<ItemType>(typeInt);
    globalState.items.emplace_back(type, name, category, amount, date, probability);
    globalState.save();
}

// Edits an existing transaction by name
void editTransaction() {
    std::cout << "Editing transaction...\n";
    std::string name;
    getInput("the name of the transaction to edit", &name, std::string(""));

    for (auto &item: globalState.items) {
        if (item.getName() == name) {
            std::string newCategory = item.getTypeName();
            std::string newDate = item.getDate();
            double newAmount = item.getAmount();
            double newProbability = item.getProbability();
            inputTransactionDetails(newCategory, newAmount, newDate, newProbability);

            item = FinancialItem(item.getType(), name, newCategory, newAmount, newDate, newProbability);
            globalState.save();
            return;
        }
    }
    std::cout << "Transaction not found.\n";
}

// Deletes a transaction by name
void deleteTransaction() {
    std::cout << "Deleting transaction...\n";
    std::string name;
    getInput("the name of the transaction to delete", &name, std::string(""));

    auto it = std::remove_if(globalState.items.begin(), globalState.items.end(),
                             [&name](const FinancialItem &item) { return item.getName() == name; });
    if (it != globalState.items.end()) {
        globalState.items.erase(it, globalState.items.end());
        globalState.save();
        std::cout << "Transaction deleted.\n";
    } else {
        std::cout << "Transaction not found.\n";
    }
}

// Main function
int main() {
    loadProgram();
    std::atexit(saveProgram);

    std::vector<std::pair<std::string, std::function<void()> > > menu = {
        {"View Detailed Summary", viewDetailedSummary},
        {"Add Transaction", addTransaction},
        {"Edit Transaction", editTransaction},
        {"Delete Transaction", deleteTransaction},
        {"Exit", exitProgram}
    };

    while (true) {
        clearScreen();
        viewSummary();
        displayMenu(menu);
    }

    return 0;
}

#pragma endregion Application
