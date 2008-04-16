#include "ndstool.h"
#include "ndstree.h"

/*
 * Variables
 */
unsigned int _entry_start;			// current position in name entry table
unsigned int file_top;				// current position to write new file to
unsigned int free_dir_id = 0xF000;	// incremented in ReadDirectory
unsigned int directory_count = 0;	// incremented in ReadDirectory
unsigned int file_count = 0;		// incremented in ReadDirectory
unsigned int total_name_size = 0;	// incremented in ReadDirectory
unsigned int file_end = 0;			// end of all file data. updated in AddFile
unsigned int free_file_id = 0;		// incremented in AddDirectory

/*
 * ReadDirectory
 * Read directory tree into memory structure
 * returns first (dummy) node
 */
TreeNode *ReadDirectory(TreeNode *node, char *path)
{
	//printf("%s\n", path);

	DIR *dir = opendir(path);
	if (!dir) { fprintf(stderr, "Cannot open directory '%s'.\n", path); exit(1); }

	struct dirent *de;
	while ((de = readdir(dir)))
	{
		if (!strcmp(de->d_name, ".")) continue;
		if (!strcmp(de->d_name, "..")) continue;

		char strbuf[MAXPATHLEN];
		strcpy(strbuf, path);
		strcat(strbuf, "/");
		strcat(strbuf, de->d_name);
		//printf("%s\n", strbuf);

		struct stat st;
		if (stat(strbuf, &st)) { fprintf(stderr, "Cannot get stat of '%s'.\n", strbuf); exit(1); }

		//if (S_ISDIR(st.st_mode) && !subdirs) continue;		// skip subdirectories

		total_name_size += strlen(de->d_name);

		if (S_ISDIR(st.st_mode))
		{
			node = node->New(de->d_name, true);
			node->dir_id = free_dir_id++;
			directory_count++;
			node->directory = ReadDirectory(new TreeNode(), strbuf);
		}
		else if (S_ISREG(st.st_mode))
		{
			node = node->New(de->d_name, false);
			file_count++;
		}
		else
		{
			fprintf(stderr, "'%s' is not a file or directory!\n", strbuf);
			exit(1);
		}
	}
	closedir(dir);

	while (node->prev) node = node->prev;	// return first
	return node;
}
