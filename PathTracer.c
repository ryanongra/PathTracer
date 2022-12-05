/*
  CSC D18 - Path Tracer code.

  Derived from the ray tracer starter code. Most function 
  names are identical, though in practice the implementation
  should be much simpler!

  You only need to modify or add code in sections
  clearly marked "TO DO" - remember to check what
  functionality is actually needed for the corresponding
  assignment!

  Last updated: Aug. 2017   - F.J.E.
*/

/*****************************************************************************
* COMPLETE THIS TEXT BOX:
*
* 1) Student Name: Ryan Ong		
* 2) Student Name:		
*
* 1) Student number: 1009760723
* 2) Student number:
* 
* 1) UtorID: ongryan2
* 2) UtorID
* 
* We hereby certify that the work kained here is our own
*
* ____Ryan Ong________             _____________________
* (sign with your name)            (sign with your name)
********************************************************************************/
// #include "iostream"
// #include "string.h"
// #include "signal.h"

#include "float.h"
#include "utils_path.h"			// <-- This includes PathTracer.h
//#define __USE_IS			// Use importance sampling for diffuse materials
//#define __USE_ES			// Use explicit light sampling
//#define __DEBUG			// <-- Use this to turn on/off debugging output

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
#define ABS(X) (X < 0 ? -X : X)

// A couple of global structures and data: An object list, a light list, and the
// maximum recursion depth
struct object3D *object_list;
struct textureNode *texture_list;
unsigned long int NUM_RAYS;
int MAX_DEPTH;

// #include "buildScene.c"			// Import scene definition
#include "coolScene.c"

void findFirstHit(struct ray3D *ray, double *lambda, struct object3D *Os, struct object3D **obj, struct point3D *p, struct point3D *n, double *a, double *b)
{
 // Find the closest intersection between the ray and any objects in the scene.
 // Inputs:
 //   *ray    -  A pointer to the ray being traced
 //   *Os     -  'Object source' is a pointer toward the object from which the ray originates. It is used for reflected or refracted rays
 //              so that you can check for and ignore self-intersections as needed. It is NULL for rays originating at the center of
 //              projection
 // Outputs:
 //   *lambda -  A pointer toward a double variable 'lambda' used to return the lambda at the intersection point
 //   **obj   -  A pointer toward an (object3D *) variable so you can return a pointer to the object that has the closest intersection with
 //              this ray (this is required so you can do the shading)
 //   *p      -  A pointer to a 3D point structure so you can store the coordinates of the intersection point
 //   *n      -  A pointer to a 3D point structure so you can return the normal at the intersection point
 //   *a, *b  -  Pointers toward double variables so you can return the texture coordinates a,b at the intersection point

 *lambda = -1;
 struct point3D intersectPt;
 struct point3D norm;
 double currLambda;

 struct object3D *rem_obj = object_list;

 while(rem_obj != NULL) {

    void (*intersect)(struct object3D *obj, struct ray3D *ray, double *lambda, struct point3D *p, struct point3D *n, double *a, double *b) 
    = rem_obj->intersect;
  
    intersect(rem_obj, ray, &currLambda, &intersectPt, &norm, a, b);  
       
    if((*lambda < 0 || currLambda < *lambda) && currLambda > 0 && rem_obj != Os) {

       *p = intersectPt;
       *n = norm;
       *lambda = currLambda;
       *obj = rem_obj;

     }

   rem_obj = rem_obj->next;
 }

}

