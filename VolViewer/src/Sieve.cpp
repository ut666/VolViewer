/*******************************************************************************

FILE
	sieve3d.c 

AUTHOR
	rwy

MODIFICATION HISTORY

	1.1	03:aug:95	rwy	First version submitted to SCCS.
	1.2	03:aug:95	rwy	Allow different resolution in 
					depth dimension.  Much faster at
					very high meshes.

DESCRIPTION

	Program to compute a fast 3D connected-set datasieve.

SCCS IDENTIFIER
	@(#)sieve3d.c	1.2 8/4/95


*******************************************************************************/
#include "Sieve.h"

/******************************************************************************/

/* GATEWAY FUNCTION */

/* Gateway function to interface with Matlab */
void sieve_gateway(VolumeObject* volobj, int src, int dst, int region_size)
{
//	return;

	double *I, *Y, max_mesh;
	byte *X, *B1, *B2, *Btmp;
	int64_proxy offsets[16], Min_min_volm, Max_min_volm, min_volm, ndir;
	int64_proxy *outputs, *filters, output, filter, out_type, num_mesh;
	int64_proxy M, N, D, k, done, Dres;
	GPH_NODE *G;
    EXT_NODE *Max, *Min;
	PEL *P;

	I = Y = NULL;
	X = B1 = B2 = Btmp = NULL;
	outputs = filters = NULL;
	G = NULL;
	Max = NULL;
	Min = NULL;
	P = NULL;

	M = volobj->texheight;
	N = volobj->texwidth;
	D = volobj->texdepth;
	Dres = 1;

	printf("M: %d\n", M);
	printf("N: %d\n", N);
	printf("D: %d\n", D);

	num_mesh = 1; //maximum meshes

    //Allocate memory for filter and output specifiers
	filters = new int64_proxy[num_mesh];//(int *)mxCalloc((unsigned int)(num_mesh), sizeof(int));
	outputs =  new int64_proxy[num_mesh];//(int *)mxCalloc((unsigned int)(num_mesh), sizeof(int));

	//Check length of filter and output specifiers

	//Get filter and output specifiers
	for (k=0; k<num_mesh; k++)
	{
		filters[k] = MEDIAN;
		outputs[k] = 1;//(k==num_mesh-1) ? 1 : 0;
	}

	//Get output type
	out_type = LOWPASS;

	//Get connectivity
	ndir = 6;

	//Allocate memory
	X = new byte[M*N*D];
	G = new GPH_NODE[M*N*D]; //(GPH_NODE *)mxCalloc((unsigned int)(M*N*D),     sizeof(GPH_NODE));
	Max = new EXT_NODE[M*N*D/4+1];//(EXT_NODE *)mxCalloc((unsigned int)(M*N*D/4+1), sizeof(EXT_NODE));
	Min = new EXT_NODE[M*N*D/4+1];//(EXT_NODE *)mxCalloc((unsigned int)(M*N*D/4+1), sizeof(EXT_NODE));

	//Allocate more buffers for bandpass outputs
	if (out_type!=LOWPASS)
	{
 	    B1 = new byte[M*N*D];//(byte *)mxCalloc((unsigned int)(M*N*D), sizeof(byte));
	    B2 = new byte[M*N*D];//(byte *)mxCalloc((unsigned int)(M*N*D), sizeof(byte));
	}
	if (out_type==FUSEBPASS)
 	    P = new PEL[M*N*D];//(PEL *)mxCalloc((unsigned int)(M*N*D), sizeof(PEL));

	//---------------------------------------------------------------
	//(NB) expecting column order as opposed to row that is given.
	//---------------------------------------------------------------
	// Set offsets in each of the possible directions
	offsets[0]=-1;   offsets[1]=+1;   offsets[2]=-M;   offsets[3]=+M;
	offsets[4]=-M*N; offsets[5]=+M*N;
	
	//Copy esinput image to byte array
	copy_to_singlechanbuffer(X, volobj->texture3d, M, N, D, src);

	//Duplicate image for bandpass outputs
	//if (out_type!='l') duplicate_image(B2, I, M*N*D);


	/**********************************************************************/

	//Process image

	//Process mesh 1
    process_mesh1(X, filters[0], M, N, D, ndir);   

	//Set up graph
	find_connected_sets(G, X, M, N, D, Dres, ndir); 

	find_adjacency_and_extrema(G, Min, Max, &Min_min_volm, 
						&Max_min_volm, M, N, D, ndir);

	//Loop over input parameters
	for (k=0; k<num_mesh; k++)
	{ 
		//Get input parameters
		max_mesh = region_size;
		filter   = filters[k];
		output   = outputs[k];

		//Apply processing with selected filter
		done = 0;
		while (!done)
		{
			//Calculate mesh to apply filtering
			if      (filter==OPENING) min_volm = Max_min_volm;
			else if (filter==CLOSING) min_volm = Min_min_volm;
			else    min_volm = (Min_min_volm<Max_min_volm) 
							? Min_min_volm : Max_min_volm;

			//printf("min_volm: %d\n", min_volm);

			//Test whether finished
			if (min_volm<=0 || min_volm>max_mesh 
							|| min_volm>=M*N*(D+(D-1)*Dres))
			{
				done = 1;
				continue;
			}

			// Apply filter at current mesh
			if (filter==OPENCLOSE)
			{
				parse_extrema_list(G, Min, Max, &Min_min_volm, 
				  &Max_min_volm, min_volm, OPENING, offsets, M, N, D, Dres);
				parse_extrema_list(G, Min, Max, &Min_min_volm, 
				  &Max_min_volm, min_volm, CLOSING, offsets, M, N, D, Dres);
			}
			else if (filter==CLOSEOPEN)
			{
				parse_extrema_list(G, Min, Max, &Min_min_volm, 
				  &Max_min_volm, min_volm, CLOSING, offsets, M, N, D, Dres);
				parse_extrema_list(G, Min, Max, &Min_min_volm, 
				  &Max_min_volm, min_volm, OPENING, offsets, M, N, D, Dres);
			}
			else
			{
				parse_extrema_list(G, Min, Max, &Min_min_volm, 
				   &Max_min_volm, min_volm, filter, offsets, M, N, D, Dres);
			}
		}
 
		if (output>0)
		{
			//Allocate matrix for output image 
			Y = new double[M*N*D];
		}

		if (output>0 && out_type==LOWPASS)
		{
			//Copy low pass image to output matrix
			//make_lowpass_image_dbl(Y, G, M*N*D);
			copy_tomultichannelbuffer(volobj->texture3d, G, M, N, D, dst);

		}
		else if (out_type!=LOWPASS)
		{
			//Get low pass image
			make_lowpass_image_int(B1, G, M*N*D);

			if (output>0)
			{
				subtract_images(Y, B1, B2, M*N*D);
				if (out_type==FUSEBPASS)
				fuse_extrema(Y, P, ndir, M, N, D);
			}
	
			//Swap buffers for next time
			Btmp = B1; B1 = B2; B2 = Btmp;
		}
	}

	if(filters!=NULL) delete[] filters;
	if(outputs!=NULL) delete[] outputs;

	if(X!=NULL) delete[] X;
	if(G!=NULL) delete[] G;
	if(Y!=NULL) delete[] Y;
	if(Max!=NULL) delete[] Max;
	if(Min!=NULL) delete[] Min;
	if(B1!=NULL) delete[] B1;
	if(B2!=NULL) delete[] B2;
	if(P!=NULL) delete[] P;
}

