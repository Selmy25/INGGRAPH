#pragma once
#include <string>

#include <GL/glew.h>
#include <ImageMagick-6/Magick++.h>

class Texture
{
public:
    // Позиция текстуры (её типа: 1D, 2D, 3D) и имя файла
    Texture(GLenum TextureTarget, const std::string& FileName);

    Texture(GLenum TextureTarget);

    //-----------------------------------------------------------------------------------
    // Загрузка текстуры из файла
    bool Load();
    // Принимает размер буфера и указатель расположение картинки в памяти
    /*void Load(u32 BufferSize, void* pData);
    void Load(const std::string& Filename);*/
    //-----------------------------------------------------------------------------------

    // Загрузка строк
    void LoadRaw(int Width, int Height, int BPP, unsigned char* pData);

    // Привязка к текстурному модулю
    void Bind(GLenum TextureUnit);

    // Получение размера картинки
    void GetImageSize(int& ImageWidth, int& ImageHeight)
    {
        ImageWidth = m_imageWidth;
        ImageHeight = m_imageHeight;
    }

private:
    void LoadInternal(void* image_data);

    std::string m_fileName; // Имя файла
    GLenum m_textureTarget; // Грубо говоря мерность текстуры
    GLuint m_textureObj; // Данные о самом изображении текстуры: тексели
    int m_imageWidth = 0;
    int m_imageHeight = 0;
    int m_imageBPP = 0;

    //Magick::Image* m_pImage; // Указатель на изображение (указатель на точку начала данных в памяти)
    //// большой бинарный объект - это полезный механизм для хранения
    //// зашифрованного изображения в память так,
    //// что оно может быть использовано сторонними программами
    //Magick::Blob m_blob;
};
