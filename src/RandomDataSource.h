#ifndef RANDOMDATASOURCE_H
#define RANDOMDATASOURCE_H

#include <limits>

#include "DataSource.h"

class RandomDataSource : public DataSource
{
	public:
		RandomDataSource(const unsigned int& maxLevel = std::numeric_limits<unsigned int>::max(), const unsigned int& totalPlayers = 1000);
		
		~RandomDataSource();
		bool retrievePlayerData(PlayerContainer& playerContainer, const unsigned int& nPlayers);

	private:
		unsigned int playerId_;
		const unsigned int maxLevel_;
		const unsigned int totalPlayers_;

		static bool playerSortFunction(const Player& left, const Player& right);
		
};

#endif //RANDOMDATASOURCE_H