/******************************************************************************/

void parse_extrema_list(GPH_NODE *Gbase, EXT_NODE *Min_base, EXT_NODE *Max_base,
       		     int64_proxy *Min_min_volm, int64_proxy *Max_min_volm, int64_proxy volm, int64_proxy filter,
				    int64_proxy *offsets, int64_proxy M, int64_proxy N, int64_proxy D, int64_proxy Dres)
{
	int64_proxy new_volm;
	EXT_NODE *Min_ptr, *Max_ptr, *Min_prev_ext, *Max_prev_ext;
	GPH_NODE *Gptr;
 	sieve_pointer Min_pos, Max_pos;

	Min_prev_ext = 0;
	Max_prev_ext = 0;

	if (filter!=OPENING && *Min_min_volm==volm)
	{
	    *Min_min_volm = 0;
            Min_pos = 0;
	}
	else Min_pos = -1;

	if (filter!=CLOSING && *Max_min_volm==volm)
	{
	    *Max_min_volm = 0;
            Max_pos = 0;
	}
	else Max_pos = -1;

        /* Check whether first entry is valid */
	if (Min_pos>=0 && Min_base->root_pel < 0) 
	{
	    	Min_prev_ext = Min_base;
	    	Min_pos = Min_base->next_ext;
	}
	if (Max_pos>=0 && Max_base->root_pel < 0) 
	{
	    	Max_prev_ext = Max_base;
	    	Max_pos = Max_base->next_ext;
	}

	/* Loop through maxima and minima lists */
	while (Min_pos>=0 || Max_pos>=0)
	{
	    /* Process minima */
            if (Min_pos>=0)
	    {
		/* Test volume of extremum */
		Gptr = Gbase+(Min_ptr=Min_base+Min_pos)->root_pel;
		if ((new_volm = -(Gptr->root_pel)) <= volm)
		{
		    if (new_volm>0)
			new_volm = process_extremum(Gbase, &Gptr, 
							offsets, M, N, D, Dres);

   		    if (new_volm<=0)
		    {
			/* Delete extremum from list */
			if (Min_prev_ext) 
				Min_prev_ext->next_ext = Min_ptr->next_ext;
			else
			{
				Min_ptr->root_pel = -1; 
				Min_prev_ext = Min_ptr;
			}
		    }
		    else
		    {
			Min_prev_ext = Min_ptr;
			Min_ptr->root_pel = Gptr-Gbase;
		    }
		}
		else 
		    Min_prev_ext = Min_ptr;

		/* Update minimum volume */
		if ((new_volm<*Min_min_volm || *Min_min_volm==0) && new_volm>0)
			*Min_min_volm = new_volm;

   	    	/* Goto next extremum */
	    	Min_pos = Min_ptr->next_ext;
	    }

	    /* Process maxima */
            if (Max_pos>=0)
	    {
		/* Test volume of extremum */
		Gptr = Gbase+(Max_ptr=Max_base+Max_pos)->root_pel;
		if ((new_volm = -(Gptr->root_pel)) <= volm)
		{
		    if (new_volm>0)
			new_volm = process_extremum(Gbase, &Gptr, 
							offsets, M, N, D, Dres);

   		    if (new_volm<=0)
		    {
			/* Delete extremum from list */
			if (Max_prev_ext) 
				Max_prev_ext->next_ext = Max_ptr->next_ext;
			else
			{
				Max_ptr->root_pel = -1; 
				Max_prev_ext = Max_ptr;
			}
		    }
		    else
		    {
			Max_prev_ext = Max_ptr;
			Max_ptr->root_pel = Gptr-Gbase;
		    }
		}
		else 
		    Max_prev_ext = Max_ptr;

		/* Update minimum volume */
		if ((new_volm<*Max_min_volm || *Max_min_volm==0) && new_volm>0)
			*Max_min_volm = new_volm;

   	    	/* Goto next extremum */
	    	Max_pos = Max_ptr->next_ext;
	    }
	}
}


/******************************************************************************/

/* PROCESS AN EXTREMUM */

