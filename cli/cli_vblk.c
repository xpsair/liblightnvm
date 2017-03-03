#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <liblightnvm_cli.h>

int erase(struct nvm_cli *cli)
{
	struct nvm_cli_cmd_args *args = &cli->args;
	ssize_t res = 0;

	for (int i = 0; i < args->naddrs; ++i) {
		struct nvm_vblk *vblk;
		
		vblk = nvm_vblk_alloc(args->dev, &args->addrs[i], 1);
		if (!vblk) {
			perror("nvm_vblk_alloc");
			return errno;
		}

		printf("** nvm_vblk_erase(...): pmode(%s)\n",
			nvm_pmode_str(nvm_dev_get_pmode(args->dev))
		);
		nvm_vblk_pr(vblk);

                nvm_cli_timer_start();
		res = nvm_vblk_erase(vblk);
		if (res < 0)
			perror("nvm_vblk_erase");
		nvm_cli_timer_stop();
		nvm_cli_timer_pr("nvm_vblk_erase");

		nvm_vblk_free(vblk);
	}

	return res < 0;
}

int read(struct nvm_cli *cli)
{
	struct nvm_cli_cmd_args *args = &cli->args;
	ssize_t res = 0;

	for (int i = 0; i < args->naddrs; ++i) {
		char *buf;
		struct nvm_vblk *vblk;
		size_t nbytes;
		const struct nvm_geo *geo;

		vblk = nvm_vblk_alloc(args->dev, &args->addrs[i], 1);
		if (!vblk) {
			perror("nvm_vblk_alloc");
			return errno;
		}

		nbytes = nvm_vblk_get_nbytes(vblk);
		geo = nvm_dev_get_geo(args->dev);

		printf("** nvm_vblk_read(...): pmode(%s)\n",
			nvm_pmode_str(nvm_dev_get_pmode(args->dev))
		);
		nvm_vblk_pr(vblk);

                nvm_cli_timer_start();
		buf = nvm_buf_alloc(geo, nbytes);
		if (!buf) {
			perror("nvm_buf_alloc");
			return errno;
		}
		nvm_cli_timer_stop();
		nvm_cli_timer_pr("nvm_buf_alloc");

                nvm_cli_timer_start();
		res = nvm_vblk_read(vblk, buf, nbytes);
		if (res < 0)
			perror("nvm_vblk_read");
		nvm_cli_timer_stop();
		nvm_cli_timer_pr("nvm_vblk_read");

		if (cli->evars.buf_pr) {
			printf("** READ:\n");
			nvm_buf_pr(buf, nbytes);
		}

		free(buf);
		nvm_vblk_free(vblk);
	}

	return res < 0;
}

int write(struct nvm_cli *cli)
{
	struct nvm_cli_cmd_args *args = &cli->args;
	ssize_t res = 0;

	for (int i = 0; i < args->naddrs; ++i) {
		char *buf;
		struct nvm_vblk *vblk;
		size_t nbytes;
		const struct nvm_geo *geo;

		vblk = nvm_vblk_alloc(args->dev, &args->addrs[i], 1);
		if (!vblk) {
			perror("nvm_vblk_alloc");
			return errno;
		}

		nbytes = nvm_vblk_get_nbytes(vblk);
		geo = nvm_dev_get_geo(args->dev);

		printf("** nvm_vblk_write(...): pmode(%s)\n",
			nvm_pmode_str(nvm_dev_get_pmode(args->dev))
		);

		nvm_vblk_pr(vblk);

                nvm_cli_timer_start();
		buf = nvm_buf_alloc(geo, nbytes);
		if (!buf) {
			perror("nvm_buf_alloc");
			return errno;
		}
		nvm_buf_fill(buf, nbytes);
		nvm_cli_timer_stop();
		nvm_cli_timer_pr("nvm_buf_alloc");

                nvm_cli_timer_start();
		res = nvm_vblk_write(vblk, buf, nbytes);
		if (res < 0)
			perror("nvm_vblk_write");
		nvm_cli_timer_stop();
		nvm_cli_timer_pr("nvm_vblk_write");

		free(buf);
		nvm_vblk_free(vblk);
	}

	return res < 0;
}

int pad(struct nvm_cli *cli)
{
	struct nvm_cli_cmd_args *args = &cli->args;
	ssize_t res = 0;

	for (int i = 0; i < args->naddrs; ++i) {
		struct nvm_vblk *vblk;

		vblk = nvm_vblk_alloc(args->dev, &args->addrs[i], 1);
		if (!vblk) {
			perror("nvm_vblk_alloc");
			return errno;
		}

		printf("** nvm_vblk_pad(...): pmode(%s)\n",
			nvm_pmode_str(nvm_dev_get_pmode(args->dev))
		);

		nvm_vblk_pr(vblk);

                nvm_cli_timer_start();
		res = nvm_vblk_pad(vblk);
		if (res < 0)
			perror("nvm_vblk_pad");
		nvm_cli_timer_stop();
		nvm_cli_timer_pr("nvm_vblk_pad");

		nvm_vblk_free(vblk);
	}

	return res < 0;
}

