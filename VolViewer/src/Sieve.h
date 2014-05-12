#ifndef SIEVE_H
#define SIEVE_H

#include <iostream>

#include "VolumeObject.h"

#define byte    unsigned char
#define sieve_pointer   long int//__int64//long int //int
#define fbits	unsigned char
#define short_int_proxy short int
#define int64_proxy long int//__int64//long int

/* Define filter types */
#define MEDIAN	  ('m')		/* process max & min in any order */ 
#define OPENING	  ('o')		/* process max only */
#define CLOSING	  ('c')		/* process min only */
#define OPENCLOSE ('M')		/* process max than min at each mesh */
#define CLOSEOPEN ('N')		/* process min then max at each mesh */

/* Define output types */
#define LOWPASS   ('l')
#define BANDPASS  ('b')
#define FUSEBPASS ('f')


/******************************************************************************/

/* KEY STRUCTURES */

/* Information stored for each image pel (14 bytes/pel) */
typedef struct graph_node {
	byte 	value;		/* Pel intensity */
	fbits	adj_flags;	/* Flags to indicate adjacency */
	sieve_pointer root_pel;	/* sieve_pointer to root pel (or volume of region) */
	sieve_pointer next_pel;	/* sieve_pointer to next pel in region */
	sieve_pointer next_adj;	/* sieve_pointer to next distinct boundary pel */
	} GPH_NODE;

/* Information stored for each extremum (8 bytes/extremum = 4 bytes/pel) */
typedef struct extrema_node {
	sieve_pointer root_pel;	/* sieve_pointer to root pel */
	sieve_pointer next_ext;	/* sieve_pointer to next extremum in list */
	} EXT_NODE;

/* Coordinates of a pel */
typedef struct pel {
	short int m, n, d;
	} PEL;


/******************************************************************************/
/* GATEWAY FUNCTION FOR VOLVIEWER = JEROME */
void sieve_gateway(VolumeObject* volobj, int src, int dst, int region_size);

/* HELPER FUNCTIONS FOR VOLVIEWER */
void copy_to_singlechanbuffer(byte *I, byte* S, int Xdim, int Ydim, int Zdim, int offset);
void copy_tomultichannelbuffer(byte* I, GPH_NODE* Gbase, int Xdim, int Ydim, int Zdim, int offset);

/******************************************************************************/
/* FUNCTION PROTOTYPES */

void parse_extrema_list(GPH_NODE *Gbase, EXT_NODE *Min_base, EXT_NODE *Max_base,
      		     int64_proxy *Min_min_volm, int64_proxy *Max_min_volm, int64_proxy volm, int64_proxy filter,
				   int64_proxy *offsets, int64_proxy M, int64_proxy N, int64_proxy D, int64_proxy Dres);
int64_proxy process_extremum(GPH_NODE *Gbase, GPH_NODE **pGptr, int64_proxy *offsets,
						 int64_proxy M, int64_proxy N, int64_proxy D, int64_proxy Dres);
void find_connected_sets(GPH_NODE *Gbase, byte *X, int64_proxy M, int64_proxy N, int64_proxy D,
							    int64_proxy Dres, int64_proxy ndir);
void find_adjacency_and_extrema(GPH_NODE *Gbase, EXT_NODE *Min_base, 
		       EXT_NODE *Max_base, int64_proxy *Min_min_volm, int64_proxy *Max_min_volm,
						 int64_proxy M, int64_proxy N, int64_proxy D, int64_proxy ndir);
int64_proxy check_lists(int64_proxy reset, sieve_pointer adj_root, int64_proxy volm);
void process_mesh1(byte *X, int64_proxy filter, int64_proxy M, int64_proxy N, int64_proxy D, int64_proxy ndir);	
void copy_to_buffer(double *I, byte *X, int64_proxy pels);
void make_lowpass_image_dbl(double *Y, GPH_NODE *Gbase, int64_proxy pels);
void make_lowpass_image_int(byte *Y, GPH_NODE *Gbase, int64_proxy pels);
void subtract_images(double *Y, byte *X1, byte *X2, int64_proxy pels);
void duplicate_image(byte *Y, byte *X, int64_proxy pels);
void fuse_extrema(double *Y, PEL *P, int64_proxy ndir, int64_proxy M, int64_proxy N, int64_proxy D);

#endif // SIEVE_H
