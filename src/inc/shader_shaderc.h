#ifndef SHADER_SHADERC_H
#define SHADER_SHADERC_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <filesystem>
#include <unordered_map>

#include <shaderc/shaderc.hpp>

class Shader {
public:
    unsigned int ID;

    Shader(const char* vertexPath, const char* fragmentPath, 
           const std::vector<std::pair<std::string, std::string>>& defines = {}) {
        std::string vertexCode = readFile(vertexPath);
        std::string fragmentCode = readFile(fragmentPath);

        std::string processedVertex = preprocessShader(vertexCode, shaderc_shader_kind::shaderc_glsl_vertex_shader, vertexPath, defines);
        std::string processedFragment = preprocessShader(fragmentCode, shaderc_shader_kind::shaderc_glsl_fragment_shader, fragmentPath, defines);

        const char* vShaderCode = processedVertex.c_str();
        const char* fShaderCode = processedFragment.c_str();

        unsigned int vertex, fragment;

        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX", vertexPath);

        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT", fragmentPath);

        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM", "LINK");

        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }

    void use() const {
        glUseProgram(ID);
    }

    void setBool(const std::string& name, bool value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }

    void setInt(const std::string& name, int value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }

    void setFloat(const std::string& name, float value) const {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }

    void setVec2(const std::string& name, const glm::vec2& value) const {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }

    void setVec2(const std::string& name, float x, float y) const {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
    }

    void setVec3(const std::string& name, const glm::vec3& value) const {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }

    void setVec3(const std::string& name, float x, float y, float z) const {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }

    void setVec4(const std::string& name, const glm::vec4& value) const {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }

    void setVec4(const std::string& name, float x, float y, float z, float w) const {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
    }

    void setMat2(const std::string& name, const glm::mat2& mat) const {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void setMat3(const std::string& name, const glm::mat3& mat) const {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void setMat4(const std::string& name, const glm::mat4& mat) const {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

private:
    shaderc::Compiler compiler;
    shaderc::CompileOptions options;
    std::string includePath;

    std::string readFile(const char* filePath) {
        std::string content;
        std::ifstream fileStream;

        fileStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        try {
            fileStream.open(filePath);
            std::stringstream stringStream;
            stringStream << fileStream.rdbuf();
            fileStream.close();
            content = stringStream.str();
            
            std::filesystem::path p(filePath);
            includePath = p.parent_path().string();
        }
        catch (std::ifstream::failure& e) {
            std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << filePath << std::endl;
        }
        return content;
    }

    std::string preprocessShader(const std::string& source, shaderc_shader_kind kind,
                                  const char* fileName,
                                  const std::vector<std::pair<std::string, std::string>>& defines) {
        shaderc::CompileOptions localOptions;
        
        localOptions.SetSourceLanguage(shaderc_source_language_glsl);
        localOptions.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
        
        for (const auto& define : defines) {
            localOptions.AddMacroDefinition(define.first, define.second);
        }

        class FileIncluder : public shaderc::CompileOptions::IncluderInterface {
        public:
            FileIncluder(const std::string& basePath) : baseDir(basePath) {}

            shaderc_include_result* GetInclude(const char* requested_source,
                                               shaderc_include_type type,
                                               const char* requesting_source,
                                               size_t include_depth) override {
                std::string fullPath;
                if (type == shaderc_include_type_relative) {
                    std::filesystem::path p(requesting_source);
                    fullPath = (p.parent_path() / requested_source).string();
                } else {
                    fullPath = (std::filesystem::path(baseDir) / requested_source).string();
                }

                auto it = includedFiles.find(fullPath);
                if (it != includedFiles.end()) {
                    return &it->second;
                }

                std::ifstream file(fullPath);
                if (!file.is_open()) {
                    static std::string error = "File not found: " + fullPath;
                    shaderc_include_result result{error.c_str(), error.size(), nullptr, 0, nullptr};
                    includedFiles[fullPath] = result;
                    return &includedFiles[fullPath];
                }

                std::stringstream buffer;
                buffer << file.rdbuf();
                std::string content = buffer.str();

                includedFiles[fullPath] = shaderc_include_result{
                    strdup(fullPath.c_str()), fullPath.size(),
                    strdup(content.c_str()), content.size(),
                    nullptr
                };
                return &includedFiles[fullPath];
            }

            void ReleaseInclude(shaderc_include_result* data) override {
                if (data->source_name) free(const_cast<char*>(data->source_name));
                if (data->content) free(const_cast<char*>(data->content));
            }

        private:
            std::string baseDir;
            std::unordered_map<std::string, shaderc_include_result> includedFiles;
        };

        localOptions.SetIncluder(std::make_unique<FileIncluder>(includePath));

        shaderc::PreprocessedSourceCompilationResult result = 
            compiler.PreprocessGlsl(source, kind, fileName, localOptions);

        if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
            std::cerr << "ERROR::SHADER::PREPROCESS_FAILED: " << fileName << std::endl;
            std::cerr << result.GetErrorMessage() << std::endl;
            return source;
        }

        return std::string(result.begin(), result.end());
    }

    void checkCompileErrors(unsigned int shader, const std::string& type, const std::string& name) {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM") {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type 
                          << " (" << name << ")\n" << infoLog 
                          << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << type 
                          << "\n" << infoLog 
                          << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }
};

#endif
