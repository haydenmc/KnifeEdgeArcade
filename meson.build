project('KnifeEdgeArcade', 'cpp',
  version : '0.1',
  default_options: [])

# C++20
cpp = meson.get_compiler('cpp')
if cpp.get_id() == 'msvc'
  add_project_arguments('/std:c++latest', language: 'cpp')
else
  add_project_arguments('-std=c++2a', language: 'cpp')
endif

projectDir  = meson.current_source_dir()

# Find mupen64plus-core
m64core_default_path = join_paths(projectDir, 'external/x64/New_Dynarec_Debug')
m64core_path = get_variable('MUPEN64PLUS_CORE_PATH', m64core_default_path)
cpp_compiler = meson.get_compiler('cpp')
m64core_lib = cpp_compiler.find_library('mupen64plus', dirs: m64core_path, required: true)

exe = executable('KnifeEdgeArcade',
  dependencies: [
    m64core_lib
  ],
  include_directories: [
    'external/mupen64plus-core/src/api'
  ],
  sources: [
    'src/Entrypoint.cpp',
    'src/m64p/Mupen64PlusCore.cpp',
  ])