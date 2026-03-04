#include "Utilities.h"

#include <signal.h>

namespace
{

void sigpipe_handler(int signal)
{
	(void)signal;
}

}	 // namespace

void ConfigureSignals()
{
	// On Linux we can receive sigpipe when rcon server dies and we trying to send additional
	// commands. We should just ignore this to avoid crashing.
	struct sigaction sh;
	struct sigaction osh;

	sh.sa_handler = &sigpipe_handler;
	// Restart interrupted system calls.
	sh.sa_flags = SA_RESTART;

	// Block every signal during the handler.
	sigemptyset(&sh.sa_mask);
	sigaction(SIGPIPE, &sh, &osh);
}
