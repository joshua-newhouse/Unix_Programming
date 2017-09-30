#ifndef CLI_H
#define CLI_H

#include <string>

class CLI {
	private:
		const char prompt;

	public:
		CLI(const char p) : prompt(p)
		{};
		bool getInput(std::string [], size_t, size_t*);
};

#endif
