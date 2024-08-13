/* 
 * File:   polyMahalanobis.h
 * Author: asobieranski
 *
 * Created on January 21, 2009, 3:27 PM
 * Refactored on August 26, 2009, 23:10 PM
 *    --> converting the functions evaluate and evaluateP2P to c++ insted of matlab
 *
 * Refactored on July 15, 2010, 17:00 PM
 *    --> converting the makeSpace to c++ insted of matlab
 */

#ifndef _POLYMAHALANOBIS_H
#define	_POLYMAHALANOBIS_H

#include <iostream>
#include <malloc.h>
#include <string.h>
#include <cv.h>

#include "pattern.h"

#define DEBUG 0

#ifdef DEBUG
#define PRINT_DEBUG(format, args...)            \
  printf("\033[34;40m"format"\033[0m", ##args);
#else
#define PRINT_DEBUG(format, args...)
#endif

namespace classifiers
{

  //polynomial basis member variables
  class lev_basis
  {
  public:
    //these two are obsolete !!!!
    //int H, W;
    //float **A_basis;

    CvMat *cvm_A_basis;
    float max_aP;
    int ind_usesize;
    unsigned int *ind_use;

    int d_proj;
    int dmssize;
    float *dms;
	
    float sigma_inv;
    lev_basis *next;
	
  lev_basis() : cvm_A_basis(NULL), ind_use(NULL), dms(NULL), next(NULL) {};
  };
  //-----------------------------------------------------------------------------
  //polynomial Model
  class polyModel
  {
  public:
    //the center of space
    float *m_center;
	
    //size of polynomial terms projections
    int num_levels;
	
    //the maximum level achieved by the projections
    int max_level;
	
    //initial dimension based on pattern
    unsigned int num_initialdim;
	
    //lev basis
    lev_basis *levBegin;
  
  public:
    //constructor
  polyModel() : m_center(NULL), levBegin(NULL) {};

    //destructor
    ~polyModel() {
      int i=0;
      lev_basis *p=levBegin;
      while(p)
      {
        lev_basis *pfree = p;
        p=p->next;
			
        //dealocating vectors
        cvReleaseMat(&pfree->cvm_A_basis);
			
        free(pfree->ind_use);
        free(pfree->dms);
        free(pfree);
			
        printf("\nLevel %d disalocated !", ++i);
      }
      if(m_center) 
        free(m_center);
		
      printf("\n");
    };
  };
  //-----------------------------------------------------------------------------
  class polyMahalanobis
  {
  private:
    //an instance of a polynomial model
    polyModel *m_model;
	
    //an instance of a pattern 
    pattern *m_pattern;
	
    //just a factorial calculation
    float fat(int n) { if(n==0) return 1; else return n*fat(n-1); }
    
    //calc a mean of a d-dimensional vector at each component
    float *calc_mean(doubleVector *data, unsigned int size, unsigned int d);
	
    //get the maximum value from a given vector
    float getMaxValue(float *in, unsigned int size);
	
    //get the maximum value from a given vector
    float getMaxAbsValue(float *in, unsigned int size);

    //similar to find in matlab
    //find in a vetor values equal to zero (0), lower (-1) or bigger than (1), and return an allocated array of indexes
    unsigned int *find_eq(int opt, float *in, unsigned int size, unsigned int &lenght);
	
    //similar to nchoosek in matlab 
    unsigned int *nchoosek(unsigned int proj, unsigned int &size);
	
    //compute next projection
    CvMat *newProjection(CvMat *A, CvMat *cross);

    //compute the variance of a CvMat column, if c==-1, all columns are a single vector
    float calcVariance(CvMat *A, int column);
    //compute the variance of each CvMat column, and return in var_dim
    void calcVariance(CvMat *A, float *var_dim);
    //remove null dimensions, e.g.lower than a variance error, and return a new one
    CvMat *removeNullDimensions(CvMat *A, unsigned int *ind_use);
    //fits the matrix A only to the ind_use vector
    CvMat *removeNullIndexes(CvMat *A, unsigned int *ind_use, unsigned int size);
	
    // *** DEBUG FLAGS ***
    void showLevBasis();
    void showMatrixValues(CvMat *A);
    void showInputData(double *im_data, unsigned int size, unsigned int dimensions);
    void showResultingData(double *res_data, unsigned int size, unsigned int levels);
  
  public:
    //constructor and destructor
    polyMahalanobis();  
    ~polyMahalanobis();
	
    //*** training step ***
    //seting a pattern structure
    bool setPattern(pattern *_pattern);
    //making a space and storing in 
    bool makeSpace(unsigned int order);

    //*** testing points or list of points ***
    //compare an im_data array of double's with size N to the center of the space
    //for the both cases it returns a double vector of dimension q-order.

    //evaluate functions
    double *evaluateToCenter(double *im_data, unsigned int size, unsigned int dimensions);
    double *evaluateToVector(double *im_data, double *refVector, unsigned int size, unsigned int dimensions);
	

        
    //get the maximum polynomial order of a created space
    int getMaxqOrder() {return m_model->num_levels;}

    //get the center of the space
    float *getCenter() { return m_model->m_center; }

    //is the number of levels accomplished ?
    inline bool isSampled() { return (m_model->num_levels > -1) ? 1 : 0; };
	
    bool hasPattern() { return (m_pattern) ? true : false; }
  };       
  //-----------------------------------------------------------------------------
};

#endif	/* _POLYMAHALANOBIS_H */

