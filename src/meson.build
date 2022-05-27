project(
    'Wayland++', 
    'c','cpp', 
    default_options : ['cpp_std=c++14'],
    version : '1.0.0')
 

if get_option('buildtype').startswith('debug')
    add_project_arguments('-DDEBUG', '-O0', '-g', language : 'cpp')
else
    add_project_arguments('-Ofast', '-g3',  language : 'cpp')
endif

includes = [

    # Local
    include_directories('.'),
    include_directories('./classes'),
    include_directories('./globals/Viewporter'),
    include_directories('./globals/Wayland'),
    include_directories('./globals/XdgShell'),

    # System
    include_directories('/usr/include'),
    include_directories('/usr/local/include'),

    # DRM
    include_directories('/usr/include/drm'),
]

libs_paths = [
    '-L/lib',
    '-L/lib64',
    '-L/usr/lib',
    '-L/usr/lib64',
    '-L/usr/local/lib',
    '-L/usr/local/lib64',

    # Multiarch support
    '-L/lib/x86_64-linux-gnu',
    '-L/usr/lib/x86_64-linux-gnu',
    '-L/usr/local/lib/x86_64-linux-gnu'
]

libwpp = library(
    'libwpp', 
    sources : [
        'classes/WBackendDRM.cpp', 
        'classes/WBackendX11.cpp',
        'classes/WClient.cpp',
        'classes/WCompositor.cpp',
        'classes/WCursor.cpp',
        'classes/WOpenGL.cpp',
        'classes/WOutput.cpp',
        'classes/WOutputManager.cpp',
        'classes/WOutputMode.cpp',
        'classes/WPoint.cpp',
        'classes/WPointF.cpp',
        'classes/WPositioner.cpp',
        'classes/WRegion.cpp',
        'classes/WSeat.cpp',
        'classes/WSurface.cpp',
        'classes/WTexture.cpp',
        'classes/WTime.cpp',
        'classes/WToplevelRole.cpp',
        'classes/WWayland.cpp',

        'globals/Viewporter/viewporter.c',
        'globals/Viewporter/Viewporter.cpp',

        'globals/Wayland/Compositor.cpp',
        'globals/Wayland/DataDevice.cpp',
        'globals/Wayland/DataDeviceManager.cpp',
        'globals/Wayland/Keyboard.cpp',
        'globals/Wayland/Output.cpp',
        'globals/Wayland/Pointer.cpp',
        'globals/Wayland/Region.cpp',
        'globals/Wayland/Seat.cpp',
        'globals/Wayland/Subcompositor.cpp',
        'globals/Wayland/Subsurface.cpp',
        'globals/Wayland/Surface.cpp',

        'globals/XdgShell/XdgPopup.cpp',
        'globals/XdgShell/XdgPositioner.cpp',
        'globals/XdgShell/xdg-shell.c',
        'globals/XdgShell/XdgSurface.cpp',
        'globals/XdgShell/XdgToplevel.cpp',
        'globals/XdgShell/XdgWmBase.cpp'
    ],
    include_directories : includes,
    dependencies : [
        declare_dependency(link_args : libs_paths + '-lwayland-server'),
        declare_dependency(link_args : libs_paths + '-lEGL'),
        declare_dependency(link_args : libs_paths + '-lGL'),
        declare_dependency(link_args : libs_paths + '-ldrm'),
        declare_dependency(link_args : libs_paths + '-lGLESv2'),
        declare_dependency(link_args : libs_paths + '-lgbm'),
        declare_dependency(link_args : libs_paths + '-linput'),
        declare_dependency(link_args : libs_paths + '-ludev'),
        declare_dependency(link_args : libs_paths + '-lpthread'),
        declare_dependency(link_args : libs_paths + '-lX11'),
        declare_dependency(link_args : libs_paths + '-lXfixes'),
        declare_dependency(link_args : libs_paths + '-lXcursor'),
        declare_dependency(link_args : libs_paths + '-lxkbcommon'),
        declare_dependency(link_args : libs_paths + '-lSOIL'),
    ])

libwpp_dep = declare_dependency(
    dependencies: [], 
    include_directories : includes,
    link_with : libwpp)


subdir('examples')
