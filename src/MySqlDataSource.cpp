#include "MySqlDataSource.h"

#include <iostream>
#include <sstream>
#include <algorithm>

#include <cppconn/driver.h>
#include <cppconn/connection.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/metadata.h>
#include <cppconn/resultset_metadata.h>
#include <cppconn/exception.h>
#include <cppconn/warning.h>
	
//#define DBHOST "tcp://127.0.0.1:3306"
//#define USER "root"
//#define PASSWORD "admin"
//#define DATABASE "test"

#define NUMOFFSET 100
#define COLNAME 200

MySqlDataSource::MySqlDataSource(const std::string& url, const std::string& user, const std::string& password, const std::string& database) :
	driver_(0),
	connection_(0),
	satetment_(0),
	result_(0),
	url_(url),
	user_(user),
	password_(password),
	database_(database)
{
	driver_ = get_driver_instance();
		
	/* create a database connection using the Driver */
	connection_ = driver_->connect(url, user, password);

	/* turn off the autocommit */
	connection_->setAutoCommit(0);

	std::cout << "\nDatabase connection\'s autocommit mode = " << connection_->getAutoCommit() << std::endl;

	/* select appropriate database schema */
	connection_->setSchema(database);

	/* create a statement object */
	satetment_ = connection_->createStatement();


}


MySqlDataSource::~MySqlDataSource()
{
	if(connection_)
	{
		connection_->close();
	}

	delete connection_;
	delete satetment_;
	delete result_;

}


bool MySqlDataSource::retrievePlayerData(PlayerContainer& playerContainer, const unsigned int& nPlayers)
{
	if(satetment_)
	{
		// Only execute the query once
		if(!result_)
		{
			result_ = satetment_->executeQuery("SELECT * FROM `testbed`.`robots_yente_test5` ORDER BY `level` ASC");
			//result_ = satetment_->executeQuery("SELECT * FROM `testbed`.`robots_master` ORDER BY `level` ASC");
		}

		unsigned int playerCounter = 0;
		unsigned int playerId = 0;
		unsigned int playerLevel = 0;

		//result->rowsCount()

		std::stringstream num_converter;

		/* fetch the data : retrieve all the rows in the result set */
		while (result_->next() && playerCounter < nPlayers) 
		{
			// Empty the contents
			num_converter.str("");

			// Clear the state flags
			num_converter.clear();

			num_converter << result_->getString(1);

			if(num_converter >> playerId)
			{
				// Empty the contents
				num_converter.str("");

				// Clear the state flags
				num_converter.clear();

				num_converter << result_->getString(2);

				if(num_converter >> playerLevel)
				{
					// Player ID, Level
					playerContainer.push_back(Player(playerId, playerLevel));	
				}
				else
				{
					break;
				}
			}
			else
			{
				break;
			}

			++playerCounter;
		} 
	}

	// Sort all the entries only if the vector is not empty
	if(!playerContainer.empty())
	{	
		std::sort(playerContainer.begin(), playerContainer.end());
	}

	return !playerContainer.empty();
}
