/*
 * nvm_cli - Command-line interface (CLI) utitities for liblightnvm
 *
 * Copyright (C) 2015-2017 Javier Gonzáles <javier@cnexlabs.com>
 * Copyright (C) 2015-2017 Matias Bjørling <matias@cnexlabs.com>
 * Copyright (C) 2015-2017 Simon A. F. Lund <slund@cnexlabs.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  - Redistributions of source code must retain the above copyright notice,
 *  this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright notice,
 *  this list of conditions and the following disclaimer in the documentation
 *  and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <getopt.h>
#include <sys/time.h>
#include <liblightnvm_cli.h>

static size_t start, stop;

static inline size_t wclock_sample(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_usec + tv.tv_sec * 1000000;
}

size_t nvm_cli_timer_start(void)
{
    start = wclock_sample();
    return start;
}

size_t nvm_cli_timer_stop(void)
{
    stop = wclock_sample();
    return stop;
}

double nvm_cli_timer_elapsed(void)
{
    return (stop-start)/(double)1000000.0;
}

void nvm_cli_timer_pr(const char *tool)
{
    printf("Ran %s, elapsed wall-clock: %lf\n", tool, nvm_cli_timer_elapsed());
}

void nvm_cli_opts_pr(struct nvm_cli_opts *options)
{
	printf("options {\n");
	printf(" mask(0x%08x),\n", options->mask);
	printf(" brief(%d),\n", options->brief);
	printf(" verbose(%d),\n", options->verbose);
	printf(" val_dec(%d),\n", options->val_dec);
	printf(" val_hex(0x%016x),\n", options->val_hex);
	printf(" file_input(%s),\n", options->file_input);
	printf(" file_output(%s),\n", options->file_output);
	printf("}\n");
}

int _parse_options(int argc, char *argv[], struct nvm_cli *cli)
{
	for (int opt = 0; (opt = getopt(argc, argv, ":bvi:o:n:x:")) != -1;) {
		switch(opt) {
		case 'b':
			cli->opts.mask |= NVM_CLI_OPT_BRIEF;
			cli->opts.brief = 1;
			break;
		case 'v':
			cli->opts.mask |= NVM_CLI_OPT_VERBOSE;
			cli->opts.verbose = 1;
			break;
		case 'i':
			cli->opts.mask |= NVM_CLI_OPT_FILE_INPUT;
			cli->opts.file_input = optarg;
			break;
		case 'o':
			cli->opts.mask |= NVM_CLI_OPT_FILE_OUTPUT;
			cli->opts.file_output = optarg;
			break;
		case 'n':
			cli->opts.mask |= NVM_CLI_OPT_VAL_DEC;
			cli->opts.val_dec = atoi(optarg);
			break;
		case 'x':
			cli->opts.mask |= NVM_CLI_OPT_VAL_HEX;
			cli->opts.val_hex = strtol(optarg, NULL, 16);
			break;

		case ':':
		case '?':
			return -1;
		default:
			break;
		}
	}

	return optind;
}

int _parse_cmd_arg_count_offset(int argc, char *argv[], struct nvm_cli *cli)
{
	const int inc = 2;

	if (argc < inc) {
		errno = EINVAL;
		return -1;
	}

	cli->args.dec_vals[0] = atoi(argv[0]);
	cli->args.dec_vals[1] = atoi(argv[1]);
	cli->args.ndec_vals = 2;

	return inc;
}

int _parse_cmd_arg_vblk_line(int argc, char *argv[], struct nvm_cli *cli)
{
	const int inc = 5;

	if (argc < inc) {
		errno = EINVAL;
		return -1;
	}

	cli->args.addrs[0].ppa = 0;
	cli->args.addrs[0].g.ch = atoi(argv[0]);
	cli->args.addrs[0].g.lun = atoi(argv[2]);
	cli->args.addrs[0].g.blk = atoi(argv[4]);

	cli->args.addrs[1].ppa = 0;
	cli->args.addrs[1].g.ch = atoi(argv[1]);
	cli->args.addrs[1].g.lun = atoi(argv[3]);
	cli->args.addrs[1].g.blk = atoi(argv[4]);
	cli->args.naddrs = 6;

	return inc;
}

int _parse_cmd_arg_addr_sec(int argc, char *argv[], struct nvm_cli *cli)
{
	const int inc = 6;

	if (argc < inc) {
		errno = EINVAL;
		return -1;
	}

	cli->args.addrs[0].ppa = 0;
	cli->args.addrs[0].g.ch = atoi(argv[0]);
	cli->args.addrs[0].g.lun = atoi(argv[1]);
	cli->args.addrs[0].g.pl = atoi(argv[2]);
	cli->args.addrs[0].g.blk = atoi(argv[3]);
	cli->args.addrs[0].g.pg = atoi(argv[4]);
	cli->args.addrs[0].g.sec = atoi(argv[5]);
	cli->args.naddrs = 1;

	return inc;
}

int _parse_cmd_arg_addr_pg(int argc, char *argv[], struct nvm_cli *cli)
{
	const int inc = 4;

	if (argc < inc) {
		errno = EINVAL;
		return -1;
	}

	cli->args.addrs[0].ppa = 0;
	cli->args.addrs[0].g.ch = atoi(argv[0]);
	cli->args.addrs[0].g.lun = atoi(argv[1]);
	cli->args.addrs[0].g.blk = atoi(argv[2]);
	cli->args.addrs[0].g.pg = atoi(argv[3]);
	cli->args.naddrs = 1;

	return inc;
}

int _parse_cmd_arg_addr_blk(int argc, char *argv[], struct nvm_cli *cli)
{
	const int inc = 3;

	if (argc < inc) {
		errno = EINVAL;
		return -1;
	}

	cli->args.addrs[0].ppa = 0;
	cli->args.addrs[0].g.ch = atoi(argv[0]);
	cli->args.addrs[0].g.lun = atoi(argv[1]);
	cli->args.addrs[0].g.blk = atoi(argv[2]);
	cli->args.naddrs = 1;

	return inc;
}

int _parse_cmd_arg_addr_lun(int argc, char *argv[], struct nvm_cli *cli)
{
	const int inc = 2;

	if (argc < inc) {
		errno = EINVAL;
		return -1;
	}

	cli->args.addrs[0].ppa = 0;
	cli->args.addrs[0].g.ch = atoi(argv[0]);
	cli->args.addrs[0].g.lun = atoi(argv[1]);
	cli->args.naddrs = 1;

	return inc;
}

int _parse_cmd_arg_addr_list(int argc, char *argv[], struct nvm_cli *cli)
{
	int inc = 0;
	
	for (int i = 0; (i < argc) && (argv[i][0] != '-'); ++i, ++inc)
		cli->args.addrs[i].ppa = strtol(argv[i], NULL, 16);

	return inc;
}

int _parse_cmd_arg_value_list(int argc, char *argv[], struct nvm_cli *cli)
{
	int inc = 0;
	
	for (int i = 0; (i < argc) && (argv[i][0] != '-'); ++i, ++inc)
		cli->args.dec_vals[i] = atoi(argv[i]);

	cli->args.ndec_vals = inc;

	return inc;
}

int _parse_cmd_arg_dev_path(int argc, char *argv[], struct nvm_cli *cli)
{
	const int inc = 1;

	if (argc < inc) {
		errno = EINVAL;
		return -1;
	}

	if (strlen(argv[0]) < (size_t)inc ||
	    strlen(argv[0]) > NVM_DEV_PATH_LEN) {
		errno = EINVAL;
		return -1;
	}

	strcpy(cli->args.dev_path, argv[0]);

	return inc;
}

int _parse_cmd_args(int argc, char *argv[], struct nvm_cli *cli)
{
	int inc = 0;
	int ret;

	// Currently all commands except (ARG_NONE) take dev_path
	if (cli->cmd.arg_type != NVM_CLI_ARG_NONE) {
		ret = _parse_cmd_arg_dev_path(argc, argv, cli);	
		if (ret < 0)
			return -1;
		inc += ret;
	}

	// Invoke the remaining parsers
	switch (cli->cmd.arg_type) {
	case NVM_CLI_ARG_VALUE_LIST:
		ret = _parse_cmd_arg_value_list(argc - inc, argv + inc, cli);
		if (ret < 0)
			return -1;
		return inc + ret;

	case NVM_CLI_ARG_ADDR_LIST:
		ret = _parse_cmd_arg_addr_list(argc - inc, argv + inc, cli);
		if (ret < 0)
			return -1;
		return inc + ret;

	case NVM_CLI_ARG_ADDR_LUN:
		ret = _parse_cmd_arg_addr_lun(argc - inc, argv + inc, cli);
		if (ret < 0)
			return -1;
		return inc + ret;

	case NVM_CLI_ARG_ADDR_BLK:
		ret = _parse_cmd_arg_addr_blk(argc - inc, argv + inc, cli);
		if (ret < 0)
			return -1;
		return inc + ret;

	case NVM_CLI_ARG_ADDR_PG:
		ret = _parse_cmd_arg_addr_pg(argc - inc, argv + inc, cli);
		if (ret < 0)
			return -1;
		return inc + ret;

	case NVM_CLI_ARG_ADDR_SEC:
		ret = _parse_cmd_arg_addr_sec(argc - inc, argv + inc, cli);
		if (ret < 0)
			return -1;
		return inc + ret;

	case NVM_CLI_ARG_VBLK_LINE:
		ret = _parse_cmd_arg_vblk_line(argc - inc, argv + inc, cli);
		if (ret < 0)
			return -1;
		return inc + ret;

	case NVM_CLI_ARG_COUNT_OFFSET:
		ret = _parse_cmd_arg_count_offset(argc - inc, argv + inc, cli);
		if (ret < 0)
			return -1;
		return inc + ret;

	case NVM_CLI_ARG_DEV_PATH:
	case NVM_CLI_ARG_NONE:
		return inc;
	}

	return -1;
}

int _parse_cmd(int argc, char *argv[], struct nvm_cli *cli)
{
	const int inc = 1;
	if (argc < inc) {
		errno = EINVAL;
		return -1;
	}

	for (int i = 0; i < cli->ncmds; ++i)
		if (strcmp(argv[0], cli->cmds[i].name) == 0) {
			cli->cmd = cli->cmds[i];
			return inc;
		}

	errno = EINVAL;
	return -1;
}

int _parse_cli_name(int argc, char *argv[], struct nvm_cli *cli)
{
	const int inc = 1;

	if (argc < inc) {
		errno = EINVAL;
		return -1;
	}

	strcpy(cli->name, argv[0]);

	return inc;
}

/**
 * Environment variable parsers
 */