int64_proxy process_extremum(GPH_NODE *Gbase, GPH_NODE **pGptr, 
				    int64_proxy *offsets, int64_proxy M, int64_proxy N, int64_proxy D, int64_proxy Dres)
{
	#define LT_INIT -1
	#define GT_INIT 256
	#define MAX_EQ  10000

	int64_proxy j, bit, num_eq, lt_num_eq, gt_num_eq, lt_val, gt_val;
	int64_proxy ext_val, adj_val, *offs, MN, pels, volm;
	sieve_pointer pos, new_root_pel, adj_root, test_root; 
	sieve_pointer lt_root[MAX_EQ], gt_root[MAX_EQ];
	fbits flags, dirns;
	GPH_NODE *pel_ptr, *adj_ptr, *adj_root_ptr, *last_pel_ptr, *ptr, *Gptr;


	/**********************************************************************/

	/* Find adjacent region with closest value */

	/* Prepare to find closest value adjacent region */
	Gptr = *pGptr;
	ext_val = Gptr->value;
	volm = -(Gptr->root_pel);
	pos = Gptr-Gbase;
        lt_val = LT_INIT; 
	gt_val = GT_INIT;
        pel_ptr = 0;
	last_pel_ptr = 0;
	check_lists(1,0,volm);
	MN   = M*N;
	pels = M*N*D;
	lt_num_eq = 0;
	gt_num_eq = 0;

	/* Add current region to list so duplicates are deleted */
	check_lists(0,Gptr-Gbase,volm);

	/* Loop over the distinct boundary pels */
	while (pos>=0)
	{ 
		pel_ptr = Gbase+pos;
		dirns = flags = pel_ptr->adj_flags;
		offs = offsets;
		bit = 1;

		/* Loop over possible adjacent directions */
		while (dirns)
		{
		    if (dirns & 1)
		    {
			/* Get value of adjacent region */
		        adj_ptr = pel_ptr+*offs;
  		        adj_root = adj_ptr->root_pel;
			adj_root = (adj_root>=0) ? adj_root : adj_ptr-Gbase;

			/* Check whether this is a duplicate */
			if (!check_lists(0, adj_root, volm))
			    flags ^= bit;
			else
			{
			    /* Record closest values above and below extrema */
			    adj_val = (adj_root+Gbase)->value;
			    if (adj_val<=ext_val)
			    {
			        if (adj_val>lt_val) 
			        {
			            lt_val = adj_val;      
			            lt_root[0] = adj_root; 
				    lt_num_eq = 1;
			        }
			        else if (adj_val==lt_val && lt_num_eq<MAX_EQ)
			            lt_root[lt_num_eq++] = adj_root; 
			    }
			    else 
			    {
			        if (adj_val<gt_val) 
			        {
			            gt_val = adj_val;      
			            gt_root[0] = adj_root; 
				    gt_num_eq = 1;
			        }
			        else if (adj_val==gt_val && gt_num_eq<MAX_EQ)   
			            gt_root[gt_num_eq++] = adj_root; 
			    }
			}
		    }
		    bit<<=1;
		    dirns>>=1;
		    offs++;
		}

		/* Update flags, deleting entry if necessary */
		if (!flags && last_pel_ptr)
		 	last_pel_ptr->next_adj = pel_ptr->next_adj;
		else
		{
			pel_ptr->adj_flags = flags;
			last_pel_ptr = pel_ptr;
		}	

		/* Goto next boundary pel */
		pos = pel_ptr->next_adj;
	}
	
	/* Return if region is not an extremum */
	if (lt_val!=LT_INIT && gt_val!=GT_INIT && lt_val!=ext_val) return (0);


	/**********************************************************************/

	/* Merge pel lists for extremum and adjacent regions */

        new_root_pel = Gptr-Gbase;
	if (lt_val!=LT_INIT) 
	{
		if (!(num_eq = lt_num_eq)) return(0);
		Gptr->value = (byte)lt_val;
	}
	else
	{
		if (!(num_eq = gt_num_eq)) return(0);
		Gptr->value = (byte)gt_val;
	}

	/* Loop over adjacent regions to merge */
	for (j=0; j<num_eq; j++)
	{
	    /* Get information for closest adjacent region */
	    adj_root = (lt_val!=LT_INIT) ? lt_root[j] : gt_root[j];
	    adj_root_ptr = adj_root+Gbase;

	    if (adj_root_ptr->root_pel >= Gptr->root_pel)
	    {	
	        /* Update volm */
	        Gptr->root_pel += adj_root_ptr->root_pel;

		/* Account for depth resolution */
		if (Dres)
		{
		    volm = Gptr->root_pel;
	            pos  = adj_root_ptr-Gbase;
	            while (pos>=0) 
		    {
			ptr = Gbase+pos;
		        if (pos-MN>=0   && (ptr-MN)->root_pel==new_root_pel)
			    volm-=Dres; 
		        if (pos+MN<pels && (ptr+MN)->root_pel==new_root_pel)
			    volm-=Dres; 
		        pos = ptr->next_pel;
		    }
	            if (new_root_pel-MN>=0)
		    {
			test_root = (Gptr-MN)->root_pel;
			test_root = (test_root>=0) ? test_root:new_root_pel-MN;
			if (test_root==adj_root) volm-=Dres; 
 		    }
	            if (new_root_pel+MN<pels)
		    {
			test_root = (Gptr+MN)->root_pel;
			test_root = (test_root>=0) ? test_root:new_root_pel+MN;
			if (test_root==adj_root) volm-=Dres; 
 		    }
		    Gptr->root_pel = volm;
		}

	        /* Modify root pels of adjacent region */ 
	        (ptr=adj_root_ptr)->root_pel = new_root_pel;
	        while (ptr->next_pel>=0) 
	        {
		    ptr = Gbase+ptr->next_pel;
		    ptr->root_pel = new_root_pel;
	        }

	        /* Insert adjacent region after root pel of extrema */
	        ptr->next_pel = Gptr->next_pel;
                Gptr->next_pel = adj_root_ptr-Gbase;

	        /* Add adjacent boundary pels after extremum boundary pels */
	        last_pel_ptr->next_adj = adj_root;
	        if (j!=num_eq-1)
	        {
		    last_pel_ptr = Gbase+last_pel_ptr->next_adj;
		    while (last_pel_ptr->next_adj>=0)
	                last_pel_ptr = Gbase+last_pel_ptr->next_adj;
	        }
	    }
	    else
	    {	
	        /* Update volm */
	        adj_root_ptr->root_pel += Gptr->root_pel;

		/* Account for depth resolution */
		if (Dres)
		{
		    volm = adj_root_ptr->root_pel;
	            pos  = Gptr-Gbase;
	            while (pos>=0) 
		    {
			ptr = Gbase+pos;
		        if (pos-MN>=0   && (ptr-MN)->root_pel==adj_root)
			    volm-=Dres; 
		        if (pos+MN<pels && (ptr+MN)->root_pel==adj_root)
			    volm-=Dres; 
		        pos = ptr->next_pel;
		    }
	            if (adj_root-MN>=0)
		    {
			test_root = (adj_root_ptr-MN)->root_pel;
			test_root = (test_root>=0) ? test_root : adj_root-MN;
			if (test_root==new_root_pel) volm-=Dres; 
 		    }
	            if (adj_root+MN<pels)
		    {
			test_root = (adj_root_ptr+MN)->root_pel;
			test_root = (test_root>=0) ? test_root : adj_root+MN;
			if (test_root==new_root_pel) volm-=Dres; 
 		    }
		    adj_root_ptr->root_pel = volm;
		}

	        /* Modify root pels of extremum */ 
	        (ptr=Gptr)->root_pel = adj_root;
	        while (ptr->next_pel>=0) 
	        {
		    ptr = Gbase+ptr->next_pel;
		    ptr->root_pel = adj_root;
	        }

	        /* Insert extremum after root pel of adjacent region */
	        ptr->next_pel = adj_root_ptr->next_pel;
                adj_root_ptr->next_pel = new_root_pel;

	        /* Add extremum boundary pels */
		if (j!=num_eq-1)
		{
		    /* Add extremum boundary pels after adjacent boundary pels*/
		    ptr = adj_root_ptr;
		    while (ptr->next_adj>=0)
	                ptr = Gbase+ptr->next_adj;
	            ptr->next_adj = new_root_pel;
		}
		else
		{
		    /* Insert extremum boundary pels after adjacent root pel */
		    last_pel_ptr->next_adj = adj_root_ptr->next_adj;
		    adj_root_ptr->next_adj = new_root_pel;
		}

		/* Make adjacent region the extremum */
		Gptr = adj_root_ptr;
		new_root_pel = adj_root;
	    }
	}
	*pGptr = Gptr;
	//if (Gptr->root_pel>=0) printf("New volm is negative\n");
	return (-(Gptr->root_pel));
}


