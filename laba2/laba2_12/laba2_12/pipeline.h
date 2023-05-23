#ifndef PIPELINE_H
#define	PIPELINE_H

#include "math_3d.h"

//класс конвейера абстрагирует детали получения всех преобразований, необходимых для одного объекта. 
//Здесь представлены 3 private вектора, хранящих результат после каждой операции нахождения матрицы преобразованием. 
//Кроме того, представлен API для назначения данных и функция для получения итога всех преобразований.
class Pipeline
{
public:
	Pipeline()
	{
		m_scale = Vector3f(1.0f, 1.0f, 1.0f);
		m_worldPos = Vector3f(0.0f, 0.0f, 0.0f);
		m_rotateInfo = Vector3f(0.0f, 0.0f, 0.0f);
	}

	void Scale(float ScaleX, float ScaleY, float ScaleZ)
	{
		m_scale.x = ScaleX;
		m_scale.y = ScaleY;
		m_scale.z = ScaleZ;
	}

	void WorldPos(float x, float y, float z)
	{
		m_worldPos.x = x;
		m_worldPos.y = y;
		m_worldPos.z = z;
	}

	void Rotate(float RotateX, float RotateY, float RotateZ)
	{
		m_rotateInfo.x = RotateX;
		m_rotateInfo.y = RotateY;
		m_rotateInfo.z = RotateZ;
	}

	void SetPerspectiveProj(float FOV, float Width, float Height, float zNear, float zFar)
	{
		m_persProj.FOV = FOV;
		m_persProj.Width = Width;
		m_persProj.Height = Height;
		m_persProj.zNear = zNear;
		m_persProj.zFar = zFar;
	}

	const Matrix4f* GetTrans();

private:
	void InitScaleTransform(Matrix4f& m) const;
	void InitRotateTransform(Matrix4f& m) const;
	void InitTranslationTransform(Matrix4f& m) const;
	void InitPerspectiveProj(Matrix4f& m) const;

	Vector3f m_scale;
	Vector3f m_worldPos;
	Vector3f m_rotateInfo;

	//Структура, названная m_persProj, была добавлена в класс Pipeline и содержит параметры для проецирования перспективы.
	struct {
		float FOV;
		float Width;
		float Height;
		float zNear;
		float zFar;
	} m_persProj;

	Matrix4f m_transformation;
};


#endif	/* PIPELINE_H */