# save parrent dir for whole compile
SRC_BASE_DIR:=$(CURDIR)
export SRC_BASE_DIR

# write path to makefiles/
BUILD_CONFIG_BASE_DIR:=$(SRC_BASE_DIR)/build_config
BUILD_PROCESS_BASE_DIR:=$(SRC_BASE_DIR)/build_process
CODE_BASE_DIR:=$(BUILD_PROCESS_BASE_DIR)/code
MAKEFILES_BASE_DIR:=$(BUILD_PROCESS_BASE_DIR)/makefiles
MAKEFILES_COMPONENT_MODS_DIR:=$(MAKEFILES_BASE_DIR)/component_mods
MAKEFILES_COMMON_BASE_DIR:=$(MAKEFILES_BASE_DIR)/common
MAKEFILES_PROFILES_BASE_DIR:=$(MAKEFILES_COMMON_BASE_DIR)/profiles
MAKEFILES_TOOLCHAINS_BASE_DIR:=$(MAKEFILES_COMMON_BASE_DIR)/toolchains
SCRIPTS_BASE_DIR:=$(BUILD_PROCESS_BASE_DIR)/scripts
export \
       BUILD_CONFIG_BASE_DIR \
       BUILD_PROCESS_BASE_DIR \
       CODE_BASE_DIR \
       MAKEFILES_BASE_DIR \
	   MAKEFILES_COMMON_BASE_DIR \
       MAKEFILES_COMPONENT_MODS_DIR \
	   MAKEFILES_PROFILES_BASE_DIR \
	   MAKEFILES_TOOLCHAINS_BASE_DIR \
       SCRIPTS_BASE_DIR

# if gendir was not set by upper call of make, set it now
ifeq (,$(GENDIR))
GEN_BASE_DIR:=$(SRC_BASE_DIR)/gen
endif
export GEN_BASE_DIR

# sub dirs of gen/
GEN_INCLUDES_DIR=$(GEN_BASE_DIR)/$(PROFILE)_$(TC)_$(MEM_CHECK)/includes
GEN_LIBS_DIR=$(GEN_BASE_DIR)/$(PROFILE)_$(TC)_$(MEM_CHECK)/libs
GEN_COMPONENTS_DEPS_DIR=$(GEN_BASE_DIR)/$(PROFILE)_$(TC)_$(MEM_CHECK)/components_deps
export \
       GEN_INCLUDES_DIR \
       GEN_LIBS_DIR \
       GEN_COMPONENTS_DEPS_DIR

include $(MAKEFILES_COMMON_BASE_DIR)/dirs_features.mk

