TURNS_PER_DAY = 3000
DAYS_PER_YEAR = 200
MONTHS_PER_YEAR = 10
HOURS_PER_DAY = 20
MINUTES_PER_HOUR = 60
SECONDS_PER_MINUTE = 60
SECONDS_PER_HOUR = SECONDS_PER_MINUTE*MINUTES_PER_HOUR
MINUTES_PER_DAY=MINUTES_PER_HOUR*HOURS_PER_DAY
TURNS_PER_YEAR = DAYS_PER_YEAR * TURNS_PER_DAY
TURNS_PER_HOUR = TURNS_PER_DAY / HOURS_PER_DAY
TURNS_PER_MINUTE = TURNS_PER_HOUR / MINUTES_PER_HOUR
#TURNS_PER_SECOND = TURNS_PER_HOUR / SECONDS_PER_HOUR
SECONDS_PER_TURN = (HOURS_PER_DAY*SECONDS_PER_HOUR)/3000

days_per_month = [20, 19, 21, 20, 20, 19, 19, 21, 21, 20]
cumulative_turns_per_month = [0, 
    20 * TURNS_PER_DAY,
    39 * TURNS_PER_DAY,
    60 * TURNS_PER_DAY,
    80 * TURNS_PER_DAY,
    100 * TURNS_PER_DAY,
    119 * TURNS_PER_DAY,
    138 * TURNS_PER_DAY,
    159 * TURNS_PER_DAY,
    180 * TURNS_PER_DAY,
    200 * TURNS_PER_DAY,]

month_names = [
    "Jujalst",
    "Ibasis",
    "Keb",
    "Qrelm",
    "Eduth",
    "Greiq",
    "Qleb",
    "Vuwak",
    "Xeps",
    "Drajans",
    ]

day_phases = [150, 550, 650, 1400, 1500, 2250, 2350, 2550, 2850, 3000]
phases_of_the_day = [
		"Midnight",
		"Early Morning",
		"Dawn",
		"Mid-morning",
		"Noon",
		"Afternoon",
		"Dusk",
		"Evening",
		"Night",
		"Midnight",
    ]
    
def get_year(turns):
    return turns/TURNS_PER_YEAR

def get_month(turns):
    mnth = turns % TURNS_PER_YEAR
    for n in range(0,MONTHS_PER_YEAR+1):
        if mnth < cumulative_turns_per_month[n]:
            return n
    assert(False)
    
def get_day(turns):
    return (((turns % TURNS_PER_YEAR)- cumulative_turns_per_month[get_month(turns) - 1])/TURNS_PER_DAY )+1
    
def get_hour(turns):
    turns *= SECONDS_PER_TURN
    return (turns / SECONDS_PER_HOUR) % HOURS_PER_DAY

def get_minute(turns):
    turns *= SECONDS_PER_TURN
    return (turns/SECONDS_PER_MINUTE % MINUTES_PER_HOUR)

def get_seconds(turns):
    turns *= SECONDS_PER_TURN
    return turns%SECONDS_PER_MINUTE
    
def get_tod_str(turns):
    turns = turns % TURNS_PER_DAY
    cnt = 0
    for n in day_phases:
        #print n, turns
        if turns <= n:
            return phases_of_the_day[cnt]
        cnt += 1
    assert(False)

def printable(turns):
    hh = get_hour(turns)
    mm = get_minute(turns)
    DD = get_day(turns)
    MM = month_names[get_month(turns)-1]
    YY = get_year(turns)    
    tod_str = get_tod_str(turns)
    print "%02d/%s/%04d %02d:%02d %s" % (DD,MM,YY,hh,mm,tod_str)

if __name__ == '__main__':
    for n in range(0,TURNS_PER_DAY*2+1,1):
        printable(n)
    #printable(0)
    #for n in cumulative_turns_per_month:
    #    printable(n)
    #printable(540000)
