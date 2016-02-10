#include <iostream>
#include "pxl/pxl.hh"
#include <gsl/gsl_blas.h>

using namespace pxl;
using namespace std;
int main()
{
// BLAS example using the GNU Scientific Library and the PXL BasicMatrix
// In a similar way every other Blas compatible library can be used

// Computes, where the result is stored in a PXL BasicMatrix
// compare with http://www.gnu.org/software/gsl/manual/html_node/BLAS-Examples.html
//	[ 0.11 0.12 0.13 ]  [ 1011 1012 ]     [ 367.76 368.12 ]
//  [ 0.21 0.22 0.23 ]  [ 1021 1022 ]  =  [ 674.06 674.72 ]
//                      [ 1031 1032 ]


double a[] = { 0.11, 0.12, 0.13,
               0.21, 0.22, 0.23 };

double b[] = { 1011, 1012,
               1021, 1022,
               1031, 1032 };

BasicMatrix R(2,2);
gsl_matrix_view A = gsl_matrix_view_array(a, 2, 3);
gsl_matrix_view B = gsl_matrix_view_array(b, 3, 2);
gsl_matrix_view C = gsl_matrix_view_array(R.getArray(), 2, 2);
/* Compute C = A B */

gsl_blas_dgemm (CblasNoTrans, CblasNoTrans,
                1.0, &A.matrix, &B.matrix,
                0.0, &C.matrix);

for (int i=0;i<R.getSize1(); i++)
{
	for (int j=0;j<R.getSize2(); j++)
	{
		cout << R(i,j) << " ";
	}
	cout << endl;
}

}
