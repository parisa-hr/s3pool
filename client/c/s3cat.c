#define _XOPEN_SOURCE 500
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "s3pool.h"

void usage(const char* pname, const char* msg)
{
	fprintf(stderr, "Usage: %s [-h] -p port bucket:key ...\n", pname);
	fprintf(stderr, "Copy s3 files to stdout.\n\n");
	fprintf(stderr, "    -p port : specify the port number of s3pool process\n");
	fprintf(stderr, "    -h      : print this help message\n");
	fprintf(stderr, "\n");
	if (msg) {
		fprintf(stderr, "%s\n\n", msg);
	}

	exit(msg ? -1 : 0);
}


void fatal(const char* msg)
{
	fprintf(stderr, "FATAL: %s\n", msg);
	exit(1);
}


void doit(int port, char* bktkey_)
{
	char errmsg[200];
	char* bktkey = strdup(bktkey_);
	if (!bktkey) {
		fatal("out of memory");
	}

	char* colon = strchr(bktkey, ':');
	if (!colon) {
		fatal("missing colon char in bucket:key");
	}

	*colon = 0;
	char* bucket = bktkey;
	char* key = colon+1;

	char* fname = s3pool_pull(port, bucket, key,
							  errmsg, sizeof(errmsg));
	if (!fname) {
		fatal(errmsg);
	}

	FILE* fp = fopen(fname, "r");
	if (!fp) {
		perror("fopen");
		exit(1);
	}

	while (1) {
		char buf[100];
		int n = fread(buf, 1, sizeof(buf), fp);
		if (ferror(fp)) {
			perror("fread");
			exit(1);
		}
		if (n != (int) fwrite(buf, 1, n, stdout)) {
			perror("fwrite");
			exit(1);
		}
		if (feof(fp)) break;
	}


	fclose(fp);
	free(bktkey);
	
}




int main(int argc, char* argv[])
{
	int opt;
	int port = -1;
	while ((opt = getopt(argc, argv, "p:h")) != -1) {
		switch (opt) {
		case 'p':
			port = atoi(optarg);
			break;
		case 'h':
			usage(argv[0], 0);
			break;
		default:
			usage(argv[0], "Bad command line flag");
			break;
		}
	}

	if (! (0 < port && port <= 65535)) {
		usage(argv[0], "Bad or missing port number");
	}

	if (optind >= argc) {
		usage(argv[0], "Need bucket and key");
	}
	
	for (int i = optind; i < argc; i++) {
		doit(port, argv[i]);
	}

	return 0;
}
