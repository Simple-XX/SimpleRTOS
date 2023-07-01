/**
 * @file cat_shell.h
 * @brief 
 * @author amoigus (648137125@qq.com)
 * @version 0.1
 * @date 2021-06-05
 * 
 * @copyright Copyright (c) 2021
 * 
 * @par 修改日志：
 * Date              Version Author      Description
 * 2021-06-05 1.0    amoigus             内容
 */

#ifndef SHELL_H
#define SHELL_H

#include "catos_config.h"
#include "catos_types.h"

#include "cat_shell_port.h"

/*************** configs ****************/
#define CAT_USE_SECTION           (1)   /**< 使用段*/
#define CAT_MAX_CMD_NUM           (20)  /**< 最大命令数（命令结构体数）*/

#define CAT_MAX_CMD_LEN           (20)  /**< 单条命令最大长度*/

#define CAT_MAX_CMD_ARGS          (5)   /**< 最大参数数量*/
#define CAT_MAX_ARG_LEN           (20)  /**< 单条参数最大长度*/

                                        /**< 每次可输入的最大字符数(126)*/
#define CAT_MAX_SHELL_INPUT       (CAT_MAX_CMD_LEN + CAT_MAX_CMD_ARGS*(CAT_MAX_ARG_LEN + 1) + 1)

#define CAT_MAX_HISTORY           (2)  /**< 最大历史记录数*/
#define CAT_MAX_HIS_LEN           (CAT_MAX_SHELL_INPUT) /**< 单条历史记录最大长度*/
#define CAT_MAX_HIS_MEM           (CAT_MAX_HISTORY * CAT_MAX_HIS_LEN)

#define CAT_BUF_SIZE              (CAT_MAX_SHELL_INPUT)

#define CAT_SHELL_MEM_SIZE        ( \
    CAT_BUF_SIZE + \
    CAT_MAX_HIS_MEM \
)

/*************** typedefs ****************/
typedef cat_int16_t (*shell_io_callback_t)(cat_uint8_t *, cat_uint16_t );
typedef struct _cat_shell_cmd_t cat_shell_cmd_t;
typedef struct _cat_shell_instance_t cat_shell_instance_t;
typedef struct _cat_shell_config_t cat_shell_config_t;

typedef enum
{
    CAT_CMD_TYPE_CMD = 0,
    CAT_CMD_TYPE_KEY,
} cmd_type_t;

struct _cat_shell_cmd_t
{
    cmd_type_t type;
    union 
    {
        struct
        {
            const cat_uint8_t *name;
            const cat_uint8_t *desc;
            void *(*do_cmd)(void *cmd_arg);
            void *(*reserved)(void);
        } cmd;
        struct
        {
            const cat_uint8_t *name;
            cat_uint32_t value;
            void (*action)(cat_shell_instance_t *);
        } key;
        
    } content;
};

struct _cat_shell_instance_t
{
    struct
    {
        cat_uint8_t *buf;                         /**< 缓冲区(4*/
        cat_uint8_t *args[CAT_MAX_CMD_ARGS];         /**< 根据输入解析的参数(4*/
        cat_uint16_t length;                      /**< 当前输入数据的长度(2*/
        cat_uint16_t arg_num;                     /**< 参数的个数(2*/
        cat_uint16_t buf_size;                    /**< 缓冲区总大小(2*/
        cat_uint32_t current_combine_key_val;     /**< 当前累计的键值*/
        cat_uint8_t  current_combine_key_offset;  /**< 当前组合键偏移*/
    } buffer;
    struct
    {
        /* 历史记录：用上下键循环显示 */
        cat_uint8_t *historys[CAT_MAX_HISTORY];   /**< 历史记录*/
        cat_uint16_t his_num;                        /**< 历史记录条数*/
        cat_uint16_t current;                        /**< 当前显示的是第几条*/
    } history;
    struct
    {
        cat_shell_cmd_t *cmd_base;               /**< 命令列表起始地址*/
        cat_uint16_t cmd_num;                        /**< 命令数量*/
    } cmd_list;
    struct
    {
        cat_uint8_t is_inited;
    } status;
    // shell_io_callback_t read;           /**< 读，参数：0：将要存放的起始地址， 1：长度*/
    // shell_io_callback_t write;          /**< 写，参数：0：要写的数据的起始地址， 1：长度*/
};

