#ifndef SL_CONF_H_
#define SL_CONF_H_

/*
 * sophia database
 * sphia.org
 *
 * Copyright (c) Dmitry Simonenko
 * BSD License
*/

typedef struct slconf slconf;

struct slconf {
	char *dir;
	int   dir_read;
	int   dir_write;
	int   dir_create;
	int   rotatewm;
};

#endif