/******************************************************************************/

/* FIND CONNECTED-SETS */

void find_connected_sets(GPH_NODE *Gbase, byte *Xbase, int64_proxy M, int64_proxy N, 
						      int64_proxy D, int64_proxy Dres, int64_proxy ndir)
{
	int64_proxy m, n, d, m0, n0, d0, val, dir[8], volm, MN;
	fbits last_dirn, other_regions;
	sieve_pointer pos, next_pel;
	GPH_NODE *Gptr, *pel_ptr, *last_pel_ptr;
	byte *X, *Xptr;

	/* Define allowed directions given previous move */
	if (ndir==6)
	{
		dir[0] = (1+4+8+16+32);
		dir[1] = (2+4+8+16+32);
		dir[2] = (1+2+4+16+32);
		dir[3] = (1+2+8+16+32);
		dir[4] = (1+2+4+8+16);
		dir[5] = (1+2+4+8+32);
	}

	Gptr = Gbase;
	X = Xptr = Xbase;
	pos = 0;
	MN  = M*N;

	for (d=0; d<D; d++)
	{
		for (n=0; n<N; n++)
		{
			for (m=0; m<M; m++, pos++, X++, Gptr++)
			{
				/* Test whether already used */
				if (!Gptr->next_adj)
				{
					/* Add root pel */
					volm = 1;
					Gptr->next_adj = 1;
					other_regions  = 0;
					last_pel_ptr   = Gptr;
					val = Gptr->value = *X;

					/* Try 3 valid directions to move from root pel */
					if (m!=M-1)
					{
						if (val==*(X+1)) 
						{
							last_pel_ptr->next_pel = pos+1;
							last_pel_ptr = Gptr+1;
							last_pel_ptr->adj_flags = 2;
							last_pel_ptr->next_adj = ((d)<<23|(m+1)<<12)|(n+1);
						}
						else other_regions |= 2;
					}
					if (n!=N-1)
					{
						if (val==*(X+M)) 
						{
							last_pel_ptr->next_pel = pos+M;
							last_pel_ptr = Gptr+M;
							last_pel_ptr->adj_flags = 8;
							last_pel_ptr->next_adj = ((d)<<23|(m)<<12)|(n+1+1);
						}
						else other_regions |= 8;
					}
					if (d!=D-1)
					{
						if (val==*(X+MN)) 
						{
							last_pel_ptr->next_pel = pos+MN;
							last_pel_ptr = Gptr+MN;
							last_pel_ptr->adj_flags = 32;
							last_pel_ptr->next_adj = ((d+1)<<23|(m)<<12)|(n+1);
							volm+=Dres;
						}
						else other_regions |= 32;
					}

					/* Check other directions for other regions */
					if (m!=0 && val!=*(X-1))   other_regions |= 1;
					if (n!=0 && val!=*(X-M))   other_regions |= 4;
					if (d!=0 && val!=*(X-MN))  other_regions |= 16;

					/* Add end of list marker */
					last_pel_ptr->next_pel = -1;

					/* Store directions to other regions */
					Gptr->adj_flags = other_regions;

					/* Loop over all pels added to region */
					next_pel = Gptr->next_pel;
					while (next_pel >= 0)
					{
						pel_ptr = Gbase + next_pel;
						Xptr = Xbase + next_pel;
						volm++;

						/* Set root pel */
						pel_ptr->root_pel = pos;

						/* Get previous direction */
						last_dirn = pel_ptr->adj_flags;
						m0 = (pel_ptr->next_adj >> 12) & 2047;
						n0 = (pel_ptr->next_adj & 4095) - 1;
						d0 = pel_ptr->next_adj >> 23;
						//if (d0<0)
						//	printf("d0<0\n");
						other_regions = 0;

						/* Consider all possible directions */
						if ((last_dirn & dir[0]) && m0!=0)
						{
							if (val==*(Xptr-1)) 
							{
								if(!(pel_ptr-1)->next_adj) 
								{
									last_pel_ptr->next_pel = next_pel-1;
									last_pel_ptr = pel_ptr-1;
									last_pel_ptr->adj_flags = 1;
									last_pel_ptr->next_adj = 			
										(d0<<23|(m0-1)<<12)|(n0+1);
								}
							}
							else other_regions |= 1;
						}
						if ((last_dirn & dir[1]) && m0!=M-1)
						{
							if (val==*(Xptr+1)) 
							{
								if(!(pel_ptr+1)->next_adj) 
								{
									last_pel_ptr->next_pel = next_pel+1;
									last_pel_ptr = pel_ptr+1;
									last_pel_ptr->adj_flags = 2;
									last_pel_ptr->next_adj =
										(d0<<23|(m0+1)<<12)|(n0+1);
								}
							}
							else other_regions |= 2;
						}
						if ((last_dirn & dir[2]) && n0!=0)
						{
							if (val==*(Xptr-M)) 
							{
								if(!(pel_ptr-M)->next_adj) 
								{
									last_pel_ptr->next_pel = next_pel-M;
									last_pel_ptr = pel_ptr-M;
									last_pel_ptr->adj_flags = 4;
									last_pel_ptr->next_adj =
										(d0<<23|(m0)<<12)|(n0-1+1);
								}
							}
							else other_regions |= 4;
						}
						if ((last_dirn & dir[3]) && n0!=N-1)
						{
							if (val==*(Xptr+M)) 
							{
								if(!(pel_ptr+M)->next_adj) 
								{
									last_pel_ptr->next_pel = next_pel+M;
									last_pel_ptr = pel_ptr+M;
									last_pel_ptr->adj_flags = 8;
									last_pel_ptr->next_adj =
										(d0<<23|(m0)<<12)|(n0+1+1);
								}
							} 
							else other_regions |= 8;
						}
						if ((last_dirn & dir[4]) && d0!=0)
						{
							if (val==*(Xptr-MN)) 
							{
								if(!(pel_ptr-MN)->next_adj) 
								{
									last_pel_ptr->next_pel = next_pel-MN;
									last_pel_ptr = pel_ptr-MN;
									last_pel_ptr->adj_flags = 16;
									last_pel_ptr->next_adj =
										((d0-1)<<23|(m0)<<12)|(n0+1);
								}
							}
							else other_regions |= 16;
						}
						if (d0!=D-1)
						{
							if (val==*(Xptr+MN)) 
							{
								if(!(pel_ptr+MN)->next_adj) 
								{
									last_pel_ptr->next_pel = next_pel+MN;
									last_pel_ptr = pel_ptr+MN;
									last_pel_ptr->adj_flags = 32;
									last_pel_ptr->next_adj =
										((d0+1)<<23|(m0)<<12)|(n0+1);
								}
								volm+=Dres;
							}
							else other_regions |= 32;
						}

						/* Add end of list marker */
						last_pel_ptr->next_pel = -1;

						/* Store directions of other regions */
						pel_ptr->adj_flags = other_regions;

						/* Goto next pel */
						next_pel = pel_ptr->next_pel;
					}

					/* Store region volume */
					Gptr->root_pel = (sieve_pointer)(-volm);
					/*printf("Region vol = %d\n",volm);*/
				}
			}
		}
	}
}


