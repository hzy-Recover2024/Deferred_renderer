/**
 * @file ScreenQuad.h
 * @brief 屏幕四边形网格
 * 
 * 用于全屏后处理效果的四边形网格。
 */

#ifndef SCREEN_QUAD_H
#define SCREEN_QUAD_H

#include <glad/glad.h>

/**
 * @brief 屏幕四边形类
 * 
 * 单例模式，提供全屏四边形渲染功能。
 * 用于延迟渲染的光照通道、后处理等。
 */
class ScreenQuad
{
public:
    static ScreenQuad& GetInstance()
    {
        static ScreenQuad instance;
        return instance;
    }

    void Render() const
    {
        glBindVertexArray(m_VAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
    }

    unsigned int GetVAO() const { return m_VAO; }

    ScreenQuad(const ScreenQuad&) = delete;
    ScreenQuad& operator=(const ScreenQuad&) = delete;

private:
    ScreenQuad() : m_VAO(0), m_VBO(0) { Init(); }

    ~ScreenQuad()
    {
        if (m_VAO != 0) glDeleteVertexArrays(1, &m_VAO);
        if (m_VBO != 0) glDeleteBuffers(1, &m_VBO);
    }

    void Init()
    {
        float vertices[] = {
            -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
             1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
             1.0f, -1.0f, 0.0f,  1.0f, 0.0f
        };

        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);

        glBindVertexArray(m_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

        glBindVertexArray(0);
    }

    unsigned int m_VAO;
    unsigned int m_VBO;
};

#endif