int line_erase(struct nvm_cli *cli)
{
	struct nvm_cli_cmd_args *args = &cli->args;
	ssize_t res = 0;

	struct nvm_vblk *vblk;
	struct nvm_addr bgn, end;

	bgn = args->addrs[0];
	end = args->addrs[1];
	
	vblk = nvm_vblk_alloc_line(args->dev, bgn.g.ch, end.g.ch, bgn.g.lun,
				   end.g.lun, bgn.g.blk);
	if (!vblk) {
		perror("nvm_vblk_alloc");
		return errno;
	}

	printf("** nvm_vblk_erase(...): pmode(%s)\n",
		nvm_pmode_str(nvm_dev_get_pmode(args->dev))
	);

	nvm_vblk_pr(vblk);

        nvm_cli_timer_start();
	res = nvm_vblk_erase(vblk);
	if (res < 0)
		perror("nvm_vblk_erase");
	nvm_cli_timer_stop();
	nvm_cli_timer_pr("nvm_vblk_erase");

	nvm_vblk_free(vblk);

	return res < 0;
}

int line_read(struct nvm_cli *cli)
{
	struct nvm_cli_cmd_args *args = &cli->args;
	ssize_t res = 0;

	struct nvm_addr bgn, end;
	struct nvm_vblk *vblk;
	size_t nbytes;
	const struct nvm_geo *geo;
	char *buf;

	bgn = args->addrs[0];
	end = args->addrs[1];

	vblk = nvm_vblk_alloc_line(args->dev, bgn.g.ch, end.g.ch, bgn.g.lun,
				   end.g.lun, end.g.blk);
	if (!vblk) {
		perror("nvm_vblk_alloc");
		return errno;
	}
	nbytes = nvm_vblk_get_nbytes(vblk);
	geo = nvm_dev_get_geo(args->dev);

	printf("** nvm_vblk_read(...): pmode(%s)\n",
		nvm_pmode_str(nvm_dev_get_pmode(args->dev))
	);
	nvm_vblk_pr(vblk);

        nvm_cli_timer_start();
	buf = nvm_buf_alloc(geo, nbytes);
	if (!buf) {
		perror("nvm_buf_alloc");
		return errno;
	}
	nvm_cli_timer_stop();
	nvm_cli_timer_pr("nvm_buf_alloc");

        nvm_cli_timer_start();
	res = nvm_vblk_read(vblk, buf, nbytes);
	if (res < 0)
		perror("nvm_vblk_read");
	nvm_cli_timer_stop();
	nvm_cli_timer_pr("nvm_vblk_read");

	if (cli->evars.buf_pr) {
		printf("** READ:\n");
		nvm_buf_pr(buf, nbytes);
	}

	free(buf);
	nvm_vblk_free(vblk);

	return res < 0;
}

int line_write(struct nvm_cli *cli)
{
	struct nvm_cli_cmd_args *args = &cli->args;
	ssize_t res = 0;

	struct nvm_addr bgn, end;
	struct nvm_vblk *vblk;
	size_t nbytes;
	const struct nvm_geo *geo;
	char *buf;

	bgn = args->addrs[0];
	end = args->addrs[1];

	vblk = nvm_vblk_alloc_line(args->dev, bgn.g.ch, end.g.ch, bgn.g.lun,
				   end.g.lun, end.g.blk);
	if (!vblk) {
		perror("nvm_vblk_alloc");
		return errno;
	}
	nbytes = nvm_vblk_get_nbytes(vblk);
	geo = nvm_dev_get_geo(args->dev);

	printf("** nvm_vblk_write(...):\n");
	nvm_vblk_pr(vblk);

        nvm_cli_timer_start();
	buf = nvm_buf_alloc(geo, nbytes);
	if (!buf) {
		perror("nvm_buf_alloc");
		return errno;
	}
	nvm_buf_fill(buf, nbytes);
	nvm_cli_timer_stop();
	nvm_cli_timer_pr("nvm_buf_alloc");

        nvm_cli_timer_start();
	res = nvm_vblk_write(vblk, buf, nbytes);
	if (res < 0)
		perror("nvm_vblk_write");
	nvm_cli_timer_stop();
	nvm_cli_timer_pr("nvm_vblk_write");

	free(buf);
	nvm_vblk_free(vblk);

	return res < 0;
}