/******************************************************************************/

/* FIND DISTINCT BOUNDARY PELS FOR EACH CONNECTED-SET */

void find_adjacency_and_extrema(GPH_NODE *Gbase, EXT_NODE *Min_base, 
		      EXT_NODE *Max_base, int64_proxy *Min_min_volm, int64_proxy *Max_min_volm, 
						  int64_proxy M, int64_proxy N, int64_proxy D, int64_proxy ndir)
{
	int64_proxy num_min, num_max, val, sign, volm, MN;
	fbits dirns, flags;
	GPH_NODE *Gptr, *pel_ptr, *adj_ptr, *prev_adj_ptr;
	EXT_NODE *Min_ptr, *Max_ptr;
	sieve_pointer adj_root, pos;

        Gptr = Gbase; 
	Min_ptr = Min_base;
	Max_ptr = Max_base;
        num_min = 0;
        num_max = 0;
	*Min_min_volm = 0;	
	*Max_min_volm = 0;
	MN = M*N;

	for (pos=0; pos<M*N*D; pos++, Gptr++)
	{
	    /* Test if current node is root node */
	    if (Gptr->root_pel<0)
	    {

		/* Trap special case of single pel region */
		if (Gptr->next_pel<0)
		{
		    Gptr->root_pel = -1;
		    Gptr->next_adj = -1;
		}
		else
		{
		    sign = 0;
		    volm = -(Gptr->root_pel);
		    check_lists(1, 0, volm);

		    /* Loop over all pels in connected-set */
		    pel_ptr = Gptr;
		    prev_adj_ptr = 0;
	            val  = Gptr->value;
		    volm = -(Gptr->root_pel);
		    do
		    {
		        /* Find new regions adjacent to current pel */
		        dirns = pel_ptr->adj_flags;
		        flags = 0;

		        if (dirns & 1) 
			{
			    adj_ptr  = pel_ptr-1;
			    adj_root = (adj_ptr->root_pel>=0) ? 
					       adj_ptr->root_pel:adj_ptr-Gbase;
			    if (check_lists(0, adj_root, volm))
		    	    {
			        flags |= 1;
				sign |= ((int64_proxy)(Gbase+adj_root)->value>val)+1;
			    }
			}

		        if (dirns & 2) 
			{
			    adj_ptr  = pel_ptr+1;
			    adj_root = (adj_ptr->root_pel>=0) ? 
					       adj_ptr->root_pel:adj_ptr-Gbase;
			    if (check_lists(0, adj_root, volm))
		    	    {
			        flags |= 2;
				sign |= ((int64_proxy)(Gbase+adj_root)->value>val)+1;
			    }
			}

		        if (dirns & 4) 
			{
			    adj_ptr  = pel_ptr-M;
			    adj_root = (adj_ptr->root_pel>=0) ? 
					       adj_ptr->root_pel:adj_ptr-Gbase;
			    if (check_lists(0, adj_root, volm))
		    	    {
			        flags |= 4;
				sign |= ((int64_proxy)(Gbase+adj_root)->value>val)+1;
			    }
			}

		        if (dirns & 8) 
			{
			    adj_ptr  = pel_ptr+M;
			    adj_root = (adj_ptr->root_pel>=0) ? 
					       adj_ptr->root_pel:adj_ptr-Gbase;
			    if (check_lists(0, adj_root, volm))
		    	    {
			        flags |= 8;
				sign |= ((int64_proxy)(Gbase+adj_root)->value>val)+1;
			    }
			}

		        if (dirns & 16) 
			{
			    adj_ptr  = pel_ptr-MN;
			    adj_root = (adj_ptr->root_pel>=0) ? 
					       adj_ptr->root_pel:adj_ptr-Gbase;
			    if (check_lists(0, adj_root, volm))
		    	    {
			        flags |= 16;
				sign |= ((int64_proxy)(Gbase+adj_root)->value>val)+1;
			    }
			}

		        if (dirns & 32) 
			{
			    adj_ptr  = pel_ptr+MN;
			    adj_root = (adj_ptr->root_pel>=0) ? 
					       adj_ptr->root_pel:adj_ptr-Gbase;
			    if (check_lists(0, adj_root, volm))
		    	    {
			        flags |= 32;
				sign |= ((int64_proxy)(Gbase+adj_root)->value>val)+1;
			    }
			}

		        /* Set node data */
		        if (flags && prev_adj_ptr) 
			    prev_adj_ptr->next_adj = pel_ptr-Gbase;  
		        if (flags || prev_adj_ptr==0)
		            prev_adj_ptr = pel_ptr;
		        pel_ptr->adj_flags = flags;

		        /* Goto next pel in connected-set */
		        pel_ptr = Gbase+pel_ptr->next_pel;
		    }
		    while (pel_ptr>=Gbase);

		    /* Store end of list marker */
		    prev_adj_ptr->next_adj = -1;

		    /* Determine whether extremum */
		    if (sign==2)
		    {
		        Min_ptr->root_pel = (Gptr-Gbase);
		        (Min_ptr++)->next_ext = ++num_min;		
		        if (volm<*Min_min_volm || *Min_min_volm==0) 
				*Min_min_volm = volm;
		    }
		    else if (sign==1)
		    {
		        Max_ptr->root_pel = (Gptr-Gbase);
		        (Max_ptr++)->next_ext = ++num_max;		
		        if (volm<*Max_min_volm || *Max_min_volm==0) 
				*Max_min_volm = volm;
		    }
		}	
	    }
	}

	/* Add end of list marker */
	if (num_min) (Min_ptr-1)->next_ext = -1;
	if (num_max) (Max_ptr-1)->next_ext = -1;
}


