# Floaty Boaty Go Go

A small game written in C++ using OpenGL for rendering.

![Cover image](img/cover.png)

## About

Floaty Boaty Go Go is a result of a 3-week project that was an assignment for programmers participating in the DADIU programme. Durig this game development course, all game programmers were asked to author a game without the use of a game engine. In my case, I chose to build what became Floaty Boaty Go Go. The project was scheduled to take three weeks, but this has exceeded that time frame.

In the game, the player controls an airship and must fight other airships controlled by a simple AI. The game is mostly used to explore how to write a code base that facilitates a somewhat good asset pipeline and how to make a flexible system that can be used to build possibly not just one single game or game type.

## Architecture

The core architecture of the games draws inspiration from the [ECS](ECS) pattern. With such a pattern, each entity is by itself only an id without any behavior
or functionality. Different logic and presence of the entities manifest
via components. This way, entities can change what they are and what they
are capable of at runtime.

The [ECS](ECS) pattern is implemented to give each component an easy interface
to access other components. An example of this is the `AIControls` interacting
with the `ShipMovement`. The `ShipMovement` is the component that moves
each ship determining speed, height, and similar properties. The `AIControls` components
implement an AI hardly worthy of its title, which moves forwards
until reaching the center of the map where it stops. The update loop for the
`AIControls` is as follows:

```
void AIControls::Update()
{
    Transform *transform = GetComponent<Transform>();

    glm::vec3 position = transform->GetLocalPosition();

    position.y = 0.0f;

    // stop the ship when reaching the center
    if (glm::length(position) < 1.0f)
    {
        ShipMovement *shipMovement = GetComponent<ShipMovement>();

        shipMovement->SetThrottle(0.0f);
    }
}
```

This demonstrates how the components can get pointers to other components
on the same entity, and interact with them.

## Assets

All game specific assets (including) code is located in the _assets_ folder.
Before runtime, a python script (`AssetDatabaseBuilder.py`) walks through
the _assets_ folder and compiles a _JSON_ file that maps all asset types
to paths for some assets, creates a _meta_ file that contains information
on how assets are at runtime.

Some assets must are written by hand in _JSON_ format. As an example,
`material` assets must be written in _JSON_ format manually, an example
is as follows:

```
{
    "type": "material",
    "name": "island",
    "shader": "island",
    "uniforms":
    {
        "_Sand": "sand",
        "_Grass": "grass"
    }
}
```

Here, one identifies the type and name of the material, which must be present for the asset database.
Script to pick it up. Then, the materail specifies a shader along with a list of uniforms.

The asset database can in a particular build configuration monitor changes in the
_asset_ folder, and keep track of which assets depends on which files.
If asset tracking is enabled, assets may change at runtime. Swapping assets at runtime has so far proven to be useful for shaders and textures.

Below is an example of a material that changes from a missing texture to back to the original existing texture.
<p align="center">
<img src="img/missingtexture.gif"/>
</p>

Another example is changing the settings of the texture loading. One can choose
some simple things like whether to use mipmaps or not, what kind of sampling filter
to use, repeat modes etc.

<p align="center">
<img src="img/texturesettings.gif"/>
</p>

[ECS]: https://en.wikipedia.org/wiki/Entity%E2%80%93component%E2%80%93system
