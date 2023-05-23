#include <GL/freeglut.h>
#include "camera.h"

const static float StepScale = 0.1f; // Размер шага
const static int MARGIN = 10; // Граница экрана

Camera::Camera(int WindowWidth, int WindowHeight)
{
    m_windowWidth = WindowWidth;
    m_windowHeight = WindowHeight;
    m_pos = Vector3f(0.0f, 0.0f, 0.0f); // Позиция в начале координат
    m_target = Vector3f(0.0f, 0.0f, 1.0f); // Направление в сторону убывающей z
    m_up = Vector3f(0.0f, 1.0f, 0.0f); // Вверхний вектор смотрит вверх по оси y
    m_target.Normalize();

    Init();
}


Camera::Camera(int WindowWidth, int WindowHeight, const Vector3f& Pos, const Vector3f& Target, const Vector3f& Up)
{
    m_windowWidth = WindowWidth;
    m_windowHeight = WindowHeight;
    m_pos = Pos;

    m_target = Target;
    m_target.Normalize(); // Нормализация вектора

    m_up = Up;
    m_up.Normalize(); // Нормализация вектора

    Init();
}

void Camera::Init()
{
    // Проекция вектора на плоскость xz
    // Вектор направления по горизантали. Нужен для вращения тела влево\вправо
    Vector3f HTarget(m_target.x, 0.0, m_target.z);
    HTarget.Normalize(); // Нормализуем вектор

    // Проверка квантернионов: проверяем в какой из четвертинок в координатной плоскости лежит векто
    // исходя и этого регулеруем горизонтальный угол
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

    // Подсчитываем вертикальный угол
    // Так как тут вращение только до 90 градусов, замарачиваться не нужно
    // Минус нужен, чтоб не вращался в противоположную от движения мыши сторону
    m_AngleV = -ToDegree(asin(m_target.y));

    // Проверка курсора, лежит ли он на крае экрана
    m_OnUpperEdge = false;
    m_OnLowerEdge = false;
    m_OnLeftEdge = false;
    m_OnRightEdge = false;
    // Настройка позиции мыши в середине экрана
    m_mousePos.x = m_windowWidth / 2;
    m_mousePos.y = m_windowHeight / 2;

    // Функция glut перемещает курсор на указанные нами координаты (центр экрана)
    glutWarpPointer(m_mousePos.x, m_mousePos.y);
}

bool Camera::OnKeyboard(int Key)
{
    bool Ret = false; // Флаг - кнопка для движения камеры не нажата

    // Проверка нажатой кнопки на клавиатуре
    switch (Key) {

    case GLUT_KEY_UP: // Камеры движется вперёд
    {
        // Вектор позиции изменяется по направлению вектора target на велечину StepScale
        m_pos += (m_target * StepScale);
        Ret = true;
    }
    break;

    case GLUT_KEY_DOWN: // Камера движется назад
    {
        // позиция изменяется обратно движению вперёд
        m_pos -= (m_target * StepScale); 
        Ret = true;
    }
    break;

    case GLUT_KEY_LEFT: // Камера движется влево
    {
        Vector3f Left = m_target.Cross(m_up); // Вычисления вектор, направленного влево
        Left.Normalize(); // Приведение его к нормальному виду
        Left *= StepScale; // Изменение величины вектора на StepScale
        m_pos += Left; // Изменение позиции влево на StepScale
        Ret = true;
    }
    break;

    case GLUT_KEY_RIGHT: // Камера движется влево
    {
        // Аналогично движению влево
        Vector3f Right = m_up.Cross(m_target);
        Right.Normalize();
        Right *= StepScale;
        m_pos += Right;
        Ret = true;
    }
    break;
    }

    return Ret;
}

// Обрабатывает движение мыши
void Camera::OnMouse(int x, int y)
{
    // Высчитывается изменение координат мыши
    const int DeltaX = x - m_mousePos.x;
    const int DeltaY = y - m_mousePos.y;

    // Новые координаты фиксируются как текущие
    m_mousePos.x = x;
    m_mousePos.y = y;

    // Расчитывается горизонтальный и вертикальный углы
    // 20.0f нужен для скорости изменения
    m_AngleH += (float)DeltaX / 20.0f;
    m_AngleV += (float)DeltaY / 20.0f;

    // Обновляем флаги границ экрана согласно положению курсора
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

    // Оновляем векторы направления и вверх
    Update();
}

// вызвывается из главного цикла рендера
// Необходима, когда мышь не двигается, но находится около одной из границ экрана
// Мышь не двигается, но экран будет двигаться
void Camera::OnRender()
{
    bool ShouldUpdate = false;

    // Проверяем какой флаг положения курсора активен
    // И в соответствии с ним меняем горизонтальный и/или вертикальный углы
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

    // Если углы изменились, пересчитываем вектора направления и вверх
    if (ShouldUpdate) {
        Update();
    }
}

// Пересчёт векторов
// Всё делается согласно квантерниону (вращение вектора вокруг другого)
void Camera::Update()
{
    // Устанавливаем вертикальный вектор (ось), вокруг которого будет вращаться вектор
    const Vector3f Vaxis(0.0f, 1.0f, 0.0f);

    // Rotate the view vector by the horizontal angle around the vertical axis
    Vector3f View(1.0f, 0.0f, 0.0f); // Вектор обзора - параллелен земле и направо
    View.Rotate(m_AngleH, Vaxis); // врощаем его вокруг оси на угол m_AngelH
    View.Normalize(); // Приводим к нормальному

    // Rotate the view vector by the vertical angle around the horizontal axis
    Vector3f Haxis = Vaxis.Cross(View); // Находим новую горизонтальную ось
    Haxis.Normalize(); // Нормализуем
    View.Rotate(m_AngleV, Haxis); // Вращаем вектор обзора вогруг уже горизонтальной оси

    m_target = View; // Впектор направления = вектору обзора
    m_target.Normalize(); // нормализуем

    m_up = m_target.Cross(Haxis); // Находим новый вектор вверх горизонтальная ось * вектор направления
    m_up.Normalize(); // нормализуем
}