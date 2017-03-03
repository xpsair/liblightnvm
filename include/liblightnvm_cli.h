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
/**
 * @file liblightnvm_cli.h
 */
#ifndef __LIBLIGHTNVM_CLI_H
#define __LIBLIGHTNVM_CLI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <liblightnvm.h>

#define NVM_CLI_CMD_LEN 50

/**
 * Enumeration of environment variables
 */
enum nvm_cli_evar_type {
	NVM_CLI_ENV_BE_ID,
	NVM_CLI_PMODE,
	NVM_CLI_META_MODE,
	NVM_CLI_NOVERIFY,
	NVM_CLI_ERASE_NADDRS_MAX,
	NVM_CLI_READ_NADDRS_MAX,
	NVM_CLI_WRITE_NADDRS_MAX,
};

/**
 * Storage for parsed environment variable dec_vals
 */
struct nvm_cli_evars {
	int be_id;
	int pmode;
	int meta_mode;
	int noverify;
	int erase_naddrs_max;
	int read_naddrs_max;
	int write_naddrs_max;
	int buf_pr;
	int meta_pr;
};

/**
 * Positional argument types
 */
enum nvm_cli_cmd_arg_type {
	NVM_CLI_ARG_NONE,
	NVM_CLI_ARG_DEV_PATH,
	NVM_CLI_ARG_VALUE_LIST,
	NVM_CLI_ARG_ADDR_LIST,
	NVM_CLI_ARG_ADDR_LUN,
	NVM_CLI_ARG_ADDR_BLK,
	NVM_CLI_ARG_ADDR_PG,
	NVM_CLI_ARG_ADDR_SEC,
	NVM_CLI_ARG_VBLK_LINE,
	NVM_CLI_ARG_COUNT_OFFSET,
};

/**
 * Storage for positional arguments
 */
struct nvm_cli_cmd_args {
	char dev_path[1024];		///< For ALL
	struct nvm_dev *dev;		///< For ALL
	const struct nvm_geo *geo;	///< For ALL
	struct nvm_addr addrs[1024];	///< Parsed gen addresses
	int naddrs;			///< Number of parsed gen addresses
	size_t dec_vals[1024];		///< Parsed decimal dec_vals
	int ndec_vals;			///< Number of parsed decical values
	size_t hex_vals[1024];		///< Parsed decimal values
	int nhex_value;			///< Number of parsed decical values
};

/**
 * Types of option-arguments
 */
enum nvm_cli_opt_type {
	NVM_CLI_OPT_NONE = 0x0,
	NVM_CLI_OPT_BRIEF = 0x1,
	NVM_CLI_OPT_VERBOSE = 1 << 1,
	NVM_CLI_OPT_FILE_INPUT = 1 << 2,
	NVM_CLI_OPT_FILE_OUTPUT = 1 << 3,
	NVM_CLI_OPT_VAL_DEC = 1 << 4,
	NVM_CLI_OPT_VAL_HEX = 1 << 5,
};

/**
 * Storage for opt-arguments
 */
struct nvm_cli_opts {
	int mask;		///< Mask of all provided options
	int brief;		///< For NVM_CLI_OPT_BRIEF
	int verbose;		///< For NVM_CLI_OPT_VERBOSE
	char *file_input;	///< For NVM_CLI_OPT_FILE_INPUT
	char *file_output;	///< For NVM_CLI_OPT_FILE_OUTPUT
	int val_dec;		///< For NVM_CLI_OPT_VAL_DEC
	int val_hex;		///< For NVM_CLI_OPT_VAL_HEX
};

void nvm_cli_opts_pr(struct nvm_cli_opts *options);

struct nvm_cli;

typedef int (*nvm_cli_cmd_func)(struct nvm_cli*);

struct nvm_cli_cmd {
	char name[NVM_CLI_CMD_LEN];	// Command name
	nvm_cli_cmd_func func;		// Command function
	enum nvm_cli_cmd_arg_type arg_type;	// Positional argument type
	int opt_types;			// Mask of supported options
};

struct nvm_cli {
	const char *title;		// Defined by user
	const char *description;	// Defined by user

	struct nvm_cli_cmd *cmds;	// Defined by user
	int ncmds;			// Defined by user

	char name[NVM_CLI_CMD_LEN];	// Constructed by _parse_cli

	struct nvm_cli_cmd cmd;		// Selected from `cmds` by _parse_cmd

	struct nvm_cli_cmd_args args;	// Constructed by _parse_args
	struct nvm_cli_opts opts;	// Constructed by _parse_opts
	struct nvm_cli_evars evars;	// Constructed from ENV
};

/**
 * Start the global timer
 *
 * @return timestamp, in ms, when the timer was started
 */
size_t nvm_cli_timer_start(void);

/**
 * Stop the global timer
 *
 * @return timestamp, in ms, when the timer was stopped
 */
size_t nvm_cli_timer_stop(void);

/**
 * Return elapsed time
 *
 * @return Elapsed time, in seconds
 */
double nvm_cli_timer_elapsed(void);

/**
 * Print out elapsed time prefix with the given string
 *
 * @param tool Prefix to use
 */
void nvm_cli_timer_pr(const char *tool);

/**
 * Provide backend via ENV("NVM_CLI_BE_ID")
 *
 * @note
 * If NVM_CLI_BE_ID is not set, then NVM_BE_ANY is returned
 */
int nvm_cli_be_id(void);

/**
 * Override plane_mode via ENV("NVM_CLI_PMODE")
 *
 * @note
 * If NVM_CLI_PMODE is not set, the device default is returned.
 *
 * @param dev Device handle obtained with `nvm_dev_open`
 * @return On success, user-supplied plane_mode is returned. On error, -1 and
 * errno set to indicate the error.
 */
int nvm_cli_pmode(struct nvm_dev *dev);

/**
 * Provide an override for device meta_mode via CLI ENV("NVM_CLI_META_MODE")
 *
 * @note
 * If NVM_CLI_META_MODE is not set, the device default is returned.
 *
 * @param dev Device handle obtained with `nvm_dev_open`
 * @return On success, user-supplied plane_mode is returned. On error, -1 and
 * errno set to indicate the error.
 */
int nvm_cli_meta_mode(struct nvm_dev *dev);

void nvm_cli_usage_pr(struct nvm_cli *cli);

int nvm_cli_init(struct nvm_cli *cli, int argc, char *argv[]);

int nvm_cli_run(struct nvm_cli *cli);

void nvm_cli_destroy(struct nvm_cli *cli);

#ifdef __cplusplus
}
#endif

#endif /* __LIBLIGHTNVM_CLI.H */
