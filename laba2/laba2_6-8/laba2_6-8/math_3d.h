#pragma once

#ifndef MATH_3D_H
#define	MATH_3D_H


struct Vector3f
{
	float x;
	float y;
	float z;

	Vector3f()
	{
	}

	Vector3f(float _x, float _y, float _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}
};

//������� ��������� ��� ������� 4 �� 4
struct Matrix4f
{
	float m[4][4];
};


#endif	/* MATH_3D_H */
