
#ifndef LIB_LIST_H
#define LIB_LIST_H


#include <stdint.h>

#include <lib/mem.h>




struct list_head {
	struct list_head *prev, *next;
};



static inline void list_init(struct list_head *h)
{
	h->prev = h;
	h->next = h;
}


static inline void list_add(struct list_head *head, struct list_head *new)
{
	new->next = head->next;
	new->prev = head;
	head->next->prev = new;
	head->next = new;
}



static inline void list_add_back(struct list_head *head, struct list_head *new)
{
	new->next = head;
	new->prev = head->prev;
	head->prev->next = new;
	head->prev = new;
}



static inline void list_del(struct list_head *e)
{
	e->prev->next = e->next;
	e->next->prev = e->prev;
}



static inline struct list_head *list_next(struct list_head *h)
{
	return h->next;
}



static inline struct list_head *list_prev(struct list_head *h)
{
	return h->prev;
}



static inline struct list_head *list_pop(struct list_head *h)
{
	struct list_head *l = h->next;

	list_del(h->next);

	return l;
}



static inline u8 list_empty(const struct list_head *head)
{
	return head->next == head;
}



/* Linux kernel terminology */
#define list_for_each_entry(_var, _head, _memb)			\
for (_var = containerof ((_head)->next, typeof (*_var), _memb);	\
	&((_var)->_memb) != (_head);				\
	_var = containerof ((_var)->_memb.next, typeof (*_var), _memb))


#define list_for_each(_var, _head)				\
for (_var = (_head)->next; (_var) != (_head); _var = _var->next)



#endif /* LIB_LIST_H */


