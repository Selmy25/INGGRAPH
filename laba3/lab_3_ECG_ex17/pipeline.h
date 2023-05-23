#pragma once
#ifndef PIPELINE_H
#define	PIPELINE_H

#include "math_3d.h"

class Pipeline
{
public:
    // Конструктор, настраивающий векторы по умолчанию
    Pipeline()
    {
        m_scale = Vector3f(1.0f, 1.0f, 1.0f);
        m_worldPos = Vector3f(0.0f, 0.0f, 0.0f);
        m_rotateInfo = Vector3f(0.0f, 0.0f, 0.0f);
    }

    // Настройка масштаба
    void Scale(float ScaleX, float ScaleY, float ScaleZ)
    {
        m_scale.x = ScaleX;
        m_scale.y = ScaleY;
        m_scale.z = ScaleZ;
    }

    // Позиция в виртуальном мире
    void WorldPos(float x, float y, float z)
    {
        m_worldPos.x = x;
        m_worldPos.y = y;
        m_worldPos.z = z;
    }

    // Настройка вращения
    void Rotate(float RotateX, float RotateY, float RotateZ)
    {
        m_rotateInfo.x = RotateX;
        m_rotateInfo.y = RotateY;
        m_rotateInfo.z = RotateZ;
    }

    // Настройка проекции перспективы
    void SetPerspectiveProj(float FOV, float Width, float Height, float zNear, float zFar)
    {
        m_persProj.FOV = FOV;
        m_persProj.Width = Width;
        m_persProj.Height = Height;
        m_persProj.zNear = zNear;
        m_persProj.zFar = zFar;
    }

    // Настройка камеры
    void SetCamera(const Vector3f& Pos, const Vector3f& Target, const Vector3f& Up)
    {
        m_camera.Pos = Pos;
        m_camera.Target = Target;
        m_camera.Up = Up;
    }

    const Matrix4f* GetTrans();

private:

    Vector3f m_scale; // Масштаб
    Vector3f m_worldPos; // Позиция
    Vector3f m_rotateInfo; // Вращение

    // Структура для проекции перспективы
    struct {
        float FOV;
        float Width;
        float Height;
        float zNear;
        float zFar;
    } m_persProj;

    // Структура описывающая камеру
    struct {
        Vector3f Pos;
        Vector3f Target;
        Vector3f Up;
    } m_camera;

    Matrix4f m_transformation;
};


#endif	/* PIPELINE_H */