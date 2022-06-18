// This has been adapted from the Vulkan tutorial

#include "MyProject.hpp"

//MODEL
//const std::string MODEL_PATH = "models/viking_room.obj";
//const std::string MODEL_PATH = "models/statues/davidStatue.obj";
//const std::string MODEL_PATH = "models/statues/hercules.obj";
//const std::string DISCO_MODEL_PATH = "models/misc/doors.obj";

//@todo WallsAndFloor2 ha soffitto
const std::string STRUCTURE_MODEL_PATH = "models/misc/WallsAndFloor2.obj"; //"models/misc/WallsAndFloor2.obj";
//const std::string VENUS_MODEL_PATH = "models/statues/venus.obj";
//const std::string DISCO_MODEL_PATH = "models/statues/discobolus.obj";

const std::string VENUS_MODEL_PATH = "models/paints/Frames.obj";
const std::string DISCO_MODEL_PATH = "models/paints/Munch.obj";

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
//const std::string VENUS_TEXTURE_PATH = "textures/statues/statue_venus.jpg";
//const std::string DISCO_TEXTURE_PATH = "textures/statues/discobolusTexture.png";

const std::string VENUS_TEXTURE_PATH = "textures/paints/T_picture_frame_BaseColor.tga";

const std::string DISCO_TEXTURE_PATH = "textures/paints/Munch_Scream.jpg";
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
};

struct UniformBufferObject {
    alignas(16) glm::mat4 model;

};

const glm::mat4 idMatrix = glm::mat4(1);
const glm::vec3 xAxis = glm::vec3(1, 0, 0);
const glm::vec3 yAxis = glm::vec3(0, 1, 0);
const glm::vec3 zAxis = glm::vec3(0, 0, 1);


// MAIN ! 
class MyProject : public BaseProject {
	protected:
    //
    glm::vec3 camPos = glm::vec3(2.0f, 2.0f, 2.0f);

	// Here you list all the Vulkan objects you need:
	
	// Descriptor Layouts [what will be passed to the shaders]
	DescriptorSetLayout DSLGlobal;
    DescriptorSetLayout DSLObj;

	// Pipelines [Shader couples]
	Pipeline P1;

    DescriptorSet DS_GLOBAL;

    Model M_STRUCTURE;
    Texture T_STRUCTURE;
    DescriptorSet DS_STRUCTURE;

	// Models, textures and Descriptors (values assigned to the uniforms)
	Model M_VENUS;
	Texture T_VENUS;
	DescriptorSet DS_VENUS; //!= positioning for same copy of obj need != DS

    Model M_DISCO;
    Texture T_DISCO;
    DescriptorSet DS_DISCO;



