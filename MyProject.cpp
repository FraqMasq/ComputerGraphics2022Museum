// This has been adapted from the Vulkan tutorial

#include "MyProject.hpp"



struct Asset {
    const std::string ObjPath;
    const std::string TexturePath;
    const glm::vec3 pos;
    const float scale;
};

struct Map {
    stbi_uc* museumMap;
    int museumMapWidth, museumMapHeight;
    void loadMap();
};


struct Component {
    Model M;
    Texture T;
    DescriptorSet DS;
};


const std::vector<Asset> AssetVector = {
        {"models/misc/WallsAndFloor2.obj", "textures/misc/floor_wall.png", {0,0.0, 0.0}, 1.0},
        {"models/statues/venus.obj", "textures/statues/statue_venus.jpg", {0,0.0, 0.0}, 1.0},
        {"models/statues/discobolus.obj", "textures/statues/discobolusTexture.png", {0,0.0, 0.0}, 1.0},
        {"models/statues/pedestal.obj", "textures/statues/pedestal.jpg", {0,0.0, 0.0}, 1.0},
        {"models/statues/hercules.obj", "textures/statues/hercules.jpg", {0,0.0, 0.0}, 1.0},
        {"models/statues/davidStatue.obj", "textures/statues/davidTexture.jpg", {0,0.0, 0.0}, 1.0},

        {"models/paints/Frames.obj", "textures/paints/T_picture_frame_BaseColor.tga", {0,0.0, 0.0}, 1.0},
        {"models/paints/Bathers.obj", "textures/paints/theBathers_Cezanne.jpg", {0,0.0, 0.0}, 1.0},
        {"models/paints/Munch.obj", "textures/paints/Munch_Scream.jpg", {0,0.0, 0.0}, 1.0},
        {"models/paints/VanGogh.obj", "textures/paints/VanGogh_self.jpg", {0,0.0, 0.0}, 1.0}

};

const int numAssets = 10;

//used to index AssetVector and ComponentVector
enum ASSETS {STRUCTURE, VENUS, DISCOBOLUS, PEDESTAL, HERCULES, DAVID, FRAMES, BATHERS, MUNCH, VANGOGH};


//MODEL
//@todo usare AssetVector, indicizzato tramite enum ASSETS
//const std::string MODEL_PATH = "models/viking_room.obj";
//const std::string MODEL_PATH = "models/statues/davidStatue.obj";
//const std::string DISCO_MODEL_PATH = "models/misc/doors.obj";


//const std::string STRUCTURE_MODEL_PATH = "models/misc/WallsAndFloor2.obj"; //"models/misc/WallsAndFloor2.obj";
//const std::string VENUS_MODEL_PATH = "models/statues/venus.obj";
//const std::string DISCO_MODEL_PATH = "models/statues/discobolus.obj";

//const std::string VENUS_MODEL_PATH = "models/paints/Frames.obj";
//const std::string DISCO_MODEL_PATH = "models/paints/Munch.obj";

//const std::string DISCO_MODEL_PATH = "models/paints/Bathers.obj";



//Già posizionati:
//const std::string VENUS_MODEL_PATH = "models/statues/pedestal.obj";
//const std::string DISCO_MODEL_PATH = "models/statues/hercules.obj";

//TEXTURE
//const std::string TEXTURE_PATH = "textures/viking_room.png";
//const std::string TEXTURE_PATH = "textures/statues/davidTexture.jpg";
//const std::string TEXTURE_PATH = "textures/statues/hercules.jpg";
//const std::string DISCO_TEXTURE_PATH = "textures/misc/door.png";

const std::string STRUCTURE_TEXTURE_PATH = "textures/misc/floor_wall.png";
const std::string VENUS_TEXTURE_PATH = "textures/statues/statue_venus.jpg";
const std::string DISCO_TEXTURE_PATH = "textures/statues/discobolusTexture.png";
const std::string MAP_TEXTURE_PATH = "textures/MuseumCanStep.png";

