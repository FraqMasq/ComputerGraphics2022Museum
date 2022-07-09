// This has been adapted from the Vulkan tutorial

#include "MyProject.hpp"
#include <unordered_map>





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
        {"models/misc/WallsAndFloor2.obj", "textures/misc/floor_wall.png", {0.0,0.0, 0.0}, 1.0},

        {"models/statues/venus.obj", "textures/statues/statue_venus.jpg", {6.222,0.0, -7.317}, 1.0},
        {"models/statues/discobolus.obj", "textures/statues/discobolusTexture.png", {1.416,0.0, -7.204}, 1.0},
        {"models/statues/pedestal.obj", "textures/statues/pedestal.jpg", {-0.5,0.0, -3.001}, 1.0},
        {"models/statues/hercules.obj", "textures/statues/hercules.jpg", {-0.5,1.653, -2.989}, 1.0},
        {"models/statues/davidStatue.obj", "textures/statues/davidTexture.jpg", {7.578,0.0, -1.291}, 1.0},
        

        {"models/paints/Frames.obj", "textures/paints/T_picture_frame_BaseColor.tga", {0.0,0.0, 0.0}, 1.0},
        {"models/paints/HorizontalPicture.obj", "textures/paints/theBathers_Cezanne.jpg", {9.393, 2.842, 1.641}, 1.0},
        {"models/paints/VerticalPicture.obj", "textures/paints/Munch_Scream.jpg", {13.97, 3.558, -1.757}, 1.0},
        {"models/paints/VerticalPicture.obj", "textures/paints/VanGogh_self.jpg", {13.97, 3.558, 2.558}, 1.0},

        {"models/misc/PopUp.obj", "textures/misc/popup/PopUpVenus.png", {-5.0, -5.0, -5.0}, 0.2},
        {"models/misc/PopUp.obj", "textures/misc/popup/PopUpDiscobolus.png", {-5.0, -5.0, -5.0}, 0.2},
        {"models/misc/PopUp.obj", "textures/misc/popup/PopUpHercules.png", {-5.0, -5.0, -5.0}, 0.2},
        {"models/misc/PopUp.obj", "textures/misc/popup/PopUpDavid.png", {-5.0, -5.0, -5.0}, 0.2},
        {"models/misc/PopUp.obj", "textures/misc/popup/PopUpBathers.png", {-5.0, -5.0, -5.0}, 0.2},
        {"models/misc/PopUp.obj", "textures/misc/popup/PopUpMunch.png", {-5.0, -5.0, -5.0}, 0.2},
        {"models/misc/PopUp.obj", "textures/misc/popup/PopUpVanGogh.png", {-5.0, -5.0, -5.0}, 0.2}

};

const std::vector<Asset> AssetVector2 = {
        {"models/misc/StructureScene2.obj", "textures/misc/StructureScene2Texture.jpeg", {0.0,0.0, 0.0}, 1.0},

        {"models/planets/PlanetsSphere.obj", "textures/planets/venus.jpg", {0.0,2.0, 0.0}, 1.0},
        {"models/planets/PlanetsSphere.obj", "textures/planets/mercury.jpg", {0.0,2.0, 2.0}, 0.1},
        {"models/planets/PlanetsSphere.obj", "textures/planets/venus.jpg", {0.0,2.0, 3.0}, 0.25},
        {"models/planets/PlanetsSphere.obj", "textures/planets/earth.jpg", {0.0,2.0, 4.0}, 0.25},
        {"models/planets/PlanetsSphere.obj", "textures/planets/mars.jpg", {0.0,2.0, 5.0}, 0.15},
        {"models/planets/PlanetsSphere.obj", "textures/planets/jupiter.jpg", {0.0,2.0, 8.0}, 1.1},
        //{"models/planets/PlanetsSphere.obj", "textures/planets/saturnWithRings.jpg", {0.0,2.0, 10.0}, 1.0},
        {"models/planets/Staturn.obj", "textures/planets/saturnWithRings.jpg", {0.0,2.0, 11.0}, 1.0},
        {"models/planets/PlanetsSphere.obj", "textures/planets/neptune.jpg", {0.0,2.0, 14.0}, 0.5},
        {"models/planets/PlanetsSphere.obj", "textures/planets/uranus.jpg", {0.0,2.0, 17.0}, 0.5}
       

};
const int numAssets = 17; // (Frames, Structure and Pedestal = 3) + 2*(NStatues + NPaints)
const int numAssets2 = 10; // 

