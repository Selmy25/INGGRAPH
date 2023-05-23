#include <iostream>
#include "texture.h"
// Для вывода ошибки и упрощения типа переменной
#include "util.h"
// Библиотеки для загрузки фото в GLUT
#include "3rdparty/stb_image.h"
#include "3rdparty/stb_image_write.h"


Texture::Texture(GLenum TextureTarget, const std::string& FileName)
{
    m_textureTarget = TextureTarget;
    m_fileName = FileName;
}

Texture::Texture(GLenum TextureTarget)
{
    m_textureTarget = TextureTarget;
}

//void Texture::Load(u32 BufferSize, void* pData)
//{
//    void* image_data = stbi_load_from_memory((const stbi_uc*)pData, BufferSize, &m_imageWidth, &m_imageHeight, &m_imageBPP, 0);
//
//    LoadInternal(image_data);
//
//    stbi_image_free(image_data);
//}

// Загрузка изображения
bool Texture::Load()
{
    // Порядок загрузки, OpenGL загружает с низу вверх
    stbi_set_flip_vertically_on_load(1);
    // Загрузка текстуры из файла и передача указателя на начало текстуры в памяти
    unsigned char* image_data = stbi_load(m_fileName.c_str(), &m_imageWidth, &m_imageHeight, &m_imageBPP, 0);

    if (!image_data) {
        printf("Can't load texture from '%s' - %s\n", m_fileName.c_str(), stbi_failure_reason());
        exit(0);
    }

    printf("Width %d, height %d, bpp %d\n", m_imageWidth, m_imageHeight, m_imageBPP);

    LoadInternal(image_data);

    stbi_image_free(image_data);

    return true;
}

//void Texture::Load(const std::string& Filename)
//{
//    m_fileName = Filename;
//
//    if (!Load()) {
//        exit(0);
//    }
//}

void Texture::LoadRaw(int Width, int Height, int BPP, unsigned char* pData)
{
    m_imageWidth = Width;
    m_imageHeight = Height;
    m_imageBPP = BPP;

    LoadInternal(pData);
}

// Загрузка текстуры в glut
void Texture::LoadInternal(void* image_data)
{
    // Генерирование текстуры объекта
    glGenTextures(1, &m_textureObj);
    // Свзяываем объект текстуры с целью текстуры (её размерностью)
    glBindTexture(m_textureTarget, m_textureObj);

    if (m_textureTarget == GL_TEXTURE_2D) {
        switch (m_imageBPP) {
        case 1:
            glTexImage2D(m_textureTarget, 0, GL_RED, m_imageWidth, m_imageHeight, 0, GL_RED, GL_UNSIGNED_BYTE, image_data);
            break;

        case 3:
            glTexImage2D(m_textureTarget, 0, GL_RGB, m_imageWidth, m_imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
            break;

        case 4:
            glTexImage2D(m_textureTarget, 0, GL_RGBA, m_imageWidth, m_imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
            break;

        default:
            NOT_IMPLEMENTED;
        }
    }
    else
    {
        printf("Support for texture target %x is not implemented\n", m_textureTarget);
        exit(1);
    }

    // Найстройка филтров
    glTexParameteri(m_textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(m_textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(m_textureTarget, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(m_textureTarget, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(m_textureTarget, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glGenerateMipmap(m_textureTarget);

    glBindTexture(m_textureTarget, 0);
}

// функция принимает модуль текстуры как параметр типа enum (GL_TEXTURE0, GL_TEXTURE1 и т.д.).
// Тем самым он станет активным через glActiveTexture() и затем привязываем объект текстур к модулю
void Texture::Bind(GLenum TextureUnit)
{
    glActiveTexture(TextureUnit);
    glBindTexture(m_textureTarget, m_textureObj);
}