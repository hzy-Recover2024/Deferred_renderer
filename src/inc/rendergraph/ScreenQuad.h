/**
 * @file ScreenQuad.h
 * @brief 屏幕四边形网格类定义 / Screen quad mesh class definition
 * 
 * 用于全屏后处理效果的四边形网格。
 * Quad mesh for fullscreen post-processing effects.
 */

#ifndef SCREEN_QUAD_H
#define SCREEN_QUAD_H

#include <glad/glad.h>

/**
 * @brief 屏幕四边形类 / Screen quad class
 * 
 * 单例模式，提供全屏四边形渲染功能。
 * Singleton pattern, provides fullscreen quad rendering functionality.
 * 
 * 用于延迟渲染的光照通道、后处理等。
 * Used for deferred rendering lighting pass, post-processing, etc.
 */
class ScreenQuad
{
public:
    /**
     * @brief 获取单例实例 / Get singleton instance
     * @return ScreenQuad& 实例引用 / Instance reference
     */
    static ScreenQuad& GetInstance()
    {
        static ScreenQuad instance;
        return instance;
    }

    /**
     * @brief 渲染全屏四边形 / Render fullscreen quad
     */
    void Render() const
    {
        glBindVertexArray(m_VAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
    }

    /**
     * @brief 获取 VAO / Get VAO
     * @return VAO ID
     */
    unsigned int GetVAO() const { return m_VAO; }

    /// 禁止拷贝 / Disable copy
    ScreenQuad(const ScreenQuad&) = delete;
    ScreenQuad& operator=(const ScreenQuad&) = delete;

private:
    ScreenQuad() : m_VAO(0), m_VBO(0)
    {
        Init();
    }

    ~ScreenQuad()
    {
        if (m_VAO != 0)
        {
            glDeleteVertexArrays(1, &m_VAO);
            m_VAO = 0;
        }
        if (m_VBO != 0)
        {
            glDeleteBuffers(1, &m_VBO);
            m_VBO = 0;
        }
    }

    void Init()
    {
        /// 四边形顶点数据：位置 + 纹理坐标 / Quad vertex data: position + texcoord
        float vertices[] = {
            // position          // texcoord
            -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,  // 左上 / top-left
            -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,  // 左下 / bottom-left
             1.0f,  1.0f, 0.0f,  1.0f, 1.0f,  // 右上 / top-right
             1.0f, -1.0f, 0.0f,  1.0f, 0.0f   // 右下 / bottom-right
        };

        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);

        glBindVertexArray(m_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        /// 位置属性 / Position attribute
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

        /// 纹理坐标属性 / Texcoord attribute
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

        glBindVertexArray(0);
    }

    unsigned int m_VAO;  ///< 顶点数组对象 / Vertex array object
    unsigned int m_VBO;  ///< 顶点缓冲对象 / Vertex buffer object
};

#endif
