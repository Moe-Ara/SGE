<div id="top"></div>

<!-- PROJECT LOGO -->
<br />
<div align="center">
  <a href="https://github.com/yourusername/SimpleGameEngine">
  </a>

  <h3 align="center">Simple Game Engine (SGE)</h3>

  <p align="center">
    A simple yet powerful game engine designed for learning and experimentation with modern OpenGL.
    <br />
  </p>
</div>

<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
      <ul>
        <li><a href="#built-with">Built With</a></li>
      </ul>
    </li>
    <li>
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#prerequisites">Prerequisites</a></li>
        <li><a href="#installation">Installation</a></li>
      </ul>
    </li>
    <li><a href="#roadmap">Roadmap</a></li>
    <li><a href="#contributing">Contributing</a></li>
    <li><a href="#contact">Contact</a></li>
  </ol>
</details>

<!-- ABOUT THE PROJECT -->
## About The Project

Simple Game Engine (SGE) is a project built for the purpose of learning and experimenting with modern OpenGL. This engine supports basic rendering, camera control, lighting, and player m_movement mechanics. It serves as an excellent starting point for anyone interested in game development and computer GRAPHICS.

Here's why:
* Learn the basics of 3D rendering using OpenGL
* Implement and understand camera controls
* Experiment with basic lighting techniques
* Develop a foundational understanding of game engine architecture

### Main Demo
[![Main Demo Screenshot][main-demo-screenshot]](#)
<br />
### Lighting Demo
[![Lighting Demo Screenshot][lighting-demo-screenshot]](https://github.com/Moe-Ara/SGE/tree/master/resources/images/Lighting.png)

<p align="right">(<a href="#top">back to top</a>)</p>

### Built With

This project is built using the following technologies and libraries:

* [OpenGL](https://www.opengl.org/) (3.3 core)
* [GLFW](https://www.glfw.org/)
* [GLAD](https://glad.dav1d.de/) (vendored, generated loader)
* [GLM](https://glm.g-truc.net/0.9.9/index.html)
* [EnTT](https://github.com/skypjack/entt) (vendored, ECS core)
* [Dear ImGui](https://github.com/ocornut/imgui) (in-app debug/editor overlay)

See [docs/ARCHITECTURE.md](ARCHITECTURE.md) for how these fit together, how to extend
the engine, and a gap list of what's missing for a production-scale game.

<p align="right">(<a href="#top">back to top</a>)</p>

<!-- GETTING STARTED -->
## Getting Started

To get a local copy up and running, follow these simple steps.

### Prerequisites

Make sure you have the following dependencies installed (Debian/Ubuntu package names
shown; use vcpkg or your OS's package manager equivalents elsewhere):
* GLFW (`libglfw3-dev`)
* GLM (`libglm-dev`)
* Dear ImGui (`libimgui-dev`)
* A C++20 compiler + CMake 3.16+