int _evar_pmode(struct nvm_cli *cli)
{
	struct nvm_dev *dev = cli->args.dev;
	const struct nvm_geo *geo = cli->args.geo;

	char *pmode_env = getenv("NVM_CLI_PMODE");
	if (!pmode_env) {
		cli->evars.pmode = nvm_dev_get_pmode(dev);
		return 0;
	}

	switch(strtol(pmode_env, NULL, 16)) {
	case NVM_FLAG_PMODE_QUAD:
		if (geo->nplanes < 4) {	// Verify
			errno = EINVAL;
			return -1;
		}
		return NVM_FLAG_PMODE_QUAD;
	case NVM_FLAG_PMODE_DUAL:
		if (geo->nplanes < 2) {	// Verify
			errno = EINVAL;
			return -1;
		}
		return NVM_FLAG_PMODE_DUAL;
	case NVM_FLAG_PMODE_SNGL:
		return NVM_FLAG_PMODE_SNGL;

	default:
		errno = EINVAL;
		return -1;
	}
}

int _evar_noverify(struct nvm_cli *cli)
{
	cli->evars.noverify = getenv("NVM_CLI_NOVERIFY") ? 1 : 0;

	return 0;
}

int _evar_buf_pr(struct nvm_cli *cli)
{
	cli->evars.buf_pr = getenv("NVM_CLI_BUF_PR") ? 1 : 0;

	return 0;
}

