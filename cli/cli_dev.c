#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <liblightnvm.h>
#include "liblightnvm_cli.h"

int info(NVM_CLI_CMD_ARGS *args)
{
	printf("** Device information  -- nvm_dev_pr **\n");
	nvm_dev_pr(args->dev);

	return 0;
}

//
// Remaining code is CLI boiler-plate
//
static NVM_CLI_CMD cmds[] = {
	{"info", info, NVM_CLI_ARG_NONE, NULL},
};

static int ncmds = sizeof(cmds) / sizeof(cmds[0]);

int main(int argc, char **argv)
{
	NVM_CLI_CMD *cmd;
	int ret = 0;

	cmd = nvm_cli_setup(argc, argv, cmds, ncmds);
	if (cmd) {
		ret = cmd->func(cmd->args);
	} else {
		nvm_cli_usage(argv[0], "NVM Device (nvm_dev_*)", cmds, ncmds);
		ret = 1;
	}
	
	nvm_cli_teardown(cmd);

	return ret != 0;
}

