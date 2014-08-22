#pragma once

#include <memory>

// abstract system interface
class isystem
{
public:
	isystem(int priority);
	virtual ~isystem();
	virtual void start() = 0;
	virtual void end() = 0;
	virtual void update(double t) = 0;

	int get_priority() const { return priority_; }
private:
	int priority_;
};

typedef std::shared_ptr<isystem> isystem_ptr;
