#include "camera.hpp"
#include "../ecs/entity.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 

namespace our {
    // Reads camera parameters from the given json object
    void CameraComponent::deserialize(const nlohmann::json& data){
        if(!data.is_object()) return;
        std::string cameraTypeStr = data.value("cameraType", "perspective");
        if(cameraTypeStr == "orthographic"){
            cameraType = CameraType::ORTHOGRAPHIC;
        } else {
            cameraType = CameraType::PERSPECTIVE;
        }
        near = data.value("near", 0.01f);
        far = data.value("far", 100.0f);
        fovY = data.value("fovY", 90.0f) * (glm::pi<float>() / 180);
        orthoHeight = data.value("orthoHeight", 1.0f);
    }

    // Creates and returns the camera view matrix
    glm::mat4 CameraComponent::getViewMatrix() const {
        auto owner = getOwner();
        auto M = owner->getLocalToWorldMatrix();
        //TODO: (Req 8) Complete this function [DONE]
        //HINT:
        // In the camera space:
        // - eye is the origin (0,0,0)
        // - center is any point on the line of sight. So center can be any point (0,0,z) where z < 0. For simplicity, we let center be (0,0,-1)
        // - up is the direction (0,1,0)
        // but to use glm::lookAt, we need eye, center and up in the world state.
        // Since M (see above) transforms from the camera to thw world space, you can use M to compute:
        // - the eye position which is the point (0,0,0) but after being transformed by M
        // - the center position which is the point (0,0,-1) but after being transformed by M
        // - the up direction which is the vector (0,1,0) but after being transformed by M
        // then you can use glm::lookAt

        glm::vec3 eye = M * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        glm::vec3 center = M * glm::vec4(0.0f, 0.0f, -1.0f, 1.0f);
        glm::vec3 up = M * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);

        return glm::lookAt(eye, center, up);
        //To use glm::lookat, we need to use vec3.
        //the given data are in terms of vec3 as well.
        //However, to use the data properly we need to transform them to the world space. So we multiply it by M.
        //The problem is that M is a mat4 and the points are a vec3. So we add either 1 or 0 to the end of the vec3 to make it a vec4.
        //We studied that a 3D point is a vec4 with the last element being 1.
        //We also studied that a 3D vector is a vec4 with the last element being 0.
        // So we appended these data to their correspondings to be able to multiply them by M.
        //But the result of M * vec4 is a vec4.
        //So we use a vec3 in the result to store the first 3 elements of the vec4 and neglect the last element.
    }

    // Creates and returns the camera projection matrix
    // "viewportSize" is used to compute the aspect ratio
    glm::mat4 CameraComponent::getProjectionMatrix(glm::ivec2 viewportSize) const {
        //TODO: (Req 8) Wrtie this function [DONE]
        // NOTE: The function glm::ortho can be used to create the orthographic projection matrix
        // It takes left, right, bottom, top. Bottom is -orthoHeight/2 and Top is orthoHeight/2.
        // Left and Right are the same but after being multiplied by the aspect ratio
        // For the perspective camera, you can use glm::perspective
        // It takes fovY, aspect, near and far
        float aspectRatio = viewportSize.x / viewportSize.y;
        if (cameraType == CameraType::ORTHOGRAPHIC){
            float left = aspectRatio * -orthoHeight / 2;
            float right = aspectRatio * orthoHeight / 2;
            float bottom = -orthoHeight / 2;
            float top =  orthoHeight / 2;
            return glm::ortho(left,right,bottom,top);
        } else {
            return glm::perspective(fovY, aspectRatio, near, far);
        }
    }
}