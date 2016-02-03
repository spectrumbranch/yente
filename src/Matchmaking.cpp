// Matchmaking.cpp : Defines the entry point for the console application.
//
#include <vector>
#include <iostream>
#include <assert.h>
#include <algorithm>
#include <fstream>
#include <time.h>

#include "MySqlDataSource.h"
#include "RandomDataSource.h"
#include "Player.h"

#define PLAYER_BATCH_SIZE 1024

struct Misfit
{
	Misfit(const unsigned int& index, const unsigned int& level) : playerIndex_(index), playerLevel_(level) {}
	~Misfit() {}

	unsigned int playerIndex_;
	unsigned int playerLevel_;
};

#define MISFIT_CONTAINER_TYPE std::pair<Misfit, Misfit>

bool misfitSortFunction(const MISFIT_CONTAINER_TYPE& left, const MISFIT_CONTAINER_TYPE& right)
{
	std::pair<unsigned int, unsigned int> leftSortPair((left.second.playerLevel_ - left.first.playerLevel_),  (std::numeric_limits<unsigned int>::max() - left.first.playerIndex_ - left.second.playerIndex_));
	std::pair<unsigned int, unsigned int> rightSortPair((right.second.playerLevel_ - right.first.playerLevel_), (std::numeric_limits<unsigned int>::max() - right.first.playerIndex_ - right.second.playerIndex_));

	// If the left pair's player level difference is less than the right pair and their original ordering is at a higher index
	return leftSortPair < rightSortPair; 	
}