//const std::string VENUS_TEXTURE_PATH = "textures/paints/T_picture_frame_BaseColor.tga";
//const std::string DISCO_TEXTURE_PATH = "textures/paints/Munch_Scream.jpg";
//const std::string DISCO_TEXTURE_PATH = "textures/paints/theBathers_Cezanne.jpg";

//Già posizionati:
//const std::string VENUS_TEXTURE_PATH = "textures/statues/pedestal.jpg";
//const std::string DISCO_TEXTURE_PATH = "textures/statues/hercules.jpg";

// The uniform buffer object used in this example
//IT may be splitted in {view, proj}(set0, binding0) and {model}(set1, binding0)
// among with {texture} (set1, binding0)
// and add a DSLinit for set0
//need to bind ds_global with VKBindDescriptorSet and change the others params
//also add in MapMemory
//at the end change in shaders the set position
// vert: (glb is set0bind0, obj is set1bind0)
// frag: text is set1bind1
struct GlobalUniformBufferObject {
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
    alignas(16) glm::vec3 lightDir;
    //alignas(16) glm::vec3 lightDir2;
    //alignas(16) glm::vec3 lightDir3;
    alignas(16) glm::vec3 lightPos1;
    alignas(16) glm::vec3 lightPos2;
    alignas(16) glm::vec3 spotPos1; //the position of the light.
    alignas(16) glm::vec3 spotPos2; //the position of the light.
    alignas(16) glm::vec3 spotPos3; //the position of the light.
    alignas(16) glm::vec3 spotPos4; //the position of the light.
    alignas(16) glm::vec3 lightColor;
    alignas(16) glm::vec3 ambColor;
    alignas(16) glm::vec4 coneInOutDecayExp;
};

struct UniformBufferObject {
    alignas(16) glm::mat4 model;

};

const glm::mat4 idMatrix = glm::mat4(1.0f);
const glm::vec3 xAxis = glm::vec3(1.0f, 0.0f, 0.0f);
const glm::vec3 yAxis = glm::vec3(0.0f, 1.0f, 0.0f);
const glm::vec3 zAxis = glm::vec3(0.0f, 0.0f, 1.0f);


// MAIN ! 
class MyProject : public BaseProject {
protected:
    //
    glm::vec3 camPos = glm::vec3(2.0f, 2.0f, 0.0f);

    // Here you list all the Vulkan objects you need:

    // Descriptor Layouts [what will be passed to the shaders]
    DescriptorSetLayout DSLGlobal;
    DescriptorSetLayout DSLObj;

    // Pipelines [Shader couples]
    Pipeline P1;

    DescriptorSet DS_GLOBAL;

    Map museumMap;


    std::vector<Component> componentsVector;

public:



protected:



    // Here you set the main application parameters
    void setWindowParameters() {
        // window size, titile and initial background
        windowWidth = 800;
        windowHeight = 600;
        windowTitle = "My Project";
        initialBackgroundColor = { 0.0f, 0.0f, 0.0f, 1.0f };

        // Descriptor pool sizes -> modify if add other models
        uniformBlocksInPool = numAssets + 1;
        texturesInPool = numAssets;
        setsInPool = numAssets + 1;
    }

