#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <liblightnvm.h>
#include <liblightnvm_cli.h>

int info(struct nvm_cli *cli)
{
	printf("** Device information  -- nvm_dev_pr **\n");
	nvm_dev_pr(cli->args.dev);

	return 0;
}

/**
 * Command-line interface (CLI) boiler-plate
 */

/* Define commands */
static struct nvm_cli_cmd cmds[] = {
	{"info", info, NVM_CLI_ARG_DEV_PATH, NVM_CLI_OPT_NONE},
};

/* Define the CLI */
static struct nvm_cli cli = {
	.title = "NVM Device (nvm_dev_*)",
	.description = "Retrieve device information",
	.cmds = cmds,
	.ncmds = sizeof(cmds) / sizeof(cmds[0]),
};

/* Initialize and run */
int main(int argc, char **argv)
{
	if (nvm_cli_init(&cli, argc, argv) < 0) {
		switch (errno) {
		case EINVAL:
			nvm_cli_usage_pr(&cli);
			break;
		default:
			perror("FAILED: ");
			break;
		}
		return 1;
	}

	if (nvm_cli_run(&cli) < 0)
		perror(cli.cmd.name);
	
	nvm_cli_destroy(&cli);

	return 0;
}
