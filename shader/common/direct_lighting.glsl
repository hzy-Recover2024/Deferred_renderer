/**
 * @file direct_lighting.glsl
 * @brief 直接光照 irradiance 计算 / Direct lighting irradiance calculations
 * 
 * 提供三种直接光源的 irradiance 计算函数（不包含 BRDF 和 cosTheta）：
 * Provides irradiance calculation functions for three types of direct lights
 * (excluding BRDF and cosTheta):
 * - Directional Light (平行光)
 * - Point Light (点光源)
 * - Spot Light (聚光灯)
 * 
 * irradiance = lightColor * lightIntensity * attenuation
 * 
 * 使用示例 / Usage example:
 * @code
 * #include "direct_lighting.glsl"
 * 
 * void main() {
 *     vec3 N = normalize(normal);
 *     vec3 V = normalize(viewPos - worldPos);
 *     vec3 L = normalize(lightPos - worldPos);
 *     
 *     // 获取 irradiance（使用色温）
 *     vec3 irradiance = getPointLightIrradiance(worldPos, lightPos, lightColor, lightIntensity, lightInvRadius, colorTemperature);
 *     
 *     // 自行计算 BRDF 和 cosTheta
 *     float NoL = max(dot(N, L), 0.0);
 *     vec3 brdf = yourBRDF(N, V, L);
 *     vec3 outgoing = brdf * irradiance * NoL;
 * }
 * @endcode
 */

#ifndef DIRECT_LIGHTING_GLSL
#define DIRECT_LIGHTING_GLSL

// ==================== 色温转换 / Color Temperature Conversion ====================

/**
 * @brief 色温转 RGB / Color temperature to RGB
 * 
 * 使用 Tanner Helland 算法将开尔文色温转换为 RGB 颜色。
 * Converts Kelvin color temperature to RGB using Tanner Helland's algorithm.
 * 
 * 色温参考 / Color temperature reference:
 * - 1800K: 烛光 / Candle
 * - 2700K: 钨丝灯 / Tungsten
 * - 4000K: 日光灯 / Fluorescent
 * - 5500K: 正午阳光 / Noon sunlight
 * - 6500K: 阴天 / Overcast
 * - 10000K+: 蓝天 / Clear blue sky
 * 
 * @param kelvin 色温（开尔文，1000-40000）/ Color temperature (Kelvin, 1000-40000)
 * @return vec3 RGB 颜色 / RGB color
 */
vec3 colorTemperatureToRGB(float kelvin)
{
    kelvin = clamp(kelvin, 1000.0, 40000.0) / 100.0;
    
    vec3 color;
    
    // Red / 红色通道
    if (kelvin <= 66.0)
    {
        color.r = 1.0;
    }
    else
    {
        color.r = 1.292936 * pow(kelvin - 60.0, -0.1332);
        color.r = clamp(color.r, 0.0, 1.0);
    }
    
    // Green / 绿色通道
    if (kelvin <= 66.0)
    {
        color.g = 0.390082 * log(kelvin) - 0.631841;
        color.g = clamp(color.g, 0.0, 1.0);
    }
    else
    {
        color.g = 1.292936 * pow(kelvin - 60.0, -0.0755);
        color.g = clamp(color.g, 0.0, 1.0);
    }
    
    // Blue / 蓝色通道
    if (kelvin >= 66.0)
    {
        color.b = 1.0;
    }
    else if (kelvin <= 19.0)
    {
        color.b = 0.0;
    }
    else
    {
        color.b = 0.543207 * log(kelvin - 10.0) - 1.196254;
        color.b = clamp(color.b, 0.0, 1.0);
    }
    
    return color;
}

/**
 * @brief 混合光源颜色和色温 / Blend light color with color temperature
 * 
 * 将用户定义的光源颜色与色温颜色相乘。
 * Multiplies user-defined light color with color temperature color.
 * 
 * @param lightColor 光源颜色 / Light color
 * @param colorTemperature 色温（开尔文）/ Color temperature (Kelvin)
 * @return vec3 混合后的颜色 / Blended color
 */
vec3 blendColorWithTemperature(vec3 lightColor, float colorTemperature)
{
    vec3 tempColor = colorTemperatureToRGB(colorTemperature);
    return lightColor * tempColor;
}

// ==================== 衰减函数 / Attenuation Functions ====================

/**
 * @brief 计算平方衰减 / Calculate square falloff attenuation
 * 
 * 使用平方衰减公式，在 lightRadius 处衰减为 0。
 * Uses square falloff formula, attenuation reaches 0 at lightRadius.
 * 
 * @param posToLight 光源到片元的向量 / Vector from fragment to light
 * @param lightInvRadius 光源影响半径的倒数 / Inverse of light influence radius
 * @return float 衰减值 / Attenuation value
 */
float getSquareFalloffAttenuation(vec3 posToLight, float lightInvRadius)
{
    float distanceSquare = dot(posToLight, posToLight);
    float factor = distanceSquare * lightInvRadius * lightInvRadius;
    float smoothFactor = max(1.0 - factor * factor, 0.0);
    return (smoothFactor * smoothFactor) / max(distanceSquare, 1e-4);
}

