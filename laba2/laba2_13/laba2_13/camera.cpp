#include <GL/freeglut.h>
const double M_PI = 3.14159;


#include "camera.h"

const static float STEP_SCALE = 0.1f;
const static int MARGIN = 10;

Camera::Camera(int WindowWidth, int WindowHeight)
{
    m_windowWidth = WindowWidth;
    m_windowHeight = WindowHeight;
    m_pos = Vector3f(0.0f, 0.0f, 0.0f);
    m_target = Vector3f(0.0f, 0.0f, 1.0f);
    m_target.Normalize();
    m_up = Vector3f(0.0f, 1.0f, 0.0f);

    Init();
}

//Конструктор камеры теперь принимает размеры окна.  Нам это потребуется для перемещения курсора в центр экрана.
Camera::Camera(int WindowWidth, int WindowHeight, const Vector3f& Pos, const Vector3f& Target, const Vector3f& Up)
{
    m_windowWidth = WindowWidth;
    m_windowHeight = WindowHeight;
    m_pos = Pos;

    m_target = Target;
    m_target.Normalize();

    m_up = Up;
    m_up.Normalize();

    Init(); //установит внутренние параметры камеры.
}


void Camera::Init()
{
    Vector3f HTarget(m_target.x, 0.0, m_target.z); //создаем новый вектор, названый HTarget (направление по горизонтали), который является проекцией исходного вектора направления на плоскость XZ.
    HTarget.Normalize();  //нормируем его

    // Затем мы проверяем какой кватернион соответствует вектору для конечного подсчета значения координаты Z. 
    if (HTarget.z >= 0.0f)
    {
        if (HTarget.x >= 0.0f)
        {
            m_AngleH = 360.0f - ToDegree(asin(HTarget.z));
        }
        else
        {
            m_AngleH = 180.0f + ToDegree(asin(HTarget.z));
        }
    }
    else
    {
        if (HTarget.x >= 0.0f)
        {
            m_AngleH = ToDegree(asin(-HTarget.z));
        }
        else
        {
            m_AngleH = 90.0f + ToDegree(asin(-HTarget.z));
        }
    }

    m_AngleV = -ToDegree(asin(m_target.y));      //Далее мы подсчитываем вертикальный угол

    //У камеры появилось 4 новых параметра для проверки не касается ли курсор границ экрана. 
    //Мы собираемся добавить автоматическое вращение в этом случае, это позволит нам поворачиваться на все 360 градусов. 
    //Мы назначаем все флаги в false, так как курсор в начале в центре экрана
    m_OnUpperEdge = false;
    m_OnLowerEdge = false;
    m_OnLeftEdge = false;
    m_OnRightEdge = false;
    //вычисляем центр экрана (основывается на разрешении экрана), и новая функция glutWarpPointer перемещает курсор.
    m_mousePos.x = m_windowWidth / 2;
    m_mousePos.y = m_windowHeight / 2;

    glutWarpPointer(m_mousePos.x, m_mousePos.y);
}

//Эта функция двигает камеру согласно событиям клавиатуры.

bool Camera::OnKeyboard(int Key)
{
    bool Ret = false;

    switch (Key) {
        /*Движение вперед и назад - это движение вдоль вектора направления,
        нам нужно только прибавить или вычесть вектор направления из вектора позиции.
            Вектор направления остается без изменений.Прежде чем прибавлять / вычитать вектор направления,
            мы умножаем его на константу названую 'StepSize' (размер шага).Мы делаем это для всех направлений.
            Размер шага позволяет удобно изменять скорость*/
    case GLUT_KEY_UP:
    {
        m_pos += (m_target * STEP_SCALE);
        Ret = true;
    }
    break;

    case GLUT_KEY_DOWN:
    {
        m_pos -= (m_target * STEP_SCALE);
        Ret = true;
    }
    break;

    //Перемещение в бок происходит вдоль вектора, перпендикулярного плоскости, 
    //    которую образуют вектор направления и вектор вверх.Плоскость делит трехмерное пространство на 2 части и у нас 2 вектора,
    //    перпендикулярных этой плоскости, и они противоположно направлены.Один мы можем назвать левым, а другой правым.
    //    Они получаются векторным произведением вектора направления и вектора вверх.В зависимости от порядка векторов, 
    //    получаются соответственно левый или правый вектора.После получения нужного вектора, мы нормируем его и увеличиваем на размер шага и, 
    //    наконец, прибавляем к вектору позиции(что совершит перемещение влево или вправо).И снова вектора вверх и направления остались без изменений.
    case GLUT_KEY_LEFT:
    {
        Vector3f Left = m_target.Cross(m_up);
        Left.Normalize();
        Left *= STEP_SCALE;
        m_pos += Left;
        Ret = true;
    }
    break;

    case GLUT_KEY_RIGHT:
    {
        Vector3f Right = m_up.Cross(m_target);
        Right.Normalize();
        Right *= STEP_SCALE;
        m_pos += Right;
        Ret = true;
    }
    break;
    }

    return Ret;
}

