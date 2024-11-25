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
    ItemType type;
    std::string name;
    double amount;

public:
    FinancialItem(ItemType type, const std::string& name, double amount)
        : type(type), name(name), amount(amount) {}

    void display() const {
        std::string typeName = getTypeName();
        std::cout << typeName << " - " << name << ": " << amount << std::endl;
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

    void serialize(std::ofstream& out) const {
        out << getTypeName() << "," << name << "," << amount << "\n";
    }
    
    std::string getName() const {
    	return name;
	}

    [[nodiscard]] ItemType getType() const {
        return type;
    }

    [[nodiscard]] double getAmount() const {
        return amount;
    }

    static FinancialItem deserialize(std::istream& in) {
        std::string typeName, name;
        double amount;

        std::getline(in, typeName, ',');
        std::getline(in, name, ',');
        in >> amount;
        in.ignore(); // Ignore newline

        ItemType type;
        if (typeName == "Asset") type = ItemType::Asset;
        else if (typeName == "Liability") type = ItemType::Liability;
        else if (typeName == "Income") type = ItemType::Income;
        else if (typeName == "Expense") type = ItemType::Expense;
        else throw std::invalid_argument("Invalid item type");

        return FinancialItem(type, name, amount);
    }
};

void viewDetailedSummary(){
	
}
void serializeAllItems(const std::vector<FinancialItem>& items, const std::string& filename) {
    std::ofstream out(filename);
    for (const auto& item : items) {
        item.serialize(out);
    }
}

void deserializeAllItems(std::vector<FinancialItem>& items, const std::string& filename) {
    std::ifstream in(filename);
    while (in.peek() != EOF) {
        items.push_back(FinancialItem::deserialize(in));
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

void waitToContinue() {
    std::cout << std::endl;
    std::cout << "Press Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get(); // Pause until Enter is pressed
    std::cout << std::endl;
}

void displayMenu(const std::vector<std::pair<std::string, std::function<void()> > > &menu) {
    std::string choice;
    while (true) {
        std::cout << "\n-------- Main Menu --------\n";
        for (size_t i = 0; i < menu.size(); ++i) {
            std::cout << i + 1 << ". " << menu[i].first << "\n";
        }
        std::cout << "Enter your choice: ";
        std::cin >> choice;

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

void addTransaction() {
    std::cout << "Adding transaction...\n";
    std::string name;
    double amount;
    int typeInt;
    std::cout << "Enter type (0: Asset, 1: Liability, 2: Income, 3: Expense): ";
    std::cin >> typeInt;
    std::cout << "Enter name: ";
    std::cin >> name;
    std::cout << "Enter amount: ";
    std::cin >> amount;

    ItemType type = static_cast<ItemType>(typeInt);
    globalState.items.emplace_back(type, name, amount);
    globalState.save();
}

void editTransaction() {
    std::cout << "Editing transaction...\n";
    std::string name;
    std::cout << "Enter the name of the transaction to edit: ";
    std::cin >> name;

    for (auto& item : globalState.items) {
        if (item.getName() == name) {
            double newAmount;
            std::cout << "Enter new amount: ";
            std::cin >> newAmount;
            item = FinancialItem(item.getType(), name, newAmount);
            globalState.save();
            return;
        }
    }
    std::cout << "Transaction not found.\n";
}

void deleteTransaction() {
    std::cout << "Deleting transaction...\n";
    std::string name;
    std::cout << "Enter the name of the transaction to delete: ";
    std::cin >> name;

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

void monthlyReport() {
    std::cout << "Generating monthly report...\n";
    // Implement report generation logic here
}

void annualReport() {
    std::cout << "Generating annual report...\n";
    // Implement report generation logic here
}

void incomeVsExpenses() {
    std::cout << "Generating income vs expenses report...\n";
    // Implement report generation logic here
}

void assetAndLiabilityBreakdown() {
    std::cout << "Generating asset and liability breakdown...\n";
    // Implement breakdown logic here
}

void manageCategories() {
    std::cout << "Managing categories...\n";
    // Implement category management here
}

void setBudgetLimits() {
    std::cout << "Setting budget limits...\n";
    // Implement budget limit setting here
}

void notificationPreferences() {
    std::cout << "Setting notification preferences...\n";
    // Implement notification preference setting here
}

void securitySettings() {
    std::cout << "Adjusting security settings...\n";
    // Implement security settings here
}

void userGuide() {
    std::cout << "Displaying user guide...\n";
    // Implement user guide display here
}

void tutorials() {
    std::cout << "Displaying tutorials...\n";
    // Implement tutorials display here
}

void contactSupport() {
    std::cout << "Contacting support...\n";
    // Implement support contact here
}

void syncWithBankAccounts() {
    std::cout << "Syncing with bank accounts...\n";
    // Implement bank account sync here
}

void setFinancialGoals() {
    std::cout << "Setting financial goals...\n";
    // Implement financial goals setting here
}

void spendingInsights() {
    std::cout << "Generating spending insights...\n";
    // Implement spending insights here
}

void predictiveAnalytics() {
    std::cout << "Performing predictive analytics...\n";
    // Implement predictive analytics here
}

void sharedBudgeting() {
    std::cout << "Managing shared budgeting...\n";
    // Implement shared budgeting here
}

void exportData() {
    std::cout << "Exporting data...\n";
    // Implement data export here
}

void importData() {
    std::cout << "Importing data...\n";
    // Implement data import here
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
