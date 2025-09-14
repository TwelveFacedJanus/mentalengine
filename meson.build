project(
  'Mental Engine',
  'cpp',
  version: '0.1.0',
  default_options: [
    'cpp_std=c++17',
    'warning_level=3',
    'werror=true',
    'buildtype=debugoptimized',
  ],
)

# Dependencies
glfw_dep = dependency('glfw3', required: true)
glew_dep = dependency('glew', required: true)

# ImGui - using files from external directory
imgui_files = files(
  'external/imgui/imgui.cpp',
  'external/imgui/imgui_demo.cpp',
  'external/imgui/imgui_draw.cpp',
  'external/imgui/imgui_tables.cpp',
  'external/imgui/imgui_widgets.cpp',
  'external/imgui/backends/imgui_impl_glfw.cpp',
  'external/imgui/backends/imgui_impl_opengl3.cpp',
)

# Include directories for ImGui
imgui_includes = include_directories(
  'external/imgui',
  'external/imgui/backends'
)

# macOS framework dependencies
if host_machine.system() == 'darwin'
  opengl_dep = dependency('appleframeworks', modules: ['OpenGL'], required: true)
  cocoa_dep = dependency('appleframeworks', modules: ['Cocoa'], required: true)
  iokit_dep = dependency('appleframeworks', modules: ['IOKit'], required: true)
  corevideo_dep = dependency('appleframeworks', modules: ['CoreVideo'], required: true)
endif

# Source files (organized for clarity)
sources = files(
  'source/main.cpp',
  'source/Core/Math.cpp',
  'source/T1/Camera/Camera.cpp',
  'source/T1/Renderer/Renderer.cpp',
)

# Collect dependencies
dependencies = [
  glfw_dep,
  glew_dep,
]

# Add macOS frameworks if building on macOS
if host_machine.system() == 'darwin'
  dependencies += [opengl_dep, cocoa_dep, iokit_dep, corevideo_dep]
endif

# Executable
executable(
  'MentalEngine',
  sources + imgui_files,  # Combine main sources with ImGui files
  dependencies: dependencies,
  include_directories: imgui_includes,
  install: true,
  cpp_args: [
    '-DENGINE_NAME="@0@"'.format(meson.project_name()),
    '-DIMGUI_IMPL_OPENGL_LOADER_GLEW',  # Tell ImGui to use GLEW for OpenGL loader
  ],
)
