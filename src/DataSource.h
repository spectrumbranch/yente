#ifndef DATASOURCE_H
#define DATASOURCE_H

#include "Player.h"

class DataSource
{
	public:
		DataSource() {}
		virtual ~DataSource() {}

		// Eventually change return type to an enum for state
		virtual bool retrievePlayerData(PlayerContainer& playerContainer, const unsigned int& nPlayers) = 0;

};
#endif // DATASOURCE_H