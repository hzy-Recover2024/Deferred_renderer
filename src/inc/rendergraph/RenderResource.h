#ifndef RENDER_RESOURCE_H
#define RENDER_RESOURCE_H

#include <string>

enum class ResourceType {
    Texture,
    Buffer,
    Framebuffer,
    Unknown
};

class RenderResource
{
public:
    std::string Name;
    ResourceType Type;
    unsigned int ID;
    unsigned int RefCount;

    RenderResource(const std::string& name, ResourceType type);
    virtual ~RenderResource() = default;

    virtual void Create() = 0;
    virtual void Destroy() = 0;
    virtual bool IsValid() const = 0;

    void AddRef() { ++RefCount; }
    void Release() { if (--RefCount == 0) Destroy(); }

protected:
    static unsigned int s_NextID;
};

#endif