	// Here you set the main application parameters
	void setWindowParameters() {
		// window size, titile and initial background
		windowWidth = 800;
		windowHeight = 600;
		windowTitle = "My Project";
		initialBackgroundColor = {0.0f, 0.0f, 0.0f, 1.0f};
		
		// Descriptor pool sizes -> modify if add other models
		uniformBlocksInPool = 4;
		texturesInPool = 3;
		setsInPool = 8;
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
		P1.init(this, "shaders/vert.spv", "shaders/frag.spv", {&DSLGlobal, &DSLObj});

		// Models, textures and Descriptors (values assigned to the uniforms)
		M_VENUS.init(this, VENUS_MODEL_PATH);
		T_VENUS.init(this, VENUS_TEXTURE_PATH);
        //M2, T2 AND DS2 for other obj 2 and init (same DS_layout -> same positioning)
		DS_VENUS.init(this, &DSLObj, {
		// the second parameter, is a pointer to the Uniform Set Layout of this set
		// the last parameter is an array, with one element per binding of the set.
		// first  elmenet : the binding number
		// second element : UNIFORM or TEXTURE (an enum) depending on the type
		// third  element : only for UNIFORMs, the size of the corresponding C++ object
		// fourth element : only for TEXTUREs, the pointer to the corresponding texture object
					{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
					{1, TEXTURE, 0, &T_VENUS}
				});

        M_DISCO.init(this, DISCO_MODEL_PATH);
        T_DISCO.init(this, DISCO_TEXTURE_PATH);
        DS_DISCO.init(this, &DSLObj, {
                {0, UNIFORM, sizeof(UniformBufferObject), nullptr},
                {1, TEXTURE, 0, &T_DISCO}
        });

        M_STRUCTURE.init(this, STRUCTURE_MODEL_PATH);
        T_STRUCTURE.init(this, STRUCTURE_TEXTURE_PATH);
        DS_STRUCTURE.init(this, &DSLObj, {
                {0, UNIFORM, sizeof(UniformBufferObject), nullptr},
                {1, TEXTURE, 0, &T_STRUCTURE}
        });

        DS_GLOBAL.init(this, &DSLGlobal, {
                {0, UNIFORM, sizeof(GlobalUniformBufferObject), nullptr}
        });
	
	}

	void localReInit() {
		// Pipelines [Shader couples]
		// The last array, is a vector of pointer to the layouts of the sets that will
		// be used in this pipeline. The first element will be set 0, and so on..
		P1.reinit(this, "shaders/vert.spv", "shaders/frag.spv", { &DSLGlobal, &DSLObj }); //maybe reinit is equal to init at the end, check if needs another moethod
		
	}

	// Here you destroy all the objects you created!		
	void localCleanup() {
		DS_VENUS.cleanup();
		T_VENUS.cleanup();
		M_VENUS.cleanup();

        DS_DISCO.cleanup();
        T_DISCO.cleanup();
        M_DISCO.cleanup();

        DS_STRUCTURE.cleanup();
        T_STRUCTURE.cleanup();
        M_STRUCTURE.cleanup();

        DS_GLOBAL.cleanup();

		P1.cleanup();
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


        //@todo vector per offsets e vertexBuff? e indicizziamo con dictionary
        //We need differend command buffer and index buffer
        /*FROM HERE*/
		VkBuffer vertexBuffers[] = {M_VENUS.vertexBuffer};
		// property .vertexBuffer of models, contains the VkBuffer handle to its vertex buffer
		VkDeviceSize offsets[] = {0};
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
		// property .indexBuffer of models, contains the VkBuffer handle to its index buffer
		vkCmdBindIndexBuffer(commandBuffer, M_VENUS.indexBuffer, 0,
								VK_INDEX_TYPE_UINT32);

		// property .pipelineLayout of a pipeline contains its layout.
		// property .descriptorSets of a descriptor set contains its elements.
		vkCmdBindDescriptorSets(commandBuffer,
						VK_PIPELINE_BIND_POINT_GRAPHICS,
						P1.pipelineLayout, 1, 1, &DS_VENUS.descriptorSets[currentImage],
						0, nullptr);



		// property .indices.size() of models, contains the number of triangles * 3 of the mesh.
		vkCmdDrawIndexed(commandBuffer,
					static_cast<uint32_t>(M_VENUS.indices.size()), 1, 0, 0, 0);
        /*TO HERE duplicated for each obj*/

        VkBuffer vertexBuffersDisco[] = {M_DISCO.vertexBuffer};
        // property .vertexBuffer of models, contains the VkBuffer handle to its vertex buffer
        VkDeviceSize offsetsDisco[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffersDisco, offsetsDisco);
        // property .indexBuffer of models, contains the VkBuffer handle to its index buffer
        vkCmdBindIndexBuffer(commandBuffer, M_DISCO.indexBuffer, 0,
                             VK_INDEX_TYPE_UINT32);
        vkCmdBindDescriptorSets(commandBuffer,
                                VK_PIPELINE_BIND_POINT_GRAPHICS,
                                P1.pipelineLayout, 1, 1, &DS_DISCO.descriptorSets[currentImage],
                                0, nullptr);
        vkCmdDrawIndexed(commandBuffer,
                         static_cast<uint32_t>(M_DISCO.indices.size()), 1, 0, 0, 0);


        VkBuffer vertexBuffersStruct[] = {M_STRUCTURE.vertexBuffer};
        VkDeviceSize offsetsStruct[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffersStruct, offsetsStruct);
        vkCmdBindIndexBuffer(commandBuffer, M_STRUCTURE.indexBuffer, 0,
                             VK_INDEX_TYPE_UINT32);
        vkCmdBindDescriptorSets(commandBuffer,
                                VK_PIPELINE_BIND_POINT_GRAPHICS,
                                P1.pipelineLayout, 1, 1, &DS_STRUCTURE.descriptorSets[currentImage],
                                0, nullptr);
        vkCmdDrawIndexed(commandBuffer,
                         static_cast<uint32_t>(M_STRUCTURE.indices.size()), 1, 0, 0, 0);

	}


    float computeDeltaTime(){
        // Compute time
        static auto startTime = std::chrono::high_resolution_clock::now();
        static float lastTime = 0.0f;
        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>
                (currentTime-startTime).count();
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
        float rx = 0;
        float ry = 0;
        float rz = 0;
        float mx = 0;
        float my = 0;
        float mz = 0;
        static glm::vec3 YPR = glm::vec3(glm::radians(0.0f), 0.0f, glm::radians(0.0f));

        static glm::vec3 ux = glm::vec3(glm::rotate(idMatrix, YPR.x,yAxis) *
                                        glm::vec4(1,0,0,1));
        static glm::vec3 uy = yAxis;
        static glm::vec3 uz = glm::vec3(glm::rotate(idMatrix, YPR.x,yAxis) *
                                        glm::vec4(0,0,-1,1));

        omega = 1; //[Rad/s]
        mu = 1; //[unit/s]

        dt = computeDeltaTime();

        glm::vec3 oldPos = camPos;
        if(glfwGetKey(window, GLFW_KEY_LEFT)) {
            YPR.x += dt * omega;
        }
        if(glfwGetKey(window, GLFW_KEY_RIGHT)) {
            YPR.x -= dt * omega;
        }
        if(glfwGetKey(window, GLFW_KEY_UP)) {
            YPR.y += dt * omega;
        }
        if(glfwGetKey(window, GLFW_KEY_DOWN)) {
            YPR.y -= dt * omega;
        }
        if(glfwGetKey(window, GLFW_KEY_Q)) {
            YPR.z -= dt * omega;
        }
        if(glfwGetKey(window, GLFW_KEY_E)) {
            YPR.z += dt * omega;
        }
        if(glfwGetKey(window, GLFW_KEY_A)) {
            camPos -= mu * glm::vec3(glm::rotate(glm::mat4(1.0f), YPR.x,
                                                           glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(1,0,0,1)) * dt;
        }
        if(glfwGetKey(window, GLFW_KEY_D)) {
            camPos += mu * glm::vec3(glm::rotate(glm::mat4(1.0f), YPR.x,
                                                           glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(1,0,0,1)) * dt;
        }
        if(glfwGetKey(window, GLFW_KEY_W)) {
            camPos -= mu * glm::vec3(glm::rotate(glm::mat4(1.0f), YPR.x,
                                                           glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(0,0,1,1)) * dt;
        }
        if(glfwGetKey(window, GLFW_KEY_S)) {
            camPos += mu * glm::vec3(glm::rotate(glm::mat4(1.0f), YPR.x,
                                                           glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(0,0,1,1)) * dt;
        }


        /*@todo implement canStep to enable stopping the movement (A06.cpp)
        if(!canStep(camPos.x, camPos.z)) {
            camPos = oldPos;
        }
        */

        gubo.view = LookInDirMat(camPos, YPR);

        /*
		gubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f),
							   glm::vec3(0.0f, 0.0f, 0.0f),
							   glm::vec3(0.0f, 0.0f, 1.0f));
        */


		gubo.proj = glm::perspective(glm::radians(45.0f),
						swapChainExtent.width / (float) swapChainExtent.height,
						0.1f, 50.0f);
		gubo.proj[1][1] *= -1;

        vkMapMemory(device, DS_GLOBAL.uniformBuffersMemory[0][currentImage], 0,
                    sizeof(gubo), 0, &data);
        memcpy(data, &gubo, sizeof(gubo));
        vkUnmapMemory(device, DS_GLOBAL.uniformBuffersMemory[0][currentImage]);

        //For venus
        ubo.model = idMatrix;/*glm::rotate(glm::mat4(1.0f),
                                glm::radians(270.0f), //*time
                                glm::vec3(1.0f, 0.0f, 0.0f))*
                    glm::translate(glm::mat4(3.0f), glm::vec3(-1.5f, 0.0f, 0.0f));
                    */
		// Here is where you actually update your uniforms
        //Also duplicated!
		vkMapMemory(device, DS_VENUS.uniformBuffersMemory[0][currentImage], 0,
							sizeof(ubo), 0, &data);
		memcpy(data, &ubo, sizeof(ubo));
		vkUnmapMemory(device, DS_VENUS.uniformBuffersMemory[0][currentImage]);

        //For discobolus
        ubo.model = idMatrix; /*glm::rotate(glm::mat4(1.0f),
                                glm::radians(270.0f), //*time
                                glm::vec3(1.0f, 0.0f, 0.0f)) *
                                        glm::translate(glm::mat4(3.0f), glm::vec3(1.5f, 0.0f, 0.0f));
        */
        // Here is where you actually update your uniforms
        //Also duplicated!
        vkMapMemory(device, DS_DISCO.uniformBuffersMemory[0][currentImage], 0,
                    sizeof(ubo), 0, &data);
        memcpy(data, &ubo, sizeof(ubo));
        vkUnmapMemory(device, DS_DISCO.uniformBuffersMemory[0][currentImage]);


        //For Structure
        ubo.model = idMatrix;
        vkMapMemory(device, DS_STRUCTURE.uniformBuffersMemory[0][currentImage], 0,
                    sizeof(ubo), 0, &data);
        memcpy(data, &ubo, sizeof(ubo));
        vkUnmapMemory(device, DS_STRUCTURE.uniformBuffersMemory[0][currentImage]);


	}	
};

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