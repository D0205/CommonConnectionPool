#pragma once
#include <string>
#include <mysql.h>
#include <ctime>
using namespace std;
// ʵ��MySQL���ݿ�����Ӳ���
class Connection {
public:
	// ��ʼ�����ݿ�����
	Connection();
	// �ͷ����ݿ�������Դ
	~Connection();
	bool connect(string ip,
		unsigned short port,
		string users,
		string password,
		string dbname);
	// ���²��� insert delete update
	bool update(string sql);
	// ��ѯ����
	MYSQL_RES* query(string sql);
	// ˢ�����ӵ���ʼ�Ŀ���ʱ���
	void refreshAliveTime() {
		_alivetime = clock();
	}
	// ���ش���ʱ��
	clock_t getAliveTime() { return clock() - _alivetime; }
private:
	MYSQL* _conn; //��ʾ��MySQL Server��һ������
	clock_t _alivetime; // �������״̬�����ʼ���ʱ��
};