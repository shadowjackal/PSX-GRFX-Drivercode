
#include "fix16.hpp"
#include "inline_n.h"
#include "sincosmath.h"
#include "matrixstuff.h"

#define qN	10
#define qA	12
#define B	19900
#define	C	3516

int isin(int x) {

    int c, x2, y;

    c= x<<(30-qN);              // Semi-circle info into carry.
    x -= 1<<qN;                 // sine -> cosine calc

    x= x<<(31-qN);              // Mask with PI
    x= x>>(31-qN);              // Note: SIGNED shift! (to qN)

    x= x*x>>(2*qN-14);          // x=x^2 To Q14

    y= B - (x*C>>14);           // B - x^2*C
    y= (1<<qA)-(x*y>>16);       // A - x^2*(B-x^2*C)

    return c>=0 ? y : -y;

}

int icos(int x) {

    return isin( x+1024 );

}

MATRIX *RotMatrix(SVECTOR *r, MATRIX *m) {

	short s[3],c[3];
	MATRIX tm[3];

	s[0] = isin(r->vx);		s[1] = isin(r->vy);		s[2] = isin(r->vz);
	c[0] = icos(r->vx);		c[1] = icos(r->vy);		c[2] = icos(r->vz);

	// mX
	m->m[0][0] = ONE;		m->m[0][1] = 0;			m->m[0][2] = 0;
	m->m[1][0] = 0;			m->m[1][1] = c[0];		m->m[1][2] = -s[0];
	m->m[2][0] = 0;			m->m[2][1] = s[0];		m->m[2][2] = c[0];

	// mY
	tm[0].m[0][0] = c[1];	tm[0].m[0][1] = 0;		tm[0].m[0][2] = s[1];
	tm[0].m[1][0] = 0;		tm[0].m[1][1] = ONE;	tm[0].m[1][2] = 0;
	tm[0].m[2][0] = -s[1];	tm[0].m[2][1] = 0;		tm[0].m[2][2] = c[1];

	// mZ
	tm[1].m[0][0] = c[2];	tm[1].m[0][1] = -s[2];	tm[1].m[0][2] = 0;
	tm[1].m[1][0] = s[2];	tm[1].m[1][1] = c[2];	tm[1].m[1][2] = 0;
	tm[1].m[2][0] = 0;		tm[1].m[2][1] = 0;		tm[1].m[2][2] = ONE;

	PushMatrix();

	MulMatrix0( m, &tm[0], &tm[2] );
	MulMatrix0( &tm[2], &tm[1], m );

	PopMatrix();

	return m;

}

MATRIX *TransMatrix(MATRIX *m, VECTOR *r) {

	m->t[0] = r->vx;
	m->t[1] = r->vy;
	m->t[2] = r->vz;

	return m;

}