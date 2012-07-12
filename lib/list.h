
#ifndef LIST_H
#define LIST_H


#include <inc/types.h>


#define containerof(_ptr, _type, _member)       (_type *)((char *)(_ptr) - __builtin_offsetof (_type, _member))



struct list_head {
	struct list_head *prev, *next;
};



static inline void list_init (struct list_head *h)
{
	h->prev = h;
	h->next = h;
}



#define list_foreach_entry(_var, _head, _member)				\
	for (_var = containerof ((_head)->next, typeof (*_var), _member);	\
		&_var->_member != (_head);					\
		_var = containerof (p->_member.next, typeof (*_var), _member))



#endif /* LIST_H */


