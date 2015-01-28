/*
 * =====================================================================================
 *
 *       Filename:  gfx_sceneController.h
 *
 *    Description:  A utility for navigating virtual environment with mouse and keyboard input
 *
 *        Version:  1.0
 *        Created:  06/09/2014 17:50:12
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail -> wolftype
 *   Organization:  
 *
 * =====================================================================================
 */
/*-----------------------------------------------------------------------------
 *  CONTROL OF SCENE MATRICES WITH MOUSE AND KEYBOARD 
 *-----------------------------------------------------------------------------*/

#ifndef GFX_SCENECONTROLLER_H_INCLUDED
#define GFX_SCENECONTROLLER_H_INCLUDED

#include "gfx_scene.h"
#include "gfx_control.h"
#include <sstream> 
#include <map>

namespace gfx{


    class SceneController : public InputEventHandler {

      Scene * mScene; 
      GFXio * mIO;

      Quat tModelRot, tCameraRot;                               ///< store temporary here.
      
      float mRotVel, mVel, mModelRotVel;

     public:     
  
        SceneController( Scene * s = NULL ) : 
        mScene(s), mRotVel(.02), mVel(.1), mModelRotVel(.1) 
        {};  
        
        Scene& scene() { return *mScene; }
        void scene(Scene *s ){ mScene=s; }
         
        GFXio& io() { return *mIO; }
        void io(GFXio * io) { mIO=io; }
                            
        void inputCalc();        ///< Calculate Mouse Movements based on x and dx
        void viewCalc();         ///< Calculate Window Matrices, Screen Coordinates of Mouse


        ViewData& vd()  { return io().viewdata; }
        Keyboard& kd()  { return io().keyboard; }
        Mouse& md()     { return io().mouse; }

        Camera& camera() { return mScene -> camera; }    
        MPose& model() { return mScene -> model; }
        
        /*-----------------------------------------------------------------------------
         *  NAVIGATION
         *-----------------------------------------------------------------------------*/
        void keyboardCamSpin(float acc, bool trigger);
        void keyboardCamTranslate(float acc, bool trigger);
        void keyboardModelTransform(float acc, bool trigger);
        void keyboardNavigate();
        void keyboardNavigateStop();

        void mouseCamSpin(float acc, bool trigger);  
        void mouseCamTranslate(float acc, bool trigger);
        
        void mouseModelTransform_(float acc, bool trigger);  
        void mouseModelTransform();

        void mouseNavigate();
        void mouseNavigateStop();   
  
        virtual void windowTransform(){} 
        virtual void onKeyDown(const Keyboard&);   
        virtual void onKeyUp(const Keyboard&);   
        virtual void onMouseDown(const Mouse&);
        virtual void onMouseUp(const Mouse&);
        virtual void onMouseMove(const Mouse&);
        virtual void onMouseDrag(const Mouse&);
      
};   

    void SceneController :: onMouseDown(const Mouse& m){
      if (io().mode(ControlMode::Navigate)){
        tModelRot = mScene -> model.rot(); //save state on click 
      }
     }

    void SceneController :: onMouseMove(const Mouse& m){
      viewCalc();
    }

    void SceneController :: onMouseDrag(const Mouse& m){ 
      if (io().mode(ControlMode::Navigate) ){
        mouseNavigate();
      }
    }        
      
    void SceneController:: onMouseUp(const Mouse& m){
      if (io().mode(ControlMode::Navigate)){
        mouseNavigateStop();
      }
    }

   
    void SceneController::onKeyDown(const Keyboard& k){

      if (io().keyboard.code==Key::Tab){
          io().toggle(ControlMode::Edit);
          io().toggle(ControlMode::Navigate);
      }

      if ( io().mode(ControlMode::Navigate) ){
       if (io().trigger == true) keyboardNavigate();
       io().trigger = false;
      }
     
     }

