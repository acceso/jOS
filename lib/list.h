
#ifndef LIST_H
#define LIST_H


#include <inc/types.h>


#define containerof(_ptr, _type, _member) \
	(_type *)((char *)(_ptr) - __builtin_offsetof (_type, _member))



struct list_head {
	struct list_head *prev, *next;
};



static inline void
list_init (struct list_head *h)
{
	h->prev = h;
	h->next = h;
}


static inline void
list_add (struct list_head *new, struct list_head *head)
{
	new->next = head->next;
	new->prev = head;
	head->next->prev = new;
	head->next = new;
}


#define list_foreach_entry(_var, _head, _member)				\
	for (_var = containerof ((_head)->next, typeof (*_var), _member);	\
		&_var->_member != (_head);					\
		_var = containerof (p->_member.next, typeof (*_var), _member))



#endif /* LIST_H */


