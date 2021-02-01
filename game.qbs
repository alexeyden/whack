import qbs 1.0

Project {
    name: "Whack"
    minimumQbsVersion: "1.6"
    
    StaticLibrary {
        name: "jsoncpp"
        
        Properties {
            condition: qbs.targetOS.contains('android')
            qbs.architectures: ["armv7a"]
        }
        
        Export {
            Depends { name: "cpp" }
            
            cpp.includePaths: [
                "deps/jsoncpp/include/"
            ]

            Properties {
                condition: qbs.targetOS.contains('android')
                cpp.staticLibraries: [
                    sourceDirectory + "/deps/android/jsoncpp/src/lib_json/libjsoncpp.a"
                ]
            }
            
            Properties {
                condition: !qbs.targetOS.contains('android')
                cpp.staticLibraries: [
                    sourceDirectory + "/deps/linux/jsoncpp/src/lib_json/libjsoncpp.a"
                ]
            }
        }
    }
    
    StaticLibrary {
        name: "msgpackc"
        
        Properties {
            condition: qbs.targetOS.contains('android')
            qbs.architectures: ["armv7a"]
        }
        
        Export {
            Depends { name: "cpp" }
            
            cpp.includePaths: [
                "deps/msgpackc/include/"
            ]
        }
    }

    StaticLibrary {
        name: "sdl2"
        
        Properties {
            condition: qbs.targetOS.contains('android')
            qbs.architectures: ["armv7a"]
            cpp.defines: ["__ANDROID__"]
        }
        
        Export {
            Depends { name: "cpp" }
            
            cpp.includePaths: [
                "deps/SDL2/include/"
            ]
            
            Properties {
                condition: qbs.targetOS.contains('android')
                
                cpp.staticLibraries: [
                    sourceDirectory + "/deps/SDL2/obj/local/armeabi-v7a/libSDL2.a"
                ]
                cpp.dynamicLibraries: [
                    "GLESv2", "GLESv1_CM"
                ]
            }
            
            Properties {
                condition: !qbs.targetOS.contains('android')
                
                cpp.staticLibraries: [
                    sourceDirectory + "/deps/linux/SDL2/libSDL2.a"
                ]
                cpp.dynamicLibraries: [
                    "EGL", "dl", "pthread", "GLESv2"
                ]
            }
        }
    }
    
    CppApplication {
        name: "test_runner"
        condition: !qbs.targetOS.contains('android')
        
        Depends { name: "cpp" }
        Depends { name: "graphics" }
        Depends { name: "util" }
        Depends { name: "backend" } 
        Depends { name: "sdl2" }
        
        cpp.cxxLanguageVersion: ["c++11"]
        cpp.includePaths: [ "src/" ]
        
        Group {
            prefix: "test/"
            files: [ "*_test.h" ]
            fileTags: ["cxx_test"]
        }
        
        Rule {
            multiplex: true
            inputs: ["cxx_test"]
            
            Artifact {
                filePath: "runner.cpp"
                fileTags: ["cpp"]
                cpp.cxxLanguageVersion: ["c++11"]
            }
            
            prepare: {
                var cmd = new Command("cxxtestgen");
                var inputFiles = inputs.cxx_test.map(function(x) { return x.filePath; });
                
                cmd.arguments = [ "--root", "--error-printer", "-o", output.filePath].concat(inputFiles);
                cmd.description = "Generating cxxtest runner";
                return cmd;
            }
        }
        
    }
    
    CppApplication {
        name: "whack"
        condition: !qbs.targetOS.contains('android')
        
        Depends { name: "cpp" }
        Depends { name: "game" }
        Depends { name: "backend" }
        Depends { name: "sdl2" }
        
        cpp.cxxLanguageVersion: ["c++11"]
        cpp.includePaths: [ "src/" ]
        
        files: [ "main.cpp" ]
    }
   
    QtGuiApplication {
        name: "level_editor"
        condition: !qbs.targetOS.contains('android')
      
        Depends { name: "cpp" }
        Depends { name: "game" }
        Depends { name: "backend" }
        Depends { name: "sdl2" }
        Depends { name: "msgpackc" }
				Depends { name: "Qt.widgets" }
        
        cpp.cxxLanguageVersion: [ "c++11" ]
        cpp.includePaths: [ "src/" ]
      
        Group {
            prefix: "editor/"
            files: [
                "levelviewer.cpp",
                "levelviewer.h",
                "lighttablemodel.cpp",
                "lighttablemodel.h",
                "renderer.cpp",
                "renderer.h",
                "textureview.cpp",
                "textureview.h",
                "main_window.cpp",
                "main_window.h",
                "main_window.ui",
                "new_level.ui",
                "shaders.qrc",
                "main.cpp",
            ]
        }
    }
    
    DynamicLibrary {
        name: "whack_android"
        condition: qbs.targetOS.contains('android')
        
        Depends { name: "cpp" }
        Depends { name: "game" }
        Depends { name: "backend" }
        Depends { name: "sdl2" }

        Android.ndk.appStl: "gnustl_static"
        qbs.architectures: ["armv7a"]
        
        cpp.dynamicLibraries: [
            "log", "android"
        ]
        files: [
            "deps/SDL2/src/main/android/SDL_android_main.c",
            "main.cpp"
        ]

        cpp.cxxLanguageVersion: ["c++11"]
        cpp.includePaths: [ "src/" ]
    }
    /*
    AndroidApk {
        condition: qbs.targetOS.contains('android')
        name: "Whack"
        packageName: "org.libsdl.app"
        
        Depends { productTypes: ["android.nativelibrary"] }

        resourcesDir: sourceDirectory + "/apk/res"
        sourcesDir: sourceDirectory + "/apk/src"
        manifestFile: sourceDirectory + "/apk/AndroidManifest.xml"
        assetsDir: sourceDirectory + "/data"
    }
    */
    StaticLibrary {
        name: "backend"
        
        Depends { name: "cpp" }
        Depends { name: "sdl2" }
        
        Properties {
            condition: qbs.targetOS.contains('android')
            qbs.architectures: ["armv7a"]
            Android.ndk.appStl: "gnustl_static"
        }

        cpp.includePaths: [ "src/" ]
        cpp.cxxLanguageVersion: [ "c++11" ]
        
        Group {
            prefix: "src/backend/"
            files: [
                "backend.cpp",
                "rwops_streambuf.cpp",
                "rwops_stream.cpp"
            ]
        }
    }
    
    StaticLibrary {
        name: "behaviour"
        
        Depends { name: "cpp" }
        Depends { name: "sdl2" }
        
        Properties {
            condition: qbs.targetOS.contains('android')
            qbs.architectures: ["armv7a"]
            Android.ndk.appStl: "gnustl_static"
        }
        
        cpp.includePaths: [ "src/" ]
        cpp.cxxLanguageVersion: [ "c++11" ]
        
        Group {
            prefix: "src/behaviour/"
            files: [
                "emptybhv.cpp",
                "hangaroundbhv.cpp",
                "meleeattackbhv.cpp",
                "robberbhv.cpp"
            ]
        }
    }
    
    StaticLibrary {
        name: "controller"
        
        Depends { name: "cpp" }
        Depends { name: "sdl2" }
        
        Properties {
            condition: qbs.targetOS.contains('android')
            qbs.architectures: ["armv7a"]
            Android.ndk.appStl: "gnustl_static"
        }
        
        cpp.includePaths: [ "src/" ]
        cpp.cxxLanguageVersion: [ "c++11" ]
        
        Group {
            prefix: "src/controller/"
            files: [
                "inputmanager.cpp"
            ]
        }
    }
    
    StaticLibrary {
        name: "game"
        
        Depends { name: "level" }
        Depends { name: "msgpackc" }
        Depends { name: "ui" } 
        Depends { name: "level" }
        Depends { name: "resources" }
        Depends { name: "graphics" }
        Depends { name: "controller" }
        Depends { name: "backend" }
        Depends { name: "sdl2" }
        
        Properties {
            condition: qbs.targetOS.contains('android')
            qbs.architectures: ["armv7a"]
            Android.ndk.appStl: "gnustl_static"
        }
        
        cpp.includePaths: [ "src/" ]
        cpp.cxxLanguageVersion: [ "c++11" ]
        
        Group {
            prefix: "src/game/"
            files: [
                "application.cpp",
                "gamescreen.cpp",
                "menuscreen.cpp",
                "settingsscreen.cpp"
            ]
        }
    }
    
    StaticLibrary {
        name: "graphics"
        
        Depends { name: "cpp" }
        Depends { name: "jsoncpp" }
        Depends { name: "backend" }
        Depends { name: "sdl2" }
        
        Properties {
            condition: qbs.targetOS.contains('android')
            qbs.architectures: ["armv7a"]
            Android.ndk.appStl: "gnustl_static"
        }
        
        cpp.cxxLanguageVersion: [ "c++11" ]
        cpp.includePaths: [ "src/" ]
        
        
        Group {
            prefix: "src/graphics/"
            files: [
                "font.cpp",
                "ninepatch.cpp",
                "renderer2d.cpp",
                "textureatlas.cpp",
                "textureatlasreader.cpp",
                "texture.cpp"
            ]
        }
    }
    
    StaticLibrary {
        name: "level"
        
        Depends { name: "cpp" }
        Depends { name: "msgpackc" }
        Depends { name: "util" }
        Depends { name: "graphics" }
        Depends { name: "behaviour" }
        Depends { name: "objects" }
        Depends { name: "sdl2" }
        
        Properties {
            condition: qbs.targetOS.contains('android')
            qbs.architectures: ["armv7a"]
            Android.ndk.appStl: "gnustl_static"
        }
        
        cpp.cxxLanguageVersion: [ "c++11" ]
        cpp.includePaths: [ "src/" ]
        
        Group {
            prefix: "src/level/"
            files: [
                "block.cpp",
                "colorskydome.cpp",
                "entityvisualinfoextractor.cpp",
                "level.cpp",
                "levelloader.cpp",
                "levelraycaster.cpp",
                "lightingcalculator.cpp",
                "nightskydome.cpp",
                "pathfinder.cpp",
                "pathfinderjob.cpp",
                "physicsmanager.cpp",
                "renderer.cpp",
                "staticskydome.cpp"
            ]
        }
    }
    
    StaticLibrary {
        name: "objects"
        
        Depends { name: "cpp" }
        Depends { name: "behaviour" }  
        Depends { name: "sdl2" }
        
        Properties {
            condition: qbs.targetOS.contains('android')
            qbs.architectures: ["armv7a"]
            Android.ndk.appStl: "gnustl_static"
        }
        
        cpp.cxxLanguageVersion: [ "c++11" ]
        cpp.includePaths: [ "src/" ]
        
        Group {
            prefix: "src/objects/"
            files: [
                "decoration.cpp",
                "enemy.cpp",
                "gunmanager.cpp",
                "player.cpp",
                "robber.cpp",
                "splash.cpp",
                "visualentity.cpp"
            ]
        }
    }
    
    StaticLibrary {
        name: "resources"
        
        Depends { name: "cpp" }
        Depends { name: "graphics" }
        Depends { name: "sdl2" }
        
        Properties {
            condition: qbs.targetOS.contains('android')
            qbs.architectures: ["armv7a"]
            Android.ndk.appStl: "gnustl_static"
        }
        
        cpp.includePaths: [ "src/" ]
        cpp.cxxLanguageVersion: ["c++11"]
        
        Group {
            prefix: "src/resources/"
            files: [
                "resources.cpp"
            ]
        }
    }
    
    StaticLibrary {
        name: "ui"
        
        Depends { name: "cpp" }
        Depends { name: "sdl2" }
        
        Properties {
            condition: qbs.targetOS.contains('android')
            qbs.architectures: ["armv7a"]
            Android.ndk.appStl: "gnustl_static"
        }
        
        cpp.includePaths: [ "src/" ]
        cpp.cxxLanguageVersion: ["c++11"]

        Group {
            prefix: "src/ui/"
            files: [
                "hud.cpp",
                "uibutton.cpp",
                "uicheckbox.cpp",
                "uiconsole.cpp",
                "uieditbox.cpp",
                "uiimage.cpp",
                "uidirpad.cpp",
                "uijoypad.cpp",
                "uiimagebutton.cpp",
                "uilabel.cpp",
                "uilayout.cpp",
                "uilinearlayout.cpp",
                "uimanager.cpp"
            ]
        }
    }
    
    StaticLibrary {
        name: "util"
        
        Depends { name: "cpp" }
        Depends { name: "sdl2" }
       
       /*
        Properties {
            condition: qbs.targetOS.contains('android')
            qbs.architectures: ["armv7a"]
            Android.ndk.appStl: "gnustl_static"
        }
        */
        
        cpp.includePaths: [ "src/" ]
        cpp.cxxLanguageVersion: ["c++11"]
        
        Group {
            prefix: "src/util/"
            files: [
                "jobrunner.cpp",
                "math.cpp",
                "raytracer.cpp",
                "util.cpp"
            ]
        }
    }
}