    // Here you load and setup all your Vulkan objects
    void localInit() {
        // Descriptor Layouts [what will be passed to the shaders]
        DSLGlobal.init(this, {
                {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS}
            });

        DSLObj.init(this, {
            // this array contains the binding:
            // first  element : the binding number
            // second element : the time of element (buffer or texture)
            // third  element : the pipeline stage where it will be used
            {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT},
            {1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}
            });

        // Pipelines [Shader couples]
        // The last array, is a vector of pointer to the layouts of the sets that will
        // be used in this pipeline. The first element will be set 0, and so on..
        P1.init(this, "shaders/vert.spv", "shaders/frag.spv", { &DSLGlobal, &DSLObj });

        componentsVector.resize(AssetVector.size());
        for (int j = 0; j < componentsVector.size(); j++) {
            componentsVector[j].M.init(this, AssetVector[j].ObjPath);
            componentsVector[j].T.init(this, AssetVector[j].TexturePath);
            componentsVector[j].DS.init(this, &DSLObj, {
                /*
                // the second parameter, is a pointer to the Uniform Set Layout of this set
                // the last parameter is an array, with one element per binding of the set.
                // first  elmenet : the binding number
                // second element : UNIFORM or TEXTURE (an enum) depending on the type
                // third  element : only for UNIFORMs, the size of the corresponding C++ object
                // fourth element : only for TEXTUREs, the pointer to the corresponding texture object */
                {0, UNIFORM, sizeof(UniformBufferObject), nullptr},
                {1, TEXTURE, 0, &componentsVector[j].T}
                });
        }
        // Models, textures and Descriptors (values assigned to the uniforms)

        /*
        // the second parameter, is a pointer to the Uniform Set Layout of this set
        // the last parameter is an array, with one element per binding of the set.
        // first  elmenet : the binding number
        // second element : UNIFORM or TEXTURE (an enum) depending on the type
        // third  element : only for UNIFORMs, the size of the corresponding C++ object
        // fourth element : only for TEXTUREs, the pointer to the corresponding texture object */



        DS_GLOBAL.init(this, &DSLGlobal, {
                {0, UNIFORM, sizeof(GlobalUniformBufferObject), nullptr}
            });

        museumMap.loadMap();

    }

    void localReInit() {
        // Pipelines [Shader couples]
        // The last array, is a vector of pointer to the layouts of the sets that will
        // be used in this pipeline. The first element will be set 0, and so on..
        P1.reinit(this, "shaders/vert.spv", "shaders/frag.spv", { &DSLGlobal, &DSLObj }); //maybe reinit is equal to init at the end, check if needs another moethod

    }

    // Here you destroy all the objects you created!		
    void localCleanup() {

        for (int j = 0; j < componentsVector.size(); j++) {
            componentsVector[j].M.cleanup();
            componentsVector[j].T.cleanup();
            componentsVector[j].DS.cleanup();
        }

        DS_GLOBAL.cleanup();

		//P1.cleanup();
		DSLGlobal.cleanup();
        DSLObj.cleanup();
    }

    //@todo Controllo se P1 != VK_NULL_HANDLE -> cleanup
    void localPipeCleanup() {

        P1.cleanup();
    }

