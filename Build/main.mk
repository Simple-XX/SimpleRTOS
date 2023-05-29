#定义基础命令
export RM_FILE=/bin/rm -f
export RM_DIR=/bin/rm -rf
export MK_DIR=/bin/mkdir -p
export CP=cp

#顶部目录，通常为src，main.mk被包含在src/Makefile中，pwd自然是src的绝对地址
TOP_DIR = $(shell pwd)
RELATIVE_PATH = . #相对路径，刚开始是在src中，可以直接访问./output
#编译规则目录
BUILD_DIR = $(TOP_DIR)/../Build
#-编译链配置目录,在项目配置文件中使用
#CONFIG_DIR = $(TOP_DIR)/../Build
#项目配置目录
PROJECT_DIR = $(SRC_DIR)/projects
#通用配置目录
COMMON_CONFIG_DIR = $(PROJECT_DIR)/common_configs
#读取输入的参数
export PROJECT = $(p)

#输出目录
OUT_DIR = output
OUT_PROJECT = $(OUT_DIR)/$(PROJECT)
OBJ_DIR = $(OUT_PROJECT)/obj
LIB_DIR = $(OUT_PROJECT)/lib
BIN_DIR = $(OUT_PROJECT)/bin

#字体颜色
TEXT_COLOR_RED   		= \033[1;31m
TEXT_COLOR_GREEN 		= \033[1;32m
TEXT_COLOR_YELLOW 		= \033[1;33m
TEXT_COLOR_BLUE     	= \033[1;34m
TEXT_COLOR_PURPLE   	= \033[1;35m
TEXT_COLOR_DARKGREEN 	= \033[1;36m
TEXT_COLOR_WHITE	 	= \033[1;37m
TEXT_COLOR_END   		= \033[0m

#检验输入的project是否有相应的配置文件
ifeq ($(PROJECT_DIR)/$(PROJECT)/$(PROJECT).config,$(wildcard $(PROJECT_DIR)/$(PROJECT)/$(PROJECT).config))

#包含编译链配置
include $(PROJECT_DIR)/$(PROJECT)/$(PROJECT).config

#输出文件
OUT_MAP := $(BIN_DIR)/$(MAP_NAME)
OUT_ELF := $(BIN_DIR)/$(ELF_NAME)
OUT_BIN := $(BIN_DIR)/$(BIN_NAME)
OUT_HEX := $(BIN_DIR)/$(HEX_NAME)

#如果没有特别配置，就使用--list生成.map文件
LIST_MAP_FILE ?= --list
LIST_MAP_FILE_FLAG = $(LIST_MAP_FILE)=$(OUT_MAP)

#git信息
BUILD_TIME := $(shell date "+%Y-%m-%d %H:%M:%S")
COMMIT_TIME_YEAR := $(shell git log -n 1 | grep "Date" | awk '{print $$6}')
COMMIT_TIME_MON := $(shell git log -n 1 | grep "Date" | awk '{print $$3}')
COMMIT_TIME_DAY := $(shell git log -n 1 | grep "Date" | awk '{print $$4}')
COMMIT_TIME_HMS := $(shell git log -n 1 | grep "Date" | awk '{print $$5}')
COMMIT_TIME := $(COMMIT_TIME_YEAR)-$(COMMIT_TIME_MON)-$(COMMIT_TIME_DAY) $(COMMIT_TIME_HMS)
SOURCE_COMMIT_INFO := commit:$(COMMIT_TIME)/build:$(BUILD_TIME)
GIT_REVISION := $(shell git log -n 1 | grep "commit" | awk '{print $$2}')
GIT_AUTHOR := $(shell git log -n 1 | grep "Author" | awk '{print $$2}')
GIT_EMAIL_FULL  := $(shell git log -n 1 | grep "Author" | awk '{print $$3}')
GIT_EMAIL := $(subst <,,$(subst >,,$(GIT_EMAIL_FULL)))
#开始时间
start_time := $(shell date "+%Y-%m-%d %H:%M:%S")

#设定为伪目标
.PHONY: clean $(SRC_DIR)
#以Makefile_content编译src目录 不输出进出目录的信息
all: $(OUT_ELF)


$(OUT_ELF): $(BIN_DIR) $(SRC_DIR)
	@echo "$(TEXT_COLOR_WHITE)<----------------------build done$(TEXT_COLOR_END)"
	@echo "$(TEXT_COLOR_WHITE)start link---------------------->$(TEXT_COLOR_END)"
	@$(LD) $(CLINK_FLAGS) -o $@ $(LIST_MAP_FILE_FLAG) $(shell find $(OUT_DIR)/$(PROJECT) -name *.o)
	$(CP) -O binary -S $@ $(OUT_BIN)
	$(CP) -O ihex   -S $@ $(OUT_HEX)
	@echo "$(TEXT_COLOR_WHITE)<-----------------------link done$(TEXT_COLOR_END)"
	@$(SZ) $@
	@echo "source git info: commit at $(COMMIT_TIME) by $(GIT_AUTHOR)@$(GIT_EMAIL)"
	@echo "build $(TEXT_COLOR_GREEN)$(PROJECT)$(TEXT_COLOR_END), start time: $(start_time) end time: $(shell date "+%Y-%m-%d %H:%M:%S")"

$(BIN_DIR):
	@$(MK_DIR) $(OUT_DIR)/$(PROJECT)
	@$(MK_DIR) $(OUT_DIR)/$(PROJECT)/bin
	@$(MK_DIR) $(OUT_DIR)/$(PROJECT)/obj
	@$(MK_DIR) $(OUT_DIR)/$(PROJECT)/lib

$(SRC_DIR):
	@echo "$(TEXT_COLOR_WHITE)start build project: $(PROJECT)------->$(TEXT_COLOR_END)"
	@$(MAKE) --no-print-directory -C $@ -f $(SRC_DIR)/Makefile_content
#	@$(MAKE) -C $@ -f $(SRC_DIR)/Makefile_content

else #ifeq ($(PROJECT_DIR)/$(PROJECT)/$(PROJECT).config,$(wildcard $(PROJECT_DIR)/$(PROJECT)/$(PROJECT).config))

all:
	@echo "config file=$(PROJECT_DIR)/$(PROJECT)/$(PROJECT).config"
	@echo "use make p=[PROJECT] to make project"
	exit 1

endif #ifeq ($(CONFIG_DIR)/$(PROJECT).config,$(wildcard $(CONFIG_DIR)/$(PROJECT).config))

#清除生成文件和目录
clean:
	@$(RM_DIR) $(OUT_DIR)

#导出所有变量
.EXPORT_ALL_VARIABLES:

#不导出的变量
unexport d				#make时指定的宏定义
unexport OUT_DIR		#
unexport OUT_PROJECT	#
unexport BIN_DIR		#可执行文件目录，因为最终生成是在这里