/******************************************************************************/

/* STORE ADJACENT REGIONS IN HASH TABLE */

int64_proxy check_lists(int64_proxy reset, sieve_pointer root, int64_proxy volm)
{
	#define VOLM_SWITCH  50 
	#define LIST_LENGTH  50000 //5000 increased for 64 bits

	static sieve_pointer lists[64][LIST_LENGTH];
	static int64_proxy lists_len[64];
	sieve_pointer *list;
	int64_proxy k, hash, *len;

	if (volm<VOLM_SWITCH)
	{
	    if (!reset)
	    {
		hash = ((root & 768)>>6) | (root & 3);  
		list = lists[hash];
		for (k=0; k<lists_len[hash]; k++)
			if (*(list++) == root) return (0);
		*list = root;
	        lists_len[hash]++;
  	        return (1);
	    }
	    else
	    {
		len = lists_len;
		*(len++) = 0; *(len++) = 0; *(len++) = 0; *(len++) = 0; 
		*(len++) = 0; *(len++) = 0; *(len++) = 0; *(len++) = 0;		
		*(len++) = 0; *(len++) = 0; *(len++) = 0; *(len++) = 0;		
		*(len++) = 0; *(len++) = 0; *(len++) = 0; *(len++) = 0;
		return (0);
	    }
	}
	else
	{
	    if (!reset)
	    {
		hash = ((root & 768)>>4) | (root & 15);  
		list = lists[hash];
		for (k=0; k<lists_len[hash]; k++)
			if (*(list++) == root) return (0);
		*list = root;
	        lists_len[hash]++;
  	        return (1);
            }
	    else
	    {
		len = lists_len;
		*(len++) = 0; *(len++) = 0; *(len++) = 0; *(len++) = 0; 
		*(len++) = 0; *(len++) = 0; *(len++) = 0; *(len++) = 0;		
		*(len++) = 0; *(len++) = 0; *(len++) = 0; *(len++) = 0;		
		*(len++) = 0; *(len++) = 0; *(len++) = 0; *(len++) = 0;
		*(len++) = 0; *(len++) = 0; *(len++) = 0; *(len++) = 0; 
		*(len++) = 0; *(len++) = 0; *(len++) = 0; *(len++) = 0;		
		*(len++) = 0; *(len++) = 0; *(len++) = 0; *(len++) = 0;		
		*(len++) = 0; *(len++) = 0; *(len++) = 0; *(len++) = 0;
		*(len++) = 0; *(len++) = 0; *(len++) = 0; *(len++) = 0; 
		*(len++) = 0; *(len++) = 0; *(len++) = 0; *(len++) = 0;		
		*(len++) = 0; *(len++) = 0; *(len++) = 0; *(len++) = 0;		
		*(len++) = 0; *(len++) = 0; *(len++) = 0; *(len++) = 0;
		*(len++) = 0; *(len++) = 0; *(len++) = 0; *(len++) = 0; 
		*(len++) = 0; *(len++) = 0; *(len++) = 0; *(len++) = 0;		
		*(len++) = 0; *(len++) = 0; *(len++) = 0; *(len++) = 0;		
		*(len++) = 0; *(len++) = 0; *(len++) = 0; *(len++) = 0;
		return (0);
	    }
	}
}


