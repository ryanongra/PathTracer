void buildScene(void)
{
 // Sets up all objets in the scene. This involves creating each object,
 // defining the transformations needed to shape and position it as
 // desired, specifying the reflectance properties (albedos and colours)
 // and setting up textures where needed.
 //
 // NOTE: Light sources are now EXCLUSIVELY area light sources. They are
 //       defined by regular objects whose 'isLightSource' flag is set
 //       to 1. Therefore, you can create light sources with any shape
 //       and colour using the same object primitives and transforms
 //       you're using to set up the scene.
 //
 // To create hierarchical objects:
 //    You must keep track of transformations carried out by parent objects
 //    as you move through the hierarchy. Declare and manipulate your own
 //    transformation matrices (use the provided functions in utils.c to
 //    compound transformations on these matrices). When declaring a new
 //    object within the hierarchy
 //    - Initialize the object
 //    - Apply any object-level transforms to shape/rotate/resize/move
 //      the object using regular object transformation functions
 //    - Apply the transformations passed on from the parent object
 //      by pre-multiplying the matrix containing the parent's transforms
 //      with the object's own transformation matrix.
 //    - Compute and store the object's inverse transform as usual.
 //
 // NOTE: After setting up the transformations for each object, don't
 //       forget to set up the inverse transform matrix!

 struct object3D *o;
 struct point3D p;

 // Cornell box
 o=newSphere(0.3,0.7,0.0,.5,.5,.5,.05,1.4);	// Left
 Scale(o,500,500,500);
 Translate(o,-510,0,5);
 invert(&o->T[0][0],&o->Tinv[0][0]);
 insertObject(o,&object_list);

 o=newSphere(0.3,0.7,0.0,.5,.5,.5,.05,1.4);		// Right
 Scale(o,500,500,500);
 Translate(o,510,0,5);
 invert(&o->T[0][0],&o->Tinv[0][0]);
 insertObject(o,&object_list);

 o=newSphere(0.3,0.7,0.0,.5,.5,.5,.05,1.4);		// Back
 Scale(o,500,500,500);
 Translate(o,0,0,515);
 invert(&o->T[0][0],&o->Tinv[0][0]);
 insertObject(o,&object_list);

 o=newSphere(0.3,0.7,0.0,.5,.5,.5,.02,1.4);	// Bottom
 Scale(o,500,500,500);
 Translate(o,0,-510,5);
 invert(&o->T[0][0],&o->Tinv[0][0]);
 insertObject(o,&object_list);

 o=newSphere(0.3,0.7,0.0,.5,.5,.5,.05,1.4);		// Top
 Scale(o,500,500,500);
 Translate(o,0,510,5);
 invert(&o->T[0][0],&o->Tinv[0][0]);
 insertObject(o,&object_list);

 // Snowman
 o=newSphere(0.8,0.0,0.2,.99,.99,.99,.01,1.54);		
 Scale(o,3.5,3.5,3.5);
 Translate(o,0,-7.0,4.5);
 invert(&o->T[0][0],&o->Tinv[0][0]);
 insertObject(o,&object_list);

 o=newSphere(0.8,0.0,0.2,.99,.99,.99,.01,1.54);		
 Scale(o,2.5,2.5,2.5);
 Translate(o,0,-1.8,4.5);
 invert(&o->T[0][0],&o->Tinv[0][0]);
 insertObject(o,&object_list);

 o=newSphere(0.8,0.0,0.2,.99,.99,.99,.01,1.54);		
 Scale(o,1.5,1.5,1.5);
 Translate(o,0,1,4.5);
 invert(&o->T[0][0],&o->Tinv[0][0]);
 insertObject(o,&object_list);

 // Snowman heads on floor

 o=newSphere(0.0,1.0,0.0,.99,.99,.99,.01,1.54);		
 Scale(o,1.5,1.5,1.5);
 Translate(o,-6,-10.0,10);
 invert(&o->T[0][0],&o->Tinv[0][0]);
 insertObject(o,&object_list);

 o=newSphere(0.0,0.0,1.0,.99,.99,.99,.01,1.54);		
 Scale(o,1.5,1.5,1.5);
 Translate(o,-3,-10.0,2.5);
 invert(&o->T[0][0],&o->Tinv[0][0]);
 insertObject(o,&object_list);

 o=newSphere(0.0,1.0,0.0,.99,.99,.99,.01,1.54);		
 Scale(o,1.5,1.5,1.5);
 Translate(o,5,-10.0,1.5);
 invert(&o->T[0][0],&o->Tinv[0][0]);
 insertObject(o,&object_list);


 // Lights
 o=newPlane(1.00,0.00,0.0,0.5,0.0,0.0,0.0,1.54);
 Scale(o,.5,0.5,1);
 RotateX(o,PI/2);
 Translate(o,7,9.995,12);
 invert(&o->T[0][0],&o->Tinv[0][0]);
 o->isLightSource=1;
 insertObject(o,&object_list);

 o=newPlane(1.00,0.00,0.0,0.5,0.0,0.0,0.0,1.54);
 Scale(o,.5,0.5,1);
 RotateX(o,PI/2);
 Translate(o,-7,9.995,12);
 invert(&o->T[0][0],&o->Tinv[0][0]);
 o->isLightSource=1;
 insertObject(o,&object_list);

 o=newPlane(1.00,0.00,0.0,0.0,0.5,0.0,0.0,1.54);
 Scale(o,.5,0.5,1);
 RotateX(o,PI/2);
 Translate(o,0,9.995,12);
 invert(&o->T[0][0],&o->Tinv[0][0]);
 o->isLightSource=1;
 insertObject(o,&object_list);

}