/**
 * @file cat_shell_cmds.c
 * @author mio_wen (648137125@qq.com)
 * @brief 部分shell命令定义
 * @version 0.1
 * @date 2022-07-30
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "cat_shell.h"

#if (CATOS_ENABLE_CAT_SHELL == 1)

#include "cat_stdio.h"
#include "cat_error.h"

IMPORT_SECTION(cat_shell_cmd)

/*************** key declares *****************/

#if (CATOS_SHELL_USE_HISTORY == 1)
void up_action(cat_shell_instance_t *shell_inst)
{
    //CAT_SYS_PRINTF("[cat_shell_cmds] up is pressed !\r\n");
    cat_history_up(shell_inst);
    CAT_SYS_PRINTF("%s", shell_inst->buffer.buf);
}
CAT_DECLARE_KEY(up, CAT_KEY_DIRECTION_UP, up_action);

void down_action(cat_shell_instance_t *shell_inst)
{
    //CAT_SYS_PRINTF("[cat_shell_cmds] down is pressed !\r\n");
    cat_history_down(shell_inst);
    CAT_SYS_PRINTF("%s", shell_inst->buffer.buf);
}
CAT_DECLARE_KEY(down, CAT_KEY_DIRECTION_DOWN, down_action);

void left_action(cat_shell_instance_t *shell_inst)
{
    //CAT_SYS_PRINTF("[cat_shell_cmds] left is pressed !\r\n");
}
CAT_DECLARE_KEY(left, CAT_KEY_DIRECTION_LEFT, left_action);

void right_action(cat_shell_instance_t *shell_inst)
{
    //CAT_SYS_PRINTF("[cat_shell_cmds] right is pressed !\r\n");
}
CAT_DECLARE_KEY(right, CAT_KEY_DIRECTION_RIGHT, right_action);
#endif

void backspace_action(cat_shell_instance_t *shell_inst)
{
    //DEBUG_PRINT("[cat_shell_cmds] backspace is pressed !\r\n");
	if(shell_inst->buffer.length > 0)
    {
        CAT_SYS_PRINTF("\b \b");//清行
        shell_inst->buffer.buf[shell_inst->buffer.length--] = '\0';
    }
    
    //shell_inst->buffer.buf
}
CAT_DECLARE_KEY(backspace, CAT_KEY_BACKSPACE, backspace_action);

void enter_action(cat_shell_instance_t *shell_inst)
{
    CAT_SYS_PRINTF("\r\n");
    shell_inst->buffer.buf[shell_inst->buffer.length] = '\0';

    if(shell_inst->buffer.length != 0)
    {
        cat_execute_cmd(shell_inst);

#if (CATOS_SHELL_USE_HISTORY == 1)
        cat_history_save(shell_inst);
#endif

        shell_inst->buffer.buf[0] = '\0';
        shell_inst->buffer.length = 0;
    }
    
    CAT_SYS_PRINTF("cat>");
}
CAT_DECLARE_KEY(enter, CAT_KEY_ENTER, enter_action);

/*************** cmd declares *****************/
void *do_help(void *arg)
{
    (void)arg;
	
	cat_uint8_t i = 0;
    cat_shell_cmd_t *temp;

	CAT_FOREACH_CMD(temp)
	{
        /* 只打印命令 */
        if(temp->type == CAT_CMD_TYPE_CMD)
        {
            CAT_SYS_PRINTF("%2d %s\r\n", i, temp->content.cmd.name);
		    i++;
        }
	    
    }
		
	return NULL;
}
CAT_DECLARE_CMD(help, print help msg, do_help);

#if 0
void *do_test_cmd(void *arg)
{
    (void)arg;
    CAT_SYS_PRINTF("test cmd func is called\r\n");

    return NULL;
}
CAT_DECLARE_CMD(test_cmd, for test, do_test_cmd);

void *do_cmd2(void *arg)
{
    (void)arg;
    CAT_SYS_PRINTF("cmd2 is called\r\n");

    return NULL;
}
CAT_DECLARE_CMD(cmd2, for test, do_cmd2);


void *do_test_args(void *arg)
{
    CAT_ASSERT(arg);

    cat_uint8_t i = 0;
    cat_shell_instance_t *inst = (cat_shell_instance_t *)arg;
    
    if(inst->buffer.arg_num == 0)
    {
        CAT_SYS_PRINTF("[do_test_args] no arg\r\n");
    }
    else
    {
        for(i=0; i<inst->buffer.arg_num; i++)
        {
            CAT_SYS_PRINTF("arg[%d]: %s\r\n", i, inst->buffer.args[i]);
        }
    }
		
		return NULL;
}
CAT_DECLARE_CMD(test_args, test args, do_test_args);

void *do_test_atoi(void *arg)
{
    CAT_ASSERT(arg);

    cat_uint8_t i = 0;
    cat_int32_t result = 0;
    cat_shell_instance_t *inst = (cat_shell_instance_t *)arg;
    
    if(inst->buffer.arg_num != 1)
    {
        CAT_SYS_PRINTF("[do_test_atoi] usage: test_atoi [STRING]\r\n");
    }
    else
    {
        cat_atoi(&result, inst->buffer.args[0]);
        CAT_SYS_PRINTF("arg[%d]: %s->", i, inst->buffer.args[i]);
        CAT_SYS_PRINTF("%d\r\n", result);
    }
		
		return NULL;
}
CAT_DECLARE_CMD(test_atoi, test atoi, do_test_atoi);
#endif

void *do_clear(void *arg)
{
    (void)arg;

    CAT_SYS_PRINTF(CAT_SHELL_CLR_SCREEN);

    return NULL;
}
CAT_DECLARE_CMD(clear, clear screen, do_clear);

#if (CATOS_ENABLE_CPUUSAGE_STAT == 1)
void *do_cpu_usage(void *arg)
{
    (void)arg;
    cat_uint32_t usage_integer, usage_decimal;

    cat_cpu_usage_get(&usage_integer, &usage_decimal);

    CAT_SYS_PRINTF("[cpu_usage] %d.%d\r\n", usage_integer, usage_decimal);

    return NULL;
}
CAT_DECLARE_CMD(cpu_usage, get cpu usage, do_cpu_usage);
#endif //#if (CATOS_ENABLE_CPUUSAGE_STAT == 1)

#endif /* #if (CATOS_ENABLE_CAT_SHELL == 1) */
