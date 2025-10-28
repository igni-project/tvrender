#include "id.h"

int search_id(int *ids, int idCount, int id)
{
	int idx = 0;

	while (idx < idCount)
	{
		if (ids[idx] == id)
		{
			return idx;
		}

		++idx;
	}

	return -1;
}