/******************************************************************************/

/* PROCESS RAW IMAGE AT MESH 1 */

/* Process mesh 1 */
/* Note that at mesh 1 the median, rec_median, M & N sieves are identical */
void process_mesh1(byte *X, int64_proxy filter, int64_proxy M, int64_proxy N, int64_proxy D, int64_proxy ndir)	
{
	int64_proxy m, n, d, MN;
	byte val;

	MN = M*N;

	for (d=0; d<D; d++)
	  for (n=0; n<N; n++)
	    for (m=0; m<M; m++, X++)
	    {
		val = *X;

		/* Test whether edge pel */
		if (m==0 || m==M-1 || n==0 || n==N-1 || d==0 || d==D-1)
		{
			/* Test if maximum */
			if (filter!=CLOSING &&
			    (m==0 || *(X-1)<val)  && (m==M-1 || *(X+1)<val) &&
			    (n==0 || *(X-M)<val)  && (n==N-1 || *(X+M)<val) &&
			    (d==0 || *(X-MN)<val) && (d==D-1 || *(X+MN)<val))
			{
			    /* Find maximum adjacent pel */
			    val = m!=0 ? *(X-1) : 0;
			    if (m!=M-1 && *(X+1)>val)  val=*(X+1);
			    if (n!=0   && *(X-M)>val)  val=*(X-M);
			    if (n!=N-1 && *(X+M)>val)  val=*(X+M);
			    if (d!=0   && *(X-MN)>val) val=*(X-MN);
			    if (d!=D-1 && *(X+MN)>val) val=*(X+MN);
				
			    /* Substitute new value */
			    *X = val;
			}
			else

			/* Test if minimum */
			if (filter!=OPENING &&
			    (m==0 || *(X-1)>val)  && (m==M-1 || *(X+1)>val) &&
			    (n==0 || *(X-M)>val)  && (n==N-1 || *(X+M)>val) &&
			    (d==0 || *(X-MN)>val) && (d==D-1 || *(X+MN)>val))
			{
			    /* Find minimum adjacent pel */
			    val = m!=0 ? *(X-1) : 255;
			    if (m!=M-1 && *(X+1)<val)  val=*(X+1);
			    if (n!=0   && *(X-M)<val)  val=*(X-M);
			    if (n!=N-1 && *(X+M)<val)  val=*(X+M);
			    if (d!=0   && *(X-MN)<val) val=*(X-MN);
			    if (d!=D-1 && *(X+MN)<val) val=*(X+MN);
				
			    /* Substitute new value */
			    *X = val;
			}

		}
		else
		{
			/* Test if maximum */
			if (filter!=CLOSING &&
			    *(X-1)<val && *(X+1)<val  && *(X-M)<val &&
			    *(X+M)<val && *(X-MN)<val && *(X+MN)<val)
			{
			    /* Find maximum adjacent pel */
			    val = *(X-1);
			    if (*(X+1)>val)  val=*(X+1);
			    if (*(X-M)>val)  val=*(X-M);
			    if (*(X+M)>val)  val=*(X+M);
			    if (*(X-MN)>val) val=*(X-MN);
			    if (*(X+MN)>val) val=*(X+MN);
				
			    /* Substitute new value */
			    *X = val;
			}
			else

			/* Test if minimum */
			if (filter!=OPENING &&
			    *(X-1)>val && *(X+1)>val  && *(X-M)>val &&
			    *(X+M)>val && *(X-MN)>val && *(X+MN)>val)
			{
			    /* Find minimum adjacent pel */
			    val = *(X-1);
			    if (*(X+1)<val)  val=*(X+1);
			    if (*(X-M)<val)  val=*(X-M);
			    if (*(X+M)<val)  val=*(X+M);
			    if (*(X-MN)<val) val=*(X-MN);
			    if (*(X+MN)<val) val=*(X+MN);
				
			    /* Substitute new value */
			    *X = val;
			}
		}
	    }
}


/******************************************************************************/

/* COPY INPUT IMAGE TO BYTE ARRAY */

void copy_to_buffer(double *I, byte *X, int64_proxy pels)	
{
	//printf("1288 got here\n");
    int64_proxy k;
	for (k=0; k<pels; k++)
		*(X++) = (byte)*(I++);
    //printf("1292 got here\n");
}


/******************************************************************************/

/* RESYNTHESIZE OUTPUT IMAGE */

void make_lowpass_image_dbl(double *Y, GPH_NODE *Gbase, int64_proxy pels)
{
	int64_proxy k;
        GPH_NODE *Gptr;

	Gptr = Gbase;

	/* Loop over all pels */
	for (k=0; k<pels; k++, Gptr++, Y++)
		*Y = (Gptr->root_pel < 0) ?
		    (double)Gptr->value : (double)(Gbase+Gptr->root_pel)->value;
}

void make_lowpass_image_int(byte *Y, GPH_NODE *Gbase, int64_proxy pels)
{
	int64_proxy k;
        GPH_NODE *Gptr;

	Gptr = Gbase;

	/* Loop over all pels */
	for (k=0; k<pels; k++, Gptr++, Y++)
		*Y = (Gptr->root_pel < 0) ?
		                    Gptr->value : (Gbase+Gptr->root_pel)->value;
}

	/******************************************************************************/

/* SUBTRACT TWO IMAGES */
					
void subtract_images(double *Y, byte *X1, byte *X2, int64_proxy pels)
{
	int64_proxy k;
	
	for (k=0; k<pels; k++)
		*(Y++) = (double)((int)*(X2++)-(int)*(X1++));
}