int main(int argc, char* argv[])
{
	if(argc < 5)
	{
		std::cout << std::endl 
		          << "Error: Incorrect number of arguments" << std::endl << std::endl
		          << "Usage: Yente <DB IP:DB Port> <DB User> <DB password> <DB Name> <Output File>"
		          << std::endl << std::endl;
		return 1;
	}	 

	srand(time(NULL));
	MySqlDataSource mySql(argv[1], argv[2], argv[3], argv[4]);
	//("127.0.0.1:3306", "melon", "d403eqLz1#", "testbed");
	
	RandomDataSource dataSource;

	// Reserve all memory
	unsigned int* matchingPlayers = new unsigned int[PLAYER_BATCH_SIZE];
	
	PlayerContainer players;
	players.reserve(PLAYER_BATCH_SIZE);

	std::vector<MISFIT_CONTAINER_TYPE > misfitPlayers;
	misfitPlayers.reserve(PLAYER_BATCH_SIZE);

	//Special bitset specialization
	std::vector<bool> playerMatched(PLAYER_BATCH_SIZE, false);
	playerMatched.reserve(PLAYER_BATCH_SIZE);

	std::vector<unsigned int> outcastPlayers;
	outcastPlayers.reserve(PLAYER_BATCH_SIZE);

	size_t matchingIndex = 0;
	size_t startIndex = 0;
	size_t endIndex = 0;
	size_t nextPlayerIndex = 0;
	size_t currentRangeSize = 0;
	int randIndex = 0; 
    	unsigned int swapId = 0;
	unsigned int totalPlayers = 0;

	std::ofstream outputFile(argv[5]);

	unsigned int DEBUG_PERFECT_MATCHES = 0;
	unsigned int DEBUG_MISFIT_MATCHES = 0;
	
	//dataSource

	unsigned int dbPulls = 0;
	
	// Get from the database sorted in level order
	while(mySql.retrievePlayerData(players, PLAYER_BATCH_SIZE))
	{
		++dbPulls;
		
		// Reset tracking variables
		matchingIndex = 0;
		startIndex = 0;
		endIndex = 0;
		nextPlayerIndex = 0;
		currentRangeSize = 0;
		randIndex = 0; 
		swapId = 0;

		//Until no more data in the current block
		while(endIndex < players.size())
		{
			endIndex = startIndex;
			nextPlayerIndex = endIndex + 1;
			currentRangeSize = 0;

			// Get the range for the current level
			while(endIndex < players.size() && players.at(startIndex).getLevel() == players.at(endIndex).getLevel())
			{
				// Copy the current player into the reserve buffer since we are iterating anyways
				matchingPlayers[endIndex - startIndex] = endIndex; //players.at(endIndex).getId();
			
				// Keep the range index correct
				endIndex = nextPlayerIndex;
				++nextPlayerIndex;
				++currentRangeSize;
			}

			// If there are any matching entries
			if(currentRangeSize)
			{
				// Shuffle the elements by iterating backwards
				for(size_t shuffleIndex = currentRangeSize - 1; shuffleIndex > 0; --shuffleIndex)
				{
					// Get a random index from the beginning to the current
					randIndex = rand() % (shuffleIndex + 1);

					// Swap the entries
					swapId = matchingPlayers[shuffleIndex];
					matchingPlayers[shuffleIndex] = matchingPlayers[randIndex];
					matchingPlayers[randIndex] = swapId;
				}

				// Write pairs to file
				for(matchingIndex = 0; matchingIndex + 1 < currentRangeSize; matchingIndex += 2)
				{
					//Error condition if this pair has already been matched
					assert(!playerMatched.at(matchingPlayers[matchingIndex]));
					assert(!playerMatched.at(matchingPlayers[matchingIndex + 1]));

					// Record that this player was matched
					playerMatched.at(matchingPlayers[matchingIndex]) = true;
					playerMatched.at(matchingPlayers[matchingIndex + 1]) = true;

					totalPlayers += 2;
					DEBUG_PERFECT_MATCHES += 2;

					outputFile << "< " << players.at(matchingPlayers[matchingIndex]).getId() << "[" << players.at(matchingPlayers[matchingIndex]).getLevel() << "] , " << players.at(matchingPlayers[matchingIndex + 1]).getId() << "[" << players.at(matchingPlayers[matchingIndex + 1]).getLevel() << "] > " << std::endl;  
				}
			}

			// hold any misfits for final evaluation
			// This occurs when there is an odd number of matches
			if(matchingIndex == 0 || currentRangeSize - matchingIndex == 1)
			{
				++totalPlayers;

				// Error condition if this player has not already been matched
				assert(!playerMatched.at(matchingPlayers[matchingIndex]));

				// If there was a previous match
				if(!misfitPlayers.empty())
				{
					// Update the previous entry to take into account the current
					misfitPlayers.back().second.playerIndex_ = matchingPlayers[matchingIndex];
					misfitPlayers.back().second.playerLevel_ = players.at(matchingPlayers[matchingIndex]).getLevel();
				}

				// Place this misfit to be updated on the next iteration
				misfitPlayers.push_back(std::make_pair(Misfit(matchingPlayers[matchingIndex], players.at(matchingPlayers[matchingIndex]).getLevel()), 
					                    Misfit(std::numeric_limits<unsigned int>::max(), std::numeric_limits<unsigned int>::max())));
			}

			startIndex = endIndex;
		} // while there is more data in the current block

		// Deal with misfits

		// Sort all the entries only if the vector is not empty
		if(!misfitPlayers.empty())
		{	
			std::sort(misfitPlayers.begin(), misfitPlayers.end(), misfitSortFunction);
		}

		for(std::vector<MISFIT_CONTAINER_TYPE >::iterator misfitItr = misfitPlayers.begin();
			misfitItr != misfitPlayers.end();
			++misfitItr)
		{
			// Handle outcasted players
			if(misfitItr->second.playerIndex_ == std::numeric_limits<unsigned int>::max())
			{
				//Set the "first" player in the pair as an outcast if it is not matched before iterating for next potential pair.
				if(!playerMatched.at(misfitItr->first.playerIndex_) && (outcastPlayers.empty() || outcastPlayers.back() != misfitItr->first.playerIndex_))
				{
					outcastPlayers.push_back(misfitItr->first.playerIndex_);
				}
				
				continue;
			}

			// If neither of the players have already been matched
			if(!playerMatched.at(misfitItr->first.playerIndex_) && !playerMatched.at(misfitItr->second.playerIndex_))
			{
				// Record this pair match
				playerMatched.at(misfitItr->first.playerIndex_) = true;
				playerMatched.at(misfitItr->second.playerIndex_) = true;

				DEBUG_MISFIT_MATCHES+=2;
				outputFile << "===<" << players.at(misfitItr->first.playerIndex_).getId() << "[" << players.at(misfitItr->first.playerIndex_).getLevel() << "] , " << players.at(misfitItr->second.playerIndex_).getId() << "[" << players.at(misfitItr->second.playerIndex_).getLevel() << "]>===" << std::endl;
			}
			// Match was not made
			else
			{
				//Set the "first" player in the pair as an outcast if it is not matched before iterating for next potential pair.
				if(!playerMatched.at(misfitItr->first.playerIndex_) && (outcastPlayers.empty() || outcastPlayers.back() != misfitItr->first.playerIndex_))
				{
					outcastPlayers.push_back(misfitItr->first.playerIndex_);
				}
			}
			
		}

		// Empty the vector for use by the next block
		players.clear();
	} // while data has been retrieved for the next block
	
	unsigned int tester = outcastPlayers.size();

	delete[] matchingPlayers;

	return 0;
}

