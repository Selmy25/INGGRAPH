#pragma once
#ifndef CAMERA_H
#define	CAMERA_H

#include "math_3d.h"

//Класс хранит 3 свойства, которые характеризуют камеру - позиция, направление и верхний вектор. 
//Так же добавлены 2 конструктора. По умолчанию просто располагает камеру в начале координат, направляет ее в сторону уменьшения Z,
//а верхний вектор устремляет в "небо" (0,1,0). Но есть возможность создать камеру с указанием значений атрибутов
class Camera
{
public:

    Camera(int WindowWidth, int WindowHeight);

    Camera(int WindowWidth, int WindowHeight, const Vector3f& Pos, const Vector3f& Target, const Vector3f& Up);

    /*OnKeyboard() доставляет события клавиатуры в класс.Он возвращает значение типа bool, который указывает, воспринято ли камерой событие.
    Если клавиша подходит(одна из указывающих направление), возвращаемое значение - true.Если нет - false.*/
    bool OnKeyboard(int Key);

    void OnMouse(int x, int y);

    void OnRender();

    const Vector3f& GetPos() const
    {
        return m_pos;
    }

    const Vector3f& GetTarget() const
    {
        return m_target;
    }

    const Vector3f& GetUp() const
    {
        return m_up;
    }

private:

    void Init();
    void Update();

    Vector3f m_pos;
    Vector3f m_target;
    Vector3f m_up;

    int m_windowWidth;
    int m_windowHeight;

    float m_AngleH;
    float m_AngleV;

    bool m_OnUpperEdge;
    bool m_OnLowerEdge;
    bool m_OnLeftEdge;
    bool m_OnRightEdge;

    Vector2i m_mousePos;
};

#endif	/* CAMERA_H */