void PathTrace(struct ray3D *ray, int depth, struct colourRGB *col, struct object3D *Os, int CEL)
{
 // Trace one light path through the scene.
 //
 // Parameters:
 //   *ray   -  A pointer to the ray being traced
 //   depth  -  Current recursion depth for recursive raytracing
 //   *col   - Pointer to an RGB colour structure so you can return the object colour
 //            at the intersection point of this ray with the closest scene object.
 //   *Os    - 'Object source' is a pointer to the object from which the ray 
 //            originates so you can discard self-intersections due to numerical
 //            errors. NULL for rays originating from the center of projection. 
 
 double lambda;			// Lambda at intersection
 double a,b;			// Texture coordinates
 struct object3D *obj;		// Pointer to object at intersection
 struct point3D p;		// Intersection point
 struct point3D n;		// Normal at intersection
 double R,G,B;			// Handy in case you need to keep track of some RGB colour value
 double dice;			// Handy to keep a random value
 struct ray3D *next_ray;	// For the new ray to be used in recursive calls
 
 next_ray = (struct ray3D *)calloc(1,sizeof(struct ray3D));
 dice = .5*drand48();

  if (depth>MAX_DEPTH)	// Max recursion depth reached. Return black (no light coming into pixel from this path).
  {
    col->R=ray->Ir;	// These are accumulators, initialized at 0. Whenever we find a source of light these
    col->G=ray->Ig;	// get incremented accordingly. At the end of the recursion, we return whatever light
    col->B=ray->Ib;	// we accumulated into these three values.
    return;
  }

  findFirstHit(ray,&lambda,Os,&obj,&p,&n,&a,&b);

  if(lambda > 0) {

    /////////////////////////////////////////////////////////////
    // LIGHT SOURCE
    /////////////////////////////////////////////////////////////
    if(obj->isLightSource) {

      if(CEL) {
        ray->Ir += ray->R * obj->col.R;
        ray->Ig += ray->G * obj->col.G;
        ray->Ib += ray->B * obj->col.B;
      }

      col->R = MIN(ray->Ir, 1);
      col->G = MIN(ray->Ig, 1);
      col->B = MIN(ray->Ib, 1);
      return;
    }
    else {
      dice = drand48();

      ray->R = ray->R * obj->col.R;
      ray->G = ray->G * obj->col.G;
      ray->B = ray->B * obj->col.B;

      if (dice < obj->diffPct) {
        /////////////////////////////////////////////////////////////
        // DIFFUSE
        /////////////////////////////////////////////////////////////

        cosWeightedSample(&n,&next_ray->d);
        // randomSample(&n,&next_ray->d);

        double nDotD = dot(&n,&next_ray->d);
        ray->R = ray->R * nDotD;
        ray->G = ray->G * nDotD;
        ray->B = ray->B * nDotD;

        CEL = checkELS(ray, &p,&n,obj);

      } else if (dice > obj->tranPct/(obj->reflPct + obj->tranPct)) {
        /////////////////////////////////////////////////////////////
        // REFLECTION
        /////////////////////////////////////////////////////////////
        struct point3D refDir;
        double rayDirDotN = dot(&ray->d, &n);

        next_ray->d.px = -2 * rayDirDotN * n.px + ray->d.px;
        next_ray->d.py = -2 * rayDirDotN * n.py + ray->d.py;
        next_ray->d.pz = -2 * rayDirDotN * n.pz + ray->d.pz;

        
        double randTheta = drand48() * obj->refl_sig;
        double randPhi = drand48() * obj->refl_sig;

        double rotXMat[4][4] = {{1.0, 0.0, 0.0, 0.0},
                                {0.0, cos(randTheta), -sin(randTheta), 0.0},
                                {0.0, sin(randTheta), cos(randTheta), 0.0},
                                {0.0, 0.0, 0.0, 1.0}};
        double rotYMat[4][4] = {{cos(randPhi), 0.0, sin(randPhi), 0.0},
                                {0.0, 1.0, 0.0, 0.0},
                                {-sin(randPhi), 0.0, cos(randPhi), 0.0},
                                {0.0, 0.0, 0.0, 1.0}};

        matVecMult(rotXMat, &next_ray->d);
        matVecMult(rotYMat, &next_ray->d);
        normalize(&next_ray->d);

      } else {

        /////////////////////////////////////////////////////////////
        // REFRACTION (transmitted)
        /////////////////////////////////////////////////////////////
        bool entering = dot(&ray->d, &n) > 0 ? true : false;

        double r;
        double c;
        struct point3D transDir;
        double tranX, tranY, tranZ;
        struct point3D tempNormal;

        if (entering) {
          tempNormal = *(newPoint(-n.px, -n.py, -n.pz));
          r = obj->r_index;
        } else {
          tempNormal = *(newPoint(n.px, n.py, n.pz));
          r = 1 / obj->r_index;
        }

        double critAngle = asin(1 / r);
        double incAngle = angleBetween(&ray->d, &n);
        c = -(dot(&tempNormal, &ray->d));
        if (incAngle > critAngle) {
          double rayDirDotN = dot(&ray->d, &n);
          next_ray->d.px = -2 * rayDirDotN * tempNormal.px + ray->d.px;
          next_ray->d.py = -2 * rayDirDotN * tempNormal.py + ray->d.py;
          next_ray->d.pz = -2 * rayDirDotN * tempNormal.pz + ray->d.pz; 
        } else {
          next_ray->d.px = (r*ray->d.px) + ((r*c) - sqrtf(1 - ((r*r) * (1 - (c*c))))) * tempNormal.px;
          next_ray->d.py = (r*ray->d.py) + ((r*c) - sqrtf(1 - ((r*r) * (1 - (c*c))))) * tempNormal.py;
          next_ray->d.pz = (r*ray->d.pz) + ((r*c) - sqrtf(1 - ((r*r) * (1 - (c*c))))) * tempNormal.pz;
        }

        obj = NULL;
        
      }

      normalize(&next_ray->d);
      initRay(next_ray, &p, &next_ray->d);

      next_ray->R = ray->R;
      next_ray->G = ray->G;
      next_ray->B = ray->B;
      next_ray->Ir = ray->Ir;
      next_ray->Ig = ray->Ig;
      next_ray->Ib = ray->Ib;

      NUM_RAYS++;

      PathTrace(next_ray,depth+1,col,obj,CEL);
      free(next_ray);
    }
  } else {
    col->R=0;
    col->G=0;
    col->B=0;
    return;
  }
}

