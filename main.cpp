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
#include <chrono>
#include <ctime>


const std::string& CURRENCY = "IDR";
std::string formatIDR(double amount) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(0) << amount;
    std::string numStr = oss.str();
    int n = numStr.length() - 1;
    int insertPosition = n - 2;

    while (insertPosition > 0) {
        numStr.insert(insertPosition, ".");
        insertPosition -= 3;
    }

    return "IDR " + numStr;
}
std::string formatCurrency(double amount) {
    std::ostringstream oss;


    if (CURRENCY == "IDR") {
        oss << formatIDR(amount);
    } else if (CURRENCY == "USD") {
        oss.imbue(std::locale("en_US.UTF-8")); // Use the US locale for thousands separator
        oss << "USD " << std::fixed << std::setprecision(2) << amount;
    } else {
        return "Unsupported currency";
    }

    return oss.str();
}

enum class ItemType {
    Asset,
    Liability,
    Income,
    Expense
};

class FinancialItem {
private:
	std::string category;
    ItemType type;
    std::string name;
    double amount;
    std::string date;
    double probability;

public:
    FinancialItem(ItemType type, std::string &name, std::string &category, double amount, std::string &date,
                  double probability)
        : type(type), name(name), category(category), amount(amount), date(date), probability(probability) {
    }

    void display() const {
        std::string typeName = getTypeName();
        std::cout << "(" << date << ") " << name << " (" << category << "): " << formatCurrency(amount) << " (" <<
                typeName << ") " << (probability * 100) << "%\n";
    }

    [[nodiscard]] std::string getTypeName() const {
        switch (type) {
            case ItemType::Asset: return "Asset";
            case ItemType::Liability: return "Liability";
            case ItemType::Income: return "Income";
            case ItemType::Expense: return "Expense";
        }
        return "";
    }

    void serialize(std::ofstream &out) const {
        out << getTypeName() << "," << name << "," << category << "," << amount << "," << date << "," << probability <<
                "\n";
    }

    [[nodiscard]] std::string getName() const {
    	return name;
	}

    [[nodiscard]] ItemType getType() const {
        return type;
    }

    [[nodiscard]] double getAmount() const {
        return amount;
    }

    [[nodiscard]] std::string getDate() const {
        return date;
    }

    [[nodiscard]] double getProbability() const {
        return probability;
    }

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

void viewDetailedSummary(){
	
}
const std::string &HEADER = "Type,Name,Category,Amount,Date,Probability";

void serializeAllItems(const std::vector<FinancialItem>& items, const std::string& filename) {
    std::ofstream out(filename);
    // print header
    out << HEADER << "\n";
    for (const auto& item : items) {
        item.serialize(out);
    }
}

void deserializeAllItems(std::vector<FinancialItem>& items, const std::string& filename) {
    std::ifstream in(filename);
    while (in.peek() != EOF) {
        // Skip the header if matches
        std::string line = "";
        std::getline(in, line);
        if (line == HEADER) continue;

        items.push_back(FinancialItem::deserialize(line));
    }
}

struct GlobalState {
    std::vector<FinancialItem> items;

    GlobalState() {
        load();
    }

    void save() const {
        serializeAllItems(items, "financial_items.csv");
    }

    void load() {
		try {
            items.clear();
			deserializeAllItems(items, "financial_items.csv");
        } catch (const std::exception& e) {
            std::cerr << "Error loading items: " << e.what() << std::endl;
        }
    }
};

GlobalState globalState;
void clearScreen() {
    // Use "clear" for UNIX/Linux/MacOS, "CLS" for Windows
#ifdef _WIN32
    system("CLS");
#else
    system("clear");
#endif
}

// Save the program state to disk
void saveProgram() {
    globalState.save();
}

// Load the program state from disk
void loadProgram() {
    globalState.load();
}

void exitProgram() {
    std::cout << "Exiting program...\n";
    saveProgram();
    exit(0);
}

std::string getCurrentDate() {
    // Get the current time
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);

    // Create a string stream for formatting
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d"); // Use double dashes

    return oss.str();
}

void waitToContinue() {
    std::cout << std::endl;
    std::cout << "Press Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get(); // Pause until Enter is pressed
    std::cout << std::endl;
}
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


void viewSummary() {
    double totalAssets = 0.0;
    double totalLiabilities = 0.0;
    double totalIncome = 0.0;
    double totalExpenses = 0.0;

    for (const auto& item : globalState.items) {
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

    std::cout << "Summary:\n";
    std::cout << "Total Assets: " << formatCurrency(totalAssets) << "\n";
    std::cout << "Total Liabilities: " << formatCurrency(totalLiabilities) << "\n";
    std::cout << "Total Income: " << formatCurrency(totalIncome) << "\n";
    std::cout << "Total Expenses: " << formatCurrency(totalExpenses) << "\n";
}


void inputTransactionDetails(std::string &category, double &amount, std::string &date, double &probability) {
    getInput("category", &category, category.empty() ? std::string("General") : category);
    getInput("amount", &amount, amount != 0.0 ? amount : 0.0);

    const std::string &currentDate = getCurrentDate();
    getInput("date", &date, date.empty() ? currentDate : date);
    getInput("probability", &probability, probability != 0.0 ? probability : 1.0);
}

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

void editTransaction() {
    std::cout << "Editing transaction...\n";
    std::string name;
    getInput("the name of the transaction to edit", &name, std::string(""));

    for (auto& item : globalState.items) {
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

void deleteTransaction() {
    std::cout << "Deleting transaction...\n";
    std::string name;
    getInput("the name of the transaction to delete", &name, std::string(""));

    auto it = std::remove_if(globalState.items.begin(), globalState.items.end(),
                             [&name](const FinancialItem& item) { return item.getName() == name; });
    if (it != globalState.items.end()) {
        globalState.items.erase(it, globalState.items.end());
        globalState.save();
        std::cout << "Transaction deleted.\n";
    } else {
        std::cout << "Transaction not found.\n";
    }
}


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
