
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

  ImGuiWindowFlags wflags = 0;

  int iBool, iFloat = 0;
  
   GFXAppImGui (int w = 800, int h = 600, string name = "gfx with imgui")
      : GFXApp<GLFWContext> (w, h, name)
  {
  }

  //called before START, overloaded with additional imgui setups
  //note, subclasses should still call "setup" themselves
  void _setup(){
    printf("set up IMGUI\n");
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

    setup();
 }

  //called by App::onRender
  void onEarlyRender(){
   ImGui_ImplOpenGL2_NewFrame();
   ImGui_ImplGlfw_NewFrame();
   ImGui::NewFrame();
  }

  //override to use your own settings
  virtual bool guiBegin(){
    iBool = iFloat = 0;
    if (!ImGui::Begin("Gui",NULL, wflags))
    {
        ImGui::End();
        return false;
    }
    ImGui::PushItemWidth(ImGui::GetFontSize()* -5);
    return true;
  }

  //override to use your own settings
  virtual void guiEnd(){
    ImGui::PopItemWidth();
    ImGui::End();
  }

  //helper bool func
  void gui(bool& val, string nm = ""){
       stringstream name;

       if (nm == "" )
       {
         name << "b_" << iBool;
         iBool++;
       }
       else name << nm;

       ImGui::Checkbox(name.str().c_str(), &val);

  }

  //helper float slider func
  void gui(float& val, string nm = "", float min = 0, float max = 1){

       stringstream name;

       if (nm == "" )
       {
           name << "float_" << iFloat;
           iFloat++;
       }
       else name << nm;

       ImGui::SliderFloat(name.str().c_str(), &val, min, max, "%.4f");

  }


  virtual bool onGuiBegin(){
    iBool = iFloat = 0;
    if (!ImGui::Begin("Test",NULL, wflags))
    {
        ImGui::End();
        return false;
    }
    ImGui::PushItemWidth(ImGui::GetFontSize() * -5);
    return true;
  };

  virtual void onDrawGui(){};

  virtual void onGuiEnd(){
    ImGui::PopItemWidth();
    ImGui::End();
  };


  void _onDraw(){
    if (onGuiBegin()){
      onDrawGui();
      onGuiEnd();
    }
    onDraw();
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
