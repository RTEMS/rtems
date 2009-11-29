inline int cmp(const char *a, bool a_isdir, const char *b, bool b_isdir)
{
	// oh... directory sort doesn't matter since we write out dir- and filenames seperately
	//if (a_isdir && !b_isdir) return -1;
	//if (b_isdir && !a_isdir) return +1;
	return strcmp(a, b);
}

struct TreeNode
{
	unsigned int dir_id;		// directory ID in case of directory entry
	const char *name;		// file or directory name
	TreeNode *directory;		// nonzero indicates directory. first directory node is a dummy
	TreeNode *prev, *next;		// linked list

	TreeNode()
	{
		dir_id = 0;
		name = "";
		directory = 0;
		prev = next = 0;
	}

	// new entry in same directory
	TreeNode *New(const char *name, bool isdir)
	{
		TreeNode *newNode = new TreeNode();
		newNode->name = strdup(name);

		TreeNode *node = this;

		if (cmp(name, isdir, node->name, node->dir_id) < 0)	// prev
		{
			while (cmp(name, isdir, node->name, node->dir_id) < 0)
			{
				if (node->prev)
					node = node->prev;
				else
					break;		// insert after dummy node
			}
		}
		else
		{
			while (node->next && (cmp(name, isdir, node->next->name, node->next->dir_id) >= 0))
			{
				node = node->next;
			}
		}

		// insert after current node
		newNode->prev = node;
		newNode->next = node->next;
		if (node->next) node->next->prev = newNode;
		node->next = newNode;

		return newNode;
	}
};

TreeNode *ReadDirectory(TreeNode *node, char *path);
