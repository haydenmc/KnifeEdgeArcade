project('knife-edge-arcade', 'cpp',
  version : '0.1',
  default_options: [])

# C++20
cpp = meson.get_compiler('cpp')
if cpp.get_id() == 'msvc'
  add_project_arguments('/std:c++latest', language: 'cpp')
else
  add_project_arguments('-std=c++2a', language: 'cpp')
endif

# Subprojects
fmt_wrap = subproject('fmt',
  default_options: [
    'default_library=static'
  ])
spdlog_wrap = subproject('spdlog',
  default_options: [
    'tests=false',
    'default_library=static',
    'compile_library=true',
    'external_fmt=enabled',
  ])
tomlplusplus_wrap = subproject('tomlplusplus', default_options: ['default_library=static'])

# Find mupen64plus-core
projectDir  = meson.current_source_dir()
m64core_default_path = join_paths(projectDir, 'external/x64/New_Dynarec_Debug')
m64core_path = get_variable('MUPEN64PLUS_CORE_PATH', m64core_default_path)
cpp_compiler = meson.get_compiler('cpp')
m64core_lib = cpp_compiler.find_library('mupen64plus', dirs: m64core_path, required: true)

exe = executable('knife-edge-arcade',
  dependencies: [
    dependency('tomlplusplus'),
    spdlog_wrap.get_variable('spdlog_dep'),
    m64core_lib
  ],
  include_directories: [
    'src/knife-edge-arcade',
    'external/mupen64plus-core/src/api',
  ],
  cpp_pch: 'src/knife-edge-arcade/pch.h',
  sources: [
    'src/knife-edge-arcade/Configuration.cpp',
    'src/knife-edge-arcade/Entrypoint.cpp',
    'src/knife-edge-arcade/KnifeEdge.cpp',
    'src/knife-edge-arcade/m64p/Core.cpp',
    'src/knife-edge-arcade/m64p/Plugin.cpp',
  ])