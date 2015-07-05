#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct initrd_header {
	unsigned char magic;
	char name[64];
	unsigned int offset;
	unsigned int length;
};

int main(int argc, char **argv)
{
	int nheaders = (argc - 1) / 2;
	struct initrd_header headers[64];
	printf("size of header: %zd\n", sizeof(struct initrd_header));
	unsigned int off = sizeof(struct initrd_header) * 64 + sizeof(int);
	
	int i;
	for (i = 0; i < nheaders; i++) {
		printf("writing file %s->%s at %#x\n", argv[i*2+1], argv[i*2+2], off);
		strcpy(headers[i].name, argv[i*2+2]);
		headers[i].offset = off;
		FILE *fp = fopen(argv[i*2+1], "r");
		if (fp == NULL) {
			perror(argv[i*2+1]);
			return 1;
		}
		fseek(fp, 0, SEEK_END);
		headers[i].length = ftell(fp);
		off += headers[i].length;
		fclose(fp);
		headers[i].magic = 0xBF;
	}

	FILE *fp2 = fopen("initrd.img", "w");
	fwrite(&nheaders, sizeof(int), 1, fp2);
	fwrite(headers, sizeof(struct initrd_header), 64, fp2);

	for (i = 0; i < nheaders; i++) {
		FILE *fp = fopen(argv[i*2+1], "r");
		unsigned char *buf = (unsigned char *)malloc(headers[i].length);
		fread(buf, 1, headers[i].length, fp);
		fwrite(buf, 1, headers[i].length, fp2);
		fclose(fp);
		free(buf);
	}

	fclose(fp2);

	return 0;
}