int _evar_meta_pr(struct nvm_cli *cli)
{
	cli->evars.meta_pr = getenv("NVM_CLI_META_PR") ? 1 : 0;

	return 0;
}

int _evar_erase_naddrs_max(struct nvm_cli *cli)
{
	char *erase_naddrs_max = getenv("NVM_CLI_ERASE_NADDRS_MAX");
	if (!erase_naddrs_max) {
		cli->evars.erase_naddrs_max = nvm_dev_get_erase_naddrs_max(
								cli->args.dev);
		return 0;
	}

	cli->evars.erase_naddrs_max = atoi(erase_naddrs_max);

	return 0;
}

int _evar_write_naddrs_max(struct nvm_cli *cli)
{
	char *write_naddrs_max = getenv("NVM_CLI_WRITE_NADDRS_MAX");
	if (!write_naddrs_max) {
		cli->evars.write_naddrs_max = nvm_dev_get_write_naddrs_max(
								cli->args.dev);
		return 0;
	}

	cli->evars.write_naddrs_max = atoi(write_naddrs_max);

	return 0;
}

int _evar_read_naddrs_max(struct nvm_cli *cli)
{
	char *read_naddrs_max = getenv("NVM_CLI_READ_NADDRS_MAX");
	if (!read_naddrs_max) {
		cli->evars.read_naddrs_max = nvm_dev_get_read_naddrs_max(
								cli->args.dev);
		return 0;
	}

	cli->evars.read_naddrs_max = atoi(read_naddrs_max);

	return 0;
}



