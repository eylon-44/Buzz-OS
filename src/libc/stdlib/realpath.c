// stdlib/realpath.c // ~ eylon

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

/* Get the canonicalized absolute pathname.

    realpath() resolves all references to /./../ and extra '/' characters in
    the null-terminated string named by [path] to produce a canonicalized absolute pathname.
    
    The resulting pathname is stored as a null-terminated string, up to a maximum of PATH_MAX
    bytes, in the buffer pointed to by [resolved_path]. The resulting path will have no /./ or /../ components.

    If resolved_path is specified as NULL, then realpath() uses malloc() to allocate a buffer of up
    to PATH_MAX bytes to hold the resolved pathname, and returns a pointer to this buffer. The caller
    should deallocate this buffer using free()
    .
    On success, realpath() returns a pointer to the resolved_path. On error, NULL is returned and the
    contents of [resolved_path] are undefined.

    #include <stdlib.h>
    char* realpath(const char* path, char resolved[PATH_MAX]);
*/
char* realpath(const char* path, char resolved[PATH_MAX])
{
	char *p, *q, *s;
	size_t left_len, resolved_len;
	char left[PATH_MAX], next_token[PATH_MAX];

	if (path[0] == '/') {
		resolved[0] = '/';
		resolved[1] = '\0';
		if (path[1] == '\0') {
			return resolved;
        }
		resolved_len = 1;
		left_len = strlcpy(left, path + 1, sizeof(left));
	}
    else {
		if (getcwd(resolved, PATH_MAX) == NULL) {
			strlcpy(resolved, ".", PATH_MAX);
			return NULL;
		}
		resolved_len = strlen(resolved);
		left_len = strlcpy(left, path, sizeof(left));
	}
	if (left_len >= sizeof(left) || resolved_len >= PATH_MAX) {
		return NULL;
	}

	// Iterate over path components in [left]
	while (left_len != 0) {
		// Extract the next path component and adjust [left] and its length
		p = strchr(left, '/');
		s = p ? p : left + left_len;
		if (s - left >= (int) sizeof(next_token)) {
			return NULL;
		}
		memcpy(next_token, left, s - left);
		next_token[s - left] = '\0';
		left_len -= s - left;
		if (p != NULL) {
			memmove(left, s + 1, left_len + 1);
        }
		if (resolved[resolved_len - 1] != '/') {
			if (resolved_len + 1 >= PATH_MAX) {
				return NULL;
			}
			resolved[resolved_len++] = '/';
			resolved[resolved_len] = '\0';
		}
		if (next_token[0] == '\0')
			continue;
		else if (strcmp(next_token, ".") == 0)
			continue;
		else if (strcmp(next_token, "..") == 0) {
			// Strip the last path component except when we have a single "/"
			if (resolved_len > 1) {
				resolved[resolved_len - 1] = '\0';
				q = strrchr(resolved, '/') + 1;
				*q = '\0';
				resolved_len = q - resolved;
			}
			continue;
		}

		/* Append the next path component and lstat() it. If lstat() fails we still can return successfully if
		    there are no more path components left. */
		resolved_len = strlcat(resolved, next_token, PATH_MAX);
		if (resolved_len >= PATH_MAX) {
			return NULL;
		}
	}

	// Remove trailing slash except when the resolved pathname is a single '/'
	if (resolved_len > 1 && resolved[resolved_len - 1] == '/')
		resolved[resolved_len - 1] = '\0';

	return (resolved);
}