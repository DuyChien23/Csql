//
// Created by chiendd on 28/11/2024.
//

#include <sstream>
#include <string>
#include <gtest/gtest.h>

#include "../src/core/controller/main_controller.h"
#include "../src/core/controller/tokenizer/tokenizer.h"

struct Log {
    std::string msg;

    Log(std::string _msg = "") : msg(_msg) {
    };

    void log() {
        std::cout << msg;
    }

    void logln() {
        std::cout << msg << std::endl;
    }
};

Log executeStatement(const std::string sql) {
    std::istringstream iss(sql);
    Tokenizer tokenizer(iss);
    tokenizer.tokenize();

    std::stringstream theOutput;
    MainController mainController(theOutput);
    Statement *statement = mainController.parser(tokenizer);
    if (statement == nullptr) {
        throw std::runtime_error("Failed to parse the statement");
    } else {
        statement->execute();
    }

    std::string msg = mainController.getOutput().c_str();
    return Log(msg);
}

void prehandle() {
    executeStatement("DROP DATABASE test_query;");
    executeStatement("CREATE DATABASE test_query;");
    executeStatement("USE test_query;");

    executeStatement("BEGIN TRANSACTION;");
    executeStatement("CREATE TABLE A (customer_id INT PRIMARY KEY,name VARCHAR(50));");
    executeStatement("CREATE TABLE B (order_id INT PRIMARY KEY,customer_id INT,product VARCHAR(50));");
    executeStatement("CREATE TABLE C (delivery_id INT PRIMARY KEY,order_id INT,status VARCHAR(50));");
    executeStatement("COMMIT;");

    executeStatement("BEGIN TRANSACTION;");
    executeStatement("INSERT INTO A VALUES (1, 'Alice');");
    executeStatement("INSERT INTO A VALUES (2, 'Bob');");
    executeStatement("INSERT INTO A VALUES (3, 'Calinton');");

    executeStatement("INSERT INTO B VALUES (101, 1, 'Laptop');");
    executeStatement("INSERT INTO B VALUES (102, 3, 'Smartphone');");

    executeStatement("INSERT INTO C VALUES (1, 101, 'Delivered');");
    executeStatement("INSERT INTO C VALUES (2, 104, 'In Progress');");

    executeStatement("COMMIT;");
}

TEST(StatementTest, SelectStatement) {
    prehandle();
    executeStatement("BEGIN TRANSACTION;");
    executeStatement("UPDATE A SET name = concat(A.name, 'xxx') where A.customer_id <> 2;").logln();
    executeStatement("SELECT * FROM A;").logln();
    executeStatement("COMMIT;");
    executeStatement("BEGIN TRANSACTION;");
    executeStatement("DELETE FROM A WHERE A.name not like '%xxx';").logln();
    executeStatement("SELECT * FROM A;").logln();
    executeStatement("COMMIT;");
}


