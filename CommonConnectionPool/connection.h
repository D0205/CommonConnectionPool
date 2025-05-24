#pragma once
#include <string>
#include <mysql.h>
#include <ctime>
using namespace std;
// 实现MySQL数据库的连接操作
class Connection {
public:
	// 初始化数据库连接
	Connection();
	// 释放数据库连接资源
	~Connection();
	bool connect(string ip,
		unsigned short port,
		string users,
		string password,
		string dbname);
	// 更新操作 insert delete update
	bool update(string sql);
	// 查询操作
	MYSQL_RES* query(string sql);
	// 刷新连接的起始的空闲时间点
	void refreshAliveTime() {
		_alivetime = clock();
	}
	// 返回存活的时间
	clock_t getAliveTime() { return clock() - _alivetime; }
private:
	MYSQL* _conn; //表示和MySQL Server的一条连接
	clock_t _alivetime; // 进入空闲状态后的起始存活时间
};