#include <string>
#include <vector>

namespace Command {
	typedef enum redType {
		RD_NONE = 0, RD_IN, RD_OUT, RD_APPEND
	} Redirection_te;

	typedef struct command {
		std::vector<std::string> args;
		Redirection_te redirection;
		std::string fileName;
	} Command_ts;

	void process(std::string [], size_t);
	void print(Command_ts cmd);
}
