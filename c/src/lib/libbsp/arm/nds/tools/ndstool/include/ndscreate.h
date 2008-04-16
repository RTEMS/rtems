#include "ndstree.h"

void AddFile(char *rootdir, char *prefix, char *entry_name, unsigned int file_id, unsigned int alignmask);
void AddDirectory(TreeNode *node, char *prefix, unsigned int this_dir_id, unsigned int _parent_id, unsigned int alignmask);
void Create();
