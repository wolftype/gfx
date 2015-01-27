

#ifndef GFX_GRAPH_H_INCLUDED
#define GFX_GRAPH_H_INCLUDED


namespace gfx{
 
 /*! Templated half edge structure (of vecs etc)
 */ 

 template<class T>
 struct HEGraph {

   struct HalfEdge;
   struct Face;
   struct Edge;
   
   struct Node {
     Node() : ptr(NULL), edge(NULL) {}
     T * ptr;              // Pointer to type T
     HalfEdge * edge;      // An emanating half-edge
   };
   
   //HALF EDGE
   struct HalfEdge{ 
     
     HalfEdge() : node(NULL), face(NULL), opp(NULL), next(NULL) {}
          
     Node    * node;      // Incident vertex
     Face    * face;      // Face membership

     HalfEdge  * opp;       // Opposite half-edge
     HalfEdge  * next;      // Next half-edge counterclockwise
   };

   
   struct Face {
     Face() : edge(NULL) {}
     HalfEdge * edge;       //Any edge
   };
   
    struct Edge {
       Edge() : a(NULL), b(NULL) {}
       HalfEdge *a, *b;
     };

    void facet( T& pa,  T& pb,  T& pc){

       Node * na, *nb, *nc;
       HalfEdge * a, *b, *c;
       Edge * ea, *eb, *ec;
       Face * f;
        
        na = new Node; nb = new Node; nc = new Node;
         
        a = new HalfEdge; a -> node = na; ea = new Edge; ea -> a = a;
        b = new HalfEdge; b -> node = nb; eb = new Edge; eb -> a = b;
        c = new HalfEdge; c -> node = nc; ec = new Edge; ec -> a = c;

        na -> ptr  = &pa; 
        nb -> ptr  = &pb; 
        nc -> ptr  = &pc; 

        f = new Face(); f -> edge = a;

        na -> edge = b; nb -> edge = c; nc -> edge = a;

         a -> next = b; a -> face = f;
         b -> next = c; b -> face = f;
         c -> next = a; c -> face = f;

        mHalfEdge.push_back(a);
        mHalfEdge.push_back(b);
        mHalfEdge.push_back(c);

        mEdge.push_back(ea);
        mEdge.push_back(eb);
        mEdge.push_back(ec);

        mFace.push_back(f);

       // a -> node -> ptr -> vprint();

    }


    //add a new Node and Face To Edge
    HEGraph& add( T& v, Edge& edge ){

      Node * n = new Node;

      HalfEdge * a = new HalfEdge; 
      HalfEdge * b = new HalfEdge; 
      HalfEdge * c = new HalfEdge;

      mHalfEdge.push_back(a);
      mHalfEdge.push_back(b);
      mHalfEdge.push_back(c);

      Edge * ea = new Edge;
      Edge * eb = new Edge;

      mEdge.push_back(ea);
      mEdge.push_back(eb);

      ea -> a = a; eb -> a = b;

      Face * f = new Face;
      mFace.push_back(f);
      f -> edge = a;

      n -> ptr = &v; 
      n -> edge = b;
      a -> node = n; 

      edge.b = c; edge.a -> opp = c; c -> opp = edge.a; 
      
      a -> next = b;  b -> next = c; c -> next = a;
      a -> face = f; b -> face = f; c -> face = f;

      
      b -> node = edge.a -> node; 
      c -> node = edge.a -> next -> next -> node; 
      c -> node -> ptr -> vprint();


      return *this;
    }

    vector<HalfEdge*>    mHalfEdge;
    vector<Face*>        mFace;
    vector<Edge*>        mEdge;

    HalfEdge& halfedge( int idx ) { return *mHalfEdge[idx]; }
    Face& face( int idx ) { return *mFace[idx]; }
    Edge& edge( int idx ) { return *mEdge[idx]; }


   // vector<Corner>      mCorner;

 };   

} //gfx::


     /* struct Corner { */
     /*   Corner next, prev;  // next / prev corners in face */
     /*   HalfEdge hedge; */
     /*   Edge edge;      // incoming edge */
     /*   Node node;      // incident vertex */
     /* }; */


#endif
