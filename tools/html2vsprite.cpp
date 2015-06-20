#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <string>

using namespace std;

///Color triplet for ncurses.
struct SCTriple {
	short r,g,b;
};

///ncurses GUI <pixel> type
struct SGUIPixel {
	char sym;
	SCTriple fg,bg;
};

int row = 0;
int col = 0;
int ccol = 0;

vector<vector<SGUIPixel> > img;

char* getsym(char* str)
{
	char buf[1024];
	int r;
	
	/* Fucking DUMB implementation */
	
	do {
		r = sscanf(str,"<font %1024[^>]",buf);
		str++;
	} while ((r != 1) && (r != EOF));
	
	if (r == 1) {
		str += 6 + strlen(buf); //pattern + buf + one char
		//printf("'%s' '%s'\n",buf,str);
		return str;
	}
	return NULL;
}

SCTriple ncol(int x)
{
	SCTriple r;
	
	r.r = (short)((x>>16)&0xff) * 1000 / 256;
	r.g = (short)((x>> 8)&0xff) * 1000 / 256;
	r.b = (short)((x>> 0)&0xff) * 1000 / 256;
	
	return r;
}

SGUIPixel parse(char* str)
{
	char buf[128];
	char* tmp, * bak = str;
	int r;
	SGUIPixel cur;
	
	tmp = strstr(str,"bgcolor");
	if (!tmp) {
		cur.bg.r = 0; cur.bg.g = 0; cur.bg.b = 0;
	} else {
		tmp += 10;
		sscanf(tmp,"%x",&r);
		cur.bg = ncol(r);
		//printf("%s\t-> %d 0x%x\n",tmp,r,r);
		str = tmp;
	}
	
	tmp = strstr(str,"color");
	if (!tmp) {
		cur.fg.r = 0; cur.fg.g = 0; cur.fg.b = 0;
	} else {
		tmp += 8;
		sscanf(tmp,"%x",&r);
		cur.fg = ncol(r);
		//printf("%s\t-> %d 0x%x\n",tmp,r,r);
	}
	
	tmp = getsym(bak);
	if (!tmp) {
		printf("\nNo symbol detected for row %d col %d\n",row,col);
		abort();
	}
	
	if (tmp[0] == '<') cur.sym = ' ';
	else if (tmp[0] == '&') {
		do {
			r = sscanf(tmp,"&%128[^;]",buf);
			tmp++;
		} while ((r != 1) && (r != EOF));
		
		if (r == 1) {
			if (!strcmp(buf,"amp")) cur.sym = '&';
			else if (!strcmp(buf,"lt")) cur.sym = '<';
			else if (!strcmp(buf,"gt")) cur.sym = '>';
			else {
				printf("\nUnknown special character '%s' !\n",buf);
				abort();
			}
		} else {
			printf("\nUnable to parse string '%s' !\n",bak);
			abort();
		}
	} else if (isprint(tmp[0])) cur.sym = tmp[0];
	else if (tmp[0] == (char)226) cur.sym = '\"';
	else {
		printf("\nUnprintable character %hhu [0x%02hhX] at row %d col %d\n",
				(unsigned char)tmp[0],(unsigned char)tmp[0],row,col);
		abort();
	}
	
	return cur;
}

void outpic(void)
{
	puts("Picture:");
	for (int r = 0; r < row; r++) {
		for (int c = 0; c < ccol; c++)
			putchar(img[r][c].sym);
		putchar('\n');
	}
	puts("End of picture.");
}

bool equ(SCTriple a, SCTriple b)
{
	return (!((a.r != b.r) || (a.g != b.g) || (a.b != b.b)));
}

int main(int argc, char* argv[])
{
	FILE* fi, *fo;
	char str[4096],buf[4096];
	int fsm = 0;
	char* tmp;
	int r,c,p;
	bool end;
	char transp;
	SGUIPixel pair;
	vector<string> frm;
	string line;
	vector<string>::iterator fit;
	
	if (argc != 4) abort();
	
	fi = fopen(argv[1],"r");
	if (!fi) abort();
	
	while (!feof(fi)) {
		fgets(str,sizeof(str),fi);
		switch (fsm) {
		case 0:
			if (strstr(str,"<tr>")) {
				fsm++;
				row++;
				img.push_back(vector<SGUIPixel>());
				printf("Row %d found\n",row);
			}
			break;
			
		case 1:
			if (strstr(str,"</tr>")) {
				if (ccol && (ccol != col)) {
					printf("Row %d have %d columns, but last row had %d columns!\n",row,col,ccol);
					abort();
				}
				ccol = col;
				fsm = 0;
				col = 0;
			} else {
				memset(buf,0,sizeof(buf));
				tmp = strstr(str,"<td ");
				if (tmp) {
					col++;
					//printf("Column %d detected\n",col);
					img[row-1].push_back(parse(tmp));
				}
			}
			break;
		}
	}
	
	fclose(fi);
	
	printf("Read success. %d rows by %d columns\n",row,ccol);
	
	outpic();
	
	transp = atoi(argv[3]);
	if ((transp <= 0) || (transp > 255)) abort();
	printf("Transparent char code %d\n",transp);
	
	printf("\nWriting to %s...\n",argv[2]);
	
	if (!strcmp(argv[2],"-"))
		fo = stdout;
	else
		fo = fopen(argv[2],"w");
	if (!fo) abort();
	
	for (;;) {
		end = true;
		
		for (r = 0; r < row; r++) {
			line = "";
			for (c = 0; c < ccol; c++) {
				if (img[r][c].sym) {
					if (end) {
						end = false;
						pair = img[r][c];
						img[r][c].sym = 0;
					} else {
						if (equ(pair.fg,img[r][c].fg) &&
							equ(pair.bg,img[r][c].bg)) {
							pair = img[r][c];
							img[r][c].sym = 0;
						} else {
							pair.sym = transp;
							if (img[r][c].sym == transp)
								img[r][c].sym = 0;
						}
					}
				} else {
					pair.sym = transp;
				}
				//fputc(pair.sym,fo);
				line += pair.sym;
			}
			//fputc('\n',fo);
			frm.push_back(line);
		}
		
		if (end) break;
		
		p++;
		fprintf(fo,"%hd %hd %hd %hd %hd %hd\n",pair.fg.r,pair.fg.g,pair.fg.b,
												pair.bg.r,pair.bg.g,pair.bg.b);
		for (fit = frm.begin(); fit != frm.end(); ++fit)
			fprintf(fo,"%s\n",fit->c_str());
		frm.clear();
	}
	fprintf(fo,"%d %d %d\n",ccol,row,p);
	
	if (fo != stdout)
		fclose(fo);
	
	printf("Done.\n");
	
	return 0;
}
