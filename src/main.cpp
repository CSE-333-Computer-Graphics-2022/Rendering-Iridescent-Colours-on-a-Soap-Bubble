// CG Project - Soap Bubble

#include "imgui_setup.h"
#include "camera.h"
#include "renderengine.h"
#include "world.h"
#include "material.h"
#include "object.h"
#include "sphere.h"
#include "triangle.h"
#include "lightsource.h"
#include "pointlightsource.h"
#include "PerlinNoise.h"
#include<iostream>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../depends/stb/stb_image.h"
#include "../depends/stb/stb_image_write.h"

#define RENDER_BATCH_COLUMNS 100 // Number of columns to render in a single go. Increase to gain some display/render speed!

Camera *camera;
RenderEngine *engine;

int screen_width = 800, screen_height = 600; // This is window size, used to display scaled raytraced image.
int image_width = 1920, image_height = 1080; // This is raytraced image size. Change it if needed.
GLuint texImage;

int main(int, char**)
{
    // Setup window
    GLFWwindow *window = setupWindow(screen_width, screen_height);

    ImVec4 clearColor = ImVec4(1.0f, 1.0f, 1.0f, 1.00f);

    // Setup raytracer camera. This is used to spawn rays.
    Vector3D camera_position(0, 0, 10);
    Vector3D camera_target(0, 0, 0); //Looking down -Z axis
    Vector3D camera_up(0, 1, 0);
    float camera_fovy =  45;
    camera = new Camera(camera_position, camera_target, camera_up, camera_fovy, image_width, image_height);

    //Create a world
    World *world = new World;
    world->setAmbient(Color(1));
    world->pn = PerlinNoise(2);
    world->setBackground(Color(0.0, 0.0, 0.0));

    Material *m = new Material(world);
    m->color = Color(0.1, 0.1, 0.2);
    m->ka = 0.1;
    m->kd = 0.9;
    m->ks = 1;
    m->n = 64;
    Object *triangle = new Triangle(Vector3D(-10, -7, 0), Vector3D(15, -7, 0), Vector3D(-7, -4, -50) , m);
    world->addObject(triangle);

    //outer bubble
    Material *m2 = new Material(world);
    m2->color = Color(1, 1, 1);
    m2->ka = 0.1;
    m2->kd = 0.9;
    m2->ks = 5;
    m2->n = 32;
    m2->eta = 1.3;
    m2->km = 2.2;
    Object *sphere = new Sphere(Vector3D(1.5, 0.0, -7.5), 5, m2);
    sphere->isDielectric();
    world->addObject(sphere);

    //inner bubble
    Material *m3 = new Material(world);
    m3->color = Color(1, 1, 1);
    m3->ka = 0.1;
    m3->kd = 0.9;
    m3->ks = 5;
    m3->n = 32;
    m3->eta = 1.0;
    m3->km = 2.0;
    Object *sphere2 = new Sphere(Vector3D(1.5, 0.0009, -7.5), 4.999, m3);
    sphere2->isDielectric();
    world->addObject(sphere2);

    //red sphere
    Material *m4 = new Material(world);
    m4->color = Color(0.7, 0.1, 0.1);
    m4->ka = 0.1;
    m4->kd = 0.9;
    m4->ks = 1;
    m4->n = 64;
    Object *sphere3 = new Sphere(Vector3D(-3.5, 0.0, 1), 1, m4);
    world->addObject(sphere3);

    LightSource *light = new PointLightSource(world, Vector3D(-10, 5, 0), Color(1, 1, 1));
    world->addLight(light);
    LightSource *light2 = new PointLightSource(world, Vector3D(20, 20, 20), Color(1, 1, 1));
    world->addLight(light2);

    engine = new RenderEngine(world, camera);

    //Initialise texture
    glGenTextures(1, &texImage);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texImage);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, camera->getBitmap());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        bool render_status;
        for(int i=0; i<RENDER_BATCH_COLUMNS; i++) 
            render_status = engine->renderLoop(); // RenderLoop() raytraces 1 column of pixels at a time.
        if(!render_status)
        {
            // Update texture
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texImage);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image_width, image_height, GL_RGB, GL_UNSIGNED_BYTE, camera->getBitmap());
        } 

        ImGui::Begin("Lumina", NULL, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("Size: %d x %d", image_width, image_height);
        if(ImGui::Button("Save")){
          char filename[] = "img.png";
          stbi_write_png(filename, image_width, image_height, 3, camera->getBitmap(),0);        
        }
        //Display render view - fit to width
        int win_w, win_h;
        glfwGetWindowSize(window, &win_w, &win_h);
        float image_aspect = (float)image_width/(float)image_height;
        float frac = 0.95; // ensure no horizontal scrolling
        ImGui::Image((void*)(intptr_t)texImage, ImVec2(frac*win_w, frac*win_w/image_aspect), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));

        ImGui::End();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    glDeleteTextures(1, &texImage);

    cleanup(window);

    return 0;
}
