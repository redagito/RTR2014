#pragma once

/**
 * \brief Input provider interface.
 * Represents an abstract source. The input is handled by registered input handlers.
 */
class IInputProvider
{
public:
	virtual ~IInputProvider();
};
