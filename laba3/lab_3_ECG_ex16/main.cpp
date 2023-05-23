#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "math_3d.h"
#include "pipeline.h"
#include "camera.h"
#include "texture.h"

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

struct Vertex
{
    Vector3f m_pos; // Координата фигуры
    Vector2f m_tex; // Координата текстуры

    Vertex() {}

    Vertex(Vector3f pos, Vector2f tex)
    {
        m_pos = pos;
        m_tex = tex;
    }
};

// Буфер вершин
GLuint VBO;
// Ещё один буфер вершин
GLuint IBO;
// Система координат камеры
GLuint gWVPLocation;

GLuint gSampler; // Семплер для текстуры
Texture* pTexture = nullptr; // Указатель на начало данных текстуры

Camera* pGameCamera = nullptr; // Переменная, содержащая значения камеры

// Вершинный шейдер
static const char* pVS = "                                                          \n\
#version 330                                                                        \n\
                                                                                    \n\
layout (location = 0) in vec3 Position;                                             \n\
layout (location = 1) in vec2 TexCoord;                                             \n\
                                                                                    \n\
uniform mat4 gWVP;                                                                  \n\
                                                                                    \n\
out vec2 TexCoord0;                                                                 \n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
    gl_Position = gWVP * vec4(Position, 1.0);                                       \n\
    TexCoord0 = TexCoord;                                                           \n\
}";

// Фрагментный шейдер
static const char* pFS = "                                                          \n\
#version 330                                                                        \n\
                                                                                    \n\
in vec2 TexCoord0;                                                                  \n\
                                                                                    \n\
out vec4 FragColor;                                                                 \n\
                                                                                    \n\
uniform sampler2D gSampler;                                                         \n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
    FragColor = texture2D(gSampler, TexCoord0.xy);                                  \n\
}";


static void RenderSceneCB()
{
    pGameCamera->OnRender();

    glClear(GL_COLOR_BUFFER_BIT);

    static float Scale = 0.0f; // Масштаб

    Scale += 0.1f; // Изменяем для движения

    // Конвейер
    Pipeline p;
    p.Rotate(0.0f, Scale, 0.0f);
    p.WorldPos(0.0f, 0.0f, 3.0f);
    // Установка камеры
    p.SetCamera(pGameCamera->GetPos(), pGameCamera->GetTarget(), pGameCamera->GetUp());
    // Установка перпективы
    p.SetPerspectiveProj(60.0f, WINDOW_WIDTH, WINDOW_HEIGHT, 1.0f, 100.0f);

    glUniformMatrix4fv(gWVPLocation, 1, GL_TRUE, (const GLfloat*)p.GetTrans());

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)12);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    pTexture->Bind(GL_TEXTURE0);
    glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    glutSwapBuffers();
}

// Функция для обратного вызова, обрабатывающая нажатие клаивиши на клавиатуре
// Принимает занчение клавиши и координаты мыши в момент нажатия клавиши
static void SpecialKeyboardCB(int key, int x, int y)
{
    pGameCamera->OnKeyboard(key);
}

// Для завершения программы
static void KeyboardCB(unsigned char Key, int x, int y)
{
    switch (Key) {
    case 'q':
        glutLeaveMainLoop();
    }
}

// Функция обратного вызова, вызывающая метод OnMouse класса Camera
static void PassiveMouseCB(int x, int y)
{
    pGameCamera->OnMouse(x, y);
}

static void InitializeGlutCallbacks()
{
    // Показывает изображение на экран
    glutDisplayFunc(RenderSceneCB);
    // функция рендера, вызывающая только при получении определённого события
    glutIdleFunc(RenderSceneCB);
    // Регистрация новой функции обратного вызова для получения специальных событий клавиатуры
    glutSpecialFunc(SpecialKeyboardCB);
    // Регестрирует движение мыши
    glutPassiveMotionFunc(PassiveMouseCB);
    // Регестрирует нажатие клавиши
    glutKeyboardFunc(KeyboardCB);
}

