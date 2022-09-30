#当前目录
CUR_DIR = $(shell pwd)

#目标名(当前目录的目录名(不带完整前缀))
TARGET := $(lastword $(subst /, ,$(CUR_DIR)))
#输出目录
#OBJ_OUT_DIR = $(CUR_DIR)
OBJ_OUT_DIR = $(subst $(TOP_DIR),$(RELATIVE_PATH)/$(OBJ_DIR),$(CUR_DIR))
#给obj目标添加输出目录作为前缀
OBJ_TARGET = $(patsubst %,$(OBJ_OUT_DIR)/%,$(obj-y))
#要创建的输出目录，并排序
OBJ_Y_DIR = $(sort $(dir $(OBJ_TARGET)))

CINCLUDE_FILE_FLAG := $(addprefix -I$(RELATIVE_PATH)/,$(CINCLUDE_FILE))



#设置为伪目标，否则检测到文件或目录存在就不会执行命令
.PHONY: verify $(subdir-y) $(TARGET)

all: verify $(OBJ_Y_DIR) $(TARGET)

#在子makefile里声明编译前需要打印的信息，VERIFY_MSG = xxx
verify:
ifdef VERIFY_MSG
	@echo "verify msg: $(VERIFY_MSG)"
endif
#	@echo "CUR_DIR=$(CUR_DIR)"
#	@echo "TOP_DIR=$(TOP_DIR)"
#	@echo "RELATIVE_PATH=$(RELATIVE_PATH)"
#	@echo "OBJ_DIR=$(OBJ_DIR)"
#	@echo "OBJ_OUT_DIR=$(OBJ_OUT_DIR)"
#	@echo "TARGET=$(TARGET)"
#	@echo "subdir-y=$(subdir-y)"
#	@echo "OBJ_TARGET=$(OBJ_TARGET)"
#	@echo "OBJ_Y_DIR=$(OBJ_Y_DIR)"

#创建输出目录
$(OBJ_Y_DIR):
	@echo "********************************************make$(OBJ_Y_DIR)"
	@if [ ! -d "$@" ];then $(MK_DIR) $@ ;fi;


#让all目标间接依赖OBJ_TARGET，又由于OBJ_TARGET的.o没有给出，
#就会和$(OBJ_OUT_DIR)/%.o匹配
#############!!note1:subdir-y必须在OBJ_TARGET之后，因为subdir-y的编译
#需要增加RELATIVE_PATH的层数
$(TARGET): $(OBJ_TARGET) $(subdir-y)

#编译文件
$(OBJ_OUT_DIR)/%.o: $(CUR_DIR)/%.c
	$(CC) $(CINCLUDE_FILE_FLAG) $(CFLAGS) -o $@ -c $<

$(OBJ_OUT_DIR)/%.o: $(CUR_DIR)/%.s
	$(AS) $(ASMINCLUDE_FILE_FLAG) $(ASMFLAGS) -o $@ -c $<

$(OBJ_OUT_DIR)/%.o: $(CUR_DIR)/%.S
	$(AS) $(ASMINCLUDE_FILE_FLAG) $(ASMFLAGS) -o $@ -c $<

#更改相对路径，以便目录层级创建
#############!!note2:添加../必须放在这里，放前面的话生成的obj会再往上一个目录...
#就不在目标的output目录了
ifdef subdir-y
RELATIVE_PATH := $(addprefix ../,$(RELATIVE_PATH))
endif

#编译子目录
$(subdir-y):
	@$(MAKE) -C $@


