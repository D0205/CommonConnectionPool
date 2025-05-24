#include <iostream>
#include <ctime>
#include <thread>
#include "public.h"
#include "connection.h"
#include "CommonConnectionPool.h"
using namespace std;

int main()
{
    /*Connection conn;
    char sql[1024] = { 0 };
    sprintf(sql, "insert into user(name, age, sex) values ('%s', %d, '%s')",
        "zhangsan", 20, "male");
    conn.connect("127.0.0.1", 3306, "root", "05.wypscnkjzzdzj", "chat");
    conn.update(sql);*/
    clock_t begin = clock();
    //ConnectionPool* cp = ConnectionPool::getConnectionPool();
    thread t1([]() {
        ConnectionPool* cp = ConnectionPool::getConnectionPool();
        
        for (int i = 0; i < 1250; ++i) {
            char sql[1024] = { 0 };
            sprintf(sql, "insert into user(name, age, sex) values ('%s', %d, '%s')",
                "zhangsan", 20, "male");
            shared_ptr<Connection> sp = cp->getConnection();
            sp->update(sql);
        }
        });
    thread t2([]() {
        ConnectionPool* cp = ConnectionPool::getConnectionPool();

        for (int i = 0; i < 1250; ++i) {
            char sql[1024] = { 0 };
            sprintf(sql, "insert into user(name, age, sex) values ('%s', %d, '%s')",
                "zhangsan", 20, "male");
            shared_ptr<Connection> sp = cp->getConnection();
            sp->update(sql);
        }
        });
    thread t3([]() {
        ConnectionPool* cp = ConnectionPool::getConnectionPool();

        for (int i = 0; i < 1250; ++i) {
            char sql[1024] = { 0 };
            sprintf(sql, "insert into user(name, age, sex) values ('%s', %d, '%s')",
                "zhangsan", 20, "male");
            shared_ptr<Connection> sp = cp->getConnection();
            sp->update(sql);
        }
        });
    thread t4([]() {
        ConnectionPool* cp = ConnectionPool::getConnectionPool();

        for (int i = 0; i < 1250; ++i) {
            char sql[1024] = { 0 };
            sprintf(sql, "insert into user(name, age, sex) values ('%s', %d, '%s')",
                "zhangsan", 20, "male");
            shared_ptr<Connection> sp = cp->getConnection();
            sp->update(sql);
        }
        });
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    clock_t end = clock();
    std::cout << (end - begin) << "ms" << endl;
#if 0
    for (int i = 0; i < 1000; ++i) {
        /*Connection conn;
        char sql[1024] = { 0 };
        sprintf(sql, "insert into user(name, age, sex) values ('%s', %d, '%s')",
            "zhangsan", 20, "male");
        conn.connect("127.0.0.1", 3306, "root", "05.wypscnkjzzdzj", "chat");
        conn.update(sql);*/
        // Á¬½Ó³Ø
        shared_ptr<Connection> sp = cp->getConnection();
        char sql[1024] = { 0 };
        sprintf(sql, "insert into user(name, age, sex) values ('%s', %d, '%s')",
            "zhangsan", 20, "male");
        sp->update(sql);
    }
#endif
    return 0;
}