//used to index AssetVector and ComponentVector
enum ASSETS {STRUCTURE,
        VENUS, DISCOBOLUS, PEDESTAL, HERCULES, DAVID,
        FRAMES,
        BATHERS, MUNCH, VANGOGH,
        POPUPVENUS, POPUPDISCOBOLUS, POPUPHERCULES, POPUPDAVID,
        POPUPBATHERS, POPUPMUNCH, POPUPVANGOGH
};

const std::string MAP_TEXTURE_PATH = "textures/MuseumCanStep.png";

std::unordered_map<int, int> mapping = {
        {VENUS, POPUPVENUS},
        {DISCOBOLUS, POPUPDISCOBOLUS},
        {PEDESTAL, POPUPHERCULES},
        {HERCULES, POPUPHERCULES},
        {DAVID, POPUPDAVID},
        {MUNCH, POPUPMUNCH},
        {VANGOGH, POPUPVANGOGH},
        {BATHERS, POPUPBATHERS}

};

struct GlobalUniformBufferObject {
    alignas(16) glm::vec3 lightPos1;
    alignas(16) glm::vec3 lightPos2;
    alignas(16) glm::vec3 lightColor;
    alignas(16) glm::vec3 ambColor;
    alignas(16) glm::vec4 coneInOutDecayExp;
    alignas(16) glm::vec3 spotPosition1;
    alignas(16) glm::vec3 spotPosition2;
    alignas(16) glm::vec3 spotPosition3;
    alignas(16) glm::vec3 spotPosition4;
    alignas(16) glm::vec3 spotDirection1;
    alignas(16) glm::vec3 spotDirection2;
    alignas(16) glm::vec3 spotDirection3;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
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
   // DescriptorSetLayout DSLGlobal2;
    DescriptorSetLayout DSLObj2;

    // Pipelines [Shader couples]
    Pipeline P1;
    Pipeline P2;

    DescriptorSet DS_GLOBAL;
    DescriptorSet DS_GLOBAL2;

    Map museumMap;

    std::vector<Component> componentsVector;
    std::vector<Component> componentsVector2;

