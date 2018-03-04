/*
 * =====================================================================================
 *
 *       Filename:  gfx_file.h
 *
 *    Description:  file write
 *
 *        Version:  1.0
 *        Created:  03/25/2015 11:11:16
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Pablo Colapinto (), gmail->wolftype
 *   Organization:  wolftype
 *
 * =====================================================================================
 */

#ifndef GFX_FILE_INCLUDED
#define GFX_FILE_INCLUDED

#include <fstream>

namespace gfx {


struct File
{

  unsigned char *pixels;

  void resize (int w, int h)
  {
    if (pixels)
      delete[] pixels;
    pixels = new unsigned char[(int) w * h * 4];
  }

  /*-----------------------------------------------------------------------------
   *  Write to .ppm format (easiest...)
   *-----------------------------------------------------------------------------*/
  void write (int w, int h, string dir, string prefix = "test")
  {

    /*-----------------------------------------------------------------------------
     *  File Name
     *-----------------------------------------------------------------------------*/
    static int fnum = -1;
    fnum++;
    stringstream name;
    name << prefix << "_" << fnum;
    stringstream os;
    os << dir << name.str () << ".ppm";
    string filename = os.str ();

    /*-----------------------------------------------------------------------------
     *  Read Pixel Buffer into pixels array
     *-----------------------------------------------------------------------------*/
    glReadPixels (0, 0, w, h, GL_RGBA, GL::UBYTE, pixels);

    /*-----------------------------------------------------------------------------
     *  Output Stream
     *-----------------------------------------------------------------------------*/
    ofstream frame (filename.c_str (), ios::binary);

    /*-----------------------------------------------------------------------------
     *  Write .ppm File Header
     *-----------------------------------------------------------------------------*/
    frame << "P6\n" << w << " " << h << "\n255\n";

    /*-----------------------------------------------------------------------------
     *  Write File Data
     *-----------------------------------------------------------------------------*/
    for (int j = h - 1; j >= 0; j--)
      {
        for (int i = 0; i < w; i++)
          {
            frame.write ((char *) (pixels + w * 4 * j + 4 * i), 3);
          }
      }

    /*-----------------------------------------------------------------------------
     *  Close File
     *-----------------------------------------------------------------------------*/
    frame.close ();

    /*-----------------------------------------------------------------------------
     *  Convert to compressed image format and delete
     *-----------------------------------------------------------------------------*/
    string command = "sh ~/scripts/ppm2jpg.sh " + filename;
    system (command.c_str ());
  }

  void writeAlpha (int w, int h, string dir, string prefix = "alpha")
  {
    /*-----------------------------------------------------------------------------
     *  File Name
     *-----------------------------------------------------------------------------*/
    static int fnum = -1;
    fnum++;
    stringstream name;
    name << prefix << "_" << fnum;
    stringstream os;
    os << dir << name.str () << ".ppm";
    string filename = os.str ();

    /*-----------------------------------------------------------------------------
     *  Read Pixel Buffer into pixels array
     *-----------------------------------------------------------------------------*/
    glReadPixels (0, 0, w, h, GL_RGBA, GL::UBYTE, pixels);

    /*-----------------------------------------------------------------------------
     *  Output Stream
     *-----------------------------------------------------------------------------*/
    ofstream frame (filename.c_str (), ios::binary);

    /*-----------------------------------------------------------------------------
     *  Write .ppm File Header
     *-----------------------------------------------------------------------------*/
    frame << "P6\n" << w << " " << h << "\n255\n";

    /*-----------------------------------------------------------------------------
     *  Write File Data
     *-----------------------------------------------------------------------------*/
    for (int j = h - 1; j >= 0; j--)
      {
        for (int i = 0; i < w; i++)
          {
            char *alpha = (char *) (pixels + w * 4 * j + 4 * i + 3);
            frame.write (alpha, 1);
            frame.write (alpha, 1);
            frame.write (alpha, 1);
          }
      }

    /*-----------------------------------------------------------------------------
     *  Close File
     *-----------------------------------------------------------------------------*/
    frame.close ();

    /*-----------------------------------------------------------------------------
     *  Convert to compressed image format and delete
     *-----------------------------------------------------------------------------*/
    string command = "sh ~/scripts/ppm2jpg.sh " + filename;
    system (command.c_str ());
  }

  ///Open A filepath into myfile
  static void Open (std::string filepath, std::fstream &myfile)
  {
    // search by going up directory up to 5 times
    int attempts = 0;
    std::string nfilepath = filepath;
    while (attempts < 5)
      {
        // try opening and return if successful
        myfile.open (nfilepath.c_str (), std::ios::in);
        if (myfile.is_open ())
          return;
        // go up 1 dir if file not found
        nfilepath = "../" + nfilepath;
        attempts += 1;
      }
    // fail 5 times, complain
    if (!myfile.is_open ())
      {
        printf ("file not found\n");
        //        throw std::invalid_argument("Error: File " + filepath + " Not Found.");
      }
    return;
  }

  /// Read rilepath into a string
  static std::string Read (std::string filepath)
  {
    std::fstream myfile;
    File::Open (filepath, myfile);

    if (myfile.is_open ())
      {
        std::string fileString ((std::istreambuf_iterator<char> (myfile)),
                                (std::istreambuf_iterator<char> ()));
        return fileString;
      }

    return "";
  }
};


}  // gfx::

#endif
