<h1 align="center">Origo</h1>

<p align="center">
A Linux-first 3D engine written in C++ and GLSL.<br>
Focused on clarity, control, and correctness.
</p>

<p align="center">
  <img src="editor/workspace/resources/textures/origo_logo.png" alt="Origo Logo" width="400"/>
</p>

---

<h2 align="center">Overview</h2>

<p align="center">
Origo is a self-contained 3D engine built for Linux.  
This game engine is being developed for submission to the CV-Tag 2026 competition.
</p>

---
<h2 align="center">Requirements</h2>

<div align="center">
  <table style="margin:auto;">
    <thead>
      <tr>
        <th>Requirement</th>
        <th>Notes</th>
      </tr>
    </thead>
    <tbody>
      <tr><td>Linux</td><td>Developed and tested on Arch</td></tr>
      <tr><td>C++ GCC Compiler</td><td>C++23</td></tr>
      <tr><td>OpenGL</td><td>Proper driver support required</td></tr>
      <tr><td>CMake</td><td>Build system</td></tr>
      <tr><td>Optional</td><td>stb_image, glm, etc.</td></tr>
    </tbody>
  </table>
</div>

<hr/>

<h2 align="center">Project Layout</h2>

<div align="center">
  <table style="margin:auto;">
    <thead>
      <tr>
        <th>Path</th>
        <th>Description</th>
      </tr>
    </thead>
    <tbody>
      <tr><td><code>origo/</code></td><td>Engine source</td></tr>
      <tr><td><code>game/</code></td><td>Example application</td></tr>
      <tr><td><code>editor/</code></td><td>WIP editor</td></tr>
      <tr><td><code>resources/</code></td><td>Shaders, textures, models</td></tr>
      <tr><td><code>vendor/</code></td><td>Third-party deps</td></tr>
      <tr><td><code>CMakeLists.txt</code></td><td>Root build config</td></tr>
      <tr><td><code>pch.*</code></td><td>Precompiled headers</td></tr>
      <tr><td><code>run.sh</code></td><td>Run helper</td></tr>
      <tr><td><code>generate.sh</code></td><td>Build helper</td></tr>
      <tr><td><code>overview.sh</code></td><td>Repo overview helper</td></tr>
    </tbody>
  </table>
</div>

---

<h2 align="center">Building</h2>

<div align="">
  <pre><code>git clone https://github.com/salar/origo.git
cd origo
mkdir build
./generate.sh</code></pre>
</div>

<p align="center"><strong>Run the example (from repo root on Linux):</strong></p>

<div align="">
  <pre><code>./run.sh</code></pre>
</div>

---

<h2 align="center">Status</h2>

<p align="center">
Under active development. APIs and layout may change.
</p>

--- 

<h3 align="center">Credits</h3>

<p align="center">
This project includes third-party assets.
See CREDITS.md for full attributions.
</p>
