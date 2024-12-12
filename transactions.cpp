#include <iostream>
#include <unordered_map>
#include <stack>
#include <stdexcept>

class memoryDB
{
private:
    std::unordered_map<std::string, int> mainDB;
    std::unordered_map<std::string, int> transactionsDB;
    bool inTransaction = false;


public:
    int get(const std::string key)
    {
        if (inTransaction && transactionsDB.find(key) != transactionsDB.end())
        {
            return transactionsDB[key];
        }
        if (mainDB.find(key) != mainDB.end())
        {
            return mainDB[key];
        }
        throw std::runtime_error("Key not found");
    }

    void put(std::string key, int val)
    {
        if (!inTransaction)
        {
            throw std::runtime_error("No transaction in progress");
        }

        transactionsDB[key] = val;
    }

    void begin_transaction()
    {
        if (inTransaction)
        {
            throw std::runtime_error("Transaction already in progress");
        }
        inTransaction = true;
        transactionsDB.clear();
    }

    void commit()
    {
        if (!inTransaction)
        {
            throw std::runtime_error("No transaction in progress");
        }
        for (const auto& pair : transactionsDB)
        {
            mainDB[pair.first] = pair.second;
        }
        inTransaction = false;
        transactionsDB.clear();
    }

    void rollback()
    {
        if (!inTransaction)
        {
            throw std::runtime_error("No transaction in progress");
        }

        inTransaction = false;
        transactionsDB.clear();
    }
};


int main() {
    memoryDB db;

    try{
        db.get("A"); // should throw an error because A doesn’t exist in the DB yet
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
    }

    try {
        db.put("A", 5); // should throw an error because a transaction is not in progress
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
    }

    db.begin_transaction(); // starts a new transaction
    db.put("A", 5); // set’s value of A to 5, but its not committed yet

    try {
        std::cout << db.get("A") << std::endl; // should throw an error because updates to A are not committed yet
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
    }

    db.put("A", 6); // update A’s value to 6 within the transaction
    db.commit(); // commits the open transaction

    std::cout << db.get("A") << std::endl; // should return 6, that was the last value of A to be committed

    try {
        db.commit(); // should throw an error, because there is no open transaction
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
    }

    try {
        db.rollback(); // should throw an error because there is no ongoing transaction
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
    }

    try {
        std::cout << db.get("B") << std::endl; // should throw an error because B does not exist in the database
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
    }

    db.begin_transaction(); // starts a new transaction
    db.put("B", 10); // Set key B’s value to 10 within the transaction
    db.rollback(); // Rollback the transaction - revert any changes made to B

    try {
        std::cout << db.get("B") << std::endl; // should throw an error because changes to B were rolled back
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
    }

    return 0;
}