int _evar_meta_mode(struct nvm_cli *cli)
{
	char *meta_mode_env = getenv("NVM_CLI_META_MODE");
	if (!meta_mode_env) {
		cli->evars.meta_mode = nvm_dev_get_meta_mode(cli->args.dev);
		return 0;
	}

	switch(strtol(meta_mode_env, NULL, 16)) {
	case NVM_META_MODE_NONE:
		cli->evars.meta_mode = NVM_META_MODE_NONE;
		return 0;
	case NVM_META_MODE_ALPHA:
		cli->evars.meta_mode = NVM_META_MODE_ALPHA;
		return 0;
	case NVM_META_MODE_CONST:
		cli->evars.meta_mode = NVM_META_MODE_CONST;
		return 0;

	default:
		errno = EINVAL;
		return -1;
	}
}

int _evar_be_id(struct nvm_cli *cli)
{
	char *id = getenv("NVM_CLI_BE_ID");
	if (!id) {
		cli->evars.be_id = NVM_BE_ANY;
		return 0;
	}

	switch(strtol(id, NULL, 16)) {
	case NVM_BE_ANY:
		cli->evars.be_id = NVM_BE_ANY;
		return 0;
	case NVM_BE_IOCTL:
		cli->evars.be_id = NVM_BE_IOCTL;
		return 0;
	case NVM_BE_SYSFS:
		cli->evars.be_id = NVM_BE_SYSFS;
		return 0;

	default:
		errno = EINVAL;
		return -1;
	}
}

