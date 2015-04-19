/*
 * entry.cpp
 *
 *  Created on: Apr 7, 2015
 *      Author: matrixsmaster
 *
 *  Native ASCII-art 3D application testing unit
 *  Re-uses some code from TDWS and 3dMeshText Parser
 */

#include "CurseGUI.h"
#include "visual.h"
#include "u3ds.h"

#define MODE 3
/** TEST MODES:
 * 1 - testing CurseGUI background with some <atmospheric> animation
 * 2 - testing flat sprites in 3D space
 * 3 - scanline renderer
 * 4 - testing interpolation alg. for Z-plane interpolation
 * 5 - scanline renderer with direct calculation (without perspective distortion)
 * 6 - scanline renderer with <brutal> Z-plane calculations
 * 7 - some simple CurseGUI tests
 * 8 - test rectangulation
 */

/* Number of vertices */
#if (MODE == 2) || (MODE > 6)
#define NPNT 4
#else
#define NPNT (4*6)
#endif


#define PRNPNT(Xyz) printf("[%.2f; %.2f; %.2f]",Xyz.x,Xyz.y,Xyz.z)
#define STRPNT(Str,Xyz) snprintf(Str,sizeof(Str),"[% 06g; % 06g; % 06g]",Xyz.x,Xyz.y,Xyz.z)

CurseGUI* gui;
SGUIPixel* back;

