#include "material.hpp"

#include "../asset-loader.hpp"
#include "deserialize-utils.hpp"

namespace our
{

    // This function should setup the pipeline state and set the shader to be used
    void Material::setup() const
    {
        // TODO: (Req 7) Write this function [DONE]
        pipelineState.setup(); // Calling the pipeline setup
        shader->use();         // Setting which shader to use
    }

    // This function read the material data from a json object
    void Material::deserialize(const nlohmann::json &data)
    {
        if (!data.is_object())
            return;

        if (data.contains("pipelineState"))
        {
            pipelineState.deserialize(data["pipelineState"]);
        }
        shader = AssetLoader<ShaderProgram>::get(data["shader"].get<std::string>());
        transparent = data.value("transparent", false);

        // if (data.contains("light"))       // deserializing the light object if it exists
        // {
        //     light->deserialize(data["light"]);
        // }
    }

    // This function should call the setup of its parent and
    // set the "tint" uniform to the value in the member variable tint
    void TintedMaterial::setup() const
    {
        // TODO: (Req 7) Write this function [DONE]
        Material::setup();         // Calling material setup
        shader->set("tint", tint); // setting the uniform value of tint
    }

    // This function read the material data from a json object
    void TintedMaterial::deserialize(const nlohmann::json &data)
    {
        Material::deserialize(data);
        if (!data.is_object())
            return;
        tint = data.value("tint", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    }

    // This function should call the setup of its parent and
    // set the "alphaThreshold" uniform to the value in the member variable alphaThreshold
    // Then it should bind the texture and sampler to a texture unit and send the unit number to the uniform variable "tex"
    void TexturedMaterial::setup() const
    {
        // TODO: (Req 7) Write this function [DONE]
        TintedMaterial::setup();                       // Calling the tinted material setup
        shader->set("alphaThreshold", alphaThreshold); // setting the alphathreshold uniform value
        glActiveTexture(GL_TEXTURE0);
        if (texture != nullptr)
            texture->bind(); // binding the texture
        else
            Texture2D::unbind();
        if (sampler != nullptr)
            sampler->bind(0);
        else
            Sampler::unbind(0);
        shader->set("tex", 0); // setting the tex uniform value
    }

    // This function read the material data from a json object
    void TexturedMaterial::deserialize(const nlohmann::json &data)
    {
        TintedMaterial::deserialize(data);
        if (!data.is_object())
            return;
        alphaThreshold = data.value("alphaThreshold", 0.0f);
        texture = AssetLoader<Texture2D>::get(data.value("texture", ""));
        sampler = AssetLoader<Sampler>::get(data.value("sampler", ""));
    }

    void LitMaterial::setup() const
    {
        TexturedMaterial::setup();
        // glActiveTexture(GL_TEXTURE0);
        // if (texture)
        //     texture->bind();
        // else
        //     Texture2D::unbind();
        // if (sampler)
        //     sampler->bind(0);
        // else
        //     Sampler::unbind(0);
        // shader->set("tex", 0);

        glActiveTexture(GL_TEXTURE1);
        if (albedoMap)
            albedoMap->bind();
        else
            Texture2D::unbind();
        if (albedoMapSampler)
            albedoMapSampler->bind(1);
        else
            Sampler::unbind(1);
        shader->set("material.albedo_map", 1);

        glActiveTexture(GL_TEXTURE2);
        if (specularMap)
            specularMap->bind();
        else
            Texture2D::unbind();
        if (specularMapSampler)
            specularMapSampler->bind(2);
        else
            Sampler::unbind(2);
        shader->set("material.specular_map", 2);

        glActiveTexture(GL_TEXTURE3);
        if (roughnessMap)
            roughnessMap->bind();
        else
            Texture2D::unbind();
        if (roughnessMapSampler)
            roughnessMapSampler->bind(3);
        else
            Sampler::unbind(3);
        shader->set("material.roughness_map", 3);

        glActiveTexture(GL_TEXTURE4);
        if (emissiveMap)
            emissiveMap->bind();
        else
            Texture2D::unbind();
        if (emissiveMapSampler)
            emissiveMapSampler->bind(4);
        else
            Sampler::unbind(4);
        shader->set("material.emissive_map", 4);

        glActiveTexture(GL_TEXTURE5);
        if (aoMap)
            aoMap->bind();
        else
            Texture2D::unbind();
        if (aoMapSampler)
            aoMapSampler->bind(5);
        else
            Sampler::unbind(5);
        shader->set("material.ambient_occlusion_map", 5);

    }

    // This function read the LitMaterial data from a json object
    void LitMaterial::deserialize(const nlohmann::json &data)
    {
        TexturedMaterial::deserialize(data);
        if (!data.is_object())
            return;
        albedoMap = AssetLoader<Texture2D>::get(data.value("albedoMap", "white"));
        specularMap = AssetLoader<Texture2D>::get(data.value("specularMap", "black"));
        roughnessMap = AssetLoader<Texture2D>::get(data.value("roughnessMap", "black"));
        emissiveMap = AssetLoader<Texture2D>::get(data.value("emissiveMap", "black"));
        aoMap = AssetLoader<Texture2D>::get(data.value("aoMap", "white"));
    }

}