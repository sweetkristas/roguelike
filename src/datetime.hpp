#pragma once

#include <string>

// Get in-game time. We let game-time increment at a rate of 3000 turns per day, split
// at dat 1800, night 1200. At least initially anyway. At the automatic turn rate of
// one turn per 200mS(real time) it gives a day length of 10 minutes. 6 minutes of day
// and 4 minutes night (in real time).

class datetime
{
public:
	datetime(int turns);
	
	int get_hour();
	int get_minute();
	int get_second();

	int get_day();
	int get_month();
	int get_year();
	
	int is_day();
	int is_night();

	std::string printable();
private:
	int time_of_day_index();
	int turns_;
};
