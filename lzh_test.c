/* lzh test program - @Sanguinarious <Sanguinarious@OccultusTerra.com> */

#include <stdio.h>
#include <stdlib.h>
#include "lzh.h"

typedef struct {
	char *inb, *outb;
	int in_size, in_read, in_left;
	int out_size, out_written, out_left;
} compress_info;

int lzh_read(void *d, int n, void *z) {
	compress_info *p=(compress_info *)z;
	if(p->inb!=NULL && p->in_size>0) {
		if(n <= p->in_left) {
			memcpy(d, p->inb, n);
			p->in_read += n; p->in_left -= n;
			p->inb += n;
			return n;
		} else {
			if(p->in_left<=0)
				return 0;
			else if(n>0 && p->in_left>0) {
				int x;
				memcpy(d, p->inb, p->in_left);
				x=p->in_left; p->in_read += x; p->in_left=0;
				p->inb += p->in_left;
				return x;
			} else
				return 0;
		}
	} else
		return 0;
}

int lzh_write(void *d, int n, void *z) {
	compress_info *p=(compress_info *)z;
	if(p->outb!=NULL && p->out_size>0) {
		if(n <= p->out_left) {
			memcpy(p->outb, d, n);
			p->out_written += n; p->out_left -= n;
			p->outb += n;
			return n;
		} else {
			if(p->out_left<=0)
				return 0;
			else if(n>0 && p->out_left>0) {
				int x;
				memcpy(p->outb, d, p->out_left);
				x=p->out_left; p->out_written += x; p->out_left=0;
				p->outb += p->out_left;
				return x;
			} else
				return 0;
		}
	} else
		return 0;
}

void setup(compress_info *c, char *in, int insize, char *out, int outsize) {
	c->inb = in; c->outb = out;
	c->in_left = c->in_size = insize;
	c->out_left = c->out_size = outsize;
	c->out_written = c->in_read = 0;
}

int main(int argc, char **argv) {
	char test_buffer[8096], out[10240];
	int y=0, x=8000;
	compress_info c, d;
	memzero(&c, sizeof(compress_info));
	memzero(&d, sizeof(compress_info));
	memzero(test_buffer, 8096);
	memzero(out, 10240);
	memset(test_buffer, 'X', x);
	setup(&c, test_buffer, x, out, 10240);
	y=lzh_freeze(lzh_read, lzh_write, malloc, free, (void *)&c);
	printf("%i Compressed %i to %i\n", y, x, c.out_written);
	memzero(test_buffer, 8096);
	setup(&d, out, c.out_written, test_buffer, 8096);
	y=lzh_melt(lzh_read, lzh_write, malloc, free, x, (void *)&d);
	printf("%i Decompressed %i to %i\n", y, x, d.out_written);
	memset(out, 'X', x);
	if(memcmp(out, test_buffer, x))
		printf("Success Bitches\n");
	else
		printf("Bastard\n");
	return 0;
}
