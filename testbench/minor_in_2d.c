/* 'Search for rectangular areas in a 2D array'
 * algorithms testbench.
 * I'm sorry for this totally undocumented code,
 * but it's for my personal use only.
 * In this test I've reinvented the wheel(s), but this
 * process was truly full of joy.
 * */
 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <ctype.h>
#include <math.h>

#define MODE 2
#define ROWS 50
#define COLS 140
#define MAXZ 1024

//Print initial buffer
#define PRTI 1

//Print copied buffer
//#define PRTB 1

//Print resulting buffers
//#define PRTR 1

//Print rectangles co-ordinates
//#define PRTC 1

#define USE_FIELDS_INTERSECTION 1

struct zbuf {
	int sx,sy,ex,ey;
	char c;
};

#if MODE == 1
#undef ROWS
#define ROWS 6
const char* in[ROWS] = {
	"0--------------------",
	"-----1111--------2222",
	"3377711111111----66--",
	"3377771111111--666688",
	"--77771111111-6666-88",
	"--5777--------66---48",
};
#elif MODE == 2
char* in[ROWS];
#endif

#define RNDMACRO(X) (int)floor((float)rand() / (float)RAND_MAX * (float)X);

/* Globals ('cause I'm too lazy now) */
int i,j,k,s,f,z;
int sx,sy,ex,ey,lx,ly;
int cycles,cycle;
int area;
unsigned sz;
unsigned long long len;
char c;
char* buf;
struct timeval tb,te;
struct zbuf zb[MAXZ];
int curz;
double ratio[3],speed;

/* ****************************************************************** */

inline int imin(int a, int b)
{
	return ((a < b)? a:b);
}

inline int imax(int a, int b)
{
	return ((a > b)? a:b);
}

void copybuf()
{
#ifdef PRTB
	printf("Buffer copy:\n");
	for (i=0; i<ROWS; i++) {
		memcpy(buf+(i*s),in[i],s);
		for (j=0; j<s; j++)
			putchar(*(buf+(i*s)+j));
		putchar('\n');
	}
#else
	for (i=0; i<ROWS; i++)
		memcpy(buf+(i*s),in[i],s);
#endif
	memset(zb,0,sizeof(zb));
	z = 0;
}

void printbuf()
{
#ifdef PRTR
	for (j=0; j<s; j++) putchar('0'+j);
	putchar('\n');
	for (i=0; i<ROWS; i++) {
		for (j=0; j<s; j++)
			putchar(*(buf+(i*s)+j));
		putchar('\n');
	}
#endif
}

void printres(int n)
{
	printf("Result:\n");
	
	printbuf();
	memset(buf,'-',sz);
	k = 0;
	area = 0;
	
	for (z = 0; z < curz; z++) {
		if (zb[z].ex && zb[z].ey) {
			k++;
			area += abs(zb[z].ex-zb[z].sx) * abs(zb[z].ey-zb[z].sy);
#ifdef PRTC
			printf("Rectangle %d: [%d %d] : [%d %d] char %c\n",z,
					zb[z].sx,zb[z].sy,zb[z].ex,zb[z].ey,zb[z].c);
			for (i = zb[z].sy; i <= zb[z].ey; i++) {
				for (j = zb[z].sx; j <= zb[z].ex; j++) {
					if ((in[i][j] != zb[z].c) && (in[i][j] != '-'))
						buf[i*s+j] = 'X';
					else
						buf[i*s+j] = '0'+z;
				}
			}
#endif
		}
	}

	printf("Total: %d rectangles with area of %d\n",k,area);
	printbuf();
	len = te.tv_usec - tb.tv_usec;
	if (ratio[n] > 0)
		ratio[n] = (ratio[n] + (double)k / (double)len) / 2.f;
	else
		ratio[n] = (double)k / (double)len;
	speed = (double)sz / (double)len;
	printf("Elapsed time = %llu us.\n",len);
	printf("Classification speed: %f rects/us\n",ratio[n]);
	printf("Raw speed: %f points/us\n",speed);
}

