project(
    'Wayland++', 
    'c','cpp', 
    default_options : ['cpp_std=c++14'],
    version : '1.0.0')
 
add_project_arguments('-Ofast', '-g3',  language : 'cpp')

wpplib = library(
    'wpplib', 
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
    include_directories : [
        include_directories('.'),
        include_directories('./classes'),
        include_directories('./globals/Viewporter'),
        include_directories('./globals/Wayland'),
        include_directories('./globals/XdgShell'),
        include_directories('/usr/include/drm')
    ],
    dependencies : [
        declare_dependency(link_args : ['-L/usr/local/lib', '-lwayland-server']),
        declare_dependency(link_args : ['-L/usr/local/lib', '-lEGL']),
        declare_dependency(link_args : ['-L/usr/local/lib', '-lGL']),
        declare_dependency(link_args : ['-L/usr/local/lib', '-ldrm']),
        declare_dependency(link_args : ['-L/usr/local/lib', '-lGLESv2']),
        declare_dependency(link_args : ['-L/usr/local/lib', '-lgbm']),
        declare_dependency(link_args : ['-L/usr/local/lib', '-linput']),
        declare_dependency(link_args : ['-L/usr/local/lib', '-ludev']),
        declare_dependency(link_args : ['-L/usr/local/lib', '-lpthread']),
        declare_dependency(link_args : ['-L/usr/local/lib', '-lX11']),
        declare_dependency(link_args : ['-L/usr/local/lib', '-lXfixes']),
        declare_dependency(link_args : ['-L/usr/local/lib', '-lXcursor']),
        declare_dependency(link_args : ['-L/usr/local/lib', '-lxkbcommon']),
        declare_dependency(link_args : ['-L/usr/local/lib', '-lSOIL']),
    ])

wpplib_dep = declare_dependency(
    dependencies: [], 
    include_directories : [
        include_directories('.'),
        include_directories('./classes'),
        include_directories('./globals/Viewporter'),
        include_directories('./globals/Wayland'),
        include_directories('./globals/XdgShell'),
    ],
    link_with : wpplib)


subdir('examples')
