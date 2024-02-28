
/*
 *
 *       Filename:  gfx_imgui_app.h
 *
 *    Description:  an app with imgui
 *
 *        Version:  1.0
 *        Created:  01/27/2015 15:51:04
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *
 */

#ifndef gfx_imgui_app_INC
#define gfx_imgui_app_INC

#include "gfx/util/glfw_window.hpp"
#include "gfx/gfx_app.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl2.h"

namespace gfx {

struct GFXAppImGui : public GFXApp<GLFWContext>
{

  GFXAppImGui (int w = 400, int h = 400, string name = "gfx with imgui")
      : GFXApp<GLFWContext> (w, h, name)
  {
  }

  //called before START
  void setup(){
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch
    
    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(&mContext.window(), true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL2_Init();
  }

  //called everyframe 
//  void onFrame(){
  void onDraw(){
   ImGui_ImplOpenGL2_NewFrame();
   ImGui_ImplGlfw_NewFrame();
   ImGui::NewFrame();
//
   ImGui::ShowDemoWindow(); // Show demo window! :)
//                             //
//  //  GFXApp<CONTEXT>::onFrame();
//    //tthis should maybe happen after other scenes draw
    onLateRender();
  }

  void onLateRender(){
    ImGui::Render();
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
  }

  void onDestroy(){
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
  }


};

}  //gfx::

#endif /* ----- #ifndef gfx_glv_app_INC  ----- */
