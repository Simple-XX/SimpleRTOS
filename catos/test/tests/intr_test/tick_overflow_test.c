
#include "catos.h"

#include "../tests_config.h"
#if (CATOS_CAT_SHELL_ENABLE == 1 && TESTS_TICK_OVERFLOW == 1)
#include "cat_shell.h"
#include "cat_stdio.h"
void *do_test_ipc(void *arg)
{
    (void)arg;

    ipc_test();

    return CAT_NULL;
}
CAT_DECLARE_CMD(test_ipc, test ipc, do_test_ipc);
#endif