//функция используется что бы сообщить камере, что положение мыши изменилось.
void Camera::OnMouse(int x, int y)
{
    //Входящие параметры - это новые координаты. Мы начинаем с подсчета разницы между новыми координатами и предыдущими по осям и X и Y.
    const int DeltaX = x - m_mousePos.x;
    const int DeltaY = y - m_mousePos.y;
    //записываем новые значения для следующих вызовов функции.
    m_mousePos.x = x;
    m_mousePos.y = y;
    //обновляем текущие горизонтальные и вертикальные углы на эту разность в значениях.
    m_AngleH += (float)DeltaX / 20.0f;  //значение 20.0 мможно изменить для получения удобного движения
    m_AngleV += (float)DeltaY / 20.0f;

    //После проверки мы обновляем значения 'm_OnEdge' согласно положению курсора. 
    //Граница по умолчанию равна 10 пикселям, и триггер сработает, если мышь будет достаточна близка к границе.
    if (DeltaX == 0) {
        if (x <= MARGIN) {
            //    m_AngleH -= 1.0f;
            m_OnLeftEdge = true;
        }
        else if (x >= (m_windowWidth - MARGIN)) {
            //    m_AngleH += 1.0f;
            m_OnRightEdge = true;
        }
    }
    else {
        m_OnLeftEdge = false;
        m_OnRightEdge = false;
    }

    if (DeltaY == 0) {
        if (y <= MARGIN) {
            m_OnUpperEdge = true;
        }
        else if (y >= (m_windowHeight - MARGIN)) {
            m_OnLowerEdge = true;
        }
    }
    else {
        m_OnUpperEdge = false;
        m_OnLowerEdge = false;
    }
    //ызываем *Update() для перерасчета векторов направления и вектора вверх, основанных на новых горизонтальном и вертикальном углах.
    Update();
}

//функция вызывается из главного цикла рендера. Она нам необходима для случаев, когда мышь не движется, но находится около одной из границ экрана.
//В этом случае мышь не будет передавать событий, но мы все еще хотим, что бы камера вращалась (до тех пор, пока курсор не отведут от края экрана).
void Camera::OnRender()
{
    //Мы проверяем не установлен ли хоть один из флагов, и если таковой найден, то изменится один из углов. 
    //В этом случае будет вызвана функция Update() для обновления векторов камеры. 
    //Если мышь не находится в окне, то нам передается это событие, и все флаги снимаются.
    bool ShouldUpdate = false;

    if (m_OnLeftEdge) {
        m_AngleH -= 0.1f;
        ShouldUpdate = true;
    }
    else if (m_OnRightEdge) {
        m_AngleH += 0.1f;
        ShouldUpdate = true;
    }

    if (m_OnUpperEdge) {
        if (m_AngleV > -90.0f) {
            m_AngleV -= 0.1f;
            ShouldUpdate = true;
        }
    }
    else if (m_OnLowerEdge) {
        if (m_AngleV < 90.0f) {
            m_AngleV += 0.1f;
            ShouldUpdate = true;
        }
    }

    if (ShouldUpdate) {
        Update();
    }
    //Заметим, что вертикальный угол находится в пределах от -90 градусов и до +90 градусов. 
    //Это необходимо для запрета полного круга, когда мы наклоняемся вверх или вниз.
}

//функция обновляет значения векторов направления и вверх согласно горизонтальному и вертикальному углам
void Camera::Update()
{
    //начинаем с вектором обзора в "сброшенном" состоянии. Это значит, что он параллелен земле и смотрит направо.
    const Vector3f Vaxis(0.0f, 1.0f, 0.0f);

    // Мы устанавливаем вертикальную ось прямо вверх и вращаем вектор направления на горизонтальный угол относительно нее.
    //В результате получаем вектор, который соответствует искомому, но имеет не правильную высоту 
    //Совершив векторное произведение между этим вектором и вертикальной осью, мы получаем еще один вектор на плоскости XZ,
    //но он будет перпендикулярен плоскости, образованной вертикальным вектором и вектором направления.
    //Это наша новая горизонтальная ось, и настал момент вращать вектор вокруг нее на вертикальный угол.

    Vector3f View(1.0f, 0.0f, 0.0f);
    View.Rotate(m_AngleH, Vaxis);
    View.Normalize();

    //Новый вектор подсчитывается просто векторным произведением итоговым вектором направления и новым вектором вправо.
    //Если вертикальный угол снова 0, тогда вектор направления возвращается на плоскость XZ, и вектор вверх обратно (0,1,0).
    //Если вектор направления движется вверх или вниз, то вектор вверх наклоняется вперед / назад соответственно.
    Vector3f Haxis = Vaxis.Cross(View);
    Haxis.Normalize();
    View.Rotate(m_AngleV, Haxis);

    m_target = View;
    m_target.Normalize();

    m_up = m_target.Cross(Haxis);
    m_up.Normalize();
}