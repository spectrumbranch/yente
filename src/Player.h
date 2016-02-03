#ifndef PLAYER_H
#define PLAYER_H

#include <vector>

// For typedef
class Player;
typedef std::vector<Player> PlayerContainer;

class Player
{
	public:
		Player(const unsigned int& id, const unsigned int& level);
		~Player();

		unsigned int getId() const;
		unsigned int getLevel() const;

	private:
		unsigned int id_;
		unsigned int level_;
		Player();
};

inline bool operator<(const Player& left, const Player& right)
{
	return left.getLevel() < right.getLevel();
}

inline bool operator>(const Player& left, const Player& right)
{
	return left.getLevel() > right.getLevel();
}

#endif //PLAYER_H
