#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <liblightnvm_cli.h>

int erase(struct nvm_cli *cli)
{
	struct nvm_cli_cmd_args *args = &cli->args;
	const int PMODE = cli->evars.pmode;
	struct nvm_ret ret = {0,0};
	ssize_t err = 0;

	printf("** nvm_addr_erase(...) : pmode(%s)\n", nvm_pmode_str(PMODE));
	for (int i = 0; i < args->naddrs; ++i) {
		nvm_addr_pr(args->addrs[i]);
	}

	err = nvm_addr_erase(args->dev, args->addrs, args->naddrs, PMODE,
			     &ret);
	if (err) {
		perror("nvm_addr_erase");
		nvm_ret_pr(&ret);
	}

	return err ? 1 : 0;
}

int _write(struct nvm_cli *cli, int with_meta)
{
	struct nvm_cli_cmd_args *args = &cli->args;
	const int PMODE = cli->evars.pmode;
	struct nvm_ret ret = {0,0};
	ssize_t err = 0;

	int buf_nbytes = args->naddrs * args->geo->sector_nbytes;
	char *buf = NULL;
	int meta_tbytes = args->naddrs * args->geo->meta_nbytes;
	char *meta = NULL;

	buf = nvm_buf_alloc(args->geo, buf_nbytes);	// data buffer
	if (!buf)
		return ENOMEM;
	nvm_buf_fill(buf, buf_nbytes);

	if (with_meta) {				// metadata buffer
		meta = nvm_buf_alloc(args->geo, meta_tbytes);
		if (!meta) {
			free(buf);
			return ENOMEM;
		}
		for (int i = 0; i < meta_tbytes; ++i)
			meta[i] = (i / args->naddrs) % args->naddrs + 65;
	}

	printf("** nvm_addr_write(...) : pmode(%s)\n", nvm_pmode_str(PMODE));
	for (int i = 0; i < args->naddrs; ++i) {
		nvm_addr_pr(args->addrs[i]);
	}

	if (cli->evars.buf_pr) {
		printf("** Writing buffer:\n");
		nvm_buf_pr(buf, buf_nbytes);
	}
	if (meta && cli->evars.meta_pr) {
		printf("** Writing meta:\n");
		nvm_buf_pr(meta, meta_tbytes);
	}

	err = nvm_addr_write(args->dev, args->addrs, args->naddrs, buf, meta,
			     PMODE, &ret);
	if (err) {
		perror("nvm_addr_write");
		nvm_ret_pr(&ret);
	}

	free(buf);
	free(meta);

	return err ? 1 : 0;
}

int write(struct nvm_cli *cli)
{
	return _write(cli, 0);
}

int write_wm(struct nvm_cli *cli)
{
	return _write(cli, 1);
}

int _read(struct nvm_cli *cli, int with_meta)
{
	struct nvm_cli_cmd_args *args = &cli->args;
	const int PMODE = cli->evars.pmode;
	struct nvm_ret ret = {0,0};
	ssize_t err = 0;

	int buf_nbytes = args->naddrs * args->geo->sector_nbytes;
	char *buf = NULL;
	int meta_tbytes = args->naddrs * args->geo->meta_nbytes;
	char *meta = NULL;

	buf = nvm_buf_alloc(args->geo, buf_nbytes);	// data buffer
	if (!buf)
		return ENOMEM;

	if (with_meta) {				// metadata buffer
		meta = nvm_buf_alloc(args->geo, meta_tbytes);
		if (!meta) {
			free(buf);
			return ENOMEM;
		}
		memset(meta, 0, meta_tbytes);
	}

	printf("** nvm_addr_read(...) : pmode(%s)\n", nvm_pmode_str(PMODE));
	for (int i = 0; i < args->naddrs; ++i) {
		nvm_addr_pr(args->addrs[i]);
	}

	if (meta && cli->evars.meta_pr) {
		printf("** Before read meta_tbytes(%d) meta:\n", meta_tbytes);
		nvm_buf_pr(meta, meta_tbytes);
	}

	err = nvm_addr_read(args->dev, args->addrs, args->naddrs, buf, meta,
			    PMODE, NULL);
	if (err) {
		perror("nvm_addr_read");
		nvm_ret_pr(&ret);
	}
	
	if (cli->evars.buf_pr) {
		printf("** Read buffer:\n");
		nvm_buf_pr(buf, buf_nbytes);
	}
	if (meta && cli->evars.meta_pr) {
		printf("** After read meta_tbytes(%d) meta:\n", meta_tbytes);
		nvm_buf_pr(meta, meta_tbytes);
	}

	free(buf);
	free(meta);

	return err;
}

int read(struct nvm_cli *cli)
{
	return _read(cli, 0);
}

int read_wm(struct nvm_cli *cli)
{
	return _read(cli, 1);
}

int cmd_fmt(struct nvm_cli *cli)
{
	for (int i = 0; i < cli->args.naddrs; ++i)
		nvm_addr_pr(cli->args.addrs[i]);

	return 0;
}