struct _cat_shell_config_t
{
    cat_uint8_t *buffer;
    cat_uint16_t buf_size;
    //cat_uint8_t **args;
#if (CATOS_SHELL_USE_HISTORY == 1)
    cat_uint8_t **historys;
#endif
    // shell_io_callback_t read;
    // shell_io_callback_t write;
};


/*************** vars ****************/


/*************** macros ****************/

#if defined(__GNUC__)
    #include <stdio.h>
#endif

#define CAT_ONE_KEY_MASK                     (0xff000000)
#define CAT_TWO_KEY_MASK                     (0xffff0000)
#define CAT_THREE_KEY_MASK                   (0xffffff00)


#define CAT_HAS_AREADY_NONE(_value)       ((_value & CAT_ONE_KEY_MASK)   == 0x00000000)  /**< 没有累计键值*/
#define CAT_HAS_AREADY_ONE(_value)        ((_value & CAT_ONE_KEY_MASK)   == _value)  /**< 已经有一个键*/
#define CAT_HAS_AREADY_TWO(_value)        ((_value & CAT_TWO_KEY_MASK)   == _value)  /**< 已经有两个键*/
#define CAT_HAS_AREADY_THREE(_value)      ((_value & CAT_THREE_KEY_MASK) == _value)  /**< 已经有三个键*/

/******* key define ********/



#define CAT_KEY_BACKSPACE         (0x08000000)  /**< 退格*/
#define CAT_KEY_HT                (0x09000000)  /**< 制表tab*/

#define CAT_KEY_LF                (0x0a)  /**< 换行*/
#define CAT_KEY_CR                (0x0d)  /**< 归位*/
#define CAT_KEY_LFCR              (0x0a0d)
#define CAT_KEY_ENTER             (0x0d000000)

/*
 * note: 方向键键值是三个键值有序组合而成
 */
#if 0
    #define CAT_KEY_DIRECTION_FIRST     (0x1b)
    #define CAT_KEY_DIRECTION_SECOND    (0x5b)
    #define CAT_KEY_DIRECTION_UP        (0x41)
    #define CAT_KEY_DIRECTION_DOWN      (0x42)
    #define CAT_KEY_DIRECTION_LEFT      (0x44)
    #define CAT_KEY_DIRECTION_RIGHT     (0x43)
#else
    #define CAT_KEY_DIRECTION_UP        (0x1b5b4100)
    #define CAT_KEY_DIRECTION_DOWN      (0x1b5b4200)
    #define CAT_KEY_DIRECTION_LEFT      (0x1b5b4400)
    #define CAT_KEY_DIRECTION_RIGHT     (0x1b5b4300) 
#endif

/******* messages **************************/
#if 0
#define GREET_MSG_SMALL \
"_____________________________________\r\n" \
"    ___      _   ___ _        _ _   \r\n" \
"   / __|__ _| |_/ __| |_  ___| | |  \r\n" \
"  | (__/ _` |  _\\__ \\ ' \\/ -_) | |\r\n" \
"   \\___\\__,_|\\__|___/_||_\\___|_|_| \r\n" \
"_____________________________________\r\n\r\n" \
"CatShell v1.0.0 created by amoigus \r\n" \
"type \"help\" to get more info\r\n\r\n"
#else
#define GREET_MSG_SMALL \
"_____________________________________\r\n \
    ___      _   ___ _        _ _   \r\n \
   / __|__ _| |_/ __| |_  ___| | |  \r\n \
  | (__/ _` |  _\\__ \\ ' \\/ -_) | |\r\n \
   \\___\\__,_|\\__|___/_||_\\___|_|_| \r\n \
_____________________________________\r\n\r\n \
CatShell v1.0.1 created by mio \r\n \
type \"help\" to get more info\r\n\r\n"
#endif

/******* ANSI control symbol define ********/
#define CAT_SHELL_CLR_LINE          "\r\033[2k"
#define CAT_SHELL_CLR_SCREEN        "\033[2J\033[0;0H"

