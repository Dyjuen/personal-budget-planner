#include <iostream>
#include <unordered_map>
#include <functional>
#include <string>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <iomanip>
#include <sstream>

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
            deserializeAllItems(items, "financial_items.csv");
        } catch (const std::exception& e) {
            std::cerr << "Error loading items: " << e.what() << std::endl;
        }
    }
};

GlobalState globalState;

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
        if (item.getTypeName() == name) {
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
                             [&name](const FinancialItem& item) { return item.getTypeName() == name; });
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

void exitProgram() {
    std::cout << "Exiting program...\n";
    globalState.save();
    exit(0);
}



void reports() {
    std::unordered_map<std::string, std::function<void()>> menu = {
        {"1", monthlyReport},
        {"2", annualReport},
        {"3", incomeVsExpenses},
        {"4", assetAndLiabilityBreakdown},
    };

    std::string choice;
    while (true) {
        std::cout << "\n-------- Reports --------\n";
        std::cout << "1. Monthly Report\n";
        std::cout << "2. Annual Reports\n";
        std::cout << "3. Income vs. Expenses\n";
        std::cout << "4. Asset and Liability Breakdown\n";
        std::cout << "5. Back to Main Menu\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;
        auto it = menu.find(choice);
        if (it != menu.end()) {
            it->second();
        } else {
            std::cout << "Returning to Main Menu\n";
            return;
        }
    }
}

void settings() {
    std::unordered_map<std::string, std::function<void()>> menu = {
        {"1", manageCategories},
        {"2", setBudgetLimits},
        {"3", notificationPreferences},
        {"4", securitySettings},
    };

    std::string choice;
    while (true) {
        std::cout << "\n-------- Settings --------\n";
        std::cout << "1. Manage Categories\n";
        std::cout << "2. Set Budget Limits\n";
        std::cout << "3. Notification Preferences\n";
        std::cout << "4. Security Settings\n";
        std::cout << "5. Back to Main Menu\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;
        auto it = menu.find(choice);
        if (it != menu.end()) {
            it->second();
        } else {
            std::cout << "Returning to Main Menu\n";
            return;
        }
    }
}

void advancedFeatures() {
    std::unordered_map<std::string, std::function<void()>> menu = {
        {"1", syncWithBankAccounts},
        {"2", setFinancialGoals},
        {"3", spendingInsights},
        {"4", predictiveAnalytics},
        {"5", sharedBudgeting},
        {"6", exportData},
        {"7", importData}
    };

    std::string choice;
    while (true) {
        std::cout << "\n-------- Advanced Features --------\n";
        std::cout << "1. Sync With Bank Accounts\n";
        std::cout << "2. Set Financial Goals\n";
        std::cout << "3. Spending Insights\n";
        std::cout << "4. Predictive Analytics\n";
        std::cout << "5. Shared Budgeting\n";
        std::cout << "6. Export Data\n";
        std::cout << "7. Import Data\n";
        std::cout << "8. Back to Main Menu\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;
        auto it = menu.find(choice);
        if (it != menu.end()) {
            it->second();
        } else {
            std::cout << "Returning to Main Menu\n";
            return;
        }
    }
}

void help() {
    std::unordered_map<std::string, std::function<void()>> menu = {
        {"1", userGuide},
        {"2", tutorials},
        {"3", contactSupport}
    };

    std::string choice;
    while (true) {
        std::cout << "\n-------- Help --------\n";
        std::cout << "1. User Guide\n";
        std::cout << "2. Tutorials\n";
        std::cout << "3. Contact Support\n";
        std::cout << "4. Back to Main Menu\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;
        auto it = menu.find(choice);
        if (it != menu.end()) {
            it->second();
        } else {
            std::cout << "Returning to Main Menu\n";
            return;
        }
    }
}

int main() {
    std::unordered_map<std::string, std::function<void()>> menu = {
        {"1", viewSummary},
        {"2", addTransaction},
        {"3", editTransaction},
        {"4", deleteTransaction},
        {"5", reports},
        {"6", settings},
        {"7", advancedFeatures},
        {"8", help},
        {"9", exitProgram}
    };

    std::string choice;
    while (true) {
        std::cout << "\n-------- Main Menu --------\n";
        std::cout << "1. View Summary\n";
        std::cout << "2. Add Transaction\n";
        std::cout << "3. Edit Transaction\n";
        std::cout << "4. Delete Transaction\n";
        std::cout << "5. Reports\n";
        std::cout << "6. Settings\n";
        std::cout << "7. Advanced Features\n";
        std::cout << "8. Help\n";
        std::cout << "9. Exit\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        auto it = menu.find(choice);
        if (it != menu.end()) {
            it->second();
        } else {
            std::cout << "Invalid choice. Please try again.\n";
        }
    }

    return 0;
}