int line_pad(struct nvm_cli *cli)
{
	struct nvm_cli_cmd_args *args = &cli->args;
	ssize_t res = 0;

	struct nvm_vblk *vblk;
	struct nvm_addr bgn, end;

	bgn = args->addrs[0];
	end = args->addrs[1];

	vblk = nvm_vblk_alloc_line(args->dev, bgn.g.ch, end.g.ch, bgn.g.lun,
				   end.g.lun, end.g.blk);
	if (!vblk) {
		perror("nvm_vblk_alloc");
		return errno;
	}

	printf("** nvm_vblk_pad(...):\n");
	nvm_vblk_pr(vblk);

        nvm_cli_timer_start();
	res = nvm_vblk_pad(vblk);
	if (res < 0)
		perror("nvm_vblk_pad");
	nvm_cli_timer_stop();
	nvm_cli_timer_pr("nvm_vblk_pad");

	nvm_vblk_free(vblk);

	return res < 0;
}

int line_to_file(struct nvm_cli *cli)
{
	struct nvm_cli_cmd_args *args = &cli->args;
	ssize_t res = 0;

	struct nvm_addr bgn, end;
	struct nvm_vblk *vblk;
	size_t nbytes;
	const struct nvm_geo *geo;
	char *buf;

	bgn = args->addrs[0];
	end = args->addrs[1];

	vblk = nvm_vblk_alloc_line(args->dev, bgn.g.ch, end.g.ch, bgn.g.lun,
				   end.g.lun, end.g.blk);
	if (!vblk) {
		perror("nvm_vblk_alloc");
		return errno;
	}
	nbytes = nvm_vblk_get_nbytes(vblk);
	geo = nvm_dev_get_geo(args->dev);

	printf("** nvm_vblk_read(...):\n");
	nvm_vblk_pr(vblk);

        nvm_cli_timer_start();
	buf = nvm_buf_alloc(geo, nbytes);
	if (!buf) {
		perror("nvm_buf_alloc");
		return errno;
	}
	nvm_cli_timer_stop();
	nvm_cli_timer_pr("nvm_buf_alloc");

        nvm_cli_timer_start();
	res = nvm_vblk_read(vblk, buf, nbytes);
	if (res < 0)
		perror("nvm_vblk_read");
	nvm_cli_timer_stop();
	nvm_cli_timer_pr("nvm_vblk_read");

	{
		FILE *file_handle;
		char file_name[1024];

		sprintf(file_name, "vblk_line_%016lx_%016lx.bin", bgn.ppa, end.ppa);
		file_handle = fopen(file_name, "wb");
		fwrite(buf, 1, nbytes, file_handle);
		fclose(file_handle);
	}

	free(buf);
	nvm_vblk_free(vblk);

	return res < 0;
}

int set_erase(struct nvm_cli *cli)
{
	struct nvm_cli_cmd_args *args = &cli->args;
	ssize_t res = 0;

	struct nvm_vblk *vblk;
	
	vblk = nvm_vblk_alloc(args->dev, args->addrs, args->naddrs);
	if (!vblk) {
		perror("nvm_vblk_alloc");
		return errno;
	}

	printf("** nvm_vblk_erase(...): pmode(%s)\n",
		nvm_pmode_str(nvm_dev_get_pmode(args->dev))
	);

	nvm_vblk_pr(vblk);

        nvm_cli_timer_start();
	res = nvm_vblk_erase(vblk);
	if (res < 0)
		perror("nvm_vblk_erase");
	nvm_cli_timer_stop();
	nvm_cli_timer_pr("nvm_vblk_erase");

	nvm_vblk_free(vblk);

	return res < 0;
}

int set_read(struct nvm_cli *cli)
{
	struct nvm_cli_cmd_args *args = &cli->args;
	ssize_t res = 0;

	struct nvm_vblk *vblk;
	size_t nbytes;
	const struct nvm_geo *geo;
	char *buf;

	vblk = nvm_vblk_alloc(args->dev, args->addrs, args->naddrs);
	if (!vblk) {
		perror("nvm_vblk_alloc");
		return errno;
	}
	nbytes = nvm_vblk_get_nbytes(vblk);
	geo = nvm_dev_get_geo(args->dev);

	printf("** nvm_vblk_read(...): pmode(%s)\n",
		nvm_pmode_str(nvm_dev_get_pmode(args->dev))
	);
	nvm_vblk_pr(vblk);

        nvm_cli_timer_start();
	buf = nvm_buf_alloc(geo, nbytes);
	if (!buf) {
		perror("nvm_buf_alloc");
		return errno;
	}
	nvm_cli_timer_stop();
	nvm_cli_timer_pr("nvm_buf_alloc");

        nvm_cli_timer_start();
	res = nvm_vblk_read(vblk, buf, nbytes);
	if (res < 0)
		perror("nvm_vblk_read");
	nvm_cli_timer_stop();
	nvm_cli_timer_pr("nvm_vblk_read");

	if (cli->evars.buf_pr) {
		printf("** READ:\n");
		nvm_buf_pr(buf, nbytes);
	}

	free(buf);
	nvm_vblk_free(vblk);

	return res < 0;
}

