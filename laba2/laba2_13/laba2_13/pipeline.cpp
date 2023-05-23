#include "pipeline.h"


const Matrix4f* Pipeline::GetTrans()
{
	Matrix4f ScaleTrans, RotateTrans, TranslationTrans, CameraTranslationTrans, CameraRotateTrans, PersProjTrans;

	//методы ниже позволяют поменять размер, положение и поворот фигуры, положение и поворот камеры.
	ScaleTrans.InitScaleTransform(m_scale.x, m_scale.y, m_scale.z);
	RotateTrans.InitRotateTransform(m_rotateInfo.x, m_rotateInfo.y, m_rotateInfo.z);
	TranslationTrans.InitTranslationTransform(m_worldPos.x, m_worldPos.y, m_worldPos.z);
	CameraTranslationTrans.InitTranslationTransform(-m_camera.Pos.x, -m_camera.Pos.y, -m_camera.Pos.z);
	CameraRotateTrans.InitCameraTransform(m_camera.Target, m_camera.Up);
	//Метод ниже генерирует матрицу перспективы, одна из компонентов, умножая которые получаем итоговое преобразование
	PersProjTrans.InitPersProjTransform(m_persProj.FOV, m_persProj.Width, m_persProj.Height, m_persProj.zNear, m_persProj.zFar);

	//Мы ставим матрицу перспективы первой в череде умножения матриц, которые генерирует итоговое преобразование.
//Вспомним, что вектор позиции умножается с правой стороны от итоговой матрицы. 
//Поэтому сначала масштабируем, затем вращаем, перемещаем и наконец проецируем.
	m_transformation = PersProjTrans * CameraRotateTrans * CameraTranslationTrans * TranslationTrans * RotateTrans * ScaleTrans;
	return &m_transformation;
}