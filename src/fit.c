#include "fit.h"

static int gaussianFunc(const gsl_vector *paramVec, void *dataStruct, gsl_vector *costFun);
static void iterCallBackGauss(const size_t iter, void *params,
			      const gsl_multifit_nlinear_workspace *workSpace);
static int gaussianDerivFunc(const gsl_vector *paramVec, void *dataStruct,
			     gsl_matrix *jacobianMatrix);


/******************************************************************************
 *
 * This is the source file to fit data for the FuZE experiment
 *
 ******************************************************************************/


/*
 * Declares the struct dataStruct for use in this file 
 */
struct dataStruct {
  size_t numPoints;
  double *yValues;
};


/******************************************************************************
 * Function: gaussianFunc
 * Inputs: gsl_vector *, void *, gsl_vector *
 * Returns: int
 * Description: This is the gaussian function to be fit
 ******************************************************************************/

static int gaussianFunc(const gsl_vector *paramVec, void *dataStruct, gsl_vector *costFun) {
  
  size_t numPoints = ((struct dataStruct *)dataStruct)->numPoints;
  double *yValues = ((struct dataStruct *)dataStruct)->yValues;

  double A = gsl_vector_get (paramVec, 0);
  double center = gsl_vector_get (paramVec, 1);
  double sigma = gsl_vector_get (paramVec, 2);
  double b = gsl_vector_get (paramVec, 3);

  size_t ii;
  double x, Yi, ex;

  /* This is weighted by the value of the data */
  for (ii = 0; ii < numPoints; ii++) {
    /* Model Yi = A * exp(-(i - i0)^2/sigma^2) + b */
    x = (double) ii;
    ex = gsl_pow_2(x-center)/gsl_pow_2(sigma);
    Yi = A * gsl_sf_exp(-ex) + b;
    gsl_vector_set (costFun, ii, (Yi - yValues[ii]));
      
    }

  return GSL_SUCCESS;
}


/******************************************************************************
 * Function: gaussianDerivFunc
 * Inputs: gsl_vector *, void *, gsl_vector *
 * Returns: int
 * Description: This is the derivative of the gaussian function to be fit
 ******************************************************************************/

static int gaussianDerivFunc(const gsl_vector *paramVec, void *dataStruct,
			     gsl_matrix *jacobianMatrix) {
  
  size_t n = ((struct dataStruct *)dataStruct)->numPoints;

  double A = gsl_vector_get(paramVec, 0);
  double center = gsl_vector_get(paramVec, 1);
  double sigma = gsl_vector_get(paramVec, 2);

  size_t ii;

  double val, ex, x;

  for (ii = 0; ii < n; ii++) {
    /* Jacobian matrix J(i,j) = dfi / dxj, */
    /* where fi = (Yi - yi)/sigma[i],      */
    /*       Yi = A * exp(-(x-center)^2/sigma^2 ) + b  */
    /* and the xj are the parameters (A,center, sigma,b) */

    x = (double) ii;
    ex = gsl_pow_2(x-center)/gsl_pow_2(sigma);
    val = gsl_sf_exp(-ex);
    gsl_matrix_set (jacobianMatrix, ii, 0, val); 
    gsl_matrix_set (jacobianMatrix, ii, 1, 2 * A/gsl_pow_2(sigma) * (x - center) * val);
    gsl_matrix_set (jacobianMatrix, ii, 2, 2 * A /gsl_pow_3(sigma) *gsl_pow_2(x-center) * val);
    gsl_matrix_set (jacobianMatrix, ii, 3, 1.0);
    }
  return GSL_SUCCESS;

}


/******************************************************************************
 * Function: iterCallBack
 * Inputs: size_t, void *, gsl_multifi_nlinear_workspace *
 * Returns: int
 * Description: This is called during the iteration process to print out informaton
 * for the Gaussian fit
 ******************************************************************************/

static void iterCallBackGauss(const size_t iter, void *params,
			      const gsl_multifit_nlinear_workspace *workSpace) {
  
  gsl_vector *f = gsl_multifit_nlinear_residual(workSpace);
  gsl_vector *x = gsl_multifit_nlinear_position(workSpace);
  double rcond;

  /* compute reciprocal condition number of J(x) */
  gsl_multifit_nlinear_rcond(&rcond, workSpace);

  fprintf(stderr, "iter %2zu: A = %.4f, center = %.4f, width = %.4f, b = %.4f",
          iter,
          gsl_vector_get(x, 0),
          gsl_vector_get(x, 1),
          gsl_vector_get(x, 2),
	  gsl_vector_get(x, 3));
  fprintf(stderr, "cond(J) = %8.4f, |f(x)| = %.4f\n",
          1.0 / rcond,
          gsl_blas_dnrm2(f));
}


/******************************************************************************
 * Function: fitGaussian
 * Inputs: gsl_vector *, gsl_vector *, double, double, double, double
 * Returns: int
 * Description: This will fit a gaussian to the passed x vs. y vectors and
 * set the amplitude and width parameters
 ******************************************************************************/

