// This has been adapted from the Vulkan tutorial

#include "MyProject.hpp"

//MODEL
//const std::string MODEL_PATH = "models/viking_room.obj";
//const std::string MODEL_PATH = "models/statue_prova.obj";
//const std::string MODEL_PATH = "models/hercules.obj";
const std::string VENUS_MODEL_PATH = "models/venus.obj";
const std::string DISCO_MODEL_PATH = "models/discobolus.obj";

//TEXTURE
//const std::string TEXTURE_PATH = "textures/viking_room.png";
//const std::string TEXTURE_PATH = "textures/DavidFixedDiff.jpg";
//const std::string TEXTURE_PATH = "textures/hercules.jpg";
const std::string VENUS_TEXTURE_PATH = "textures/statue_venus.jpg";
const std::string DISCO_TEXTURE_PATH = "textures/manstatue.png";


// The uniform buffer object used in this example
//IT may be splitted in {view, proj}(set0, binding0) and {model}(set1, binding0)
// among with {texture} (set1, binding0)
//need change in DSL init (use obj=set1 and make it retrivable by all shaders I.e. ...ALL_GRAPHICS),
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


// MAIN ! 
class MyProject : public BaseProject {
	protected:
	// Here you list all the Vulkan objects you need:
	
	// Descriptor Layouts [what will be passed to the shaders]
	DescriptorSetLayout DSLGlobal;
    DescriptorSetLayout DSLObj;

	// Pipelines [Shader couples]
	Pipeline P1;

    DescriptorSet DS_GLOBAL;

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
		uniformBlocksInPool = 3;
		texturesInPool = 2;
		setsInPool = 3;
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
        //M2, T2 AND DS2 for other obj 2 and init (same DS_layout -> same positioning)
        DS_DISCO.init(this, &DSLObj, {
                {0, UNIFORM, sizeof(UniformBufferObject), nullptr},
                {1, TEXTURE, 0, &T_DISCO}
        });

        DS_GLOBAL.init(this, &DSLGlobal, {
                {0, UNIFORM, sizeof(GlobalUniformBufferObject), nullptr}
        });

	}

	// Here you destroy all the objects you created!		
	void localCleanup() {
		DS_VENUS.cleanup();
		T_VENUS.cleanup();
		M_VENUS.cleanup();

        DS_DISCO.cleanup();
        T_DISCO.cleanup();
        M_DISCO.cleanup();

        DS_GLOBAL.cleanup();

		P1.cleanup();
		DSLGlobal.cleanup();
        DSLObj.cleanup();
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

	}

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


		gubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f),
							   glm::vec3(0.0f, 0.0f, 0.0f),
							   glm::vec3(0.0f, 0.0f, 1.0f));
		gubo.proj = glm::perspective(glm::radians(45.0f),
						swapChainExtent.width / (float) swapChainExtent.height,
						0.1f, 10.0f);
		gubo.proj[1][1] *= -1;

        vkMapMemory(device, DS_GLOBAL.uniformBuffersMemory[0][currentImage], 0,
                    sizeof(gubo), 0, &data);
        memcpy(data, &gubo, sizeof(gubo));
        vkUnmapMemory(device, DS_GLOBAL.uniformBuffersMemory[0][currentImage]);

        //For venus
        ubo.model = glm::rotate(glm::mat4(1.0f),
                                time * glm::radians(90.0f),
                                glm::vec3(0.0f, 0.0f, 1.0f))*
                    glm::translate(glm::mat4(3.0f), glm::vec3(-1.5f, 0.0f, 0.0f));;
		// Here is where you actually update your uniforms
        //Also duplicated!
		vkMapMemory(device, DS_VENUS.uniformBuffersMemory[0][currentImage], 0,
							sizeof(ubo), 0, &data);
		memcpy(data, &ubo, sizeof(ubo));
		vkUnmapMemory(device, DS_VENUS.uniformBuffersMemory[0][currentImage]);

        //For discobolus
        ubo.model = glm::rotate(glm::mat4(1.0f),
                                time * glm::radians(90.0f),
                                glm::vec3(0.0f, 0.0f, 1.0f)) *
                                        glm::translate(glm::mat4(3.0f), glm::vec3(1.5f, 0.0f, 0.0f));
        // Here is where you actually update your uniforms
        //Also duplicated!
        vkMapMemory(device, DS_DISCO.uniformBuffersMemory[0][currentImage], 0,
                    sizeof(ubo), 0, &data);
        memcpy(data, &ubo, sizeof(ubo));
        vkUnmapMemory(device, DS_DISCO.uniformBuffersMemory[0][currentImage]);


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