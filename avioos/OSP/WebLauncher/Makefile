#
# Makefile
#
# Makefile for Avioos Web Launcher Core
#

include $(POCO_BASE)/build/rules/global
include $(POCO_BASE)/OSP/BundleCreator/BundleCreator.make

objects = BundleActivator

target      = net.avioos.osp.weblauncher
target_libs = PocoOSP PocoUtil PocoJSON PocoXML PocoFoundation

postbuild = $(SET_LD_LIBRARY_PATH) $(BUNDLE_TOOL) -n$(OSNAME) -a$(OSARCH) -o../bundles WebLauncher.bndlspec

include $(POCO_BASE)/build/rules/dylib