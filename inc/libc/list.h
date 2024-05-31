// list.h // ~ eylon

#if !defined(__LIBC_LIST_H)
#define __LIBC_LIST_H

#include <stddef.h>

/* Linked list utils.
    Each macro in here takes a [list] and an [item]; both are structs of the same
    type that contain a [next] and a [prev] attribute in them. [list] is the base
    of the list and [item] is a refrence to certain item in the list.
    
    Implementation notes: the [next] attribute of the last item in the list is always NULL,
    and the [prev] attribute of the first item is pointing to the last item in the list. 
*/

// Add an item to the start of the list so it will be the first
#define LIST_ADD_FRONT(list, item) \
    do { \
        if ((list)==NULL) { \
            (list)       = (item); \
            (list)->prev = (item); \
            (list)->next = NULL; \
        } \
        else { \
            (item)->prev = (list)->prev; \
            (item)->next = (list); \
            (list)->prev = (item); \
            (list)       = (item); \
        } \
    } while (0)


// Add an item to the end of a list so it will be the last
#define LIST_ADD_END(list, item) \
    do { \
        if ((list)==NULL) { \
            (list)       = (item); \
            (list)->prev = (item); \
            (list)->next = NULL; \
        } \
        else { \
            (item)->prev       = (list)->prev; \
            (item)->next       = NULL; \
            (list)->prev->next = (item); \
            (list)->prev       = (list)->prev->next; \
        } \
    } while (0)



// Add an item after another item to the list
#define LIST_ADD_AFTER(list, item, pos) \
    do { \
        if ((list)==NULL) { \
            (list)       = (item); \
            (list)->prev = (item); \
            (list)->next = NULL; \
        } \
        else { \
            (item)->prev = (pos); \
            (item)->next = (pos)->next; \
            if ((pos)->next!=NULL) { \
                (pos)->next->prev = (item); \
            } \
            else { \
                (list)->prev = (item); \
            } \
            (pos)->next  = (item); \
        } \
    } while (0)


// Add an item before another item to the list
#define LIST_ADD_BEFORE(list, item, pos) \
    do { \
        if ((list)==NULL) { \
            (list)       = (item); \
            (list)->prev = (item); \
            (list)->next = NULL; \
        } \
        else { \
            if ((pos)==(list)) { \
                LIST_ADD_FRONT(list, item); \
            } \
            else { \
                (pos)->prev->next = (item); \
                (item)->next      = (pos); \
            } \
        } \
    } while (0)


// Remove the first item in the list
#define LIST_REMOVE_FRONT(list) \
    do { \
        if ((list)!=NULL) { \
            if ((list)->next!=NULL) { \
                (list)->next->prev = (list)->prev; \
            } \
            (list) = (list)->next; \
        } \
    } while (0)


// Remove the last item in the list
#define LIST_REMOVE_END(list) \
    do { \
        if ((list)!=NULL) { \
            if ((list)->next!=NULL) { \
                (list)->prev = (list)->prev->prev; \
                (list)->prev->next = NULL; \
            } \
            else { \
                (list) = NULL; \
            } \
        } \
    } while (0)


// Remove a specific item from the list
#define LIST_REMOVE(list, item) \
    do { \
        if ((list) != NULL) { \
            if ((item)==(list)) { \
                LIST_REMOVE_FRONT((list)); \
            } \
            else if ((item)==(list)->prev) { \
                LIST_REMOVE_END((list)); \
            } \
            else { \
                (item)->next->prev = (item)->prev; \
                (item)->prev->next = (item)->next; \
            } \
        } \
    } while (0)

#endif