    void SceneController::onKeyUp(const Keyboard& k){
      if ( io().mode(ControlMode::Navigate)) {
        keyboardNavigateStop();
        io().trigger = true;
      }
    }


    void SceneController::viewCalc(){
        
        vd().z  = Quat::spin( Vec3f(0,0,1), !model().quat() * camera().quat() ); 
         
        /// Bottom Left (0,0) to top right (1,1)
        /* io().mouse.pos  = Vec3f( io().mouse.x / vd().w, 1 - io().mouse.y / vd().h, 0 ) ; */
        /* io().mouse.move = Vec3f( io().mouse.dx / vd().w, - io().mouse.dy / vd().h, 0 ) ; */
        
        /* io().mouse.cat     = Quat::spin( io().mouse.move * -1, !mScene->cat() ); */
        /* io().mouse.bivCat = vd().z.cross( io().mouse.cat ); */

        //GLU FUNCS (to do: replace these with our own to eliminate reliance on glu)
        Vec3f v1 = mScene->unproject( io().pos(1.0));
         //gfx::GL::unproject( io().mouse.x, vd().h - io().mouse.y , 1.0,  mScene->xf );
        Vec3f v2 = mScene->unproject( io().pos(0.0)); 
        //gfx::GL::unproject( io().mouse.x, vd().h - io().mouse.y , 0.0,  mScene->xf );
        Vec3f v3 = mScene->unproject( io().pos(0.5)); 
        //gfx::GL::unproject( io().mouse.x, vd().h - io().mouse.y , 0.5,  mScene->xf );   

        //Get vec of Mouse Position pointing into Z Space 
        vd().ray   = ( v3 - v2 ).unit();
            
        io().viewdata.projectFar  = v1 ;
        io().viewdata.projectNear = v2 ;
        io().viewdata.projectMid  = v3 ;

       // io().mouse.biv     = io().mouse.pos.cross( vd().ray ); //not used?

    }


/*-----------------------------------------------------------------------------
 *  TRANFORM MODEL USING KEYBOARD ARROWS
 *-----------------------------------------------------------------------------*/
    void SceneController :: keyboardModelTransform(float acc, bool trigger){

          model().ab() = acc;

          if (trigger) {

              // Get Rotor Ratio between camera and model view
              Quat ryz = Quat::Rotor( model().x(), camera().x() );
              Quat rxz = Quat::Rotor( model().y(), camera().y() );

              // Rotate By said Rotor
              Vec3f tyz = Quat::spin( model().x(), ryz );
              Vec3f txz = Quat::spin( model().y(), rxz );

              switch( io().keyboard.code ){
                  case Key::Up:
                  {    
                      model().db() += tyz * mModelRotVel;  
                      break;
                  }
                  case Key::Down: 
                  {
                      model().db() -= tyz * mModelRotVel;  
                      break;
                  }
                  case Key::Left:
                  {
                      model().db() -= txz * mModelRotVel;  
                      break;
                  }
                  case Key::Right:
                  {
                      model().db() += txz * mModelRotVel;  
                      break;
                  }
              }
          }
      }

    void SceneController:: keyboardCamTranslate(float acc, bool trigger){
          camera().ax() = acc;
          if (trigger){
              switch(io().keyboard.code){
                  case Key::Up:
                      if (io().keyboard.ctrl()) camera().dx() += camera().up() * mVel;
                      else camera().dx() += camera().forward() * mVel;  
                      break;
                  case Key::Down:
                      if (io().keyboard.ctrl()) camera().dx() -= camera().up() * mVel;  
                      else camera().dx() -= camera().forward() * mVel; 
                      break;
                  case Key::Left:  camera().dx() -= camera().right() * mVel; break;
                  case Key::Right: camera().dx() += camera().right() * mVel; break;              
              }
          }
      }

