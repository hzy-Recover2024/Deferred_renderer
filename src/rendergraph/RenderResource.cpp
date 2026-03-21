#include "rendergraph/RenderResource.h"

unsigned int RenderResource::s_NextID = 0;

RenderResource::RenderResource(const std::string& name, ResourceType type)
    : Name(name), Type(type), RefCount(0)
{
    ID = s_NextID++;
}