int _evar_and_dev_setup(struct nvm_cli *cli)
{
	if (!cli) {
		errno = EINVAL;
		return -1;
	}

	if (_evar_be_id(cli) < 0) {		// Backend identifier
		perror("NVM_CLI_BE_ID");
		return -1;
	}

	cli->args.dev = nvm_dev_openf(cli->args.dev_path, cli->evars.be_id);
	if (!cli->args.dev) {
		perror("nvm_dev_openf");
		return -1;
	}

	cli->args.geo = nvm_dev_get_geo(cli->args.dev);

	if ((_evar_pmode(cli) < 0) ||
	    nvm_dev_set_pmode(cli->args.dev, cli->evars.pmode)) {
		perror("NVM_CLI_PMODE");
		return -1;
	}

	if ((_evar_meta_mode(cli) < 0) ||
	    nvm_dev_set_meta_mode(cli->args.dev, cli->evars.meta_mode)) {
		perror("NVM_CLI_META_MODE");
		return -1;
	}

	if ((_evar_erase_naddrs_max(cli) < 0) ||
	    nvm_dev_set_erase_naddrs_max(cli->args.dev,
					 cli->evars.erase_naddrs_max)) {
		perror("NVM_CLI_ERASE_NADDRS_MAX");
		return -1;
	}
	if ((_evar_write_naddrs_max(cli) < 0) ||
	    nvm_dev_set_write_naddrs_max(cli->args.dev,
					 cli->evars.write_naddrs_max)) {
		perror("NVM_CLI_WRITE_NADDRS_MAX");
		return -1;
	}
	if ((_evar_read_naddrs_max(cli) < 0) ||
	    nvm_dev_set_read_naddrs_max(cli->args.dev,
					 cli->evars.read_naddrs_max)) {
		perror("NVM_CLI_READ_NADDRS_MAX");
		return -1;
	}

	if (_evar_noverify(cli) < 0) {
		perror("NVM_CLI_NOVERIFY");
		return -1;
	}
	
	for (int i = 0; (i < cli->args.naddrs) && (!cli->evars.noverify); ++i) {
		int bounds = nvm_addr_check(cli->args.addrs[i], cli->args.geo);

		if (bounds) {
			nvm_addr_pr(cli->args.addrs[i]);
			printf("Exceeded:\n");
			nvm_bounds_pr(bounds);
			errno = EINVAL;
			return -1;
		}
	}

	return 0;
}

int nvm_cli_init(struct nvm_cli *cli, int argc, char *argv[])
{
	int state = 0;
	int ret;

	ret = _parse_cli_name(argc, argv, cli);
	if (ret < 0) {
		errno = EINVAL;
		return -1;
	}
	state += ret;

	ret = _parse_cmd(argc - state, argv + state, cli);
	if (ret < 0) {
		errno = EINVAL;
		return -1;
	}
	state += ret;

	ret = _parse_cmd_args(argc - state, argv + state, cli);
	if (ret < 0) {
		errno = EINVAL;
		return -1;
	}

	ret = _parse_options(argc - state, argv + state, cli);
	if (ret < 0) {
		errno = EINVAL;
		return -1;
	}

	ret = _evar_and_dev_setup(cli);
	if (ret < 0) {
		errno = EINVAL;
		return -1;
	}

	return 0;
}

int nvm_cli_run(struct nvm_cli *cli)
{
	if (!cli) {
		errno = EINVAL;
		return -1;
	}

	return cli->cmd.func(cli);
}

void nvm_cli_destroy(struct nvm_cli *cli)
{
	if (!cli)
		return;

	// dev close
}

void nvm_cli_usage_pr(struct nvm_cli *cli)
{
	if (!cli)
		return;

	if (cli->title) {
		printf("%s -- ", cli->title);
		nvm_ver_pr();
		printf("\n");
	}

	if (cli->description) {
		printf("\n%s\n", cli->description);
	}

	printf("\nUsage:\n");

	for (int i = 0; i < cli->ncmds; ++i) {
		printf(" %s %10s ", cli->name, cli->cmds[i].name);

		switch(cli->cmds[i].arg_type) {
		case NVM_CLI_ARG_ADDR_LIST:
			printf("addr [addr...]");
			break;
		case NVM_CLI_ARG_VALUE_LIST:
			printf("num [num...]");
			break;
		case NVM_CLI_ARG_ADDR_LUN:
			printf("ch lun");
			break;
		case NVM_CLI_ARG_ADDR_BLK:
			printf("ch lun blk");
			break;
		case NVM_CLI_ARG_ADDR_PG:
			printf("ch lun blk pg");
			break;
		case NVM_CLI_ARG_ADDR_SEC:
			printf("ch lun pl blk pg sec");
			break;
		case NVM_CLI_ARG_VBLK_LINE:
			printf("ch_bgn ch_end lun_bgn lun_end blk");
			break;
		case NVM_CLI_ARG_COUNT_OFFSET:
			printf("count offset");
			break;

		case NVM_CLI_ARG_DEV_PATH:
			printf("dev_path");
			break;

		case NVM_CLI_ARG_NONE:
			break;
		}
		printf("\n");
	}

	return;
}