int checkELS(struct ray3D *ray, struct point3D *p, struct point3D *n,struct object3D *obj) {

  struct object3D *currObj = object_list;

  struct point3D testDir;
  double randX, randY, randZ;

  double dummyLambda;
  double dummyA;
  double dummyB;
  struct point3D dummyPt;
  struct point3D dummyN;
  struct object3D *testObj;
  
  struct ray3D *next_ray;

  double k;
  int CEL=1;
  
  while(currObj != NULL) {
    if(currObj->isLightSource) {

      void (*randomPoint)(struct object3D *object, double *x, double *y, double *z) 
        = currObj->randomPoint;
  
      randomPoint(currObj, &randX, &randY, &randZ);    

      testDir.px = randX - p->px;
      testDir.py = randY - p->py;
      testDir.pz = randZ - p->pz;
      normalize(&testDir);

      next_ray = (struct ray3D *)calloc(1,sizeof(struct ray3D));
      initRay(next_ray, p, &testDir);

      findFirstHit(next_ray,&dummyLambda,NULL,&testObj,&dummyPt,&dummyN,&dummyA,&dummyB);

      if(testObj != NULL) {
        if(testObj->isLightSource && dummyLambda > 0) {

          k = 2*PI*testObj->LSweight * -dot(&dummyN,&testDir)*dot(n,&testDir)/(dummyLambda*dummyLambda);
          k = MIN(k, 1);

          ray->Ir += ray->R * obj->col.R * k;
          ray->Ig += ray->G * obj->col.G * k;
          ray->Ib += ray->B * obj->col.B * k;

          CEL = 0;
        }
      }
      free(next_ray);
    }
    currObj = currObj->next;
  }
  return CEL;

}

// void handler(int nSignum, siginfo_t* si, void* vcontext) {  
//   ucontext_t* context = (ucontext_t*)vcontext;
//   context->uc_mcontext.gregs[REG_RIP]++;
// }

