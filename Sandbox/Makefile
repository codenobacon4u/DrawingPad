# Alternative GNU Make project makefile autogenerated by Premake

ifndef config
  config=debug
endif

ifndef verbose
  SILENT = @
endif

.PHONY: clean prebuild

SHELLTYPE := posix
ifeq (.exe,$(findstring .exe,$(ComSpec)))
	SHELLTYPE := msdos
endif

# Configurations
# #############################################

RESCOMP = windres
INCLUDES += -I../DrawingPad/src -I../DrawingPad/vendor -I../DrawingPad/vendor/glm -I../DrawingPad/vendor/glfw/include -I../DrawingPad/vendor/glad/include
FORCE_INCLUDE +=
ALL_CPPFLAGS += $(CPPFLAGS) -MMD -MP $(DEFINES) $(INCLUDES)
ALL_RESFLAGS += $(RESFLAGS) $(DEFINES) $(INCLUDES)
LINKCMD = $(CXX) -o "$@" $(OBJECTS) $(RESOURCES) $(ALL_LDFLAGS) $(LIBS)
define PREBUILDCMDS
endef
define PRELINKCMDS
endef
define POSTBUILDCMDS
endef

ifeq ($(config),debug)
TARGETDIR = ../bin/Debug-linux-x86_64/Sandbox
TARGET = $(TARGETDIR)/Sandbox
OBJDIR = ../bin-int/Debug-linux-x86_64/Sandbox
DEFINES += -DPW_DEBUG
ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -m64 -fPIC -g
ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -m64 -fPIC -g -std=c++17
LIBS += ../bin/Debug-linux-x86_64/DrawingPad/libDrawingPad.a ../DrawingPad/vendor/glfw/bin/Debug-linux-x86_64/GLFW/libGLFW.a ../DrawingPad/vendor/glad/bin/Debug-linux-x86_64/GLAD/libGLAD.a -lXrandr -lXi -lGL -ldl -lpthread -lX11
LDDEPS += ../bin/Debug-linux-x86_64/DrawingPad/libDrawingPad.a ../DrawingPad/vendor/glfw/bin/Debug-linux-x86_64/GLFW/libGLFW.a ../DrawingPad/vendor/glad/bin/Debug-linux-x86_64/GLAD/libGLAD.a
ALL_LDFLAGS += $(LDFLAGS) -L/usr/lib64 -m64

else ifeq ($(config),release)
TARGETDIR = ../bin/Release-linux-x86_64/Sandbox
TARGET = $(TARGETDIR)/Sandbox
OBJDIR = ../bin-int/Release-linux-x86_64/Sandbox
DEFINES += -DPW_RELEASE
ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -m64 -O2 -fPIC
ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -m64 -O2 -fPIC -std=c++17
LIBS += ../bin/Release-linux-x86_64/DrawingPad/libDrawingPad.a ../DrawingPad/vendor/glfw/bin/Release-linux-x86_64/GLFW/libGLFW.a ../DrawingPad/vendor/glad/bin/Release-linux-x86_64/GLAD/libGLAD.a -lXrandr -lXi -lGL -ldl -lpthread -lX11
LDDEPS += ../bin/Release-linux-x86_64/DrawingPad/libDrawingPad.a ../DrawingPad/vendor/glfw/bin/Release-linux-x86_64/GLFW/libGLFW.a ../DrawingPad/vendor/glad/bin/Release-linux-x86_64/GLAD/libGLAD.a
ALL_LDFLAGS += $(LDFLAGS) -L/usr/lib64 -m64 -s

else ifeq ($(config),dist)
TARGETDIR = ../bin/Dist-linux-x86_64/Sandbox
TARGET = $(TARGETDIR)/Sandbox
OBJDIR = ../bin-int/Dist-linux-x86_64/Sandbox
DEFINES += -DPW_DIST
ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -m64 -O2 -fPIC
ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -m64 -O2 -fPIC -std=c++17
LIBS += ../bin/Dist-linux-x86_64/DrawingPad/libDrawingPad.a ../DrawingPad/vendor/glfw/bin/Dist-linux-x86_64/GLFW/libGLFW.a ../DrawingPad/vendor/glad/bin/Dist-linux-x86_64/GLAD/libGLAD.a -lXrandr -lXi -lGL -ldl -lpthread -lX11
LDDEPS += ../bin/Dist-linux-x86_64/DrawingPad/libDrawingPad.a ../DrawingPad/vendor/glfw/bin/Dist-linux-x86_64/GLFW/libGLFW.a ../DrawingPad/vendor/glad/bin/Dist-linux-x86_64/GLAD/libGLAD.a
ALL_LDFLAGS += $(LDFLAGS) -L/usr/lib64 -m64 -s

endif

# Per File Configurations
# #############################################


# File sets
# #############################################

GENERATED :=
OBJECTS :=

GENERATED += $(OBJDIR)/main.o
OBJECTS += $(OBJDIR)/main.o

# Rules
# #############################################

all: $(TARGET)
	@:

$(TARGET): $(GENERATED) $(OBJECTS) $(LDDEPS) | $(TARGETDIR)
	$(PRELINKCMDS)
	@echo Linking Sandbox
	$(SILENT) $(LINKCMD)
	$(POSTBUILDCMDS)

$(TARGETDIR):
	@echo Creating $(TARGETDIR)
ifeq (posix,$(SHELLTYPE))
	$(SILENT) mkdir -p $(TARGETDIR)
else
	$(SILENT) mkdir $(subst /,\\,$(TARGETDIR))
endif

$(OBJDIR):
	@echo Creating $(OBJDIR)
ifeq (posix,$(SHELLTYPE))
	$(SILENT) mkdir -p $(OBJDIR)
else
	$(SILENT) mkdir $(subst /,\\,$(OBJDIR))
endif

clean:
	@echo Cleaning Sandbox
ifeq (posix,$(SHELLTYPE))
	$(SILENT) rm -f  $(TARGET)
	$(SILENT) rm -rf $(GENERATED)
	$(SILENT) rm -rf $(OBJDIR)
else
	$(SILENT) if exist $(subst /,\\,$(TARGET)) del $(subst /,\\,$(TARGET))
	$(SILENT) if exist $(subst /,\\,$(GENERATED)) rmdir /s /q $(subst /,\\,$(GENERATED))
	$(SILENT) if exist $(subst /,\\,$(OBJDIR)) rmdir /s /q $(subst /,\\,$(OBJDIR))
endif

prebuild: | $(OBJDIR)
	$(PREBUILDCMDS)

ifneq (,$(PCH))
$(OBJECTS): $(GCH) | $(PCH_PLACEHOLDER)
$(GCH): $(PCH) | prebuild
	@echo $(notdir $<)
	$(SILENT) $(CXX) -x c++-header $(ALL_CXXFLAGS) -o "$@" -MF "$(@:%.gch=%.d)" -c "$<"
$(PCH_PLACEHOLDER): $(GCH) | $(OBJDIR)
ifeq (posix,$(SHELLTYPE))
	$(SILENT) touch "$@"
else
	$(SILENT) echo $null >> "$@"
endif
else
$(OBJECTS): | prebuild
endif


# File Rules
# #############################################

$(OBJDIR)/main.o: src/main.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"

-include $(OBJECTS:%.o=%.d)
ifneq (,$(PCH))
  -include $(PCH_PLACEHOLDER).d
endif