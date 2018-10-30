/*
	Abstract class for all clocked entities.
*/

#ifndef _CLOCK_ENT_HPP
#define _CLOCK_ENT_HPP

class ClockedEntity {
public:
	virtual void tick(void){}
	virtual void tock(void){}
	virtual void flush(void){}
};

#endif