#pragma once

#include <vector>
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

mat4 ComputeParallelMat(float l, float r, float b, float t, float n, float f)
{
	mat4 N = mat4(1);
	mat4 R = mat4(1);
	mat4 S = mat4(1);
	mat4 T = mat4(1);

	//rotation
	//R[2][2] = -1;

	//scale
	S[0][0] = 2 / (r - l);
	S[1][1] = 2 / (t - b);
	S[2][2] = 2 / (f - n);

	//translate
	T[0][3] = -(r + l) / 2;
	T[1][3] = -(t + b) / 2;
	T[2][3] = (f + n) / 2;

	N = R * S * T;
	return N;
}

mat4 ComputePerspectiveMat(float l, float r, float b, float t, float n, float f)
{
	mat4 N = mat4(1);
	mat4 R = mat4(1);
	mat4 S = mat4(1);
	mat4 T = mat4(1);

	//rotation
	R[2][2] = -1;

	//scale
	S[0][0] = 2 / (r - l);
	S[1][1] = 2 / (t - b);
	S[2][2] = 2 / (f - n);

	//translate
	T[0][3] = -(r + l) / 2;
	T[1][3] = -(t + b) / 2;
	T[2][3] = (f + n) / 2;

	N = R * S * T;
	return N;
}