
/**
  * Example of attaching an imgui based gui
  */

#include "util/gfx_imgui_app.h"

using namespace gfx;

struct MyApp : GFXAppImGui
{

  MBO mbo;

  float f0, f1;
  bool bToggle;
  int e;

  bool bShowColor, bShowNomp;


  void setup ()
  {
    mbo = Mesh::Circle ();
    mRenderGraph.immediate (true);

    wflags |= ImGuiWindowFlags_MenuBar;
  }

  void onDraw ()
  {
    draw (mbo, 1, f0, 0);
  }

  void onDrawGui()
  {

    if(ImGui::BeginMenuBar()){
      if(ImGui::BeginMenu("Examples")){
         ImGui::MenuItem("Color", NULL, &bShowColor);
         ImGui::MenuItem("Nomp", NULL, &bShowNomp);
         ImGui::EndMenu();
      }
      ImGui::EndMenuBar();
    }

    ImGui::Button("Button");
    ImGui::Checkbox("checkbox", &bToggle);
    ImGui::InputFloat("input float", &f0, 0.01f, 1.0f, "%.3f");
    ImGui::RadioButton("radio a", &e, 0); ImGui::SameLine();
    ImGui::RadioButton("radio b", &e, 1); ImGui::SameLine();
    ImGui::RadioButton("radio c", &e, 2);
    ImGui::SliderFloat("slider float", &f1, -20, 20, "%.3f");
    ImGui::DragFloat("slider float", &f1, -20, 20, "%.3f");

  }

};


int main ()
{

  MyApp app;
  app.start ();
  return 0;
}