    void SceneController:: keyboardCamSpin(float acc, bool trigger){
          camera().ab() = acc; 
          if (trigger){
              switch(io().keyboard.code){
                  case Key::Up:    camera().db() += camera().x() * mRotVel;  break;
                  case Key::Down:  camera().db() -= camera().x() * mRotVel; break;
                  case Key::Left:  camera().db() += camera().y() * mRotVel; break;
                  case Key::Right: camera().db() -= camera().y() * mRotVel; break;              
              }
          }
      } 
  
    void SceneController:: mouseModelTransform_(float acc, bool trigger){
          model().ab() = acc;
          float wx = (float)io().mouse.dx / io().viewdata.w;
          float wy = (float)io().mouse.dy / io().viewdata.h;

          //axis cross of z and differential
          Vec3f axis = Vec3f(0,0,1).cross( Vec3f(wx,-wy,0) );
          
          if (trigger) model().db() += axis * mModelRotVel;
     }

     void SceneController:: mouseModelTransform(){
         float wx = (float)io().mouse.dx / io().viewdata.w;
         float wy = (float)io().mouse.dy / io().viewdata.h;

         //axis cross of z and differential
         Vec3f axis = Vec3f(0,0,1).cross( Vec3f(wx,-wy,0) );
         Quat quat(  axis.len(), axis.unit() );

         mScene -> model.rot() = quat * tModelRot;
     }


    void SceneController :: mouseCamTranslate(float acc, bool trigger){
          camera().ax() = acc;
          if(trigger) {
              switch(io().mouse.gesture){
                  case Mouse::Up:
                      if (io().keyboard.ctrl()) camera().dx() += camera().up() * mVel;
                      else camera().dx() += camera().forward() * mVel * .5;  
                      break;
                  case Mouse::Down:
                      if (io().keyboard.ctrl()) camera().dx() -= camera().up() * mVel;  
                      else camera().dx() -= camera().forward() * mVel * .5; 
                      break;
                  case Mouse::Left:
                     camera().dx() -= camera().right() * mVel * .5; break;
                  case Mouse::Right:
                     camera().dx() += camera().right() * mVel * .5; break;            
              }

          }
      }

    void SceneController:: mouseCamSpin(float acc, bool trigger){
          camera().ab() = acc; 
          if (trigger){
              switch(io().mouse.gesture){
                  case Mouse::Up:    camera().db() += camera().x() * mRotVel * .5;  break;
                  case Mouse::Down:  camera().db() -= camera().x() * mRotVel * .5; break;
                  case Mouse::Left:  camera().db() -= camera().y() * mRotVel * .5; break;
                  case Mouse::Right: camera().db() += camera().y() * mRotVel * .5; break;              
              }
          }
      }  
  


    /*-----------------------------------------------------------------------------
     *  Navigate Start/Stop
     *-----------------------------------------------------------------------------*/
    void SceneController :: keyboardNavigate(){
          switch(io().keyboard.code){
              case 'c':
                 camera().reset(0,0,5); 
                 model().reset();
                 break;
          }
      
          if (io().keyboard.alt()) {  
              keyboardModelTransform(1.0, true);
          } else if (io().keyboard.shift()){   
              keyboardCamTranslate(1.0, true);
          } else {                         
              keyboardCamSpin(1.0, true);
          }
    }  
    
    void SceneController:: keyboardNavigateStop(){
        keyboardModelTransform(.7, false);
        keyboardCamTranslate(.7, false);
        keyboardCamSpin(.7, false);
    }

    void SceneController :: mouseNavigate(){
       if (io().keyboard.modifier==0) {
          mouseModelTransform();
       } else if (io().keyboard.alt()) {  
             mouseModelTransform_(1.0, true);
         } else if (io().keyboard.shift()) {  
             mouseCamTranslate(1.0, true);
         } else if (io().keyboard.ctrl()){
             mouseCamSpin(1.0,true);
         }  
    } 
    
    void SceneController :: mouseNavigateStop(){
       mouseModelTransform_(.7, false);
       mouseCamTranslate(.7, false);
       mouseCamSpin(.7, false);
    }
  
} //gfx::  


#endif
