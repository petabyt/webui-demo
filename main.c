#include <stdbool.h>
#include <stdlib.h>
#include <webui.h>

char *get_fs_file(const char *filename, int *length);

const void *file_handler(const char *filename, int *length) {
	*length = 0;
	const char *data = get_fs_file(filename + 1, length);
	return data;
}

int main() {
	size_t my_window = webui_new_window();
	webui_set_file_handler(my_window, file_handler);
	webui_set_size(my_window, 640, 480);
	webui_show(my_window, "www/index.html");
	webui_wait();
	return 0;
}

#if defined(_MSC_VER)
	int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) {
	return main();
}
#endif