int cmd_gen2dev(struct nvm_cli *cli)
{
	struct nvm_cli_cmd_args *args = &cli->args;

	for (int i = 0; i < args->naddrs; ++i) {
		printf("gen-addr"); nvm_addr_pr(args->addrs[i]);
		printf("dev-addr(0x%016lx)\n",
		       nvm_addr_gen2dev(args->dev, args->addrs[i]));
	}

	return 0;
}

int cmd_gen2lba(struct nvm_cli *cli)
{
	struct nvm_cli_cmd_args *args = &cli->args;

	for (int i = 0; i < args->naddrs; ++i) {
		printf("gen-addr"); nvm_addr_pr(args->addrs[i]);
		printf("lba-addr(%064ld)\n",
		       nvm_addr_gen2lba(args->dev, args->addrs[i]));
	}

	return 0;
}

int cmd_gen2off(struct nvm_cli *cli)
{
	struct nvm_cli_cmd_args *args = &cli->args;

	for (int i = 0; i < args->naddrs; ++i) {
		printf("gen-addr"); nvm_addr_pr(args->addrs[i]);
		printf("off-addr(%064ld)\n",
		       nvm_addr_gen2off(args->dev, args->addrs[i]));
	}

	return 0;
}

int cmd_dev2gen(struct nvm_cli *cli)
{
	struct nvm_cli_cmd_args *args = &cli->args;

	for (int i = 0; i < args->ndec_vals; ++i) {
		printf("dev-addr(%064ld)\n", args->dec_vals[i]);
		nvm_addr_pr(nvm_addr_dev2gen(args->dev, args->dec_vals[i]));
	}

	return 0;
}

int cmd_lba2gen(struct nvm_cli *cli)
{
	struct nvm_cli_cmd_args *args = &cli->args;

	for (int i = 0; i < args->ndec_vals; ++i) {
		printf("lba-addr(%064ld)\n", args->dec_vals[i]);
		nvm_addr_pr(nvm_addr_lba2gen(args->dev, args->dec_vals[i]));
	}

	return 0;
}

int cmd_off2gen(struct nvm_cli *cli)
{
	struct nvm_cli_cmd_args *args = &cli->args;

	for (int i = 0; i < args->ndec_vals; ++i) {
		printf("off-addr(%064ld)\n", args->dec_vals[i]);
		nvm_addr_pr(nvm_addr_off2gen(args->dev, args->dec_vals[i]));
	}

	return 0;
}

/**
 * Command-line interface (CLI) boiler-plate
 */

/* Define commands */
static struct nvm_cli_cmd cmds[] = {
	{"erase",	erase,		NVM_CLI_ARG_ADDR_LIST, NVM_CLI_OPT_NONE},
	{"write",	write,		NVM_CLI_ARG_ADDR_LIST, NVM_CLI_OPT_NONE},
	{"read",	read,		NVM_CLI_ARG_ADDR_LIST, NVM_CLI_OPT_NONE},
	{"write_wm",	write,		NVM_CLI_ARG_ADDR_LIST, NVM_CLI_OPT_NONE},
	{"read_wm",	read,		NVM_CLI_ARG_ADDR_LIST, NVM_CLI_OPT_NONE},
	{"from_hex",	cmd_fmt,	NVM_CLI_ARG_ADDR_LIST, NVM_CLI_OPT_NONE},
	{"from_geo",	cmd_fmt,	NVM_CLI_ARG_ADDR_SEC, NVM_CLI_OPT_NONE},
	{"gen2dev",	cmd_gen2dev,	NVM_CLI_ARG_ADDR_LIST, NVM_CLI_OPT_NONE},
	{"gen2lba",	cmd_gen2lba,	NVM_CLI_ARG_ADDR_LIST, NVM_CLI_OPT_NONE},
	{"gen2off",	cmd_gen2off,	NVM_CLI_ARG_ADDR_LIST, NVM_CLI_OPT_NONE},
	{"dev2gen",	cmd_dev2gen,	NVM_CLI_ARG_VALUE_LIST, NVM_CLI_OPT_NONE},
	{"lba2gen",	cmd_lba2gen,	NVM_CLI_ARG_VALUE_LIST, NVM_CLI_OPT_NONE},
	{"off2gen",	cmd_off2gen,	NVM_CLI_ARG_VALUE_LIST, NVM_CLI_OPT_NONE},
};

/* Define the CLI */
static struct nvm_cli cli = {
	.title = "NVM address (nvm_addr_*)",
	.description = ""
		"from_(hex|geo):\n"
		" Construct physical address on generic format from hex/geo\n\n"
		"gen2(dev|lba|off):\n"
		" Convert FROM generic format TO device format, LBA, and byte offset\n\n"
		"(dev|lba|off)2gen:\n"
		" Convert TO generic format FROM device format, LBA, and byte offset\n\n"
		"Vector IO: erase/read/write/read_wm/write_wm\n"
		" Write out-of-bound-area using (read|write)_wm"
	,
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
