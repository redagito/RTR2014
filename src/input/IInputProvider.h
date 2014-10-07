#pragma once

class IInputListener;

/**
 * \brief Input provider interface.
 * Represents an abstract source. The input is handled by registered input handlers.
 */
class IInputProvider
{
public:
	virtual ~IInputProvider();
	virtual void addInputListener(IInputListener* listener) = 0;
	virtual void removeInputListener(IInputListener* listener) = 0;
};
