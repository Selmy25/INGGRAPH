#pragma once
#ifndef CAMERA_H
#define	CAMERA_H

#include "math_3d.h"


class Camera
{
public:
    // Устанавливает камеры по умолчанию в начале координат в сторону уменьшения z (как в шутерах)
    // Верхний вектор up устанавливается вверх (снова как в шутерах)
    Camera(int WindowWidth, int WindowHeight);

    // Позволяет настроить камеру самостоятельно
    Camera(int WindowWidth, int WindowHeight, const Vector3f& Pos, const Vector3f& Target, const Vector3f& Up);

    // Обрабатывает событие нажатия на клавиатуру и двигает камеру в соотвествии с нажатой кнопкой
    // Вперёд, назад, влево, вправо
    bool OnKeyboard(int Key);

    // Обрабатывает событие движения мыши. Принимает координаты мыши
    void OnMouse(int x, int y);

    // Рендер вращения когда указатель мыши на краю окна
    void OnRender();

    // Геттеры для трёх свойств
    //----------------------------------------------------------------------------------------------
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
    //----------------------------------------------------------------------------------------------

private:

    void Init(); // Устанавливает внутрение параметры камеры
    // для перерасчета векторов направления и вектора вверх,
    // основанных на новых горизонтальном и вертикальном углах
    void Update();

    Vector3f m_pos; // Позиция камеры
    Vector3f m_target; // Направление камеры
    Vector3f m_up; // Вертикальный вектор камеры

    int m_windowWidth; // Ширина окна
    int m_windowHeight; // Высота окна

    float m_AngleH; // Горизонтальный угол - для врадения камеры влево\вправо
    float m_AngleV; // Вертиакльный угол - для вращения камеры вверх\вниз

    // Флаги, указывающие находится ли указатель мыши на границе экрана
    bool m_OnUpperEdge;
    bool m_OnLowerEdge;
    bool m_OnLeftEdge;
    bool m_OnRightEdge;

    Vector2i m_mousePos; // Позиция мыши
};

#endif	/* CAMERA_H */
