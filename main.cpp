#include <iostream>
#include <unordered_map>
#include <functional> 

void viewSummary() {
    std::cout << "Viewing summary...\n";
}

void addTransaction() {
    std::cout << "Adding transaction...\n";
}

void editTransaction() {
    std::cout << "Editing transaction...\n";
}

void deleteTransaction() {
    std::cout << "Deleting transaction...\n";
}
void monthlyReport() {
    std::cout << "Deleting transaction...\n";
}
void annualReport() {
    std::cout << "Deleting transaction...\n";
}
void incomeVsExpenses() {
    std::cout << "Deleting transaction...\n";
}
void assetAndLiabilityBreakdown() {
    std::cout << "Deleting transaction...\n";
}
void manageCategories() {
    std::cout << "Categories...\n";
}
void setBudgetLimits() {
    std::cout << "Limits...\n";
}
void notificationPreferences() {
    std::cout << "Notifs...\n";
}
void securitySettings() {
    std::cout << "security...\n";
}
void userGuide() {
    std::cout << "guide...\n";
}
void tutorials() {
    std::cout << "tutor...\n";
}
void contactSupport() {
    std::cout << "support...\n";
}
void syncWithBankAccounts() {
    std::cout << "support...\n";
}
void setFinancialGoals() {
    std::cout << "support...\n";
}
void spendingInsights() {
    std::cout << "support...\n";
}
void predictiveAnalytics() {
    std::cout << "support...\n";
}
void sharedBudgeting() {
    std::cout << "support...\n";
}
void exportData() {
    std::cout << "support...\n";
}
void importData() {
    std::cout << "support...\n";
}

void exitProgram() {
    std::cout << "Exiting program...\n";
    exit(0);
}

void transaction() {
    std::unordered_map<std::string, std::function<void()>> menu = {
    {"1", viewSummary},
    {"2", addTransaction},
    {"3", editTransaction},
    {"4", deleteTransaction},
    };

    std::string choice;
    while (true) {
        std::cout << "\n-------- Transactions --------\n";
        std::cout << "1. View Summary\n";
        std::cout << "2. Add Transaction\n";
        std::cout << "3. Edit Transaction\n";
        std::cout << "4. Delete Transaction\n";
        std::cout << "5. Back to Main Menu\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;
        auto it = menu.find(choice);
        if (it != menu.end()) {
            it->second();
        } 
		else {
            std::cout << "Returning to Main Menu\n";
            return;
        }
    }
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
        } 
		else {
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
        } 
		else {
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
        std::cout << "\n-------- Settings --------\n";
        std::cout << "1. User Guide\n";
        std::cout << "2. Tutorials\n";
        std::cout << "3. Contact Support\n";
        std::cout << "4. Back to Main Menu\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;
        auto it = menu.find(choice);
        if (it != menu.end()) {
            it->second();
        } 
		else {
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
    {"5", advancedFeatures},
    {"6", help},
    };

    std::string choice;
    while (true) {
        std::cout << "\n-------- Settings --------\n";
        std::cout << "1. Manage Categories\n";
        std::cout << "2. Set Budget Limits\n";
        std::cout << "3. Notification Preferences\n";
        std::cout << "4. Security Settings\n";
        std::cout << "5. Advanced Features\n";
        std::cout << "6. Help\n";
        std::cout << "7. Back to Main Menu\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;
        auto it = menu.find(choice);
        if (it != menu.end()) {
            it->second();
        } 
		else {
            std::cout << "Returning to Main Menu\n";
            return;
        }
    }
}


int main() {
    std::unordered_map<std::string, std::function<void()>> menu = {
        {"1", transaction},
        {"2", reports},
        {"3", settings},
        {"4", exitProgram}
    };

    std::string choice;
    while (true) {
        std::cout << "\n-------- Main Menu --------\n";
        std::cout << "1. Transactions\n";
        std::cout << "2. Reports\n";
        std::cout << "3. Settings\n";
        std::cout << "4. Exit Program\n";
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
