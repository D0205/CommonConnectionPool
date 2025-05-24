#include "CommonConnectionPool.h"
#include "public.h"

// 线程安全的懒汉单例函数接口
ConnectionPool* ConnectionPool::getConnectionPool() {
	static ConnectionPool pool; // lock 和 unlock;
	return &pool;
}

bool ConnectionPool::loadConfigFile() {
	FILE* pf = fopen("mysql.ini", "r");
	if (pf == nullptr) {
		LOG("mysql.ini file is not exist!");
		return false;
	}
	while (!feof(pf)) {
		char line[1024] = { 0 };
		fgets(line, 1024, pf);
		string str = line;
		int idx = str.find("=", 0);
		if (idx == -1) {
			continue;
		}
		// password=05.wypscnkjzzdzj
		int endidx = str.find('\n', idx);
		string key = str.substr(0, idx);
		string value = str.substr(idx + 1, endidx - idx - 1);

		if (key == "ip") {
			_ip = value;
		}
		else if (key == "port") {
			_port = atoi(value.c_str());
		}
		else if (key == "username") {
			_username = value;
		}
		else if (key == "password") {
			_password = value;
		}
		else if (key == "dbname") {
			_dbname = value;
		}
		else if (key == "initSize")
		{
			_initSize = atoi(value.c_str());
		}
		else if (key == "maxSize") {
			_maxSize = atoi(value.c_str());
		}
		else if (key == "maxIdleTime") {
			_maxIdleTime = atoi(value.c_str());
		}
		else if (key == "connectionTimeout") {
			_connectionTimeout = atoi(value.c_str());
		}
	}
	return true;
}


// 线程池的构造
ConnectionPool::ConnectionPool() {
	// 加载配置项
	if (!loadConfigFile()) {
		return;
	}
	// 创建初始连接数量
	for (int i = 0; i < _initSize; ++i) {
		Connection* p = new Connection();
		p->connect(_ip, _port, _username, _password, _dbname);
		p->refreshAliveTime();
		_connectionQue.push(p);
		_connectionCnt++;
	}

	// 启动一个新的线程，作为连接的生产者
	thread produce(std::bind(&ConnectionPool::produceConnectionTask, this));
	produce.detach();

	// 启动一个新的定时线程，扫描超过maxIdleTime时间的空闲连接，对于多余的连接进行回收
	thread scanner(std::bind(&ConnectionPool::scannerConnectionTask, this));
	scanner.detach();
}

void ConnectionPool::produceConnectionTask() {
	for (;;) {
		unique_lock<mutex> lock(_queueMutex);
		while (!_connectionQue.empty()) {
			cv.wait(lock); // 队列不空，此处生产线程进入等待状态
		}
		// 连接数量没有达到上线，继续创建新的连接
		if (_connectionCnt < _maxSize) {
			Connection* p = new Connection();
			p->connect(_ip, _port, _username, _password, _dbname);
			_connectionQue.push(p);
			_connectionCnt++;
		}
		cv.notify_all(); // 通知消费者线程可以消费了
	}
}

// 给外部提供接口，从连接池中获取一个可用的空闲的连接
shared_ptr<Connection> ConnectionPool::getConnection() {
	unique_lock<mutex> lock(_queueMutex);
	while (_connectionQue.empty()) {
		if (cv_status::timeout == cv.wait_for(lock, std::chrono::milliseconds(_connectionTimeout))) {
			if (_connectionQue.empty()) {
				LOG("获取空闲连接超时，获取连接失败...");
				return nullptr;
			}
		}
		
	}
	//shared_ptr析构时， 会把connection资源直接delete，相当于调用connection的析构函数，connection就close了，
	//这里需要自定义shared_ptr的资源释放方式，把connection直接归还到queue中
	shared_ptr<Connection> sp(_connectionQue.front(),
		[&](Connection* pcon){
			// 这里在服务器应用线程调用的，所以一定要考虑队列的线程安全操作
			unique_lock<mutex> lock(_queueMutex);
			pcon->refreshAliveTime();
			_connectionQue.push(pcon);
		});
	_connectionQue.pop();
	//if (_connectionQue.empty()) {
	//	// 谁消费了队列中的最后一个connection，谁负责通知生产者生产连接
	//	cv.notify_all();
	//}
	cv.notify_all(); //消费完以后通知生产者线程检查一下，如果队列为空了，生产新的连接
	return sp;
}

//扫描超过maxIdleTime时间的空闲连接，对于多余的连接进行回收
void ConnectionPool::scannerConnectionTask() {
	for (;;) {
		// 通过sleep，模拟定时
		this_thread::sleep_for(std::chrono::seconds(_maxIdleTime));
		// 扫描整个队列，释放多余连接，一定要添加互斥锁
		unique_lock<mutex> lock(_queueMutex);
		while (_connectionCnt > _initSize) {
			Connection* p = _connectionQue.front();
			if (p->getAliveTime() > (_maxIdleTime * 1000)) {
				// 队头不超过，其他一定不超过
				_connectionQue.pop();
				_connectionCnt--;
				delete p; // 调用~connection释放连接
			}
		}
	}
}