    bool isPopupShown = false;
    int curText = 0;
    int num_scenes = 2;

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
        //per il cambio di scena?
        uniformBlocksInPool = numAssets + numAssets2 + 2;
        texturesInPool = numAssets + numAssets2;
        setsInPool = numAssets + numAssets2 + 2;
    }

    // Here you load and setup all your Vulkan objects
    void localInit() {
        // Descriptor Layouts [what will be passed to the shaders]
        
            DSLGlobal.init(this, {
                    {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS}
                });

            //DSLGlobal2.init(this, {
               // {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS}
               // });


            DSLObj.init(this, {
                // this array contains the binding:
                // first  element : the binding number
                // second element : the time of element (buffer or texture)
                // third  element : the pipeline stage where it will be used
                {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT},
                {1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}
                });

            DSLObj2.init(this, {
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

            P2.init(this, "shaders/planet_vert.spv", "shaders/planet_frag.spv", { &DSLGlobal, &DSLObj2 });


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

            std::cout << "size:" << AssetVector2.size();
            componentsVector2.resize(AssetVector2.size());
            for (int j = 0; j < componentsVector2.size(); j++) {
                componentsVector2[j].M.init(this, AssetVector2[j].ObjPath);
                componentsVector2[j].T.init(this, AssetVector2[j].TexturePath);
                componentsVector2[j].DS.init(this, &DSLObj2, {
                    /*
                    // the second parameter, is a pointer to the Uniform Set Layout of this set
                    // the last parameter is an array, with one element per binding of the set.
                    // first  elmenet : the binding number
                    // second element : UNIFORM or TEXTURE (an enum) depending on the type
                    // third  element : only for UNIFORMs, the size of the corresponding C++ object
                    // fourth element : only for TEXTUREs, the pointer to the corresponding texture object */
                {0, UNIFORM, sizeof(UniformBufferObject), nullptr},
                {1, TEXTURE, 0, &componentsVector2[j].T}
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

            DS_GLOBAL2.init(this, &DSLGlobal, {
                    {0, UNIFORM, sizeof(GlobalUniformBufferObject), nullptr}
                });

           

            museumMap.loadMap();
        
    }

    void localReInit() {
        // Pipelines [Shader couples]
        // The last array, is a vector of pointer to the layouts of the sets that will
        // be used in this pipeline. The first element will be set 0, and so on..
        
            P1.reinit(this, "shaders/vert.spv", "shaders/frag.spv", { &DSLGlobal, &DSLObj }); //maybe reinit is equal to init at the end, check if needs another moethod
            
            P2.reinit(this, "shaders/planet_vert.spv", "shaders/planet_frag.spv", { &DSLGlobal, &DSLObj2 }); //maybe reinit is equal to init at the end, check if needs another moethod
        
    }

    // Here you destroy all the objects you created!		
    void localCleanup() {
        
        for (int j = 0; j < componentsVector.size(); j++) {
            componentsVector[j].M.cleanup();
            componentsVector[j].T.cleanup();
            componentsVector[j].DS.cleanup();
        }

        for (int j = 0; j < componentsVector2.size(); j++) {
            componentsVector2[j].M.cleanup();
            componentsVector2[j].T.cleanup();
            componentsVector2[j].DS.cleanup();
        }

        DS_GLOBAL.cleanup();
        DS_GLOBAL2.cleanup();

        //P1.cleanup();
        DSLGlobal.cleanup();
        DSLObj.cleanup();
        DSLObj2.cleanup();

        //DS_GLOBAL2.cleanup();

        //DSLGlobal2.cleanup();
        
        
    }

    //@todo Controllo se P1 != VK_NULL_HANDLE -> cleanup
    void localPipeCleanup() {
        
            P1.cleanup();
            P2.cleanup();
        
    }

    static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
        auto app = reinterpret_cast<MyProject*>(glfwGetWindowUserPointer(window));
        app->framebufferResized = true;
    }

    // Here it is the creation of the command buffer:
    // You send to the GPU all the objects you want to draw,
    // with their buffers and textures
    void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) {

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, curText == 0 ? 
            P1.graphicsPipeline : P2.graphicsPipeline);

       
        vkCmdBindDescriptorSets(commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            curText == 0 ? P1.pipelineLayout : P2.pipelineLayout, 0, 1, curText == 0 ? &DS_GLOBAL.descriptorSets[currentImage] : &DS_GLOBAL2.descriptorSets[currentImage],
            0, nullptr);

        VkDeviceSize offsets[] = { 0 };

        if (curText == 0) {
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
        }
        else{
            VkBuffer vertexBuffers2[numAssets2][1];
            for (int i = 0; i < numAssets2; i++) {
                vertexBuffers2[i][0] = componentsVector2[i].M.vertexBuffer;
                vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers2[i], offsets);
                // property .indexBuffer of models, contains the VkBuffer handle to its index buffer
                vkCmdBindIndexBuffer(commandBuffer, componentsVector2[i].M.indexBuffer, 0,
                    VK_INDEX_TYPE_UINT32);

                // property .pipelineLayout of a pipeline contains its layout.
                // property .descriptorSets of a descriptor set contains its elements.
                vkCmdBindDescriptorSets(commandBuffer,
                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                    P2.pipelineLayout, 1, 1, &componentsVector2[i].DS.descriptorSets[currentImage],
                    0, nullptr);



                // property .indices.size() of models, contains the number of triangles * 3 of the mesh.
                vkCmdDrawIndexed(commandBuffer,
                    static_cast<uint32_t>(componentsVector2[i].M.indices.size()), 1, 0, 0, 0);
               

            }
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
        static float lastTime = 0.0f;
        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>
            (currentTime - startTime).count();

        //Duplicate for each obj, model may change and view/proj is same for each obj
        UniformBufferObject ubo{};
        GlobalUniformBufferObject gubo{};
        void* data;

        float omega, mu, dt;
        std::vector<float> planet_v = {0.0, 0.0, 1.0, 0.7, 0.5, 0.4, 0.2, 0.05, 0.04, 0.04 };

        static int nearestObject;
        int numCircles = 0;
        int cw;

        if(!isPopupShown){
            nearestObject = -1;
        }

        static glm::vec3 YPR = glm::vec3(glm::radians(0.0f), 0.0f, glm::radians(0.0f));

        static glm::vec3 ux = glm::vec3(glm::rotate(idMatrix, YPR.x, yAxis) *
            glm::vec4(1, 0, 0, 1));
        static glm::vec3 uy = yAxis;
        static glm::vec3 uz = glm::vec3(glm::rotate(idMatrix, YPR.x, yAxis) *
            glm::vec4(0, 0, -1, 1));

        omega = 1; //[Rad/s]
        mu = 10; //[unit/s]

        dt = time - lastTime;
        lastTime = time;
        static float debounce = time;

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
            numCircles = glm::floor(abs(YPR.x) / 6);
            std::cout << "x: " << YPR.x << "\n";
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) || (present== 1 && axes[2] >= 0.5)) {
            YPR.x -= dt * omega;
            numCircles = glm::floor(abs(YPR.x) / 6);
            std::cout << "x: " << YPR.x << "\n";
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
        
        if (curText == 0 && glfwGetKey(window, GLFW_KEY_K) && !isPopupShown){
            if(time - debounce > 0.33) {
                int notFound = 1;
                for (int i = 1; i < numAssets && notFound; i++) {
                    float dist = glm::abs(camPos.x - AssetVector[i].pos.x) + glm::abs(camPos.z - AssetVector[i].pos.z);
                    if (i != FRAMES && dist >1 && dist <= 5 ) {
                        cw = glm::sign(YPR.x);
                        float orientation = glm::abs(YPR.x);
                        orientation = orientation - 6 * numCircles;
                        if (checkOrientation(AssetVector[i].pos.x, AssetVector[i].pos.z, camPos.x, camPos.z,orientation,cw)) {
                            isPopupShown = true;
                            std::cout << "The nearest object is " << AssetVector[i].TexturePath
                                << "\n";
                            std::cout << "Mapped:\t" << i << ", " << mapping[i] << '\n';
                            notFound = 0;
                            nearestObject = i;
                        }
                    }

                }
            }
        }

        if (isPopupShown) {
            if (time - debounce > 0.33) {
                float dist = glm::abs(camPos.x - AssetVector[nearestObject].pos.x) + glm::abs(camPos.z - AssetVector[nearestObject].pos.z);
                if (dist > 6) {
                    isPopupShown = false;
                    nearestObject = -1;
                    std::cout << "Closing Popup\n";
                }
                else {
                    cw = glm::sign(YPR.x);
                    float orientation = glm::abs(YPR.x);
                    orientation = orientation - 6 * numCircles;
                    if(!checkOrientation(AssetVector[nearestObject].pos.x, AssetVector[nearestObject].pos.z, camPos.x, camPos.z, orientation, cw )) {
                        isPopupShown = false;
                        nearestObject = -1;
                        std::cout << "Closing Popup\n";
                    }
                }
            }
        }

        if (glfwGetKey(window, GLFW_KEY_L) && isPopupShown){
            isPopupShown = false;
            nearestObject = -1;
            std::cout << "Closing Popup\n";
        }

        if (glfwGetKey(window, GLFW_KEY_SPACE)) {
            if (time - debounce > 0.33) {
                curText = (curText + 1) % num_scenes;
                debounce = time;
                framebufferResized = true;
                std::cout << curText << "\n";
            }
        }

        // provvisorio, eventualmente aggiungere un'altra mappa
        if (curText == 0 && !canStep(camPos.x, camPos.z)) {
            camPos = oldPos;
        }


        gubo.view = LookInDirMat(camPos, YPR);



        gubo.proj = glm::perspective(glm::radians(45.0f),
            swapChainExtent.width / (float)swapChainExtent.height,
            0.1f, 50.0f);
        gubo.proj[1][1] *= -1;

        gubo.lightPos1 = glm::vec3(4.0f, 6.15f, -3.247f); //light between the statues
        gubo.lightPos2 = glm::vec3(11.57f, 6.515f, 7.192f); //light for the paintings

        
        gubo.spotPosition1 = glm::vec3(1.3f, 6.0f, -4.3f); //Discobolus
        gubo.spotPosition2= glm::vec3(6.3f, 6.0f, -4.2f); //venus
        gubo.spotPosition3 = glm::vec3(4.5f, 4.0f, -1.3f); //david
        gubo.spotPosition4 = glm::vec3(2.0f, 4.0f, -3.0f); //hercules
        
        /*
        //se mettiamo solo ai quadri (aggiornare anche gli shaders)
        gubo.spotPosition1 = glm::vec3(11.3f, 5.5f, 2.5f); //van gogh
        gubo.spotPosition2= glm::vec3(11.9f, 5.0f, -1.75f); //munch
        gubo.spotPosition3 = glm::vec3(12.9f, 4.5f, 2.0f); //cezanne
        */

        
        //set correct direction as angle atan(y,x) where y and x are differences between y and x coordinate of 
        //statues and spotlights
        //float theta = glm::atan(AssetVector[2].pos.z - gubo.spotPositions[0].z, AssetVector[2].pos.x - gubo.spotPositions[0].x);
        float theta = glm::radians(90.0f);
        gubo.spotDirection1 = glm::normalize(glm::vec3(cos(theta), sin(theta), 0.7f));
        //theta = glm::atan(glm::abs(AssetVector[1].pos.z - gubo.spotPositions[1].z) , glm::abs(AssetVector[1].pos.x - gubo.spotPositions[1].x));
        //theta = glm::atan(glm::abs(AssetVector[5].pos.z - gubo.spotPositions[2].z) , glm::abs(AssetVector[5].pos.x - gubo.spotPositions[2].x));
        theta = glm::radians(160.0f);
        gubo.spotDirection2 = glm::vec3(cos(theta), sin(theta), 0.0f);
        //theta = glm::atan(glm::abs(AssetVector[4].pos.z - gubo.spotPositions[3].z) , glm::abs(AssetVector[4].pos.x - gubo.spotPositions[3].x));
        theta = glm::radians(20.0f);
        gubo.spotDirection3 = glm::vec3(cos(theta), sin(theta), 0.0f);
        
        /*
        //per i quadri
        float theta = glm::radians(150.0f);
        gubo.spotDirection1 = glm::vec3(cos(theta), sin(theta), 0.0f);
        theta = glm::radians(30.0f);
        gubo.spotDirection2 = glm::vec3(cos(theta), sin(theta), 0.0f);
        */
        gubo.lightColor = glm::vec3(0.6f, 0.6f, 0.6f);
        gubo.ambColor = glm::vec3(0.1f, 0.1f, 0.1f);
        gubo.coneInOutDecayExp = glm::vec4(0.9f, 0.92f, 2.0f, 2.0f);

        if (curText == 0) {
            vkMapMemory(device, DS_GLOBAL.uniformBuffersMemory[0][currentImage], 0,
                sizeof(gubo), 0, &data);

            memcpy(data, &gubo, sizeof(gubo));
            vkUnmapMemory(device, DS_GLOBAL.uniformBuffersMemory[0][currentImage]);

            vkMapMemory(device, DS_GLOBAL.uniformBuffersMemory[0][currentImage], 0,
                sizeof(gubo), 0, &data);
            memcpy(data, &gubo, sizeof(gubo));
            vkUnmapMemory(device, DS_GLOBAL.uniformBuffersMemory[0][currentImage]);
        }
        else{
            vkMapMemory(device, DS_GLOBAL2.uniformBuffersMemory[0][currentImage], 0,
                sizeof(gubo), 0, &data);

            memcpy(data, &gubo, sizeof(gubo));
            vkUnmapMemory(device, DS_GLOBAL2.uniformBuffersMemory[0][currentImage]);

            vkMapMemory(device, DS_GLOBAL2.uniformBuffersMemory[0][currentImage], 0,
                sizeof(gubo), 0, &data);
            memcpy(data, &gubo, sizeof(gubo));
            vkUnmapMemory(device, DS_GLOBAL2.uniformBuffersMemory[0][currentImage]);
        }
            
        if (curText == 0) {
            for (int i = 0; i < numAssets; i++) {
                ubo.model = glm::translate(idMatrix, AssetVector[i].pos) *
                    glm::scale(idMatrix, glm::vec3(AssetVector[i].scale));

                //nearestObject != 0 (STRUCTURE) & mapping[.]=0 for each not defined asset
                if (isPopupShown && nearestObject * mapping[nearestObject] > 0 && i == mapping[nearestObject]) {
                    ubo.model = glm::translate(idMatrix, glm::vec3(camPos.x - 2 * sin(YPR.x), camPos.y, camPos.z - 2 * cos(YPR.x))) *
                        //glm::translate(idMatrix, AssetVector[i].pos) *
                        glm::scale(idMatrix, glm::vec3(AssetVector[i].scale)) *
                        glm::rotate(idMatrix, YPR.x, yAxis);

                }
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
        else{
                for (int i = 0; i < numAssets2; i++) {
                    if (i==0 || i == 1)
                        ubo.model = glm::translate(idMatrix, AssetVector2[i].pos) *
                            glm::scale(idMatrix, glm::vec3(AssetVector2[i].scale));
                    else
                        ubo.model = glm::rotate(idMatrix, glm::radians(270.0f) * time * planet_v[i],
                            yAxis) *
                            glm::translate(idMatrix, AssetVector2[i].pos) *
                            glm::scale(idMatrix, glm::vec3(AssetVector2[i].scale)) *
                            glm::rotate(idMatrix, glm::radians(270.0f) * time,
                                yAxis);

                    //nearestObject != 0 (STRUCTURE) & mapping[.]=0 for each not defined asset
                    if (isPopupShown && nearestObject * mapping[nearestObject] > 0 && i == mapping[nearestObject]) {
                        ubo.model = glm::translate(idMatrix, glm::vec3(camPos.x - 2 * sin(YPR.x), camPos.y, camPos.z - 2 * cos(YPR.x))) *
                            //glm::translate(idMatrix, AssetVector[i].pos) *
                            glm::scale(idMatrix, glm::vec3(AssetVector2[i].scale)) *
                            glm::rotate(idMatrix, YPR.x, yAxis);

                    }
                    /*
                     // @todo ruota intorno origine, deve ruotare l'oggetto
                      if(i == HERCULES){
                        ubo.model = ubo.model *
                                glm::rotate(idMatrix, glm::radians(270.0f) * time,
                                glm::vec3(0.0f, 1.0f, 0.0f));
                    }*/
                    vkMapMemory(device, componentsVector2[i].DS.uniformBuffersMemory[0][currentImage], 0,
                        sizeof(ubo), 0, &data);
                    memcpy(data, &ubo, sizeof(ubo));
                    vkUnmapMemory(device, componentsVector2[i].DS.uniformBuffersMemory[0][currentImage]);
                }
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

    bool checkOrientation(float obj_x, float obj_z, float my_x, float my_z, float orientation, int cw) {
        //function to check if object is visible from my point of view, cw indicates if orientation was posive 
        // (rotation clockwise) or negative (rotaion counter clockwise)
        bool isSeen = 0;
        obj_x = obj_x * cw;
        my_x = my_x * cw;
        if (orientation >= 5.25 && orientation <= 6.00 || orientation >= 0.0 && orientation <= 0.75 ) {
            if (my_z > obj_z) {
                isSeen = 1;
            }
        }
        if (orientation >= 0.75 && orientation <= 2.25) {
            if (my_x > obj_x) {
                isSeen = 1;
            }
        }
        if (orientation >= 2.25 && orientation <= 3.75) {
            if (my_z < obj_z) {
                isSeen = 1;
            }
        }
        if (orientation >= 3.75 && orientation <= 5.25) {
            if (my_x < obj_x) {
                isSeen = 1;
            }
        }
        return isSeen;
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