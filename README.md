# webui-demo

This is a template for creating an standalone executable with [webui](https://github.com/webui-dev/webui). Once statically linked, it should run on any x86_64 Linux
distro from the executable alone. The `fs.c` utility is ran at built time to recursively pack assets into a C file, which can be accessed through `get_fs_file`.

I was easily able to get a 980k static binary using `staticx`. For comparison, a GTK3.0 static binary made with the same tools comes out as 9.8mb.