int set_write(struct nvm_cli *cli)
{
	struct nvm_cli_cmd_args *args = &cli->args;
	ssize_t res = 0;

	struct nvm_vblk *vblk;
	size_t nbytes;
	const struct nvm_geo *geo;
	char *buf;

	vblk = nvm_vblk_alloc(args->dev, args->addrs, args->naddrs);
	if (!vblk) {
		perror("nvm_vblk_alloc");
		return errno;
	}
	nbytes = nvm_vblk_get_nbytes(vblk);
	geo = nvm_dev_get_geo(args->dev);

	printf("** nvm_vblk_write(...): pmode(%s)\n",
		nvm_pmode_str(nvm_dev_get_pmode(args->dev))
	);
	nvm_vblk_pr(vblk);

        nvm_cli_timer_start();
	buf = nvm_buf_alloc(geo, nbytes);
	if (!buf) {
		perror("nvm_buf_alloc");
		return errno;
	}
	nvm_buf_fill(buf, nbytes);
	nvm_cli_timer_stop();
	nvm_cli_timer_pr("nvm_buf_alloc");

        nvm_cli_timer_start();
	res = nvm_vblk_write(vblk, buf, nbytes);
	if (res < 0)
		perror("nvm_vblk_write");
	nvm_cli_timer_stop();
	nvm_cli_timer_pr("nvm_vblk_write");

	free(buf);
	nvm_vblk_free(vblk);

	return res < 0;
}

int set_pad(struct nvm_cli *cli)
{
	struct nvm_cli_cmd_args *args = &cli->args;
	ssize_t res = 0;

	struct nvm_vblk *vblk;

	vblk = nvm_vblk_alloc(args->dev, args->addrs, args->naddrs);
	if (!vblk) {
		perror("nvm_vblk_alloc");
		return errno;
	}

	printf("** nvm_vblk_pad(...): pmode(%s)\n",
		nvm_pmode_str(nvm_dev_get_pmode(args->dev))
	);
	nvm_vblk_pr(vblk);

        nvm_cli_timer_start();
	res = nvm_vblk_pad(vblk);
	if (res < 0)
		perror("nvm_vblk_pad");
	nvm_cli_timer_stop();
	nvm_cli_timer_pr("nvm_vblk_pad");

	nvm_vblk_free(vblk);

	return res < 0;
}

//
// Remaining code is CLI boiler-plate
//
static struct nvm_cli_cmd cmds[] = {
	{"erase",	erase,	NVM_CLI_ARG_ADDR_LIST,	NVM_CLI_OPT_NONE},
	{"read",	read,	NVM_CLI_ARG_ADDR_LIST,	NVM_CLI_OPT_NONE},
	{"write",	write,	NVM_CLI_ARG_ADDR_LIST,	NVM_CLI_OPT_NONE},
	{"pad",		pad,	NVM_CLI_ARG_ADDR_LIST,	NVM_CLI_OPT_NONE},

	{"set_erase",	set_erase,	NVM_CLI_ARG_ADDR_LIST,	NVM_CLI_OPT_NONE},
	{"set_read",	set_read,	NVM_CLI_ARG_ADDR_LIST,	NVM_CLI_OPT_NONE},
	{"set_write",	set_write,	NVM_CLI_ARG_ADDR_LIST,	NVM_CLI_OPT_NONE},
	{"set_pad",	set_pad,	NVM_CLI_ARG_ADDR_LIST,	NVM_CLI_OPT_NONE},

	{"line_erase",	line_erase,	NVM_CLI_ARG_VBLK_LINE,	NVM_CLI_OPT_NONE},
	{"line_read",	line_read,	NVM_CLI_ARG_VBLK_LINE,	NVM_CLI_OPT_NONE},
	{"line_write",	line_write,	NVM_CLI_ARG_VBLK_LINE,	NVM_CLI_OPT_NONE},
	{"line_pad",	line_pad,	NVM_CLI_ARG_VBLK_LINE,	NVM_CLI_OPT_NONE},
	{"line_to_file",line_to_file,	NVM_CLI_ARG_VBLK_LINE,	NVM_CLI_OPT_NONE},
};

/* Define the CLI */
static struct nvm_cli cli = {
	.title = "NVM Virtual Block (nvm_vblk_*)",
	.description = "Erase/read/write virtual blocks",
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
