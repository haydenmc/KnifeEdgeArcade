project('KnifeEdgeArcade', 'cpp',
  version : '0.1')

projectDir  = meson.current_source_dir()
m64core_path = join_paths(projectDir, 'external/x64/New_Dynarec_Debug')
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
    'src/Entrypoint.cpp'
  ])