#include "forward-renderer.hpp"
#include "../mesh/mesh-utils.hpp"
#include "../texture/texture-utils.hpp"
#include <iostream>
#include <glm/glm.hpp>

namespace our
{

    void ForwardRenderer::initialize(glm::ivec2 windowSize, const nlohmann::json &config)
    {
        // First, we store the window size for later use
        this->windowSize = windowSize;

        // Then we check if there is a sky texture in the configuration
        if (config.contains("sky"))
        {
            // First, we create a sphere which will be used to draw the sky
            this->skySphere = mesh_utils::sphere(glm::ivec2(16, 16));

            // We can draw the sky using the same shader used to draw textured objects
            ShaderProgram *skyShader = new ShaderProgram();
            skyShader->attach("assets/shaders/textured.vert", GL_VERTEX_SHADER);
            skyShader->attach("assets/shaders/textured.frag", GL_FRAGMENT_SHADER);
            skyShader->link();

            // TODO: (Req 10) Pick the correct pipeline state to draw the sky    [DONE]
            //  Hints: the sky will be draw after the opaque objects so we would need depth testing but which depth funtion should we pick?
            //  We will draw the sphere from the inside, so what options should we pick for the face culling.
            PipelineState skyPipelineState{};
            skyPipelineState.depthTesting.enabled = true;
            skyPipelineState.depthTesting.function = GL_LEQUAL;
            skyPipelineState.faceCulling.enabled = true;
            skyPipelineState.faceCulling.culledFace = GL_FRONT;
            skyPipelineState.faceCulling.frontFace = GL_CCW;

            // Load the sky texture (note that we don't need mipmaps since we want to avoid any unnecessary blurring while rendering the sky)
            std::string skyTextureFile = config.value<std::string>("sky", "");
            Texture2D *skyTexture = texture_utils::loadImage(skyTextureFile, false);

            // Setup a sampler for the sky
            Sampler *skySampler = new Sampler();
            skySampler->set(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            skySampler->set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            skySampler->set(GL_TEXTURE_WRAP_S, GL_REPEAT);
            skySampler->set(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            // Combine all the aforementioned objects (except the mesh) into a material
            this->skyMaterial = new TexturedMaterial();
            this->skyMaterial->shader = skyShader;
            this->skyMaterial->texture = skyTexture;
            this->skyMaterial->sampler = skySampler;
            this->skyMaterial->pipelineState = skyPipelineState;
            this->skyMaterial->tint = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            this->skyMaterial->alphaThreshold = 1.0f;
            this->skyMaterial->transparent = false;
        }

        // Then we check if there is a postprocessing shader in the configuration
        if (config.contains("postprocess"))
        {
            // TODO: (Req 11) Create a framebuffer   [DONE]
            glGenFramebuffers(1, &postprocessFrameBuffer);
            auto fboStatus2 = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            if (fboStatus2 != GL_FRAMEBUFFER_COMPLETE)
                std::cout << "\n\n\n\n\n\nFramebuffer0 not complete: " << fboStatus2 << "\n\n\n\n\n"
                          << std::endl;
            glBindFramebuffer(GL_FRAMEBUFFER, postprocessFrameBuffer);

            // TODO: (Req 11) Create a color and a depth texture and attach them to the framebuffer  [DONE]
            //  Hints: The color format can be (Red, Green, Blue and Alpha components with 8 bits for each channel).
            //  The depth format can be (Depth component with 24 bits).

            // glTexImage2D(GL_TEXTURE_2D, 0, format, size.x, size.y, 0, format, GL_UNSIGNED_BYTE, nullptr);
            colorTarget = texture_utils::empty(GL_RGBA, windowSize);
            depthTarget = texture_utils::empty(GL_DEPTH_COMPONENT, windowSize);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTarget->getOpenGLName(), 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTarget->getOpenGLName(), 0);

            // glTexStorage2D(GL_TEXTURE_2D, rt_levels, GL_RGBA8, rt_size.x, rt_size.y);
            // glTexImage2D(GL_TEXTURE_2D, 0, format, size.x, size.y, 0, format, GL_UNSIGNED_BYTE, nullptr);
            // TODO: (Req 11) Unbind the framebuffer just to be safe [DONE]

            // glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postprocessFrameBuffer);
            auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
                std::cout << "\n\n\n\n\n\nFramebuffer1 not complete: " << fboStatus << "\n\n\n\n\n"
                          << std::endl;
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            // Create a vertex array to use for drawing the texture
            glGenVertexArrays(1, &postProcessVertexArray);

            // Create a sampler to use for sampling the scene texture in the post processing shader
            Sampler *postprocessSampler = new Sampler();
            postprocessSampler->set(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            postprocessSampler->set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            postprocessSampler->set(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            postprocessSampler->set(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            // Create the post processing shader
            ShaderProgram *postprocessShader = new ShaderProgram();
            postprocessShader->attach("assets/shaders/fullscreen.vert", GL_VERTEX_SHADER);
            postprocessShader->attach(config.value<std::string>("postprocess", ""), GL_FRAGMENT_SHADER);
            postprocessShader->link();

            // Create a post processing material
            postprocessMaterial = new TexturedMaterial();
            postprocessMaterial->shader = postprocessShader;
            postprocessMaterial->texture = colorTarget;
            postprocessMaterial->sampler = postprocessSampler;
            // The default options are fine but we don't need to interact with the depth buffer
            // so it is more performant to disable the depth mask
            postprocessMaterial->pipelineState.depthMask = false;
        }
    }

    void ForwardRenderer::destroy()
    {
        // Delete all objects related to the sky
        if (skyMaterial)
        {
            delete skySphere;
            delete skyMaterial->shader;
            delete skyMaterial->texture;
            delete skyMaterial->sampler;
            delete skyMaterial;
        }
        // Delete all objects related to post processing
        if (postprocessMaterial)
        {
            glDeleteFramebuffers(1, &postprocessFrameBuffer);
            glDeleteVertexArrays(1, &postProcessVertexArray);
            delete colorTarget;
            delete depthTarget;
            delete postprocessMaterial->sampler;
            delete postprocessMaterial->shader;
            delete postprocessMaterial;
        }
    }

    void ForwardRenderer::render(World *world)
    {
        // First of all, we search for a camera and for all the mesh renderers
        CameraComponent *camera = nullptr;
        opaqueCommands.clear();
        transparentCommands.clear();
        lights.clear();

        for (auto entity : world->getEntities())
        {
            // If we hadn't found a camera yet, we look for a camera in this entity
            if (!camera)
                camera = entity->getComponent<CameraComponent>();
            // If this entity has a mesh renderer component
            if (auto meshRenderer = entity->getComponent<MeshRendererComponent>(); meshRenderer)
            {
                // We construct a command from it
                RenderCommand command;
                command.localToWorld = meshRenderer->getOwner()->getLocalToWorldMatrix();
                command.center = glm::vec3(command.localToWorld * glm::vec4(0, 0, 0, 1));
                command.mesh = meshRenderer->mesh;
                command.material = meshRenderer->material;
                // if it is transparent, we add it to the transparent commands list

                if (command.material->transparent)
                {
                    transparentCommands.push_back(command);
                }
                else
                {
                    // Otherwise, we add it to the opaque command list
                    opaqueCommands.push_back(command);
                }
            }
            else if (auto lightComponent = entity->getComponent<LightComponent>(); lightComponent)
            {
                lights.push_back(lightComponent);
            }
        }
        // if(lights[0].type == our::LightType::SPOT)
        // std::cout << "spottt" << std::endl;
        // If there is no camera, we return (we cannot render without a camera)
        if (camera == nullptr)
            return;

        // TODO: (Req 9) Modify the following line such that "cameraForward" contains a vector pointing the camera forward direction [DONE]
        //  HINT: See how you wrote the CameraComponent::getViewMatrix, it should help you solve this one

        glm::vec3 cameraForward = camera->getOwner()->getLocalToWorldMatrix() * glm::vec4(0.0, 0.0, -1.0f, 0.0f);
        std::sort(transparentCommands.begin(), transparentCommands.end(), [cameraForward](const RenderCommand &first, const RenderCommand &second)
                  {
            //TODO: (Req 9) Finish this function    [DONE]
            // HINT: the following return should return true "first" should be drawn before "second". 
            if (dot(cameraForward,first.center) > dot(cameraForward, second.center)) return true;
            return false; });

        // TODO: (Req 9) Get the camera ViewProjection matrix and store it in VP  [DONE]
        glm::mat4 VP = camera->getProjectionMatrix(windowSize) * camera->getViewMatrix();

        // TODO: (Req 9) Set the OpenGL viewport using viewportStart and viewportSize    [DONE]
        glViewport(0, 0, windowSize.x, windowSize.y);

        // TODO: (Req 9) Set the clear color to black and the clear depth to 1   [DONE]
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClearDepth(1.0f);

        // TODO: (Req 9) Set the color mask to true and the depth mask to true (to ensure the glClear will affect the framebuffer)   [DONE]
        glColorMask(true, true, true, true);
        glDepthMask(true);

        // If there is a postprocess material, bind the framebuffer
        if (postprocessMaterial)
        {
            // TODO: (Req 11) bind the framebuffer   [DONE]

            auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
                std::cout << "\n\n\n\n\n\nFramebuffer not complete: " << fboStatus << "\n\n\n\n\n"
                          << std::endl;
            else
            {
                glBindFramebuffer(GL_FRAMEBUFFER, postprocessFrameBuffer);
            }
        }

        // TODO: (Req 9) Clear the color and depth buffers   [DONE]
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_DEPTH_BUFFER_BIT);

        // TODO: (Req 9) Draw all the opaque commands    [DONE]
        //  Don't forget to set the "transform" uniform to be equal the model-view-projection matrix for each render command

        for (auto command : opaqueCommands)
        {
            // setting up material and some other set ups
            command.material->setup();  
            command.material->shader->set("object_to_world", command.localToWorld);
            command.material->shader->set("object_to_world_inv_transpose", glm::transpose(glm::inverse(command.localToWorld)));
            glm::vec4 eye = camera->getOwner()->getLocalToWorldMatrix() * glm::vec4(0, 0, 0, 1);    // (0,0,0) position of camera in its local space, 1 for point
            command.material->shader->set("camera_position", glm::vec3(eye));
            command.material->shader->set("light_count", (int)lights.size());
            command.material->shader->set("view_projection", VP );

            // looping on lights (We will use the 1st approach (single pass forward lighting))
            for (int i = 0; i < lights.size(); ++i)
            {
                
                std::string prefix = "lights[" + std::to_string(i) + "].";

                command.material->shader->set(prefix + "type", static_cast<int>(lights[i]->type));
                command.material->shader->set(prefix + "diffuse", lights[i]->diffuse);
                command.material->shader->set(prefix + "ambient", lights[i]->ambient);
                command.material->shader->set(prefix + "specular", lights[i]->specular);

                // Checking the light type, to set its values
                switch (lights[i]->type)
                {
                case LightType::DIRECTIONAL:
                    // (0,-1,0) light is coming from upside, and we convert it to world space
                    command.material->shader->set(prefix + "direction", glm::vec3(lights[i]->getOwner()->getLocalToWorldMatrix()*glm::vec4(0.0,-1.0,0.0,0.0)));
                    break;
                    case LightType::POINT:
                        // converting position of light to world space
                        command.material->shader->set(prefix + "position", glm::vec3(lights[i]->getOwner()->getLocalToWorldMatrix() * glm::vec4(lights[i]->getOwner()->localTransform.position, 0.0f)));
                        command.material->shader->set(prefix + "direction", glm::vec3(lights[i]->getOwner()->getLocalToWorldMatrix()*glm::vec4(0.0,-1.0,0.0,0.0)));
                        command.material->shader->set(prefix + "attenuation_constant", lights[i]->attenuation.constant);
                        command.material->shader->set(prefix + "attenuation_linear", lights[i]->attenuation.linear);
                        command.material->shader->set(prefix + "attenuation_quadratic", lights[i]->attenuation.quadratic);
                        break;
                    case LightType::SPOT:
                        command.material->shader->set(prefix + "position", glm::vec3(lights[i]->getOwner()->getLocalToWorldMatrix() * glm::vec4(lights[i]->getOwner()->localTransform.position, 0.0f)));
                        command.material->shader->set(prefix + "direction", glm::vec3(lights[i]->getOwner()->getLocalToWorldMatrix()*glm::vec4(0.0,-1.0,0.0,0.0)));
                        command.material->shader->set(prefix + "attenuation_constant", lights[i]->attenuation.constant);
                        command.material->shader->set(prefix + "attenuation_linear", lights[i]->attenuation.linear);
                        command.material->shader->set(prefix + "attenuation_quadratic", lights[i]->attenuation.quadratic);
                        command.material->shader->set(prefix + "inner_angle", lights[i]->spot_angle.inner);
                        command.material->shader->set(prefix + "outer_angle", lights[i]->spot_angle.outer);
                        break;
                }
                if (i >= 16)
                    break;
            }

            
            command.mesh->draw();
        }

        // If there is a sky material, draw the sky
        if (this->skyMaterial)
        {
            // TODO: (Req 10) setup the sky material [DONE]
            skyMaterial->setup();
            // TODO: (Req 10) Get the camera position    [DONE]
            glm::vec3 cameraPosition = camera->getOwner()->getLocalToWorldMatrix() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

            // TODO: (Req 10) Create a model matrix for the sky such that it always follows the camera (sky sphere center = camera position) [DONE]
            glm::mat4 skyModel = glm::mat4(
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                cameraPosition.x, cameraPosition.y, cameraPosition.z, 1.0f);
            // TODO: (Req 10) We want the sky to be drawn behind everything (in NDC space, z=1)  [DONE]
            //  We can acheive the is by multiplying by an extra matrix after the projection but what values should we put in it?
            glm::mat4 alwaysBehindTransform = glm::mat4(
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 1.0f);

            // TODO: (Req 10) set the "transform" uniform    [DONE]
            skyMaterial->shader->set("transform", alwaysBehindTransform * VP * skyModel);
            // TODO: (Req 10) draw the sky sphere    [DONE]
            skySphere->draw();
        }

        // TODO: (Req 9) Draw all the transparent commands   [DONE]
        //  Don't forget to set the "transform" uniform to be equal the model-view-projection matrix for each render command
        for (auto command : transparentCommands)
        {
            command.material->setup();
            command.material->shader->set("object_to_world", command.localToWorld);
            command.material->shader->set("object_to_world_inv_transpose", glm::transpose(glm::inverse(command.localToWorld)));
            glm::vec4 eye = camera->getOwner()->getLocalToWorldMatrix() * glm::vec4(0, 0, 0, 1);
            command.material->shader->set("camera_position", glm::vec3(eye));
            command.material->shader->set("light_count", (int)lights.size());
            command.material->shader->set("view_projection", VP );

            for (int i = 0; i < lights.size(); ++i)
            {
                std::string prefix = "lights[" + std::to_string(i) + "].";

                command.material->shader->set(prefix + "type", static_cast<int>(lights[i]->type));
                command.material->shader->set(prefix + "diffuse", lights[i]->diffuse);
                command.material->shader->set(prefix + "ambient", lights[i]->ambient);
                command.material->shader->set(prefix + "specular", lights[i]->specular);

                switch (lights[i]->type)
                {
                case LightType::DIRECTIONAL:
                    command.material->shader->set(prefix + "direction", glm::normalize(glm::vec3(lights[i]->getOwner()->getLocalToWorldMatrix()*glm::vec4(0.0,-1.0,0.0,0.0))));
                    break;
                    case LightType::POINT:
                        command.material->shader->set(prefix + "position", glm::vec3(lights[i]->getOwner()->getLocalToWorldMatrix() * glm::vec4(lights[i]->getOwner()->localTransform.position, 0.0f)));
                        command.material->shader->set(prefix + "direction", glm::vec3(lights[i]->getOwner()->getLocalToWorldMatrix()*glm::vec4(0.0,-1.0,0.0,0.0)));
                        command.material->shader->set(prefix + "attenuation_constant", lights[i]->attenuation.constant);
                        command.material->shader->set(prefix + "attenuation_linear", lights[i]->attenuation.linear);
                        command.material->shader->set(prefix + "attenuation_quadratic", lights[i]->attenuation.quadratic);
                        break;
                    case LightType::SPOT:
                        command.material->shader->set(prefix + "position", glm::vec3(lights[i]->getOwner()->getLocalToWorldMatrix() * glm::vec4(lights[i]->getOwner()->localTransform.position, 0.0f)));
                        command.material->shader->set(prefix + "direction", glm::vec3(lights[i]->getOwner()->getLocalToWorldMatrix()*glm::vec4(0.0,-1.0,0.0,0.0)));
                        command.material->shader->set(prefix + "attenuation_constant", lights[i]->attenuation.constant);
                        command.material->shader->set(prefix + "attenuation_linear", lights[i]->attenuation.linear);
                        command.material->shader->set(prefix + "attenuation_quadratic", lights[i]->attenuation.quadratic);
                        command.material->shader->set(prefix + "inner_angle", lights[i]->spot_angle.inner);
                        command.material->shader->set(prefix + "outer_angle", lights[i]->spot_angle.outer);
                        break;
                }
                if (i >= 16)
                    break;
            }

            
            command.mesh->draw();
        }

        // If there is a postprocess material, apply postprocessing
        if (postprocessMaterial)
        {
            // TODO: (Req 11) Return to the default framebuffer  [DONE]
            auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
                std::cout << "\n\n\n\n\n\nFramebuffer3 not complete: " << fboStatus << "\n\n\n\n\n"
                          << std::endl;
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            // TODO: (Req 11) Setup the postprocess material and draw the fullscreen triangle    [DONE]
            postprocessMaterial->setup();
            // draw the fullscreen triangle
            glBindVertexArray(postProcessVertexArray);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            glBindVertexArray(0);
        }
    }

}