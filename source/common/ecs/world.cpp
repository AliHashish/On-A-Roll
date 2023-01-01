#include "world.hpp"

namespace our {

    // This will deserialize a json array of entities and add the new entities to the current world
    // If parent pointer is not null, the new entities will be have their parent set to that given pointer
    // If any of the entities has children, this function will be called recursively for these children
    void World::deserialize(const nlohmann::json& data, Entity* parent){
        if(!data.is_array()) return;
        for(const auto& entityData : data){
            //TODO: (Req 8) Create an entity, make its parent "parent" and call its deserialize with "entityData". [DONE]
            Entity* newEntity = this->add();
            // if(newEntity->parent)
                newEntity->parent = parent;
            newEntity->deserialize(entityData);
            if(entityData.contains("children")){
                //TODO: (Req 8) Recursively call this world's "deserialize" using the children data [DONE]
                // and the current entity as the parent
                deserialize( entityData["children"], newEntity);
            }
            if(entityData.contains("components") && entityData["components"][0].contains("mesh") && entityData["components"][0]["mesh"] == "plane")
            {                
                
                // std::cout << "//////////////////platform number " << c << "//////////////////////" << std::endl;
                // PlatformRectangles.push_back(newEntity);
                // now we want to push the rectangle of the platform into the vector
                
                // Coordinates of the origin of the rectangle
                float x = entityData["position"][0];
                float y = entityData["position"][2];

                // Width and height of the rectangle
                float width = entityData["scale"][0];
                float height = entityData["scale"][1];

                // lw mfeesh rotation, hnsheel scale [0] mn el x, w scale [1] mn el y
                // w hnedrab el width w el height fy 2
                // lw fy rotation b2a, el width wl height byb2o ma3kooseen, fa ha3delhom 
                // el awl, b3deeha hrg3 a3ml el steps el 3adya b2a
                if (entityData["rotation"][1] == 90)
                {
                    std::swap(width, height);
                }

                x -= width;
                y += height;
                width *= 2;
                height *= 2;

                Rectangle R(x, y, width, height);
                PlatformRectangles.push_back(R);

                // Check if this platfrom was the winning platform
                
            }
            if(entityData.contains("WinState"))
            {
                float x = entityData["position"][0];
                float y = entityData["position"][2];
                float width = entityData["scale"][0];
                float height = entityData["scale"][1];
                
                if (entityData["rotation"][1] == 90)
                {
                    std::swap(width, height);
                }

                x -= width;
                y += height;
                width *= 2;
                height *= 2;
                
                Rectangle R(x, y, width, height);
                WinningRectangle = R;

            }
        }
    }

}