int main(int argc, char *argv[])
{
  // struct sigaction action;
  // memset(&action, 0, sizeof(struct sigaction));
  // action.sa_flags = SA_SIGINFO;
  // action.sa_sigaction = handler;
  // sigaction(SIGSEGV, &action, NULL);

 // Main function for the path tracer. Parses input parameters,
 // sets up the initial blank image, and calls the functions
 // that set up the scene and do the raytracing.
 struct image *im;		// Will hold the final image
 struct view *cam;		// Camera and view for this scene
 int sx;			// Size of the  image
 int num_samples;		// Number of samples to use per pixel
 char output_name[1024];	// Name of the output file for the .ppm image file
 struct point3D e;		// Camera view parameters 'e', 'g', and 'up'
 struct point3D g;
 struct point3D up;
 double du, dv;			// Increase along u and v directions for pixel coordinates
 struct point3D pc,d;		// Point structures to keep the coordinates of a pixel and
				// the direction or a ray
 struct ray3D *ray;		// Structure to keep the ray from e to a pixel
 struct colourRGB col;		// Return colour for pixels
 int i,j,k;			// Counters for pixel coordinates and samples
 double *rgbIm;			// Image is now double precision floating point since we
				// will be accumulating brightness differences with a 
				// wide dynamic range
 struct object3D *obj;		// Will need this to process lightsource weights
 double *wght;			// Holds weights for each pixel - to provide log response
 double pct,wt;
 
 time_t t1,t2;
 FILE *f;
				
 if (argc<5)
 {
  fprintf(stderr,"PathTracer: Can not parse input parameters\n");
  fprintf(stderr,"USAGE: PathTracer size rec_depth num_samples output_name\n");
  fprintf(stderr,"   size = Image size (both along x and y)\n");
  fprintf(stderr,"   rec_depth = Recursion depth\n");
  fprintf(stderr,"   num_samples = Number of samples per pixel\n");
  fprintf(stderr,"   output_name = Name of the output file, e.g. MyRender.ppm\n");
  exit(0);
 }
 sx=atoi(argv[1]);
 MAX_DEPTH=atoi(argv[2]);
 num_samples=atoi(argv[3]);
 strcpy(&output_name[0],argv[4]);

 fprintf(stderr,"Rendering image at %d x %d\n",sx,sx);
 fprintf(stderr,"Recursion depth = %d\n",MAX_DEPTH);
 fprintf(stderr,"NUmber of samples = %d\n",num_samples);
 fprintf(stderr,"Output file name: %s\n",output_name);

 object_list=NULL;
 texture_list=NULL;

 // Allocate memory for the new image
 im=newImage(sx, sx);
 wght=(double *)calloc(sx*sx,sizeof(double));
 if (!im||!wght)
 {
  fprintf(stderr,"Unable to allocate memory for image\n");
  exit(0);
 }
 else rgbIm=(double *)im->rgbdata;
 for (i=0;i<sx*sx;i++) *(wght+i)=1.0;
 
 buildScene();		// Create a scene. 
 
 // Mind the homogeneous coordinate w of all vectors below. DO NOT
 // forget to set it to 1, or you'll get junk out of the
 // geometric transformations later on.

 // Camera center
 e.px=0;
 e.py=0;
 e.pz=-15;
 e.pw=1;

 // To define the gaze vector, we choose a point 'pc' in the scene that
 // the camera is looking at, and do the vector subtraction pc-e.
 // Here we set up the camera to be looking at the origin.
 g.px=0-e.px;
 g.py=0-e.py;
 g.pz=0-e.pz;
 g.pw=1;
 // In this case, the camera is looking along the world Z axis, so
 // vector w should end up being [0, 0, -1]

 // Define the 'up' vector to be the Y axis
 up.px=0;
 up.py=1;
 up.pz=0;
 up.pw=1;

 // Set up view with given the above vectors, a 4x4 window,
 // and a focal length of -1 (why? where is the image plane?)
 // Note that the top-left corner of the window is at (-2, 2)
 // in camera coordinates.
 cam=setupView(&e, &g, &up, -3, -2, 2, 4);

 if (cam==NULL)
 {
  fprintf(stderr,"Unable to set up the view and camera parameters. Our of memory!\n");
  cleanup(object_list, texture_list);
  deleteImage(im);
  exit(0);
 }

 du=cam->wsize/(sx-1);		// du and dv. In the notes in terms of wl and wr, wt and wb,
 dv=-cam->wsize/(sx-1);		// here we use wl, wt, and wsize. du=dv since the image is
				// and dv is negative since y increases downward in pixel
				// coordinates and upward in camera coordinates.

 fprintf(stderr,"View parameters:\n");
 fprintf(stderr,"Left=%f, Top=%f, Width=%f, f=%f\n",cam->wl,cam->wt,cam->wsize,cam->f);
 fprintf(stderr,"Camera to world conversion matrix (make sure it makes sense!):\n");
 printmatrix(cam->C2W);
 fprintf(stderr,"World to camera conversion matrix:\n");
 printmatrix(cam->W2C);
 fprintf(stderr,"\n");

 // Update light source weights - will give you weights for each light source that add up to 1
 obj=object_list;
 pct=0;
 while (obj!=NULL)
 {
  if (obj->isLightSource)
   pct+=obj->LSweight;
  obj=obj->next;
 }
 obj=object_list;
 while (obj!=NULL)
 {
  if (obj->isLightSource)
  {
   obj->LSweight/=pct;
  }
  obj=obj->next;
 }
 fprintf(stderr,"\n");

 NUM_RAYS=0;

 t1=time(NULL);

 fprintf(stderr,"Rendering pass... ");
 for (k=0; k<num_samples; k++)
 {
  fprintf(stderr,"%d/%d, ",k,num_samples);
#pragma omp parallel for schedule(dynamic,1) private(i,j,pc,wt,ray,col,d)
  for (j=0;j<sx;j++)		// For each of the pixels in the image
  {
   for (i=0;i<sx;i++)
   {
    // Random sample within the pixel's area
    pc.px=(cam->wl+((i+(drand48()-.5))*du));
    pc.py=(cam->wt+((j+(drand48()-.5))*dv));
    pc.pz=cam->f;
    pc.pw=1;

    // Convert image plane sample coordinates to world coordinates
    matVecMult(cam->C2W,&pc);

    // Now compute the ray direction
    memcpy(&d,&pc,sizeof(struct point3D));
    subVectors(&cam->e,&d);		// Direction is d=pc-e
    normalize(&d);

    // Create a ray and do the raytracing for this pixel.
    ray = (struct ray3D *)calloc(1,sizeof(struct ray3D));
    initRay(ray, &pc, &d);

    if (&ray!=NULL)
    {
     wt=*(wght+i+(j*sx));
     PathTrace(ray,1, &col,NULL,1);
     (*(rgbIm+((i+(j*sx))*3)+0))+=col.R*pow(2,-log(wt));
     (*(rgbIm+((i+(j*sx))*3)+1))+=col.G*pow(2,-log(wt));
     (*(rgbIm+((i+(j*sx))*3)+2))+=col.B*pow(2,-log(wt));
     wt+=col.R;
     wt+=col.G;
     wt+=col.B;
     *(wght+i+(j*sx))=wt;
     //free(&ray);   
    }
   } // end for i
  } // end for j  
  if (k%25==0)  dataOutput(rgbIm,sx,&output_name[0]);  		// Update output image every 25 passes
 } // End for k 
 t2=time(NULL);

 fprintf(stderr,"\nDone!\n");

 dataOutput(rgbIm,sx,&output_name[0]);
 
 fprintf(stderr,"Total number of rays created: %ld\n",NUM_RAYS);
 fprintf(stderr,"Rays per second: %f\n",(double)NUM_RAYS/(double)difftime(t2,t1));

 // Exit section. Clean up and return.
 cleanup(object_list,texture_list);			// Object and texture lists
 deleteImage(im);					// Rendered image
 free(cam);						// camera view
 free(wght);
 exit(0);
}

