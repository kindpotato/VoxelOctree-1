

#include "OpenGL.h"
#include "Region.h"
#include <iostream>
#include "timer.h"
#include "Picture.h"
//#include "perlin.h"
#include <random>
#define LOG(x) std::cout << x << std::endl;
void showMaxDimensions();

int main()
{
    Region region;

    
    {
        struct Sphere {
            glm::uvec3 pos;
            unsigned int rad;
            unsigned int type;
        };
        std::mt19937 generator(6);
        std::uniform_int_distribution<unsigned int> spaceDist{ 10, 245 };
        std::uniform_int_distribution<unsigned int> typeDist{ 1,3};
        Sphere spheres[100];
        for (int i = 0; i < 100; ++i) {
            spheres[i].pos.x = spaceDist(generator);
            spheres[i].pos.y = spaceDist(generator);
            spheres[i].pos.z = spaceDist(generator);
            spheres[i].rad = spaceDist(generator) / 8;
            spheres[i].type = typeDist(generator);
        }
     
        unsigned int n = 0;
        for (int x = 0; x < 256; ++x) {
            for (int y = 0; y < 256; ++y) {
                for (int z = 0; z < 256; ++z) {
                    glm::uvec3 pos(x, y, z);
                    for (int i = 0; i < 30; ++i) {
                        glm::uvec3 to = spheres[i].pos - pos;
                        if (to.x*to.x + to.y*to.y + to.z*to.z < spheres[i].rad*spheres[i].rad) {
                            region.addBlock(pos,spheres[i].type);
                            ++n;
                            break;
                        }
                    }
            
                }
            }
        }
        std::cout << n << " blocks added." << std::endl;
        std::cout << n << " blocks added." << std::endl;

    }
    Player player;

    player.position = glm::vec3(20.f, 64.f, 20.f);
    GLFWwindow* window;
    {
        int x, y;
        std::cout << "Enter resolution.\nx: ";
        std::cin >> x;
        if (x == 0) x = 1280;
        std::cout << "y: ";
        std::cin >> y;
        if (y == 0) y = 720;
        window = initGLFW_GLAD("Voxel Raytracer", x, y);
    }
    //Image Buffer (for compute shader to write and for fragment shader to read)
    int tex_w = windowSize.x, tex_h = windowSize.y;
    GLuint tex_output;
    glGenTextures(1, &tex_output);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex_output);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, tex_w, tex_h, 0, GL_RGBA, GL_FLOAT, NULL);
    glBindImageTexture(0, tex_output, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);


    GLuint backBuffer;
    glGenTextures(1, &backBuffer);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, backBuffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, tex_w, tex_h, 0, GL_RGBA, GL_FLOAT, NULL);

    Picture pic("wood.jpg");
    //Block Texture
    GLuint blockTexture;
    glGenTextures(1, &blockTexture);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, blockTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    pic.generateTexture();

    showMaxDimensions();
    float verts[] = {
        -1.f,-1.f,    0.f,0.f,
        1.f,-1.f,    1.f,0.f,
        -1.f,1.f,    0.f,1.f,
        
        -1.f,1.f,    0.f,1.f,
        1.f,-1.f,    1.f,0.f,
        1.f,1.f,    1.f,1.f
    };
    
    unsigned char* blockBuffer = region.serializeChunks();

    unsigned int blockDataObject;
    glGenBuffers(1, &blockDataObject);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, blockDataObject);
    glBufferData(GL_SHADER_STORAGE_BUFFER, Region::size*Chunk::size, blockBuffer, GL_STATIC_DRAW);
    delete[] blockBuffer;
    
   // glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, Region::size * Chunk::size, blockBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, blockDataObject); //1 is the binding location
   // glBindBuffer(GL_SHADER_STORAGE_BUFFER, blockDataObject);
    GLvoid* p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, blockDataObject);
    //memcpy(p, &(blockBuffer[0]), Region::size * Chunk::size);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);



    unsigned int quadVAO;
    glGenVertexArrays(1, &quadVAO);
    glBindVertexArray(quadVAO);

    unsigned int quadVBO;
    glGenBuffers(1, &quadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), (&verts[0]), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    (GL_ARRAY_BUFFER, quadVBO);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,4*sizeof(float),(void*)0);
    glEnableVertexAttribArray(1);
    glad_glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
   

    GLuint vertShader = makeShader("vert.glsl", GL_VERTEX_SHADER);
    GLuint fragShader = makeShader("frag.glsl", GL_FRAGMENT_SHADER);
    GLuint quadProgram = makeProgram(vertShader, fragShader);

    GLuint shader = makeShader("raytracer.glsl", GL_COMPUTE_SHADER);
    GLuint program = makeProgram(shader);

    GLuint resolutionUniform = glGetUniformLocation(program, "resolution");
    GLuint timeUniform = glGetUniformLocation(program, "frame");
    GLuint camPosUniform = glGetUniformLocation(program, "camPos");
    GLuint camMatUniform = glGetUniformLocation(program, "camMat");
    GLuint lastcamMatUniform = glGetUniformLocation(program, "lastcamMat");
    GLuint lastcamPosUniform = glGetUniformLocation(program, "lastcamPos");


    glm::mat3 lastcamMat = player.camera.getMatrix();
    glm::vec3 lastcamPos = player.camera.pos;

    //unsigned int sunDirUniform = glGetUniformLocation(program, "sunDir");

    std::string saveFileName = "";


    auto lastTime = std::chrono::system_clock::now();
    Timer timer(100);

    while (!glfwWindowShouldClose(window)) {
        timer.tick();
        {
            glm::mat3 cameraMatrix = player.camera.getMatrix();

            glUseProgram(program);
            glUniform2f(resolutionUniform, windowSize.x, windowSize.y);
            glUniformMatrix3fv(camMatUniform, 1, GL_FALSE, glm::value_ptr(glm::transpose(cameraMatrix)));
            glUniformMatrix3fv(lastcamMatUniform, 1, GL_FALSE, glm::value_ptr(lastcamMat));
            glm::vec3 camPos = player.camera.pos;
            glUniform3f(camPosUniform, camPos.x, camPos.y, camPos.z);
            glUniform3f(lastcamPosUniform, lastcamPos.x, lastcamPos.y, lastcamPos.z);
            glUniform1ui(timeUniform, timer.step);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, blockTexture);
            
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, backBuffer);
            const GLuint work = 32;
            glDispatchCompute((GLuint)tex_w / work + (tex_w%work != 0), (GLuint)tex_h / work + (tex_h%work != 0), 1);

            
            //lastcamMat = cameraMatrix;
            lastcamPos = camPos;
            lastcamMat = cameraMatrix;
        }
        { //doing this here while the gpu computes
            
            glfwPollEvents();
            player.handleInputs(window);
            player.update();
            if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)) {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }

            /*if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) && GLFS_PRESS == glfwGetKey(window, GLFW_KEY_S)) {
                if(saveFileName.empty()){
                    std::cout << "Enter file name: ";
                    std::cin >> saveFileName;
                    region.save(saveFileName);
                }
                region.save();
            }*/
            
        }
        glm::ivec2 windowPos;
        glfwGetWindowPos(window, &windowPos.x, &windowPos.y);
        // make sure writing to image has finished before read
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
       
        if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_F1)) {
            shader = makeShader("raytracer.glsl", GL_COMPUTE_SHADER);
            program = makeProgram(shader);
           resolutionUniform = glGetUniformLocation(program, "resolution");
           timeUniform = glGetUniformLocation(program, "frame");
           camPosUniform = glGetUniformLocation(program, "camPos");
           camMatUniform = glGetUniformLocation(program, "camMat");
           lastcamMatUniform = glGetUniformLocation(program, "lastcamMat");
           lastcamPosUniform = glGetUniformLocation(program, "lastcamPos");
        }
        

        { // normal drawing pass
            glClear(GL_COLOR_BUFFER_BIT);
            glUseProgram(quadProgram);
            glBindVertexArray(quadVAO);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, tex_output);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        //Copies tex_output to backbuffer
        glCopyImageSubData(tex_output,GL_TEXTURE_2D,0, 0,0,0,backBuffer,GL_TEXTURE_2D,0,0,0,0,tex_w,tex_h,1);
        
        glm::vec3 chunkPos = region.checkPlayer(player);
        if (chunkPos.x != 1000) {
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, blockDataObject);
            glBufferSubData(GL_SHADER_STORAGE_BUFFER, Chunk::size * Region::vtoidx(chunkPos), Chunk::size, region.getChunk(chunkPos)->space);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        }
        glfwSwapBuffers(window);
       
    }


    return 0;
}

void showMaxDimensions() {
    int work_grp_cnt[3];

    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_cnt[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_cnt[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_cnt[2]);

    printf("max global (total) work group counts x:%i y:%i z:%i\n",
        work_grp_cnt[0], work_grp_cnt[1], work_grp_cnt[2]);

    int work_grp_size[3];

    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &work_grp_size[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &work_grp_size[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &work_grp_size[2]);

    printf("max local (in one shader) work group sizes x:%i y:%i z:%i\n",
        work_grp_size[0], work_grp_size[1], work_grp_size[2]);

}