/**
 * @brief 计算聚光灯锥角衰减 / Calculate spot light cone attenuation
 * 
 * @param L 光照方向（指向光源）/ Light direction (towards light)
 * @param lightDir 聚光灯方向 / Spot light direction
 * @param innerAngle 内锥角（弧度）/ Inner cone angle (radians)
 * @param outerAngle 外锥角（弧度）/ Outer cone angle (radians)
 * @return float 锥角衰减值 / Cone attenuation value
 */
float getSpotAngleAttenuation(vec3 L, vec3 lightDir, float innerAngle, float outerAngle)
{
    float cosOuter = cos(outerAngle);
    float cosInner = cos(innerAngle);
    float spotScale = 1.0 / max(cosInner - cosOuter, 1e-4);
    float spotOffset = -cosOuter * spotScale;

    float cd = dot(normalize(-lightDir), L);
    float attenuation = clamp(cd * spotScale + spotOffset, 0.0, 1.0);
    return attenuation * attenuation;
}

// ==================== Irradiance 计算函数 / Irradiance Calculation Functions ====================

/**
 * @brief 计算平行光 irradiance / Calculate directional light irradiance
 * 
 * 平行光无衰减。
 * Directional light has no attenuation.
 * 
 * @param lightColor 光源颜色 / Light color
 * @param lightIntensity 光源强度 / Light intensity
 * @param colorTemperature 色温（开尔文，0 表示不使用色温）/ Color temperature (Kelvin, 0 to disable)
 * @return vec3 irradiance 值 / Irradiance value
 */
vec3 getDirectionalLightIrradiance(vec3 lightColor, float lightIntensity, float colorTemperature)
{
    vec3 color = (colorTemperature > 0.0) ? blendColorWithTemperature(lightColor, colorTemperature) : lightColor;
    return color * lightIntensity;
}

/**
 * @brief 计算点光源 irradiance / Calculate point light irradiance
 * 
 * @param worldPos 片元世界位置 / Fragment world position
 * @param lightPos 光源位置 / Light position
 * @param lightColor 光源颜色 / Light color
 * @param lightIntensity 光源强度 / Light intensity
 * @param lightInvRadius 光源影响半径的倒数 / Inverse of light influence radius
 * @param colorTemperature 色温（开尔文，0 表示不使用色温）/ Color temperature (Kelvin, 0 to disable)
 * @return vec3 irradiance 值 / Irradiance value
 */
vec3 getPointLightIrradiance(
    vec3 worldPos,
    vec3 lightPos,
    vec3 lightColor,
    float lightIntensity,
    float lightInvRadius,
    float colorTemperature)
{
    vec3 posToLight = lightPos - worldPos;
    float attenuation = getSquareFalloffAttenuation(posToLight, lightInvRadius);
    vec3 color = (colorTemperature > 0.0) ? blendColorWithTemperature(lightColor, colorTemperature) : lightColor;
    return color * lightIntensity * attenuation;
}

/**
 * @brief 计算聚光灯 irradiance / Calculate spot light irradiance
 * 
 * @param worldPos 片元世界位置 / Fragment world position
 * @param lightPos 光源位置 / Light position
 * @param lightDir 聚光灯方向 / Spot light direction
 * @param lightColor 光源颜色 / Light color
 * @param lightIntensity 光源强度 / Light intensity
 * @param lightInvRadius 光源影响半径的倒数 / Inverse of light influence radius
 * @param innerAngle 内锥角（弧度）/ Inner cone angle (radians)
 * @param outerAngle 外锥角（弧度）/ Outer cone angle (radians)
 * @param colorTemperature 色温（开尔文，0 表示不使用色温）/ Color temperature (Kelvin, 0 to disable)
 * @return vec3 irradiance 值 / Irradiance value
 */
vec3 getSpotLightIrradiance(
    vec3 worldPos,
    vec3 lightPos,
    vec3 lightDir,
    vec3 lightColor,
    float lightIntensity,
    float lightInvRadius,
    float innerAngle,
    float outerAngle,
    float colorTemperature)
{
    vec3 posToLight = lightPos - worldPos;
    vec3 L = normalize(posToLight);
    
    float distanceAttenuation = getSquareFalloffAttenuation(posToLight, lightInvRadius);
    float coneAttenuation = getSpotAngleAttenuation(L, lightDir, innerAngle, outerAngle);
    vec3 color = (colorTemperature > 0.0) ? blendColorWithTemperature(lightColor, colorTemperature) : lightColor;
    
    return color * lightIntensity * distanceAttenuation * coneAttenuation;
}

// ==================== 辅助函数 / Helper Functions ====================

/**
 * @brief 获取光源方向（指向光源）/ Get light direction (towards light)
 * 
 * @param worldPos 片元世界位置 / Fragment world position
 * @param lightPos 光源位置 / Light position
 * @return vec3 归一化的光照方向 / Normalized light direction
 */
vec3 getLightDirection(vec3 worldPos, vec3 lightPos)
{
    return normalize(lightPos - worldPos);
}

#endif