int fitGaussian (gsl_vector *xVec, gsl_vector *yVec, gsl_vector *gaussFit, double *amplitude,
		 double *center, double *width, double *offset, int printOption) {

  /* 
   * Specifies the type of algorhtym used to solve non linear least squares problem.
   * this is the only option. "Trust Region Method"
   */
  const gsl_multifit_nlinear_type *trustRegionMethod = gsl_multifit_nlinear_trust;
  gsl_multifit_nlinear_workspace *workSpace;
  gsl_multifit_nlinear_fdf fdf;
  gsl_multifit_nlinear_parameters fdf_params = gsl_multifit_nlinear_default_parameters();
  const size_t numPoints = yVec->size;
  const size_t numParameters = 4;

  /* Cost function */
  gsl_vector *costFun;

  /* Jacobian matrix */
  gsl_matrix *jacobMatrix;
  
  gsl_matrix *covar = gsl_matrix_alloc (numParameters, numParameters);
  double *yValues = yVec->data;
  double weights[numPoints];
  gsl_vector_view weightVector = gsl_vector_view_array(weights, numPoints);
  struct dataStruct dataStruct = { numPoints, yValues };

  double deltaX = gsl_vector_get(xVec,1)-gsl_vector_get(xVec,0),
    xInitial = gsl_vector_get(xVec, 0);
  
  /* Initial guess at parameters */
  double paramInit[numParameters];
  paramInit[0] = *amplitude;
  paramInit[1] = (*center-xInitial)/deltaX;
  paramInit[2] = *width/deltaX;
  paramInit[3] = *offset;

  
  gsl_vector_view paramInitVec = gsl_vector_view_array(paramInit, numParameters);
  double chisq, chisq0;
  int status, info;

  const double xtol = 1e-8;
  const double gtol = 1e-8;
  const double ftol = 0.0;

  /* define the function to be minimized */
  fdf.f = gaussianFunc;
  //fdf.df = NULL;             /* set to NULL for finite-difference Jacobian */
  fdf.df = gaussianDerivFunc;  /* set to NULL for finite-difference Jacobian */
  fdf.fvv = NULL;              /* not using geodesic acceleration */
  fdf.n = numPoints;
  fdf.p = numParameters;
  fdf.params = &dataStruct;


  /* Setting weights */
  int ii;
  double sigmai;
  for (ii = 0; ii < numPoints; ii++) {
    //sigmai = yValues[ii];
    sigmai = 1;
    gsl_vector_set(&weightVector.vector, ii, 1/(sigmai));
  }
  
  /* allocate workspace with default parameters */
  workSpace = gsl_multifit_nlinear_alloc (trustRegionMethod, &fdf_params, numPoints, numParameters);

  /* initialize solver with starting point and weights */
  gsl_multifit_nlinear_winit(&paramInitVec.vector, &weightVector.vector, &fdf, workSpace);

  /* compute initial cost function */
  costFun = gsl_multifit_nlinear_residual(workSpace);
  gsl_blas_ddot(costFun, costFun, &chisq0);

  /* solve the system with a maximum of 50 iterations */
  if (printOption == 1) {
    
    status = gsl_multifit_nlinear_driver(50, xtol, gtol, ftol,
					 iterCallBackGauss, NULL, &info, workSpace);
  } else {
    status = gsl_multifit_nlinear_driver(50, xtol, gtol, ftol,
					 NULL, NULL, &info, workSpace);
  }

  /* compute covariance of best fit parameters */
  jacobMatrix = gsl_multifit_nlinear_jac(workSpace);
  gsl_multifit_nlinear_covar (jacobMatrix, 0.0, covar);

  /* compute final cost */
  gsl_blas_ddot(costFun, costFun, &chisq);

  *amplitude = gsl_vector_get(workSpace->x, 0);
  *center = gsl_vector_get(workSpace->x, 1);
  *width = gsl_vector_get(workSpace->x, 2);
  *offset = gsl_vector_get(workSpace->x, 3);

  double dof = numPoints - numParameters;
  double c = GSL_MAX_DBL(1, sqrt(chisq / dof));

  if (printOption == 1 ) {
    
    fprintf(stderr, "chisq/dof = %g\n", chisq / dof);

    fprintf (stderr, "A      = %.5f +/- %.5f\n", gsl_vector_get(workSpace->x, 0),
	     c*sqrt(gsl_matrix_get(covar,0,0)));
    fprintf (stderr, "center = %.5f +/- %.5f\n", gsl_vector_get(workSpace->x, 1),
	     c*sqrt(gsl_matrix_get(covar,1,1)));
    fprintf (stderr, "width  = %.5f +/- %.5f\n", gsl_vector_get(workSpace->x, 2),
	     c*sqrt(gsl_matrix_get(covar,2,2)));
    fprintf (stderr, "offset = %.5f +/- %.5f\n", gsl_vector_get(workSpace->x, 3),
	     c*sqrt(gsl_matrix_get(covar,3,3)));
    fprintf (stderr, "status = %s\n", gsl_strerror (status));

  }
  gsl_multifit_nlinear_free(workSpace);
  gsl_matrix_free(covar);

  /* Converting to the passed units */
  *width = *width * deltaX;
  *center = deltaX*(*center) + xInitial;

  for (ii = 0; ii < numPoints; ii++) {
    gsl_vector_set(gaussFit, ii,
		   *offset + *amplitude*\
		   gsl_sf_exp(-gsl_pow_2(gsl_vector_get(xVec, ii)-*center)/gsl_pow_2(*width)));
  }

  return 0;
  
}
