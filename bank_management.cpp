/*
 ============================================================
  BANK MANAGEMENT SYSTEM
  Language  : C++
  Concepts  : OOP, File Handling, Inheritance, STL
 ============================================================
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <ctime>
#include <vector>
#include <limits>
#include <algorithm>

using namespace std;

// ─────────────────────────────────────────────
//  UTILITY FUNCTIONS
// ─────────────────────────────────────────────

string getCurrentDateTime() {
    time_t now = time(nullptr);
    char buf[20];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
    return string(buf);
}

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void pauseScreen() {
    cout << "\n  Press Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

void printLine(char ch = '-', int len = 55) {
    cout << "  " << string(len, ch) << endl;
}

void printHeader(const string& title) {
    clearScreen();
    printLine('=');
    cout << "  " << setw(45) << left << "  BANK MANAGEMENT SYSTEM" << endl;
    printLine('=');
    cout << "  >> " << title << endl;
    printLine();
}

// ─────────────────────────────────────────────
//  BASE CLASS: Person
// ─────────────────────────────────────────────

class Person {
protected:
    string name;
    int    age;
    string address;
    string phone;

public:
    Person() : age(0) {}
    Person(const string& n, int a, const string& addr, const string& ph)
        : name(n), age(a), address(addr), phone(ph) {}

    virtual ~Person() {}

    string getName()    const { return name;    }
    int    getAge()     const { return age;     }
    string getAddress() const { return address; }
    string getPhone()   const { return phone;   }

    void setName(const string& n)    { name    = n; }
    void setAge(int a)               { age     = a; }
    void setAddress(const string& a) { address = a; }
    void setPhone(const string& p)   { phone   = p; }

    virtual void display() const {
        cout << "  Name    : " << name    << endl;
        cout << "  Age     : " << age     << endl;
        cout << "  Address : " << address << endl;
        cout << "  Phone   : " << phone   << endl;
    }
};

// ─────────────────────────────────────────────
//  DERIVED CLASS: Account  (inherits Person)
// ─────────────────────────────────────────────

class Account : public Person {
private:
    long   accNumber;
    double balance;
    string accType;       // Savings / Current

public:
    // ── Constructors ──
    Account() : accNumber(0), balance(0.0), accType("Savings") {}

    Account(long accNo, const string& n, int a,
            const string& addr, const string& ph,
            double bal, const string& type)
        : Person(n, a, addr, ph),
          accNumber(accNo), balance(bal), accType(type) {}

    // ── Getters ──
    long   getAccNumber() const { return accNumber; }
    double getBalance()   const { return balance;   }
    string getAccType()   const { return accType;   }

    // ── Setters ──
    void setBalance(double b)         { balance = b;    }
    void setAccType(const string& t)  { accType = t;    }

    // ── Operations ──
    bool deposit(double amount) {
        if (amount <= 0) return false;
        balance += amount;
        return true;
    }

    bool withdraw(double amount) {
        if (amount <= 0 || amount > balance) return false;
        balance -= amount;
        return true;
    }

    // ── Display (overrides Person) ──
    void display() const override {
        cout << "  Account No : " << accNumber << endl;
        cout << "  Type       : " << accType   << endl;
        Person::display();
        cout << fixed << setprecision(2);
        cout << "  Balance    : Rs. " << balance << endl;
    }

    // ── Serialise to one CSV line ──
    string toCSV() const {
        ostringstream oss;
        oss << accNumber << ","
            << name      << ","
            << age       << ","
            << address   << ","
            << phone     << ","
            << fixed << setprecision(2) << balance << ","
            << accType;
        return oss.str();
    }

    // ── Deserialise from one CSV line ──
    static Account fromCSV(const string& line) {
        istringstream ss(line);
        string token;
        vector<string> fields;
        while (getline(ss, token, ',')) fields.push_back(token);

        if (fields.size() < 7) return Account();

        Account a;
        a.accNumber = stol(fields[0]);
        a.name      = fields[1];
        a.age       = stoi(fields[2]);
        a.address   = fields[3];
        a.phone     = fields[4];
        a.balance   = stod(fields[5]);
        a.accType   = fields[6];
        return a;
    }
};

// ─────────────────────────────────────────────
//  FILE NAMES
// ─────────────────────────────────────────────

const string ACCOUNTS_FILE    = "accounts.dat";
const string TRANSACTIONS_FILE = "transactions.dat";

// ─────────────────────────────────────────────
//  FILE HELPER FUNCTIONS
// ─────────────────────────────────────────────

vector<Account> loadAllAccounts() {
    vector<Account> accounts;
    ifstream fin(ACCOUNTS_FILE);
    if (!fin.is_open()) return accounts;
    string line;
    while (getline(fin, line)) {
        if (!line.empty())
            accounts.push_back(Account::fromCSV(line));
    }
    fin.close();
    return accounts;
}

void saveAllAccounts(const vector<Account>& accounts) {
    ofstream fout(ACCOUNTS_FILE, ios::trunc);
    for (const auto& acc : accounts)
        fout << acc.toCSV() << "\n";
    fout.close();
}

void logTransaction(long accNo, const string& type,
                    double amount, double balAfter) {
    ofstream fout(TRANSACTIONS_FILE, ios::app);
    fout << accNo     << ","
         << type      << ","
         << fixed << setprecision(2) << amount    << ","
         << fixed << setprecision(2) << balAfter  << ","
         << getCurrentDateTime() << "\n";
    fout.close();
}

long generateAccountNumber() {
    vector<Account> accounts = loadAllAccounts();
    long maxNo = 100000;
    for (const auto& a : accounts)
        if (a.getAccNumber() > maxNo)
            maxNo = a.getAccNumber();
    return maxNo + 1;
}

// ─────────────────────────────────────────────
//  MENU FUNCTIONS
// ─────────────────────────────────────────────

// ── 1. Create Account ──────────────────────────
void createAccount() {
    printHeader("Create New Account");

    Account acc;
    string name, address, phone, accType;
    int age;
    double initialDeposit;

    cout << "\n  Enter Account Type (1-Savings / 2-Current): ";
    int choice; cin >> choice;
    cin.ignore();
    accType = (choice == 2) ? "Current" : "Savings";

    cout << "  Full Name    : "; getline(cin, name);
    cout << "  Age          : "; cin >> age; cin.ignore();
    cout << "  Address      : "; getline(cin, address);
    cout << "  Phone Number : "; getline(cin, phone);
    cout << "  Initial Deposit (min Rs.500): Rs. ";
    cin >> initialDeposit;

    if (initialDeposit < 500) {
        cout << "\n  [!] Minimum deposit is Rs.500. Account not created.\n";
        pauseScreen(); return;
    }

    long accNo = generateAccountNumber();
    Account newAcc(accNo, name, age, address, phone, initialDeposit, accType);

    vector<Account> accounts = loadAllAccounts();
    accounts.push_back(newAcc);
    saveAllAccounts(accounts);
    logTransaction(accNo, "ACCOUNT_OPEN", initialDeposit, initialDeposit);

    printLine();
    cout << "\n  [✓] Account Created Successfully!\n";
    cout << "  Your Account Number : " << accNo << "\n";
    printLine();
    pauseScreen();
}

// ── 2. Deposit Money ───────────────────────────
void depositMoney() {
    printHeader("Deposit Money");

    long accNo; double amount;
    cout << "\n  Enter Account Number : "; cin >> accNo;
    cout << "  Deposit Amount       : Rs. "; cin >> amount;

    vector<Account> accounts = loadAllAccounts();
    bool found = false;

    for (auto& acc : accounts) {
        if (acc.getAccNumber() == accNo) {
            found = true;
            if (!acc.deposit(amount)) {
                cout << "\n  [!] Invalid amount.\n";
            } else {
                saveAllAccounts(accounts);
                logTransaction(accNo, "DEPOSIT", amount, acc.getBalance());
                cout << "\n  [✓] Rs." << fixed << setprecision(2) << amount
                     << " deposited successfully.\n";
                cout << "  Updated Balance : Rs." << acc.getBalance() << "\n";
            }
            break;
        }
    }
    if (!found) cout << "\n  [!] Account not found.\n";
    pauseScreen();
}

// ── 3. Withdraw Money ──────────────────────────
void withdrawMoney() {
    printHeader("Withdraw Money");

    long accNo; double amount;
    cout << "\n  Enter Account Number : "; cin >> accNo;
    cout << "  Withdraw Amount      : Rs. "; cin >> amount;

    vector<Account> accounts = loadAllAccounts();
    bool found = false;

    for (auto& acc : accounts) {
        if (acc.getAccNumber() == accNo) {
            found = true;
            if (!acc.withdraw(amount)) {
                cout << "\n  [!] Insufficient balance or invalid amount.\n";
                cout << "  Current Balance : Rs." << fixed
                     << setprecision(2) << acc.getBalance() << "\n";
            } else {
                saveAllAccounts(accounts);
                logTransaction(accNo, "WITHDRAW", amount, acc.getBalance());
                cout << "\n  [✓] Rs." << fixed << setprecision(2) << amount
                     << " withdrawn successfully.\n";
                cout << "  Remaining Balance : Rs." << acc.getBalance() << "\n";
            }
            break;
        }
    }
    if (!found) cout << "\n  [!] Account not found.\n";
    pauseScreen();
}

// ── 4. Balance Inquiry ─────────────────────────
void checkBalance() {
    printHeader("Balance Inquiry");

    long accNo;
    cout << "\n  Enter Account Number : "; cin >> accNo;

    vector<Account> accounts = loadAllAccounts();
    bool found = false;

    for (const auto& acc : accounts) {
        if (acc.getAccNumber() == accNo) {
            found = true;
            printLine();
            acc.display();
            printLine();
            break;
        }
    }
    if (!found) cout << "\n  [!] Account not found.\n";
    pauseScreen();
}

// ── 5. Transaction History ─────────────────────
void viewTransactionHistory() {
    printHeader("Transaction History");

    long accNo;
    cout << "\n  Enter Account Number : "; cin >> accNo;

    ifstream fin(TRANSACTIONS_FILE);
    if (!fin.is_open()) {
        cout << "\n  [!] No transactions found.\n";
        pauseScreen(); return;
    }

    printLine();
    cout << "  " << left
         << setw(10) << "AccNo"
         << setw(16) << "Type"
         << setw(12) << "Amount"
         << setw(12) << "Balance"
         << "Date & Time" << endl;
    printLine();

    string line;
    bool found = false;
    while (getline(fin, line)) {
        if (line.empty()) continue;
        istringstream ss(line);
        vector<string> f;
        string tok;
        while (getline(ss, tok, ',')) f.push_back(tok);
        if (f.size() >= 5 && stol(f[0]) == accNo) {
            found = true;
            cout << "  " << left
                 << setw(10) << f[0]
                 << setw(16) << f[1]
                 << setw(12) << ("Rs." + f[2])
                 << setw(12) << ("Rs." + f[3])
                 << f[4] << endl;
        }
    }
    fin.close();

    if (!found) cout << "\n  [!] No transactions found for this account.\n";
    printLine();
    pauseScreen();
}

// ── 6. Update Account Details ──────────────────
void updateAccount() {
    printHeader("Update Account Details");

    long accNo;
    cout << "\n  Enter Account Number : "; cin >> accNo;
    cin.ignore();

    vector<Account> accounts = loadAllAccounts();
    bool found = false;

    for (auto& acc : accounts) {
        if (acc.getAccNumber() == accNo) {
            found = true;
            cout << "\n  Current Details:\n";
            printLine();
            acc.display();
            printLine();

            string newName, newAddr, newPhone;
            int newAge;

            cout << "\n  New Name    (Enter to keep: " << acc.getName() << "): ";
            getline(cin, newName);
            if (!newName.empty()) acc.setName(newName);

            cout << "  New Age     (0 to keep: " << acc.getAge() << ")    : ";
            cin >> newAge; cin.ignore();
            if (newAge > 0) acc.setAge(newAge);

            cout << "  New Address (Enter to keep): ";
            getline(cin, newAddr);
            if (!newAddr.empty()) acc.setAddress(newAddr);

            cout << "  New Phone   (Enter to keep): ";
            getline(cin, newPhone);
            if (!newPhone.empty()) acc.setPhone(newPhone);

            saveAllAccounts(accounts);
            cout << "\n  [✓] Account updated successfully.\n";
            break;
        }
    }
    if (!found) cout << "\n  [!] Account not found.\n";
    pauseScreen();
}

// ── 7. Delete Account ──────────────────────────
void deleteAccount() {
    printHeader("Delete Account");

    long accNo;
    cout << "\n  Enter Account Number to Delete : "; cin >> accNo;
    cin.ignore();

    vector<Account> accounts = loadAllAccounts();
    auto it = find_if(accounts.begin(), accounts.end(),
                      [accNo](const Account& a){ return a.getAccNumber() == accNo; });

    if (it == accounts.end()) {
        cout << "\n  [!] Account not found.\n";
        pauseScreen(); return;
    }

    cout << "\n  Account Details:\n";
    printLine();
    it->display();
    printLine();
    cout << "  Are you sure you want to DELETE this account? (y/n): ";
    char confirm; cin >> confirm;

    if (confirm == 'y' || confirm == 'Y') {
        accounts.erase(it);
        saveAllAccounts(accounts);
        logTransaction(accNo, "ACCOUNT_CLOSED", 0, 0);
        cout << "\n  [✓] Account deleted successfully.\n";
    } else {
        cout << "\n  [!] Deletion cancelled.\n";
    }
    pauseScreen();
}

// ── 8. List All Accounts ───────────────────────
void listAllAccounts() {
    printHeader("All Accounts");

    vector<Account> accounts = loadAllAccounts();
    if (accounts.empty()) {
        cout << "\n  [!] No accounts found.\n";
        pauseScreen(); return;
    }

    cout << "\n  " << left
         << setw(12) << "Acc No"
         << setw(22) << "Name"
         << setw(10) << "Type"
         << setw(8)  << "Age"
         << setw(14) << "Phone"
         << "Balance (Rs.)" << endl;
    printLine('-', 80);

    for (const auto& acc : accounts) {
        cout << "  " << left
             << setw(12) << acc.getAccNumber()
             << setw(22) << acc.getName()
             << setw(10) << acc.getAccType()
             << setw(8)  << acc.getAge()
             << setw(14) << acc.getPhone()
             << fixed << setprecision(2) << acc.getBalance() << endl;
    }

    cout << "\n  Total Accounts: " << accounts.size() << endl;
    printLine('-', 80);
    pauseScreen();
}

// ─────────────────────────────────────────────
//  MAIN MENU
// ─────────────────────────────────────────────

void showMainMenu() {
    printLine('=');
    cout << "  " << setw(45) << left << "  BANK MANAGEMENT SYSTEM" << endl;
    cout << "  " << setw(45) << left << "  Welcome! Please choose an option." << endl;
    printLine('=');
    cout << "\n"
         << "  [1]  Create New Account\n"
         << "  [2]  Deposit Money\n"
         << "  [3]  Withdraw Money\n"
         << "  [4]  Balance Inquiry\n"
         << "  [5]  Transaction History\n"
         << "  [6]  Update Account Details\n"
         << "  [7]  Delete Account\n"
         << "  [8]  List All Accounts\n"
         << "  [0]  Exit\n";
    printLine();
    cout << "  Enter your choice : ";
}

// ─────────────────────────────────────────────
//  ENTRY POINT
// ─────────────────────────────────────────────

int main() {
    int choice;
    do {
        clearScreen();
        showMainMenu();
        cin >> choice;

        switch (choice) {
            case 1: createAccount();          break;
            case 2: depositMoney();           break;
            case 3: withdrawMoney();          break;
            case 4: checkBalance();           break;
            case 5: viewTransactionHistory(); break;
            case 6: updateAccount();          break;
            case 7: deleteAccount();          break;
            case 8: listAllAccounts();        break;
            case 0:
                clearScreen();
                printLine('=');
                cout << "\n  Thank you for using Bank Management System!\n\n";
                printLine('=');
                break;
            default:
                cout << "\n  [!] Invalid choice. Please try again.\n";
                pauseScreen();
        }
    } while (choice != 0);

    return 0;
}