#if (CAT_USE_SECTION == 1) /*< 如果使用特定段来存放命令 */
#if defined(__CC_ARM) || (defined ( __ARMCC_VERSION ) && ( __ARMCC_VERSION >= 6010050 ))
    #define IMPORT_SECTION(x) \
        extern cat_uint32_t x##$$Base; \
        extern cat_uint32_t x##$$Limit;

    #define SECTION_START(x)  (&x##$$Base)
    #define SECTION_END(x)    (&x##$$Limit)
    #define SECTION_SIZE(x)   ((cat_uint32_t)&x##$$Limit - (cat_uint32_t)&x##$$Base)

    #define SECTION(x)  __attribute__((used, section(x)))
#elif defined(__GNUC__)
    /* 将在链接脚本中指定的段引入到源文件中，表现为两个变量：段起始地址和段结束地址 */
    #define IMPORT_SECTION(x) \
        extern unsigned short x##_start; \
        extern unsigned short x##_end;

    /* 获取段信息 */
    #define SECTION_START(x)  (&x##_start)
    #define SECTION_END(x)    (&x##_end)
    #define SECTION_SIZE(x)   ((unsigned short)&x##$$Limit - (unsigned short)&x##$$Base)

    /* 声明将被修饰的数据放入指定段中 */
    #define SECTION(x)  __attribute__((section(x)))
#else
    #error "no match compiler"
#endif

/* 声明shell命令 */
#define CAT_DECLARE_CMD(_name, _desc, _do_cmd) \
    const cat_shell_cmd_t \
    cat_cmd_##_name SECTION("cat_shell_cmd") =  \
    { \
        .type = CAT_CMD_TYPE_CMD, \
        .content.cmd.name = (const cat_uint8_t *)#_name, \
        .content.cmd.desc = (const cat_uint8_t *)#_desc, \
        .content.cmd.do_cmd = _do_cmd \
    }

/* 声明shell中输入字符时处理的按键 */
#define CAT_DECLARE_KEY(_name, _value, _action) \
    const cat_shell_cmd_t \
    cat_cmd_##_name SECTION("cat_shell_cmd") =  \
    { \
        .type = CAT_CMD_TYPE_KEY, \
        .content.key.name = (const cat_uint8_t *)#_name, \
        .content.key.value = _value, \
        .content.key.action = _action \
    }

/* 遍历命令列表 */
#define CAT_FOREACH_CMD(_temp) \
    for(_temp =  (cat_shell_cmd_t*)SECTION_START(cat_shell_cmd); \
        _temp != (cat_shell_cmd_t*)SECTION_END(cat_shell_cmd); \
        _temp ++)

#else //CAT_USE_SECTION


#define CAT_DECLARE_CMD(_name, _desc, _do_cmd) \
    do{  \
        const cat_shell_cmd_t \
        cat_cmd_##_name =  \
        { \
            .name = #_name, \
            .desc = #_desc, \
            .do_cmd = _do_cmd \
        } \
        cat_cmds[cmd_num] = &cat_cmd_##_name \
        cmd_num++;  \
    }while(0)


#define CAT_FOREACH_CMD(_temp, _i) \
    for(_temp=cat_cmds, _i=0; \
        _i<cmd_num&&_i<CAT_MAX_CMD_NUM; \
        _temp++, _i++)


#endif //CAT_USE_SECTION

/*************** functions *****************/

cat_int32_t cat_shell_init(cat_shell_instance_t *inst, cat_shell_config_t *cfg);
void cat_shell_task_entry(void *arg);

/**
 * @brief 处理输入的字符
 * @param data 
 */
void cat_handle_input_char(cat_shell_instance_t *inst, cat_uint8_t data);

cat_shell_cmd_t *cat_seek_cmd(cat_shell_instance_t *inst);

void cat_parse_args(cat_shell_instance_t *inst);
void cat_execute_cmd(cat_shell_instance_t *inst);

void cat_history_save(cat_shell_instance_t *shell_inst);
void cat_history_up(cat_shell_instance_t *shell_inst);
void cat_history_down(cat_shell_instance_t *shell_inst);

#endif //SHELL_H
