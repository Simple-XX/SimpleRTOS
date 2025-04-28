/**
 * @file meoo.c
 * @author 文佳源 (648137125@qq.com)
 * @brief 猫咪
 * @version 0.1
 * @date 2025-03-23
 * 
 * Copyright (c) 2025
 * 
 * @par 修订历史
 * <table>
 * <tr><th>版本 <th>作者 <th>日期 <th>修改内容
 * <tr><td>v1.0 <td>文佳源 <td>2025-03-23 <td>内容
 * </table>
 */
#include "catos.h"

/**
 * @brief 初始化meoo组件（新建组件示例）
 * 
 */
void meoo_init(void)
{
    CLOG_INFO("[meoo] component meoo init");
}

void *do_meoo(void *arg)
{
    (void)arg;
    

#define MEOO_MSG \
"\
       _                         \r\n\
       \\`*-.                     \r\n\
        )  _`-.        MEOO~     \r\n\
       .  : `. .     /            \r\n\
       : _   '  \\                \r\n\
       ; *` _.   `*-._           \r\n\
       `-.-'          `-.        \r\n\
         ;       `       `.      \r\n\
         :.       .        \\     \r\n\
         . \\  .   :   .-'   .    \r\n\
         '  `+.;  ;  '      :    \r\n\
         :  '  |    ;       ;-.  \r\n\
         ; '   : :`-:     _.`* ; \r\n\
      .*' /  .*' ; .*`- +'  `*'  \r\n\
      `*-*   `*-*  `*-*'         \r\n\
"

    cat_printf(MEOO_MSG);

    return CAT_NULL;
}
CAT_DECLARE_CMD(meoo, print a cute cat, do_meoo);