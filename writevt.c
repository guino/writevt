#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>

char *progname;

static int usage() {
	printf("Usage: %s ttydev text\n", progname);
	printf("       %s -n process_name text\n", progname);
	return 2;
}

static char *find_pid_by_name(const char *name) {
	DIR *proc_dir;
	struct dirent *entry;
	char cmdline_path[512];
	FILE *cmdline_file;
	static char pid_str[32];
	char cmdline[2048];
	size_t len;
	char *pid_end;
	int found = 0;

	proc_dir = opendir("/proc");
	if (!proc_dir)
		return NULL;

	while ((entry = readdir(proc_dir)) != NULL) {
		if (entry->d_type != DT_DIR)
			continue;

		strtol(entry->d_name, &pid_end, 10);
		if (*pid_end != '\0')
			continue;

		snprintf(cmdline_path, sizeof(cmdline_path), "/proc/%s/cmdline", entry->d_name);
		cmdline_file = fopen(cmdline_path, "r");
		if (!cmdline_file)
			continue;

		len = fread(cmdline, 1, sizeof(cmdline) - 1, cmdline_file);
		fclose(cmdline_file);
		if (len == 0)
			continue;

		cmdline[len] = '\0';
		for (size_t i = 0; i < len; i++) {
			if (cmdline[i] == '\0')
				cmdline[i] = ' ';
		}

		if (strncmp(cmdline, name, strlen(name)) == 0) {
			strncpy(pid_str, entry->d_name, sizeof(pid_str) - 1);
			pid_str[sizeof(pid_str) - 1] = '\0';
			found = 1;
			break;
		}
	}

	closedir(proc_dir);
	return found ? pid_str : NULL;
}

int main(int argc, char **argv) {
	int fd, argi;
	char *term = NULL;
	char *text = NULL;
	int use_name_flag = 0;
	char term_path[64];

	progname = argv[0];

	argi = 1;

	if (argi < argc && strcmp(argv[argi], "-n") == 0) {
		use_name_flag = 1;
		argi++;
	}

	if (argi < argc)
		term = argv[argi++];
	else {
		fprintf(stderr, "%s: no %s specified\n", progname,
			use_name_flag ? "process name" : "tty");
		return usage();
	}

	if (argi < argc)
		text = argv[argi++];
	else {
		fprintf(stderr, "%s: no text specified\n", progname);
		return usage();
	}

	if (argi != argc) {
		fprintf(stderr, "%s: too many arguments\n", progname);
		return usage();
	}

	if (use_name_flag) {
		char *pid = find_pid_by_name(term);
		if (!pid) {
			fprintf(stderr, "%s: process not found: %s\n", progname, term);
			return 1;
		}
		snprintf(term_path, sizeof(term_path), "/proc/%s/fd/0", pid);
		term = term_path;
	}

	fd = open(term, O_RDONLY);
	if (fd < 0) {
		perror(term);
		fprintf(stderr, "%s: could not open tty\n", progname);
		return 1;
	}

	while (*text) {
		if (ioctl(fd, TIOCSTI, text)) {
			perror("ioctl");
			return 1;
		}
		text++;
	}

	return 0;
}