    static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
        auto app = reinterpret_cast<MyProject*>(glfwGetWindowUserPointer(window));
        app->framebufferResized = true;
    }

    // Here it is the creation of the command buffer:
    // You send to the GPU all the objects you want to draw,
    // with their buffers and textures
    void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) {

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
            P1.graphicsPipeline);

        vkCmdBindDescriptorSets(commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            P1.pipelineLayout, 0, 1, &DS_GLOBAL.descriptorSets[currentImage],
            0, nullptr);


        VkDeviceSize offsets[] = { 0 };

        VkBuffer vertexBuffers[numAssets][1];
        for (int i = 0; i < numAssets; i++) {
            vertexBuffers[i][0] = componentsVector[i].M.vertexBuffer;
            vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers[i], offsets);
            // property .indexBuffer of models, contains the VkBuffer handle to its index buffer
            vkCmdBindIndexBuffer(commandBuffer, componentsVector[i].M.indexBuffer, 0,
                VK_INDEX_TYPE_UINT32);

            // property .pipelineLayout of a pipeline contains its layout.
            // property .descriptorSets of a descriptor set contains its elements.
            vkCmdBindDescriptorSets(commandBuffer,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                P1.pipelineLayout, 1, 1, &componentsVector[i].DS.descriptorSets[currentImage],
                0, nullptr);



            // property .indices.size() of models, contains the number of triangles * 3 of the mesh.
            vkCmdDrawIndexed(commandBuffer,
                static_cast<uint32_t>(componentsVector[i].M.indices.size()), 1, 0, 0, 0);

        }

        /* = {componentsVector[STRUCTURE].M.vertexBuffer,
                                    componentsVector[VENUS].M.vertexBuffer,
                                    componentsVector[DISCOBOLUS].M.vertexBuffer};


        // property .vertexBuffer of models, contains the VkBuffer handle to its vertex buffer
        //VkDeviceSize offsets[] = {0};

        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers[VENUS], offsets);
        // property .indexBuffer of models, contains the VkBuffer handle to its index buffer
        vkCmdBindIndexBuffer(commandBuffer, componentsVector[VENUS].M.indexBuffer, 0,
                                VK_INDEX_TYPE_UINT32);

        // property .pipelineLayout of a pipeline contains its layout.
        // property .descriptorSets of a descriptor set contains its elements.
        vkCmdBindDescriptorSets(commandBuffer,
                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                        P1.pipelineLayout, 1, 1, &componentsVector[VENUS].DS.descriptorSets[currentImage],
                        0, nullptr);



        // property .indices.size() of models, contains the number of triangles * 3 of the mesh.
        vkCmdDrawIndexed(commandBuffer,
                    static_cast<uint32_t>(componentsVector[VENUS].M.indices.size()), 1, 0, 0, 0);


        VkBuffer vertexBuffersDisco[] = {componentsVector[DISCOBOLUS].M.vertexBuffer};
        // property .vertexBuffer of models, contains the VkBuffer handle to its vertex buffer
        VkDeviceSize offsetsDisco[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers[DISCOBOLUS], offsets);
        // property .indexBuffer of models, contains the VkBuffer handle to its index buffer
        vkCmdBindIndexBuffer(commandBuffer, componentsVector[DISCOBOLUS].M.indexBuffer, 0,
                             VK_INDEX_TYPE_UINT32);
        vkCmdBindDescriptorSets(commandBuffer,
                                VK_PIPELINE_BIND_POINT_GRAPHICS,
                                P1.pipelineLayout, 1, 1, &componentsVector[DISCOBOLUS].DS.descriptorSets[currentImage],
                                0, nullptr);
        vkCmdDrawIndexed(commandBuffer,
                         static_cast<uint32_t>(componentsVector[DISCOBOLUS].M.indices.size()), 1, 0, 0, 0);


        VkBuffer vertexBuffersStruct[] = {componentsVector[STRUCTURE].M.vertexBuffer};
        VkDeviceSize offsetsStruct[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers[STRUCTURE], offsets);
        vkCmdBindIndexBuffer(commandBuffer, componentsVector[STRUCTURE].M.indexBuffer, 0,
                             VK_INDEX_TYPE_UINT32);
        vkCmdBindDescriptorSets(commandBuffer,
                                VK_PIPELINE_BIND_POINT_GRAPHICS,
                                P1.pipelineLayout, 1, 1, &componentsVector[STRUCTURE].DS.descriptorSets[currentImage],
                                0, nullptr);
        vkCmdDrawIndexed(commandBuffer,
                         static_cast<uint32_t>(componentsVector[STRUCTURE].M.indices.size()), 1, 0, 0, 0);
*/
    }


    float computeDeltaTime() {
        // Compute time
        static auto startTime = std::chrono::high_resolution_clock::now();
        static float lastTime = 0.0f;
        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>
            (currentTime - startTime).count();
        float dt = time - lastTime;
        lastTime = time;
        return dt;
    }
    /* Create a look in direction matrix
    // Pos    -> Position of the camera
    // NB Angs have angles in Rad
    // Angs.x -> direction (alpha) -> to be applyed on Y axis
    // Angs.y -> elevation (beta) -> to be applyed on X axis
    // Angs.z -> roll (rho) -> to be applyed on Z axis
     */
    glm::mat4 LookInDirMat(glm::vec3 Pos, glm::vec3 Angs) {
        glm::vec3 p = glm::vec3(0, 0, 0) - Pos;

        glm::mat4 out = glm::rotate(idMatrix, -Angs.z, zAxis) *
            glm::rotate(idMatrix, -Angs.y, xAxis) *
            glm::rotate(idMatrix, -Angs.x, yAxis) *
            glm::translate(idMatrix, p);

        return out;
    }

    //________________
    // Here is where you update the uniforms.
    // Very likely this will be where you will be writing the logic of your application.
    void updateUniformBuffer(uint32_t currentImage) {
        static auto startTime = std::chrono::high_resolution_clock::now();
        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>
            (currentTime - startTime).count();

        //Duplicate for each obj, model may change and view/proj is same for each obj
        UniformBufferObject ubo{};
        GlobalUniformBufferObject gubo{};
        void* data;

        float omega, mu, dt;

        static glm::vec3 YPR = glm::vec3(glm::radians(0.0f), 0.0f, glm::radians(0.0f));

        static glm::vec3 ux = glm::vec3(glm::rotate(idMatrix, YPR.x, yAxis) *
            glm::vec4(1, 0, 0, 1));
        static glm::vec3 uy = yAxis;
        static glm::vec3 uz = glm::vec3(glm::rotate(idMatrix, YPR.x, yAxis) *
            glm::vec4(0, 0, -1, 1));

        omega = 1; //[Rad/s]
        mu = 10; //[unit/s]

        dt = computeDeltaTime();

        int present = glfwJoystickPresent(GLFW_JOYSTICK_1);
        int axesCount;
        const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCount);
       // std::cout << present << "\tAxes available:" << axesCount<< "\n";
        if(present == 1 && axesCount != 6) {
            present = 0;
        }
        glm::vec3 oldPos = camPos;
        if (glfwGetKey(window, GLFW_KEY_LEFT) || (present== 1 && axes[2] <= -0.5)) {
            YPR.x += dt * omega;
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) || (present== 1 && axes[2] >= 0.5)) {
            YPR.x -= dt * omega;
        }
        if (glfwGetKey(window, GLFW_KEY_UP) || (present== 1 && axes[5] <= -0.5)) {
            YPR.y += dt * omega;
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) || (present== 1 && axes[5] >= 0.5)) {
            YPR.y -= dt * omega;
        }
        if (glfwGetKey(window, GLFW_KEY_Q)) {
            YPR.z -= dt * omega;
        }
        if (glfwGetKey(window, GLFW_KEY_E)) {
            YPR.z += dt * omega;
        }
        if (glfwGetKey(window, GLFW_KEY_A)  || (present== 1 && axes[0] <= -0.5)) {
            camPos -= mu * glm::vec3(glm::rotate(glm::mat4(1.0f), YPR.x,
                glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(1, 0, 0, 1)) * dt;
        }
        if (glfwGetKey(window, GLFW_KEY_D) || (present== 1 && axes[0] >= 0.5)) {
            camPos += mu * glm::vec3(glm::rotate(glm::mat4(1.0f), YPR.x,
                glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(1, 0, 0, 1)) * dt;
        }
        if (glfwGetKey(window, GLFW_KEY_W)  || (present== 1 && axes[1] <= -0.5)) {
            camPos -= mu * glm::vec3(glm::rotate(glm::mat4(1.0f), YPR.x,
                glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(0, 0, 1, 1)) * dt;
        }
        if (glfwGetKey(window, GLFW_KEY_S)  || (present== 1 && axes[1] >= 0.5)) {
            camPos += mu * glm::vec3(glm::rotate(glm::mat4(1.0f), YPR.x,
                glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(0, 0, 1, 1)) * dt;
        }


        // @todo implement canStep to enable stopping the movement (A06.cpp)
        if (!canStep(camPos.x, camPos.z)) {
            camPos = oldPos;
        }


        gubo.view = LookInDirMat(camPos, YPR);



        gubo.proj = glm::perspective(glm::radians(45.0f),
            swapChainExtent.width / (float)swapChainExtent.height,
            0.1f, 50.0f);
        gubo.proj[1][1] *= -1;

        gubo.lightDir = glm::vec3(cos(glm::radians(45.0f)), sin(glm::radians(45.0f)), 0.0f);
        gubo.lightPos1 = glm::vec3(4.0f, 3.5f, 6.0f); //light between the statues
        gubo.lightPos2 = glm::vec3(11.0f, -1.0f, 6.0f); //light for the paintings
        gubo.spotPos1 = glm::vec3(1.3f, 5.0f, -7.3f); //Discobolus
        gubo.spotPos2 = glm::vec3(6.3f, 5.0f, -7.3f); //venus
        gubo.spotPos3 = glm::vec3(6.5f, 5.0f, -1.3f); //david
        gubo.spotPos4 = glm::vec3(1.5f, 5.0f, -2.6f); //hercules
        gubo.lightColor = glm::vec3(0.6f, 0.6f, 0.6f);
        gubo.ambColor = glm::vec3(0.1f, 0.1f, 0.1f);
        gubo.coneInOutDecayExp = glm::vec4(0.9f, 0.92f, 2.0f, 1.0f);

        vkMapMemory(device, DS_GLOBAL.uniformBuffersMemory[0][currentImage], 0,
            sizeof(gubo), 0, &data);
        memcpy(data, &gubo, sizeof(gubo));
        vkUnmapMemory(device, DS_GLOBAL.uniformBuffersMemory[0][currentImage]);

        for (int i = 0; i < numAssets; i++) {
            ubo.model = idMatrix;/*glm::rotate(glm::mat4(1.0f),
                                glm::radians(270.0f), //*time
                                glm::vec3(1.0f, 0.0f, 0.0f))*
                    glm::translate(glm::mat4(3.0f), glm::vec3(-1.5f, 0.0f, 0.0f));
                    */
            /*
             // @todo ruota intorno origine, deve ruotare l'oggetto
              if(i == HERCULES){
                ubo.model = ubo.model *
                        glm::rotate(idMatrix, glm::radians(270.0f) * time,
                        glm::vec3(0.0f, 1.0f, 0.0f));
            }*/
            vkMapMemory(device, componentsVector[i].DS.uniformBuffersMemory[0][currentImage], 0,
                sizeof(ubo), 0, &data);
            memcpy(data, &ubo, sizeof(ubo));
            vkUnmapMemory(device, componentsVector[i].DS.uniformBuffersMemory[0][currentImage]);
        }
    }
    bool canStepPoint(float x, float y) {
        //museumMap.museumMapWidth/16.0 -> pixel in 1 meter
        //added 0.5 and 6 because floor is not centered in zero
        int pixX = round(fmax(0.0f, fmin(museumMap.museumMapWidth - 1, ((x + 8 - 6) * museumMap.museumMapWidth / 16.0)))); //20
        int pixY = round(fmax(0.0f, fmin(museumMap.museumMapHeight - 1, ((y + 9.5 - 0.5) * museumMap.museumMapHeight / 19.0)))); //20
        int pix = (int)museumMap.museumMap[museumMap.museumMapWidth * pixY + pixX];
        return pix > 128; //check if pixel is white (?)
    }
    const float checkRadius = 0.1;
    const int checkSteps = 12; //12

    bool canStep(float x, float y) {
        for (int i = 0; i < checkSteps; i++) {
            if (!canStepPoint(x + cos(6.2832 * i / (float)checkSteps) * checkRadius,
                y + sin(6.2832 * i / (float)checkSteps) * checkRadius)) {
                std::cout << "Hit something \n";

                return false;
            }
        }
        return true;
    }

};
    


void Map::loadMap() {
    museumMap = stbi_load(MAP_TEXTURE_PATH.c_str(),
        &museumMapWidth, &museumMapHeight,
        NULL, 1);
    if (!museumMap) {
        std::cout << MAP_TEXTURE_PATH.c_str() << "\n";
        throw std::runtime_error("failed to load map image!");
    }
    std::cout << "Station map -> size: " << museumMapWidth
        << "x" << museumMapHeight << "\n";
}

// This is the main: probably you do not need to touch this!
int main() {
    MyProject app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}