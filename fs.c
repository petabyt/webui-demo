/*
Build utility to compile files recursively into the binary.
It will recursively scan the www directory and copy all files into dump.c,
which exposes this function:
  char *get_fs_file(char *filename, int *length);
Which will return any file in the www directory, and set the length variable.
(The files are prefixed with www/)
*/

#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>

char *name_cache1[256];
char *name_cache2[256];
int cache_len = 0;

void fs_fix_file(char *name) {
	for (int i = 0; name[i] != '\0'; i++) {
		if (name[i] == '.') {
			name[i] = '_';
		} else if (name[i] == '/') {
			name[i] = '_';
		} else if (name[i] == '-') {
			name[i] = '_';
		} else if (name[i] == ' ') {
			name[i] = '_';
		} else if (name[i] == '+') {
			name[i] = '_';
		}
	}
}

void read_folder(char *name);

void add_name(char *name) {
	FILE *f = fopen(name, "r");
	if (f == NULL) {
		return;
	}

	name_cache1[cache_len] = malloc(strlen(name) + 1);
	strcpy(name_cache1[cache_len], name);
	fs_fix_file(name);
	name_cache2[cache_len] = malloc(strlen(name) + 1);
	strcpy(name_cache2[cache_len], name);
	cache_len++;
}

void read_folder(char *name) {
	DIR *d = opendir(name);
	if (d == NULL) {
		return;
	}

	struct dirent *drnt;
	while ((drnt = readdir(d)) != NULL) {
		if (drnt->d_name[0] == '.') continue;
		char buf[128];
		strcpy(buf, name);
		strcat(buf, drnt->d_name);
		puts(buf);
		if (drnt->d_type == DT_DIR) {
			strcpy(buf, name);
			strcat(buf, drnt->d_name);
			strcat(buf, "/");
			read_folder(buf);
		} else if (drnt->d_type == DT_REG) {
			add_name(buf);
		}
	}
}

void write_file(FILE *o) {
	char buf[512];

	fputs("#include <string.h>\n", o);

	for (int i = 0; i < cache_len; i++) {
		sprintf(buf, "extern const char _%s_data[];\n", name_cache2[i]);
		fwrite(buf, strlen(buf), 1, o);

		// incbin the file
		sprintf(buf, "__asm__(\"_%s_data: .incbin \\\"%s\\\"\\n\");\n", name_cache2[i], name_cache1[i]);
		fwrite(buf, strlen(buf), 1, o);

		// Specify end label
		sprintf(buf, "__asm__(\"_%s_data_end:\\n\");\n", name_cache2[i]);
		fwrite(buf, strlen(buf), 1, o);

		sprintf(buf, "extern int _%s_length;\n", name_cache2[i]);
		fwrite(buf, strlen(buf), 1, o);

		sprintf(buf, "__asm__(\"_%s_length: .int _%s_data_end - _%s_data\\n\");\n", name_cache2[i], name_cache2[i], name_cache2[i]);
		fwrite(buf, strlen(buf), 1, o);
	}

	fputs("const char *get_fs_file(const char *filename, int *length) {\n", o);
	char *tok = "";
	for (int i = 0; i < cache_len; i++) {
		sprintf(buf, "%sif (!strcmp(filename, \"%s\")) {"
			"\n*length = _%s_length; return _%s_data;\n}", tok, name_cache1[i], name_cache2[i], name_cache2[i]);
		fwrite(buf, strlen(buf), 1, o);
		tok = "else ";
	}

	fputs("return NULL;}", o);
}

int main(int argc, char *argv[]) {
	FILE *f = fopen("dump.c", "w");

	read_folder("www/");

	write_file(f);

	fclose(f);

	for (int i = 0; i < cache_len; i++) {
		free(name_cache1[i]);
		free(name_cache2[i]);
	}

	return 0;
}
