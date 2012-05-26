UW-Madison - CS559 Graphics - Spring 2012 - Project 3 - Graphics Town
---------------------------------------------------------------------
Students:  Brian Ploeckelman, James Pulec


Libraries Used:
---------------
OpenGL (obviously) - http://www.opengl.org/
SFML 1.6 (Simple-Fast Multimedia Library) - http://www.sfml-dev.org/
GLM 0.9.3 (OpenGL mathematics) - http://glm.g-truc.net/
GLM (Wavefront .obj model loader) - http://www.eden.net.nz/7/20031008glm.html
GLee - http://www.opengl.org/sdk/libs/GLee/


All lib and header files are included in this repository, 
no extra setup should be required.


Keys
----
1 - 7 : toggle various render states
M/N   : toggle between cameras
Right Ctrl : disable mouse look


Features implemented
--------------------
- Terrain
  + load height values from image
  + procedurally generate height values (diamond-square)
  + multi-textured
  + lit
  + several toggleable features 
    - wireframe/fill
    - light
    - texturing 
    - multi-texturing
    - visualization of normals

- Fluid surface simulation
  + can interact with particle system
  + lit and blended (with lighting/blending toggleable)
  + environment mapped using skybox texture
    (tried to blend between day/night same as skybox, but didn't get good results)

- Particle systems
  + billboarded, textured, and blended
  + very extensible, easy to create new effects
    - ParticleEmitters contain Particles and ParticleAffectors
      - the emitter defines how to create new particles
      - the affector defines how to update particles
      - affectors are applied independently to allow for combinations of behaviors
    - ParticleSystems contain one or more ParticleEmitters
    - the ParticleManager handles the lifetime of entire ParticleSystems

- Skybox that blends between two different textures based on "time of day"

- Generic Mesh class
- Generic MeshOverlay class 
  + construct by passing in another mesh and it 
    'drapes' itself over the parent mesh (see road)

- .obj Model loading
  + via wavefront .obj GLM library

- Generic Scene class
- Generic SceneObject class
  + extended to represent:
    - Buildings
    - Fish
    - Fishing rod
    - Fountain
    - Blimp
    - Campfire
    - Model (.obj)
    - Windmill

- Generic Camera class
  + Scene supports multiple independent cameras
    - only two at the moment, use controlled, and a follower

- Generic Light class
  + able to be repositioned and have its attributes changed
    - had a few issues with lighting though, so they don't do much at the moment


Behaviors 
---------
- Fluid surface simulation reacts to fountain particle system
- Fish swim around, staying in the main 'lake' area
- 'Runner' particle system performs a 'drunken walk' around terrain 
  (pretend it is a very fast little mole digging around underground)
- Particle systems each have their own behavior
- Windmill spins
- Fishing pole is articulated at one joint
- circling blimp

Sources
-------
http://reije081.home.xs4all.nl/skyboxes/ 
http://blender-archi.tuxfamily.org/Textures
http://www.turbosquid.com