int main(int argc, char* argv[])
{
	int i,j,k,l,s,m,key,ret,done,ix,iy,sx,sy,dx,dy;
	T3dPoint mov,cur,mid,rv,fov,upt,scl,tmp;
	T3dMatrix rot;
	T3dPoint fig[NPNT],buf[NPNT],qat[4];
	char str[1024];
	float x,y;
	float* zbuf;
	int* qbuf;
	CurseGUIWnd* lastwnd = NULL;

	memset(&mov,0,sizeof(mov));
	memset(&rot,0,sizeof(rot));
	memset(&fig,0,sizeof(fig));
	memset(&buf,0,sizeof(buf));
	memset(&upt,0,sizeof(upt));
	memset(&rv,0,sizeof(rv));

	mov.z = 1;
	fov.x = 10;
	fov.y = 10;
	fov.z = 0;
	scl.x = 1;
	scl.y = 1;
	scl.z = 1;
	rv.x = 311;
	rv.y = 177;

#if MODE == 2
	GenPlaneQ(2,upt,1,fig);
#elif (MODE > 2) && (MODE < 7)
	GenCubeQ(upt,1,fig);
#endif

#if MODE == 4

	for (i = 0; i < NPNT; i++) fig[i] = PntMulScal(fig[i],100);
	PRNPNT(fig[4]);
	printf(" -> ");
	PRNPNT(fig[5]);
	printf(" / Y(6) = ");
	cur = InterpolatePntByY(fig[4],fig[5],6);
	PRNPNT(cur);
	puts("");

	PRNPNT(fig[12]);
	printf(" | ");
	PRNPNT(fig[13]);
	puts("");
	PRNPNT(fig[14]);
	printf(" | ");
	PRNPNT(fig[15]);
	puts("");
	printf("ZQ(40,25) = %f\n",InterpolateZQ(fig+12,40,25));
	printf("ZQ2(40,25) = %f\n",InterpolateZQ2(fig+12,40,25));

#else

	gui = new CurseGUI();
	if (gui->GetLastResult()) abort();
	gui->SetColortable(gui_coltable,COLPAIRS);

resize:
	ret = gui->GetHeight()*gui->GetWidth();
	back = (SGUIPixel*)malloc(ret*sizeof(SGUIPixel));
	if (!back) {
		delete gui;
		abort();
	}
	zbuf = (float*)malloc(ret*sizeof(float));
	if (!zbuf) {
		delete gui;
		abort();
	}
	qbuf = (int*)malloc(ret*sizeof(int));
	if (!qbuf) {
		delete gui;
		abort();
	}
	gui->SetBackgroundData(back,ret);

	mid.x = gui->GetWidth() / 2;
	mid.y = gui->GetHeight() / 2;
	mid.z = 1; // viewing plane
	upt = mid;

#if (MODE == 1) || (MODE > 6)
	for (i = 0; i < gui->GetHeight(); i++)
		for (j = 0; j < gui->GetWidth(); j++) {
			l = i * gui->GetWidth() + j;
			if ((i < 3) && (j < 4)) {
				back[l].col = 4;
				back[l].sym = ' ';
			} else if (i < gui->GetHeight() / 2) {
				back[l].col = 2;
				back[l].sym = ' ';
			} else {
				back[l].col = 3;
				back[l].sym = '\\';
			}
		}
	k = 0;
	ix = iy = 0;
#endif

	done = 0;
	s = m = 0;
//	srt = 0;
	do {
		gui->Update(true);

#if (MODE > 1) && (MODE < 7)
		j = 0;
		snprintf(str,sizeof(str),"mov=[%.3f, %.3f, %.3f] rot=[%.3f, %.3f, %.3f] FOV=[%.0f, %.0f]",mov.x,mov.y,mov.z,rv.x,rv.y,rv.z,fov.x,fov.y);
		mvwaddstr(gui->GetWindow(),j++,0,str);
		snprintf(str,sizeof(str),"Cursor: %03.0f, %03.0f",upt.x,upt.y);
		mvwaddstr(gui->GetWindow(),j++,0,str);
		for (i = 0; i < NPNT / 2; i++) {
			STRPNT(str,buf[i*2]);
			mvwaddstr(gui->GetWindow(),i+j,0,str);
			STRPNT(str,buf[i*2+1]);
			mvwaddstr(gui->GetWindow(),i+j,30,str);
		}
		j += ++i;
		l = upt.y * gui->GetWidth() + upt.x;
		if (qbuf[l] >= 0) {
			snprintf(str,sizeof(str),"Selected: Polygon #%d (zbuf: %f)",qbuf[l],zbuf[l]);
			mvwaddstr(gui->GetWindow(),j++,0,str);
			for (i = 0; i < 2; i++) {
				STRPNT(str,buf[qbuf[l]*4+i*2]);
				mvwaddstr(gui->GetWindow(),j,0,str);
				STRPNT(str,buf[qbuf[l]*4+i*2+1]);
				mvwaddstr(gui->GetWindow(),j++,30,str);
			}
//			SortVectors(buf+(qbuf[l]*4),4,srt);
//			snprintf(str,sizeof(str),"Sorted by %d",srt);
//			mvwaddstr(gui->GetWindow(),j++,0,str);
//			for (i = 0; i < 2; i++) {
//				STRPNT(str,buf[qbuf[l]*4+i*2]);
//				mvwaddstr(gui->GetWindow(),j,0,str);
//				STRPNT(str,buf[qbuf[l]*4+i*2+1]);
//				mvwaddstr(gui->GetWindow(),j++,30,str);
//			}
#if MODE == 3
			snprintf(str,sizeof(str),"Vectors by distance:");
			mvwaddstr(gui->GetWindow(),j++,0,str);
//			tmp = CenterPoint(&buf[qbuf[l]*4],4);
//			upt.z = tmp.z;
			for (i = 0; i < 4; i++) {
				snprintf(str,sizeof(str),"%d: %.3f",i,Pnt2PntLen(buf[qbuf[l]*4+i],upt));
				mvwaddstr(gui->GetWindow(),j++,0,str);
			}
#endif
		}
#endif /*(MODE > 1) && (MODE < 7)*/

#if (MODE > 1) && (MODE < 8)
		mvchgat(upt.y,upt.x,1,A_REVERSE,0,NULL);
		wrefresh(gui->GetWindow());
#endif

		ret = halfdelay(1);
		if (ret != ERR) key = getch();
		else key = 0;

#if (MODE == 1) || (MODE == 8)

		if (key == 'q') done = 1;

#elif MODE != 7

		switch (key) {
		case 'q': done = 1; break;
		case 'd': mov.x += 1; break;
		case 'a': mov.x -= 1; break;
		case 's': mov.y -= 1; break;
		case 'w': mov.y += 1; break;
		case 'r': mov.z += 1; break;
		case 'f': mov.z -= 1; break;
		case 'R': mov.z += 0.1; break;
		case 'F': mov.z -= 0.1; break;
		case 'z': rv.x -= 1; break;
		case 'x': rv.x += 1; break;
		case 'c': rv.y -= 1; break;
		case 'v': rv.y += 1; break;
		case 'b': rv.z -= 1; break;
		case 'n': rv.z += 1; break;
		case '-': fov.x -= 1; break;
		case '=': fov.x += 1; break;
		case '_': fov.y -= 1; break;
		case '+': fov.y += 1; break;
		case 'i': upt.y -= 1; break;
		case 'k': upt.y += 1; break;
		case 'j': upt.x -= 1; break;
		case 'l': upt.x += 1; break;
//		case 'o': srt--;	break;
//		case 'p': srt++;	break;
		case '0': rv.x = rv.y = rv.z = 0; break;
		default: ;
		}

		if (rv.x < 0) 	rv.x = 360;
		if (rv.x > 360) rv.x = 0;
		if (rv.y < 0) 	rv.y = 360;
		if (rv.y > 360) rv.y = 0;
		if (rv.z < 0) 	rv.z = 360;
		if (rv.z > 360) rv.z = 0;
//		if (srt < 0)	srt = 0;
//		if (srt > 4)	srt = 4;

#else

		i = 0;
		switch (key) {
		case 'q': done = 1; break;
		case 'd': mov.x += 1; i = 1; break;
		case 'a': mov.x -= 1; i = 1; break;
		case 's': mov.y += 1; i = 1; break;
		case 'w': mov.y -= 1; i = 1; break;
		case 'i': upt.y -= 1; break;
		case 'k': upt.y += 1; break;
		case 'j': upt.x -= 1; break;
		case 'l': upt.x += 1; break;
		case 'f': gui->RmAllWindows(); break;
		case 'z': scl.x -= 1; i = 1; break;
		case 'x': scl.x += 1; i = 1; break;
		case 'c': scl.y -= 1; i = 1; break;
		case 'v': scl.y += 1; i = 1; break;
		case '=':
			lastwnd = gui->MkWindow(upt.x,upt.y,10,6);
			lastwnd->SetBackgroundData(back,60);
			break;
		case '-':
			if (lastwnd) gui->RmWindow(lastwnd);
			lastwnd = gui->GetWindowN(gui->GetNumWindows()-1); //it's a safe call
			break;
		default: ;
		}

		if (mov.x < 0) mov.x = 0;
		if (mov.x >= gui->GetWidth()) mov.x = gui->GetWidth() - 1;
		if (mov.y < 0) mov.y = 0;
		if (mov.y >= gui->GetHeight()) mov.y = gui->GetHeight() - 1;

		if ((i) && (lastwnd)) {
			lastwnd->Move(mov.x,mov.y); //best to avoid this
			lastwnd->Resize(scl.x,scl.y);
			if (lastwnd->UpdateSize())
				lastwnd->SetBackgroundData(back,scl.x*scl.y);
		}
#endif
		if (upt.x < 0)	upt.x = 0;
		if (upt.x >= gui->GetWidth()) upt.x = gui->GetWidth() - 1;
		if (upt.y < 0)	upt.y = 0;
		if (upt.y >= gui->GetHeight()) upt.y = gui->GetHeight() - 1;

#if (MODE > 1) && (MODE < 7)
		for (i = 0; i < (gui->GetHeight()*gui->GetWidth()); i++) {
			back[i].col = 1;
			back[i].sym = ' ';
		}
		memset(zbuf,0,gui->GetHeight()*gui->GetWidth()*sizeof(float));
		memset(qbuf,0xff,gui->GetHeight()*gui->GetWidth()*sizeof(int));

		for (i = 0; i < (sizeof(fig) / sizeof(T3dPoint)); i++) {
			rot = GenMtxRotX(rv.x * (3.1415926535 / 180));
			cur = MtxPntMul(rot,fig[i]);
			rot = GenMtxRotY(rv.y * (3.1415926535 / 180));
			cur = MtxPntMul(rot,cur);
			rot = GenMtxRotZ(rv.z * (3.1415926535 / 180));
			cur = MtxPntMul(rot,cur);

			cur = PntPlusPnt(cur,mov);
			cur = PntMulPnt(cur,scl);

			buf[i].z = cur.z;

#if MODE == 2
			if (cur.z <= 0) continue; //clip by Z
#endif

			x = ( cur.x * fov.x / cur.z ) + mid.x;
			y = (-cur.y * fov.y / cur.z ) + mid.y;

#if MODE == 5
			buf[i].x = cur.x;
			buf[i].y = cur.y;
#elif MODE == 3
			buf[i].x = x;
			buf[i].y = y;
#else
			ix = floor(x);
			iy = floor(y);
			buf[i].x = ix;
			buf[i].y = iy;
#endif

#if MODE == 2
			dx = ceil((fov.x / 2) / ((cur.z < 1)? 1:cur.z));
			if (dx < 1) dx = 1;
			dy = ceil((fov.y / 2) / ((cur.z < 1)? 1:cur.z));
			if (dy < 1) dy = 1;
			for (sx = ix - dx; sx < ix + dx; sx++)
				for (sy = iy - dy; sy < iy + dy; sy++)
					if ((sx >= 0) && (sy >= 0) && (sx < gui->GetWidth()) && (sy < gui->GetHeight())) {
						l = sy * gui->GetWidth() + sx;
						if ((zbuf[l] > 0) && (zbuf[l] < cur.z)) continue;
						zbuf[l] = cur.z;
						back[l].col = 2;
						switch (i) {
						case 0: back[l].sym = 'A'; break;
						case 1: back[l].sym = 'B'; break;
						case 2: back[l].sym = 'C'; break;
						case 3: back[l].sym = 'D'; break;
						}
					}
#endif
		}
#endif /*(MODE > 1) && (MODE < 7)*/

#if (MODE > 2) && (MODE < 7)
		//Scanline renderer
		for (sy = 0; sy < gui->GetHeight(); sy++) {
			cur.y = sy;
			for (sx = 0; sx < gui->GetWidth(); sx++) {
				cur.x = sx;
				l = sy * gui->GetWidth() + sx;
				for (j = 0; j < NPNT / 4; j++) /* For each polygon */ {

#if MODE == 5
					for (k = 0; k < 4; k++) {
						// perspective projection
						qat[k].z =   buf[j*4+k].z;
						qat[k].x = ( buf[j*4+k].x * fov.x / qat[k].z ) + mid.x;
						qat[k].y = (-buf[j*4+k].y * fov.y / qat[k].z ) + mid.y;
					}
					if (!isPntPoly4IsectA(cur,qat)) continue;

					tmp = CenterPoint(&buf[j*4],4);
					x = tmp.z * (-mid.x + cur.x) / fov.x;
					y = tmp.z * ( mid.y - cur.y) / fov.y;

					cur.z = InterpolateZQ(&buf[j*4],x,y);
#elif MODE == 3
					//check z clipping
					s = 0;
					for (k = 0; k < 4; k++) {
						qat[k] = buf[j*4+k];
						if (qat[k].z < 0) {
							s++;
//							qat[k].z = 0;
						}
					}
					if (s > 3) continue;
					//calculate the point distance
					cur.z = InterpolateZQ(qat,cur.x,cur.y);
#elif MODE == 6
					cur.z = InterpolateZQ2(&buf[j*4],cur.x,cur.y);
#endif
					if (cur.z <= 0) continue;
					if ((zbuf[l] > 0) && (zbuf[l] < cur.z)) continue;
#if MODE != 5
					if (!isPntPoly4IsectA(cur,(&buf[j*4]))) continue; //To lower the margin of error
#endif

					zbuf[l] = cur.z;
					qbuf[l] = j;
					back[l].col = j + 1;
//					back[l].sym = 'A' + (char)j;
					snprintf(str,sizeof(str),"%1.0f",cur.z);
					back[l].sym = str[0];
				} //polys
			} //X
		} //Y
#endif /*(MODE > 2) && (MODE < 7)*/

#if (MODE == 1) || (MODE > 6)
		//Grass and Sky
//		iy = abs(ix);
		for (i = 0; i < gui->GetHeight(); i++)
			for (j = 0; j < gui->GetWidth(); j++) {
				l = i * gui->GetWidth() + j;
				if (i < gui->GetHeight() / 2) {
					back[l].col = (s >= gui->GetWidth())? 1:2;
					if (j == 0) {
						back[l].sym = (rand() < (RAND_MAX/6))? '~':' ';
						/* unfinished cloud free-form generator */
#if 0
						if (ix == 0) {
							iy = rand() & (gui->GetHeight() >> 2);
							if (iy < 1) iy = 1;
							if (rand() < (RAND_MAX/8)) {
								ix = iy;
								back[l].sym = '~';
							} else {
								ix = -iy;
								back[l].sym = ' ';
							}
						} else if (iy > 0) {
							if (rand() < (RAND_MAX/3)) iy--;
							else if (rand() < (RAND_MAX/3)) iy++;
							back[l].sym = (ix>0)? '~':' ';
						} else {
							ix = iy = 0;
							back[l].sym = ' ';
						}
#endif
					}
#if MODE == 8
				/* reduced probability */
				} else if (rand() < (RAND_MAX/7)) {
#else
				} else if (rand() < (RAND_MAX/3)) {
#endif
					back[l].col = 3;
					switch (back[l].sym) {
					case '\\':
						back[l].sym = '|';
						break;
					case '|':
						back[l].sym = '/';
						break;
					default:
						back[l].sym = '\\';
					}
				}
			}

		//Clouds movement
//		if (ix > 0) ix--;
//		if (ix < 0) ix++;
		for (i = 0; i < gui->GetHeight() / 2; i++)
			for (j = gui->GetWidth()-1; j > 0; j--) {
				l = i * gui->GetWidth() + j;
				back[l].sym = back[l-1].sym;
			}

		//Sun and Moon
		for (i = 0; i < 3; i++)
			for (j = 0; j < gui->GetWidth(); j++) {
				l = i * gui->GetWidth() + j;
				if (s >= gui->GetWidth()) {
					m = s;
					s -= gui->GetWidth();
				} else m = 0;
				if ((j >= s) && (j < 4 + s)) {
					back[l].col = (m >= gui->GetWidth())? 5:4;
//					back[l].sym = ' ';
				} else {
					back[l].col = (m >= gui->GetWidth())? 1:2;
//					back[l].sym = ' ';
				}
				if (m >= gui->GetWidth()) s = m;
			}
		if (++k > 2) {
			k = 0;
			s++;
		}
		if (s >= gui->GetWidth() * 2) s = 0;
#endif /*(MODE == 1) || (MODE > 6)*/

#if MODE == 8
		//qbuf used for tracking touched pixels
		memset(qbuf,0,gui->GetHeight()*gui->GetWidth()*sizeof(int));
		for (sy = 0; sy < gui->GetHeight(); sy++) {
			l = sy * gui->GetWidth();
			for (sx = 0; sx < gui->GetWidth(); sx++,l++) {
				if ((qbuf[l] > 0) && (qbuf[qbuf[l]-1] > 0)) {
//					mvchgat(sy,sx,1,A_DIM,1,NULL);
					back[l].col = 6;
					continue;
				}
				qbuf[l] = -1;
				for (j = sy, i = sx+1; j < gui->GetHeight(); j++,i=sx) {
					for (ix = 0; i < gui->GetWidth(); i++,ix++) {
						iy = j * gui->GetWidth() + i;
						if ((back[iy].sym != back[l].sym) ||
								(qbuf[iy] > 0)) break;
						qbuf[iy] = l + 1;
					}
					if (ix < 2) break;
					else qbuf[l] = 1;
				}
			}
		}
		move(gui->GetHeight()-1,gui->GetWidth()-1);
//		wrefresh(gui->GetWindow());
#endif

		if (gui->UpdateSize()) {
			fprintf(stderr,"Resetting GUI...\n");
			free(back);
			free(zbuf);
			free(qbuf);
			gui->SetBackgroundData(NULL,0);
			gui->SoftReset();
			goto resize;
		}
	} while (!done);

	delete gui;
	free(back);
	free(zbuf);
	free(qbuf);

#endif /* MODE 4 */

	return 0;
}