/******************************************************************************/

/* DUPLICATE IMAGE */
					
void duplicate_image(byte *Y, byte *X, int64_proxy pels)
{
	int64_proxy k;
	
	for (k=0; k<pels; k++)
		*(Y++) = (byte)*(X++);
}


/******************************************************************************/

/* FUSE GRANULES IN BANDPASS IMAGE */

void fuse_extrema(double *Y, PEL *P, int64_proxy ndir, int64_proxy M, int64_proxy N, int64_proxy D)
{
	int64_proxy m, n, d, k, MN, off_m[8], off_n[8], off_d[8];
	int64_proxy curr_m, curr_n, curr_d, new_m, new_n, new_d;
	double val, ext_val;
	sieve_pointer curr, last, pos, new_pos;
	PEL *p;

	MN = M*N;

	/* Set up offsets */
	off_m[0] = -1; off_m[1] = +1; off_m[2] =  0; off_m[3] =  0;
	off_m[4] =  0; off_m[5] =  0;  
	off_n[0] =  0; off_n[1] =  0; off_n[2] = -1; off_n[3] = +1;
	off_n[4] =  0; off_n[5] =  0;  
	off_d[0] =  0; off_d[1] =  0; off_d[2] =  0; off_d[3] =  0;
	off_d[4] = -1; off_d[5] = +1;  

	/* Reset sieve_pointers */
	for (p=P, k=0; k<M*N*D; k++) (p++)->m = -1;

	/* Loop over pels in image */
	for (pos=0, d=0; d<D; d++)
	  for (n=0; n<N; n++)
	    for (m=0; m<M; m++, pos++)
	    {
		/* Test for a new region */
		if ((ext_val=Y[pos])!=0 && P[pos].m<0)
		{
		    /* Set up sieve_pointers */
		    curr = last = pos;
		    curr_m = m;
		    curr_n = n;
		    curr_d = d;

		    /* Find all pels of same sign in region */
		    while (curr_m>=0)
		    {
			if (ext_val>0)
			{
			    for (k=0; k<ndir; k++)
		 	    {
				new_m = curr_m + off_m[k];
				new_n = curr_n + off_n[k];
				new_d = curr_d + off_d[k];

				if (new_m>=0 && new_m<M && new_n>=0 && new_n<N
							&& new_d>=0 && new_d<D)
				{
			            new_pos = new_d*MN+new_n*M+new_m;
			            val = Y[new_pos];
  			            if (val>0 && P[new_pos].m<0 &&new_pos!=last)
			            {
				        P[last].m = new_m;
					P[last].n = new_n;
					P[last].d = new_d;
				        last = new_pos;
				        if (val>ext_val) ext_val = val;
			            }
				}
			    }
			}
			else
			{
			    for (k=0; k<ndir; k++)
		 	    {
				new_m = curr_m + off_m[k];
				new_n = curr_n + off_n[k];
				new_d = curr_d + off_d[k];

				if (new_m>=0 && new_m<M && new_n>=0 && new_n<N
							&& new_d>=0 && new_d<D)
				{
			            new_pos = new_d*MN+new_n*M+new_m;
			            val = Y[new_pos];
  			            if (val<0 && P[new_pos].m<0 &&new_pos!=last)
			            {
				        P[last].m = new_m;
					P[last].n = new_n;
					P[last].d = new_d;
				        last = new_pos;
				        if (val<ext_val) ext_val = val;
			            }
				}
			    }
			}

			/* Move onto next pel */
			curr_m = P[curr].m;
			curr_n = P[curr].n;
			curr_d = P[curr].d;
			curr = curr_d*MN+curr_n*M+curr_m;
		    }

		    /* Set all pels to the extreme value */
		    curr_m = m; curr_n = n; curr_d = d;
		    while (curr_m>=0)
		    {
			new_pos = curr_d*MN+curr_n*M+curr_m;
		        Y[new_pos] = ext_val;
			curr_m = P[new_pos].m;
			curr_n = P[new_pos].n;
			curr_d = P[new_pos].d;
		    }

		    P[last].m = 0;

		}
	    }
}

void copy_to_singlechanbuffer(byte *I, byte* S, int Xdim, int Ydim, int Zdim, int offset)
{
	/*S+=offset;
	for(int i=0; i<Xdim*Ydim*Zdim; i++)
	{
		*(I++) = *(S+=3);
	}*/	

	for(int i=0; i<Xdim*Ydim*Zdim; i++)
	{
		byte val;
		if(offset==0)
		{
			val = *(S++);
			*(S++);
			*(S++);
		}
		else if(offset==1)
		{
			*(S++);
			val = *(S++);
			*(S++);
		}
		else if(offset==2)
		{
			*(S++);
			*(S++);
			val = *(S++);
		}

		*(I++) = val;
	}

}
void copy_tomultichannelbuffer(byte* I, GPH_NODE* Gbase, int Xdim, int Ydim, int Zdim, int offset)
{
	/*GPH_NODE *Gptr;
	Gptr = Gbase;

	I+=offset;
	for(int i=0; i<Xdim*Ydim*Zdim; i++)
	{
		byte val = (Gptr->root_pel < 0) ? Gptr->value : (Gbase+Gptr->root_pel)->value;
		if(val<0) val = 0;

		*(I+=3) = val;
	}*/

	GPH_NODE *Gptr;
	Gptr = Gbase;
	
	// Loop over all pels
	for(int i=0; i<Xdim*Ydim*Zdim; i++, Gptr++, I++)
	{
		byte val = (Gptr->root_pel < 0) ? Gptr->value : (Gbase+Gptr->root_pel)->value;
		
		if(val<0) val = 0;

		if(offset==0)
		{
			*I = val;
			I++;
			*I = 0;
			I++;
			*I = 0;
		}
		else if(offset==1)
		{
			*I = 0;
			I++;
			*I = val;
			I++;
			*I = 0;
		}
		else if(offset==2)
		{
			*I = 0;
			I++;
			*I = 0;
			I++;
			*I = val;
		}
	}
}