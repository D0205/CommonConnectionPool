#pragma once
#include <iostream>
#include <string>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <thread>
#include <memory>
#include <functional>
#include <ctime>
#include "connection.h"
using namespace std;
// ʵ�����ӳ�ģ��

class ConnectionPool {
public:
	// ��ȡ���ӳض���ʵ��
	static ConnectionPool* getConnectionPool();
	// ���ⲿ�ṩ�ӿڣ������ӳ��л�ȡһ�����õĿ��е�����
	shared_ptr<Connection> getConnection(); // �Զ���ɾ���������������Զ��黹
private:
	// ���� #1 ���캯��˽��
	ConnectionPool();
	// �������ļ��м���������
	bool loadConfigFile();

	// �����ڶ������߳��У�ר�Ÿ�������������
	void produceConnectionTask();

	// ����һ���µĶ�ʱ�̣߳�ɨ�賬��maxIdleTimeʱ��Ŀ������ӣ����ڶ�������ӽ��л���
	void scannerConnectionTask();

	string _ip;   //mysql��ip��ַ
	unsigned short _port;  //mysql�Ķ˿ں�3306
	string _username; //mysql���û���
	string _password;// mysql������
	string _dbname; // ���ӵ����ݿ�����
	int _initSize;   // ���ӳصĳ�ʼ������
	int _maxSize;    // ���ӳص����������
	int _maxIdleTime;  // ���ӳص�������ʱ��
	int _connectionTimeout;  // ���ӳػ�ȡ���ӵĳ�ʱʱ��

	queue<Connection*> _connectionQue;  // �洢mysql���ӵĶ���
	mutex _queueMutex; // ά�����Ӷ��е��̰߳�ȫ������
	atomic_int _connectionCnt; // ��¼������������connection���ӵ�������
	condition_variable cv; // ���������������������������̺߳����������̵߳�ͨ��
	
};