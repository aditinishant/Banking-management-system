# Bank Management System — C++
### OOP + File Handling | TCS / QT Placement Ready

---

## Features
| # | Feature                 | Description                                    |
|---|-------------------------|------------------------------------------------|
| 1 | Create Account          | Savings/Current, auto account number           |
| 2 | Deposit Money           | Adds funds, logs transaction                   |
| 3 | Withdraw Money          | Validates balance before deducting             |
| 4 | Balance Inquiry         | Shows full account details                     |
| 5 | Transaction History     | Reads from transaction log file                |
| 6 | Update Account          | Edit name, age, address, phone                 |
| 7 | Delete Account          | With confirmation prompt                       |
| 8 | List All Accounts       | Tabular view of every account                  |

---

## OOP Concepts Used
- **Class & Object** — `Person`, `Account`
- **Inheritance** — `Account` inherits `Person`
- **Encapsulation** — Private data with public getters/setters
- **Polymorphism** — `display()` virtual function overridden in `Account`
- **Abstraction** — File I/O hidden behind helper functions

---

## File Handling
| File              | Purpose                                  |
|-------------------|------------------------------------------|
| `accounts.dat`    | Stores all account records (CSV format)  |
| `transactions.dat`| Appended log of every deposit/withdrawal |

Data is read into `vector<Account>` in memory, modified, then written back.

---

## Compilation & Run

### Linux / macOS
```bash
g++ -std=c++17 -o bank bank_management.cpp
./bank
```

### Windows (MinGW / g++)
```bat
g++ -std=c++17 -o bank.exe bank_management.cpp
bank.exe
```

### Windows (MSVC)
```bat
cl /EHsc bank_management.cpp /Fe:bank.exe
bank.exe
```

> Requires C++17 or later. No external libraries needed.

---

## Project Structure
```
bank_management.cpp   ← Single-file project (all code here)
accounts.dat          ← Auto-created on first account creation
transactions.dat      ← Auto-created on first transaction
```

---

## Class Diagram

```
          ┌──────────────┐
          │    Person    │  (Base Class)
          │──────────────│
          │ # name       │
          │ # age        │
          │ # address    │
          │ # phone      │
          │──────────────│
          │ + display()  │  ← virtual
          └──────┬───────┘
                 │  inherits
          ┌──────▼────────┐
          │    Account    │  (Derived Class)
          │───────────────│
          │ - accNumber   │
          │ - balance     │
          │ - accType     │
          │───────────────│
          │ + deposit()   │
          │ + withdraw()  │
          │ + display()   │  ← overrides
          │ + toCSV()     │
          │ + fromCSV()   │
          └───────────────┘
```

---

## Sample Interaction

```
  =======================================================
    BANK MANAGEMENT SYSTEM
    Welcome! Please choose an option.
  =======================================================

  [1]  Create New Account
  [2]  Deposit Money
  ...
  [0]  Exit

  Enter your choice : 1

  Enter Account Type (1-Savings / 2-Current): 1
  Full Name    : Rahul Sharma
  Age          : 22
  Address      : 12 MG Road, Kolkata
  Phone Number : 9876543210
  Initial Deposit (min Rs.500): Rs. 5000

  -------------------------------------------------------
  [✓] Account Created Successfully!
  Your Account Number : 100001
  -------------------------------------------------------
```

---

## Key Points for Interviews
1. Explain the `Person → Account` inheritance chain
2. Explain why `display()` is `virtual`
3. Explain `toCSV()` / `fromCSV()` for file persistence
4. Explain why `vector<Account>` is used over arrays
5. Explain `ios::app` for transaction log vs `ios::trunc` for accounts