// Создание вершинного буфера
static void CreateVertexBuffer()
{
    Vertex Vertices[4] = { Vertex(Vector3f(-1.0f, -1.0f, 0.5773f), Vector2f(0.0f, 0.0f)),
                           Vertex(Vector3f(0.0f, -1.0f, -1.15475), Vector2f(0.6f, 0.0f)),
                           Vertex(Vector3f(1.0f, -1.0f, 0.5773f),  Vector2f(1.1f, 0.0f)),
                           Vertex(Vector3f(0.0f, 1.0f, 0.0f),      Vector2f(0.6f, 1.1f)) };

    // Привязка буфера (буфер один) к переменной
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // Найстройка VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
}

// Создание буфера индексов
static void CreateIndexBuffer()
{
    unsigned int Indices[] = { 0, 3, 1,
                               1, 3, 2,
                               2, 3, 0,
                               1, 2, 0 };

    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);
}

// Добавление шейдеров
static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
    // Создание шейдера (тип GL_FRAGMENT_SHADER)
    GLuint ShaderObj = glCreateShader(ShaderType);

    // Проверка на корректность создания
    if (ShaderObj == 0) {
        fprintf(stderr, "Error creating shader type %d\n", ShaderType);
        exit(0);
    }

    // Настройка исходного кода шейдера
    const GLchar* p[1];
    p[0] = pShaderText;
    GLint Lengths[1];
    Lengths[0] = strlen(pShaderText);
    // Функция указывает исходный код шейдеру
    // 1 - шейдер
    // 2 - размерность массива
    // 3 - массив чаров
    // 4 - длина массива
    glShaderSource(ShaderObj, 1, p, Lengths);

    // Компиляция шейдера
    glCompileShader(ShaderObj);
    // Проверка статуса компиляции на ошибки 
    GLint success;
    glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar InfoLog[1024];
        glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
        fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
        exit(1);
    }

    // Соединяем скомпилированный объект шейдер ShaderObj с объектом программы ShaderProgram
    glAttachShader(ShaderProgram, ShaderObj);
}

// Компиляция шейдеров
static void CompileShaders()
{
    // Создание программного объекта
    GLuint ShaderProgram = glCreateProgram();

    // Проверка на корректность программы
    if (ShaderProgram == 0) {
        fprintf(stderr, "Error creating shader program\n");
        exit(1);
    }

    AddShader(ShaderProgram, pVS, GL_VERTEX_SHADER);
    AddShader(ShaderProgram, pFS, GL_FRAGMENT_SHADER);

    // Переменная для контроля успеха
    GLint Success = 0;
    // Массив для сообщения об ошибке
    GLchar ErrorLog[1024] = { 0 };

    // Линковка шейдеров
    glLinkProgram(ShaderProgram);
    // Проверка программных ошибок (например, ошибки линковки)
    glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);
    if (Success == 0) {
        glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
        exit(1);
    }

    // Проверка программы после линковки
    glValidateProgram(ShaderProgram);
    glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);
    if (!Success) {
        glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
        exit(1);
    }

    // Назначаем программу шейдеров для конвейера
    glUseProgram(ShaderProgram);

    // Запрашиваем позицию uniform-переменной
    gWVPLocation = glGetUniformLocation(ShaderProgram, "gWVP");
    assert(gWVPLocation != 0xFFFFFFFF);

    gSampler = glGetUniformLocation(ShaderProgram, "gSampler");
    assert(gSampler != 0xFFFFFFFF);
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(210, 20);
    glutCreateWindow("Main");
    //Разрешение и количество бит на пиксель
    //glutGameModeString("1024x768@32");
    //// Запускаться в полноэкранном режиме
    //glutEnterGameMode();

    InitializeGlutCallbacks();

    // Инициализация камеры
    pGameCamera = new Camera(WINDOW_WIDTH, WINDOW_HEIGHT);

    GLenum res = glewInit();
    if (res != GLEW_OK) {
        fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
        return 1;
    }

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    // Необязательный: для улучшения картинки
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    int texture_units = 0;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &texture_units);
    printf("Number of texture units %d\n", texture_units);

    CreateVertexBuffer();
    CreateIndexBuffer();

    CompileShaders();

    // Устанавливаем индексы модулей текстуры
    glUniform1i(gSampler, 0);

    // Открываем изображение
    pTexture = new Texture(GL_TEXTURE_2D, "test3.jpg");

    // Загружаем изображение в память программы
    if (!pTexture->Load()) {
        return 1;
    }

    glutMainLoop();

    return 0;
}