#ifndef MYSQLDATASOURCE_H
#define MYSQLDATASOURCE_H

#include "DataSource.h"

#include <string>

namespace sql
{
	class Driver;
	class Connection;
	class Statement;
	class ResultSet;
}

class MySqlDataSource : public DataSource
{
	public:
		MySqlDataSource(const std::string& url, const std::string& user, const std::string& password, const std::string& database);
		~MySqlDataSource();

		bool retrievePlayerData(PlayerContainer& playerContainer, const unsigned int& nPlayers);
	private:
		sql::Driver* driver_;
		sql::Connection* connection_;
		sql::Statement* satetment_;
		sql::ResultSet* result_;

		const std::string url_;
		const std::string user_;
		const std::string password_;
		const std::string database_;
};

#endif //MYSQLDATASOURCE_H
