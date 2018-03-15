#ifndef AFFINE_H
#define AFFINE_H
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

/*
 * An affine matrix A can be factored as A = TL. You need to fill up two function named 'get_linear' and 'get_translation'
 */

// TODO: Fill up get_linear function
// input: A (4 x 4 matrix)
// output: L (4 x 4 matrix)
inline mat4 get_linear(mat4 a_A)
{
	mat4 L = mat4(1.0f);
	int i, j;
	for (i=0;i<3;i++){
		for (j = 0; j < 3; j++) {
			L[i][j] = a_A[i][j];
		}
	}

	return L;
}

// TODO: Fill up get_translation function
// input: A (4 x 4 matrix)
// output: T (4 x 4 matrix)
inline mat4 get_translation(mat4 a_A)
{
	mat4 T = mat4(1.0f);
	float x, y, z;
	x = a_A[3][0];
	y = a_A[3][1];
	z = a_A[3][2];
	T[3][0] = x;
	T[3][1] = y;
	T[3][2] = z;
	return T;
}
#endif
