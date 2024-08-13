#include "polymahalanobis.h"

//-----------------------------------------------------------------------------
classifiers::polyMahalanobis::polyMahalanobis()
{
	printf("\n\tInitialize Polynomial Mahalanobis");
	//m_polyMHolder = new mahaPoly();
	//m_polyMHolder->initLibrary();

	m_model = NULL;
	m_pattern = NULL;

}
//-----------------------------------------------------------------------------
classifiers::polyMahalanobis::~polyMahalanobis()
{
	if(m_model)
		delete m_model;

	if(m_pattern)
		delete m_pattern;
}
//-----------------------------------------------------------------------------
bool classifiers::polyMahalanobis::setPattern(pattern *_pattern)
{
	//if there is an instance of m_pattern, kill it !
	if(m_pattern)
		delete m_pattern;

	m_pattern = _pattern;
	return true;
}
//-----------------------------------------------------------------------------
float *classifiers::polyMahalanobis::calc_mean(doubleVector *data, unsigned int size, unsigned int d)
{
	float *m = (float*) calloc(d, sizeof(float));

	unsigned int i=0, j=0;
	for(i=0; i<size; i++)
	{
	    for(j=0; j<d; j++)
	    {
		m[j]+=data[i].v[j];
	    }
	}
	for(j=0; j<d; j++)
	    m[j]/=size;

	return m;
}
//-----------------------------------------------------------------------------
float classifiers::polyMahalanobis::getMaxValue(float *in, unsigned int size)
{
	float max=0;
 	for(unsigned int i=0; i<size; i++)
	{
		if(in[i] > max) max=in[i];
	}
	return max;
}
//-----------------------------------------------------------------------------
float classifiers::polyMahalanobis::getMaxAbsValue(float *in, unsigned int size)
{
	float max=0;
 	for(unsigned int i=0; i<size; i++)
	{
		if(fabs(in[i]) > max) max=fabs(in[i]);
	}
	return max;
}
//-----------------------------------------------------------------------------
unsigned int *classifiers::polyMahalanobis::nchoosek(unsigned int proj, unsigned int &size)
{
	int n = proj;
	size = fat(n) / (fat(n-2) * fat(2));
	unsigned int *C = (unsigned int*) calloc(size*2+2, sizeof(unsigned int));

	if(DEBUG) PRINT_DEBUG("\n\tnchoosek to %d is %d ", n, size);

	int k=0;
	for(int i=0; i<n; i++)
	{
		for(int j=i+1; j<n; j++)
		{
			C[k++]=i+1;  C[k++]=j+1;
		}
	}
	return C;
}
//-----------------------------------------------------------------------------
unsigned int *classifiers::polyMahalanobis::find_eq(int opt, float *in, unsigned int size, unsigned int &lenght)
{
	unsigned N=0;
	for(unsigned int i=0; i<size; i++)
	{
		if(opt == -1){ if(in[i] <  0) N++;}
		if(opt ==  0){ if(in[i] == 0) N++;}
		if(opt == +1){ if(in[i] >  0) N++;}
	}
	lenght = N;
	if(!N) return NULL;

	unsigned int *indexes = (unsigned int*) calloc(N, sizeof(unsigned int));
	for(unsigned int i=0; i<N; i++)
	{
		if(opt == -1){ if(in[i] <  0) indexes[i]=i+1;}
		if(opt ==  0){ if(in[i] == 0) indexes[i]=i+1;}
		if(opt == +1){ if(in[i] >  0) indexes[i]=i+1;}
	}

	return indexes;
}
//-----------------------------------------------------------------------------
CvMat *classifiers::polyMahalanobis::newProjection(CvMat *A, CvMat *cross)
{
	unsigned int nt = A->rows;
	unsigned int dt = A->cols;

	unsigned int dp=0;
	if(cross) dp = cross->cols;

	CvMat *cvm_new_dim = cvCreateMat(nt,dt+dt+dp,CV_32FC1);
	for(unsigned int i=0; i<nt; i++)
	{
		float v;
		for(unsigned int j=0; j<dt; j++)
		{
			v=cvmGet(A,i,j);
			cvmSet(cvm_new_dim,i,j,v);
		}
	}
	//proj_A.^2
	for(unsigned int i=0; i<nt; i++)
	{
		float v;
		for(unsigned int j=0; j<dt; j++)
		{
			v=cvmGet(A,i,j);
			cvmSet(cvm_new_dim,i,j+dt,v*v);
		}
	}
	//cross_terms
	for(unsigned int i=0; i<nt; i++)
	{
		float v;
		for(unsigned int j=0; j<dp; j++)
		{
			v=cvmGet(cross,i,j);
			cvmSet(cvm_new_dim,i,j+dt+dt,v);
		}
	}
	return cvm_new_dim;
}
//-----------------------------------------------------------------------------
float classifiers::polyMahalanobis::calcVariance(CvMat *A, int column)
{
	float var=0;
	unsigned int nt = A->rows;
	unsigned int dt = A->cols;

	if(column == -1)
	{
		double sum=0;
		for(unsigned int i=0; i<nt*dt; i++)
		{
			sum+=A->data.fl[i];
		}
		double mean=sum/(nt*dt);

		sum=0;
		for(unsigned int i=0; i<nt*dt; i++)
		{
			sum+= ((A->data.fl[i]-mean) * (A->data.fl[i]-mean));
		}
		var=1/((double)(nt*dt)-1) * sum;
	}
	else
	{
		double sum=0;
		for(unsigned int i=0; i<nt; i++)
		{
			sum+=cvmGet(A,i,column);
		}
		double mean=sum/nt;

		sum=0;
		for(unsigned int i=0; i<nt; i++)
		{
			sum+= ((cvmGet(A,i,column)-mean)*(cvmGet(A,i,column)-mean));
		}
		var=1/((double)nt-1) * sum;
	}
	return var;
}
//-----------------------------------------------------------------------------
void classifiers::polyMahalanobis::calcVariance(CvMat *A, float *var_dim)
{
	unsigned int nt = A->rows;
	unsigned int dt = A->cols;

	double *mean = (double*) calloc(dt, sizeof(double));
	for(unsigned int k=0; k<dt; k++)
	{
		for(unsigned int i=0; i<nt; i++)
		{
			mean[k]= (double)mean[k]+(double)cvmGet(A,i,k);
		}
	}
	for(unsigned int k=0; k<dt; k++)
	{
		mean[k]=mean[k]/nt;
		var_dim[k]=0;
	}

	for(unsigned int k=0; k<dt; k++)
	{
		for(unsigned int i=0; i<nt; i++)
		{
			var_dim[k]= var_dim[k] + ((cvmGet(A,i,k)-mean[k]) * (cvmGet(A,i,k)-mean[k]));
		}
	}
	for(unsigned int k=0; k<dt; k++)
	{
		var_dim[k]=1/((double)(nt)-1) * var_dim[k];
	}

	free(mean);
}
//-----------------------------------------------------------------------------
CvMat *classifiers::polyMahalanobis::removeNullIndexes(CvMat *A, unsigned int *ind_use, unsigned int size)
{
	CvMat *new_dim_used = cvCreateMat(A->rows,size,CV_32FC1);
	for(int i=0; i<A->rows; i++)
	{
		for(unsigned int j=0; j<size; j++)
		{
			new_dim_used->data.fl[i*size+j]=A->data.fl[ i*A->cols + (ind_use[j]-1)];
		}
	}
	return new_dim_used;
}
//-----------------------------------------------------------------------------
CvMat *classifiers::polyMahalanobis::removeNullDimensions(CvMat *A, unsigned int *ind_use)
{
	/***
	var_new_dim = var(new_dim);
	ind_use = find(var_new_dim > 1e-8*max(var_new_dim));
	A = new_dim(:,ind_use);
	*/
	unsigned int nt = A->rows;
	unsigned int dt = A->cols;
	float *var_new_dim = (float*) calloc(dt, sizeof(float));
	calcVariance(A, var_new_dim);

	unsigned int N=0;
	float maxVar = getMaxValue(var_new_dim, dt);
	for(unsigned int i=0; i<dt; i++)
	{
		if(var_new_dim[i] > 1e-8 * maxVar) N++;
	}
	ind_use[0]=N;

	CvMat *newMat = cvCreateMat(nt,N,CV_32FC1);
	int k=0;
	for(unsigned int i=0; i<dt; i++)
	{
		if(var_new_dim[i] > 1e-8 * maxVar)
		{
			for(unsigned int j=0; j<nt; j++)
			{
				newMat->data.fl[j*N+k] = A->data.fl[j*dt+i];
			}
			ind_use[k+1]=i+1;
			k++;
		}
	}

	free(var_new_dim);
	return newMat;
}
//-----------------------------------------------------------------------------
bool classifiers::polyMahalanobis::makeSpace(unsigned int order)
{
	//there is the make space function, and ????
	printf("\n\tCreating the topological map");

	//if the pattern was not defined, kill me !
	if(!m_pattern)
	{
		printf("\n\t ***ERROR: pattern does not defined !\n");
		exit(1);
	}

	//**** old poorly written matlab routines !!!!
	//m_polyMHolder->makeSpace("conf.maha", order);
	////load classifier model
	//if(m_model) delete m_model;
		//m_model = loadModel("model.spc");
	//return true;

	//**** NEW POWERFULL C++ ROUTINES !!!!!!!!!
	//if there is a previous m_model, kill him !
	if(m_model)
		delete m_model;
	m_model = new polyModel();
	m_model->num_levels = order;
	m_model->num_initialdim = m_pattern->getDim();

	//*** sig_max = 4e-6;
	float sig_max = 4e-6;
	float eps_svd = sig_max;

	//*** [n,d] = size(X);
	//getting information
	unsigned int nt = m_pattern->getSize();
	unsigned int dt = m_pattern->getDim();
	doubleVector *data = m_pattern->getData();

	//calc the mean vector of a pattern
	if(m_model->m_center) free(m_model->m_center);
	m_model->m_center = calc_mean(data, nt, dt);

	//copying the pattern to a matrix openvc model called m_pattern
	CvMat *cvm_pattern = cvCreateMat(nt,dt,CV_32FC1);
	for(unsigned int i=0; i<nt; i++)
	{
 		for(unsigned int j=0; j<dt; j++)
		{
			cvm_pattern->data.fl[i*cvm_pattern->cols+j] = (float) data[i].v[j];
		}
	}

	//*** A = X(2:n,:) - repmat(Xc(1,:),n-1,1);
	//doing the aforementioned instruction
	CvMat* cvm_A;
	cvm_A=cvCloneMat(cvm_pattern);
	for(unsigned int i=0; i<nt; i++)
	{
		for(unsigned int j=0; j<dt; j++)
		{
			cvm_A->data.fl[i*cvm_pattern->cols+j] -= m_model->m_center[j];
		}
	}
	//here an example how to access an element
	//float t1=cvmGet(cvm_A,0,0);

	//%%%%%%%%%%%%%%%%%%%%%%%%
	//%%%%%%%%%%%%%%%%%%%%%%%%
	//%%%%%%%%%%%%%%%%%%%%%%%%
	//%%% Find PCA basis
	//%%%%%%%%%%%%%%%%%%%%%%%%
	//%%%%%%%%%%%%%%%%%%%%%%%%
	//%%%%%%%%%%%%%%%%%%%%%%%%
	//*** if nt >= dt ...
	CvMat *cvm_ATA = cvCreateMat(dt,dt,CV_32FC1);
	CvMat *cvm_Ucont = NULL;

	float *my_lamda=NULL;
	unsigned int my_lambdaSize=0;

	//*************************************************************************
	//*************************************************************************
	//*************************************************************************

	unsigned int *ind_null, *ind_basis, indn_lenght, indb_lenght;
	float s_max, s_min;

	if(nt >= dt)
	{
		//*** AtA = A'*A;
		CvMat *cvm_AT = cvCreateMat(dt,nt,CV_32FC1);
		cvTranspose(cvm_A, cvm_AT);
		cvMatMul(cvm_AT, cvm_A, cvm_ATA);

		CvMat *cvm_Uconttmp = cvCreateMat(dt,dt,CV_32FC1);
		CvMat *cvm_S  	    = cvCreateMat(dt,dt,CV_32FC1);
		CvMat *cvm_V        = cvCreateMat(dt,dt,CV_32FC1);
		cvSVD(cvm_ATA, cvm_S, cvm_Uconttmp, cvm_V, CV_SVD_U_T|CV_SVD_V_T); // A = U S V^T

		CvMat *cvm_UconttmpT = cvCreateMat(dt,dt,CV_32FC1);
		cvTranspose(cvm_Uconttmp, cvm_UconttmpT);
		cvReleaseMat(&cvm_Uconttmp);
		cvm_Uconttmp = cvm_UconttmpT;

		//*** s_val = diag(S);
		float *s_val = (float*) calloc(dt, sizeof(float));
		for(unsigned int i=0; i<dt; i++)
		{
			s_val[i]=cvmGet(cvm_S,i,i);
		}

		//*** s_max = max(s_val);
		s_max = getMaxValue(s_val, dt);

		//*** s_min = eps_svd*s_max;
		s_min = eps_svd*s_max;

		//*** s_val(s_val<s_min) = 0;
		for(unsigned int i=0; i<dt; i++)
		{
			s_val[i] = (s_val[i] < s_min) ? 0 : s_val[i];
		}

		//*** ind_null = find(s_val == 0 );
		//*** ind_basis = find(s_val > 0 );
		ind_null  = find_eq(0, s_val, dt, indn_lenght);
		ind_basis = find_eq(1, s_val, dt, indb_lenght);

		//*** my_lamda = (s_val(ind_basis))';
		if(my_lamda) free(my_lamda);
		my_lamda = (float*) calloc(indb_lenght, sizeof(float));
		my_lambdaSize = indb_lenght;
		for(unsigned int i=0; i<indb_lenght; i++)
		{
			my_lamda[i] = s_val[ind_basis[i]-1];
		}

		//*** Ucont = Ucont(:,ind_basis);
		if(cvm_Ucont) cvReleaseMat(&cvm_Ucont);
		cvm_Ucont = cvCreateMat(dt,my_lambdaSize,CV_32FC1);
		for(unsigned int i=0; i<dt; i++)
		{
			for(unsigned int j=0; j<my_lambdaSize; j++)
			{
				cvm_Ucont->data.fl[i*my_lambdaSize+j]=cvmGet(cvm_Uconttmp,i,j);
			}
		}

		//locally informations unneeded anymore
		cvReleaseMat(&cvm_AT);
		cvReleaseMat(&cvm_S);
		cvReleaseMat(&cvm_V);
		cvReleaseMat(&cvm_Uconttmp);

		//free(ind_basis);
		free(ind_null);
		free(s_val);
	}
	else
	{
		printf("\n\n\t*** This part of the code was not codified to C++ yet");
		printf("\n\t*** This error will occur when the number of dimensions is bigger than the size of samples");
		printf("\n\t*** Please, if this situation has occured, contact asobieranski@cyclops.ufsc.br");
		exit(1);
	}


	//*************************************************************************
	//*************************************************************************
	//*************************************************************************
	//*** proj_A = A * Ucont;
	CvMat *cvm_proj_A = cvCreateMat(nt,dt,CV_32FC1);
	cvMatMul(cvm_A, cvm_Ucont, cvm_proj_A);

	//*** max_aP = max(abs(proj_A(:)));
	float max_aP = getMaxAbsValue(cvm_proj_A->data.fl, cvm_proj_A->rows*cvm_proj_A->cols);

	/****if max_aP > my_eps
		proj_A = proj_A/max_aP;
	else
		max_aP = 1;
	end
	*/
	if(max_aP > eps_svd)
	{
		for(int i=0;i<cvm_proj_A->rows*cvm_proj_A->cols;i++)
		{
			cvm_proj_A->data.fl[i]/=max_aP;
		}
	}
	else
	{
		max_aP = 1;
	}

	//*** [n_proj,d_proj] = size(proj_A);
	unsigned int n_proj = cvm_proj_A->rows;
	unsigned int d_proj = cvm_proj_A->cols;

	//*** num_svds = 1;
	int num_svds = 1;

	//***
	/*
	lev(num_svds).A_basis = Ucont;
	lev(num_svds).max_aP = max_aP;
	lev(num_svds).ind_use = 1:d;
	lev(num_svds).d_proj = d_proj;
	dms = -my_lamda./(s_min * (my_lamda + s_min));
	lev(num_svds).dms = dms;
	lev(num_svds).sigma_inv = 1/s_min;
	*/
	lev_basis *newBasis = (lev_basis*) calloc(1, sizeof(lev_basis));
	newBasis->cvm_A_basis = cvm_Ucont;
	newBasis->max_aP = max_aP;
	newBasis->ind_usesize = indb_lenght;
	newBasis->ind_use = ind_basis;
	newBasis->d_proj = d_proj;
	newBasis->dmssize = indb_lenght;
	newBasis->dms = (float*) calloc(newBasis->dmssize, sizeof(float));
 	for(unsigned int i=0; i<indb_lenght; i++)
	{
		newBasis->dms[i] = -my_lamda[i] / (s_min * (my_lamda[i]+s_min));
	}
	newBasis->sigma_inv = 1/s_min;
	m_model->levBegin = newBasis;

	//*** if d_proj > 1
	bool cont;
	CvMat *cvm_new_dim;

	if(d_proj > 1)
	{
		//*** C = nchoosek(1:d_proj,2);
		unsigned int sizeC;
		unsigned int *C = nchoosek(m_model->levBegin->d_proj, sizeC);

		//*** cross_terms = proj_A(:,C(:,1)).*proj_A(:,C(:,2));
		CvMat *cvm_cross_terms = cvCreateMat(n_proj,sizeC,CV_32FC1);
		for(unsigned int k=0; k<sizeC*2; k+=2)
		{
			float v;
			for(unsigned int i=0; i<n_proj; i++)
			{
				v=cvmGet(cvm_proj_A,i,C[k]-1) * cvmGet(cvm_proj_A,i,C[k+1]-1);
				cvmSet(cvm_cross_terms,i,k/2,v);
			}
		}

		//***  new_dim = [proj_A,proj_A.^2,cross_terms];
		//*** %new_dim = [proj_A.^2,cross_terms];
		cvm_new_dim = newProjection(cvm_proj_A, cvm_cross_terms);
		float var = calcVariance(cvm_new_dim, -1);

		/*** if max(var(new_dim(:))) > my_eps
			cont = 1;
		else
			cont = 0;
		end
		*/
		if(var > eps_svd)
			cont=true;
		else
			cont=false;

		//locally informations unneeded anymore
		cvReleaseMat(&cvm_cross_terms);
		free(C);
	}
	else
	{
		cont=false;
	}

	/***
	if num_levels == 1
		cont = 0;
	end*/
	if(m_model->num_levels == 1)
	{
		cont=false;
	}

	//*** while cont == 1
	//***
	//*** 	THE BIG WHILE !!!!!!!
	//***
	//***
	lev_basis *currBasis=m_model->levBegin;
	while(cont==true)
	{
		//*** % Choose the basis set for the next polynomial
		/*
		if size(new_dim,2) == 1
			new_dim_used = new_dim;
			ind_use = 1;
			A = new_dim(:,ind_use);
		else
			var_new_dim = var(new_dim);
			ind_use = find(var_new_dim > 1e-8*max(var_new_dim));
			A = new_dim(:,ind_use);
		end
		*/
		n_proj = cvm_new_dim->rows;
		d_proj = cvm_new_dim->cols;

		if(cvm_A) cvReleaseMat(&cvm_A);

		//this variable is funny, but functional !!!! [0] size, [1,2,3... each position] !
		unsigned int *ind_use;
		if(d_proj == 1)
		{
			//cvm_A = cvCreateMat(n_proj,d_proj,CV_32FC1);
			cvm_A=cvCloneMat(cvm_new_dim);
		}
		else
		{
			//***
			/*
			var_new_dim = var(new_dim);
			ind_use = find(var_new_dim > 1e-8*max(var_new_dim));
			A = new_dim(:,ind_use);
			*/
			unsigned int *ind_useTmp = (unsigned int*) calloc(cvm_new_dim->cols + 1, sizeof(unsigned int));
			cvm_A = removeNullDimensions(cvm_new_dim, ind_useTmp);

			//gamby !
			ind_use = (unsigned int*) calloc(ind_useTmp[0], sizeof(unsigned int));
 			for(unsigned int i=0; i<ind_useTmp[0]; i++)
			{
				ind_use[i] = ind_useTmp[i+1];
			}
			free(ind_useTmp);
		}

	    //%%%%%%%%%%%%%%%%%%%%%%%%
		//%%%%%%%%%%%%%%%%%%%%%%%%
		//%%%%%%%%%%%%%%%%%%%%%%%%
		//%%% Find PCA basis
		//%%%%%%%%%%%%%%%%%%%%%%%%
		//%%%%%%%%%%%%%%%%%%%%%%%%
		//%%%%%%%%%%%%%%%%%%%%%%%%

		//*** [nt,dt] = size(A);
		nt = cvm_A->rows;
		dt = cvm_A->cols;

		if(cvm_ATA)
		{
			cvReleaseMat(&cvm_ATA);
		}
		cvm_ATA = cvCreateMat(dt,dt,CV_32FC1);

		//*** if nt >= dt
		if(nt >= dt)
		{
			//*** AtA = A'*A;
			CvMat *cvm_AT = cvCreateMat(dt,nt,CV_32FC1);
			cvTranspose(cvm_A, cvm_AT);
			cvMatMul(cvm_AT, cvm_A, cvm_ATA);


			CvMat *cvm_Uconttmp = cvCreateMat(dt,dt,CV_32FC1);
			CvMat *cvm_S  	    = cvCreateMat(dt,dt,CV_32FC1);
			CvMat *cvm_V        = cvCreateMat(dt,dt,CV_32FC1);
			cvSVD(cvm_ATA, cvm_S, cvm_Uconttmp, cvm_V, CV_SVD_U_T|CV_SVD_V_T); // A = U S V^T

			CvMat *cvm_UconttmpT = cvCreateMat(dt,dt,CV_32FC1);
			cvTranspose(cvm_Uconttmp, cvm_UconttmpT);
			cvReleaseMat(&cvm_Uconttmp);
			cvm_Uconttmp = cvm_UconttmpT;

			//*** s_val = diag(S);
			float *s_val = (float*) calloc(dt, sizeof(float));
			for(unsigned int i=0; i<dt; i++)
			{
				s_val[i]=cvmGet(cvm_S,i,i);
			}


			//*** s_max = max(s_val);
			s_max = getMaxValue(s_val, dt);

			//*** s_min = eps_svd*s_max;
			s_min = eps_svd*s_max;

			//*** s_val(s_val<s_min) = 0;
			for(unsigned int i=0; i<dt; i++)
			{
				s_val[i] = (s_val[i] < s_min) ? 0 : s_val[i];
			}

			//*** ind_null = find(s_val == 0 );
			//*** ind_basis = find(s_val > 0 );
			ind_null  = find_eq(0, s_val, dt, indn_lenght);
			ind_basis = find_eq(1, s_val, dt, indb_lenght);

			//*** my_lamda = (s_val(ind_basis))';
			if(my_lamda) free(my_lamda);
			my_lamda = (float*) calloc(indb_lenght, sizeof(float));
			my_lambdaSize = indb_lenght;
			for(unsigned int i=0; i<indb_lenght; i++)
			{
				my_lamda[i] = s_val[ind_basis[i]-1];
			}

			//*** Ucont = Ucont(:,ind_basis);
			//if(cvm_Ucont) cvReleaseMat(&cvm_Ucont);
			cvm_Ucont = cvCreateMat(dt,my_lambdaSize,CV_32FC1);
			for(unsigned int i=0; i<dt; i++)
			{
				for(unsigned int j=0; j<my_lambdaSize; j++)
				{
					cvm_Ucont->data.fl[i*my_lambdaSize+j]=cvmGet(cvm_Uconttmp,i,j);
				}
			}

			//locally informations unneeded anymore
			cvReleaseMat(&cvm_AT);
			cvReleaseMat(&cvm_S);
			cvReleaseMat(&cvm_V);
			cvReleaseMat(&cvm_Uconttmp);

			free(ind_basis);
			free(ind_null);
			free(s_val);
		}
		else
		{
			printf("\n\n\t*** This part of the code was not codified to C++ yet");
			printf("\n\t*** This error will occur when the number of dimensions is bigger than the size of samples");
			printf("\n\t*** Please, if this situation has occured, contact asobieranski@cyclops.ufsc.br");
			exit(1);
		}
		//*************************************************************************
		//*************************************************************************
		//*************************************************************************
		//*** proj_A = A * Ucont;
		if(cvm_proj_A) 	cvReleaseMat(&cvm_proj_A);
		cvm_proj_A = cvCreateMat(cvm_A->rows,cvm_Ucont->cols,CV_32FC1);
		cvMatMul(cvm_A, cvm_Ucont, cvm_proj_A);

		//*** max_aP = max(abs(proj_A(:)));
		max_aP = getMaxAbsValue(cvm_proj_A->data.fl, cvm_proj_A->rows*cvm_proj_A->cols);

		/****if max_aP > my_eps
			proj_A = proj_A/max_aP;
		else
			max_aP = 1;
		end
		*/
		if(max_aP > eps_svd)
		{
			for(int i=0;i<cvm_proj_A->rows*cvm_proj_A->cols;i++)
			{
				cvm_proj_A->data.fl[i]/=max_aP;
			}
		}
		else
		{
			max_aP = 1;
		}

		//*** [n_proj,d_proj] = size(proj_A);
		n_proj = cvm_proj_A->rows;
		d_proj = cvm_proj_A->cols;

		//*** num_svds = num_svds + 1;
		num_svds++;


		lev_basis *newBasis = (lev_basis*) calloc(1, sizeof(lev_basis));
		newBasis->cvm_A_basis = cvm_Ucont;
		newBasis->max_aP = max_aP;
		newBasis->ind_usesize = dt;
		newBasis->ind_use = ind_use;
		newBasis->d_proj = d_proj;
		newBasis->dmssize = indb_lenght;
		newBasis->dms = (float*) calloc(newBasis->dmssize, sizeof(float));
		for(unsigned int i=0; i<indb_lenght; i++)
		{
			newBasis->dms[i] = -my_lamda[i] / (s_min * (my_lamda[i]+s_min));
		}
		newBasis->sigma_inv = 1/s_min;
		currBasis->next = newBasis;
		currBasis = currBasis->next;


		if(d_proj > 1)
		{
			//*** C = nchoosek(1:d_proj,2);
			unsigned int sizeC;
			unsigned int *C = nchoosek(currBasis->d_proj, sizeC);

			//*** cross_terms = proj_A(:,C(:,1)).*proj_A(:,C(:,2));
			CvMat *cvm_cross_terms = cvCreateMat(n_proj,sizeC,CV_32FC1);
			for(unsigned int k=0; k<sizeC*2; k+=2)
			{
				float v;
				for(unsigned int i=0; i<n_proj; i++)
				{
					v=cvmGet(cvm_proj_A,i,C[k]-1) * cvmGet(cvm_proj_A,i,C[k+1]-1);
					cvmSet(cvm_cross_terms,i,k/2,v);
				}
			}

			//***  new_dim = [proj_A,proj_A.^2,cross_terms];
			//*** %new_dim = [proj_A.^2,cross_terms];
			if(cvm_new_dim) cvReleaseMat(&cvm_new_dim);
			cvm_new_dim = newProjection(cvm_proj_A, cvm_cross_terms);
			float var = calcVariance(cvm_new_dim, -1);

			/*** if max(var(new_dim(:))) > my_eps
				cont = 1;
			else
				cont = 0;
			end
			*/
			if(var > eps_svd)
				cont=true;
			else
				cont=false;

			//locally informations unneeded anymore
			cvReleaseMat(&cvm_cross_terms);
			free(C);
		}
		else
		{
			cont=false;
		}

		//***
		/* if num_svds >= num_levels
			  cont = 0;
		end*/

		if(num_svds >= m_model->num_levels)
			  cont = 0;
	}

	//releasing informations unneeded
	if(cvm_pattern) cvReleaseMat(&cvm_pattern);
	if(cvm_A) 		cvReleaseMat(&cvm_A);
	if(cvm_ATA) 	cvReleaseMat(&cvm_ATA);
	//cvReleaseMat(&cvm_Ucont); --> used above in levBegin basis
	if(cvm_proj_A) 	cvReleaseMat(&cvm_proj_A);
	if(cvm_new_dim) cvReleaseMat(&cvm_new_dim);

	free(my_lamda);

	if(DEBUG) showLevBasis();

	return true;
}
//-----------------------------------------------------------------------------
void classifiers::polyMahalanobis::showLevBasis()
{
	PRINT_DEBUG("\n\n\n\t *** DEBUG INFO ***");
	PRINT_DEBUG("\n\t m_model->num_levels: %d", m_model->num_levels);
	PRINT_DEBUG("\n\t m_model->num_initialdim: %d", m_model->num_initialdim);
	PRINT_DEBUG("\n\t m_model->center: ");
	for(unsigned int i=0; i<m_model->num_initialdim; i++)
		PRINT_DEBUG("%.3f  ", m_model->m_center[i]);

	int ibasis=0;
	for(classifiers::lev_basis *curr=m_model->levBegin; curr != NULL; curr=curr->next)
	{
		PRINT_DEBUG("\n\t\t Lev_Begin : %d", ++ibasis);
		PRINT_DEBUG("\n\t\t --> max_aP : %.3f", curr->max_aP);
		PRINT_DEBUG("\n\t\t --> sigma_inv : %.6f", curr->sigma_inv);
		PRINT_DEBUG("\n\t\t --> ind_usesize : %d", curr->ind_usesize);
		PRINT_DEBUG("\n\t\t --> ind_use: ");
		for(int i=0; i<curr->ind_usesize; i++)
			PRINT_DEBUG("%d ", curr->ind_use[i]);

		PRINT_DEBUG("\n\t\t --> dmssize : %d", curr->dmssize);
		PRINT_DEBUG("\n\t\t --> dms: ");
		for(int i=0; i<curr->dmssize; i++)
			PRINT_DEBUG("%.4f ", curr->dms[i]);

		PRINT_DEBUG("\n\t\t --> cvm_A_basis of (%d, %d)", curr->cvm_A_basis->rows, curr->cvm_A_basis->cols);
		for(int i=0; i<curr->cvm_A_basis->rows; i++)
		  {
			PRINT_DEBUG("\n\t\t");
			for(int j=0; j<curr->cvm_A_basis->cols; j++)
			{
				PRINT_DEBUG("\t%.3f", curr->cvm_A_basis->data.fl[i*curr->cvm_A_basis->cols+j]);
			}
		}		PRINT_DEBUG("\n\t\t-----------------------------------------------------------\n");
	}

	printf("\n");
}
//-----------------------------------------------------------------------------
void classifiers::polyMahalanobis::showMatrixValues(CvMat *A)
{
	PRINT_DEBUG("\n\t\tMatrix Values (%d-%d)", A->rows, A->cols);
	for(int i=0; i<A->rows; i++)
	{
		PRINT_DEBUG("\n\t\t");
		for(int j=0; j<A->cols; j++)
		{
			PRINT_DEBUG("\t%.4f ", A->data.fl[i*A->cols+j]);
		}
	}
	PRINT_DEBUG("\n");
}
//-----------------------------------------------------------------------------
void classifiers::polyMahalanobis::showInputData(double *im_data, unsigned int size, unsigned int dimensions)
{
	PRINT_DEBUG("\n\t\tInput Data is (%d-%d)", size, dimensions);
	for(unsigned int i=0; i<size; i++)
	{
		PRINT_DEBUG("\n\t\t");
		for(unsigned int j=0; j<dimensions; j++)
		{
			PRINT_DEBUG("\t%.4f ", im_data[i*dimensions+j]);
		}
	}
}
//-----------------------------------------------------------------------------
void classifiers::polyMahalanobis::showResultingData(double *res_data, unsigned int size, unsigned int levels)
{
	PRINT_DEBUG("\n\t\tResulting Data is (%d-%d)", size, levels);
	for(unsigned int i=0; i<size; i++)
	{
		PRINT_DEBUG("\n\t\t");
		for(unsigned int j=i; j<levels*size; j+=size)
		{
			PRINT_DEBUG("\t%.4f ", res_data[j]);
		}
	}
}
//-----------------------------------------------------------------------------
double *classifiers::polyMahalanobis::evaluateToVector(double *im_data, double *refVector, unsigned int size, unsigned int dimensions)
{
	//allocating the return of the method
	double *m_intensValues = (double*) calloc(size*m_model->num_levels, sizeof(double));
	m_model->max_level = 0;

	//X = U - G;
	CvMat *cvm_X = cvCreateMat(size,dimensions,CV_32FC1);
	for(unsigned int i=0; i<size; i++)
	{
		float v=0;
		for(unsigned int j=0; j<dimensions; j++)
		{
			v = im_data[i*dimensions+j] - refVector[j];
			cvmSet(cvm_X,i,j,v);
		}
	}

	int projCount = 1;
	classifiers::lev_basis *currBasis = m_model->levBegin;

	//*** proj_A = X * model.lev(i).A_basis;
	//CvMat *cvm_proj_A = cvCreateMat(cvm_X->rows,currBasis->cvm_A_basis->cols,CV_32FC1);
	CvMat *cvm_proj_A_sq = cvCreateMat(cvm_X->rows,currBasis->cvm_A_basis->cols,CV_32FC1);
	cvMatMul(cvm_X, currBasis->cvm_A_basis, cvm_proj_A_sq);

	//*** proj_A_sq = proj_A.^2;
	//CvMat *cvm_proj_A_sq = cvCreateMat(cvm_X->rows,currBasis->cvm_A_basis->cols,CV_32FC1);
	for(int i=0; i<cvm_proj_A_sq->rows; i++)
	{
		float v;
		for(int j=0; j<cvm_proj_A_sq->cols; j++)
		{
			v=cvmGet(cvm_proj_A_sq,i,j);
			cvmSet(cvm_proj_A_sq,i,j,v*v);
		}
	}

	//*** A_sq = X.^2;
	CvMat *cvm_A_sq = cvCreateMat(size,dimensions,CV_32FC1);
	for(int i=0; i<cvm_A_sq->rows; i++)
	{
		float v;
		for(int j=0; j<cvm_A_sq->cols; j++)
		{
			v=cvmGet(cvm_X,i,j);
			cvmSet(cvm_A_sq,i,j,v*v);
		}
	}

	//*** q1 = (sum((A_sq * model.lev(i).sigma_inv)'))';
	CvMat* cvm_A_sqtmp;
	cvm_A_sqtmp=cvCloneMat(cvm_A_sq);
	for(int i=0; i<cvm_A_sq->rows; i++)
	{
		for(int j=0; j<cvm_A_sq->cols; j++)
		{
			cvmSet(cvm_A_sqtmp,i,j, cvm_A_sq->data.fl[i*cvm_A_sq->cols+j] * currBasis->sigma_inv);
		}
	}

	CvMat* cvm_A_sqtmpT = cvCreateMat(cvm_A_sq->cols,cvm_A_sq->rows, CV_32FC1);
	cvTranspose(cvm_A_sqtmp, cvm_A_sqtmpT);

	double *q1 = (double*) calloc(cvm_A_sqtmpT->cols, sizeof(double));
	for(int i=0; i<cvm_A_sqtmpT->rows; i++)
	{
		for(int j=0; j<cvm_A_sqtmpT->cols; j++)
		{
			q1[j]+=cvmGet(cvm_A_sqtmpT,i,j);
		}
	}

	//releasing local memory
	if(cvm_A_sqtmp)  cvReleaseMat(&cvm_A_sqtmp);
	if(cvm_A_sqtmpT) cvReleaseMat(&cvm_A_sqtmpT);


	/***
	if size(proj_A_sq,2) > 1
		q2 = (sum((proj_A_sq.*repmat(dms,size(proj_A_sq,1),1))'))';
	else
		q2 = (proj_A_sq.*repmat(dms,size(proj_A_sq,1),1));
	end
	//*/

	//if(cvm_proj_A_sq->cols > 1)
	//{
		double *q2 = (double*) calloc(cvm_proj_A_sq->rows, sizeof(double));
		for(int i=0; i<cvm_proj_A_sq->rows; i++)
		{
			for(int j=0; j<cvm_proj_A_sq->cols; j++)
			{
				q2[i]+=cvmGet(cvm_proj_A_sq,i,j) * currBasis->dms[j];
			}
		}

	//}
	//*** q_in = q1 + q2; q_in(q_in<0) = 0;
	double *q_in = (double*) calloc(cvm_proj_A_sq->rows, sizeof(double));
	for(int i=0; i<cvm_proj_A_sq->rows; i++)
	{
		double v = q1[i] + q2[i];
		q_in[i] = (v < 0) ? 0 : v;
	}

	/***
	if DEBUG == 1
		val_all = q_in;
	end
	val = q_in;
	*/
	for(unsigned int i=0; i<size; i++)
	{
		m_intensValues[(projCount-1)*size + i] = q_in[i];
	}
	if(q1) free(q1);
	if(q2) free(q2);
	if(q_in) free(q_in);
	m_model->max_level++;

	/***
	if model.num_levels > 1
		proj = X * model.lev(i).A_basis;
		proj = proj/model.lev(i).max_aP;
		if model.lev(i).d_proj > 1
			C = nchoosek(1:model.lev(i).d_proj,2);
			cross_terms = proj(:,C(:,1)).*proj(:,C(:,2));
			new_dim = [proj,proj.^2,cross_terms];
			%new_dim = [proj.^2,cross_terms];
		else
			new_dim = [proj,proj.^2];
			%new_dim = [proj.^2];
		end
	end
	*/

	CvMat *cvm_new_dim=NULL;
	if(m_model->num_levels > 1)
	{
		//*** proj = X * model.lev(i).A_basis;
		//*** proj = proj/model.lev(i).max_aP;
		CvMat *cvm_proj_A = cvCreateMat(cvm_proj_A_sq->rows,cvm_proj_A_sq->cols,CV_32FC1);
		cvMatMul(cvm_X, currBasis->cvm_A_basis, cvm_proj_A);

		for(int i=0; i<cvm_proj_A_sq->rows; i++)
		{
			for(int j=0; j<cvm_proj_A_sq->cols; j++)
			{
				cvm_proj_A->data.fl[i*cvm_proj_A_sq->cols+j]/=currBasis->max_aP;
			}
		}
		if(currBasis->d_proj > 1)
		{
			//*** C = nchoosek(1:d_proj,2);
			unsigned int sizeC;
			unsigned int *C = nchoosek(currBasis->d_proj, sizeC);

			//*** cross_terms = proj_A(:,C(:,1)).*proj_A(:,C(:,2));
			CvMat *cvm_cross_terms = cvCreateMat(cvm_proj_A_sq->rows,sizeC,CV_32FC1);
			for(unsigned int k=0; k<sizeC*2; k+=2)
			{
				float v;
				for(int i=0; i<cvm_proj_A_sq->rows; i++)
				{
					v=cvmGet(cvm_proj_A,i,C[k]-1) * cvmGet(cvm_proj_A,i,C[k+1]-1);
					cvmSet(cvm_cross_terms,i,k/2,v);
				}
			}

			//***  new_dim = [proj_A,proj_A.^2,cross_terms];
			//*** %new_dim = [proj_A.^2,cross_terms];
			cvm_new_dim = newProjection(cvm_proj_A, cvm_cross_terms);

			free(C);
			if(cvm_cross_terms) cvReleaseMat(&cvm_cross_terms);
			if(cvm_proj_A) cvReleaseMat(&cvm_proj_A);
		}
		else
		{
			//***  new_dim = [proj_A,proj_A.^2];
			cvm_new_dim = newProjection(cvm_proj_A, NULL);
		}
	}

	/*
	for i = 2:model.num_levels
	new_dim_used = new_dim(:,model.lev(i).ind_use);
	proj_A = new_dim_used * model.lev(i).A_basis;
	proj_A_sq = proj_A.^2;
	*/
	for(projCount=2; projCount<m_model->num_levels+1; projCount++)
	{
		currBasis=currBasis->next;
		if(!currBasis) break;

		//new_dim_used = new_dim(:,model.lev(i).ind_use);
		CvMat *new_dim_used = removeNullIndexes(cvm_new_dim, currBasis->ind_use, currBasis->ind_usesize);

		//*** proj_A = X * model.lev(i).A_basis;
		if(cvm_proj_A_sq) cvReleaseMat(&cvm_proj_A_sq);
		cvm_proj_A_sq = cvCreateMat(new_dim_used->rows,currBasis->cvm_A_basis->cols,CV_32FC1);
		cvMatMul(new_dim_used, currBasis->cvm_A_basis, cvm_proj_A_sq);

		float v;
		for(int i=0; i<new_dim_used->rows; i++)
		{
			for(int j=0; j<currBasis->cvm_A_basis->cols; j++)
			{
				v=cvmGet(cvm_proj_A_sq,i,j);
				cvmSet(cvm_proj_A_sq,i,j,v*v);
			}
		}

		//*** dms = model.lev(i).dms;
	    //*** A_sq = new_dim_used.^2;
		if(cvm_A_sq) cvReleaseMat(&cvm_A_sq);
		CvMat *cvm_A_sq = cvCreateMat(new_dim_used->rows,new_dim_used->cols,CV_32FC1);
		for(int i=0; i<new_dim_used->rows; i++)
		{
			for(int j=0; j<new_dim_used->cols; j++)
			{
				v=cvmGet(new_dim_used,i,j);
				cvmSet(cvm_A_sq,i,j,v*v);
			}
		}

		/***
		q1 = (sum((A_sq * model.lev(i).sigma_inv)'))';
		if size(proj_A_sq,2) > 1
			q2 = (sum((proj_A_sq.*repmat(dms,size(proj_A_sq,1),1))'))';
		else
			q2 = (proj_A_sq.*repmat(dms,size(proj_A_sq,1),1));
		end
		q_in = q1 + q2;
		q_in(q_in<0) = 0;
		*/
		CvMat* cvm_A_sqtmp;
		cvm_A_sqtmp=cvCloneMat(cvm_A_sq);
		for(int i=0; i<cvm_A_sq->rows; i++)
		{
			for(int j=0; j<cvm_A_sq->cols; j++)
			{
				cvmSet(cvm_A_sqtmp,i,j, cvm_A_sq->data.fl[i*cvm_A_sq->cols+j] * currBasis->sigma_inv);
			}
		}

		CvMat* cvm_A_sqtmpT = cvCreateMat(cvm_A_sq->cols,cvm_A_sq->rows, CV_32FC1);
		cvTranspose(cvm_A_sqtmp, cvm_A_sqtmpT);

		double *q1 = (double*) calloc(cvm_A_sqtmpT->cols, sizeof(double));
		for(int i=0; i<cvm_A_sqtmpT->rows; i++)
		{
			for(int j=0; j<cvm_A_sqtmpT->cols; j++)
			{
				q1[j]+=cvmGet(cvm_A_sqtmpT,i,j);
			}
		}

		//releasing local memory
		if(cvm_A_sqtmp)  cvReleaseMat(&cvm_A_sqtmp);
		if(cvm_A_sqtmpT) cvReleaseMat(&cvm_A_sqtmpT);


		double *q2 = (double*) calloc(cvm_proj_A_sq->rows, sizeof(double));
		for(int i=0; i<cvm_proj_A_sq->rows; i++)
		{
			for(int j=0; j<cvm_proj_A_sq->cols; j++)
			{
				q2[i]+=cvmGet(cvm_proj_A_sq,i,j) * currBasis->dms[j];
			}
		}

		//}
		//*** q_in = q1 + q2; q_in(q_in<0) = 0;
		double *q_in = (double*) calloc(cvm_proj_A_sq->rows, sizeof(double));
		for(int i=0; i<cvm_proj_A_sq->rows; i++)
		{
			double v = q1[i] + q2[i];
			q_in[i] = (v < 0) ? 0 : v;
		}



		//*** proj = new_dim_used * model.lev(i).A_basis;
		//*** proj = proj/model.lev(i).max_aP;
		CvMat *cvm_proj = cvCreateMat(new_dim_used->rows,currBasis->cvm_A_basis->cols,CV_32FC1);
		cvMatMul(new_dim_used, currBasis->cvm_A_basis, cvm_proj);

		for(int i=0; i<cvm_proj->rows; i++)
		{
			for(int j=0; j<cvm_proj->cols; j++)
			{
				cvm_proj->data.fl[i*cvm_proj->cols+j]/=currBasis->max_aP;
			}
		}

		if(currBasis->d_proj > 1)
		{
			//*** C = nchoosek(1:d_proj,2);
			unsigned int sizeC;
			unsigned int *C = nchoosek(currBasis->d_proj, sizeC);

			//*** cross_terms = proj_A(:,C(:,1)).*proj_A(:,C(:,2));
			CvMat *cvm_cross_terms = cvCreateMat(size,sizeC,CV_32FC1);
			for(unsigned int k=0; k<sizeC*2; k+=2)
			{
				float v;
				for(int i=0; i<cvm_proj->rows; i++)
				{
					v=cvmGet(cvm_proj,i,C[k]-1) * cvmGet(cvm_proj,i,C[k+1]-1);
					cvmSet(cvm_cross_terms,i,k/2,v);
				}
			}

			//***  new_dim = [proj_A,proj_A.^2,cross_terms];
			//*** %new_dim = [proj_A.^2,cross_terms];
			if(cvm_new_dim) cvReleaseMat(&cvm_new_dim);
			cvm_new_dim = newProjection(cvm_proj, cvm_cross_terms);

			free(C);
			if(cvm_cross_terms) cvReleaseMat(&cvm_cross_terms);
			if(cvm_proj) cvReleaseMat(&cvm_proj);
		}
		else
		{
			//***  new_dim = [proj_A,proj_A.^2];
			if(cvm_new_dim) cvReleaseMat(&cvm_new_dim);
			cvm_new_dim = newProjection(cvm_proj, NULL);
		}


		/*** val = [val,q_in];
		val = (sum(val'))';

		if DEBUG == 1
			val_all = [val_all,val];
		end
		*/
		//if(DEBUG) showMatrixValues(cvm_new_dim);
		for(unsigned int i=0; i<size; i++)
		{
			m_intensValues[(projCount-1)*size + i] = q_in[i]+m_intensValues[(projCount-2)*size + i];
		}
		if(q1) free(q1);
		if(q2) free(q2);
		if(q_in) free(q_in);

		//releasing local memory
		if(new_dim_used) cvReleaseMat(&new_dim_used);
		if(cvm_A_sq) cvReleaseMat(&cvm_A_sq);
		if(cvm_proj) cvReleaseMat(&cvm_proj);

		m_model->max_level++;
	}

	//releasing local memory
	if(cvm_X) 		  cvReleaseMat(&cvm_X);
	//if(cvm_proj_A)	  cvReleaseMat(&cvm_proj_A);
	if(cvm_proj_A_sq) cvReleaseMat(&cvm_proj_A_sq);
	if(cvm_A_sq) 	  cvReleaseMat(&cvm_A_sq);
	if(cvm_new_dim)	  cvReleaseMat(&cvm_new_dim);

	//if(q1) free(q1);
	//if(q2) free(q2);
	//if(q_in) free(q_in);

	return m_intensValues;
}
//-----------------------------------------------------------------------------
double *classifiers::polyMahalanobis::evaluateToCenter(double *im_data, unsigned int size, unsigned int dimensions)
{
	if(DEBUG) showInputData(im_data, size, dimensions);

	//evaluate im_data based on the center of a topological map
	//printf("\n\tclassifying to the center... wait...");
	//printf("\n...");

	double *refVector = (double*) calloc(dimensions, sizeof(double));
	for(unsigned int i=0; i<dimensions; i++)
		refVector[i]=m_model->m_center[i];

	double *m_intensValues = evaluateToVector(im_data, refVector, size, dimensions);

	free(refVector);
	if(DEBUG) showResultingData(m_intensValues, size, m_model->num_levels);

	return m_intensValues;
}
//-----------------------------------------------------------------------------