/* ****************************************************************** */

int main(int argc, char* argv[])
{
	puts("Init");
	srand(time(NULL));
	
	if (argc > 1) cycles = atoi(argv[1]);
	else cycles = 0;
	cycle = 0;
	for (i = 0; i < 3; i++) ratio[i] = 0;
	
start_over:
	
#if MODE == 2
	for (i = 0; i < ROWS; i++) {
		in[i] = (char*)malloc(COLS+1);
		if (!in[i]) abort();
		memset(in[i],'-',COLS);
		in[i][COLS] = 0;
	}
	
	for (i = 0; i < 10; i++) {
		sx = RNDMACRO(COLS);
		sy = RNDMACRO(ROWS);
#ifdef PRTC
		printf("%d %d\n",sx,sy);
#endif
		for (j = 0; j < COLS; j++) {
			k = RNDMACRO(4);
			switch (k) {
			case 0: sy--; break;
			case 1: sx++; break;
			case 2: sy++; break;
			case 3: sx--; break;
			default:
				fprintf(stderr,"k = %d\n",k);
				abort();
			}
			if ((sx < 0) || (sy < 0)) break;
			if ((sx >= COLS) || (sy >= ROWS)) break;
			in[sy][sx] = i + '0';
		}
	}
#endif
	
#ifdef PRTI
	for (i = 0; i < ROWS; i++) printf("%s\n",in[i]);
#endif
	
	s = strlen(in[0]);
	sz = sizeof(in) / sizeof(char*) * s;
	printf("size = %u\n",sz);
	buf = (char*)malloc(sz);
	if (!buf) abort();
	
	/* ************************************************************** */
	
	printf("\n\nDumb algorithm\n");
	copybuf();
	gettimeofday(&tb,NULL);
	do {
		f = 0;
		c = 0;
		for (i = 0; i < ROWS; i++) {
			for (j = 0; j < s; j++) {
				if (!c) {
					if (isdigit(buf[i*s+j])) {
						c = buf[i*s+j];
						buf[i*s+j] = '*';
						sx = j;
						sy = i;
						ex = sx;
						ey = sy;
					}
				} else {
					if (buf[i*s+j] == c) {
						buf[i*s+j] = '*';
						if (j < sx) sx = j;
						if (j > ex) ex = j;
						if (i < sy) sy = i;
						if (i > ey) ey = i;
						f = 1;
					}
				}
			}
		}
		if (f && (ex != sx) && (ey != sy)) {
			//printf("Rectangle: [%d %d] : [%d %d]\n",sx,sy,ex,ey);
			zb[z].sx = sx;
			zb[z].sy = sy;
			zb[z].ex = ex;
			zb[z].ey = ey;
			zb[z].c = c;
			z++;
			if (z >= MAXZ) {
				fprintf(stderr,"z's out of range\n");
				abort();
			}
		}
	} while (c);
	curz = z;
	gettimeofday(&te,NULL);
	
	printres(0);
	
	/* ************************************************************** */
	
	printf("\n\nField algorithm\n");
	copybuf();
	gettimeofday(&tb,NULL);
	
	do {
		f = 0;
		c = 0;
		for (i = 0; (i < ROWS) && (!f); i++) {
			for (j = 0; (j < s) && (!f); j++) {
				if (isdigit(buf[i*s+j])) {
					c = buf[i*s+j];
					sx = j;
					sy = i;
					ex = j;
					for (ey = sy; ey < ROWS; ey++) {
						f = 0;
						if (buf[ey*s+sx] != c) {
							ey--;
							break;
						}
						for (k = sx; k < s; k++) {
							//if (k < 0) k = 0;
						//for (k = 0; k < s; k++) {
							if (buf[ey*s+k] == c) {
								buf[ey*s+k] = '*';
								f = 1;
								//if (k < sx) sx = k;
							} else /*if (k >= sx)*/ {
								k--;
								break;
							}
						}
						if (k > ex) ex = k;
						if (!f) break;
					}
					f = 1;
					if (ex == s) ex--;
					if (ey == ROWS) ey--;
				}
			}
		}
		if (f && (ex != sx) && (ey != sy)) {
			//printf("Rectangle: [%d %d] : [%d %d]\n",sx,sy,ex,ey);
			zb[z].sx = sx;
			zb[z].sy = sy;
			zb[z].ex = ex;
			zb[z].ey = ey;
			zb[z].c = c;
			z++;
			if (z >= MAXZ) {
				fprintf(stderr,"z's out of range\n");
				abort();
			}
		}
	} while (c);
	curz = z;
#if USE_FIELDS_INTERSECTION > 0
	for (i = 0; i < curz; i++) {
		if ((!zb[i].ex) || (!zb[i].ey))
			continue;
		for (j = i+1; j < curz; j++) {
			if (zb[j].c == zb[i].c) {
				zb[i].sx = imin(zb[i].sx,zb[j].sx);
				zb[i].sy = imin(zb[i].sy,zb[j].sy);
				zb[i].ex = imax(zb[i].ex,zb[j].ex);
				zb[i].ey = imax(zb[i].ey,zb[j].ey);
				zb[j].ex = 0;
				zb[j].ey = 0;
				zb[j].c = 0;
			}
		}
	}
#endif
	gettimeofday(&te,NULL);
	
	printres(1);
	
	/* ************************************************************** */
	
	printf("\n\nSelect algorithm\n");
	copybuf();
	gettimeofday(&tb,NULL);
	
	lx = 0;
	ly = 0;
	do {
		f = 0;
		c = 0;
		for (i = ly; (i < ROWS) && (!f); i++) {
			for (j = lx; (j < s) && (!f); j++) {
				if (isdigit(buf[i*s+j])) {
					c = buf[i*s+j];
					sx = j;
					sy = i;
					ex = sx;
					for (ey = sy; ey < ROWS; ey++) {
						for (k = sx; k < ((ex > sx)? ex+1:s); k++) {
							if (buf[ey*s+k] != c) {
								f = 1;
								break;
							}
						}
						if (f) {
							if (ex <= sx) {
								ex = k-1;
								f = 0;
							} else if (k == sx) {
								ey--;
								break;
							} else if (k < ex)
								break;
						}
					}
					if (ey == ROWS) ey--;
				}
			}
		}
		if (f && (ex > sx) && (ey > sy)) {
			//printf("Rectangle: [%d %d] : [%d %d]\n",sx,sy,ex,ey);
			//zb[z].sx = (sx < ex)? sx:ex;
			//zb[z].sy = (sy < ey)? sy:ey;
			//zb[z].ex = (ex > sx)? ex:sx;
			//zb[z].ey = (ey > sy)? ey:sy;
			zb[z].sx = sx;
			zb[z].sy = sy;
			zb[z].ex = ex;
			zb[z].ey = ey;
			zb[z].c = c;
			for (i = zb[z].sy; i <= zb[z].ey; i++)
				for (j = zb[z].sx; j <= zb[z].ex; j++)
					buf[i*s+j] = '*';
			z++;
			if (z >= MAXZ) {
				fprintf(stderr,"z's out of range\n");
				abort();
			}
			lx += (ex-sx);
		} else
			lx++;
		if (lx >= s) {
			ly++;
			lx = 0;
		}
		if (ly >= ROWS) break;
	} while (c);
	curz = z;
	gettimeofday(&te,NULL);
	
	printres(2);
	
	/* ************************************************************** */
	
	free(buf);
	
#if MODE == 2
	for (i = 0; i < ROWS; i++) free(in[i]);
#endif

	/* ************************************************************** */
	
	if (++cycle < cycles) goto start_over;
	
	puts("Exit");
	
	return 0;
}
